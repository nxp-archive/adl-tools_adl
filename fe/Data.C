//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main data structure for storing ADL information.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <cassert>
#include <functional>

#include "opencxx/parser/Leaf.h"
#include "opencxx/parser/token-names.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/PtreeTypeUtil.h"
#include "opencxx/BindEnumName.h"
#include "opencxx/Environment.h"

#include "helpers/fe_exceptions.h"

#include "helpers/gc_vector.h"
#include "helpers/gc_hash_set.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/StringUtils.h"
#include "helpers/BitTwiddles.h"

#include "VarWalker.h"
#include "ResWalker.h"
#include "Helpers.h"
#include "Data.h"
#include "InstrTable.h"
#include "MsgLog.h"
#include "SyntaxWalkers.h"
#include "FieldSupport.h"
#include "Handlers.h"
using namespace std;
using namespace Opencxx;
using rnumber::RNumber;

static const unsigned DefaultRegWidth = 32;

#undef const_core
#define const_core const_cast<const Core&>(core)

namespace adl {
  
  const char *MemoryName = "Mem";
  const char *MmuName = "Mmu";
  const char *CurrentRegName = "ThisReg";
  const char *CurrentCacheName = "ThisCache";
  const char *AnnotationDataStart = "AdlDataStart";
  const char *EaMaskName = "EaMask";
  const char *RaMaskName = "RaMask";
  const char *NoDefault = "undefined";
  const char *ReservedEnum = "reserved";
  const char *UpdateReg = "updateReg";
  const char *VarOpcodeName = "VarInstrOpcode";
  static int  VarOpcodeNameLen = strlen(VarOpcodeName);
  const char *VarPrefixName = "VarPrefixOpcode";
  unsigned    ReservedEntry = 0xffffffff;
  const char *RegFieldGetterPfx = "getfield_";
  const char *RegFieldSetterPfx = "setfield_";

  const gc_string EMPTY_STRING = "";

  bool adl::BitPair::endianness;
  bool adl::Data::_strict = false;
  bool adl::Data::_warn_redefine = true;
  bool adl::Data::_orig_action = false;

  bool DataStrictMode()
  {
    return Data::strictMode();
  }

  bool DataWarnRedefineMode()
  {
    return Data::warnRedefineMode();
  }

  // Return a mask that contains common opcode bits for all instrucitons
  uint64_t get_mask(const gc_list<Instr*> &instrs, unsigned enc_width) {
    unsigned max_instr_width = 0;
    ForEach(instrs,i) {
      max_instr_width = max(max_instr_width,(*i)->width());
    }
    uint64_t ret = (uint64_t)-1;
    ForEach(instrs,it) {
      Instr &instr = *(*it);
      uint64_t tmp = 0;
      for (unsigned i = 0; i != instr.sizeOpcs(); ++i) {
        Instr::Field ifp = instr.getOpc(i);
        ForEach(ifp._field->bits(),b) {
          for (int j = b->lo(max_instr_width); j <= b->hi(max_instr_width); ++j) {
            uint64_t one = 1;
            one <<= (max_instr_width - j - 1);
            tmp |= one;
          } 
        }
      }
      ret &= tmp;
    }
     
    ret <<= (enc_width - max_instr_width);
    return ret;
  }

  // Prints out an ordered list of names from a data structure of pointers
  // to objects with a name() method.
  template <class C>
  void printNamesOrdered(ostream &o,const gc_string &pfx,const C &c)
  {
    gc_vector<gc_string> names;
    ForEach(c,i) {
      names.push_back((*i)->name());
    }
    sort(names.begin(),names.end());
    ForEach(names,i) {
      o << pfx << *i << "\n";
    }
  }

  char* gc_strdup(const gc_string &s) 
  {
    char* n = new (GC) char[strlen(s.c_str())+1];
    return strcpy(n,s.c_str());
  }

  StrHashSet initAliasEscapes()
  {
    StrHashSet s;
  
    s.insert(Cia);
    s.insert("count_leading_zeros");
    s.insert("count_trailing_zeros");
    s.insert("count_ones");

    return s;
  }

  StrHashSet initGlobalResources()
  {
    StrHashSet s;
    
    // Main global functions
    s.insert("run");  // run() for micro-instructions
    s.insert("run_commit");  // run_commit() for micro-instructions
    s.insert("disp"); // disp() for internal diagnosis, no safety checks for arguments, be removed before final release
    s.insert("getCoreId");
    s.insert("getChildId");
    s.insert("getInstrCount");
    s.insert("getTDCounter");
    s.insert("resetCore");
    s.insert("portRead");
    s.insert("zero");
    s.insert("signExtend");
    s.insert("zeroExtend");
    s.insert("signedMultiply");
    s.insert("signedDivide");
    s.insert("signedMod");
    s.insert("bits");
    s.insert("Carry");
    s.insert("concat");
    s.insert("raiseException");
    s.insert("setException");
    s.insert("getMicroOpOffset");
    s.insert("setMicroOpOffset");
    s.insert("setCurrentInstrTable");
    s.insert("getCurrentInstrTable");
    s.insert("instrReadTranslate");
    s.insert("dataReadTranslate");
    s.insert("dataWriteTranslate");
    s.insert("checkInstrPerms");
    s.insert("checkDataPerms");
    s.insert("lastRealAddr");
    s.insert("getCurrentInstr");
    s.insert("getPacketPosition");
    s.insert("setPacketPosition");
    s.insert("activeContext");
    s.insert("delayed_write");
    s.insert("direct_access");
    s.insert("logMmuTranslation");

    s.insert("halt");
    s.insert("set_active_count");
    s.insert("get_active_count");
    s.insert("is_active");

    // For messaging via log_annotation.
    s.insert("info");
    s.insert("warning");
    s.insert("error");

    // Denotes the start of data to be placed into a log_annotation_data call.
    s.insert(AnnotationDataStart);

    s.insert("hex");
    s.insert("dec");
    s.insert("setw");
    s.insert("setfill");
    s.insert("showbase");
    s.insert("noshowbase");
    s.insert("true");
    s.insert("false");

    s.insert(MemoryName);
    s.insert(MmuName);
    s.insert(CurrentRegName);

    s.insert("CacheLogRead");
    s.insert("CacheILogRead");
    s.insert("CacheRead");
    s.insert("CacheWrite");
    s.insert("CacheFlush");
    s.insert("CacheTouch");
    s.insert("CacheAlloc");
    s.insert("CacheInvalidate");
    s.insert("CacheLock");
    s.insert("CacheUnlock");
    s.insert("CacheIFetch");
    s.insert("MaxCacheAccess");

    s.insert("InstrAccess");
    s.insert("DataAccess");

    s.insert("syscall_enabled");
    s.insert("syscall_trigger");
    s.insert("syscall_add_arg");
    s.insert("syscall_return_code");
    s.insert("syscall_errno");

    s.insert("InstrTrans");
    s.insert("LoadTrans");
    s.insert("StoreTrans");

    s.insert("enable_rollback");
    s.insert("commit_rollback");
    s.insert("flush_rollback");
    
    s.insert("set_ignore_mode");
    s.insert("ignore_enabled");

    s.insert("jit_test_func");

    s.insert("instrHasAttr");
    s.insert("clear_mmu_cache");

    s.insert("replaceInstr");
    s.insert("createInstr");
    s.insert("combineInstr");       // Combine micro instructions into a macro one
    s.insert("patchInstr");         // Patch a macro instruction at the 32 LSBs with a micro one
    s.insert("savePrefix");
    s.insert("getPriorPacket");
    s.insert("hasLabel");
    s.insert("adjustCurLabel");

    // For perf. model communication.
    s.insert("getExtInstrFlag");
    s.insert("setExtInstrFlag");

    return s;
  }

  static StrHashSet GlobalResources = initGlobalResources();

  static StrHashSet ValidAliasEscapes = initAliasEscapes();

  bool validAliasEscape(const gc_string &name)
  {
    return ValidAliasEscapes.count(name);
  }

  // List all global resources/special names here.
  bool globalResource(const gc_string &name)
  {
    if (GlobalResources.count(name)) return true;

    // Bit of a hack:  Allow gcc-style builtins.
    if (name.compare(0,strlen("__builtin_"),"__builtin_") == 0) return true;

    return false;
  }


  Base::Base(Opencxx::Environment *env,Opencxx::Ptree *loc) : 
    _loc(loc), 
    _env(env) 
  {
  }

  Base::~Base() 
  {
  }

  void Base::dump() const
  {
    dump(cerr,"");
  }

  void Doc::dump_doc(ostream &o,const gc_string &pfx,bool header) const
  {
    if (!_doc.empty()) {
      if (header) {
        o << pfx << "Documention:\n\n"
          << _doc << "\n";
      } else {
        o << pfx << _doc << "\n";
      }
    }
  }

  void Named::dump_doc(ostream &o,const gc_string &pfx,bool header) const
  {
    if (has_doc_name()) {
      o << pfx << "Documentation Name:  " << _doc_name << "\n";
    }
    Doc::dump_doc(o,pfx,header);
  }

  Aspect::Aspect(Environment *env,Ptree *loc,const gc_string &name) :
    Base(env,loc),
    Named(name),
    _instr_pre_exec(false),
    _instr_post_exec(false),
    _exclusive(false),
    _priority(numeric_limits<unsigned>::max()),
    _action(0)
  {
  }

  void Aspect::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  " << _name;
    if (_exclusive) {
      o << " (exclusive)";
    }
    if (has_priority()) {
      o << " (priority " << _priority << ")";
    }
    o << '\n';
    if (!_labels.empty()) {
      o << pfx << "Labels:  ";
      printSequence(o,_labels,true);
    }
    if (!_attrs.empty()) {
      o << pfx << "Attributes:  " << _attrs << "\n";
    }
    if (!_def_checks.empty()) {
      o << pfx << "Defined Checks:  ";
      printSequence(o,_def_checks,true);
    }
    if (_instr_pre_exec) {
        o << pfx << "Trigger on instruction pre-exec.\n";
    }
    if (_instr_post_exec) {
        o << pfx << "Trigger on instruction post-exec.\n";
    }
    o << pfx << "Action:  " << _action << "\n";
    dump_doc(o,pfx,true);
  }

  Aspects::Aspects (Opencxx::Environment *env,Opencxx::Ptree *loc) : 
    Base(env,loc)
  {
    // Inject a pointer to this object into the environment so that it can be retrieved
    // during code translation.
    addParentObj(env,this);
  }

  void Aspects::add_aspects(const Aspects &src)
  {
    // Overwrite from source.
    ForEach(src._aspects,i) {
      _aspects[i->first] = i->second;
    }
  }

  bool Aspects::getAspects(AspectList &al,const gc_string &label) const
  {
    bool ex = false;

    ForEach(_aspects,i) {
      const Aspect &a = *i->second;

      if (label.empty() || a.labels().count(label)) {
        al.push_back(&a);
        if (a.exclusive()) {
          ex = true;
        }
      }
    }
    return ex;
  }  

  void Aspects::dump(ostream &o,const gc_string &pfx) const
  {
    if (!_aspects.empty()) {
      gc_string pfx1 = indent(pfx);
      o << pfx << "Aspects:\n";
      printHashOrdered(o,pfx1,_aspects);
    }
  }

  // Used for adding individual attributes from the parser.
  bool Attrs::add_attr(AttrItem aitem, bool remove,bool check)
  {
    auto a = _attrs.find(aitem.attr());
    auto &p = aitem._params;

    if(remove) {
      if (a != _attrs.end()) {
        _attrs.erase(a);
        return true;
      } 	
      if (!check) return false;
      p._remove = true;   
    }
    if (a != _attrs.end()) {
      switch (p._type) {
      case AttrParams::IntList:	
        a->second._int_list.insert(a->second._int_list.end(),p._int_list.begin(),p._int_list.end());
      case AttrParams::StrList:	
        a->second._str_list.insert(a->second._str_list.end(),p._str_list.begin(),p._str_list.end());
      case AttrParams::Int:
        a->second._int_list.clear();
        a->second._int_list.insert(a->second._int_list.end(),p._int_list.begin(),p._int_list.end());	
      case AttrParams::Str:	
        a->second._str_list.clear();
        a->second._str_list.insert(a->second._str_list.end(),p._str_list.begin(),p._str_list.end());  
      default:
        _attrs.insert(std::make_pair(aitem.attr(),p)); 
      }     
    } else {
      _attrs.insert(std::make_pair(aitem.attr(),p));
    }
    return true;
  }
 
  // Used for adding multiple attributes, generally due to inheritence, e.g. a
  // shorthand inheriting attributes, an implementation version of an
  // instruction, etc.  This behavior is also controlled by the object's
  // _inherit flag: If _inherit is false, then this behavior is skipped.
  //
  // attrs:           Attributes to add to this object.
  // use_new_values:  Controls who wins when merging.  If use_new_values is true, 
  //                  then we take from attrs, otherwise, we take from this object.
  void Attrs::add_attrs(const AttrList& attrs,bool use_new_values) 
  {   
    if (_inherit) {
      ForEach(attrs,i) {
        auto a = _attrs.find(i->first);
        // Already exists?
        if (a != _attrs.end()) {
          // Remove?  If either an existing or a new item says to remove, then we
          // remove.
          if ( (i->second._remove && use_new_values) || a->second._remove) {
            _attrs.erase(a);
          } else {
            // Same type?  If so, or if it doesn't have a type, then we allow an
            // overwrite.
            AttrParams &trg = a->second;
            const AttrParams &src = i->second;
            if ( (trg._type == AttrParams::NoParam || trg._type == src._type) && use_new_values) {
              trg = src;
            }
          }
          // remove must exist
        } else if (i->second._remove) {
          PError(loc(),"Unknown attribute.");	  
        } else {
          _attrs.insert(*i);
        }
      }
    }
  }

  void Attrs::finalize() 
  {
    ForEach(_attrs,i) {
      if (i->second._remove) {
        PError(loc(),"Trying to remove unknown attribute '" << Data::get_attr_name(i->first) << "'.");	 
      }
    }
  };
  
  const AttrParams *Attrs::has_attr(unsigned c) const
  {
    IfFind(x,_attrs,c) {
      return &x->second;
    } else {
      return 0;
    }
  } 

  const AttrParams *Attrs::has_attr(const AttrItem &ai) const
  {
    IfFind (x,_attrs,ai.attr()) {
      if (x->second == ai.params()) {
        return &x->second;
      }
    }
    return 0;
  } 
  
  void AttrParams::record_int_param(Opencxx::Ptree* p,Opencxx::Environment *env) {
    _int_list.push_back(get_uint(p,"parameter.",env));
  }
  
  void AttrParams::record_str_param(Opencxx::Ptree* p) {
    _str_list.push_back(get_str(p,"parameter."));
  };

  bool AttrParams::operator==(const AttrParams &x) const
  {
    return (_type == x._type && _int_list == x._int_list && _str_list == x._str_list);
  }
  
  ostream &operator<<(std::ostream &o,const AttrParams &ap) 
  {
    bool first = true;
    switch (ap._type) {
    case AttrParams::Int:
      o << "(" << ap._int_list.front() << ")";
      break;
    case AttrParams::Str:
      o << "('" << ap._str_list.front() << "') ";
      break; 
    case AttrParams::IntList:
      o << "(";	
      ForEach(ap._int_list,i) {
        list_sep(o,first);
        o << *i;
      }
      o <<  ")";
      break;	
    case AttrParams::StrList:
      o << "(";	
      ForEach(ap._str_list,i) {
        list_sep(o,first);
        o << "'" << *i << "'";
      }
      o << ")";
      break;	
    default :
      break;
    }
    return o;
  }

  ostream &operator<<(std::ostream &o,const AttrItem &ai)
  {
    o << Data::get_attr_name(ai.attr()) << ai.params();
    return o;
  }

  ostream &operator<<(std::ostream &o,const AttrItems &ai)
  {
    bool first = true;
    ForEach(ai,i) {
      list_sep(o,first);
      o << *i;
    }
    return o;
  }  
  
  ostream &operator<<(std::ostream &o,const AttrList &al)
  {
    bool first = true;
    ForEach(al,i) {
      pfx_sep(o," ",first);
      o << Data::get_attr_name(i->first) << i->second;
    }
    return o;
  }

  void Attrs::dump(ostream &o) const
  {
    if (!_attrs.empty()) {
      o << "Attributes:  " << _attrs;
    }
  }

  void Attrs::dump(ostream &o,const gc_string &pfx) const
  {
    if (!_attrs.empty()) {
      o << pfx;
      dump(o);
      o << "\n";
    }
    Aspects::dump(o,pfx);
  }

  //
  // The entire data model.
  //

  const char *Cia = "cia";
  const char *Nia = "nia";
  const char *Unarchitected = "unarchitected";
  const char *Indeterminate = "indeterminate";
  // When adding documentation attributes, see getDocAttributes()
  // in misc/docgen.C, which is used to suppress these attributes
  // in generated documentation.
  const char *DocHidden        = "doc_hidden"; // attr: hide all documentation for this object
  const char *DocNoCode        = "doc_no_code"; // attr: hide implementation block in documentation
  const char *DocNoExpandExprs = "doc_no_expand_exprs"; // attr:  do not expand expressions in documentation/database.
  const char *DefaultInstrTable = "other";
  const char *PrefixInstrTable  = "prefix";
  const char *DefaultTestSet = "default_test_set";

  Data::Data(Environment *env) : Base(env,0)
  {
    // Add in predefined classes.
    add_attr(Cia);
    add_attr(Nia);
    add_attr(DocHidden);
    add_attr(DocNoCode);
    add_attr(DocNoExpandExprs);
    add_attr(DefaultInstrTable);
    add_attr(PrefixInstrTable);
  }

  static Data::AttrHash &get_attr_hash()
  {
    static Data::AttrHash attrs;
    return attrs;
  }
 
  const Data::AttrHash &Data::get_attrs()
  {
    return get_attr_hash();
  }

  ostream &operator<<(ostream &os,AttrParams::Type t)
  {
    switch (t) {
    case AttrParams::NoParam:
      os << "no attribute type";
      break;
    case AttrParams::Int:
      os << "integer attribute type";
      break;
    case AttrParams::IntList:
      os << "integer-list attribute type";
      break;
    case AttrParams::Str:
      os << "string attribute type";
      break;
    case AttrParams::StrList:
      os << "string-list attribute type";
      break;
    }
    return os;
  }

  int Data::add_attr(const gc_string &n,AttrParams::Type type)
  {
    Data::AttrHash &c = get_attr_hash();
    int cn = c.size();
    c.insert(make_pair(n,Attr(cn,type)));
    return cn;
  }

  // Add to the list of C++ helper code.
  void Data::add_decl(Ptree *exp)
  {
    _decls.push_back(exp);
  }
  
  int Data::get_attr(const gc_string &nm, AttrParams::Type type,bool require_valid_attr)
  {
    Data::AttrHash &c = get_attr_hash();
    Data::AttrHash::iterator i = c.find(nm);
    
    if (i == c.end()) {
      if (require_valid_attr) {
        RError("Unknown attribute " << nm);
      } else {
        return -1;
      }
    }

    if (i->second._type == AttrParams::NoParam) {
      // If it doesn't yet have a type, then set it.
      i->second._type = type;      
    } else if ((type != AttrParams::NoParam) && (type != i->second._type) && require_valid_attr) {
      // Otherwise, if the caller specified a type, make sure it has that type
      // if require_valid_attr is true.
      RError("Bad type for attribute " << nm << ":  Found '" << type << "' and expected '" << i->second._type << "'."); 
    }

    return i->second._id;
  }

  int Data::get_attr(const gc_string &nm)
  {
    int err = -1;
    Data::AttrHash &c = get_attr_hash();
    Data::AttrHash::iterator i = c.find(nm);
    
    return (i == c.end()) ? err : i->second._id;
  }

  const char *Data::get_attr_name(int s)
  {
    Data::AttrHash &c = get_attr_hash();
    ForEach(c,i) {
      if (i->second._id == s) {
        return i->first.c_str();
      }
    }
    return 0;
  }
  
  void Data::dump_attrs(std::ostream &o)
  {
    Data::AttrHash &c = get_attr_hash();
    ForEach(c,i) {
      o << i->first << " = " << i->second._id << "\n";
    }
  }

  const System *Data::get_sys(const gc_string &name) const
  {
    SysHash::const_iterator i = _systems.find(name);
    return (i == _systems.end()) ? 0 : i->second;
  }

  Core *Data::get_core(const gc_string &name)
  {
    CoreHash::iterator i = _cores.find(name);
    return (i == _cores.end()) ? 0 : i->second;
  }

  const Core *Data::get_core(const gc_string &name) const
  {
    return const_cast<Data&>(*this).get_core(name);
  }

  // Iterates over each core, finalizing it.
  void Data::finalize(MsgLog &ml)
  {
    ForEach(_cores,i) {
      // TBD: may be can have cores wit different endianness
      if(i->second->bitEndianness() != _cores.begin()->second->bitEndianness()) {
        PError(loc(),"All cores in architecture must have the same bit endianness.");
      }
    }   
    ForEach(_cores,i) {      
      i->second->finalize(ml);
    }
       
    ForEach(_systems,i) {
      i->second->finalize(ml);
    }    
    
     
  }

  void Data::dump(ostream &o,const gc_string &pfx) const
  {
    if (!_decls.empty()) {
      o << pfx << "=======================\n"
        << pfx << "C++ Outer Declarations:\n"
        << pfx << "=======================\n";
      ForEach(_decls,i) {
        o << pfx << "\n" << *i
          << pfx << "\n\n//-------------------------\n";
      }
      o << "\n";
    }
    if (!_cores.empty()) {
      o << pfx << "======\n"
        << pfx << "Cores:\n"
        << pfx << "======\n";
      ForEach(_cores,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }
    if (!_systems.empty()) {
      o << pfx << "=======\n"
        << pfx << "Systems:\n"
        << pfx << "=======\n";
      ForEach(_systems,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }
  }

  //
  // A common class for any item that may be within a system.
  //

  const SysItem *find_item(const ResItems &c,const gc_string &s)
  {
    auto iter = c.find(s);
    return (iter == c.end()) ? 0 : iter->second;
  }

  const SysItem * Resources::contains(const RegBase &r) const
  {
    if (const SysItem *si = find_item(_regs,r.name())) {
      return si;
    } else {
      return find_item(_regfiles,r.name());
    }
  }

  const SysItem * Resources::contains(const Reg &r) const
  {
    return find_item(_regs,r.name());
  }

  const SysItem * Resources::contains(const RegFile &r) const
  {
    return find_item(_regfiles,r.name());
  }

  const SysItem * Resources::contains(const MmuLookup &m) const
  {
    return find_item(_mmu,m.name());
  }

  const SysItem * Resources::contains(const Parm &p) const
  {
    return find_item(_parms,p.name());
  }

  const SysItem * Resources::contains(const Cache &c) const
  {
    return find_item(_caches,c.name());
  }

  const SysItem * Resources::contains(const Memory &m) const
  {
    return find_item(_memories,m.name());
  }

  const SysItem * Resources::contains(const EventBus &eb) const
  {
    return find_item(_eventbuses,eb.name());
  }

  const SysItem * Resources::contains(const ExtResource &er) const
  {
    return find_item(_extresources,er.name());
  }


  void replace_insert(ResItems &trg,const ResItems &src)
  {
    ResItems tmp = trg;
    trg = src;
    trg.insert(tmp.begin(),tmp.end());
  }

  void Resources::update(const Resources &x) 
  {
    replace_insert(_regs,x._regs);
    replace_insert(_regfiles,x._regfiles);
    replace_insert(_mmu,x._mmu);
    replace_insert(_parms,x._parms);
    replace_insert(_caches,x._caches);
    replace_insert(_memories,x._memories);
    replace_insert(_eventbuses,x._eventbuses);
    replace_insert(_extresources,x._extresources);
  }

  bool Resources::empty() const 
  {
    return ( _regs.empty() && _regfiles.empty() && _mmu.empty() && 
             _parms.empty() && _caches.empty() && _memories.empty() &&
             _eventbuses.empty() && _extresources.empty() );
  }

  void Resources::dump(std::ostream &o,const gc_string &pfx) const
  {
    if (!_regs.empty()) {
      o << pfx << "Registers:  ";
      printSequenceKeys(o,_regs,true);
    }
    if (!_regfiles.empty()) {
      o << pfx << "Register-files:  ";
      printSequenceKeys(o,_regfiles,true);
    }
    if (!_mmu.empty()) {
      o << pfx << "MMU lookups:  ";
      printSequenceKeys(o,_mmu,true);
    }
    if (!_parms.empty()) {
      o << pfx << "Parameters:  ";
      printSequenceKeys(o,_parms,true);
    }
    if (!_caches.empty()) {
      o << pfx << "Caches:  ";
      printSequenceKeys(o,_caches,true);
    }
    if (!_memories.empty()) {
      o << pfx << "Memories:  ";
      printSequenceKeys(o,_memories,true);
    }
    if (!_eventbuses.empty()) {
      o << pfx << "Event buses:  ";
      printSequenceKeys(o,_eventbuses,true);
    }
    if (!_extresources.empty()) {
      o << pfx << "External resources:  ";
      printSequenceKeys(o,_extresources,true);
    }
  }

  SysItem::SysItem(Opencxx::Environment *env,Ptree *loc,const gc_string &name) :
    Aspects(env,loc),
    Named(name)
  {
  }

  void SysItem::add_shared_reg(const gc_string &name)
  {
    add_shared_reg(name,this);
  }

  void SysItem::add_shared_regfile(const gc_string &name)
  {
    add_shared_regfile(name,this);
  }

  void SysItem::add_shared_mmu(const gc_string &name)
  {
    add_shared_mmu(name,this);
  }

  void SysItem::add_shared_parm(const gc_string &name)
  {
    add_shared_parm(name,this);
  }

  void SysItem::add_shared_cache(const gc_string &name)
  {
    add_shared_cache(name,this);
  }

  void SysItem::add_shared_memory(const gc_string& name)
  {
    add_shared_memory(name,this);
  }

  void SysItem::add_shared_eventbus(const gc_string& name)
  {
    add_shared_eventbus(name,this);
  }

  void SysItem::add_shared_extresource(const gc_string& name)
  {
    add_shared_extresource(name,this);
  }

  void SysItem::add_shared_reg(const gc_string &name,const SysItem *owner)
  {
    _shared._regs.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_regfile(const gc_string &name,const SysItem *owner)
  {
    _shared._regfiles.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_mmu(const gc_string &name,const SysItem *owner)
  {
    _shared._mmu.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_parm(const gc_string &name,const SysItem *owner)
  {
    _shared._parms.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_cache(const gc_string &name,const SysItem *owner)
  {
    _shared._caches.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_memory(const gc_string &name,const SysItem *owner)
  {
    _shared._memories.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_eventbus(const gc_string &name,const SysItem *owner)
  {
    _shared._eventbuses.insert(make_pair(name,owner));
  }

  void SysItem::add_shared_extresource(const gc_string &name,const SysItem *owner)
  {
    _shared._extresources.insert(make_pair(name,owner));
  }


  const SysItem * SysItem::is_shared(const Reg &reg) const 
  { 
    return _shared.contains(reg);
  }

  const SysItem * SysItem::is_shared(const RegFile &rf) const 
  { 
    return _shared.contains(rf);
  }

  const SysItem * SysItem::is_shared(const RegBase &rb) const 
  { 
    if (const Reg *r = dynamic_cast<const Reg*>(&rb)) {
      return is_shared(*r);
    }
    else if (const RegFile *rf = dynamic_cast<const RegFile*>(&rb)) {
      return is_shared(*rf);
    } else {
      abort();
    }    
  }

  const SysItem * SysItem::is_shared(const MmuLookup &ml) const 
  { 
    return _shared.contains(ml);
  }

  const SysItem * SysItem::is_shared(const Parm &p) const 
  { 
    return _shared.contains(p);
  }

  const SysItem * SysItem::is_shared(const Cache &c) const
  {
    return _shared.contains(c);
  }

  const SysItem * SysItem::is_shared(const Memory &m) const
  {
    return _shared.contains(m);
  }

  const SysItem * SysItem::is_shared(const EventBus &eb) const
  {
    return _shared.contains(eb);
  }

  const SysItem * SysItem::is_shared(const ExtResource &er) const
  {
    return _shared.contains(er);
  }

  //
  // Information about a collection of cores or other systems.
  //

  System::System(Opencxx::Environment *env,Ptree *loc,const gc_string &name) :
    SysItem(env,loc,name),
    _share_loc(0)
  {
  }

  const System *System::get_sys(const gc_string &name) const
  {
    SysHash::const_iterator i = _local_systems.find(name);
    return (i == _local_systems.end()) ? 0 : i->second;
  }

  const Core *System::get_core(const gc_string &name) const
  {
    CoreHash::const_iterator i = _local_cores.find(name);
    return (i == _local_cores.end()) ? 0 : i->second;
  }

  const SysItem *System::get_item(const gc_string &name) const
  {
    ForEach(_items,iter) {
      if (iter->first == name) {
        return iter->second;
      }
    }
    return 0;
  }

  const System *System::get_sys_item(const gc_string &name) const
  {
    return dynamic_cast<const System *>(get_item(name));
  }

  const Core *System::get_core_item(const gc_string &name) const
  {
    return dynamic_cast<const Core *>(get_item(name));
  }

  void System::add_shared_reg(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_reg(name,owner);
    ForEach(_items,i) {
      i->second->add_shared_reg(name,owner);
    }
  }

  void System::add_shared_regfile(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_regfile(name,owner);
    ForEach(_items,i) {
      i->second->add_shared_regfile(name,owner);
    }
  }

  void System::add_shared_mmu(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_mmu(name,owner);
    ForEach(_items,i) {
      i->second->add_shared_mmu(name,owner);
    }
  }

  void System::add_shared_parm(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_parm(name,owner);
    ForEach(_items,i) {
      i->second->add_shared_parm(name,owner);
    }
  }

  void System::add_shared_cache(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_cache(name,owner);
    ForEach(_items,i) {
      i->second->add_shared_cache(name,owner);
    }
  }

  void System::add_shared_memory(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_memory(name,owner);
    ForEach(_items,i) {
      i->second->add_shared_memory(name,owner);
    }
  }


  void System::add_shared_eventbus(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_eventbus(name,owner);
    ForEach(_items,i) {
      auto x = *i;
      x.second->add_shared_eventbus(name,owner);
    }
  }

  void System::add_shared_extresource(const gc_string &name,const SysItem *owner)
  {
    SysItem::add_shared_extresource(name,owner);
    ForEach(_items,i) {
      auto x = *i;
      x.second->add_shared_extresource(name,owner);
    }
  }


  bool System::check_is_local(SysItem &si) const
  {
    ForEach(_local_systems,iter) {
      if (iter->second == &si) {
        return true;
      }
    }
    ForEach(_local_cores,iter) {
      if (iter->second == &si) {
        return true;
      }
    }
    return false;
  }

  int System::numCores() const
  {
    int count = 0;
    ForEach(_items,i) {
      count += i->second->numCores();
    }
    return count;
  }

  // The system simply passes the back, querying the first of its constituent
  // items.  This should be okay, as a shared resource must exist in all children
  // (this is checked when the shared resources is added).
  const Reg *System::get_reg(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_reg(name);
  }

  const RegFile *System::get_regfile(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_regfile(name);
  }

  const MmuLookup *System::get_mmulookup(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_mmulookup(name);
  }

  const Parm *System::get_parm(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_parm(name);
  }

  const Cache *System::get_cache(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_cache(name);
  }

  const Memory *System::get_memory(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_memory(name);
  }

  const EventBus *System::get_eventbus(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_eventbus(name);
  }

  const ExtResource *System::get_extresource(const gc_string &name) const
  {
    assert(!_items.empty());
    return _items.front().second->get_extresource(name);
  }

  template <class T>
  void System::check_shared_object(const gc_string &name,const gc_string &descr,
                                   const T * (SysItem::*get)(const gc_string &) const) const
  {
    const T *sr = 0;
    ForEach(_items,iter) {
      SysItem &si = *(iter->second);
      // Now make sure that all constituent items are locally-defined.
      if (!check_is_local(si)) {
        PError(_share_loc,"Attempt to add shared resource but constituent item '" << si.name() << "' is not locally declared.");
      }

      if (const T *r = (si.*get)(name)) {
        if (!sr) {
          sr = r;
        } else if (!(*r == *sr)) {
          PError(_share_loc,"Shared " << descr << " '" << name << "' is not identical across items in this system.");
        }
        r->check_sharing_okay();
      } else {
        PError(_share_loc,"Unknown shared " << descr <<  " " << name);
      }
    }
  }

  void System::finalize(MsgLog &ml)
  {
    // Finalize all constituent items.
    ForEach(_local_cores,i) {
      i->second->finalize(ml);
    }
    ForEach(_local_systems,i) {
      i->second->finalize(ml);
    }

    // Now do error checking on shared resources.
    ForEach(_shared._regs,item) {
      LogPError(ml,check_shared_object<Reg>(item->first,"register",&SysItem::get_reg));
    }

    ForEach(_shared._regfiles,item) {
      LogPError(ml,check_shared_object<RegFile>(item->first,"register file",&SysItem::get_regfile));
    }

    ForEach(_shared._mmu,item) {
      LogPError(ml,check_shared_object<MmuLookup>(item->first,"mmu lookup",&SysItem::get_mmulookup));
    }

    ForEach(_shared._parms,item) {
      LogPError(ml,check_shared_object<Parm>(item->first,"parameter",&SysItem::get_parm));
    }

    ForEach(_shared._memories,item) {
      LogPError(ml,check_shared_object<Memory>(item->first,"memory",&SysItem::get_memory));
    }

    ForEach(_shared._eventbuses,item) {
      LogPError(ml,check_shared_object<EventBus>(item->first,"event bus",&SysItem::get_eventbus));
    }

    ForEach(_shared._extresources,item) {
      LogPError(ml,check_shared_object<ExtResource>(item->first,"external resource",&SysItem::get_extresource));
    }

    ForEach(_shared._caches,item) {
      LogPError(ml,{
          check_shared_object<Cache>(item->first,"cache",&SysItem::get_cache);
          // For each shared cache, each subsequent one must be shared.  The
          // following code is a bit redundant, since we really only need to check
          // from the top down, rather than each one, but there should never be a
          // lot of them, so it shouldn't be a performance issue.
          for (const Cache *c = get_cache(item->first)->next(); c; c = c->next()) {
            if (!is_shared(*c)) {
              PError(c->loc(),"Cache " << item->first << " is shared but subsequent cache " << c->name() << " is not.");
            }
          }
        });
    }

    // Now check to make sure that child-id indexing doesn't exceed any
    // register-file sizes.
    check_child_indexing(ml,0);
  }

  void System::check_child_indexing(MsgLog &ml,unsigned child_id)
  {
    ForEach(_items,i) {
      i->second->check_child_indexing(ml,child_id++);
    }
  }

  void System::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "System:  " << name() << "\n";
    gc_string pfx2 = indent(pfx);
    gc_string pfx3 = indent(pfx2);
    gc_string pfx4 = indent(pfx3);
    if (!_type.empty()) {
      o << pfx2 << "Type:  " << _type << "\n";
    }
    if (!_shared.empty()) {
      o << pfx2 << "Shared Resources:\n";
      _shared.dump(o,pfx3);
    }
    if (!_local_cores.empty()) {
      o << pfx2 << "Locally-defined Cores:\n";
      printHashOrdered(o,pfx3,_local_cores);
    }
    if (!_local_systems.empty()) {
      o << pfx2 << "Locally-defined Systems:\n";
      printHashOrdered(o,pfx3,_local_systems);
    }
    o << pfx2 << "Constituent Items:\n";  
    ForEach(_items,i) {
      o << pfx3 << i->first << ":  " << i->second->name() << "\n";
    }
    o << pfx << "-------------------------------\n";
  }

  //
  // Information about a single architecture.
  //

  Core::Core(Opencxx::Environment *env,Ptree *loc,const gc_string &name) :
    SysItem(env,loc,name),
    _active_watch(0),
    _itable_watch(0),
    _ea_mask(env,loc,EaMaskName),
    _ra_mask(env,loc,RaMaskName),
    _immediate_writehooks(ImmNotSet),
    _cia(0),
    _nia(0),
    _mmu(0),
    _exception_is_branch(true),
    _has_instr_memory(false),
    _has_prefetch_memory(false),
    _has_micro_op_offset(false),
    _max_micro_ops(0),
    _parallel_execution(0),
    _packetized_encoding(false),
    _packet_pos_incr(0),
    _max_prefix_bits(0),
    _num_ret_fields(0),
    _mem_types(0),
    _bitEndianness(Core::Big),
    _instrEndianness(Core::Big),
    _dataEndianness(Core::Big),
    _asmcfg(env,loc),
    _max_write_delay(0),
    _post_fetch_skip(false),
    _mmu_handling(false),
    _reserved_bits_value(RsrvNotSet), 
    _is_CIA_commit_write(false),
    _reg_accesses(0)
  {
  }

  template <class C>
  void LoggedHash(C &container,MsgLog &ml,Core &core)
  {
    ForEach(container,i) {
      LogPError(ml,i->second->finalize(core,ml));
    }
  }

  template <class C>
  void LoggedIdHash(C &container,MsgLog &ml,Core &core,unsigned id_start)
  {
    ForEach(container,i) {
      LogPError(ml,i->second->finalize(core,ml,id_start++));
    }
  }

  template <class C>
  void LoggedList(C &container,MsgLog &ml,Core &core)
  {
    ForEach(container,i) {
      LogPError(ml,(*i)->finalize(core,ml));
    }
  }

  void check_enums(Ptree *loc, const StrIntList &enums) 
  {
    if (enums.empty()) {
      return;
    }
    // Check for uniqueness.  We allow the special reserved gc_string to be repeated,
    // but nothing else may be.
    set<gc_string> used_enums;
    ForEach(enums,i) {
      if (i->first == ReservedEnum) {
        continue;
      }
      
      if (used_enums.count(i->first)) {
        PError(loc,"Duplicate enumeration value '" << i->first << "' found.");
      }
      used_enums.insert(i->first);
    }
    
    if (used_enums.empty()) {
      PError(loc,"The enumerated field has no non-reserved entries.");
    } else if (used_enums.size() == 1) {
      PError(loc,"An enumerated field must have more than one item.");
    }
  }


  struct CheckCode {

    CheckCode(Environment *env,const Core &core,
              bool return_ok = true,
              bool reg_targets_ok = true) : 
      _core(core),
      _nccore(0),
      _vw(env,&core,0,return_ok)
    {
      init(reg_targets_ok);
    }

    CheckCode(Environment *env,Core &core,
              bool return_ok = true,
              bool reg_targets_ok = true) : 
      _core(core),
      _nccore(&core),
      _vw(env,&core,&core,return_ok)
    {
      init(reg_targets_ok);
    }

    void init(bool reg_targets_ok)
    {
      _reg_trg_ok = reg_targets_ok;
      _has_parms = false;
      _is_instr_code = false;
      _is_asm_code = false;
      _record_helper_usage = true;
      _ignore_escapes = false;
      _translated = 0;
    }

    void set_is_instr_code(bool i) { _is_instr_code = i; };
    void set_is_asm_code(bool i) { _is_asm_code = i; };
    void set_record_helper_usage(bool r) { _record_helper_usage = r; };
    void set_return_ok(bool r) { _vw.set_return_ok(r); };
    void set_reg_targets_ok(bool r) { _reg_trg_ok = r; };
    void set_func_call_info(const gc_string &name,PtreeArray *args) { _vw.set_func_call_info(name,args); }
    void set_instr(const InstrBase *i) { _vw.set_instr(i); };
    void set_ignore_escapes(bool i) { _ignore_escapes = i; };

    bool has_parms() const { return _has_parms; };

    const VarList &escapes() const { return _escapes; };
    const VarList &srcs() const { return _vw.get_srclist(); };
    const VarList &trgs() const { return _vw.get_trglist(); };

    const UsageInfo &usageinfo() const { return _vw.usageinfo(); };

    const gc_string &mo_offset_field() const { return _vw.mo_offset_field(); };

    Ptree *translated() const { return _translated; };

    // Filters out known resources.  Unknown resources are stored in the _escapes
    // list.
    void check_code_filter(Ptree* action)
    {
      // Check the action for unknown variables.
      _translated = 0;
      _has_parms = false;
      _escapes.clear();
      if (action) {
        _translated = _vw.Translate(action);
        // If we found escaping vars, make sure that they map to registers, register files, or caches.
        _escapes = _vw.get_escapes();
        for (auto i = _escapes.begin(); i != _escapes.end(); ) {
          if (_core.is_resource(i->second)) {
            if (_core.get_parm(i->second)) {
              _has_parms = _has_parms || true;
            }
            _escapes.erase(i++);
          } else {
            ++i;
          }
        }
        if (!_reg_trg_ok) {
          // If necessary, make sure that we're not targeting any registers.
          auto tl = _vw.get_trglist();
          ForEach(tl,i) {
            if (_core.get_reg(i->second) || _core.get_regfile(i->second)) {
              PError(i->first,"Register modifications are not allowed here.");
            }
          }
        }
      }
      if (_nccore) {
        if (_record_helper_usage) {
          if (_is_instr_code) {
            _nccore->_instr_helpers.add(usageinfo()._helpers);
          } 
          else if (_is_asm_code) {
            _nccore->add_asm_helpers(usageinfo()._helpers);
          }
          else {
            _nccore->_non_instr_helpers.add(usageinfo()._helpers);
          }
        }
        _nccore->_used_helpers.add(usageinfo()._helpers);
        // We have to iterate and manually insert register info so that we add
        // register files w/o any of the begin/end access-specific stuff.
        record_used_regs(usageinfo()._src_regs);
        record_used_regs(usageinfo()._trg_regs);
      }
    }

    void record_used_regs(const UsedRegs &urs)
    {
      ForEach(urs,i) {
        const UsedReg &ur = *i;
        _nccore->_used_regs.insert(ur.regb());
      }
    }
  
    // Generates errors for any variables which don't map to known resources.
    void check_code(Ptree* &action,const StrSet &excepts = StrSet())
    {
      check_code_filter(action);
      if (!_ignore_escapes) {
        ForEach(_escapes,i) {
          if (!excepts.count(i->second)) {
            PError(i->first,"Unknown variable/resource found in action code:  " << i->first);    
          }
        }
      }
      action = _translated;
    }

    // Same as above, but also records usage info.
    void check_code(HelperInfo &info,const StrSet &excepts = StrSet())
    {
      check_code(info._func,excepts);
      info.update(usageinfo());
    }

    const Core &_core;
    Core        *_nccore;     // Optional non-const Core for updating information.
    VarWalker   _vw;
    bool        _reg_trg_ok;
    bool        _has_parms;
    bool        _is_instr_code;
    bool        _is_asm_code;
    bool        _record_helper_usage;
    bool        _ignore_escapes;
    VarList     _escapes;
    Ptree      *_translated;
  };
  
  void Core::pushRegAccess(PtreeRegRead *p)
  {
    if (!p) return;
    if (!_reg_accesses) {
      _reg_accesses = p;
    } else {
      p->_next = _reg_accesses;
      _reg_accesses = p;
    }
  }

  void Core::add_delayed(const gc_string &d,Opencxx::Ptree *dexpr)
  {
    _delayed_writes.insert(d);

    // If we can statically determine the delay, then store the max of the
    // values encountered.
    unsigned delay;
    if (get_uint(delay,dexpr,env())) {
      _max_write_delay = max(_max_write_delay,delay);
    }
  }

  // Helper function for recording in othe core any delayed writes (registers or
  // memory).
  void record_delayed_writes(Core &core,const UsageInfo &usageinfo)
  {
    ForEach(usageinfo._trg_regs,i) {
      if (i->delayed()) {
        core.add_delayed(i->regb()->name(),i->_delay);
      }
    }

    ForEach(usageinfo._trg_mems,i) {
      if (i->delayed()) {
        core.add_delayed(i->name(),i->_delay);
      }
    }
  }

  void Core::add_concurrent(const gc_string &d) 
  {
    _concurrent_writes.insert(d);
  }

  void Core::pc_setup()
  {
    // We must have an NIA and CIA register for this model to be valid.

    // First, find the cia and nia registers.  The attr
    // should always exist b/c it's added by Core.
    int cia_attr  = Data::get_attr(Cia);
    int nia_attr = Data::get_attr(Nia);
    assert (cia_attr >= 0 && nia_attr >= 0);
  
    // We check all registers to ensure that there isn't
    // more than one.
    ForEach(_regs,i) {
      if (i->second->has_attr(cia_attr)) {
        if (_cia) {
          PError(loc(),"More than one register is identified as a current-instruction-address register:  " 
                 << _cia->name() << " and " << i->second->name() << ".");
        } else {
          _cia = i->second;
        }
      }
      if (i->second->has_attr(nia_attr)) {
        if (_nia) {
          PError(loc(),"More than one register is identified as a next-instruction-address register:  " 
                 << _nia->name() << " and " << i->second->name() << ".");
        } else {
          _nia = i->second;
        }
      }
    }

  }

  void Core::updateInstrFieldReloc(const gc_string &ifname,const Relocation *reloc)
  {
    auto f = get_instrfield(ifname);
    assert(f);

    f->_relocs.push_back(reloc);
  }
  
  void Core::add_helper(const gc_string &name,Opencxx::Ptree *func)
  {
    _helpers[name] = func;
  }

  void Core::add_type_decl(Opencxx::Ptree *decl)
  {
    _type_decls.push_back(decl);
  }

  AuxFunc *Core::add_aux_func(Opencxx::Ptree *func,const gc_string &rtype,
                              const gc_string &comment,bool isConst)
  {
    auto af = new AuxFunc(func,rtype,comment,isConst);
    _aux_funcs.push_back(af);
    return af;
  }

  bool Core::del_aux_func(const AuxFunc *aux)
  {
    if (!aux) return false;

    ForEach(_aux_funcs,i) {
      if ( *i == aux ) {
        _aux_funcs.erase(i);
        return true;
      }
    }
    return false;
  }

  void check_reghook_child_indexing(MsgLog &ml,Ptree *loc,const char *msg,
                                    const RegName &rn,unsigned child_id)
  {
    if (rn.has_child_id()) {
      if (!rn.regfile()) {
        // Is this redundant?
        AddError(ml,loc,"Target of child-id indexed " << msg << " is not a register-file.");
        return;
      }
      if (child_id >= rn.regfile()->size()) {
        AddError(ml,loc,"Number of children exceeeds size of register-file for child-id indexed " << msg << ".");
      }
    }
  }

  void check_reghook_child_indexing(MsgLog &ml,Ptree *loc,const RegHook &rh,unsigned child_id)
  {
    if (rh.aliased()) {
      check_reghook_child_indexing(ml,loc,"alias",rh.alias(),child_id);
    }
  }

  void Core::check_child_indexing(MsgLog &ml,unsigned child_id)
  {
    ForEach(_regs,i) {
      const Reg &reg = *i->second;
      if (const RegHook *rh = reg.readhook()) {
        check_reghook_child_indexing(ml,rh->loc(),*rh,child_id);
      }
      if (const RegHook *rh = reg.writehook()) {
        check_reghook_child_indexing(ml,rh->loc(),*rh,child_id);
      }
    }
    ForEach(_regfiles,i) {
      const RegFile &rf = *i->second;
      if (rf.sparse()) {
        ForEach(rf.entries(),j) {
          const RegEntry &re = *j->second;
          check_reghook_child_indexing(ml,re.loc(),"entry",re.reg(),child_id);
        }
      }
    }
  }

  bool Core::hasMicroInstrs() const
  {
    ForEach(_instrfields,i) {
      const InstrField &field = *i->second;

      if (field.type() == InstrField::tInstr) {
        return true;
      }
    }

    return false;
  }
  
  void Core::finalize(MsgLog &ml)
  {
    // Set endianness for bit pairs
    BitPair::set_endianness(is_big_endian());
    // Check for duplicate parm names-
    // for now, they must be unique even across
    // parameters.  This is due to the stupid requirements
    // of C++.
    ForEach(_parms,i) {
      const StrSet &options = i->second->options();
      ForEach(options,j) {
        
        auto ip = _alloptions.insert(make_pair(*j,i->second));
        
        if (*j == "true" || *j == "false") {
          continue;
        }
	
        if (!ip.second) {
          PError(i->second->loc(),"Parameter option '" << *j << 
                 "' already used by parameter '" << ip.first->second->name() << "'.");
        }
      }
    }

    // Populate the global MMU lookup hash, if needed, so that we know all of
    // the names that might occur in code.
    if (_mmu) {
      _mmu->populate_global_lookups(ml);
    }
    // Finalize all groups.
    //   LoggedHash(_groups,ml,*this);
    
    // Finalize all parameters.
    LoggedHash(_parms,ml,*this);

    // Finalize all registers first- this will populate the
    // register hashes of any owning register files.
    LoggedIdHash(_regs,ml,*this,0);

    // Now handle all register files.  We want to set the size first, before finalizing, so that
    // we don't run into issues doing range checking, since an aliased register-file might not yet
    // have its size set due to ordering issues.
    ForEach(_regfiles,i) {
      LogPError(ml,i->second->finalize_alias(*this,ml));
    }
    LoggedIdHash(_regfiles,ml,*this,_regs.size());

    // This sets up pointers to the current and next instruction registers (which
    // might be the same register).
    LogPError(ml,pc_setup());

    if (!_cia) {
      PError(loc(),"Did not find any registers identified as a current-instruction-address register (of class " << Cia << ").");
    }

    if (!_nia) {
      PError(loc(),"Did not find any registers identified as a next-instruction-address register (of class " << Nia << ").");
    }

    // Handle all exceptions.
    LoggedHash(_exceptions,ml,*this);

    // Handle the MMU, if appropriate.
    if (_mmu) {
      LogPError(ml,_mmu->finalize(ml,*this));
    }

    // Only one instruction memory may be defined.
    Memory *prefetch_mem=0, *instr_mem=0; 
    LogPError(ml, {
        ForEach(_memories,i) {
          Memory *mem = i->second;
          if (mem->instr_mem()) {
            if(_has_instr_memory) {
              PError(loc(),"More than one instruction memory for core:  " << i->second->name());
            }
            instr_mem = mem;
            _has_instr_memory = true;
          }
          if (mem->prefetch_mem()) {
            if (mem->addr_unit()) {
              PError(loc(),"Pre-fetch memory can not have adressing unit.");
            } 
            if (_has_prefetch_memory) {
              PError(loc(),"More than one prefetch memory for core:  " << i->second->name());
            }
	
            prefetch_mem = mem;
            _has_prefetch_memory = true;
          }
        } 
      });
    
    if (_has_prefetch_memory && _has_instr_memory) {
      prefetch_mem->_addr_unit = instr_mem->_addr_unit;
    }

    // We have packetized encodings if we have either a parallel-execution
    // architecture (VLIW) or we encode into micro-instructions.
    if (_parallel_execution) {
      _packetized_encoding = true;
    }
    if (hasMicroInstrs()) {
      _packetized_encoding = true;
    }
   

    // Handle all memories.  Memory IDs start at 1 so that the
    // global memory can always considered to be 0.
    LoggedIdHash(_memories,ml,*this,1);

    // Handle all event buses.
    LoggedHash(_eventbuses,ml,*this);

    // Handler all external resources.
    LoggedHash(_extresources,ml,*this);

    // Handle AsmConfig. AsmRules should be finalized prior to instructions
    _asmcfg.finalize(*this,ml);

    // Handle all contexts.
    LoggedIdHash(_contexts,ml,*this,0);
    
    // Finalize instruction fields and instructions.
    finalize_instrs(ml);

    // Check the cache structure.
    finalize_caches(ml);

    _ra_mask.finalize(*this);
    _ea_mask.finalize(*this);

    StrSet excepts;
    ForEach(_instrfields,i) {
      InstrField &f = *(i->second);
      if (is_prefix_info(f.name())) {
        excepts.insert(f.name());
      }
    }

    if (_parallel_execution) {
      excepts.insert("skip_instruction");
    }

    // The post-fetch handler should take an unsigned integer.
    LogPError(ml,{
        if (_post_fetch_handler) {
          CheckCode cc(_env,*this,false);
          cc.check_code_filter(_post_fetch_handler._func);
          auto escapes = cc.escapes();
          ForEach(escapes,i) {
            if (_parallel_execution && i->second == "skip_instruction") {
              _post_fetch_skip = true;
            } else if (!excepts.count(i->second)) {
              PError(i->first,"Unknown variable/resource found in action code:  " << i->first);
            }
          }
          _post_fetch_handler._func = cc.translated();
          _post_fetch_handler.update(cc.usageinfo());
	    
          // First argument is the instruction's size.
          if (!arg_is_uint(_post_fetch_handler._func,0)) {
            PError(_post_fetch_handler._func,"First argument to post-fetch handler must be an unsigned integer.");
          }
        }
      });

    excepts.insert("skip_instruction");

    // These functions take no parameters, so we just check for unknown resources.
    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_pre_fetch_handler,excepts); 
      });

    excepts.clear();

    LogPError(ml,{ 
        CheckCode cc(_env,*this); 
        cc.check_code(_itable_watch,excepts); 
      });

    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_pre_cycle_handler); 
      });

    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_post_cycle_handler); 
      });

    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_branch_taken_handler); 
      });

    LogPError(ml,{ 
        if (_addr_check_handler) {
          CheckCode cc(_env,*this,false); 
          cc.check_code(_addr_check_handler); 
          int arity = get_arity(_addr_check_handler._func);
          if (! (arity == 3 || arity == 6) ) {
            PError(_addr_check_handler._func,"The address-check function takes either 3 or 6 arguments.");
          } 
          if (!arg_is_addr(_addr_check_handler._func,0)) {
            PError(_addr_check_handler._func,"First argument to address-check handler must be of type addr_t.");
          }
          if (!arg_is_addr(_addr_check_handler._func,1)) {
            PError(_addr_check_handler._func,"Second argument to address-check handler must be of type addr_t.");
          }
          if (!arg_is_type(_addr_check_handler._func,"TransType",2)) {
            PError(_addr_check_handler._func,"Third argument to address-check handler must be of type TransType.");
          }
          if (arity > 3) {
            if (!arg_is_int(_addr_check_handler._func,3)) {
              PError(_addr_check_handler._func,"Fourth argument to address-check handler must be of type int.");
            }
            if (!arg_is_int(_addr_check_handler._func,4)) {
              PError(_addr_check_handler._func,"Fifth argument to address-check handler must be of type int.");
            }
            if (!arg_is_int(_addr_check_handler._func,5)) {
              PError(_addr_check_handler._func,"Sixth argument to address-check handler must be of type int.");
            }
          }
        }
      });

    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_pre_exception_handler); 
        if (!arg_is_type(_pre_exception_handler._func,"Exception",0)) {
          PError(_pre_exception_handler._func,"Argument to pre-exception handler must be of type Exception.");
        }
      });

    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_post_exception_handler); 
        if (!arg_is_type(_post_exception_handler._func,"Exception",0)) {
          PError(_post_exception_handler._func,"Argument to post-exception handler must be of type Exception.");
        }
      });

    if (_invalid_entry_read) {
      LogPError(ml,{
          CheckCode cc(_env,*this,true);
          cc.check_code(_invalid_entry_read);
          if (!arg_is_uint(_invalid_entry_read._func,0)) {
            PError(_invalid_entry_read._func,"First argument to invalid-entry-read hook should be an unsigned integer.");
          }
          if (!arg_is_uint(_invalid_entry_read._func,1)) {
            PError(_invalid_entry_read._func,"Second argument to invalid-entry-read hook should be an unsigned integer.");
          }
        });
    }

    if (_invalid_entry_write) {
      LogPError(ml,{
          CheckCode cc(_env,*this,false);
          cc.check_code(_invalid_entry_write);
          if (!arg_is_uint(_invalid_entry_write._func,0)) {
            PError(_invalid_entry_read._func,"First argument to invalid-entry-write hook should be an unsigned integer.");
          }
          if (!arg_is_uint(_invalid_entry_write._func,1)) {
            PError(_invalid_entry_read._func,"Second argument to invalid-entry-write hook should be an unsigned integer.");
          }
        });
    }

    ForEach(_instrfields,i) {
      InstrField &f = *(i->second);
      if (is_prefix_info(f.name())) {
        excepts.insert(f.name());
      }
    }
    excepts.insert("getPacketPosition");
    excepts.insert("setPacketPosition"); 
    LogPError(ml,{ 
        if (_post_packet_handler) {
          if (!_parallel_execution) {
            PError(loc(),"Post-packet handler defined only for parallel architectures.");
          }
          CheckCode cc(_env,*this,false); 
          cc.check_code(_post_packet_handler,excepts);
        }
      });
    LogPError(ml,{ 
        if (_post_packet_commit_handler) {
          if (!_parallel_execution) {
            PError(loc(),"Post-packet-commit handler defined only for parallel architectures.");
          }
          CheckCode cc(_env,*this,false); 
          cc.check_code(_post_packet_commit_handler,excepts);
        }
      });
    LogPError(ml,{ 
        CheckCode cc(_env,*this,false); 
        cc.check_code(_post_exec_handler,excepts);
      });

    
    excepts.clear();

    LogPError(ml,{ 
        CheckCode cc(_env,*this); 
        cc.check_code(_active_watch); 
      });

    // Assemble instructions if we have instructions and we don't
    // have any errors.
    if (!_instrs.empty() && !ml.error_count()) {
      // Add default decode table if no tables were explicitly created.
      if (_instrtables.empty()) { 
        if (!add_instr_table(DefaultInstrTable, 0, _max_instr_width)) {   // Use add_instr_table() here instead
            PError(loc(), "Unable to create default instruction table."); // This error should not occur!
        }
      }

      check_prefix();

      check_assembler_fields();

      build_instrtables(ml);
    }

    ForEach(_instrtables,i) {      
      unsigned pos = i->second._pos;
      const InstrTable* itable = i->second._table;
      if (pos == 0 && !(itable->is_prefix())) {
        //FIX   PError(loc(),"Prefix instruction table should be the default table.");
      }
    }

    ForEach(_instrtables,i) {
      i->second._table->finalize();
    }

    // Extra step to check the instruction-table-type reference 
    ForEach(_instrfields, i) {		
      LogPError(ml, i->second->finalize(*this)); // For instruction-table type
    }

    // Mark instructions as being micro-ops if any instruction fields are
    // associated with instruction tables.
    tagMicroOps();   

    // Finalize all relocations.
    LoggedHash(_relocs,ml,*this);

    // Make sure that all relocation values are unique.
    checkRelocationUniqueness(ml);
	
    // Check for unknown variables in decode-retry handler and make sure
    // that it takes the right types.
    LogPError(ml,{
        if (_decode_retry_handler) {
          ForEach(_instrtables,i) { // The code may refer to instruction tables names
            excepts.insert(Data::get_attr_name(i->first));
          }
          CheckCode cc(_env,*this);
          cc.check_code(_decode_retry_handler,excepts);
          // First argument is an address
          if (!arg_is_addr(_decode_retry_handler._func,0)) {
            PError(_decode_retry_handler._func,"First argument to decode-retry handler must be an address.");
          }
          // Second argument is an unsigned integer.
          if (!arg_is_uint(_decode_retry_handler._func,1)) {
            PError(_decode_retry_handler._func,"Second argument to decode-retry handler must be an unsigned integer.");
          }
        }
      });
    LogPError(ml,{
        if (_decode_miss_handler) {
          ForEach(_instrtables,i) { // The code may refer to instruction tables names
            excepts.insert(Data::get_attr_name(i->first));
          }
          CheckCode cc(_env,*this,false);
          cc.check_code(_decode_miss_handler,excepts);
          // First argument is an address
          if (!arg_is_addr(_decode_miss_handler._func,0)) {
            PError(_decode_miss_handler._func,"First argument to decode-miss handler must be an address.");
          }
          // Second argument is an unsigned integer.
          if (!arg_is_uint(_decode_miss_handler._func,1)) {
            PError(_decode_miss_handler._func,"Second argument to decode-miss handler must be an unsigned integer.");
          }
        }
      });
    excepts.clear();
    
    create_asm_excepts(excepts,true,false);

    // Check pre-assembler hook, for now we allow only to prefix fields and assembler parameters in it.    
    LogPError(ml,{
        if (_pre_asm_handler) {
          CheckCode cc(_env,*this,false);
          cc.set_is_asm_code(true);
          cc.check_code(_pre_asm_handler,excepts);
        }
      });
  
    create_asm_excepts(excepts,false,true);

    // Check post-assembler hook, for now we allow only to use argument and prefix fields and assembler parameters in it.    
    LogPError(ml,{
        if (_post_asm_handler) {
          if (!arg_is_uint(_post_asm_handler._func,0)) {
            PError(_post_asm_handler._func,"First argument to post-asm handler must be an unsigned integer.");
          }
          CheckCode cc(_env,*this,false);
          cc.set_is_asm_code(true);
          cc.check_code(_post_asm_handler,excepts);
        }
      });
    
    // Check post-packet assembler hook, for now we allow only to prefix fields adn assembler parameters in it.      
    LogPError(ml,{
        if (_post_packet_asm_handler) {
          if (!_packetized_encoding) {
            PError(loc(),"Post-packet-asm handler defined only for packetized architectures.");
          }
          if (!arg_is_type(_post_packet_asm_handler._func,"InstrBundle",0)) {
            PError(loc(),"First argument to post-packet-asm handler must be 'InstrBundle'.");
          }
          CheckCode cc(_env,*this);
          cc.set_is_asm_code(true);
          cc.check_code(_post_packet_asm_handler,excepts);
        }
      });
    excepts.clear();

    // Check all helper functions.  We do this last so that its usage information won't be overwritten by the
    // call-specific info which gets set during the processing of other stuff. 
    check_helpers(ml);

    LoggedHash(_groups,ml,*this);

    // Right now, we require that there by a maximum of 63 instruction
    // attributes, so that we can always use a (max) 64-bit integer as a flag
    // set.  We can relax this in the future, if it's a problem, but it's
    // convenient to have right now.
    if (_instr_attrs.size() > 64) {
      PError(loc(),"A maximum of 64 total attributes may be applied to instructions.  The current number is " << _instr_attrs.size());
    }
    if (_regfile_attrs.size() > 64) {
      PError(loc(),"A maximum of 64 total attributes may be applied to register-files.  The current number is " << _regfile_attrs.size());
    }

    // In addition, we limit the number of attributes used within an MMU to those that
    // can fit within the offset portion of the smallest translation.  This allows us to cache
    // the bits for performance reasons.  Again, this is a performance optimization, so we can relax
    // this in the future if we must.
    if (_mmu) {
      ForEach( (_mmu->top_lookups()), i) {
        const MmuLookup &l = *i->second;
        if (l.is_interface() || !l.valid_translating_lookup()) continue;
        unsigned pagesize = l.min_page_size();
        unsigned nbits = log2(pagesize);
        if (_mmu_instr_attrs.size() > nbits) {
          PError(loc(),"The total number of instruction attributes used within MMU tests (" << _mmu_instr_attrs.size() << ") must be less than the "
                 "number of offset bits in the smallest translation size (which is " << nbits << " for " << l.name() << " TLB).");
        }
      }
    }

    // The nia and cia are always considered used.
    _used_regs.insert(_cia);
    _used_regs.insert(_nia);

    // We set up a list of helper info objects here, by pointing into the core
    // structure itself.  This first initialization always needs to point to the
    // first HelperInfo object in this "list".  The second points to the dummy
    // object which must always be the last object in the "list".
    _misc_funcs_begin = &_addr_check_handler;
    _misc_funcs_end   = &_end_handler;

    updateRegUsage();
  }

  void Core::checkRelocationUniqueness(MsgLog &ml)
  {
    gc_map<int,const Relocation *> rvalues;
    
    ForEach(_relocs,i) {
      const Relocation *r = i->second;
      auto ip = rvalues.insert(make_pair(r->value(),r));

      if (!ip.second) {
        AddError(ml,r->loc(),"Relocation value conflict between " << ip.first->second->name() << " and " << r->name() << ".");
      }
    }
  }
  
  // Given an access object, update the register appropriately with usage
  // information.
  void Core::updateRegUsage(RegAccess &ra)
  {
    const RegBase *rb = ra.rb();
    const RegField *rf = ra._field;

    if (ra._write) {
      if (ra._left) {
        rb->set_reg_usage(RegSliceWrite);
      } else if (rf) {
        rf->set_field_usage(RegFieldWrite);
        rb->set_reg_usage(RegFieldWrite);
      } else {
        rb->set_reg_usage(RegWrite);
      }      
    } else {
      if (ra._left) {
        rb->set_reg_usage(RegSliceRead);
      } else if (rf) {
        rf->set_field_usage(RegFieldRead);
        rb->set_reg_usage(RegFieldRead);
      } else {
        rb->set_reg_usage(RegRead);
      }
    }
  }

  void Core::updateRegUsage()
  {
    PtreeRegRead *p = _reg_accesses;

    while (p) {
      updateRegUsage(p->_regaccess);
      p = p->_next;
    }
    
    // We always want to be able to read and write the nia and cia.
    _cia->set_reg_usage(RegRead | RegWrite);
    _nia->set_reg_usage(RegRead | RegWrite);

    // We no longer need this list, so don't hold a poiner, incase the gc can
    // dispose of the objects.
    _reg_accesses = 0;
  }

  // Finalize instruction fields and instructions.
  void Core::finalize_instrs(MsgLog &ml)
  {
    // Handle all instruction fields.  
    // First finalize those which are target of a 'type' key
    // This doesn't finalize the action b/c we
    // must update prefix_excepts with instruction field information before
    // handling the action code so that any helpers that are called can also be
    // scanned.
    StrSet  instrfield_type_target;
    InstrField::InstrFieldMMap nested;
    InstrFieldHash instrfield_aliases;
    ForEach(_instrfields,i) {
      InstrField &f = *(i->second);
      if (f.type() == InstrField::tRefField) {
        IfFind(it,_instrfields,f.field_type_name()) {
          IfNotFind(j,instrfield_type_target,f.field_type_name()) {
            LogPError(ml,it->second->finalize(*this,ml,nested));
            instrfield_type_target.insert(it->first);
          }
        }
      }
    }
  
    ForEach(_instrfields,i) {
      IfFind(it,instrfield_type_target,i->first) {
        continue;
      }
      if (i->second->is_alias()) {
        instrfield_aliases.insert(*i);
      } else {
        LogPError(ml,i->second->finalize(*this,ml,nested));
      }
    }

    ForEach(nested,i) {
      LogPError(ml,i->second->finalize(*this,ml,_instrfields,i->first));
    }

    // We allow prefix information to be used in helpers, so we build up this
    // set here so that it may be used by other callers of process_helper.
    ForEach(_instrfields,i) {
      InstrField &f = *(i->second);
      if (f.is_prefix()) {
        _prefix_excepts.insert(f.name());
      } 
    }
    
    ForEach(_instrfields,i) {
      LogPError(ml,i->second->finalize_action(*this));
    }

    InstrHash 	  generated;
    InstrHash 	  dummies;

    // Dummy instructions are instructions which have the name of a
    // sub-instructions.  They're used to apply modifications to all instructions
    // which are created with the sub-instruction of their name.  We remove them
    // from the list and add them to the special structure so that overrides may
    // be applied.
    ForEach(_instrs,i) {
      if (i->second->is_dummy()) {
        dummies.insert(*i);
      }
    }

    ForEach(dummies,i) {
      _instrs.erase(i->first);
    }

    // We expand sub-instructions here, so that re-numbering of pseudo-fields can
    // be done during finalization.
    ForEach(_instrs,i) {
      Instr &instr = *i->second;
      LogPError(ml,instr.expand_subinstructions(*this,ml,generated,dummies));
    }

    // For each of the generated instructions, move over fields from the
    // sub-instruction into the new instruction.
    ForEach(generated,i) {
      i->second->transfer_fields(*this,ml);
    }

    // Finalize pseudo instructions tmpfields to enable prefix bits calculation
    finalize_tmpfields(ml,0,true);
    ForEach(instrfield_aliases,i) {
      LogPError(ml,i->second->finalize_alias(*this,ml));
    }
    
    calc_prefix_bits(); 

    calc_ret_fields();
   
    if( need_encoding() ) {
      encode_instrs(ml);
    }

    finalize_tmpfields(ml,&generated,false);

    // Handle all subinstructions.
    ForEach(_subinstrs,i) {
      LogPError(ml,i->second->finalize(ml,*this));
    }
  
    // Handle all instructions.  Aliases are postponed until the others have been
    // processed.
    gc_list<Instr*> aliases;
    gc_list<Instr*> nested_alias;
    StrList 	  succints;

    // At this point, the generated list contains just sub-instruction expansions,
    // so we'll finalize all of those.
    ForEach(generated,i) {
      LogPError(ml,i->second->finalize(*this,ml));
    }

    // Now finalize the instructions.  The generated list may gain instructions
    // created via nesting.
    ForEach(_instrs,i) {
      if (i->second->is_alias()) {
        aliases.push_back(i->second);
      } else {
        if (i->second->is_succint()) {
          succints.push_back(i->second->name());
        }
        LogPError(ml,i->second->finalize(*this,ml,generated,dummies));
      }
      ForEach(i->second->get_nested(),it) {
        IfFind(dumm,dummies,it->first) {
          if (dumm->second->is_remove()) {
            continue;
          }
        }
        nested_alias.push_back(it->second);
      }
    }
    
    ForEach(nested_alias,i) {
      LogPError(ml,(*i)->finalize(*this,ml,generated,dummies));
    }

    ForEach(succints,i) {
      _instrs.erase(*i);
    }
    // Finally, add all generated instructions to the list of instructions.
    _instrs.insert(generated.begin(),generated.end());
    // Now we able to finalize aliaes
    ForEach(aliases,i) {
      LogPError(ml,(*i)->finalize(*this,ml,generated,dummies));
    }
   
    // Computes mask of every instruction fields, and set id
    postprocess_instrfields();     
   
    // After all instructions are built check that dummies were legal
    ForEach(dummies,i) {
      if (i->second->is_remove()) {
        // Don't bother with error checking on removes- it can get annoying with
        // complex groups and such.
        _instrs.erase(i->first);
      } else if (!get_instr(i->first) && !get_subinstr(i->first)) {
        PError(i->second->loc(),"Attempt to modify instruction '" << i->first  << "' which has not yet been defined.");
      }
    }
    // Erase removed nested instructions.
    gc_list<Instr*>  to_remove;
    ForEach(_instrs,k) {
      InstrHash &instrs = (k->second->_instrs);
      bool removed = false;
      ForEach(dummies,i) {
        if (i->second->is_remove()) {
          if (instrs.erase(i->first)) {
            removed = true;
          }
        }
      }
      // if all nested instructions were removed and the instruction
      // has no opcode it should be removed.
      if (removed && instrs.empty() && k->second->sizeOpcs() == 0) {
        to_remove.push_back(k->second);
      }
    }  
    ForEach(to_remove,i) {
      _instrs.erase((*i)->name());
    }

   
    // Then process split fields in the instructions.  Must be done after
    // initial finalization because we look at an instruction's type and use its
    // fields in order to understand what fields actually exist over prefix
    // fields.
    ForEach(_instrs,i) {
      LogPError(ml,i->second->finalize_split_fields());
    }

    // Then iterate over all of the resulting instructions to set their IDs.
    unsigned id = 0;
    ForEach(_instrs,i) {
      Instr &instr = *i->second;
      
      instr._id = id++;
      _max_micro_ops = max(_max_micro_ops,instr.micro_op_count());
    }

    if (has_micro_op_offset() && !has_micro_ops()) {
      PError(loc(),"No instruction fields in this core have micro-operations, yet setMicroOpOffset() was called in an instruction handler.");
    }
  }

  Core::FieldMicroOpMap Core::getMicroOpFields() const
  {
    FieldMicroOpMap micro_op_tables;
    
    // Collect up micro-op names.
    ForEach(_instrfields,i) {
      const InstrField &field = *i->second;
      if (field.type() == InstrField::tInstr) {
        int id = Data::get_attr(field.instrtablename());        
        micro_op_tables.insert(make_pair(field.name(),
                                         MicroOpInfo{field.instrtablename(),id}));
      }
    }

    return micro_op_tables;
  }
  
  void Core::tagMicroOps()
  {   
    if (has_micro_ops()) {
      // Maps instruction table names to attribute IDs.
      auto micro_op_tables = getMicroOpFields();

      ForEach(_instrs,i) {
        Instr &instr = *i->second;

        ForEach(micro_op_tables,aiter) {
          int id = aiter->second._id;
          if (instr.has_attr(id)) {
            instr.set_is_micro_op();
          }
        }
      }
    }
  }

  // Check all auxiliary functions.
  void Core::check_aux_funcs(MsgLog &ml)
  {
    ForEach(_aux_funcs,i) {
      LogPError(ml,{
          CheckCode cc(_env,*this);
          cc.check_code((*i)->_func);
        });
    }

    // Update usage for any registers used by these aux funcs.
    updateRegUsage();
  }

  Ptree *Core::check_func(UsageInfo &ui,Ptree *func) const
  {
    if (!func) return 0;
    CheckCode cc(_env,*this);
    cc.check_code(func);
    ui = cc.usageinfo();

    // Return the updated and translated function.
    return func;
  }

  void Core::create_asm_excepts(StrSet &excepts,bool pre,bool post)
  {
    if (pre) {
      ForEach(_asmcfg.parms(),i) {
        _prefix_excepts.insert(i->first);
      }
      excepts.insert(_prefix_excepts.begin(),_prefix_excepts.end());
      ForEach(_instrfields,i) {
        if (!i->second->is_prefix()) {
          excepts.insert(i->first);
        }
      }
    }
    if (post) {
      excepts.insert("instrHasAttr");
      excepts.insert("instrBlk");
      excepts.insert(_blocks.begin(),_blocks.end());
      excepts.insert("size");
    }
  }

  // Checks all un-checked helpers.
  void Core::check_helpers(MsgLog &ml)
  {
    // Helper functions used by just the assembler can refer to instruction
    // fields.
    StrSet asm_excepts;
    create_asm_excepts(asm_excepts,true,true);

    // We're done with needing any translated-helper information that was cached.
    _translated_helpers.clear();

    ForEach(_helpers,i) {
      if (!i->second.done()) {
        LogPError(ml,{
            // Now scan it with no arguments.
            const StrSet &excepts = (asm_helpers().count(i->first) ? asm_excepts : _prefix_excepts);
            process_helper(&i->second,i->second,i->first,0,0,excepts,true);
          });
      }
    }
  }

  bool Core::set_mmu_handling(bool h)
  {
    bool o = _mmu_handling;
    _mmu_handling = h;
    return o;
  }

  void Core::add_used_reg(const RegBase *r)
  {
    _used_regs.insert(r);
  }

  void Core::set_has_micro_op_offset()
  {
    _has_micro_op_offset = true;
  }
  
  void Core::add_instr_attr(unsigned attr)
  {
    _instr_attrs.insert(attr);
    // If the MMU flag is set, then we record it as an MMU instruction attribute
    // as well.
    if (_mmu_handling) {
      _mmu_instr_attrs.insert(attr);
    }
  }

  void Core::add_instr_attr(const gc_string &attr)
  {
    int a = Data::get_attr(attr);
    if (a < 0) {
      RError("Unknown attribute:  " << attr);
    }
    add_instr_attr(a);
  }

  void Core::add_regfile_attr(unsigned attr)
  {
    _regfile_attrs.insert(attr);
  }

  void Core::add_regfile_attr(const gc_string &attr)
  {
    int a = Data::get_attr(attr);
    if (a < 0) {
      RError("Unknown attribute:  " << attr);
    }
    add_regfile_attr(a);
  }

  void Core::add_asm_helpers(const StrSet &helpers)
  {
    _asm_helpers.insert(helpers.begin(),helpers.end());          
  }

  void Core::add_disasm_helper(const gc_string &h)
  {
    _disasm_helpers.insert(h);
  }

  // Checks the code in a helper function and updates usage info.  Note that
  // this may be called by external users, such as VarWalker, which calls this
  // in order to handle nested helpers.
  void Core::process_helper(HelperInfo *result,HelperInfo &src,const gc_string &name,
                            PtreeArray *args,const InstrBase *instr,const StrSet &excepts,
                            bool top_level)
  {
    // This guard is here to prevent us from looping infinitely if we encounter
    // a recursive call.
    if (!src.in_process()) {
      // First, see if we've already translated this helper *with these
      // arguments*.  This is important, since we're tracking usage information.
      HelperArgs ha(name,args);
      IfFind(x,_translated_helpers,ha) {
        // Found it, so update the results.
        if (result) {
          result->update(x->second);
          result->_func = x->second._func;
          result->set_done();
        }
        return;
      }
      // Not found, so we have to process it.
      src.set_in_process();
      CheckCode cc(_env,*this);
      // If we have arguments, store them for use in evaluating constant expressions.
      if (args) {
        cc.set_func_call_info(name,args);
      }
      if (instr) {
        cc.set_instr(instr);
      }
      if (top_level) {
        // Other usage should have already recorded the usage of these
        // functions, so we don't bother doing it here.
        cc.set_record_helper_usage(false);
      } else {
        // We'll catch escapes later when we parse the helper by itself.
        cc.set_ignore_escapes(true);
      }
      cc.check_code(src._func,excepts);
      src.set_clear();

      // Cache the results for later use.
      auto ip = _translated_helpers.insert(make_pair(ha,HelperInfo()));
      HelperInfo &hi = ip.first->second;
      hi.update(cc.usageinfo());
      hi._func = cc.translated();

      // Record delayed-write information.
      record_delayed_writes(*this,cc.usageinfo());
      
      // Store any results into the supplied object.
      if (result) {
        result->update(cc.usageinfo());
        result->_func = cc.translated();
        result->set_done();
      }
    }
  }

  struct TablePosSorter2 {
    bool operator()(const ITableEntry *x,const ITableEntry *y) {
      return x->_pos < y->_pos;
    }
  };


  void Core::build_instrtables(MsgLog &ml) {
   
    int pfx_itbl =  Data::get_attr(PrefixInstrTable);
    // Now sort instruction according to instruction table
    ForEach(_instrs,i) {
      Instr *instr = i->second;
      if ((instr->assembler() && !instr->action()) || instr->is_pseudo() || 
          (!instr->get_nested().empty() && instr->getOpcs().empty())) { 
        continue;
      }
      bool added = false;
      ForEach(_instrtables,table) {
        if (instr->has_attr(table->first)) { 
          if (instr->is_prefix() && (int)table->first == pfx_itbl) {
            // To default prefix table we will add later.
            continue;
          }
          table->second._instrs.push_back(instr);
          added = true;
        }
      }
      if (!added) {
        int c = (instr->is_prefix()) ? Data::get_attr(PrefixInstrTable) : Data::get_attr(DefaultInstrTable);
        auto iter = _instrtables.find(c);
        if (iter != _instrtables.end()) {
          iter->second._instrs.push_back(instr);
        } 
      }
    }

    // Set encoding width // after instructions are categorized into the tables
    ForEach(_instrtables,t) {  
      unsigned max_instr_width = 0;
      ForEach(t->second._instrs, instr) {
         unsigned width = (*instr)->width();
         if (max_instr_width < width) max_instr_width = width;
      }
      if (!max_instr_width) max_instr_width = _max_instr_width; // For empty tables, fall back to _max_instr_table, to avoid mask.resize(0) error
      unsigned table_width = ((max_instr_width - 1) / 32 + 1) * 32;  // Round to 32-bit words
      t->second._table->setWidth(table_width); 
      t->second._table->setInstrWidth(max_instr_width);
    }    

    
    ForEach(_instrtables,i) {
      InstrTable *table = i->second._table;
      try {
        table->add_instrs(i->second._instrs,ml);
      }
      catch (decode_error &err) {
        PError(loc(),"Instruction table " << Data::get_attr_name(i->first) << ":  " << err.what());
      }
    }
  }


  bool Core::need_encoding() const
  {
    ForEach(_instrs,i) {
      if (i->second->need_encoding()) {
        return true;
      }
    }
    return false;
  }

  
  void Core::encode_instrs(MsgLog &ml) 
  {
    unsigned num_instrs = _instrs.size();
 
    _max_prefix_bits = 0;

    // Count nested instructions, which  are not in the _instrs hash, yet.
    ForEach(_instrs,i) {
      num_instrs += (i->second->get_nested().size());
    }
    
    unsigned instr_width = 0;
    // Number of bits required for opcode    
    unsigned opcd_size = log2(num_instrs+1) + 1 ;
    // find maximum insturction width.
    ForEach(_instrs,i) {
      unsigned width = i->second->fields_width(*this);
      instr_width = max(instr_width,width);
      ForEach(i->second->get_nested(),n) {
        width = n->second->fields_width(*this);
        instr_width = max(instr_width,width);
      }
    }

    instr_width += opcd_size;
    if (instr_width%8) {
      instr_width = (instr_width/8 + 1)*8;
    }

    // opcode and maximal number of required bits are updated
    // during encoding
    unsigned opcd = 1;
    ForEach(_instrs,i) {
      i->second->encode(*this,opcd,opcd_size,instr_width);
     
      ForEach(i->second->get_nested(),n) {
        n->second->_nested = true;
        n->second->encode(*this,opcd,opcd_size,instr_width);
      }
    }
  }

  void Core::calc_ret_fields() 
  {
    ForEach(_instrfields,i) {
      const InstrField &f = *(i->second);
      int  cnt = 0;
      ForEach(f.instrfields(),k) {
        const InstrField &n = *(k->second);
        if (n.bits().empty()) {
          ++cnt;
        }
      }
      _num_ret_fields = max(cnt,_num_ret_fields);
    }
  }

  void Core::calc_prefix_bits() {
    // find the maximum number of prefix bits added to an instruction
    ForEach(_instrs,i) {
      if (!i->second->is_pseudo()) {
        continue;
      }

      unsigned prefix_bits = 0;
      ForEach(i->second->_init_all,j) {
        if (InstrBase::Field *field = InstrBase::get_field(*j)) {
          const InstrField *f = get_instrfield(field->_name);
          if (f && f->is_prefix()) {
            prefix_bits += f->width();
          } 
        }
      }
      _max_prefix_bits = max(prefix_bits, _max_prefix_bits);
    } 
  }

  void Core::check_prefix() 
  {
    bool has_prefix = false;
    ForEach(_instrs,i) {
      if (i->second->is_prefix()) {
        has_prefix = true;
        break;
      }
    }

    if (!has_prefix) {
      return;
    }
    // check all instructions use prefix
    if (_max_prefix_bits) {
      ForEach(_instrs,i) {
        const Instr *instr = i->second;
        if (instr->has_variable_field() || instr->is_prefix() || instr->is_pseudo() || instr->get_type() || instr->is_abstract()) {
          continue;
        }
        // This is a typeless instruction - is it fully encoded in prefix
        ForEach(instr->getFields(),f) {
          if (f->_field && !f->_field->is_prefix()) {
            PError(loc(),"Instruction '" << instr->name() << " 'does not use prefix information and is not prefix encoded.");
          }
        }
      }
    }

    int c = Data::get_attr(PrefixInstrTable);
    auto iter = _instrtables.find(c);
    if (iter == _instrtables.end()) {
      PError(loc(),"Prefix instruction table undefined although prefix instructions are used.");
    } else if (iter->second._pos !=0) {
      //PError(loc(),"Prefix instruction table should be the default table.");
    }


  }

  // Check that all implementations of a field marked 'assembler'
  // are either within the fetch set or are split between fetch and prefix
  void Core::check_assembler_fields() 
  {
    gc_multimap<const InstrField*,int>  split_fields; // fields split between fetch and prefix
    gc_set<const InstrField*> contig_fields;

    if (!_max_prefix_bits) {
      return;
    }

    ForEach(_instrs,i) {
      const Instr &instr = *(i->second);
      ForEach(instr.getFields(),j) {
        const InstrField *f = j->_field;
        if (f && f->is_assembler()) {
          if (f->highbit() > instr.fetch_width()) {
            int over = 0;
            ForEach(f->bits(),b) {
              if (b->max() >=  instr.fetch_width()) {
                over += (b->max() -  instr.fetch_width() + 1);
              }
            }
            split_fields.insert(make_pair(f,over));
          } else {
            contig_fields.insert(f);
          }
        }
      }
    }

    int   val = 0;
    const InstrField *f = 0;
    ForEach(split_fields,i) {
      if (f == i->first) {
        if (i->second != val) {
          PError(loc(),"Assembler field " << f->name() << " has more than one parition width.");
        }
      } else {
        f = i->first;
        val = i->second;
      }
    }

    ForEach(split_fields,i) {
      if (contig_fields.find(i->first) != contig_fields.end()) {
        PError(loc(),"Assembler field " << i->first->name() << " is both split and not."); 
      }
    }
  }


  // Pseudo instructions tmpfields are finalized to compute prefix_bits.  This is
  // required in the encoding-less flow.  The rest's tmpfields are finalized.
  // Note: Sub-instructions do not have their tmpfields finalized b/c we want to
  // allow their numbering to be relative to other fields in the final
  // instruction.
  void Core::finalize_tmpfields(MsgLog &ml,InstrHash *generated,bool pseudo) 
  {
    InstrFieldHash tmpfields;

    ForEach(_instrs,i) {
      if (i->second->is_pseudo() == pseudo) {
        LogPError(ml,i->second->finalize_tmpfields(*this,tmpfields));
      }
    }

    if (generated) {
      ForEach((*generated),i) {
        LogPError(ml,i->second->finalize_tmpfields(*this,tmpfields));
      }
    }

    InstrField::InstrFieldMMap dummy;
    ForEach(tmpfields,i) { // Tmpfields do not have nested fields
      InstrField *ifield = i->second;
      if (ifield->is_variable()) {
        ifield->clear_variable();
        LogPError(ml,ifield->finalize(*this,ml,dummy));
        _instrfields.insert(make_pair(ifield->name(),ifield));
      } else {
        gc_string      outer = ifield->name();
        LogPError(ml,ifield->finalize(*this,ml,_instrfields,outer));
      }
    }
  }

  bool Core::has_instr_memory() const 
  {
    return _has_instr_memory;
  }

  bool Core::has_prefetch_buffer() const
  {
    return _has_prefetch_memory;
  }


  // Setup the caches.
  void Core::finalize_caches(MsgLog &ml)
  {
    ForEach(_caches,i) {
      LogPError(ml,{
          Cache *c = i->second;
          if (!c->_level) {
            PError(c->loc(),"No level set for cache.");
          }
          add_cache(c);
        });
    }
    check_cache_hierarchy(ml,_instr_caches);
    check_cache_hierarchy(ml,_data_caches);
  }

  // Compute maximum instruction width, compute mask for every field.
  void Core::postprocess_instrfields() {
    // Default is one word (32 bit).
    _max_instr_width = 32;
    // Find maximum.
    ForEach(_instrs,i) {
      _max_instr_width = i->second->_width > _max_instr_width ? i->second->_width : _max_instr_width ;
    }
    // Round to words.  
    _max_instr_width = 32*((_max_instr_width - 1) / 32 + 1);
    // Now we are able to compute mask.
    ForEach(_instrfields,i) {
      i->second->compute_mask(_max_instr_width);
    }   
    // set fields' id
    int id = 0;
    ForEach(_instrfields,i) { 
      i->second->set_id(id++);
    }
  }

  void Core::check_cache_hierarchy(MsgLog &ml,Caches &caches)
  {
    bool unified = false;
    Cache *last = 0;
    unsigned id = 0;
    ForEach(caches,i) {
      if (*i) {
        Cache &cache = **i;
        LogPError(ml,{
            cache.finalize(*this,ml,id++);
            if (cache.unified()) {
              unified = true;
            } else {
              if (unified) {
                PError(cache.loc(),cache.name() << " cache is split but lower-level caches are already unified.");  
              }
            }
            if (last) {
              last->_next = &cache;
            }
            last = &cache;
          });
      }
    }
  }         

  // bool Core::add_instr_table(const gc_string &name,unsigned pos)
  // Refactor with one more parameter as width
  // No overloading since add_instr_table() is only used in one occasion in Handlers.C
  bool Core::add_instr_table(const gc_string &name, unsigned pos, unsigned width) 
  {
    int c = Data::get_attr(name);
    if (c < 0) {
      return false;
    }
    if (!_instrtables.count(c)) {
        InstrTable * pInstrTable = new InstrTable(width); // width default to 0 if no width argument
        _instrtables.insert(make_pair(c, ITableEntry(c, pInstrTable, pos))); 
		_instr_tables.insert(make_pair(name, pInstrTable));	//
	  
    }
    return true;
  }      
  
  Parm *Core::get_parm(const gc_string &name)
  {
    ParmHash::iterator i = _parms.find(name);
    return (i == _parms.end()) ? 0 : i->second;
  }

  Context *Core::get_ctx(const gc_string &name)
  {
    ContextHash::iterator i = _contexts.find(name);
    return (i == _contexts.end()) ? 0 : i->second;
  }

  RegBase *Core::get_regitem(const gc_string &name)
  {
    if (auto rf = get_regfile(name)) {
      return rf;
    }
    else if (auto r = get_reg(name)) {
      return r;
    } else {
      return 0;
    }
  }

  RegFile *Core::get_regfile(const gc_string &name)
  {
    RegFileHash::iterator i = _regfiles.find(name);
    return (i == _regfiles.end()) ? 0 : i->second;
  }

  Reg *Core::get_reg(const gc_string &name)
  {
    RegHash::iterator i = _regs.find(name);
    return (i == _regs.end()) ? 0 : i->second;
  }

  InstrField *Core::get_instrfield(const gc_string &name)
  {
    InstrFieldHash::iterator i = _instrfields.find(name);
    return (i == _instrfields.end()) ? 0 : i->second;
  }

  Relocation *Core::get_reloc(const gc_string &name)
  {
    RelocationHash::iterator i = _relocs.find(name);
    return (i == _relocs.end()) ? 0 : i->second;
  }

  PrefixCounter *Core::get_pfx_ctr(const gc_string &name)
  {
    PrefixCounterHash::iterator i = _prefix_counters.find(name);
    return (i == _prefix_counters.end()) ? 0 : i->second;
  }

  SubInstr *Core::get_subinstr(const gc_string &name)
  {
    SubInstrHash::iterator i = _subinstrs.find(name);
    return (i == _subinstrs.end()) ? 0 : i->second;
  }


  Instr *Core::get_instr(const gc_string &name)
  {
    InstrHash::iterator i = _instrs.find(name);
    return (i == _instrs.end()) ? 0 : i->second;
  }

  Exception *Core::get_exception(const gc_string &name)
  {
    ExceptionHash::iterator i = _exceptions.find(name);
    return (i == _exceptions.end()) ? 0 : i->second;
  }

  Cache *Core::get_cache(const gc_string &name)
  {
    CacheHash::iterator i = _caches.find(name);
    return (i == _caches.end()) ? 0 : i->second;
  }

  Memory *Core::get_memory(const gc_string &name) 
  {
    MemoryHash::iterator i = _memories.find(name);
    return (i == _memories.end()) ? 0 : i->second;
  }

  InstrTable *Core::get_instrtable(const gc_string &name) //
  {
    InstrTableHash::iterator i = _instr_tables.find(name);
    return (i == _instr_tables.end()) ? 0 : i->second;
  }  
  
  EventBus *Core::get_eventbus(const gc_string &name) 
  {
    EventBusHash::iterator i = _eventbuses.find(name);
    return (i == _eventbuses.end()) ? 0 : i->second;
  }

  ExtResource *Core::get_extresource(const gc_string &name) 
  {
    ExtResourceHash::iterator i = _extresources.find(name);
    return (i == _extresources.end()) ? 0 : i->second;
  }

  HelperInfo *Core::get_helper(const gc_string &name) const
  {
    HelperMap::iterator i = _helpers.find(name);
    return (i == _helpers.end()) ? 0 : &(i->second);
  }

  const MmuLookup *Core::get_mmulookup(const gc_string &name) const
  {
    if (_mmu) {
      return const_cast<const MMU*>(_mmu)->get_top_mmulookup(name);
    } else {
      return 0;
    }
  }

  bool Core::is_block(const gc_string &name) const 
  {
    return _blocks.find(name) != _blocks.end();
  }

  bool Core::get_regbase_enum(const gc_string &name, int &val) const 
  {
    size_t pos = name.find_first_of(":");
    if (pos == gc_string::npos || name.substr(pos,2) != "::") {
      return false;
    }
    const RegBase *rb = 0 ;
    if ((rb = get_reg(name.substr(0,pos-1))) || (rb = get_regfile(name.substr(0,pos-1)))) {
      return rb->get_enumerated(name.substr(pos+3,name.size()-pos-2),val);
    }
    return false;
  }

  bool Core::has_prefix_instruction() const
  {
    ForEach(_instrs,i) {
      if (i->second->is_prefix()) {
        return true;
      }   
    }
    return false;
  };

  const RegField *check_regfield_suffix(unsigned pfxlen,const gc_string &name,const RegBase &r)
  {
    // We found a matching register prefix, so we then expect a '_' followed by a field name.
    unsigned fo = pfxlen+r.name().size();
    if ( name.at(fo) != '_' ) {
      return 0;
    }
    // Finally, try to find a register-field match.
    ++fo;
    ForEach(r.fields(),i) {
      const RegField &rf = *i->second;
      gc_string sfx = name.substr(fo);
      if (sfx == rf.name()) {
        return &rf;
      }
    }
    return 0;
  }

  // Returns a pointer to a register field object if 'name' matches a
  // register/register-file getter/setter helper function.  These have the form
  // "getfield_<regname>_<fieldname>" or "setfield...".
  const RegField *Core::is_regfield_helper(const gc_string &name) const
  {
    // Hack- we're assuming that both prefixes are of the same length.
    unsigned pfxlen = strlen(RegFieldGetterPfx);
    // Minimum size must be one char for the register + _ + one char for field.
    if (name.size() < pfxlen+3) {
      return 0;
    }
    if (name.compare(0,pfxlen,RegFieldGetterPfx)  == 0|| name.compare(0,pfxlen,RegFieldSetterPfx) == 0) {
      // Now try and match the next portion to the name of a register or register-file.
      ForEach (regs(),i) {
        const Reg &reg = *i->second;
        if ( name.compare(pfxlen,reg.name().size(),reg.name()) == 0) {
          if (const RegField *rf = check_regfield_suffix(pfxlen,name,reg)) {
            return rf;
          }
        }
      }
      ForEach (regfiles(),i) {
        const RegFile &reg = *i->second;
        if ( name.compare(pfxlen,reg.name().size(),reg.name()) == 0) {
          if (const RegField *rf = check_regfield_suffix(pfxlen,name,reg)) {
            return rf;
          }
        }
      }
    }
    return 0;
  }

  void Core::add_local_resource(const gc_string &name)
  {
    _local_resources.insert(name);
  }
  
  bool Core::is_resource(const gc_string &name) const
  {
    int  dummy;
    if (globalResource(name)) {
      return true;
    }
    else if (_local_resources.count(name)) {
      return true;
    }
    else if (Data::get_attr(name) >= 0) {
      // It's an attribute.
      return true;
    }
    else if (get_parm(name)) {
      // Is it a parameter?
      return true;
    }
    else if (_alloptions.count(name)) {
      // Is it a parameter option?
      return true;
    }
    else if (get_reg(name)) {
      // Is it a register?
      return true;
    }
    else if (get_regfile(name)) {
      // Is it a register file?
      return true;
    }
    else if (get_pfx_ctr(name)) {
      // Is it a prefix counter?
      return true;
    }
    else if (get_regbase_enum(name,dummy)) {
      // Is it a register enumerated value ?
      return true;
    }
    else if (get_exception(name)) {
      // Is it an exception?
      return true;
    }
    else if (_mmu && (const_cast<const MMU*>(_mmu)->get_any_mmulookup(name) || _mmu->is_lookup_enum(name))) {
      // Is it an MMU lookup (used for writing/reading the MMU arrays) or an enum representing the lookup's id?
      return true;
    }
    else if (get_cache(name)) {
      // Is it a cache?
      return true;
    }
    else if (get_memory(name)) {
      // Is it a memory?
      return true;
    }
    else if (get_eventbus(name)) {
      // Is it an event bus?
      return true;
    }
    else if (get_extresource(name)) {
      // Is it an opaque, external resource?
      return true;
    }
    else if (get_ctx(name)) {
      // Is it a context?
      return true;
    }
    else if ((Data::get_attr(name) > 0) && (_instrtables.size() > 1)) {
      // Is it an instruction class name?  If so, the use of those
      // is limited to when we have more than one instruction table.
      return true;
    }
    else if (is_regfield_helper(name)) {
      // Is this a register field helper, e.g. setfield_foo_x, etc.?
      return true;
    } else {
      return false;
    }
  }

  bool Core::is_prefix_info(const gc_string &name) const 
  {
    const InstrField *f = get_instrfield(name);
    if (f && f->is_prefix() && f->is_pseudo() && !f->index()&& f->blks().empty()) {
      return true;
    }
    return false;
  }

  Cache *Core::get_cache(Caches &caches,unsigned level)
  {
    unsigned ci = level - 1;
    return (ci < caches.size()) ? caches[ci] : 0;
  }

  Cache *Core::get_first_cache(Caches &caches)
  {
    ForEach(caches,i) {
      if (*i) {
        return *i;
      }
    }
    return 0;
  }

  Cache *Core::get_last_cache(Caches &caches)
  {
    RevForEach(caches,i) {
      if (*i) {
        return *i;
      }
    }
    return 0;
  }

  // The assumption we make here is that if the cache is unified, then
  // the *same* object is used in both arrays.
  Cache *Core::get_cache(CacheType type,unsigned level)
  {
    if (type == InstrCache || type == UnifiedCache) {
      return get_cache(_instr_caches,level);
    } else {
      return get_cache(_data_caches,level);
    }
  }

  Cache *Core::get_first_cache(CacheType type)
  {
    if (type == InstrCache || type == UnifiedCache) {
      return get_first_cache(_instr_caches);
    } else {
      return get_first_cache(_data_caches);
    }
  }

  Cache *Core::get_last_cache(CacheType type)
  {
    if (type == InstrCache || type == UnifiedCache) {
      return get_last_cache(_instr_caches);
    } else {
      return get_last_cache(_data_caches);
    }
  }

  void Core::add_cache(Caches &caches,Cache *c)
  {
    assert(c && c->level() > 0);
    unsigned ci = c->level() - 1;
    if (ci >= caches.size()) {
      caches.resize(ci+1);
    }
    caches[ci] = c;
  }

  void Core::add_cache(Cache *c)
  {
    assert(c);
    if (c->type() == InstrCache || c->type() == UnifiedCache) {
      add_cache(_instr_caches,c);
    }
    if (c->type() == DataCache || c->type() == UnifiedCache) {
      add_cache(_data_caches,c);
    }
  }

  bool Core::has_caches() const
  {
    return has_instr_caches() || has_data_caches();
  }

  bool Core::has_instr_caches() const
  {
    if (_instr_caches.empty()) {
      return false;
    }

    ForEach(_instr_caches,i) {
      if (*i) {
        return true;
      }
    }

    return false;
  }

  bool Core::has_data_caches() const
  {
    if (_data_caches.empty()) {
      return false;
    }

    ForEach(_data_caches,i) {
      if (*i) {
        return true;
      }
    }

    return false;
  }

  bool Core::has_caches(CacheType type) const
  {
    switch (type) {
    case InstrCache:
      return has_instr_caches();
    case DataCache:
      return has_data_caches();
    default:
      RError("Bad CacheType value.");
    }
  }

  unsigned Core::max_cache_level() const
  {
    return max(_instr_caches.size(),_data_caches.size());
  }

  template <class T>
  const Context *is_ctx_resource(const ContextHash &contexts,const T &res)
  {
    ForEach(contexts,i) {
      if ( i->second->contains(res)) {
        return i->second;
      }
    }  
    return 0;
  }

  const Context *Core::is_ctx_resource(const Reg &reg) const
  {
    return adl::is_ctx_resource(_contexts,reg);
  }

  const Context *Core::is_ctx_resource(const RegFile &rf) const
  {
    return adl::is_ctx_resource(_contexts,rf);
  }

  const Context *Core::is_ctx_resource(const RegBase &r) const
  {
    return adl::is_ctx_resource(_contexts,r);
  }

  const Context *Core::is_ctx_resource(const MmuLookup &l) const
  {
    return adl::is_ctx_resource(_contexts,l);
  }

  struct TablePosSorter {
    bool operator()(const ITableEntry *x,const ITableEntry *y) {
      return x->_pos < y->_pos;
    }
  };

  void Core::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  \"" << _name << "\"\n";
    if (!_type.empty()) {
      o << pfx << "Type:  " << _type << "\n";
    }

    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    if (_bitEndianness == Little) {
      o << pfx << "Bit Order:              little endian\n";
    }
    if (_instrEndianness == Little) {
      o << pfx << "Instruction Byte Order: little endian\n";
    }
    if (_dataEndianness == Little) {
      o << pfx << "Data Byte Order:        little endian\n";
    }

    if (!_parms.empty()) {
      o << pfx << "Architectural Parameters:\n";
      printHashOrdered(o,pfx1,_parms);
    }
    if (!_blocks.empty()) {
      o << pfx << "Blocks:\n" << pfx;
      printSequence(o,_blocks,true);
    }
    if (!_regfiles.empty()) {
      o << pfx << "Register Files:\n";
      printHashOrdered(o,pfx1,_regfiles);
    }
    if (!_exceptions.empty()) {
      o << pfx << "Exceptions:\n";
      printHashOrdered(o,pfx1,_exceptions);
    }
    if (!_regs.empty()) {
      o << pfx << "Registers:\n";
      printHashOrdered(o,pfx1,_regs);
    }
    if (!_contexts.empty()) {
      o << pfx << "Contexts:\n";
      printHashOrdered(o,pfx1,_contexts);
    }

    if (_parallel_execution > 0) {
      o << pfx << "Parallel execution packet size: " << _parallel_execution << "\n";
    }

    if (_packet_pos_incr > 0) {
      o << pfx << "Packet position increment:  " << _packet_pos_incr << "\n";
    }

    o << pfx << "Current-instruction-address register:  " << _cia->name() << "\n"
      << pfx << "Next-instruction-address register:  " << _nia->name() << "\n";

    o << pfx << "Real-address Mask:\n";
    _ra_mask.dump(o,pfx1);

    o << pfx << "Effective-address Mask:\n";
    _ea_mask.dump(o,pfx1);

    if (!_delayed_writes.empty()) {
      o << pfx << "Delayed Write Targets:  ";
      printSequence(o,_delayed_writes,true);
    }

    if (!_concurrent_writes.empty()) {
      o << pfx << "Concurrent Write Targets:  ";
      printSequence(o,_concurrent_writes,true);
    }

    if (_mmu) {
      _mmu->dump(o,pfx);
    }

    dump_caches(o,pfx);

    if( !_memories.empty() ) {
      o << pfx << "Memories:\n";
      printHashOrdered(o,pfx1,_memories);
    }

    if( !_eventbuses.empty() ) {
      o << pfx << "Event buses:\n";
      printHashOrdered(o,pfx1,_eventbuses);
    }

    if( !_extresources.empty() ) {
      o << pfx << "External resources:\n";
      printHashOrdered(o,pfx1,_extresources);
    }

    if (!_relocs.empty()) {
      o << pfx << "Relocations:\n";
      printHashOrdered(o,pfx1,_relocs);
    }

    if (!_prefix_counters.empty()) {
      o << pfx << "Prefix Counters:\n";
      printHashOrdered(o,pfx1,_prefix_counters);
    }


    if (!_instrfields.empty()) {
      o << pfx << "Instruction Fields:\n";
      ForEach(_instrfields,i) {
        if (!(i->second->encap_field())) {
          i->second->dump(o,pfx1);
        }
      }
    }

    if (!_subinstrs.empty()) {
      o << pfx << "Subinstructions:\n";
      printHashOrdered(o,pfx,_subinstrs);
    }
    if (!_instrs.empty()) {
      o << pfx << "Instructions:\n";
      printHashOrdered(o,pfx1,_instrs);
    }
    if (!_instrtables.empty()) {
      gc_vector<const ITableEntry *> tables;
      ForEach(_instrtables,i) {
        tables.push_back(&(i->second));
      }
      sort(tables.begin(),tables.end(),TablePosSorter());
      o << pfx << "\nInstruction Tables:\n";
      ForEach(tables,i) {
        const ITableEntry &entry = **i;
        o << pfx << Data::get_attr_name(entry._id);
        if (entry._table->is_prefix()) {
          o << "(prefix)";	  
        }
        o << ":\n";
        o << pfx1;
        entry._table->dump(o,pfx1);
      }
    }

    if (!_instr_attrs.empty()) {
      o << pfx << "\nInstruction Attributes:  ";
      ForEach(_instr_attrs,i) {
        o << Data::get_attr_name(*i);
        if (_mmu_instr_attrs.count(*i)) {
          o << "(mmu)";
        }
        o << " ";
      }
      o << "\n";
    }
    if (!_regfile_attrs.empty()) {
      o << pfx << "\nRegister-file Attributes:  ";
      ForEach(_regfile_attrs,i) {
        o << Data::get_attr_name(*i) << " ";
      }
      o << "\n";
    }

    if (_decode_miss_handler) {
      o << pfx << "\nDecode-Miss Handler:  ";
      _decode_miss_handler.print(o,pfx1);
    }
    if (!_exception_is_branch) {
      o << pfx << "Exception is not a branch.\n\n";
    }
    if (_branch_taken_handler) {
      o << pfx << "\nBranch-taken Handler:  ";
      _branch_taken_handler.print(o,pfx1);
    }
    if (_addr_check_handler) {
      o << pfx << "\nAddress-check Handler:  ";
      _addr_check_handler.print(o,pfx1);
    }
    if (_pre_exception_handler) {
      o << pfx << "\nPre-exception Handler:  ";
      _pre_exception_handler.print(o,pfx1);
    }
    if (_post_exception_handler) {
      o << pfx << "\nPost-exception Handler:  ";
      _post_exception_handler.print(o,pfx1);
    }
    if (_pre_cycle_handler) {
      o << pfx << "\nPre-Cycle Handler:  ";
      _pre_cycle_handler.print(o,pfx1);
    }
    if (_pre_fetch_handler) {
      o << pfx << "\nPre-Fetch Handler:  ";
      _pre_fetch_handler.print(o,pfx1);
    }
    if (_post_fetch_handler) {
      o << pfx << "\nPost-Fetch Handler:  ";
      _post_fetch_handler.print(o,pfx1);
    }
    if (_post_exec_handler) {
      o << pfx << "\nPost-Exec Handler:  ";
      _post_exec_handler.print(o,pfx1);
    }
    if (_post_cycle_handler) {
      o << pfx << "\nPost-Cycle Handler:  ";
      _post_cycle_handler.print(o,pfx1);
    }
    if (_post_asm_handler) {
      o << pfx << "\nPost-Asm Handler:  ";
      _post_asm_handler.print(o,pfx1);
    }
    if (_post_packet_asm_handler) {
      o << pfx << "\nPost-Packet-Asm Handler:  ";
      _post_packet_asm_handler.print(o,pfx1);
    }
    if (_post_packet_handler) {
      o << pfx << "\nPost-Packet Handler:  ";
      _post_packet_handler.print(o,pfx1);
    }
    if (_post_packet_commit_handler) {
      o << pfx << "\nPost-Packet-Commit Handler:  ";
      _post_packet_commit_handler.print(o,pfx1);
    }
    if (_invalid_entry_read) {
      o << pfx << "\nGlobal sparse register-file invalid-entry-read:  ";
      _invalid_entry_read.print(o,pfx1);
    }
    if (_invalid_entry_write) {
      o << pfx << "\nGlobal sparse register-file invalid-entry-write:  ";
      _invalid_entry_write.print(o,pfx1);
    }
    if (_active_watch) {
      o << pfx << "\nCore Activity Function:\n"
        << _active_watch << "\n";
    }
    if (_itable_watch) {
      o << pfx << "\nCore Instruction-table Watch Function:\n"
        << _itable_watch << "\n";
    }
    if (!_helpers.empty()) {
      o << pfx << "\nHelper Functions:";
      ForEach(_helpers,i) {
        o << '\n' << pfx1 << i->first << ":\n";
        i->second.print(o,pfx2);
      }
    }
    if (!_type_decls.empty()) {
      o << pfx << "\nUser Type Declarations:\n";
      ForEach(_type_decls,i) {
        o << pfx1 << *i << "\n";
      }
    }

    Aspects::dump(o,pfx);

    dump_doc(o,pfx,true);

    if (!_groups.empty()) {
      o << pfx << "\nGroups:\n";
      printHashOrdered(o,pfx1,_groups);
    }

    _asmcfg.dump(o,pfx);

    o << pfx << "-------------------------------\n";

  }

  void Core::dump_caches(ostream &o,gc_string pfx) const
  {
    if (has_caches()) {
      o << pfx << "Caches:\n";
      pfx = indent(pfx);
      ForRange(max_cache_level(),i) {
        const Cache *ic, *dc;
        if ( (ic = get_cache(InstrCache,i+1)) ) {
          ic->dump(o,pfx);
        }
        if ((dc = get_cache(DataCache,i+1)) && (ic != dc)) {
          dc->dump(o,pfx);
        }
      }
    }
  }

  Context::Context(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc), Named(name),
    _id(0),
    _num_contexts(1),
    _is_thread(false),
    _active(0)
  {
  }

  void Context::add_reg(const gc_string &r,const Core *c)
  {
    _regs.insert(make_pair(r,c));
  }

  void Context::add_regfile(const gc_string &rf,const Core *c)
  {
    _regfiles.insert(make_pair(rf,c));
  }

  void Context::add_mmulookup(const gc_string &ml,const Core *c)
  {
    _mmu.insert(make_pair(ml,c));
  }

  void Context::add_mem_layout_item(const gc_string &n)
  {
    _mem_layout.push_back(n);
  }

  void Context::add_mem_omit_item(const gc_string &n)
  {
    _mem_omit.insert(n);
  }

  template <class T>
  void Context::check_items(MsgLog &ml,const ResItems &items,const gc_string &descr,const Core &core,
                            const T * (Core::*get)(const gc_string &) const) const
  {
    ForEach(items,i) {
      if (! (core.*get)(i->first) ) {
        AddError(ml,loc(),"Unknown " << descr << " " << i->first);
      }
    }
  }

  void Context::finalize(Core &core,MsgLog &ml,unsigned id)
  {
    _id = id;

    Attrs::finalize();

    if (_num_contexts == 0) {
      AddError(ml,loc(),"The number of contexts must be greater than 0.");
    }

    check_items<Reg>(ml,_regs,"register",core,&Core::get_reg);

    check_items<RegFile>(ml,_regfiles,"register file",core,&Core::get_regfile);

    check_items<MmuLookup>(ml,_mmu,"MMU lookup",core,&Core::get_mmulookup);

    StrSet res;
    ForEach(_regs,i) res.insert(i->first);
    ForEach(_regfiles,i) res.insert(i->first);

    // Each MMU lookup mentioned must have serialization routines.
    ForEach(_mmu,i) {
      const gc_string &ln = i->first;
      res.insert(ln);
      const MmuLookup *l = core.get_mmulookup(ln);
      // We'll already have marked this as an error if the lookup doesn't exist,
      // so skip here to avoid segfault.
      if (!l) continue;
      if (!l->mem_read() || !l->mem_write()) {
        AddError(ml,loc(),"MMU lookup " << ln << " in context " << name() << " lacks memory serialization routines.");
      }
    }

    // Check layout and omit items to make sure that they are valid context resources.
    ForEach(_mem_omit,i) {
      if (!res.count(*i)) {
        AddError(ml,loc(),"Unknown resource in memory omit list:  " << *i);
      }
    }
    ForEach(_mem_layout,i) {
      if (!res.count(*i)) {
        AddError(ml,loc(),"Unknown resource in memory layout list:  " << *i);
      }
    }

    // If a layout list is specified, then it must cover all items except those
    // explicitly specified in the omit list.
    if (!_mem_layout.empty()) {
      ForEach(_mem_omit,i) {
        res.erase(*i);
      }
      ForEach(_mem_layout,i) {
        res.erase(*i);
      }
      if (!res.empty()) {
        AddError(ml,loc(),"Not all resources were covered by the mem_layout and mem_omit lists:  " << res);
      }
    }

    // Do we have the program counter?  If so, then we consider ourselves to be a thread.
    _is_thread = (contains(*(core.cia())));

    CheckCode cc(_env,core);
    cc.check_code(_active);
  }

  void Context::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  " << name();
    if (is_thread()) {
      o << " (thread)";
    }
    o << "\n";
    o << pfx << "Number contexts:  " << _num_contexts << "\n";
    Attrs::dump(o,pfx);
    Resources::dump(o,pfx);
    if (!_mem_layout.empty()) {
      o << pfx << "Memory layout:  " << _mem_layout << "\n";
    }
    if (_active) {
      o << pfx << "Context Activity Function:  " << _active << "\n";
    }

    dump_doc(o,pfx,true);

    o << pfx << "-------------------------------\n";    
  }

  ostream &operator<<(ostream &o,const CtxInstance &ctx)
  {
    if (ctx) {
      o << ctx._context->name() << "(" << ctx._index << ").";
    }
    return o;
  }

  AddrMask::AddrMask(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc), Named(name)
  {
    clear();
  }

  void AddrMask::clear()
  {
    _constant = true;
    _initial = 0;
    _watch = 0;
  }

  void AddrMask::finalize(Core &core)
  {
    Attrs::finalize();

    if (const Reg *cia = core.cia()) {
      // If no address masks were set, then we default to using the
      // same size as the current-instruction-address register.
      if (!_initial) {
        _initial = ( ( cia->width() == AddrBits) ? 
                     (addr_t)(-1) : 
                     (((addr_t)1) << cia->width()) - 1  );
      }
    }

    if (_watch) {
      _constant = false;
    }

    // The watch expression should not modify any registers- just its own things.
    CheckCode cc(_env,core,false,false);
    cc.check_code_filter(_watch);
    ForEach(cc.escapes(),i) {
      if (i->second != name()) {
        // We allow the mask name.
        PError(i->first,"Unknown variable/resource found in watch function:  " << i->first);
      }
    }
    _watch = cc.translated();
  }

  void AddrMask::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Initial mask:  0x" << hex << _initial << dec;
    if (_constant) {
      o << " (constant)";
    } else {
      o << " (non-constant)";
    }
    o << "\n";
    if (_watch) {
      o << pfx << "Watch Expr:  " << _watch << "\n";
    }
    Attrs::dump(o,pfx);
  }


  Parm::Parm(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _constant(false),
    _watch(0)
  {
  }

  bool Parm::is_bool() const
  {
    return (_options.size() == 2 && _options.count("true") && _options.count("false"));
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a register.
  bool Parm::operator==(const Parm &x) const
  {
    return (_options == x._options) && (_constant == x._constant);
  }

  void Parm::finalize(Core &core,MsgLog &ml)
  {
    Attrs::finalize();

    // Every parm must have a value or else it's an error.
    LogPError(ml,{
        if (!has_value()) {
          PError(loc(),"No default value was specified for parameter '" << name() << "'.");
        }
        if (_options.empty()) {
          PError(loc(),"Parameter '" << name() << "' has no options.");
        }
        if (!_options.count(_value)) {
          PError(loc(),"'" << _value << "' is not a valid option for parameter '" << name() << "'.");
        }
      });

    // Special case:  We may either have the options "true" and "false" or else these options
    // may not be used at all.
    LogPError(ml,{
        if (!is_bool()) {
          if (_options.count("false")) {
            PError(loc(),"Parameter '" << name() << "' may only have the option 'false' if it is strictly a boolean.");
          }
          if (_options.count("true")) {
            PError(loc(),"Parameter '" << name() << "' may only have the option 'true' if it is strictly a boolean.");
          }
        }
      });

    LogPError(ml,{
        CheckCode cc(_env,core,false,false);
        cc.check_code(_watch);
      });
  }

  void Parm::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  " << name() << "\n"
      << pfx << "Options:  ";
    printSequence(o,_options,true);
    o << pfx << "Value:  " << _value << "\n"
      << pfx << "Const:  " << _constant << "\n"
      << pfx << "Boolean:  " << is_bool() << "\n";
    Attrs::dump(o,pfx);
    if (_watch) {
      o << pfx << "Watch Expr:  " << _watch << "\n";
    }
    dump_doc(o,pfx,true);
    o << pfx << "-------------------------------\n";    
  }

  //
  // Information common to register files and registers.
  //
  RegBase::RegBase(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _id(0),
    _width(0),
    _offset(0),
    _pseudo(false),
    _serial(false),
    _resettype(None),
    _resetvalue(0),
    _resetaction(0),
    _external_write(0),
    _usage_flags(0)
  {
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a register.
  bool RegBase::operator==(const RegBase &x) const
  {
    if (width() != x.width()) {
      return false;
    }

    if (reserved_mask() != x.reserved_mask()) {
      return false;
    }

    if (readonly_mask() != x.readonly_mask()) {
      return false;
    }

    return true;
  }

  void RegBase::check_sharing_okay() const
  {
    if (pseudo()) {
      PError(loc(),"Pseudo/aliased registers may not be marked as shared.");
    }
  }

  bool RegBase::branch_target(const Core& core) const {
    if (core.nia() == this) {
      return true;
    }
    // FIXME
    int bt_attr = Data::get_attr(gc_string("branch_target"));
    if (has_attr(bt_attr)) {
      return true;
    }
    return false;
  }

  const RegField *RegBase::get_field(const gc_string &n) const
  {
    auto iter = _fields.find(n);
    return (iter == _fields.end()) ? 0 : iter->second;
  }

  bool RegBase::get_enumerated(const gc_string &s, int &val) const 
  {
    auto it = find_if(_enums.begin(),_enums.end(),equal_1st_in_pair<gc_string,int>(s));
    if (it == _enums.end()) {
      return false;
    }
    val = it->second;
    return true;
  }

  bool RegBase::is_enumerated(const gc_string &s) const 
  {
    return (find_if(_enums.begin(),_enums.end(),equal_1st_in_pair<gc_string,int>(s)) != _enums.end());
  }

  void RegBase::set_default_width()
  {
    if ( _width == 0 ) {
      _width = DefaultRegWidth;
    }
  }

  bool RegBase::is_reg_used() const
  { 
    return _usage_flags != 0; 
  }

  bool RegBase::is_reg_read() const
  {
    return _usage_flags & RegRead;
  }

  bool RegBase::is_reg_written() const
  {
    return _usage_flags & RegWrite;
  }

  bool RegBase::is_reg_slice_read() const
  {
    return _usage_flags & RegSliceRead;
  }

  bool RegBase::is_reg_slice_written() const
  {
    return _usage_flags & RegSliceWrite;
  }

  bool RegBase::is_reg_field_read() const
  {
    return _usage_flags & RegFieldRead;
  }

  bool RegBase::is_reg_field_written() const
  {
    return _usage_flags & RegFieldWrite;
  }

  void RegBase::finalize(Core &core,unsigned id,bool has_writehook)
  {
    Attrs::finalize();

    set_default_width();

    if (_log_name.empty()) {
      _log_name = _name;
    }

    _id = id;

    ForEach(_fields,i) {
      i->second->finalize(core,_offset,_width,has_writehook);
    }

    // check for uniqueness, for reserved only entries  
    check_enums(loc(),_enums);

    if (_resettype == Enum) {
      if (!is_enumerated()) {
        PError(_resetvalue,"Unknown enum value.");
      }
      gc_string  reset = _resetvalue->ToString();
      int     val = 0;
      if (!get_enumerated(reset,val) && !core.get_regbase_enum(reset,val)) {
        PError(_resetvalue,"Unknown enum value.");
      }
      _resetvalue = Ptree::Make("%d",val);
      _resettype = Value;
    }

    StrSet excepts;
    ForEach(_enums,i) {
      excepts.insert(i->first);
    }

    // Check reset code for unknown variables.
    if (_resetaction) {
      CheckCode cc(_env,core,false);
      cc._vw.set_cur_reg(this);
      cc.check_code(_resetaction,excepts);

      // If we're a context resource then the reset function must take an integer
      // argument, which may be used to reset the register based upon which
      // context index we're dealing with.
      int numargs = get_arity(_resetaction);
      if ( core.is_ctx_resource(*this) ) {
        if (numargs != 1) {
          PError(_resetaction,"Reset function for a context resource should take one argument.");
        }
        if ( !arg_is_uint(_resetaction,0) ) {
          PError(_resetaction,"First argument to context-resource reset function must be an unsigned integer.");
        }
      } else {
        if ( numargs ) {
          PError(_resetaction,"Reset function for a non-context resource should not take any parameters.");
        }
      }
    }

    // Check the external-write hook, if it exists.
    if (_external_write) {
      CheckCode cc(_env,core,false);
      cc._vw.set_cur_reg(this);
      cc.check_code(_external_write,excepts);
    }

    // Calculate reserved and read-only masks from field data.
    _reserved.resize(width());
    _readonly.resize(width());

    // The reserved mask starts off as completely filled- we then
    // toggle off bits as we encounter non-reserved fields. 
    if (!_fields.empty()) {
      _reserved.set();
      ForEach (_fields,i) {
        RegField &rf = *(i->second);
        ForEach(rf.bits(),i) {
          unsigned lo = i->lo(_width)-_offset;
          unsigned hi = i->hi(_width)-_offset;
          if (rf.indexed()) {
            // If we have an indexed field, then we know it covers the entire
            // register.
            lo = 0;
            hi = width()-1;
          }
          if (!rf.reserved()) {
            _reserved.reset(lo,hi);
          }
          if (rf.readonly()) {
            _readonly.set(lo,hi);
          }
        }
      }
    }
  }

  static void printRegUsedFlags(ostream &o,const gc_string &pfx,unsigned usage_flags)
  {
    if (usage_flags) {
      o << pfx << "Usage:  ";

      bool first = true;

      if (usage_flags & RegRead) {
        list_sep(o,first);
        o << "read";
      }
      if (usage_flags & RegWrite) {
        list_sep(o,first);
        o << "written";
      }
      if (usage_flags & RegSliceRead) {
        list_sep(o,first);
        o << "slice-read";
      }
      if (usage_flags & RegSliceWrite) {
        list_sep(o,first);
        o << "slice-written";
      }
      if (usage_flags & RegFieldRead) {
        list_sep(o,first);
        o << "field-read";
      }
      if (usage_flags & RegFieldWrite) {
        list_sep(o,first);
        o << "field-written";
      }
      if (usage_flags & RegFieldHelperUsed) {
        list_sep(o,first);
        o << "helper-used";
      }
      o << "\n";
    }
  }

  void RegBase::dump(std::ostream &o,const gc_string &pfx) const
  {
    printRegUsedFlags(o,pfx,_usage_flags);

    if (_name != _log_name) {
      o << pfx << "Log name:  " << _log_name << "\n";
    }
    o << pfx << "Width:  " << _width << "\n";
    if (_offset) {
      o << pfx << "Offset:  " << _offset << "\n";
    }
    if (_pseudo) {
      o << pfx << "Pseudo:  " << _pseudo << "\n";
    }
    if (_serial) {
      o << pfx << "Serial: " << _serial << "\n";
    }

    gc_string newpfx = indent(pfx);
    if (!_fields.empty()) {
      printHashOrdered(o,newpfx,_fields);
    }
    Attrs::dump(o,pfx);
    if (_resettype != None) {
      o << pfx << "Reset:  ";
      if (_resettype == Value || _resettype == Enum) {
        o << _resetvalue;
      } else if (_resettype == Action) {
        o << _resetaction;
      }
      o << "\n";
    }
    if (_external_write) {
      o << pfx << "External Write:  " << _external_write << "\n";
    }
    if (!_enums.empty()) {
      o << pfx << "Enums:  ";
      ForEach(_enums,i) {
        o << "'" << i->first << "' ";
      }
      o << "\n";
    }

    dump_doc(o,pfx,true);
    o << pfx << "-------------------------------\n";
  }

  //
  // Helper class for objects which refer to registers.  Stores a
  // gc_string name which will be looked up during finalization.
  //

  RegName::RegName() : _name(0), _reg(0), _index(InvalidIndex) {};

  RegName::RegName(Opencxx::Ptree *n) : 
    _name(n), _reg(0), _index(InvalidIndex) 
  {}

  const Reg *RegName::reg() const 
  { 
    return dynamic_cast<const Reg*>(_reg); 
  }

  const RegFile *RegName::regfile() const 
  { 
    return dynamic_cast<const RegFile*>(_reg); 
  }

  void RegName::finalize(const Core &core)
  {
    using namespace PtreeUtil;

    if (_name) {
      // First, see if we have a register or register-file reference and make
      // sure that index is valid if the latter.
      auto rp = get_reg_refp(_name,"register/register-file name.",true,core.env());
      _index = rp.second;

      // Then see if we're referencing a specific context.
      Ptree *c, *i, *r;
      if ( Match(rp.first,"[ [ %? ( [ %? ] ) ] . %? ]",&c,&i,&r)) {
        const char *cname = get_ident(c,"context name");
        int index = get_uint(i,"context index",core.env());
        if (const Context *ctx = core.get_ctx(cname)) {
          if ( index >= ctx->num_contexts() ) {
            PError(_name,"Index of " << index << " exceeds number of contexts for " << cname << " (" 
                   << ctx->num_contexts() << ").");
          }
          _ctx = CtxInstance(ctx,index);
          _name = r;
        } else {
          PError(_name,"Unknown context '" << cname << "'.");
        }
      } else {
        _name = rp.first;
      }

      const char *rn = get_ident(_name,"register/register-file name.");

      if ( !(_reg = core.get_reg(rn)) ) {
        if ( !(_reg = core.get_regfile(rn)) ) {
          PError(_name,"Register/register-file not found:  " << rp.first);
        }
        if ( !has_child_id() && (_index >= (int)regfile()->size()) ) {
          PError(_name,"Index of " << dec << _index << " is out of bounds of register-file " << _reg->name());
        }
      }
      _name = 0;
    }
  }

  std::ostream &operator<<(std::ostream &o,const RegName &rn)
  {
    if (rn._reg) {
      o << rn._ctx << rn._reg->name();
      if (rn.has_child_id()) {
        o << "(child_id)";
      }
      else if (rn.has_index()) {
        o << "(" << dec << rn._index << ")";
      }
    }
    return o;
  }

  //
  // Information about a register entry in a sparse register file.
  //

  RegEntry::RegEntry(Environment *env,Ptree *loc,unsigned id) :
    Attrs(env,loc),
    _id(id),
    _read(0),
    _write(0),
    _syntax("")
  {
  }

  // Does basic finalization and returns true if the result is a register-file
  // which doesn't have a single entry.  This indicates that we should map
  // consecutive entries in the parent sparse file.
  bool RegEntry::finalize(Core &core,MsgLog &ml,unsigned max_size)
  {
    Attrs::finalize();

    _reg.finalize(core);

    if (!_reg && !(_read && _write)) {
      AddError(ml,loc(),"No register/register-file specified and a read and write hook were not defined.");
    }

    if (_id >= max_size) {
      AddError(ml,loc(),"Entry index of " << _id << " exceeds size of register file (" << max_size << ").");
    }

    if (_read) {
      _read->finalize(core,false,_reg);
    } else if (_reg) {
      const_cast<RegBase*>(_reg())->set_reg_usage(RegRead);
    }

    if (_write) {
      _write->finalize(core,true,_reg);
    } else if (_reg) {
      const_cast<RegBase*>(_reg())->set_reg_usage(RegWrite);
    }

    if (_reg()) {
      core.add_used_reg(_reg());
    }
    
    return (_reg.regfile() && !(_reg.has_index() || _reg.has_child_id()));

  }

  void RegEntry::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << dec << _id << ":  " << _reg << "\n";
    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);
    if (_read) {
      o << pfx1 << "Read hook:  ";
      _read->dump(o,pfx2);
      o << "\n";
    }
    if (_write) {
      o << pfx1 << "Write hook:  ";
      _write->dump(o,pfx2);
      o << "\n";
    }
    if (syntax() != "") {
      o << pfx2 << "Syntax: '" << syntax() << "'\n";     
    }
  }

  RegEntryHook::RegEntryHook(Environment *env,Ptree *loc) :
    Attrs(env,loc),
    _ignore(false),
    _action(0)
  {
  }

  void RegEntryHook::finalize(Core &core,bool is_write,const RegName &reg)
  {
    Attrs::finalize();
    if (!_action && !_ignore) {
      PError(loc(),"No action function specified and ignore flag not set.");
    }

    if (_ignore && _action) {
      PError(loc(),"You cannot specify the ignore flag and an action sequence.");
    }

    if (_ignore && !is_write) {
      PError(loc(),"The ignore flag is meaningless for a read hook.");
    }

    CheckCode cc(_env,core,!is_write);
    if (reg) {
      cc._vw.set_cur_reg(reg(),reg.index());
    }
    cc.check_code(_action);
    _usageinfo.update(cc.usageinfo());
  }

  void RegEntryHook::dump(ostream &o,const gc_string &pfx) const
  {
    if (_action) {
      o << _action;
    }  
    if (_ignore) {
      o << "(ignored)";
    }
    Attrs::dump(o,pfx);
    o << '\n';
    printUsageInfo(o,_usageinfo,pfx);
  }

  //
  // Information about a register file.
  //
  RegFile::RegFile(Environment *env,Ptree *loc,const gc_string &name) :
    RegBase(env,loc,name),
    _size(0),
    _read(0),
    _write(0)
  {
  }

  bool RegFile::has_immediate_writehook() const 
  { 
    return (_write && _write->immediate()); 
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a register.
  bool RegFile::operator==(const RegFile &x) const
  {
    if (size() != x.size()) {
      return false;
    }

    return RegBase::operator==(x);
  }

  void RegFile::check_sharing_okay() const
  {
    if (sparse()) {
      PError(loc(),"Sparse register-files may not be marked as shared.");
    }

    RegBase::check_sharing_okay();
  }

  bool RegFile::has_alias() const
  {
    if (_read && _read->aliased()) {
      return true;
    }
    if (_write && _write->aliased()) {
      return true;
    }
    return false;
  }

  bool RegFile::has_start_stop() const
  {
    if (_write) {
      return _write->has_start_stop();
    }
    return false;
  }

  bool RegFile::has_hook_action() const
  {
    if (_write) {
      return _write->action();
    }
    return false;
  }

  void check_alias_width(MsgLog &ml,const RegHookBase *hook,unsigned width)
  {
    if (hook && hook->aliased() ) {
      unsigned awidth = (hook->slice()) ? ( hook->slice_width()) : hook->alias()()->width();
      if (awidth < width) {
        AddWarning(ml,hook->loc(),"Target of alias is smaller than the alias.  This will result in 0-padding in the higher-order bits.");
      } else if (awidth > width) {
        AddWarning(ml,hook->loc(),"Target of alias is larger than the alias.  This will result in truncation.");
      }
    }
  }

  void RegFile::finalize_alias(Core &core,MsgLog &ml)
  {
    if (_read) {
      bool has_parms = false;
      _read->finalize(this,core,false,has_parms);
      if (_read->ignore()) {
        PError(loc(),"Read hooks cannot specify the ignore flag.");
      }
    }
    if (_write) {
      bool has_parms = false;
      _write->finalize(this,core,true,has_parms);
      
    }
    if (_read && _read->aliased() && _write && (_write->aliased() || _write->ignore())) {
      _pseudo = true;

      if ( _write->ignore() || (*_read == *_write) ) {
        if ( _width == 0) {
          if ( _read->slice() ) {
            _width = _read->slice_width();
          } else {
            _width = _read->alias()()->width();
          }
        }
      }   

      if (_size == 0) {
        unsigned r_alias = _read->alias().regfile()->size();
        unsigned w_alias = _write->alias().regfile()->size();
        if (r_alias != w_alias) {
          PError(loc(),"Register file is a pure alias but register-file sizes are not the same and size is not set.");
        } else {
          _size = r_alias;
        }
      } 
    }

    // Take care of this now to avoid unnecessary warnings.
    set_default_width();
  }

  void RegFile::finalize(Core &core,MsgLog &ml,unsigned id)
  {
    check_alias_width(ml,_read,_width);
    check_alias_width(ml,_write,_width);

    if ( _size == 0 ) {
      PError(loc(),"Size not set for register file and this is not a pure alias to another register file.");
    }
  
    if (sparse()) {
      ForEach(_entries,i) {
        RegEntry &re = *i->second;
        LogPError(ml,{
            if (re.finalize(core,ml,_size)) {
              // If we return true, then we have a register-file which shouldw
              // map across consecutive entries.  Change the existing entry to
              // the first element, then replicate and increment.
              re.set_alias_index(0);
              unsigned new_id = re.id() + 1;
              for (unsigned index = 1; index != re.reg().regfile()->size(); index++,new_id++) {
                RegEntry *new_entry = new RegEntry(re);
                new_entry->set_id(new_id);
                new_entry->set_alias_index(index);
                _entries[new_id] = new_entry;
              }
            }
      });
      }
    }

    if (_pseudo) {
      if (!_read) {
        PError(loc(),"Register-file declared as pseudo but no read hook or alias is present.");
      }
      if (!_write) {
        PError(loc(),"Register-file declared as pseudo but no write hook or alias is present.");
      }
    }
    if (sparse() && (_read || _write)) {
      PError(loc(),"Sparse register files may not have read or write hooks.");
    }
    RegBase::finalize(core,id,_write);

    if (_invalid_entry_read) {
      LogPError(ml,{
          CheckCode cc(_env,core,true);
          cc.check_code(_invalid_entry_read);
          if (!arg_is_uint(_invalid_entry_read._func,0)) {
            PError(_invalid_entry_read._func,"First argument to invalid-entry-read hook should be an unsigned integer.");
          }
        });
    }

    if (_invalid_entry_write) {
      LogPError(ml,{
          CheckCode cc(_env,core,false);
          cc.check_code(_invalid_entry_write);
          if (!arg_is_uint(_invalid_entry_write._func,0)) {
            PError(_invalid_entry_read._func,"First argument to invalid-entry-write hook should be an unsigned integer.");
          }
          if (!arg_is_bits(_invalid_entry_write._func,1,width())) {
            PError(_invalid_entry_read._func,"Second argument to invalid-entry-write hook should be a bits value that is the width of the register file.");
          }
        });
    }

    if (_external_write) {
      if (get_arity(_external_write) != 2) {
        PError(_external_write,"The external-write hook takes two parameters.");
      }
      // Arg 0:  Index of element to set.
      if (!arg_is_uint(_external_write,0)) {
        PError(_external_write,"First argument to external-write hook function must be an unsigned integer.");
      }

      // Arg 0:  New register value.
      if (!arg_is_bits(_external_write,1,width())) {
        PError(_external_write,"Second argument to external-write hook must be a 'bits' of the same width as the register.");
      }
    }

  }

  void RegFile::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  \"" << _name << "\"\n"
      << pfx << "Size:  " << _size << "\n";
    if (!_prefix.empty()) {
      o << pfx << "Prefix:  " << _prefix << "\n";
    }
    if (sparse()) {
      o << pfx << "Contained registers:\n";
      gc_string newpfx = indent(pfx);
      ForEach(_entries,i) {
        i->second->dump(o,newpfx);
      }
    }
    gc_string npfx = indent(pfx);
    if (_read) {
      o << pfx << "Read alias/hook:\n";
      _read->dump(o,npfx);
      o << "\n";
    }
    if (_write) {
      o << pfx << "Write alias/hook:\n";
      _write->dump(o,npfx);
      o << "\n";
    }
    if (_invalid_entry_read) {
      o << pfx << "Invalid-Entry-Read Handler:  ";
      _invalid_entry_read.print(o,npfx);
    }
    if (_invalid_entry_write) {
      o << pfx << "Invalid-Entry-Write Handler:  ";
      _invalid_entry_write.print(o,npfx);
    }

    RegBase::dump(o,pfx);
  }

  //
  // Information about a single register.
  //

  Reg::Reg(Environment *env,Ptree *loc,const gc_string &name) :
    RegBase(env,loc,name),
    _read(0),
    _write(0)
  {
  }

  bool Reg::has_immediate_writehook() const 
  { 
    return (_write && _write->immediate()); 
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a register.
  bool Reg::operator==(const Reg &x) const
  {
    return RegBase::operator==(x);
  }

  bool Reg::has_alias(bool check_hooks) const
  {
    if (_read) {
      if (_read->aliased()) return true;

      if (check_hooks) {
        const UsageInfo &ui = _read->usageinfo();
        if (ui._src_regs.size() > 0) return true;
      }
    }
    if (_write) {
      if(_write->aliased()) return true;

      if (check_hooks) {
        const UsageInfo &ui = _write->usageinfo();
        if (ui._trg_regs.size() > 0) return true;
      }
    }
    return false;
  }

  bool Reg::has_start_stop() const
  {
    if (_write) {
      return _write->has_start_stop();
    }
    return false;
  }

  bool Reg::has_hook_action() const
  {
    if (_write) {
      return _write->action();
    }
    return false;
  }

  // Final construction pass- does various checks and populates
  // various data structures.
  void Reg::finalize(Core &core,MsgLog &ml,unsigned id)
  {
    if (_pseudo) {
      if (!_read) {
        PError(loc(),"Register declared as psuedo but no read hook or alias is present.");
      }
      if (!_write) {
        PError(loc(),"Register declared as psuedo but no write hook or alias is present.");
      }
    }

    if (_read) {
      bool has_parms = false;
      // read hook does not take value argument
      _read->finalize(this,core,false,has_parms);
      if (_read->ignore()) {
        PError(loc(),"Read hooks cannot specify the ignore flag.");
      }
      if (has_parms && core.is_shared(*this)) {
        PError(loc(),"Read hooks of shared variables may not access architectural parameters.");
      }
    }
    if (_write) {
      bool has_parms = false;
      _write->finalize(this,core,true,has_parms);
      if (has_parms && core.is_shared(*this)) {
        PError(loc(),"Write hooks of shared variables may not access architectural parameters.");
      }
    }

    // If we're aliased to the same register, or we ignore writes, then we
    // generate our width from the target's width, unless otherwise specified.
    if (_read && _read->aliased() && _write && (_write->aliased() || _write->ignore())) {
      _pseudo = true;

      if ( _write->ignore() || (*_read == *_write) ) {
        if ( _width == 0) {

          if ( _read->slice() ) {
            _width = _read->slice_width();
          } else {
            _width = _read->alias()()->width();
          }
        }
      }
    }

    check_alias_width(ml,_read,_width);
    check_alias_width(ml,_write,_width);

    RegBase::finalize(core,id,_write);

    if (_external_write) {
      if (get_arity(_external_write) != 1) {
        PError(_external_write,"The external-write hook takes one parameter.");
      }
      // Arg 0:  New register value.
      if (!arg_is_bits(_external_write,0,width())) {
        PError(_external_write,"Argument to external-write hook must be a 'bits' of the same width as the register.");
      }
    }

  }

  void Reg::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  \"" << _name << "\"\n";
    gc_string npfx = indent(pfx);
    if (_read) {
      o << pfx << "Read alias/hook:\n";
      _read->dump(o,npfx);
      o << "\n";
    }
    if (_write) {
      o << pfx << "Write alias/hook:\n";
      _write->dump(o,npfx);
      o << "\n";
    }
    RegBase::dump(o,pfx);
  }

  //
  // Register hook stuff.
  // 

  RegHookBase::RegHookBase(Environment *env,Ptree *loc) :
    Base(env,loc),
    _ignore(false),
    _slice(false),
    _immediate(ImmNotSet),
    _has_start_stop(false),
    _low(0),
    _hi(0),
    _lshift(0),
    _action(0),
    _slice_value(0)
  {
  }

  bool RegHookBase::setImmediate(const Core &core,bool is_write)
  {
    // The immediate flag inherits its value from the core if it's not set.
    bool imm_not_set = false;
    if (is_write && _action) {
      if (_immediate == ImmNotSet) {
        imm_not_set = true;
        _immediate = (core.immediate_writehooks()) ? ImmTrue : ImmFalse;
      }
    } else {
      // This doesn't matter for reads.
      _immediate = ImmFalse;
    }
    return imm_not_set;
  }

  void RegHookBase::finalize(const RegBase *parent,Core &core,
                             bool is_write,bool &has_parms,
                             const pair<Opencxx::Ptree*,Opencxx::Ptree*> &slice_parms)
  {
    _alias.finalize(core);

    if (!_action && !_alias && !_ignore) {
      PError(loc(),"No alias or action function specified and ignore flag not set.");
    }
    if (_action && _alias) {
      PError(loc(),"You cannot have both an alias specifier and an action function.");
    }
    if (_low > _hi) {
      PError(loc(),"The low-slice index must be <= the high-slice index.");
    }

    if (_slice_value) {
      // For a slice, we expect a pair of integers.
      Ptree *l,*h,*ls;
      if (PtreeUtil::Match(_slice_value,"[ ( [ %? , %? ] ) ]",&l,&h)) {
        ls = h;
        unsigned low = 0,hi = 0,lshift = 0;
        if (PtreeUtil::Match(l,"[ ( [ %? , %? ] ) ]",&l,&h)) {
          lshift = get_uint(ls,"slice left-shift value.",env());
        }
        low = get_uint(l,"slice alias lower index.",env());
        hi = get_uint(h,"slice alias upper index.",env());
        _slice = true;
        _low = low;
        _hi = hi;
        _lshift = lshift;
      } else {
        PError(_slice_value,"Bad slice specifier- expected a pair of two integers.");
      }
      _slice_value = 0;

    }

    if (_alias()) {
      const_cast<RegBase*>(_alias())->set_reg_usage((_slice) ? (RegSliceRead | RegSliceWrite) : (RegRead | RegWrite));
    }

    if (parent == _alias()) {
      PError(loc(),"An alias cannot refer to itself.");
    }

    if (_ignore && (_alias || _action)) {
      PError(loc(),"You cannot specify the ignore flag and an action sequence or alias.");
    }

    // If we have an action, then it can only refer to other registers- any
    // variables which are not declared in the scope of the action must be
    // registers or enumerated values, else it's an error.  The user can also
    // call the updateReg function to use a ValueBundle to perform the default
    // update.
    StrSet  excepts;
    excepts.insert(UpdateReg);
    ForEach(parent->enums(),i) {
      excepts.insert(i->first);
    }

    CheckCode cc(_env,core);
    cc._vw.set_cur_reg(parent);
    cc._vw.set_slice_ignore(slice_parms);
    cc.check_code(_action,excepts);
    _usageinfo.update(cc.usageinfo());
    has_parms = cc.has_parms();
  }

  void RegHookBase::dump(ostream &o,const gc_string &pfx) const
  {
    if (immediate()) {
      o << pfx << "Immediate\n";
    }
    if (_alias) {
      o << pfx << _alias;
      if (_slice) {
        o << "[" << _low << ":" << _hi;
        if (_lshift) {
          o << " << " << _lshift;
        } 
        o << "]";
      }
    } else if (_action) {
      o << pfx << _action;
    }
    if (_ignore) {
      o << pfx << "(ignored)";
    }
  }

  bool operator==(const RegHookBase &x,const RegHookBase &y)
  {
    return ( x._alias == y._alias &&
             x._ignore == y._ignore &&
             x._immediate == y._immediate &&
             x._slice == y._slice &&
             x._low == y._low &&
             x._hi == y._hi &&
             x._lshift == y._lshift &&
             x._action == y._action);
  }

  RegFileHook::RegFileHook(Environment *env,Ptree *loc) :
    RegHookBase(env,loc)
  {
  }

  unsigned RegFileHook::getMappedIndex(unsigned index) const
  {
    if (index < _aliasregs.size()) {
      return _aliasregs[index];
    } else {
      return index;
    }
  }

  void RegFileHook::finalize(RegFile *parent,Core &core,bool is_write,bool &has_parms)
  {
    using namespace PtreeUtil;

    pair<Opencxx::Ptree*,Opencxx::Ptree*> slice_parms;
    unsigned width = (parent->width() == 0) ? DefaultRegWidth : parent->width();

    bool imm_not_set = setImmediate(core,is_write);

    // Do type-checking on the action function, if it exists.
    if (_action) {
      if (is_write) {
        int arity = get_arity(_action);
        if (! (arity == 2 || arity == 4) ) {
          PError(_action,"The write-hook takes either 2 or 4 arguments.");
        } 

        // Arg 0: argument should be an unsigned.
        if (!arg_is_uint(_action,0)) {
          PError(_action,"First argument to write-hook action function must be an unsigned integer.");
        }
        // Arg 1:  Make sure that the argument's type is 'bits' of the right width.
        if (!arg_is_bits(_action,1,width)) {
          if (core.parallel_execution()) {
            if (!arg_is_type(_action,"ValueBundle",1)) {
              PError(_action,"Second argument to write-hook action function must be either 'ValueBundle' or a `bits` of the same width as the registers in a register file.");
            } else {
              if (_immediate) {
                if (imm_not_set) {
                  _immediate = ImmFalse;
                } else {
                  PError(_action,"An immediate write-hook cannot use the ValueBundle interface.");
                }
              }
              core.add_concurrent(parent->name());
            }
          } else {
            PError(_action,"Second argument to write-hook action function must be a `bits` of the same width as the registers in a register file.");
          }
        }

        // If two additional arguments are present, then they're start/stop indices and must be of type 'unsigned'.
        if (arity > 2) {
          _has_start_stop = true;
          if (!arg_is_uint(_action,2) || !arg_is_uint(_action,3)) {
            PError(_action,"Third and fourth arguments to write-hook action function must be an unsigned integer.");
          }
          slice_parms.first  = get_func_arg_name(_action,2);
          slice_parms.second = get_func_arg_name(_action,3);
        }
      } else {
        // First argument should be an unsigned.
        if (!arg_is_uint(_action,0)) {
          PError(_action,"First argument to read-hook action function must be an unsigned integer.");
        }
      }
    }

    RegHookBase::finalize(parent,core,is_write,has_parms,slice_parms);
    if (_alias.has_regfile()) {
      if (_alias.has_index()) {
        PError(loc(),"Register files may not be mapped to single elements of another register file.");
      }
      if (!_aliasregs.empty()) {
        if (_aliasregs.size() != parent->size()) {
          PError(loc(),"The size of the list of aliased registers (" 
                 << _aliasregs.size() << ") != the size of the register file (" 
                 << parent->size() << ").");
        }
        ForEach(_aliasregs,i) {
          if ( *i >= _alias.regfile()->size() ) {
            PError(loc(),"Aliased register " << *i << " is out of bounds of the aliased register file.");
          }
        }
      }
    } else if (_alias.has_reg()) {
      PError(loc(),"A register file may only be aliased to another register file.");
    }

  }

  void RegFileHook::dump(ostream &o,const gc_string &pfx) const
  {
    RegHookBase::dump(o,pfx);
    if (_alias && !_aliasregs.empty()) {
      o << " (Regs:  ";
      ForEach (_aliasregs,i) {
        o << *i << " ";
      }
      o << ")";
    }
    o << '\n';
    printUsageInfo(o,_usageinfo,pfx);
  }

  bool operator==(const RegFileHook &x,const RegFileHook &y)
  {
    return (x._aliasregs == y._aliasregs) && operator==((const RegHookBase&)x,(const RegHookBase&)y);
  }

  RegHook::RegHook(Environment *env,Ptree *loc) :
    RegHookBase(env,loc)
  {
  }

  void RegHook::finalize(Reg *parent,Core &core,bool is_write,bool &has_parms)
  {
    pair<Opencxx::Ptree*,Opencxx::Ptree*> slice_parms;

    unsigned width = (parent->width() == 0) ? DefaultRegWidth : parent->width();

    // The immediate flag inherits its value from the core if it's not set.
    bool imm_not_set = setImmediate(core,is_write);

    // Do type-checking on the action function, if it exists.
    if (_action) {
      if (is_write) {
        int arity = get_arity(_action);
        if (! (arity == 1 || arity == 3) ) {
          PError(_action,"The write-hook takes either 1 or 3 arguments.");
        } 

        // Make sure that the first argument's type is 'bits' of the right width.
        if (!arg_is_bits(_action,0,width)) {
          if (core.parallel_execution()) {
            if (!arg_is_type(_action,"ValueBundle",0)) {
              PError(_action,"First argument to write-hook action function must be either 'ValueBundle' or a `bits` of the same width as the registers in a register file.");
            } else {
              if (_immediate) {
                if (imm_not_set) {
                  _immediate = ImmFalse;
                } else {
                  PError(_action,"An immediate write-hook cannot use the ValueBundle interface.");
                }
              }
              core.add_concurrent(parent->name());
            }
          } else {
            PError(_action,"First argument to write-hook action function must be a `bits` of the same width as the registers in a register file.");
          }
        }

        // If two additional arguments are present, then they're start/stop indices and must be of type 'unsigned'.
        if (arity > 1) {
          _has_start_stop = true;
          if (!arg_is_uint(_action,1) || !arg_is_uint(_action,2)) {
            PError(_action,"Second and third arguments to write-hook action function must be an unsigned integer.");
          }
          slice_parms.first  = get_func_arg_name(_action,1);
          slice_parms.second = get_func_arg_name(_action,2);
        }
      }
    }

    RegHookBase::finalize(parent,core,is_write,has_parms,slice_parms);

    if (_alias.has_regfile() && !(_alias.has_child_id() || _alias.has_index())) {
      PError(loc(),"A register may only be aliased to an element of a register-file.");
    }
  }

  void RegHook::dump(ostream &o,const gc_string &pfx) const
  {
    RegHookBase::dump(o,pfx);
    o << '\n';
    printUsageInfo(o,_usageinfo,pfx);
  }

  void BitPairVect::record_bits_adapter(Ptree *p,Environment *env)
  {
    record_bits(p,true,env);
  }

  bool BitPairVect::record_bits(Ptree *p,bool error,Environment *env)
  {
    Ptree *l,*h;
    unsigned lo,hi;
    if (get_uint(lo,p,env)) {
      add_bits(lo,lo);
    } else if (PtreeUtil::Match(p,"[ ( [ %? , %? ] ) ]",&l,&h)) {
      if (get_uint(lo,l,env)) {
        if (get_uint(hi,h,env)) {
          add_bits(lo,hi);
          return true;
        } else if (error) {
          PError(p,"Expected an integer for instruction field upper bit index.");
        } else {
          return false;
        }
      } else if (error) {
        PError(p,"Expected an integer for instruction field lower bit index.");
      } else {
        return false;
      }
    } else if (error) {
      PError(p,"Each element of the bit specification should be an integer or a pair of integers.");
    } else {
      return false;
    }
    return true;
  }

  unsigned BitPairVect::highbit() const {
    unsigned high(0);
    ForRange(size(),i) {
      unsigned mx = at(i).l() > at(i).r() ? at(i).l() : at(i).r() ;
      high = high > mx ? high : mx ;
    }
    return high;
  }

  unsigned BitPairVect::lowbit() const {
    unsigned low(-1);
    ForRange(size(),i) {
      unsigned mn = at(i).l() < at(i).r() ? at(i).l() : at(i).r() ;
      low = low > mn ? mn : low ;
    }
    return low;
  }

  unsigned BitPairVect::width() const {
    unsigned ret(0);
    ForRange(size(),i) {
      ret += at(i).width();
    }
    return ret;
  }

  // Returns true if bp intersects with any element in this object.
  bool BitPairVect::intersect(const BitPair &bp) const
  {
    ForEach((*this),i) {
      if ( (*i).intersect(bp) ) {
        return true;
      }
    }
    return false;
  }
  
  void BitPairVect::finalize(Ptree *loc)
  {
    ForRange(size(),i) {
      at(i).check_order(loc);
    }    
    if (size() > 1) {
      BitPairVect new_bits;
      int start = -1;
      int end = -1;
      ForRange(size() - 1,i) {
        if (start == -1) {
          start = at(i).l();
          end = at(i).r();
        }
        if ((at(i).hi(0) + 1) == (at(i+1).lo(0))) {
          end = at(i+1).r();
        } else {
          new_bits.push_back(BitPair(start,end));
          start = end = -1;
        }
      }
      if (start != -1) {
        new_bits.push_back(BitPair(start,end));
      }

      int last = size() - 1;
      if ((at(last - 1).hi(0) + 1) != at(last).lo(0)) {
        // If the last was OK, just add it also
        new_bits.push_back(at(last));
      }
      *this = new_bits;
    } 
 



    
  }
  
  //
  // Information about register fields.
  //

  RegField::RegField (Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _indexed(false),
    _indexcount(0),
    _reserved(false),
    _readonly(false),
    _writable(false),
    _usage_flags(0)
  {
  }

  bool RegField::is_field_used() const
  {
    return _usage_flags;
  }

  bool RegField::is_field_read() const
  {
    return _usage_flags & RegFieldRead;
  }

  bool RegField::is_field_written() const
  {
    return _usage_flags & RegFieldWrite;
  }

  bool RegField::is_field_helper_used() const
  {
    return _usage_flags & RegFieldHelperUsed;
  }

  void RegField::set_indexed(unsigned w)
  {
    _bits.clear();
    _bits.add_bits(0,w-1);
    _indexed = true;
  }
 
  void RegField::finalize(Core& core,unsigned regoffset,unsigned regwidth,bool has_writehook)
  {
    Attrs::finalize();

    // For now, we don't support having the writable attribute set if we also
    // have a write-hook, since that can result in confusing behavior.
    // Basically, what happens is that, unless you write your write-hook in a
    // very specific manner, the writable field doesn't work correctly.  So, we
    // issue an error here for that case.
    if (_writable && has_writehook) {
      PError(loc(),"Fields with the 'writable' attribute and write-hooks are not supported.");
    }

    if (_indexed) {
      unsigned val = _bits[0].r();
      _bits.clear();
      if (core.is_big_endian()) {
        _bits.add_bits(0,val);
      } else {
        _bits.add_bits(val,0);
      }    
    }
    _bits.finalize(loc());
    
    _width = 0;   
    int lowbit  = 1000;
    int highbit = 0;

    if (indexed()) {
      if ( _bits[0].hi(regwidth) < 0) {
        if (highbit == 0) {
          PError(loc(),"Indexed-field width must be non-zero.");
        }
      }
    }
    
    ForEach (_bits,i) {
      for (auto j = _bits.begin(); j != i; ++j) {
        if (i->intersect(*j)) {
          PError(loc(),"Overlapping bit ranges for register field.");
        }
      }
      _width += i->width();
      lowbit  =  lowbit < i->lo(regwidth) ?  lowbit : i->lo(regwidth);
      highbit = highbit > i->hi(regwidth) ? highbit : i->hi(regwidth);     
    }     

     
    if (indexed()) {
      if (is_split()) {
        PError(loc(),"Indexed-field may not be split.");
      }    
      if ( (highbit >= (int)regwidth) ) {
        PError(loc(),"Index greater than width of register.");
      }
      if (regwidth % width()) {
        PError(loc(),"Indexed-field width is not an integer-divisor of the register width.");
      }
      _indexcount = regwidth / width();
    } else {
      if ( (highbit >= (int)(regwidth+regoffset)) || (lowbit >= (int)(regwidth+regoffset))) {
        PError(loc(),"Index greater than width of register.");
      }
      if ( (highbit < (int)regoffset) || (lowbit < (int)regoffset)) {
        PError(loc(),"Index less than offset of register.");
      }    
    }
        
    if (core.parallel_execution()) {
      _write.resize(regwidth);
      ForEach(_bits,i) {
        unsigned lo = i->lo(regwidth);
        unsigned hi = i->hi(regwidth);
        if (!indexed()) {
          lo -= regoffset;
          hi -= regoffset;
        } 
        _write.set(lo,hi);
      }
    }    
  }

  void RegField::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << _name;

    
    if (indexed()) {
      o << "[" << width() << "bits x " << index_count() << "]";
    } else {
      if (_bits.size() > 1) {
        o << ": ";
      }
      ForEach(_bits,i) {
        if (i->l() != i->r()) {
          o << "[" << i->l() << "," << i->r() << "] ";
        } else {
          o << "[" << i->l() << "] ";
        } 
      }      
    }
    if (_reserved) {
      o << " (reserved)";
    }
    if (_readonly) {
      o << " (readonly";
      if (_writable) {
        o << ", field-writable";
      }
      o << ")";
    }
    o << "  ";
    Attrs::dump(o);
    o << "\n";
    gc_string newpfx = indent(pfx);
    printRegUsedFlags(o,newpfx,_usage_flags);
    dump_doc(o,newpfx,false);
  }

  //
  // Information about instruction fields.
  //

  bool BitPair::intersect(const BitPair &x) const
  { 
    if (is_big_endian()) {
      if (x.r() < l())
        return false;
      if (x.l() > r())
        return false; 
      return true;    
    } else { 
      if (x.r() > l())
        return false;
      if (x.l() < r())
        return false; 
      return true;    
    }    
  }

  bool BitPair::subset(const BitPair &x) const
  { 
    if (is_big_endian()) {
      return (x.l() >= l() && x.r() <= r());
    } else { 
      return (x.l() <= l() && x.r() >= r());
    }    
  }

  void BitPair::check_order(Ptree* loc) const {
    bool error(false);
    gc_string comp("");
    if (is_big_endian()) {
      error = (_l > _r);
      comp = "<=";
    } else {
      error = (_l < _r);
      comp = ">=";   
    }
    if (error) {
      PError(loc,"Left bit index must be " << comp << " right bit index.");
    }
  }
  
  ostream &operator<<(ostream &o,BitPair bp)
  {
    o << "[" << bp.l() << "," << bp.r() << "]";
    return o;
  }

  InstrFieldTable::InstrFieldTable(Environment *env, Ptree *loc) :
    Base(env,loc),
    _entry_size(0),
    _size(0)
  {
  }


  bool    InstrFieldTable::is_reserved(const InstrFieldTableEntry &entry)
  {
    return (entry.size() == 1) && (entry[0] == ReservedEntry);
  }

  void 	InstrFieldTable::add_reserved() 
  {
    add_entry();
    add_to_entry(ReservedEntry);
 
  }

  void	InstrFieldTable::add_entry() 
  {
    _entries.push_back(InstrFieldTableEntry());
  }

  void 	InstrFieldTable::add_to_entry(unsigned v)
  {
    _entries.back().push_back(v);
  }


  void	InstrFieldTable::dump(std::ostream &o, const gc_string &pfx) const
  {
    if (!_entries.empty()) {
      bool first_entry = true;
      o << pfx << "Table:  (" ;
      ForEach(_entries,e) {
        list_sep(o,first_entry);
        if(is_reserved(*e)) {
          o << "reserved";
        } else {
          o <<  "( ";
          printSequence(o,*e,false);
          o << ")";
        }
      }
      o << ")\n";
    }
  }

  void	InstrFieldTable::finalize(const Core & core,MsgLog &ml) 
  {
    unsigned max_val = 0;
    ForEach(_entries,e) {
      const InstrFieldTableEntry &en = *e;
      if (!is_reserved(en)) {
        if (_entry_size) {
          if (_entry_size != en.size()) {
            PError(loc(),"Table entries of different size.");
          }
        } else {
          _entry_size = en.size();
        }
        ForEach(en,v) {
          if (*v > max_val) {
            max_val = *v;
          }
        }
      }
    }
    if (!_entries.empty() && _entry_size == 0) {
      PError(loc(),"No entries defined for this table.");
    }
    _size = log2(max_val)+1;
  }


  // Relocations are automatically indexed
  Relocation::Relocation(Environment *env,Ptree *loc, const gc_string &name) : 
    Attrs(env,loc), 
    Named(name), 
    _value(Undef), 
    _width(Undef),
    _field_width(Undef),
    _pcrel(Undef),
    _right_shift(Undef),  // Specifies the number of bits the relocation value is right-shifted before it is encoded
    _offset(Undef),       // Specifies the offset relative to the width of the instruction
    _bitpos(0),
    _is_signed(Undef),
    _check_overflow(true),
    _action(0),
    _src_mask(0),
    _dst_mask(0),
    _macro_op_field(0),
    _instr(0),
    _macro_op_instr(0)
  {
  }

  void Relocation::updateFromInstrField(const InstrField &field,MsgLog &ml)
  {
    if (field.is_prefix()) {
      PError(loc(),"Prefix field can not have relocations.");
    } else if (field.type() != InstrField::tImm) {
      PError(loc(),"Only immediate field can have relocations.");
    } else if (field.is_variable()) {
      PError(loc(),"Variable field can not have relocations.");
    } else if (field.encap_field()) {
      PError(loc(),"Nested field can not have relocations.");
    } else if (field.is_parm()) {
      PError(loc(),"Parm field can not have relocations.");
    } else if (field.is_alias()) {
      PError(loc(),"Relocation is not defined for instruction field alias."); 
    }

    if (_field_width == Relocation::Undef) {
      _field_width = field.width();
    } else {
      if (field_width() != field.width()) {
        AddError(ml,field.loc(),"Field-width mismatch with relocation " << name());
      }
    }

    if (_is_signed == Relocation::Undef) {
      _is_signed = field.is_signed();
    } else {
      if (is_signed() != field.is_signed()) {
        AddError(ml,field.loc(),"Signed mismatch with relocation " << name());
      }
    }

    // No error checking here, because we want to allow the user to put a
    // PC-relative calculated value into a normal instruction field.
    if (_pcrel == Relocation::Undef) {
      _pcrel = (field.addr() == InstrField::aPcRel);;
    }

    if (_right_shift == Relocation::Undef) {
      _right_shift = field.shift();
    } else {
      if ((unsigned)right_shift() != field.shift()) {
        AddError(ml,field.loc(),"Instruction-field shift flag mismatch with relocation " << name());
      }
    }
  }

  const Instr *Relocation::find_instr(Core &core_nc,MsgLog &ml,const InstrField &field)
  {
    const Instr *matching_instr = 0;
    
    // Find first instruction which uses this field.
    int width = -1;

    const Core &core = *(&core_nc);
    ForEach(core.instrs(),i) {
      const Instr *instr = i->second;
      int index;
      if ((index = instr->getField(field.name())) != -1) {
        if (width < 0) {
          matching_instr = instr;
          width = instr->width();
          // Now we have to update that implementation field's relocation as well.
          auto f = instr->getField(index);
          if (f._field) {
            core_nc.updateInstrFieldReloc(f._field->name(),this);
          }
        } else if (instr->width() != (unsigned)width) {
          AddError(ml,loc(),"Field " << field.name() << ", referenced by relocation " << name() << " is used by instructions of different widths.");
        }
      }
    }

    return matching_instr;
  }

  void Relocation::handle_instr_field(Core &core_nc,MsgLog &ml)
  {
    const Core &core = *(&core_nc);
    
    // Is this a pair?  If so, then it identifies a micro-op and a macro-op.  If
    // just an identifier, then it identifies just a micro-op.
    Ptree *micro_op, *macro_op;
    if (PtreeUtil::Match(_field._p,"[ ( [ %? , %? ] ) ]",&micro_op,&macro_op)) {
      auto micro_fn = get_ident(micro_op,"relocation's micro-op instruction field name.");
      auto macro_fn = get_ident(macro_op,"relocation's macro-op instruction field name.");

      _field._d = core.get_instrfield(micro_fn);
      _macro_op_field = core.get_instrfield(macro_fn);

      if (!_field._d) {
        AddError(ml,loc(),"Unknown micro-op instruction field " << micro_fn);
      }

      if (!_macro_op_field) {
        AddError(ml,loc(),"Unknown macro-op instruction field " << macro_fn);
      }      
    } else {
      auto fn = get_ident(_field._p,"relocation's instruction field name.");
      _field._d = core.get_instrfield(fn);

      if (!_field._d) {
        AddError(ml,loc(),"Unknown instruction field " << fn);
      }
    }
   
    if (_field._d) {
      const InstrField &field = *(_field._d);

      // Add a pointer back to here for convenience when querying fields for a
      // default relocation.  If more than one relocation matches to the same
      // field, then the behavior is not well defined, but then again, it
      // doesn't really make sense to have more than one default relocation.
      core_nc.updateInstrFieldReloc(field.name(),this);

      updateFromInstrField(field,ml);
    }
  }

  const InstrField *Relocation::findFieldImpl(const Core &core,MsgLog &ml,const InstrField &field)
  {
    // Scan through all fields and find ones which implement this pseudo
    // field.  If there's more than one, it's an error.
    const InstrField *f_impl = nullptr;
    ForEach(core.instrfields(),i) {
      const InstrField *f = i->second;
      if (f->interface() == &field) {
        if (!f_impl) {
          f_impl = f;
        } else {
          AddError(ml,loc(),"Field " << f->name() << ", referenced by relocation " << name() << " is used by "
                   "multiple instruction field implementations.  This is not allowed.");
          break;
        }
      }
    }
    return f_impl;
  }
  
  void Relocation::finalize(Core &core_nc,MsgLog &ml) 
  {
    Attrs::finalize();

    const Core &core = *(&core_nc);
    // Copy over relevant information from the linked instruction field, if it
    // has one.
    if (_field) {
      handle_instr_field(core_nc,ml);
    }
    
    if (_value == Undef) {
      PError(loc(),"Undefined relocation value.");
    }

    if (_field_width == Undef) {
      _field_width = 0;
    }

    if (_pcrel == Undef) {
      _pcrel = 0;
    }

    if (_right_shift == Undef) {
      _right_shift = 0;
    }

    if (_is_signed == Undef) {
      _is_signed = 0;
    }
  
    if (_action) {
      LogPError(ml,{
          CheckCode cc(_env,core);
          cc.check_code(_action);
          if (!arg_is_addr(_action,0)) {
            AddError(ml,_action,"Argument 1 of action function must be of type addr_t.");
          }
          if (!arg_is_int(_action,1)) {
            AddError(ml,_action,"Argument 2 of action function must be of type int.");
          }
        });
    }

    // Our field shouldn't be a pseudo field.  If it is, then we have to find
    // the implementation field.  There should only be one, since if there are
    // multiple implementations, it implies multiple ways in which the linker
    // would have to insert the relocation.
    if (_field && _field().is_pseudo()) {
      _field.set_dptr(findFieldImpl(core,ml,_field()));
    }

    // Sanity check.
    assert(!(_field && _field().is_pseudo()));
    
    // Find the associated instruction, if little-endian.
    if (_field && core.is_little_endian()) {
      _instr = find_instr(core_nc,ml,*_field._d);

      if (!_instr) {
        AddError(ml,loc(),"Field " << _field().name() << ", referenced by relocation " << name() << " is not used by "
                 "any instructions in a little-endian architecture.  This is not allowed because we cannot determine position information for the field.");
      }
    }

    // If we have micro ops, then check to see if this relocation is associated
    // with a micro-op.  If it is, then we have to find the field(s) which
    // stores the instruction associated with this relocation.  Each relocation
    // has to be unique wrt. a macro-op field, since there's no way to convey,
    // within the relocation, which field is being used.  Thus, if we find more
    // than one item, we have to issue an error.
    if (_field && core.has_micro_ops()) {
      if (!_instr) {
        _instr = find_instr(core_nc,ml,*_field._d);
      }
      
      // Only proceed if we found an instruction and that instruction is a micro-op.  Should we require an instruction?
      if (_instr && _instr->is_micro_op()) {
        // We need to figure out which instruction fields store the instruction
        // table associated with this instruction.
        auto micro_op_tables = core.getMicroOpFields();

        bool macro_op_specified = _macro_op_field;
        bool specified_field_found = false;
        bool found = false;
        ForEach(micro_op_tables,aiter) {
          int id = aiter->second._id;
          if (_instr->has_attr(id)) {
            const gc_string &fn = aiter->first;
            const InstrField *f = core.get_instrfield(fn);

            if (found) {
              AddError(ml,loc(),"Relocation " << name() << " is associated with a micro op that can be used in multiple macro ops (" << _macro_op_field->name() << " and " << f->name() << ")");
            } else {
              if (macro_op_specified) {
                if (_macro_op_field == f) {
                  specified_field_found = true;
                }
              } else {
                _macro_op_field = f;
                found = true;
              }
            }
          }
        }

        // If the user specified a macro-op field and that was never found to be
        // associated with this instruction, then we signal an error.
        if (macro_op_specified && !specified_field_found) {
          AddError(ml,loc(),"Relocation " << name() << " is not associated with macro-op field " << _macro_op_field->name());
        }

        // Find the associated instruction, if little-endian.
        if (_macro_op_field && core.is_little_endian()) {
          _macro_op_instr = find_instr(core_nc,ml,*_macro_op_field);
          
          if (!_macro_op_instr) {
            AddError(ml,loc(),"Macro-op field " << _macro_op_field->name() << ", referenced by relocation " << name() << " is not used by "
                     "any instructions in a little-endian architecture.  This is not allowed because we cannot determine position information for the field.");
          }

          if (_macro_op_field->is_pseudo()) {
            _macro_op_field = findFieldImpl(core,ml,*_macro_op_field);
          }
        }
      }
    }
             
    // If not specified, then the destination mask is simply set to 1s for every
    // bit of the field width.
    if (!_dst_mask && _field_width) {
      unsigned shift = sizeof(uint64_t)*8 - _field_width;
      _dst_mask = (uint64_t)(-1);
      _dst_mask = (_dst_mask << shift) >> shift;
    }

  }

  void Relocation::dump(ostream &o, const gc_string &pfx) const
  {
    o << pfx << _name << " " << _value << "\n";
    gc_string pfx1 = indent(pfx);
    if (!_abbrev.empty()) {
      o << pfx1 << "Abbrev:  " << _abbrev << "\n";
    }
    o << pfx1 << "Field width:  " << _field_width << "\n"
      << pfx1 << "PC-relative:  " << _pcrel << "\n";
    if (_width != Undef) {
      o <<  pfx1 << "Instr width:  " << _width << "\n";
    }    
    if (_right_shift != Undef) {
      o <<  pfx1 << "Right-shift:  " << _right_shift << "\n";
    }  
    if (!_right_shift_exp.empty()) {
      o <<  pfx1 << "Right-shift expression:  " << _right_shift_exp << "\n";
    }      
    if (_offset != Undef) {
      o << pfx1 << "Offset:  " << _offset << "\n";
    }
    if (_bitpos) {
      o << pfx1 << "Bitpos:  " << _bitpos << "\n";
    }
    if (_is_signed) {
      o << pfx1 << "Signed:  True\n";
    }
    if (_check_overflow) {
      o << pfx1 << "Check Overflow:  True\n";
    }
    if (_src_mask != 0) {
      o << pfx1 << "Src Mask:  0x" << hex << _src_mask << dec << "\n";
    }
    if (_dst_mask != 0) {
      o << pfx1 << "Dst Mask:  0x" << hex << _dst_mask << dec << "\n";
    }
    if (_action) {
      o << pfx1 << "Action:  " << _action << "\n";
    }
    Attrs::dump(o,pfx1);
    dump_doc(o,pfx1,true);
  }

  PrefixCounter::PrefixCounter (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _reset_value(0)
  {}

  void PrefixCounter::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << name() << ":  " << dec << _reset_value << "\n";
  }

  InstrField::InstrField(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _highbit(0),
    _width(0),
    _size(0),
    _mask(0),
    _type(tImm),
    _display(Default),
    _overlay(false),
    _signed(false),
    _unsigned_ub(false),
    _inverted(false),
    _pseudo(false),
    _prefix(false),
    _complement(false),
    _allow_conflict(false),
    _addr_type(aNone),
    _shift(0),
    _offset(0),
    _indexed(0),
    _index_endianness(UseParent),
    _encap_field(0),
    _ref(0),
    _instrtable(0), //
    _regfile(0),
    _memory(0),
    _table(env,loc),
    _default(0),
    _action(0),
    _pfield(0),
    _syntax(0),
    _alias(0),
    _reserved(false),
    _assembler(false),
    _disassemble(dsTrue),
    _parm(false),
    _generated(false),
    _splitInfo(0),
    _used(false)
  {
  }


  InstrField *InstrField::build_impl(Environment *env,Ptree *loc,const gc_string &name, InstrField *o,bool is_reserved)
  {
    InstrField *ifield = NULL;
    if(o) {
      ifield = new InstrFieldImpl(env,loc,name,*o);
    } else { // This can happen only for variable width fields which encode opcode
      ifield = new InstrField(env,loc,name);
      ifield->_pseudo = false;      
      ifield->_complement = true;
    }
    ifield->_generated = true;
    ifield->_reserved = is_reserved;
    return ifield;
  }
				   

  rnumber::RNumber InstrField::setValue(uint64_t val, unsigned width) const
  {
    RNumber result(0,width);
    RNumber ones(0,width);
    ones.invert();    
    // Iterate backwards so that we can take bits from the right.
    RevForEach(_bits,i) {
      int s = i->lo(width);
      int e = i->hi(width);
      int swidth = e - s + 1;
      // Mask for getting the portion of the value we want.
      RNumber mask = ~(ones << swidth);
      RNumber tmp_val(val,64);
      RNumber v = (RNumber(tmp_val,mask.size()) & mask);
      // Consume these bits that we've just used.
      val >>= swidth;
      // Add in the value we just got.
      result |= (v << (width - e - 1)); 
    }
    return result;
  }

  bool InstrField::has_default_value() const
  {
    return _default_value != NoDefault;
  }


  int32_t InstrField::default_value() const
  {
    int32_t ret = -1;
    if (_default_value == NoDefault){
      return ret;  
    }
    if (_enums.empty()){
      return _default_num_value;
    }
    else{
      ForEach(_enums,i){
        if (i->first ==_default_value){
          return ret+1;	
        }
        ret++;
      }   
    }
    return ret;
  }

  // Width of mask equals the maximum instruction width. 
  void InstrField::compute_mask(unsigned max_instr_width) {
    unsigned m = max_instr_width;
    RNumber ones(0,m);    
    ones.invert();
   
    _mask.resize(m);
       
    ForEach (_bits,i) {
      RNumber mask(ones);
      mask = (mask << (i->lo(m))) >> i->lo(m);
      mask = (mask >> (max_instr_width-1-i->hi(m))) << (max_instr_width-1-i->hi(m));
      _mask |= mask;
    }
  }
  
  bool InstrField::legal_var(Ptree *p, const gc_string &name) const
  {
    return (name == "ThisField" || name == "Index" ||  PtreeUtil::Match(p,"[bits ( [ %* , %* ] ) ]"));
  }

  const InstrField *InstrField::getField(const gc_string &name) const 
  {
    auto it = _instrfields.find(name);
    return (it == _instrfields.end()) ? 0 : it->second;
  }

  // We sort by the enum value, but we prioritize non-reserved enum strings or
  // "reserved" so that they'll show up first, and thus we'll ignore the
  // reserved status.
  bool sort_enum_item(const pair<gc_string,int> &x,const pair<gc_string,int> &y)
  {
    if (x.second == y.second) {
      if (x.first == ReservedEnum && y.first != ReservedEnum) {
        return false;
      } else if (x.first != ReservedEnum && y.first == ReservedEnum) {
        return true;
      }
    }
    return x.second < y.second;
  }

  void InstrField::process_syntax(Core &core)
  {
    if (!_syntax) {
      return;
    }
    const InstrField *f = is_alias() ? _alias->_target : this;

    Ptree *errloc = (_syntax->_orig->Number()) ? (*(_syntax->_orig))[0] : loc();
    if (f->_instrfields.empty()) {
      PError(errloc,"No nested fields defined.");
    }
    bool output_field = false;
    ForEach(f->_instrfields,n) {
      const InstrField &f = *(n->second);
      if ((output_field = !f._fields.empty())) {
        break;
      }
    }
    if (!output_field) {
      PError(errloc,"No nested output fields defined.");
    }

    // We're implicitly enumerated if we're table-based and the syntax string
    // just specifies table-elements as the instruction fields, e.g.
    //
    // syntax = ("%f:%f:%f",Foo(0),Foo(1),Foo(2));
    bool implicit_enum = false;
    UIntVect tbl_indices;

    for(unsigned i = 1; i < _syntax->_orig->Number(); ++i) {
      Ptree *p = (*(_syntax->_orig))[i];
      if (Ptree *tm = get_func_call(p,0)) {
        // If it's a function call, then it must have one constant argument
        // which must have a valid range wrt. the table.
        if (!is_constrained()) {
          PError(errloc,"Table-mapping references are only allowed when a table is defined.");
        }
        if (get_func_call_numargs(tm) != 1) {
          PError(errloc,"For table-mappings, only one argument may be specified.");
        }
        unsigned index = get_uint(get_func_call_arg(tm,0),"table-mapping index.",env());
        tbl_indices.push_back(index);
        if (index >= _table.entry_size()) {
          PError(errloc,"Table-mapping index is larger than the size of the table entries.");
        }
        implicit_enum = true;
      }
      else if (const char* name = get_simple_ident(p,"syntax specifier.")) {
        // Not a function, so it has to be an identifier.  If we've already seen
        // table-mappings, then we don't allow anything else.
        if (implicit_enum) {
          PError(errloc,"You cannot mix table-mappings and nested fields.");
        }
        const InstrField *tf = f->getField(name);
        if (!tf) {
          PError(errloc,"Bad field name in syntax specifier: " << name);
        }
        if (tf->fields().empty()) {
          PError(errloc,"Non output field name in syntax specifier: " << name);
        }
        _syntax->_items.push_back(make_pair(Field(gc_strdup(name)),-1));
        _syntax->_items.back().first._field = tf;
      } 
    }

    // process format - this code is an adaptation of Instr::process_format
    gc_string nfmt;
    nfmt.reserve(_syntax->_fmt.size());
    unsigned count = 0;
    gc_string::iterator siter = _syntax->_fmt.begin();
    gc_string::iterator send  = _syntax->_fmt.end();
    while(siter != send) {
      if (*siter == '%') {
        ++siter;
        // Skip this check if we're implicitly creating enumerated items.  We'll
        // delete the syntax item when done.
        if (!implicit_enum && count >= _syntax->_items.size()) {
          PError(errloc,"Too many format codes were found than fields listed in syntax field.");
        }
        // we have a control code. Output the appropriated display routine.
        bool skip = false;
        switch (*siter) {
        case 'f':
          break;
        case 'p': // These are not supported in instruction field format
        case 'i':
          PError(errloc,"Bad format code: " << *siter);
          break;
        case '%':
          // just print %
          nfmt += *siter;
          skip = true;
          break;
        default:
          PError(errloc,"Bad format code: " << *siter);
        }
        if (!skip) {
          nfmt += (char)(Syntax::Offset + count);
          ++count;
        }
        ++siter;
      } else {
        // Non-control code- collect gc_string up to the next control code and
        // display that.
        while (siter != send && *siter != '%') {
          nfmt += *siter;
          ++siter;
        }
      }
    }
    _syntax->_fmt = nfmt;
    
    // Now look for the special case where we're basically just creating
    // enumerations.
    if (implicit_enum) {
      if (tbl_indices.size() != _table.entry_size()) {
        PError(errloc,"The number of table mappings in the syntax string does not match the table entry-size.");
      }
      // OK, we look good, so create our enumerations.  We add on to any existing enums.
      addImplicitEnums(tbl_indices);
      // Now we have to sort the enums so that other generators work correctly;
      // different strings with the same value have to be adjacent.
      _enums.sort(sort_enum_item);
      // We don't need the syntax item any more, since we're just using
      // enumerations.
      delete _syntax;
      _syntax = 0;
    }
  }

  // Given a list of table-index mappings extracted from the original syntax
  // string, we generate implicit enums by taking the table data and inserting
  // it into the appropriate field locations in the syntax string.
  void InstrField::addImplicitEnums(const UIntVect &tlb_indices)
  {
    // Create an output pattern from the syntax string.
    gc_string pfx = (_regfile) ? _regfile->prefix() : "";
    gc_ostringstream ss;
    unsigned index = 0;
    ForEach(_table.entries(),t_iter) {
      const InstrFieldTable::InstrFieldTableEntry &entry = *t_iter;
      int counter = 0;
      ForEach(_syntax->_fmt,i) {
        if (_syntax->isfield(*i)) {
          ss << pfx << entry[counter++];
        } else {
          ss <<  *i;
        }
      }
      _enums.push_back(make_pair(ss.str(),index++));
      ss.str("");
    }
  }

 
  void InstrField::finalize(Core &core, MsgLog &ml, InstrFieldMMap &nested)
  {
    finalize(core,ml);
    ForEach(_instrfields,i) {
      nested.insert(make_pair(_name,i->second));  
    }
    process_syntax(core);
  }

  void InstrField::finalize(const Core &core, MsgLog &ml, InstrFieldHash &fields,const gc_string &outer_name)
  {
    const InstrField *outer = core.get_instrfield(outer_name);
    finalize(core,ml,outer->width());
    
    gc_string full_name = outer_name + "." + _name;
    fields.insert(make_pair(full_name,this));
  }



  void InstrField::check_dims(const Core &core,MsgLog &ml) 
  {
    // Some of these flags may not be set for nested fields.
    // This is checked in the finalize nested stage

    if (_complement) {
      //   variable fields have no width/size or nested fields. 
      if (_size != 0 || _width != 0) {
        PError(loc(),"Complementary fields have no width/size.");
      }
      if (!_instrfields.empty()) {
        PError(loc(),"Complementary fields have no nested fields.");
      }
    } 
    else if (_pseudo) { // non-complementary pseudo field
      if (_width == 0) {
        if (_encap_field) {
          _width = _encap_field->_size;
        } else if (_size != 0) {
          _width = _size;
        } else if (_indexed !=0) {
          _width = _indexed*core.parallel_execution();
          _size  = _indexed;
        } else {
          PError(loc(),"Pseudo instruction field width is undefined.");
        }
      } 
      if (_size == 0) {
        if (is_constrained()) {
          _size = _table.size();
        } else {
          _size = (_indexed !=0) ? _indexed : _width;
        }
      }
    } else { // non-pseudo field
      if (_complement) {
        PError(loc(),"Non-pseudo complementary instruction field.");
      } else {
        if (_width == 0 && _size == 0 && _indexed == 0) {
          PError(loc(),"Instruction field's bits are undefined.");
        } else if (_size == 0) {
          if (is_constrained()) {
            _size = _table.size();
          } else {
            _size = (_indexed !=0) ? _indexed : _width;
          }
        }
      }
      if (_width > 64) {
        PError(loc(),"Instruction field width larger then 64 bits.");
      } 
      if (_size > 64) {
        PError(loc(),"Instruction field size larger then 64 bits.");
      } 
    }
    if (_indexed != 0) {
      if (!_prefix) {
        PError(loc(),"Non-prefix indexed field.");
      }
      if ((_width%_indexed) != 0) {
        PError(loc(),"Indexed-field width is not an integer-divisor of instruction field width.");
      }
    }
   
  }
  void InstrField::check_bits(unsigned &width, unsigned maxWidth)
  {
    // Make sure that none of the bits in the bits array overlap.
    // This algorithm is O(n^2), but since we will generally have very
    // few sequences (usually just 2), it doesn't matter very much.
    width = 0;
    ForEach (_bits,i) {
      for (auto j = _bits.begin(); j != i; ++j) {
        if (i->intersect(*j)) {
          PError(loc(),"Overlapping bit ranges for instruction field.");
        }
      }
      if (maxWidth  > 0 && (i->lo(maxWidth) >= (int)maxWidth || i->hi(maxWidth) >= (int)maxWidth)) {
        PError(loc(),"Nested instruction field bits exceeds outer field's width.");
      }
      width += i->width();
    }
    _highbit = _bits.highbit();
    
  }

  void InstrField::finalize_fields(const Core &core, MsgLog &ml) 
  {
    bool ref_encap = false;
    ForEach(_fields,i) {
      Field &f = *i;
      
      if (f._type == Field::Range && (!f._name || f._name == _name)) {
        if (!f._field) {
          f._field = this;
        }
        f._name = f._field->name().c_str();
      } else {
        // We allow a reference to the parent field in order to use the parent's
        // table.  We also allow an outer field to refer to children, in order
        // to "build-up" a larger field from constituent components.
        auto j = instrfields().find(f._name);
        if (j == instrfields().end()) {
          if (_encap_field && f._name == _encap_field->name()) {
            f._field = _encap_field;
            ref_encap = true;
          } else {
            PError(loc(),"Unkown instruction field '" << f._name << "'.");
          }
        } else if (!(j->second->_fields.empty())){
          PError(loc(),"Recursive fields definition.");
        } else {
          f._field = j->second;
        }
      }
      if (f._type == Field::Range) {
        if (f._field->is_constrained()) {
          PError(loc(),"Constrained field '" << f._field->name() << "' take only one index." );
        }
        else if (f._range.max() > f._field->size()-1) {
          PError(loc(),"Index out of field '" << f._field->name() << "' range.");
        }
      } 
      else if (f._type == Field::RefIdx) {
        if (f._field->is_constrained()) {
          const InstrFieldTable &t = f._field->table();
          if ((int)t.entry_size() - 1 < (int) f._value) {
            PError(loc(),"Out of range index for field '" << f._field->name() <<"'." );
          }
          f._type = Field::TblIdx;
        } 
        else if (f._value > f._field->width()) {
          PError(loc(),"Out of range index for field '" << f._field->name() << "'.");
        } else { // It is actually a range
          f._type = Field::Range;
          f._range = BitPair(f._value,f._value);
          f._value = 0;
        }
      }
    }
    // If our fields list references the parent as a table reference, then we
    // only allow a single item, since it's not well specified how to pack
    // multiple items together (we don't know how many bits to use for the table
    // reference's results).
    if (ref_encap && _fields.size() > 1) {
      PError(loc(),"If the parent field is referenced, then 'fields' may only contain a single item.");
    }
  }

  void InstrField::finalize_alias(Core &core,MsgLog &ml) 
  {
    const InstrField *target = const_core.get_instrfield(_alias->_target_name);
    if (!target) {
      PError(loc(),"Unknown instruction-field alias target:  " << _alias->_target_name);
    }
    if (target->name() == name()) {
      PError(loc(),"Instruction field cannot be alias to itself.");
    }
    if (!target) {
      PError(loc(),"Target instruction field does not exist.");
    }
    if (!_bits.empty()) {
      PError(loc(),"Bits are not defined for instruction field alias.");
    }
    if (_width) {
      PError(loc(),"Width is not defined for instruction field alias.");
    }
    if (_pseudo) {
      PError(loc(),"The 'pseudo' key is not defined for Instruction field alias.");
    }
    if (_parm) {
      PError(loc(),"Parm field can not be alias.");
    }
    if (_prefix) {
      PError(loc(),"The 'prefix' key is not defined for Instruction field alias.");
    }
    if (_shift) {
      PError(loc(),"Shift is not defined for instruction field alias.");
    }
    if (_offset) {
      PError(loc(),"Offset is not defined for instruction field alias.");
    }
    if (_action) {
      PError(loc(),"Action is not defined for instruction field alias.");
    } 
    handle_default_value();
    _alias->_target = target;
    process_syntax(core);
  } 
 
  void InstrField::handle_default_value() {
    if (_default) {
      if (is_enumerated()) {
        _default_value = get_str(_default,"default value.");
      } else {
        _default_num_value = get_uint(_default,"default value.",env());
      }
    } else {
      _default_value = NoDefault;
    }
  }

  



  void InstrField::finalize(const Core &core,MsgLog &ml, unsigned maxWidth) 
  {
    _bits.finalize(loc());
  
    // The instruction-field name should not conflict with any other resources,
    // since that could create ambiguities within action code.
    if (core.is_resource(name())) {
      PError(loc(),"The instruction field's name conflicts with another resource defined in this core.");
    }
    if (_pseudo && !encap_field() && !_bits.empty()) { /// nested fields may have bits which are relative to outer field
      PError(loc(),"Pseudo instruction fields have no bits.");
    }
    
    if (_complement || _parm) {
      _pseudo = true;
      
      if (!_bits.empty()) {
        if (_parm) {
          PError(loc(),"Parm instruction fields have no bits.");
        } else {
          PError(loc(),"Complementary instruction fields have no bits.");
        }
      }
    }

    if (_bits.empty() && !_pseudo &&  (_fields.empty() || !_encap_field)) {
      PError(loc(),"Non-pseudo field without bits or fields.");
    }
    handle_default_value();

    unsigned width = 0;
    check_bits(width,maxWidth);
    if (!_width) {
      _width = width;
    } else if (_pseudo && _width < width) {
      PError(loc(),"Specified width is smaller then actual width."); 
    }
    const char* ref = _ref ? get_str(_ref,"instruction field reference1.") : 0;
    if (!ref && encap_field()) {
      ref = encap_field()->_ref ? get_str(encap_field()->_ref,"instruction field reference2.") : 0;
    }
    
    // Resolve any references, if we're not an immediate field.
    switch (_type) {
    case tImm:
      if (ref) {
        if ( (_regfile = core.get_regfile(ref)) ) {
          _type = tRegFile;
        } 
        else if ( (_memory = core.get_memory(ref)) ) {
          _type = tMem;
        } 
        else {
          PError(loc(),"A reference object was specified, but field is of immediate type.");
        }
      }
      break;
    case tRegFile:
      if (!ref) {
        PError(loc(),"Field is of register-file-type but no reference was specified.");
      } else {
        if (! (_regfile = core.get_regfile(ref))) {
          PError(_ref,"Unknown register file:  " << _ref);
        }
      }
      break;
    case tMem:
      if (!ref) {
        PError(loc(),"Field is of memory-type but no reference was specified.");
      } else {
        if (! (_memory = core.get_memory(ref))) {
          PError(_ref,"Unknown memory:  " << _ref);
        }
      }
      break;
    case tInstr:	// For micro-instructions
      if (!ref) {
        PError(loc(),"Field is of instruction table type but no reference was specified.");
      }
      break;	  
    case tRefField: // This type is not user defined
      if (!(_pfield = core.get_instrfield(_pfield_name))) {
        PError(loc(),"Unknown instruction field.");
      }
      if (!_prefix && !_assembler) {
        PError(loc(),"Only prefix and assembler fields can reference fields.");
      }
      if (_complement) {
        PError(loc(),"Field can not have variable width.");
      }
      if (has_default_value() &&_pfield->has_default_value()) {
        if (default_value() != _pfield->default_value()) {
          PError(loc(),"Default value inconsistent with target field.");
        } 
      }
      if (_prefix) {
        if (!_pfield->_complement) { // target defines interface
          if (_indexed) {
            if (_indexed != _pfield->_indexed && _indexed != _pfield->_width) {
              PError(loc(),"Index information inconsistent with target field."); 
            } else if (_width && _width != _pfield->_width) {
              PError(loc(),"Width inconsistent with target field."); 
            }
          } else if (_width != _pfield->_width) {
            PError(loc(),"Width inconsistent with target field."); 
          } else if (_size != _pfield->_size) {
            PError(loc(),"Size inconsistent with target field."); 
          }
        } else if (!_pfield->_assembler) {
          PError(loc(),"Variable width target of prefix field must be assembler field."); 
        }
        const StrList &tblks = _pfield->_blks;
        if (_blks.empty() != tblks.empty()) {
          PError(loc(),"Block inconsistent with target field.");
        }
        ForEach(_blks,b) { // O(n2), but number of blocks is very small 
          if (find(tblks.begin(),tblks.end(),*b) == tblks.end()) {
            PError(loc(),"Block inconsistent with target field.");
          }
        }
      }
      break;
    }

    // Implementations of pseudo fields will require this field.  We also need
    // it if we're referencing an instruction table, for micro-ops, since we
    // build tables later.
    if (!_pseudo && _type != tInstr) {
      _ref = 0; 
    }

    if (_addr_type != aNone) {
      if ( _type == tRegFile ) {
        PError(loc(),"Field cannot be an address type and a register-file type.");
      } else if ( _parm ) {
        PError(loc(),"Address type not defined for parm field.");
      }
    }

    // check for uniqueness, for reserved only entries  
    check_enums(loc(),_enums);

    if (_parm) {
      if (_prefix) {
        PError(loc(),"Parm field can not be a prefix field.");
      }
      if (!_ranges.empty()) {
        PError(loc(),"Valid ranges are not defined for parm field.");
      }
      if (!_masks.empty()) {
        PError(loc(),"Valid masks are not defined for parm field.");
      }
      if (!_table.entries().empty()) {
        PError(loc(),"Table is not defined for parm field.");
      }
      if (!_blks.empty()) {
        PError(loc(),"Blks are not defined for parm field.");
      }
      if (!_instrfields.empty()) {
        PError(loc(),"Parm field can not have nested fields.");
      }
      if (_action) {
        PError(loc(),"Action is not defined for parm field.");
      }
      if (_indexed) {
        PError(loc(),"Parm field can not be indexed.");
      }
      if (_alias) {
        PError(loc(),"Alias is not defined for parm field.");
      }
      if (_shift) {
        PError(loc(),"Shift is not defined for parm field.");
      }
      if (_offset) {
        PError(loc(),"Offset is not defined for parm field.");
      }      
      if (_inverted) {
        PError(loc(),"Inverted is not defined for parm field.");
      }
      if (_assembler) {
        PError(loc(),"Assembler is not defined for parm field.");
      }
      if (_overlay) {
        PError(loc(),"Overlay is not defined for parm field.");
      }
      if (_syntax) {
        PError(loc(),"Syntax is not defined for parm field.");
      }
      if (_display != Default) {
        PError(loc(),"Display is not defined for parm field.");
      }
    }

    if (!_ranges.empty()) {
      ForEach(_ranges,i) {
        //left should be smaller than right
        if (i->first > i->second) {
          PError(loc(),"Valid ranges are not properly defined.");
        }
        if (!_signed) {
          if (i->first < 0 || i->second < 0) {
            PError(loc(),"Negative range for unsigned field."); 
          }        
          unsigned int mx = ( 1 << _width ) - 1;       
          if (i->first > mx || i->second > mx) {
            AddWarning(ml,loc(),"Valid range is greater then field.");
          }
        }
      }
    }


    _table.finalize(core,ml);
    if (has_default_value()) {
      if (is_enumerated()) {    
        if (_default_value == ReservedEnum) {
          PError(loc(),"Invalid default value.");
        }
        //check that default value is in enum      
        if (find_if(_enums.begin(), _enums.end(),
                    equal_1st_in_pair<gc_string,int>(_default_value)) == _enums.end()){
          PError(loc(),"Invalid default value.");
        }    
      } else { // check numeric default value
        ForEach(_ranges,r) {
          if (_default_num_value < r->first || _default_num_value > r->second) {
            PError(loc(),"Default value out of range for field.");
          }
        }
        ForEach(_masks,m) {
            if ((_default_num_value & m->first) != (m->first & m->second)) {
                PError(loc(),"Default value does not conform with valid mask.");
            }
        }
      }
    }
    //if we have empty gc_string in enumerated field than default must be empty gc_string
    bool has_empty_gc_string = (find_if(_enums.begin(), _enums.end(), 
                                     equal_1st_in_pair<gc_string,int64_t> (EMPTY_STRING))) != _enums.end();
    if (is_enumerated() && has_default_value() && 
        _default_value != EMPTY_STRING && has_empty_gc_string) {   
      PError(loc(),"Invalid default value."); 
    }

    check_dims(core,ml);

    if (_blks.size() > 1) {
      if (_prefix) {
        if (_blks.size() < core.blocks().size()) {
          PError(loc(),"Field partially associated with core's blcoks.");
        }
      } else {
        PError(loc(),"Field associated with more than one block.");
      }
    }
    
    ForEach(_blks,blk) {
      if (!core.is_block(*blk)) {
        PError(loc(),"Undefined block " << *blk << ".") ;
      }
    }
    ForEach(_instrfields,i) {
      i->second->finalize_nested(core,ml,this);
    }
    finalize_fields(core,ml);
  }

  // Deal with the instruction-table type, check the validity of the reference  
  void InstrField::finalize(const Core &core) 
  {
    if (_type == tInstr) {     // For micro-instructions
      if (!_ref) return;       // An error is already logged if this doesn't
                               // exist, so just skip finalization here.

      _instrtablename = _ref->ToString();
      if (!(_instrtable = core.get_instrtable(_instrtablename))) {
        PError(_ref, "Unknown instruction table: " << _instrtablename);
      } else {
        const_cast<InstrTable*>(_instrtable)->set_embedded(); // Indicate the instruction table is for micro instructions

        // Skip if the table is empty- we'll assume that the user is adding
        // something incrementally and just hasn't defined any instructions in
        // this table yet.
        if ( !_instrtable->empty()) {
          // The width of instruction-type field should match the maximum width of the instructions in the related table
          if ( _instrtable->getInstrWidth() != _width) { 
            PError(loc(), "Field width of " << _width << " does not match the width (" <<_instrtable->getInstrWidth()
                   << ") of instructions associated with table " << _instrtablename << "."); 
          }
        }
      }

      // Now we're done with it.
      _ref = 0;
    }
  }    
  
  
  void InstrField::finalize_action(Core &core)
  {
    if (_action)  {
      if (_indexed) {
        get_func(_action,1,"indexed field action.");    
      } else {
        get_func(_action,0,"field action.");    
      }
      
      if (_width > 32) {
        PError(loc(),"Action code not supported fields with more then 32 bits.");
      }

      if (!_fields.empty()) {
        PError(loc(),"Field with both fields and action code defined.");
      }

      CheckCode cc(_env,core,false);
      cc.check_code_filter(_action);
      _action = cc.translated();
      ForEach(cc.escapes(),i) {
        if (!legal_var(i->first, i->second) && !getField(i->second)) {
          PError(i->first,"Unknown variable/resource found in action code:  " << i->first);
        }
      }

      if (_indexed) {
        if (!arg_is_uint(_action,0)) {
          PError(_action,"Argument of indexed field action must be an unsigned integer.");
        }
      }
    }
  }

  void InstrField::finalize_nested(const Core &core,MsgLog &ml,InstrField *out)
  {
    _encap_field = out;

    if (_pseudo) {
      PError(loc(),"Pseudo nested fields are not allowed.");
    }
    if (_complement) {
      PError(loc(),"Complementary nested fields are not allowed.");
    }
    if (_prefix) {
      PError(loc(),"Prefix nested fields are not allowed.");
    }
    if (!_blks.empty()) {
      PError(loc(),"Nested fields have no blk key.");
    }


    if (!_instrfields.empty()) {
      PError(loc(),"Too deep nesting.");
    }

    _bits.finalize(loc());
    if (!_fields.empty() && !_bits.empty()) {
      PError(loc(),"Nested fields can not have both fields and bits keys.");
    }

    if (!_width) {
      ForEach(_bits,i) {
        _width += i->width();
      }
    }

    _table.finalize(core,ml);

    bool all_range = true;
    unsigned size = 0;
    ForEach(_fields,i) {
      Field &f = *i;
      if (f._type == Field::Range) {
        f._field = out;
        size += f._range.width();
      } else {
        all_range = false;
      }
    }
    if (all_range) {
      _size = size;
    }

    check_dims(core,ml);

    _prefix = out->_prefix;
    
    copy(out->_blks.begin(),out->_blks.end(),back_inserter(_blks));

    _pseudo = out->_pseudo;
  }

  ostream &operator<<(ostream &o, const InstrField::Field &f)
  {
    switch (f._type) {
    case InstrField::Field::Ref: 
      o << f._field->name();
      break;
    case InstrField::Field::Range:
      if (f._field) {
        o << (f._field)->name() << "(" << f._range.l() << "," << f._range.r() << ")";
      } else {
        o << "(" << f._range.l() << "," << f._range.r() << ")";
      }
      break;
    case InstrField::Field::TblIdx:
    case InstrField::Field::RefIdx:
      o << f._field->name() << "(" << f._value << ")";
      break;
    }
    return o;
  }

  // This prints the syntax item.  It reconstructs the gc_string from the
  // field information stored inside of it.
  ostream &operator<<(ostream &o,const InstrField::Syntax &s)
  {
    for (gc_string::const_iterator siter = s._fmt.begin(); siter != s._fmt.end(); ++siter) {
      if (s.isfield(*siter)){
        o << "%f";
      }
      else{  
        o << *siter;
      }
    }
    return o;
  }
  void InstrField::print_alias(ostream &o,const gc_string &pfx) const
  {
    if (_alias) {
      o << pfx << "Alias:  " <<  _alias->_target->name() << "\n";
    }
  }


  void InstrField::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << _name;
    o << ": ";

    print_alias(o,pfx);
   
    ForEach(_bits,i) {
      o << *i << ' ';
    }
    if (_regfile) {
      o << "[regfile:  " << _regfile->name() << "] ";
    }
    if (_memory) {
      o << "[mem: " << _memory->name() << "] ";
    }
    if (_pfield) {
      o << "[instrfield: " << _pfield->name() << "] ";
    }
    if (_instrtable) {
      o << "[instr table: " << _instrtablename << "] ";
    }
    if (!_used) {
      o << "[not used] ";
    }
    o << "\n";

    gc_string npfx = indent(pfx);
    gc_string nnpfx = indent(npfx);

    switch (_display) {
    case Default:
      break;
    case Dec:
      o << npfx << "Display:  dec\n";
      break;
    case SignedDec:
      o << npfx << "Display:  signed-dec\n";
      break;      
    case Hex:
      o << npfx << "Display:  hex\n";
      break;
    case Name:
      o << npfx << "Display:  name\n";
      break;
    }

    Attrs::dump(o,npfx);

    if (_signed) {
      o << npfx << "Signed:  " << _signed;
      if (_unsigned_ub) {
        o << " (unsigned upper-bound)";
      }
      o << "\n";
    }

    if (_allow_conflict) {
      o << npfx << "Conflict allowed.\n";
    }

    if (_inverted) {
      o << npfx << "Inverted:  " << _inverted << "\n";
    }

    if (_pseudo) {
      o << npfx << "Pseudo:  " << _pseudo << "\n";
  
      if (_complement) {
        o << npfx << "Complementary:  " << _complement << "\n";
      }
      
      if (_width && _bits.empty()) {
        o << npfx << "Width:   " << _width << "\n";
      }
      
      if (_size && _size != _width) {
        o << npfx << "Size: " << _size << "\n";
      }
    }

    if (!_blks.empty()) {
      o << npfx << "Block:  ";
      printSequence(o,_blks,true);
    }

    if (!relocs().empty()) {
      ForEach(relocs(),i) {
        const Relocation *r = *i;
        o << npfx << "Reloc:  " << r->name();
        if (r->macro_op_field()) {
          o << " (macro-op field: " << r->macro_op_field()->name() << ")";
        }
        o << "\n";
      }
    }
    
    if (_prefix) {
      o << npfx << "Prefix:  " << _prefix << "\n";
    }

    if (_indexed != 0) {
      o << npfx << "Indexed:  " << _indexed << " ";
      switch (_index_endianness) {
      case Big:
        o << "(big-endian)";
        break;
      case Little:
        o << "(little-endian)";
        break;
      default:
        break;
      }
      o << '\n';
    }

    if (_reserved) {
      o << npfx << "Reserved:  " << _reserved << "\n";
    }

    if (_assembler) {
      o << npfx << "Assembler:  " << _assembler << "\n";
    }

    switch (_disassemble) {
    case dsTrue:
      break;
    case dsFalse:
      o << npfx << "Disassemble:  false\n";
      break;
    case dsPrefix:
      o << npfx << "Disassemble:  prefix\n";
      break;
    default:
      break;
    }

    if (_parm) {
      o << npfx << "Parm:  " << _parm << "\n";
    }

    switch (_addr_type) {
    case aNone:
      break;
    case aAbs:
      o << npfx << "Addr:  Absolute\n";
      break;
    case aPcRel:
      o << npfx << "Addr:  PC-relative\n";
      break;    
    }

    if (_shift) {
      o << npfx << "Shift:  " << dec << _shift << "\n";
    }

    if (_offset) {
      o << npfx << "Offset:  " << dec << _offset << "\n";
    }

    if (!_enums.empty()) {
      o << npfx << "Enums:  ";    
      bool open = false;
      ForEach(_enums,current) {
        auto next =  current;
        next++;
        //print braces for repeating values
        if (!open && next != _enums.end() && current->second == next->second) {
          o << "( ";
          open = true;     
        }      
        o << "'" << current->first << "' ";
        if (open && next != _enums.end() && current->second != next->second) {
          o << ") ";
          open = false;     
        }
      }
      if (open) {
        o << ") ";
      }
      o << "\n";
    }

    _table.dump(o,pfx);

    if (!_ranges.empty()) {
      o << npfx << "Valid ranges:  " ;
      printSequenceOfPairs(o,_ranges,true);   
    } 

    if (!_masks.empty()) {
      o << npfx << "Valid masks:  " << hex << showbase;
      printSequenceOfPairs(o,_masks,true);   
      o << dec << noshowbase;
    } 
    

    dump_doc(o,npfx,false);

    if (has_default_value()) {
      if (is_enumerated()) {
        if (_default_value != EMPTY_STRING) {
          o << npfx << "Default value :"<< "'" << _default_value << "'" << "\n";
        }
      } else {
        o << npfx << "Default value :"<< "'" << _default_num_value << "'" << "\n";  
      }
    }    

    if (_action) {
      o << pfx << "Action:  " << _action << "\n";
    }

    if (!_instrfields.empty()) {
      o << npfx << "Nested instruction fields:\n";
      ForEach(_instrfields,i) {
        i->second->dump(o,nnpfx);
      }
    }

    if (!_fields.empty()) {
      gc_string npfx = indent(pfx);
      o << npfx << "Fields:  ( ";
      printSequence(o,_fields,false);
      o << ")\n";
    }

    if (_syntax) {
      o << pfx << "Syntax:  \"" << *_syntax << "\":  ";
      ForEach(_syntax->_items,i) {
        o << " " << i->first._field->name();
      }
      o <<"\n";
    }
  }


  //
  // Information about instruction field implementation  
  // 

  InstrFieldImpl::InstrFieldImpl(Environment *env,Ptree *loc,const gc_string &name,const InstrField &trgt) :
    InstrField(env,loc,name),
    _impl(trgt)
  {
    auto tfields = trgt.fields();
    if (!tfields.empty()) {
      ForEach(tfields,f) {
        _fields.push_back(*f);
      }
      
      char *cname = gc_strdup(name);
      
      ForEach(_fields,f) {
        if (f->_name && f->_name == trgt.name()) {
          f->_name = cname;
        }
        if (f->_field && f->_field == &trgt) {
          f->_field = this;
        }
      }
    }
    _width = trgt.width();
  }
  
  void InstrFieldImpl::finalize(const Core &core, MsgLog &ml, InstrFieldHash &fields,const gc_string &outer)
  {
    fields.insert(make_pair(_name,this));
    ForEach(_impl.instrfields(),i) {
      gc_string name = _name + "." + i->second->name();
      fields.insert(make_pair(name,i->second));
    }

    unsigned width = 0;
    _bits.finalize(loc());
    check_bits(width);

    if (_impl.width()) {
      if (_impl.width() && _impl.width() < width) {
        PError(loc(),_impl.name() << ":  Specified width is smaller than actual width."); 
      }
      if  (!_impl.index() && _impl.width() > width) {
        PError(loc(),_impl.name() << ":  Specified width is larger than actual width.");
      }
    }
    _width = width;
    finalize_fields(core,ml);
  }

  
  void InstrFieldImpl::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << _name;
    o << ": ";

    ForEach(_bits,i) {
      o << *i << ' ';
    }

    gc_string pfx1 = indent(pfx);
    o << "\n" << pfx1 << "Implements:  " << _impl.name() << "\n";
  }
  

  //
  // Information about instructions base 
  // 

  // This prints the syntax item.  It reconstructs the gc_string from the
  // field information stored inside of it.
  ostream &operator<<(ostream &o,const InstrBase::Syntax &s)
  {
    for (gc_string::const_iterator siter = s._fmt.begin(); siter != s._fmt.end(); ++siter) {
      if (s.isPfield(*siter)) {
        o << "%p";
      } 
      else if (s.isFfield(*siter)){
        o << "%f";
      }
      else{  
        o << *siter;
      }
    }
    return o;
  }


  InstrBase::Syntax::Syntax(const Syntax &o) :
    _fmt(o._fmt),
    _items(o._items.begin(),o._items.end())
  {
    _orig = new (GC) PtreeArray;
  
    ForRange(o._orig->Number(),i) {
      _orig->Append((*o._orig)[i]);
    }
    // _orig = o._orig;
  }
  
  const char* InstrBase::TmpFields::get_name(const char* name) const
  {
    auto it = _impls.find(name); 
    return it == _impls.end() ? name : it->second.c_str(); 
  }

  const char* InstrBase::TmpFields::get_inv_name(const char* name) const
  {
    auto it = _ifcs.find(name); 
    return it == _ifcs.end() ? name : it->second.c_str(); 
  }

  void InstrBase::TmpFields::finalize(const Core &core) 
  {
    StrStrMap nimpls;
    StrStrMap nifcs;
    ForEach(_impls,i) {
      const InstrField *tgt= core.get_instrfield(i->first);
      if (tgt) {
        const gc_string &s = i->first;
        const gc_string &t = i->second;
        ForEach(tgt->instrfields(),f) {
          gc_string ssub = s + gc_string(".") + f->first;
          gc_string tsub = t + gc_string(".") + f->first;
          nimpls.insert(make_pair(ssub,tsub));
          nifcs.insert(make_pair(tsub,ssub));
        }
      }
    }
    ForEach(nimpls,i) {
      _impls.insert(*i);
    }
    ForEach(nifcs,i) {
      _ifcs.insert(*i);
    }
  }

  void InstrBase::TmpFields::add_impl(const gc_string &s, const gc_string &t) 
  {
    _impls.insert(make_pair(s,t)); 
    _ifcs.insert(make_pair(t,s)); 
  }

  //processes format gc_string of syntax and dsyntax
  unsigned int Instr::process_format(Syntax* syntax, gc_string name,bool require_name_first) 
  {       
    
    if (!syntax) {
      return 0;
    }
    Ptree *errloc = (syntax->_orig->Number()) ? (*syntax->_orig)[0] : loc();
    gc_string nfmt;
    nfmt.reserve(syntax->_fmt.size());
    unsigned count = 0;
    gc_string::iterator siter = syntax->_fmt.begin();
    gc_string::iterator send  = syntax->_fmt.end();
    enum { inP,afterP} state;

    // This is to indicate that we come across what's potentially an instruction
    // name- either %i or non-whitespace, non-format code chars.  We allow a %p
    // sequence to precede an instruction name, but not %f.
    bool iname = false;
    bool handled_name = false;

    state = inP;
    while (siter != send) {        
      if (*siter == '%') {
        ++siter;
        if (*siter != 'i' && (count >= syntax->_items.size())) {
          PError(errloc,"Too many format codes were found than fields listed in the syntax field.");
        }
        if (iname && *siter != 'i') {
          // If we get a non-%i format code, then we consider the name handling
          // to be complete.  Anything else is just decoration syntax on the
          // name.
          handled_name = true;
        }
        // We have a control code.  Output the appropriate display routine.
        bool skip = false;
        switch (*siter) {
        case 'f':
          if (!iname) {
            if (require_name_first) {
              PError(errloc,"Found a field control code before the instruction name.");
            } else {
              // Consider the name to be done, since we don't require one.
              handled_name = true;
            }
          }
          // Instruction field. 
          if (state == inP){
            state = afterP; 
          }	        
          break;
        case 'p': 
          // Permutable instruction field.	 
          if (state == afterP){
            PError (errloc,"Bad format code:  " << *siter);
          }
          else{
            state = inP;
          }	  
          break;
        case 'i':
          // Instruction name.
          // Add the instruction's name to the format string.
          syntax->_in_begin = nfmt.size();
          nfmt += name;
          // One less so that we can always bump the end pointer in order to
          // handle the non %i case.
          syntax->_in_end = nfmt.size()-1; 
          skip = true;
          iname = true;
          handled_name = true;
          break;
        case '%':
          // Just print "%".
          nfmt += *siter;
          skip = true;
          break;
        default:
          PError (errloc,"Bad format code:  " << *siter);
        }
        if (!skip) {	  
          nfmt += (char)(Syntax::Offset + count);	
          ++count;
          if (*siter == 'p'){
            syntax->_perm_end++;
            if (!iname) {
              syntax->_pfx_end++;
            }
          }
        }
        ++siter;
      } 
      else {
        // Non-control code- collect gc_string up to the next control code and
        // display that.
        while (siter != send && *siter != '%') {
          if (!isspace(*siter)) {
            if (!handled_name) {
              if (!iname) {
                syntax->_in_end = syntax->_in_begin = nfmt.size();
              } else {
                syntax->_in_end++;
              }
              iname = true;
            }
          } else if (iname) {
            handled_name = true;
          }
          nfmt += *siter;
          ++siter;
        }
      }
    }  
    syntax->_fmt = nfmt;
    syntax->_read_count = count;
    // Unconditional bump for the end-pointer, if we did actually get something.
    if (iname) {
      syntax->_in_end++;
    }
    // Make sure that we actually got a name.
    if ( !iname && require_name_first) {
      PError(errloc,"Could not extract an instruction name from the syntax string.");
    }
    return count;
  }
  
  //if need to invert
  bool InstrBase::Syntax::have_to_invert() const { 
    auto i = _items.begin();
    ForEach(_items,i) { 
      auto fp = i->first;
      if ( fp._type == Instr::Field::Expr ||
           (fp._type == Instr::Field::Value &&
            !fp._field->is_constrained())) {
        return true;  
      
      }
    }
    return false;
  };

  InstrBase::InstrBase(Environment *env, Ptree *loc, const gc_string &name) :  
    Attrs(env,loc),
    Named(name),
    _action(0),
    _syntax(0),
    _dsyntax(0),
    _is_micro_op(false),
    _mo_count(0),
    _branch_type(BranchNone)
  {
  }


  InstrBase::InstrBase(const InstrBase &o, const gc_string &name) :
    Attrs(o.env(),o.loc()),
    Named(name),
    _init_all(o._init_all),
    _fields(o._fields),
    _opcs(o._opcs),
    _all(o._all),
    _action(o._action),
    _syntax(o._syntax ? new Syntax(*o._syntax) : 0),
    _blks(o._blks),
    _is_micro_op(o._is_micro_op),
    _mo_count(o._mo_count),
    _branch_type(BranchNone)
  {
  }

  InstrBase::TmpField *InstrBase::get_tmpfield(InstrBase::FieldBase *f)
  {
    return dynamic_cast<InstrBase::TmpField *>(f);
  }

  InstrBase::Field *InstrBase::get_field(InstrBase::FieldBase *f)
  {
    return dynamic_cast<InstrBase::Field *>(f);
  }

  InstrBase::Field InstrBase::getField(unsigned x) const
  {
    if (x < sizeFields()) {
      return _fields.at(x);
    }
    return _nestedfields.at(x-sizeFields());
  }

  InstrBase::Field InstrBase::getOpc(unsigned x) const
  {
    if (x < sizeOpcs()) {
      return _opcs.at(x);
    }
    return _nestedopcs.at(x-sizeOpcs());
  }

  InstrBase::FieldBase *InstrBase::find_bfield(const gc_string &nm)
  {
    ForEach(_init_all,i) {
      if ( (*i)->_name && (nm == (*i)->_name)) {
        return *i;
      }
    }
    return 0;
  }

  int InstrBase::getFieldOrParent(const gc_string &nm) const
  {
    int index = getField(nm);
    if (index >= 0) return index;

    // Otherwise, search through fields manually, matching against parent names if the fields are indices.
    int count = 0;
    ForEach(_fields,i) {
      const InstrField *f = i->_field;
      if (f->interface() && f->interface()->name() == nm) {
        return count;
      }
      ++count;
    }

    return -1;
  }
  
  int InstrBase::findField(const StrHash &fieldhash,const gc_string &nm) const
  {
    auto iter = fieldhash.find(nm);
    if (iter == fieldhash.end()) {
      if (_tmpfields) {
        if (const char* nname = _tmpfields->get_name(nm.c_str())) {
          iter = fieldhash.find(nname);
        }
      }
    }
    return (iter == fieldhash.end()) ? -1 : iter->second;
  }

  // Take all fields out of the initial list and put them into
  // the opcode or field array, plus the "all" array.  By the
  // time this is called, no tmpfields should be left.
  void InstrBase::distr_fields(Fields &fields,Fields &opcs,TmpFields *tmpfields,BFields &src,
                               bool allow_opc_impl,bool drop_tmpfields)
  {
    ForEach(src,i) {
      if (Field *f = get_field(*i)) {
        if (f->isOpcode() || f->isOpcodeImp()) { 
          opcs.push_back(*f);
        } else { 
          if (!f->_name) {
            PError(loc(),"Integer-only fields not allowed in the field specification.");
          }
          fields.push_back(*f);
        }
      } else if (tmpfields) {
        tmpfields->push_back(*(get_tmpfield(*i)));
      } else if (!drop_tmpfields) {
        PError(loc(),"No temporary fields should be left at this point.");
      }
    }
    src.clear();
  }

   
  // Bit order
  struct FieldOrder {
    bool operator()(const InstrBase::Field &x,const InstrBase::Field &y) const {
      if (x._field->is_pseudo() || y._field->is_pseudo()) {
        return false;
      }
      else {    
        if (BitPair::is_big_endian()) {
          return (x._field->highbit() < y._field->highbit());
        } else {
          return (x._field->highbit() > y._field->highbit());
        }
      }
    };
  };

  // This converts the original syntax description (a ptree gc_string and a list of fields)
  // into the Instr::Syntax data structure.
  void InstrBase::ptree_to_syntax(const Core &core, Syntax* syntax)
  {
    using namespace PtreeUtil;
    // This scans over the field list, making sure that all fields are valid and that the
    // proper number exist.
    
    for (unsigned i = 1; i < syntax->_orig->Number(); ++i) {
      Ptree *p = (*syntax->_orig)[i],*f,*a,*v;  	
      const InstrField* targetField = 0;
      const InstrField* outerField = 0;
      //simple identifier
      if (const char* name = get_simple_ident(p,0)) {	  
        targetField = core.get_instrfield(gc_string(name)); 
        syntax->_items.push_back(make_pair(InstrBase::Field(name),-1));
      }  // subfield expression a.b(1)
      else if(Match(p,"[ [%? . %?] ( [ %? ] )]",&f,&a,&v)) {
        const char *outer  = get_simple_ident(f,"instruction field name.");
        const char *nested = get_simple_ident(a,"nested field name.");  
        if ((outerField = core.get_instrfield(outer))) {
          targetField = outerField->getField(nested);
        }
        gc_string nm = gc_string(outer) + "." + gc_string(nested);
        char *name = gc_strdup(nm);
        unsigned arg;
        if (get_uint(arg,v,env())) {	    	  
          syntax->_items.push_back(make_pair(InstrBase::Field(name,arg),-1));  
        }
        else {
          syntax->_items.push_back(make_pair(InstrBase::Field(InstrBase::Field(name,a)),-1));
        }
      }
      else if(Match(p,"[reverse ( [ %? ] )]",&f)) {
        const char* name = get_simple_ident(f,"instruction field name.");
        targetField = core.get_instrfield(name);
        syntax->_items.push_back(make_pair(InstrBase::Field(name,Field::Reverse),-1));
      }
      else if(Match(p,"[symmetric ( [ %? ] )]",&f)) {
        const char* name = get_simple_ident(f,"instruction field name.");
        targetField = core.get_instrfield(name);
        syntax->_items.push_back(make_pair(InstrBase::Field(name,Field::Symmetric),-1));
      }  
      //Initialization X(1) or expression
      else if (Match(p,"[%? ( [ %? ] )]",&f,&a)) {	 
        const char *name = get_simple_ident(f,"instruction field name.");
        targetField = core.get_instrfield(gc_string(name));
        unsigned arg;
        const gc_string str_arg = a->ToString();
        const InstrField *alias_field = core.get_instrfield(str_arg);
        if (get_uint(arg,a,env())) {	    	  
          syntax->_items.push_back(make_pair(InstrBase::Field(name,arg),-1));  
        } else if (alias_field && alias_field->is_alias()) {
          syntax->_items.push_back(make_pair(InstrBase::Field(InstrBase::Field(name,alias_field)),-1));   
          targetField = alias_field;	  
        } else {
          syntax->_items.push_back(make_pair(InstrBase::Field(InstrBase::Field(name,a)),-1));	       
        }
      } // subfield identifier
      else if(Match(p,"[ %? . %?]",&f,&a)) { 
        const char *outer  = get_simple_ident(f,"instruction field name.");
        const char *nested = get_simple_ident(a,"nested field name.");  
        if ((outerField = core.get_instrfield(outer))) {
          targetField = outerField->getField(nested);
        }
        gc_string nm = gc_string(field_name(outer)) + "." + gc_string(nested);
        char *name = gc_strdup(nm);
        syntax->_items.push_back(make_pair(InstrBase::Field(name),-1));
      }  //unnamed expression
      else {
        syntax->_items.push_back(make_pair(InstrBase::Field(InstrBase::Field("",p)),-1));	       
      }
      syntax->_items.back().first._field = targetField;	
      syntax->_items.back().first._outer = outerField;	
    }
  }
  //checks validity of syntax
  void InstrBase::process_syntax(Core &core)
  {   
    if (_syntax) {
      InstrBase::ptree_to_syntax(core,_syntax);
      Syntax &syntax = *_syntax;
      Ptree *errloc = (syntax._orig->Number()) ? (*syntax._orig)[0] : loc();      
      ForEach(syntax._items,i){
        Field &fp = i->first;
        gc_string name(fp._name);	
        if(fp._type == Instr::Field::Value){        
          int f_ix = getOpc(name);
          if (f_ix > 0) {
            const Field &ff = getOpc(f_ix);
            if (fp._value != ff._value) {
              PError(errloc,"Bad opcode assignment"); 
            } 
          }
          else {	 
            int f_ix = getField(name);
            if (f_ix < 0) {
              PError(errloc,"Bad field name index in syntax specifier: " << name);
            }
            fp._subinstr = getField(f_ix)._subinstr;
            const InstrField *f = getField(f_ix)._field;
            f->set_used();
            InstrFieldTable   t = f->table();
            if ((int)t.entry_size() - 1 < (int)fp._value) {
              PError(errloc,"Bad field name index in syntax specifier: " << fp._name);
            }
          }
        } 
        else {	
          int f_ix = getField(name);
          if (f_ix < 0) {
            //can be pseudo
            const InstrField* f = const_core.get_instrfield(gc_string(name));
            if(!f) {
              PError(errloc,"Bad field name in syntax specifier : " << name);
            } else if(!f->is_pseudo() && !f->is_alias()) {
              PError(errloc,"Bad field name in syntax specifier : " << name);
            }
            f->set_used();
          }
          if (f_ix >= 0) {
            fp._subinstr = getField(f_ix)._subinstr;
            const InstrField *f = getField(f_ix)._field;
            if (fp._trans != Field::None) { // Check transofrmation validity
              if (!f->is_constrained()) {
                bool constrained = false;
                ForEach(f->instrfields(),n) {
                  constrained |= n->second->is_constrained();
                }
                if (!constrained) {
                  PError(errloc,"Bad transformation in syntax specifier : " << name);
                }
              }
            } else if (f->is_assembler()) {
              PError(errloc,"Assembler field can not occur in syntax specifier :" << name);
            } else if (f->is_reserved()) {
              PError(errloc,"Reserved field can not occur in syntax specifier :" << name);
            }
            f->set_used();
          }
        }  	
      }
      // Now check to make sure that all fields were handled.
      for (unsigned i = 0; i != sizeFields(); ++i) {
        Field  f = getField(i);
        // Skip overlay fields, since they're optiona.
        if (f._field->is_overlay()) {
          continue;
        }
        if (f._field->is_reserved() || f._type == Field::Resrvd) {
          continue;
        }

        // Variable length opcode fields do not appear in syntax. 
        if (f._field->is_variable()) {
          continue;
        }
        bool found=false;
        const InstrField *fld = getField(i)._field;
        const gc_string name = field_name(fld->name());
        gc_string osname;

        if (fld->encap_field()) {
          osname = (fld->encap_field())->name();
        }
       
        ForEach(syntax._items,k) {
          gc_string fname;
          if (!k->first._field) {
            continue;
          }
          if (k->first._field->is_alias()) {
            if (!k->first._field->get_alias()->_target) {
              PError(errloc,"Field '" << getField(i)._name << "' not handled in syntax field.");
            } else {
              fname = field_name(k->first._field->get_alias()->_target->name());
            }
          } else {
            fname = field_name(k->first._field->name());
          } 
          const gc_string oname = (k->first._outer ? field_name(k->first._outer->name()) : "");
          if(fname == name || oname == name) {
            found = true ;
          } else if (!osname.empty() && (fname == osname || oname == osname)) {
            found = true;
          }
        }
      
        if (found) {
          if (f._field->is_assembler()) {
            PError(errloc,"Assembler field '" << getField(i)._name << "' may not be handled by syntax.");
          } else if (f._field->is_parm()) {
            PError(errloc,"Parm field '" << getField(i)._name << "' may not be handled by syntax.");
          }
        } else if ((!f._field->is_assembler()) 
                   && (f._field->type() != InstrField::tInstr)) // If instruction field is of instruction type, it can be absent in the syntax
                  {  
                    PError(errloc,"Field '" << getField(i)._name << "' not handled in syntax field.");
                  }
      }
    }
  }

  //checks validity of dsyntax
  void InstrBase::process_dsyntax(MsgLog &ml,Core &core) {    
    if (_dsyntax) {
      Syntax &dsyntax = *_dsyntax;   
      Ptree *errloc = (dsyntax._orig) ? (*dsyntax._orig)[0] : loc();      
      ForEach(dsyntax._items,i){
        Field &fp = i->first;
        // Check that expressions are built from real fields or from helper
        // functions.  If we find a helper fucntions, then it needs to be tagged
        // as used by the disassembler.
        if (fp._type == Instr::Field::Expr) {
          SimpleVarWalker vw(_env);
          vw.Translate(fp._expr);	  
          VarList vars = vw.get_vars();
          ForEach(vars,var) {
            gc_string n = var->first->ToString();
            int f_ix = getField(n);
            if (f_ix < 0 ){
              if (core.get_helper(n)) {
                core.add_disasm_helper(n);
              } else {
                PError(errloc,"Identifier is not an instruction field nor a helper for this instruction.:  '" << var->first << "'");
              }
            }
          }
        }
        else if (fp._type == Instr::Field::Ref) {
          int f_ix = getField(gc_string(fp._name));
          if (f_ix < 0) {
            PError(errloc,"Invalid instruction field '" << fp._name << "'.");
          }
          fp._subinstr = getField(f_ix)._subinstr;
        }
      }
    }
  }
  void InstrBase::print_intro(std::ostream &o,const gc_string &pfx) const 
  { 
    o << "\n"; 
  }
 
  //walks thru the syntax and builds appropriate inverse expressions
  Instr::ExtSyntaxItems Instr::build_ext_items(Environment* env,Core& core){ 
    ExtSyntaxItems ext_items;
    ForRange(_syntax->_items.size(),i) {
      auto fp = _syntax->_items.at(i);	
      ExtSyntaxItem ext_item;

      if (fp.first._type == Instr::Field::Expr) {	
        SimpleVarWalker vw(env);	    
        //first find inputs
        vw.Translate(fp.first._expr);

        ForEach(vw.get_vars(),ii) {
          gc_string field = ii->second;
          ext_item._inputs.insert(field);
        }
        ext_item._output = fp.first._field->name();
        //if not mapping to number we dont need to invert this expression
        CondWalker cw(env);
        cw.Translate(fp.first._expr);
        ext_item._orig_expr = fp.first._expr;
        if (cw.get_exprs()->Number() > 0) {
          ext_item._type = ExtSyntaxItem::Conditional;
          ext_item._inv_exprs = new (GC) PtreeArray;

          bool to_number = true;	    
          ForRange(cw.get_exprs()->Number(),ii) {	  
            Ptree *p = (*cw.get_exprs())[ii];
            unsigned tmp;
            if (to_number && !get_uint(tmp,p,env)) {
              to_number = false;
            }
            InvWalker iw(env);
            ext_item._inv_exprs->Append(iw.Translate(p));
          }

          if (to_number) {
            ext_item._type = ExtSyntaxItem::Constants;
            ext_item._orig_expr = fp.first._expr;
          }
        }
        else {
          ext_item._type = ExtSyntaxItem::Unconditional;
          InvWalker iw(env);
          ext_item._inv_exprs = new (GC) PtreeArray;
          //only one backward expr in this case
          ext_item._inv_exprs->Append(iw.Translate(fp.first._expr)); 
        }  
      }
      ext_items.insert(make_pair(ext_item._output,ext_item));
    }
    return ext_items; 
  }
 
  // returns pair - (position,val) for syntax like ("%f",RA(0x1f)), when
  // the field is not tabled. In this case we remove field from dsyntax and print
  // value into format gc_string
  Instr::UIntStrVect Instr::search_fixed_vals(Ptree* loc){
    UIntStrVect fixed_vals;
    ForRange(_syntax->_items.size(),i) {
      auto fp = _syntax->_items.at(i);    
      if (fp.first._type == Instr::Field::Value) {
        gc_string val;
        bool found = false;
        if ( fp.first._field->is_enumerated()) {
          ForEach(fp.first._field->enums(),ii) {
            if ((uint32_t)(ii->second) == fp.first._value) {
              val = ii->first;
              found = true;
              break;
            }
          }
          if (!found) {
            PError(loc,"Invalid syntax definiton for " << fp.first._field->name());
          }
          else {
            fixed_vals.push_back(make_pair(i,val));
          }	
        }
        else if(!fp.first._field->is_constrained()){	  
          gc_ostringstream os; 
          os << fp.first._value ; 
          val = os.str();
          fixed_vals.push_back(make_pair(i,val));
        }	
      }
    }
    return fixed_vals;
  }
  
  // changes format gc_string according to fixed values found by search_fixed_vals
  void Instr::process_fixed_vals(Syntax* ds, UIntStrVect& fixed_vals){
    uint32_t pos = 0;
    uint32_t index = 0;
    vector<uint32_t> indices;
    uint32_t index_const = 0;
    uint32_t i = 0;
    //find indices at which we will insert fixed value;
    while ((pos != ds->_fmt.size()) && ((uint32_t)index_const != fixed_vals.size())) {
      if (_syntax->isfield(ds->_fmt.at(pos))) {
        if (i == fixed_vals.at(index_const).first) {
          indices.push_back(pos);
          index_const++;
        }
        i++;	
      }	
      index++; 
      pos++;         
    }      
    if (!indices.empty()) {
      gc_string tmpstr = "";
      ForRange(indices.size(),i) {
        int start = (i == 0) ? 0 : indices.at(i - 1) + 1;
        int end   = (i == 0) ? indices.at(i) : indices.at(i);
        tmpstr += ds->_fmt.substr(start,end - start) + fixed_vals.at(i).second;
      }
      tmpstr += ds->_fmt.substr(indices.back() + 1);

      //now fix field values
      int offset = Syntax::Offset;
      gc_string tmpstr2 = "";
      ForRange(tmpstr.size(),i) {
        char c = tmpstr.at(i);
        if (_syntax->isfield(c)) {
          c = offset;
          offset++;
        }
        tmpstr2 += c;
      }
      ds->_fmt = tmpstr2;
    }
  }
 
  // builds inverse expression based on found values and checks
  Ptree *Instr::build_inv_expr(gc_vector<Ptree *>& values,gc_vector<pair<Ptree*, gc_string> >& checks,
                                  gc_string target,Environment* env){
    PtreeArray expr;
    expr.Append(Ptree::Make("("));
    ForEach(values,val) {
      if (PtreeUtil::Eq(*val,values.back())) {
        expr.Append(*val);
      }
      else {
        PtreeArray cond;
        ForEach(checks,ch) {
          if (ch != checks.begin()) {
            cond.Append(Ptree::Make(" && "));
          }
          //substitute  by possible invert
          SubstWalker sw(env,target,*val);
          Ptree *subst = sw.Translate(ch->first);
          cond.Append(PtreeUtil::List(Ptree::Make("(("),subst,Ptree::Make(") == "),Ptree::Make("%s",ch->second.c_str()),Ptree::Make(")")));
        }
        expr.Append(cond.All());
        expr.Append(Ptree::Make("?"));
        expr.Append(*val);
        expr.Append(Ptree::Make(":"));
      }
    }        
    expr.Append(Ptree::Make(")"));
    return expr.All();
  }
  
  //generates values and checks for real field
  void Instr::gen_val_checks(gc_vector<Ptree *> &values,
                             gc_vector<pair<Ptree*, gc_string> > &checks,
                             gc_string& target,
                             ExtSyntaxItems &ext_items){
    ForEach(ext_items,ii) {
      auto ext_item = ii->second;
      if (ext_item._output == target) {
        if (ext_item._type == ExtSyntaxItem::Conditional){
          ForRange(ext_item._inv_exprs->Number(),iii) {
            Ptree *str = PtreeUtil::List(Ptree::Make("("),(*ext_item._inv_exprs)[iii],Ptree::Make(")"));
            values.push_back(str);
          }
        }  
        else if (ext_item._type == ExtSyntaxItem::Unconditional) {
          PError(loc(),"Error in syntax inverter.");
        }  	      
      }
      else if (ext_item._type == ExtSyntaxItem::Constants) {
        if (ext_item._inputs.count(target)>0) {
          checks.push_back(make_pair(ext_item._orig_expr,ext_item._output));
        }		
      }
    }
  }
 
 
  // generates values and checks for pseudo field
  // returns false if found unconditional expression
  bool Instr::gen_val_checks_for_pseudo(gc_vector<Ptree *> &values,
                                        gc_vector<pair<Ptree*, gc_string> > &checks,
                                        Instr::Syntax::FieldId& fp,
                                        gc_string &target,
                                        ExtSyntaxItems &ext_items,
                                        const Core &core,
                                        Environment* env){
    
    ForEach(ext_items,ii) {
      auto ext_item = ii->second;
      if (ext_item._inputs.count(target) > 0) {
        if (ext_item._type == ExtSyntaxItem::Conditional){
          ForRange(ext_item._inv_exprs->Number(),iii) {
            SubstWalker sw(env,target,ext_item._output);
            Ptree *subst = sw.Translate((*ext_item._inv_exprs)[iii]);
            Ptree *str = PtreeUtil::List(Ptree::Make("("),subst,Ptree::Make(")"));
            values.push_back(str);
          }
        }  
        else if (ext_item._type == ExtSyntaxItem::Unconditional) {	      
          SubstWalker sw(env,target,ext_item._output);
          fp.first._type = Instr::Field::Expr;
          fp.first._expr = sw.Translate((*ext_item._inv_exprs)[0]);
          values.clear();
          checks.clear();
          return false;
        }
        else if (ext_item._type == ExtSyntaxItem::Constants) {
          if (ext_item._inputs.count(target)>0) {
            checks.push_back(make_pair(ext_item._orig_expr,ext_item._output));
          }		
        }  	      
      }	  
    }
    
    //if failed to find normal checks then try to build checks from conditional
    //expressions. 
    if (checks.empty()) {
      ForEach(ext_items,ii) {
        auto ext_item = ii->second;
        if (ext_item._inputs.count(target) > 0) {
          if (ext_item._type == ExtSyntaxItem::Conditional){
            checks.push_back(make_pair(ext_item._orig_expr,ext_item._output));
          }
        } 
      }    
    }
    
    return !checks.empty();
  }
  
  //returns true if "cia" is used in expression 
  bool Instr::pc_found(Ptree* expr) {
    SimpleVarWalker vw(0);	    
    vw.Translate(expr);
    ForEach(vw.get_vars(),ii) {
      if (ii->second == Cia) {
        return true;	
      }
    }
    return false;
  }   
  
  // Builds  dsyntax based on syntax:
  // Assume syntax and dsyntax format gc_strings are the same
  // For each field in syntax we check if it is modified by any expression.
  //   If it is modified by a invertible (one-to-one) function then we invert it and
  // put to dsyntax.
  //   If it is modified by non-invertible fuctions (many-to-one) then we invert
  // every case of such a function. In order to choose the right inverted expression
  // we look for expressions that have the same input and map to a list of
  // fixed numbers (not functions), call it check expressions. Check expressions
  // do not appear in dsyntax.
  //   Now in order to choose right inv expression we put every possible value
  // into check expressions and choose the first that satisfies all the checks.
  void Instr::invert_syntax(MsgLog &ml,Core &core) {
    auto ds = new InstrBase::Syntax;
    //it will have the same format gc_string (subject to fixed values)
    ds->_fmt = _syntax->_fmt;
    
    //Iems that have forward along with (possibly many) inverted expressions
    Instr::ExtSyntaxItems ext_items = build_ext_items(env(),core);   
    
    //for Instr::Field::Value fetch information in the form < position of %f, gc_string > ;
    Instr::UIntStrVect fixed_vals = search_fixed_vals(loc());

    PMap exprs; // Stores inverted expressions for use during expansion.
    
    ForRange(_syntax->_items.size(),i) {
      auto fp = _syntax->_items.at(i); 
      gc_string target = fp.first._field->name();     
      //we are done
      if (fp.first._type == Instr::Field::Expr) {
        break;
      }
      //check if need to change a only the format gc_string      
      else if (fp.first._type == Instr::Field::Value) {
        if(fp.first._field->is_constrained()){
          ds->_items.push_back(fp); 
        }		
      }
      else if (fp.first._field->is_pseudo()) {
        gc_vector<Ptree *> values;//possible field values
        gc_vector<pair<Ptree*, gc_string> > checks; //checks: expr == field
        if (!gen_val_checks_for_pseudo(values,checks,fp,target,ext_items,core,env())) {
          ds->_items.push_back(fp);
          exprs.insert(make_pair(fp.first._name,fp.first._expr));
        }
        else {
          if (values.empty()) {	 
            ds->_items.push_back(fp);
            continue;
          } else if (checks.empty())	{
            AddWarning(ml,loc(),"Syntax inversion failed for " << target << ".");
            return;
          } else {	    
            Ptree *expr = build_inv_expr(values,checks,target,env());
            fp.first._type = Instr::Field::Expr;
            fp.first._expr = expr;
            ds->_items.push_back(fp);
            exprs.insert(make_pair(fp.first._name,expr));
          }    
        }
      }
      else {
        auto iter = ext_items.find(target);
        if (iter != ext_items.end()) {
          //do not need to show in dsyntax
          if (iter->second._type == ExtSyntaxItem::Constants) {
            continue;
          }
          //check for PC, for now do not do invert
          else if (pc_found(iter->second._orig_expr)) {
            fp.first._type = Instr::Field::Ref;
            ds->_items.push_back(fp);	    
          }	  
          //just invert
          else if (iter->second._type == ExtSyntaxItem::Unconditional) {
            fp.first._type = Instr::Field::Expr;
            fp.first._expr = (*iter->second._inv_exprs)[0];
            ds->_items.push_back(fp);	  
          }
          //need to collect all possible values and checks
          else if (iter->second._type == ExtSyntaxItem::Conditional) {
            gc_vector<Ptree *> values;
            gc_vector<pair<Ptree*, gc_string> > checks; 
            gen_val_checks(values,checks,target,ext_items);
            if (checks.empty())	{
              fp.first._type = Instr::Field::Ref;
              ds->_items.push_back(fp);
              exprs.insert(make_pair(fp.first._name,fp.first._expr));
              continue;
            }    
            Ptree *expr = build_inv_expr(values,checks,target,env());
            fp.first._type = Instr::Field::Expr;
            fp.first._expr = expr;
            ds->_items.push_back(fp);	     
            exprs.insert(make_pair(fp.first._name,expr));
          }
        }
        else {
          ds->_items.push_back(fp);
        }      
      }       
    }
    // Any final substitutions to replace pseudo fields with their expansions.
    SubstWalker sw(env(),exprs);
    ForRange(ds->_items.size(),i) {
      auto fp = &ds->_items.at(i); 
      if (fp->first._type == Instr::Field::Expr) {
        fp->first._expr = sw.Translate(fp->first._expr);
      }
      
    }     
    //now process fixed values    
    if (!fixed_vals.empty()){
      process_fixed_vals(ds,fixed_vals);
    }    	 
    _dsyntax = ds;  
  }
  
  void InstrBase::print_alias(std::ostream &l,const gc_string &pfx) const  
  {
  }

  void InstrBase::process_fields(const Core &core,Fields &fields,StrHash &fieldhash,Fields &nested)
  {
    // Look up each field and make sure that it exists.
    // Make sure that each field fits within the size of the instruction.
    // Also, populate the field hash.
    fieldhash.clear();
    ForEach(fields,i) {
      Field &f = *i;
      if (!f._name) {
        PError(loc(),"Integer-only fields not allowed in the field specification.");
      }
      if (!(f._field = core.get_instrfield(f._name))) {
        PError(loc(),"Unknown instruction field " << f._name);
      }
      if (getAll(f._name) >= 0) {
        PError(loc(),"Field '" << f._name << "':  Attempt to add a duplicate instruction field in " 
               << _name << "."); 
      }    
      // We're inserting them into this hash simply so that we can detect
      // duplicates.  Once we're done adding all fields, we'll sort them so that they're in
      // order, then insert them into the hash with their final index.
      _all.push_back(*i);
      _allhash.insert(make_pair(f._name,0));
    }
    // Sort the fields by order of high-bit.
    
    sort(fields.begin(),fields.end(),FieldOrder());
    
    // Now we go back and set up the hashes, since the indices are now fixed.
    int index = 0;
    ForEach(fields,i) {
      fieldhash.insert(make_pair(i->_name,index++));
    }

    ForEach(fields,i) {
      const InstrField *ifield = i->_field;
      if (ifield) {
        ForEach(ifield->instrfields(),it) {
          gc_string name =  ifield->name() + "." + it->first;
          fieldhash.insert(make_pair(name,index++));
          nested.push_back(Field(name.c_str()));
          nested.back()._field = it->second;
        }
      }
    }
  }

  void  sort(FieldBitsIter first, FieldBitsIter last, bool src)
  {
    for(auto it = first; it != last; ++it) {
      auto nxt = it;
      while(++nxt != last) {
        MappedBits  &mb  = it->second;
        MappedBits  &nmb = nxt->second;
        if (src) {
          if (mb.src()  > nmb.src()) {
            swap(mb,nmb);
          } 
        } else {
          if (mb.trgt() > nmb.trgt()) {
            swap(mb,nmb);
          } 
        }
      }
    }
  }
  
  const char *gen_name(const gc_string &n, FieldBitsIter first, FieldBitsIter last) 
  {
    gc_ostringstream os;
    os << n << "_imp_bits_" ;
    bool second = false;
    for(FieldBitsIter it=first; it != last; ++it) 
      {
        if (second) {
          os << "x";
        }
        second = true;
        const BitPair &src  = it->second.src();
        os << "_" << src.l() << "_" << src.r() << "_" ;
      }
    os << ends;
    gc_string name = os.str();
    return gc_strdup(name);
  }

  const char *InstrBase::gen_impl(const Core &core,FieldBitsIter iter, FieldBitsIter last, 
                                  InstrField *target, InstrFieldHash &generated, 
                                  const char *nname,bool is_reserved) 
  {
    if (target) {
      // Make sure that none of the bits in the target bits array overlap
      // This algorithm is O(n^2), but since we will generally have very
      // few sequences (usually just 2), it doesn't matter very much.
      for(auto j = iter; j != last; ++j) {
        const BitPair &jtgt = j->second.trgt();
        auto k = j;
        while(++k != last) {
          if (jtgt.intersect(k->second.trgt())) {
            //          PError(loc(),"Overlapping bit ranges for target instruction field.");
          }
        }
      }
      auto j = last;
      --j;

      unsigned max_index = (target->width() ? target->width() : target->index() * core.parallel_execution());
      const BitPair tgt = j->second.trgt();
      if (target->width() <= tgt.max() && max_index <= tgt.max()) {
        PError(loc(),"Index out of field '" << target->name() << "' range.");
      }
    }

    gc_string tname = (target ? target->name() : nname);
    const char *name = gen_name(tname,iter,last);
    if(generated.find(name) == generated.end()) { // new implementation
      gc_string sname(name);
      InstrField *ifield = InstrField::build_impl(env(),loc(),sname,target,is_reserved); 
      int prev =  -1;
      unsigned tgt_width = target ? target->width() : 0;
      IntVect *gaps = (ifield->gaps());
      int       total_gap = 0;
      while (iter != last) {
        const BitPair &src = iter->second.src();
        const BitPair &tgt = iter->second.trgt();
        src.check_order(loc());
        tgt.check_order(loc());
        if (target && target->index()) {
          gaps->push_back(tgt.lo(tgt_width) - prev - 1);
          prev = tgt.hi(tgt_width);
          total_gap += gaps->back();
        }
        ifield->add_bits(src);
        ++iter;
      }
      // Here we take into account the last gap
      if (target && target->index() && prev != (int)target->width()) {
        total_gap += target->width() - prev - 1;
      }
      if (total_gap) {
        gaps->push_back(total_gap);
      } else if (gaps) {
        gaps->clear();
      }
      generated.insert(make_pair(name,ifield));
    } 
    _tmpfields->add_impl(tname,gc_string(name));
    return name;
  }
  
  bool InstrBase::add_tmp_field(const Core &core,unsigned &new_start,FieldBase *src,unsigned start,bool le,bool check_pseudo_fields)
  {
    if (TmpField *t = get_tmpfield(src)) {
      if (!_tmpfields) {
        _tmpfields = new TmpFields;
      }
      _tmpfields->push_back(*t);
      new_start += _tmpfields->back()._src.width();
      return true;
    } else if (check_pseudo_fields) {
      if (Field *fld = get_field(src)) {
        if (fld->_name) {
          const InstrField *f = core.get_instrfield(fld->_name);
          if (f && !f->is_parm()) {    
            if (f->is_pseudo() && !f->is_variable()) {
              // We have a pseudo field, so it must be moved to the tmp-field data
              // structure.
              if (!_tmpfields) {
                _tmpfields = new TmpFields;
              }
              unsigned s = (le) ? (start + f->width()-1) : start;
              unsigned e = (le) ? start : (start + f->width()-1);
              if (fld->_type == Field::Value) {
                _tmpfields->push_back(TmpField(fld->_value,s,e));
              } else {
                _tmpfields->push_back(TmpField(gc_strdup(f->name().c_str()),s,e));
              }
              _tmpfields->back()._subinstr = fld->_subinstr;
              new_start += f->width();
              return true;
            }
            new_start = f->bits().highbit()+1;
          }
        }
      }
    }
    return false;
  }

  // If any pseudo fields were placed directly within our encoding, then we move
  // them to the tmpfield structure and process them.  In addition, we assign them
  // bits using a basic left-to-right encoding.
  void InstrBase::check_for_pseudo_fields(const Core &core,bool check_pseudo_fields)
  {
    // For big-endian, we start from the left with a bit value of 0.  For
    // little-endian, we start from the right.
    unsigned bit = 0;
    if (core.bitEndianness() == Core::Big) {
      // Big-endian.
      auto iter = _init_all.begin();
      while (iter != _init_all.end()) {
        // If we don't find the field, then we'll ignore it for now- later error
        // checking will issue the error about an unknown field.
        if (add_tmp_field(core,bit,*iter,bit,false,check_pseudo_fields)) {
          _init_all.erase(iter--);
          ++iter;
          continue;
        }
        ++iter;
      }
    } else {
      // Little-endian.
      auto iter = _init_all.rbegin();
      while (iter != _init_all.rend()) {
        // If we don't find the field, then we'll ignore it for now- later error
        // checking will issue the error about an unknown field.
        if (add_tmp_field(core,bit,*iter,bit,true,check_pseudo_fields)) {
          _init_all.erase((++iter).base());
          continue;
        }
        ++iter;
      }
    }
  }

  void check_split_value(const Instr::TmpField &sf,Ptree *loc)
  {
    unsigned val = sf._value;
    unsigned len = log2(val)+1;
    unsigned width = sf._src.width();
    if (width < len) {
      PError(loc,"Value is larger than specified width.");
    }
  }

  uint64_t get_split_value(FieldBits::const_iterator iter,FieldBits::const_iterator end)
  {
    uint64_t val = 0;
    for ( ; iter != end; ++iter) {
      const BitPair &src = iter->second.src();
      val <<= src.width();
      val |= iter->second.value();
    }
    return val;
  }

  void InstrBase::finalize_tmpfields(const Core &core, InstrFieldHash &generated, bool prefix,bool check_pseudo_fields) 
  {
    check_for_pseudo_fields(core,check_pseudo_fields);

    if (!_tmpfields) {
      return;
    }

    FieldBits  fieldbits;
    FieldBits  opcbits;
    FieldBits  resrvdbits;
    gc_set<const InstrField *> val_fields;

    // collect all bits 
    ForEach(_tmpfields->_fields,i) {
      TmpField &sf = *i;
      InstrField *targetField = 0;

      if (sf._name)  {
        targetField = const_cast<InstrField *>(core.get_instrfield(sf._name));
        if(!targetField) {
          PError(loc(),"Unknown instruction field " << sf._name);
        }
      }
      
      if(sf._stat == TmpField::Mapped) {
        fieldbits.insert(make_pair(make_pair(targetField,sf._subinstr), MappedBits(sf._src)));
      } 
      else if(sf._type == TmpField::Ref || sf._type == TmpField::RValue) { 
        // split field (possibly with value).
        unsigned swidth = sf._src.width();
        unsigned twidth = sf._tgt.width();

        if (swidth < twidth) {
          PError(loc(),"Source field size is smaller than target field size.");
        } else if (twidth < swidth) {
          PError(loc(),"Target field size is smaller than source field size.");
        }
        
        if (sf._type == TmpField::RValue) {
          check_split_value(sf,loc());
          val_fields.insert(targetField);
        }

        fieldbits.insert(make_pair(make_pair(targetField,sf._subinstr),MappedBits(sf._src,sf._tgt,sf._value)));      
      }
      else if (sf._type == TmpField::Resrvd) {
        resrvdbits.insert(make_pair(make_pair((InstrField*)0,(const SubInstrGrp*)0),MappedBits(sf._src)));
      } else if (sf._type == TmpField::Value) { // split opcode
        check_split_value(sf,loc());
        opcbits.insert(make_pair(make_pair(targetField,sf._subinstr),MappedBits(sf._src,sf._value)));
      }
    }

    // Handle fields
    auto it = fieldbits.begin();
    auto end = fieldbits.end();
    while(it != end) {
      auto nxt = fieldbits.upper_bound(it->first);
      InstrField *targetField = it->first.first;
      bool has_value = val_fields.count(targetField);
      // need to sort for split fields by target bits 
      sort(it,nxt,false); // ((bits(2,4),Fld(3,5)),(bits(8,9),Fld(1,2))) => ((8,9),(2,4))
      const char* name = gen_impl(core,it,nxt,targetField,generated,0,false);
      auto field = (has_value) ? new Field(name,get_split_value(it,nxt)) : new Field(name);
      field->_subinstr = it->first.second;
      if (targetField->is_reserved()) {
        field->_type = Field::Resrvd;
      } else if (has_value) {
        field->_type = Field::RValue;
      }
      _init_all.push_back(field);
      it = nxt; // update loop
    }

    // Handle opcode bits
    auto opcit = opcbits.begin();
    auto opcend = opcbits.end();
    if (opcit != opcend) {
      sort(opcit,opcend,true);
      const char *var_name = prefix ? VarPrefixName : VarOpcodeName;
      const char *name = gen_impl(core,opcit,opcend,0,generated,var_name,false);
      if (prefix) {
        (generated.find(name)->second)->setPrefix();
      }

      // Generated opcode value
      auto field = new Field(name,get_split_value(opcit,opcend));
      field->_subinstr = it->first.second;
      field->_type = Field::RValue;
      _init_all.push_back(field);
    }
    
    // Handle reserved bits
    auto resrvdit = resrvdbits.begin();
    auto resrvdend = resrvdbits.end();
    if (resrvdit != resrvdend) {
      sort(resrvdit,resrvdend,true);
      const char *name = gen_impl(core,resrvdit,resrvdend,0,generated,ReservedEnum,true);
      auto field = new Field(name);
      field->_subinstr = it->first.second;
      field->_type = Field::Resrvd;
      _init_all.push_back(field);
    }
  }

	struct InstrAspectPred : AspectPred {
		virtual bool operator()(const VarWalker &vw,const Aspect &a,bool pre) const
		{
			if (vw.is_outer_scope()) {
				if (pre && a.instr_pre_exec()) {
					return true;
				}
				if (!pre && a.instr_post_exec()) {
					return true;
				}
			}
			return false;
		}
	};

  // Given a field name, searches for an associated prefix counter.  Returns 0 if not found.
  const gc_string *InstrBase::get_prefix_counter(const gc_string &field_name) const
  {
    ForEach(_prefix_counters,i) {
      if ( i->second == field_name ) {
        return &(i->first);
      }
    }
    return 0;
  }


  // This makes sure that prefix counter bindings are valid.  We don't allow
  // prefix counters to reference fields that already contain action code, since
  // it's then confusing about what the semantics should be.  We also make sure
  // that the prefix-counter exists and that the field exists.
  void InstrBase::process_pfx_ctrs(MsgLog &ml,Core &core)
  {
    ForEach(_prefix_counters,i) {
      const gc_string &pc = i->first;
      const gc_string &fn = i->second;

      if (! const_core.get_pfx_ctr(pc) ) {
        PError(loc(),"Prefix counter " << pc << " does not exist.");
      }
      int findex;
      if ( (findex = getField(fn)) < 0) {
        PError(loc(),"Prefix counter " << pc << " is bound to unknown field (" << fn <<") for this instruction.");
      }
      const InstrField *field = getField(findex)._field;
      assert(field);
      if (field->action()) {
        PError(loc(),"Prefix counter " << pc << " bound to a field (" << fn << ") which also contains action code.");
      }
      if (!field->is_prefix()) {
        PError(loc(),"Prefix counter " << pc << " bound to a field (" << fn << ") which is not a prefix field.");
      }
      if (!field->index()) {
        PError(loc(),"Prefix counter " << pc << " bound to a field (" << fn << ") which is not marked as indexed.");
      }
    }
    ForEach(_prefix_counter_mods,i) {
      const gc_string &pc = i->first;

      if (! const_core.get_pfx_ctr(pc) ) {
        PError(loc(),"Prefix counter " << pc << " (in modification list) does not exist.");
      }
    }
  }

  void InstrBase::finalize(MsgLog &ml,Core &core, bool actionless, bool alias,bool assembler,
                           ResExcludes *src_excludes,ResExcludes *trg_excludes)
  {
    assert(_env);
    
    if (_tmpfields) {
      _tmpfields->finalize(core);
    }
  
    // Check to see if this instruction is a shorthand for another.
    process_shorthand(core);

    // Note: Finalizing attributes after shorthands so that the shorthand will
    // get the parent's attributes first.
    Attrs::finalize();

    process_fields(core,_fields,_fieldhash,_nestedfields);
    process_fields(core,_opcs,_opchash,_nestedopcs);

    if (_all.empty() && !alias && !assembler) {
      PError(loc(),"No fields defined for this instruction.");
    }

    ForEach(_blks,blk) {
      if (!core.is_block(*blk)) {
        PError(loc(),"Undefined block.");
      }
    }

    process_width(core.prefix_bits(),core.need_encoding());

    // Check that only one variable field is defined
    bool variable = false;
    ForEach(_all,i) {
      const InstrField &ifield = *(i->_field);
      if (ifield.is_variable()) {
        if (variable) {
          PError(loc(),"More then one variable width field used.");
        }
        if(ifield.is_assembler()) {
          PError(loc(),"Assembler variable width fields can not be used in instructions.");
        }
        variable = true;
      }
    }
    // Check for overlaps by applying field masks.
    
    unsigned cw = 0; // Highest bit
    ForEach(_all,i) {
      cw = max(cw,i->_field->bits().highbit()); 
    }
    ++cw; // width is +1
    RNumber mask(0,cw) ;
    
    ForEach(_all,i) {
      const InstrField &ifield = *(i->_field);
      if (!ifield.is_overlay()) {
        RNumber tmpmask(0,mask.size()) ;
        ForEach(ifield.bits(),b) {
          for (int i = b->lo(cw) ; i <= b->hi(cw);i++) {
            tmpmask.setBit(i,1);
          }
        }	
        if ((mask & tmpmask) != 0) {
          PError(loc(),"Field '" << ifield.name() << "' overlaps with previous fields.");
        }
        mask |= tmpmask;
      }
    }

    // Now sort all of the fields to put them in order.
    sort(_all.begin(),_all.end(),FieldOrder());
    
    // Reinsert into all field, since the order has changed.
    _allhash.clear();
    int index = 0;
    bool  is_prefix = false;
    ForEach(_all,i) {
      if (i->_field && i->_field->is_prefix()) {
        is_prefix = true;
      }
      if (i->_field && i->_field->type() == InstrField::tInstr) {
        ++_mo_count;
      }
      _allhash.insert(make_pair(i->_name,index++));
    }

    // Scan the action function for unknown resources.
    if (_action) {
      if (actionless) {
        PError(loc(),"Alias, pseudo, and prefix instructions can not have action function.");	
      }

      // Translate action code from abstract to implementation fields
      if (_tmpfields) {
        VarWalker vw(_env,&core,&core,true);
        vw.set_track_usage(false);
        _action = vw.Translate(_action);
      }

      CheckCode cc(_env,core,true);
      cc._vw.set_excludes(src_excludes,trg_excludes);
      cc._vw.set_instr(this);
			InstrAspectPred a;
			cc._vw.set_aspects_pred(&a);
      cc.set_is_instr_code(true);
      cc.check_code_filter(_action);
      _action = cc.translated();

      _mo_offset_field = cc.mo_offset_field();

      // If we found escaping vars, make sure that they map to registers or register files.
      bool error = false;
      ForEach(cc.escapes(),i) {
        if (getField(i->second) < 0) {
          error = true;
          LogPError(ml,PError(i->first,"Unknown variable/resource found in action code:  " << i->first));
        }
      }
      if (error) return;

      // Now store source and target register information.  If prior data
      // exists, i.e. the user has explicitly set source and target, then ignore
      // the collected info.
      UsedRegs orig_srcs = _usageinfo._src_regs;
      UsedRegs orig_trgs = _usageinfo._trg_regs;
      _usageinfo.update(cc.usageinfo());
      if (!orig_srcs.empty()) {
        _usageinfo._src_regs = orig_srcs;
        _usageinfo._explicit_srcs = true;
      }
      if (!orig_trgs.empty()) {
        _usageinfo._trg_regs = orig_trgs;
        _usageinfo._explicit_trgs = true;
      }

      // Record any delayed writes in the core.      
      record_delayed_writes(core,_usageinfo);
      
      // We consider this to be a source if we modify the nia.
      IfFind(iter,_usageinfo._trg_regs,UsedReg(core.nia())) {
        _branch_type = (iter->conditional()) ? BranchCond : BranchUncond;
      } else {
        _branch_type = BranchNone;
      }

      // FIXME
      int bt_attr = Data::get_attr(gc_string("branch_target"));
      ForEach(_usageinfo._trg_regs,i) {
        if (i->regb()->has_attr(bt_attr)) {
          _branch_type =  (iter->conditional()) ? BranchCond : BranchUncond;
        }
      }
    } 

    // Then we check to see if we have any address-related instruction fields.
    // If we have a pc-relative field, then we mark the cia as a source.  We
    // don't bother marking the nia as a target because that should come through
    // in the action code itself, since we don't have any implicit nia updating
    // actions.
    //
    // Note: We do this for aliases as well, since they can have a field that uses
    // the CIA, but the target of the alias might not.
    ForEach(getFields(),iter) {
      const InstrField &field = *(iter->_field); 
      if (field.addr() == InstrField::aPcRel) {
        _usageinfo._src_regs.insert(core.cia());
      }
    }

    // Check the syntax for validity.
    process_syntax(core);
    process_dsyntax(ml,core);

    LogPError(ml,process_pfx_ctrs(ml,core));
  }

  void InstrBase::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  " << _name;
    print_intro(o,pfx);

    if (is_branch()) {
      o << pfx << "Branch:  " << ((is_branch() == BranchCond) ? "Conditional" : "Unconditional") << '\n';
    }

    if (is_micro_op()) {
      o << pfx << "Is micro op.\n";
    } else if (has_micro_ops()) {
      o << pfx << "Micro ops:  " << micro_op_count() << "\n";
    }
    if (has_micro_op_offset()) {
      o << pfx << "Micro-op offset field: " << micro_op_offset_field() << '\n';
    }

    if (_syntax) {
      o << pfx << "Syntax:  \"" << *_syntax << "\":  ";
      ForEach(_syntax->_items,i) {
        o << " " << i->first;
        if(i->second > -1) {
          o << "(" << i->second << ")";
        }
      }
      o << "\n";
    }
    
    if (!_blks.empty()) {
      if (1 == _blks.size()) {
        o << pfx << "Block:  " << get_blk() << "\n";
      } else {
        o << pfx << "Blocks: ";
        ForEach(_blks,i) {
          o << " " << *i; 
        }
        o << "\n";
      }
    }

    if (_dsyntax) {
      o << pfx << "DSyntax:  \"" << *_dsyntax << "\":  ";
      ForEach(_dsyntax->_items,i) {
        o << " " << i->first;
        if(i->second > -1) {
          o << " ( " << i->second << " ) ";
        }
      }
      o << "\n";
    }
    Attrs::dump(o,pfx);

    o << pfx << "Fields: ";
    ForEach(_all,i) {
      o << " " << *i; 
    }
    o << "\n";

    if (!_prefix_counters.empty()) {
      gc_string pfx1 = indent(pfx);
      o << pfx << "Prefix Counter Bindings:\n";
      ForEach(_prefix_counters,i) {
        o << pfx1 << i->first << ":  Bound to " << i->second << '\n';
      }
      o << "\n";
    }
    if (!_prefix_counter_mods.empty()) {
      gc_string pfx1 = indent(pfx);
      o << pfx << "Prefix Counter Modifications:  ";
      ForEach(_prefix_counter_mods,i) {
        const gc_string &pc = i->first;
        bool incr = i->second;
        o << pc << ((incr) ? "++" : "--") << " ";
      }
      o << '\n';
    }

    print_alias(o,pfx);

    if (_action) {
      o << pfx << "Action:  " << _action << "\n";
    }

    // We're using the accessor member so that we get the usage information that
    // the user of the data model would get.  This changes slightly for aliases,
    // since they use their targets, if applicable.
    printUsageInfo(o,usageinfo(),pfx);

    dump_doc(o,pfx,true);
  }


  //
  // Information about subinstructions.
  // 

  SubInstr::SubInstr(Environment *env,Ptree *loc, const gc_string &name) :
    InstrBase(env,loc,name) 
  {
  }

  void SubInstr::finalize(MsgLog &ml,Core &core) 
  {
    // Substitute any SubInstr with its constituent InstrFields
    distr_fields(_fields,_opcs,0,_init_all,true,true);

    InstrBase::finalize(ml,core); 

    // If we succeeeded in finalizing the action code, then we record pointers
    // to the constituent items of the action function.  This must be done after
    // finalization so that we get the transformed code which has the register
    // access objects in it.
    _body = get_func_body(_action);
    _args = get_func_args(_action);
  }

  void SubInstr::process_syntax(Core &core)
  {
    // Validity check of the syntax
    if(_syntax) {
      InstrBase::process_syntax(core);
 
      Syntax &syntax = *_syntax;
      Ptree *errloc = (syntax._orig->Number()) ? (*syntax._orig)[0] : loc();
      unsigned count = 0;
      gc_string::iterator siter = syntax._fmt.begin();
      gc_string::iterator send  = syntax._fmt.end();
      while (siter != send) {
        if (*siter == '%') {
          ++siter;
          if (*siter == 'f' || *siter == 'p') {
            if (count >= syntax._items.size()) {
              PError(errloc,"Too many format codes were found than fields listed in the syntax field.");
            }
            ++count;
          }
        }
        ++siter;
      }
      if (count < syntax._items.size()) {
        PError(errloc,"Fewer format codes were found than fields listed in the syntax field.");
      }
    }
  }
  void SubInstr::process_dsyntax(MsgLog &ml,Core &core) {
    if(_dsyntax) {
      InstrBase::process_dsyntax(ml,core);
 
      Syntax &syntax = *_dsyntax;
      Ptree *errloc = (syntax._orig->Number()) ? (*syntax._orig)[0] : loc();
      unsigned count = 0;
      gc_string::iterator siter = syntax._fmt.begin();
      gc_string::iterator send  = syntax._fmt.end();
      while (siter != send) {
        if (*siter == '%') {
          ++siter;
          if (*siter == 'f') {
            if (count >= syntax._items.size()) {
              PError(errloc,"Too many format codes were found than fields listed in the syntax field.");
            }
            ++count;
          }
        }
        ++siter;
      }
      if (count < syntax._items.size()) {
        PError(errloc,"Fewer format codes were found than fields listed in the syntax field.");
      }
    }
  }

  void SubInstr::process_fields(const Core &core,Fields &fields,StrHash &fieldhash,Fields &nestedfields)
  {
    InstrBase::process_fields(core,fields,fieldhash,nestedfields);
  }

  void SubInstr::dump(ostream &o, const gc_string &pfx) const
  {
    InstrBase::dump(o,pfx);
    o << pfx << "-------------------------------\n";
  }

  gc_string Instr::Field::get_name(bool use_doc_name) const
  {
    if (_field && use_doc_name) {
      // If we want a documentation name, then if it doesn't have one, then we
      // probably want the parent name, if one exists.  I doubt we ever want to
      // see "FOO_impl..." in the documentation!
      if (_field->has_doc_name()) {
        return _field->docName();
      } else if (const InstrField *i = _field->interface()) {
        return i->docName();
      }
    }
    // Default:  Return our basic name.
    return _name;
  }

  //
  // Information about instructions.
  // 

  void Instr::Field::dump(std::ostream &o,bool use_doc_name) const
  {
    switch (_type) {
    case Instr::Field::Ref:
    case Instr::Field::RValue:
      switch(_trans) {
      case Instr::Field::Reverse:
        o << "reverse(" << get_name(use_doc_name) << ")";
        break;
      case Instr::Field::Symmetric:
        o << "symmetric(" << get_name(use_doc_name) << ")";
        break;
      case Instr::Field::None:
        o << get_name(use_doc_name);
        break;
      case Instr::Field::TransCnt: 
        break;
      }
      break;
    case Instr::Field::Value:
      o << get_name(use_doc_name) << "(" << _value << ")";
      break;
    case Instr::Field::Expr:
      o << get_name(use_doc_name) << "(" << _expr << ")";
      break;
    case Instr::Field::Alias:
      o << get_name(use_doc_name) << "(" << _field->name() << ")";
      break;
    case Instr::Field::Resrvd:
      o << get_name(use_doc_name) << "(reserved)";
      break;
    }
  }

  ostream &operator<<(ostream &o,const Instr::Field &f)
  {
    f.dump(o,false);
    return o;
  }

  const Reg *Instr::Dependencies::DepItem::reg() const
  {
    return dynamic_cast<const Reg*>(_reg);
  }

  const RegFile *Instr::Dependencies::DepItem::regfile() const
  {
    return dynamic_cast<const RegFile*>(_reg);
  }

  ostream &operator<<(ostream &o,const Instr::Dependencies::DepItem &d)
  {
    if (d._gmem) {
      o << MemoryName;
    }
    else if (d._mem) {
      o << d._mem->name();
    } 
    else if (d._reg) {
      o << d._reg->name();
      if (d._index) {
        o << "(" << d._index << ")";
      }
    } 
    else {
      o << "<emtpy>";
      return o;
    }
    if (d.has_seq()) {
      o << "($" << d._seq << ")";
    }
    return o;
  }

  bool Instr::Dependencies::getDepRes(DepItem &di,MsgLog &ml,Ptree *p,const Core &core)
  {
    gc_string n = p->ToString();
    if (auto mem = core.get_memory(n)) {
      di._mem = mem;
      return false;
    } else if (auto reg = core.get_reg(n)) {
      di._reg = reg;
      return false;
    } else if (auto rf = core.get_regfile(n)) {
      di._reg = rf;
      return true;  							  	 						      	
    } else if (PtreeUtil::Eq(p,MemoryName)) {
      di._gmem = true;
      return false;
    } else {
      AddError(ml,p,"Bad resource '" << p << "' for dependency item.");
      return false;
    }
  }

  void Instr::Dependencies::getDepSeq(DepItem &di,MsgLog &ml,Ptree *p)
  {
    using namespace PtreeUtil;

    if (!p->IsLeaf()) {
      Ptree *n = p->Car();
      if (*(n->GetPosition()) == '$') {
        di._seq = atoi(n->GetPosition()+1);
        if (di._seq == 0) {
          AddError(ml,p,"Bad sequence number:  First access is $1.");
        }
        return;
      }
    }
    AddError(ml,p,"Malformed sequence number specified.");
  }

  // p should be a dependency item of the form:
  //   Mem
  //   Reg
  //   Regfile(index)
  //  Optional:  Trailing sequence number ($n).
  //  If register file, then expression must be made of constants or instruction fields.
  Instr::Dependencies::DepItem Instr::Dependencies::parseDepItem(Ptree *p,MsgLog &ml,const Core &core,const Instr *instr)
  {
    using namespace PtreeUtil;

    DepItem di;
    if (p->IsLeaf()) {
      // Simple item.
      getDepRes(di,ml,p,core);
      return di;
    } else {
      Ptree *res,*index,*rest,*snum;
      if (Match(p,"[ %? ( %? ) %r]",&res,&index,&rest)) {
        if (getDepRes(di,ml,res,core)) {
          // The index expression must consist of only constants or instruction fields.
          ResWalker w(instr);
          w.Translate(index);
          if (w.is_fixed()) {
            di._index = index;
          } else {
            AddError(ml,p,"Bad dependency-item index expression.  It must consist of only constants and/or instruction fields.");
          }
          if (Match(rest,"[ ( %? ) ]",&snum)) {
            getDepSeq(di,ml,snum);
          }
        } else {
          // If a memory or register item, then the index will be a sequence
          // number.
          getDepSeq(di,ml,index);
        }
      } else {
        AddError(ml,p,"Malforemd dependency item.");
      }
    }
    return di;
  }

  // p should be a dependency expression of the form: trg = src [, src ...].
  void Instr::Dependencies::parseDepExpr(Ptree *p,MsgLog &ml,const Core &core,const Instr *instr)
  {
    using namespace PtreeUtil;

    Ptree *expr = First(p);
    Ptree *trg, *srcs, *src, *srest;
    DepExpr de;
    if (expr && Match(expr,"[ %? = %? ]",&trg,&srcs)) {
      de._trg = parseDepItem(trg,ml,core,instr);
      srcs = (srcs->IsA(ntParenExpr)) ? Second(srcs) : srcs;
      while (srcs) {
        if (Match(srcs,"[ %? , %? ]",&srest,&src)) {
          de._srcs.push_front(parseDepItem(src,ml,core,instr));
          srcs = srest;
        } else {
          de._srcs.push_back(parseDepItem(srcs,ml,core,instr));
          srcs = 0;
        }
      }
    } else {
      AddError(ml,p,"Expected an expression of the form 'trg = srcs' for a dependency-tracking item.");
    }
    if (de._srcs.empty()) {
      AddError(ml,p,"No sources were specified for this dependency expression.");
    }
    _exprs.push_back(de);
  }

  void Instr::Dependencies::finalize(MsgLog &ml,const Core &core,const Instr *instr)
  {
    if (PtreeBrace *pb = dynamic_cast<PtreeBrace*>(_orig)) {
      Ptree* body = PtreeUtil::Second(pb);
      Ptree* rest = body;
      while(rest != 0){
        Ptree* p = rest->Car();
        parseDepExpr(p,ml,core,instr);
        rest = rest->Cdr();
      }
    } else {
      AddError(ml,_orig,"Expected a brace-delimited block for dependency information");
    }
    _orig = 0;
  }

  void Instr::Dependencies::dump(ostream &o,const gc_string &pfx) const
  {
    ForEach(_exprs,i) {
      const DepExpr &expr = *i;
      o << pfx << expr._trg << " = ";
      bool first = true;
      ForEach(expr._srcs,j) {
        const DepItem &di = *j;
        list_sep(o,first);
        o << di;
      }
      o << '\n';
    }
  }

  Instr::Instr(Environment *env,Ptree *loc,const gc_string &name, int dummy,bool remove) :
    InstrBase(env,loc,name),
    _id(0),
    _width(0),
    _fetch_width(0),
    _little_endian(false),
    _assemble(true),
    _asm_rank(DefaultAsmRank),
    _disassemble(true),
    _assembler(0),
    _alias_action(0),
    _shorthands(0),
    _shorthand(false),
    _has_subinstrs(false),
    _dummy(dummy),
    _remove(remove),
    _prefix(false),
    _pseudo(false),
    _type(0),
    _nested(false),
    _need_encoding(false),
    _allow_conflict(false),
    _reserved_bits_value(RsrvNotSet),
    _enable_post_fetch(PostFetchTrue),
    _process_sub_syntax(false),
    _process_sub_dsyntax(false),
    _dependencies(0)
  {
  }

  Instr::Instr(const Instr& o, const gc_string &name) :
    InstrBase(o,name),
    _width(o._width),
    _assemble(o._assemble),
    _disassemble(o._disassemble),
    _assembler(o._assembler),
    _alias_action(o._alias_action),
    _shorthands(0),
    _shorthand(o._shorthand),
    _has_subinstrs(o._has_subinstrs),
    _prefix(o._prefix),
    _pseudo(o._pseudo),
    _type(o._type),
    _need_encoding(o._need_encoding),
    _allow_conflict(o._allow_conflict),
    _reserved_bits_value(o._reserved_bits_value),
    _enable_post_fetch(o._enable_post_fetch),
    _enable_post_fetch_func(o._enable_post_fetch_func),
    _asmrules(o._asmrules),
    _dependencies(o._dependencies)
  {
  }

  static bool uniqueAspectActions(const Aspect *x,const Aspect *y)
  {
    return x->action() == y->action();
  }

  // We examine child instructions' aspects as well, if they don't have action
  // code, i.e. they're just aliases and could never match anyway.
  bool Instr::getAspects(AspectList &al,const gc_string &label) const
  {
    // First, get our own.
    bool rc = Aspects::getAspects(al,label);

    // Then get child items.
    ForEach(get_nested(),i) {
      const Instr &instr = *i->second;
      if (!instr.action()) {
        bool trc = instr.getAspects(al,label);
        rc = trc || rc;
      }
    }

    // Then sort so that duplicates are nest to each other and then remove these
    // duplicates so that we only have unique aspects.
    al.sort();
    al.unique(uniqueAspectActions);
    return rc;
  }

  int count_non_tmps(const InstrBase::BFields &fields)
  {
    int count = 0;
    ForEach(fields,i) {
      if (!(*i)->is_tmp()) {
        ++count;
      }
    }
    return count;
  }

  int count_tmps(const InstrBase::BFields &fields)
  {
    int count = 0;
    ForEach(fields,i) {
      if ((*i)->is_tmp()) {
        ++count;
      }
    }
    return count;
  }

  void Instr::encode(const Core &core, unsigned &opcd, unsigned opcd_size, unsigned width) 
  {
    // if it is a pseudo or assembler only instruction
    if (_pseudo || (!_action && _assembler)) {
      return;
    }

    if (count_tmps(_init_all)) {

      // left and right bits
      unsigned lb = 0; 
      unsigned rb = 0;

      bool  little_end = core.is_little_endian();
      BFields new_tmpfields;
      
      if (little_end) {
        lb = width-1;
        rb = lb - opcd_size + 1;
      } else {
        rb = opcd_size - 1;
      }
      
      new_tmpfields.push_back(new TmpField(opcd,lb,rb));

      StrSet  counted; // fields already encoded
      ForEach(_init_all,iter) {	
        if (TmpField *tf = get_tmpfield(*iter)) {
          const char* fname = tf->_name;
          if (fname && counted.find(fname) == counted.end()) {
            counted.insert(fname);
            unsigned fw = core.get_instrfield(fname)->width();
            if (little_end) {
              lb = rb - 1;
              rb -= fw;
            } else {
              lb = rb + 1;
              rb += fw;
            }
            new_tmpfields.push_back(new TmpField(fname,lb,rb)); 
          }
        }
      }
      _init_all = new_tmpfields;
      ++opcd;
      _width = width;
    } else { // no tmpfields
      bool all_pseudo = true;
      ForEach(_init_all,i) {
        if (Field *field = get_field(*i)) {
          const InstrField *f = field->_field;
          all_pseudo &= (!f || f->is_pseudo());;
        }
      }
      if (!all_pseudo) {
        PError(loc(),"Encoding generation not supported for non bit-mapped fields sytnax in instruction :" << _name);
      }
    }
  }

  void Instr::process_fields(const Core &core,Fields &fields,StrHash &fieldhash,Fields &nestedfields)
  {
    InstrBase::process_fields(core,fields,fieldhash,nestedfields);
    // Make sure that each field fits within the size of the instruction.
    ForEach(fields,i) {
      Field &f = *i;
      const InstrField &ifield = *(f._field);
      if (_width > 0 && ifield.highbit() >= _width) {
        PError(loc(),"Field '" << f._name << "' is larger than the instruction.");
      }
    }
    if (is_alias()) {
      // If we have an alias, then we might have fields which have indices which
      // need to be set up.
      ForEach(_alias,i) {
        Alias *alias = *i;
        ForEach(alias->_fields,a_iter) {
          Field &afield = *a_iter;
          if (afield._type == Field::Ref) {
            int index = getField(afield._sname);
            assert(index >= 0);
            afield._value = index;
          }
        }
      }
    }
  }

  const InstrList &Instr::shorthands() const 
  { 
    assert(_shorthands); 
    return *_shorthands; 
  }

  bool Instr::is_abstract() const 
  {
    return _opcs.empty();
  }

  unsigned Instr::fields_width(const Core &core) const
  {
    if (_pseudo) {
      return 0;
    }
    unsigned width = 0;
    StrSet counted;
    ForEach(_init_all,i) {
      if (TmpField *tf = get_tmpfield(*i)) {
        const char* fname = tf->_name;;
        if (fname && counted.find(fname) == counted.end()) { // not an opcode, not counted
          if (auto field = core.get_instrfield(fname)) { 
            counted.insert(fname);
            width += field->width();
          } else {
            PError(loc(),"Unknown instruction field " << fname);
          }
        }
      }
    }
    return width;
  }

  int Instr::add_alias_field(const char *orig,bool add_fields, bool &is_opc)
  {
    const char *name = gc_strdup(field_name(orig).c_str());

    int f_index;
    is_opc = false;
    if (add_fields) {
      // Add this field if it doesn't already exist.  We need to add it to the
      // hash in order to check for duplicates.
      auto ip = _fieldhash.insert(make_pair(name,_fields.size()));
      if (ip.second) {
        _fields.push_back(Field(name));
      }
      f_index = ip.first->second;
    } else {
      // We're not adding fields, so make sure it exists in our field list.
      auto ix = _fieldhash.find(name);
      if (ix == _fieldhash.end()) {
        if((ix = _opchash.find(name)) == _opchash.end()) {
          PError(loc(),"Field '" << name << "' used in alias does not exist in instruction's field definition.");
        } else {
          is_opc = true;
        }
      }
      f_index = ix->second;
    }
    return f_index;
  }

  bool isGeneratedOpcode(const Instr::FieldBase &f)
  {
    return strncmp(f._name,VarOpcodeName,VarOpcodeNameLen) == 0;
  }

  void Instr::process_var_opcode(TmpAlias *tmpalias,const Instr *target)
  {
    // Handle the variable length encoding field.  This is the implicit
    // opcode which is then mapped to a target variable-length field, if one
    // exists.  There will only be one of these.
    ForEach(_opcs,f) {
      auto f_type = f->_type;
      if (f_type == Field::RValue && isGeneratedOpcode(*f)) {
        unsigned id = 0;
        ForEach(tmpalias->_fields,k) {
          int index = target->getField(tmpalias->_field_names[id]);
          if (index >= 0) {
            Field tf = target->getField(index);
            if (tf._field->is_variable()) {
              tmpalias->_fields[id]._name = 0;
              tmpalias->_fields[id]._value = f->_value;
              tmpalias->_fields[id]._type = f_type;
              gc_string s = tf._field->name();
              gc_string t = f->_name;
              _tmpfields->add_impl(s,t);
              break;
            }
            ++id;
          }
        }
        break;
      }
    }
  }

  void Instr::process_shorthand(Core &core)
  {
    // If no fields exist, then extract from the alias.
    bool add_fields = _fields.empty();   
    bool no_width = (_width == 0);

    if (!add_fields && _tmpalias.size() > 1) {
      PError(loc(),"Only assembler shorthands may have multiple alias targets.");
    }

    if (_alias_action) {
      _shorthand = true;
      _disassemble = false;
      if (!_tmpalias.empty()) {
        PError(loc(),"Simple aliases may not also have alias action code.");
      }
      return;
    }

    bool no_blk = (get_blk().empty());

    ForEach (_tmpalias,ta_iter) {
      TmpAlias *tmpalias = *ta_iter;
      auto alias = new Alias;
      // The instruction named by the alias must exist.
      const Instr *target = const_core.get_instr(tmpalias->_name.c_str());
      alias->_target= target;
      if (!target) {
        PError(loc(),"Unknown alias target:  " << tmpalias->_name);
      }

      // We don't allow the initialization-syntax form for aliases.
      if (!tmpalias->_opcs.empty()) {
        PError(loc(),"The initialization-syntax form is not allowed for aliases.");
      }

      // The size of the shorthand is inherited from the target, unless already set.
      if (no_width) {
        _width = max(_width,target->width());
      }

      if (!_type) {
        _type = target->get_type();
      }
      
      // The block of the shorthand can be inherited from the target.  The
      // target may not.  However, for shorthands with multiple targets, we
      // record all target blocks.
      if (no_blk) { 
        if (!target->get_blk().empty()) {
          _blks.push_back(target->get_blk());
        }
      }

      if (_tmpfields) {
        // sort alias fields by target
        Fields  tmp_fields(tmpalias->_fields);
        StrVect tmp_names(tmpalias->_field_names);
        StrHash  field_idx;
        int      idx = 0;
        ForEach(tmp_fields,f) {
          if (f->_type == Field::Ref) {
            field_idx.insert(make_pair(f->_name,0));
          }
        }
        ForEach(target->_fields,f) {
          auto it = field_idx.find(f->_name);
          if (it != field_idx.end()) {
            it->second = idx++;
          }
        }

        if ((unsigned) idx == tmp_fields.size()) { // else field missing in target an detailed  error mesage later
          idx--; // points to last free idx.

          ForRange(tmp_fields.size(),k) {
            int  to_loc;
            if (tmp_fields[k]._type ==  Field::Ref) {
              auto it = field_idx.find(tmp_names[k]);
              to_loc = it->second;
            } else { // push it at the end
              to_loc = idx--;	      
            }
            tmpalias->_field_names[to_loc] = tmp_names[k];
            tmpalias->_fields[to_loc] = tmp_fields[k];
          }
        }

        process_var_opcode(tmpalias,target);
      }

      // No fields were explicitly defined, so we attempt to extract them from
      // the alias.
      if (add_fields) {
        _shorthand = true;
        // Copy attributes, required for instrtable information, plus we want
        // these shorthands to inherit from the original.  However, we want any
        // items in the shorthand to override what's inherited, thus we don't
        // want to use the new values.
        add_attrs(target->attrs(),false);
      } else {
        // Temporarily populate lookup hash.
        assert(_fieldhash.empty());
        int index = 0;
        ForEach(_fields,i) {
          _fieldhash.insert(make_pair(i->_name,index++));
          if (auto fld = const_core.get_instrfield(i->_name)) {   // TBD - clean up
            ForEach(fld->instrfields(),n) {
              gc_string fname = i->_name + gc_string(".") + n->first;
              _fieldhash.insert(make_pair(fname,index++));
            }
          }
        }

        assert(_opchash.empty());
        index = 0;
        ForEach(_opcs,i) {
          _opchash.insert(make_pair(i->_name,index++));
        }
      }

      StrSet asm_names;
      // Each field must correspond to a field in the target 
      ForRange(tmpalias->_fields.size(),i) {
        Field &afield = tmpalias->_fields[i];
        int index = target->getField(tmpalias->_field_names[i]);
        if (index < 0) {
          PError(loc(),"Target of alias has no field named " << tmpalias->_field_names[i]);
        }
        Field tf = target->getField(index);
        if (!tf._field) {
          PError(loc(),"Target not finalized, probably due to prior error.");
        }
        if (tf._field->is_assembler()) {
          asm_names.insert(tf._field->name());
        }
        bool  is_opc = true;
        if (afield._type == Field::Expr) { // check whether nested field
          Ptree *expr = afield._expr;
          Ptree *l,*r;
          if (PtreeUtil::Match(expr,"[ %? . %? ]",&l,&r)) {
            gc_string  fname = l->ToString() + gc_string(".") + r->ToString();
            if (const_core.get_instrfield(fname)) {
              afield._name = gc_strdup(fname);
              afield._type = Instr::Field::Ref;
            }
          }
        }
        if (afield._name) {
          // Named field in alias- this means that we'll map this field to the
          // target instruction's field.  However, we must check for compatibility.
          // For now, the alias must be smaller than the target to avoid any data loss.
          if (auto sf = const_core.get_instrfield(afield._name)) {
            if (sf != tf._field) {
              if (!sf->is_pseudo() && sf->width() > tf._field->width()) {
                PError(loc(),"Size of field '" << tf._field->name() << "' in target of alias "
                       << "is smaller than field '" << sf->name() << "'.");
              }
            }
            add_alias_field(afield._name,add_fields, is_opc);
            alias->_fields.push_back(afield);
            Field &fa = alias->_fields.back();
            fa._sname = fa._name;
            fa._name  = tf._name;
            fa._field = tf._field;
            // We need to assign its index later, once we've sorted, otherwise
            // they'll be incorrect.
            if (is_opc) {
              fa._type = Field::RValue; //Field::RValue;
            } 
          } else {
            PError(loc(),"Unknown instruction field " << afield._name);
          }
        } else {
          // Unnamed field- use integer value or handle as expression.
          if (afield._type == Field::Expr) {
            VarWalker vw(_env,&core,&core,false);
            alias->_fields.push_back(Field(tf._name,vw.Translate(afield._expr)));
            // Now see if this has caused us to need to add any fields.
            VarList escapes = vw.get_escapes();
            ForEach(escapes,var) {
              // Ignore if it's a special item, such as PC or a bit-mask function.
              if (!validAliasEscape(var->second)) {
                add_alias_field(get_str(var->first,"field name"),add_fields,is_opc);
              }
            }
            if (!vw.usageinfo()._helpers.empty()) {
              // A helper function is used.  Record as a helper used by the assembler.
              // We only allow arbitrary helpers for shorthands, at present, and not for
              // other kinds of aliases.
              if (!_shorthand) {
                PError(loc(),"Non-shorthand aliases may not use helper functions in their expressions.");
              }
              // We can't disassemble arbitrary helper functions, so we turn
              // this off.
              _disassemble = false;
              core.add_asm_helpers(vw.usageinfo()._helpers);
            }
          } else {
            alias->_fields.push_back(Field(tf._name,afield._value));
          }
          Field &fa = alias->_fields.back();
          fa._field = tf._field;
        }
      }

      if (!(tmpalias->_fields.size() + tmpalias->_opcs.size() == target->_fields.size())) {
        if (add_fields) {
          // Add assembler fields
          ForRange(target->_fields.size(),i) {
            Field tf = target->getField(i);
            const InstrField *field = (tf._field->interface() ?  tf._field->interface() : tf._field);
            if (field->is_assembler() && asm_names.find(field->name()) == asm_names.end()) {
              char *fname = gc_strdup(field->name().c_str());
              alias->_fields.push_back(Field(fname));	    
              Field &fa = alias->_fields.back();
              fa._sname = fname;
              fa._name  = tf._name;
              bool is_opc;
              fa._value = add_alias_field(fname,add_fields,is_opc);
              fa._field = tf._field;
            }
          }
        } 
        Field  tf = target->_fields.front();

        if (tf._field && tf._field->is_pseudo()) {
          PError(loc(),"Some target pseudo fields are not mapped in alias:  " << tf._field->name());
        }
      }

      // Sort the fields for completeness: The order in which they're written
      // doesn't matter, by default, rather it's the order of their bits in the
      // encodings which are important.
      sort(alias->_fields.begin(),alias->_fields.end(),FieldOrder());

      _alias.push_back(alias);
    }

    // We're done with all temporary aliases.
    _tmpalias.clear();

    if (!_shorthand && !_disassemble) {
      PError(loc(),"The disassemble option may only be turned off for instruction shorthands.");
    }
  }


  void InstrAlias::print(std::ostream &o,const Instr &instr) const
  {
    o << _target->name() << " ";
    ForEach(_fields,i) {
      switch (i->_type) {
      case InstrBase::Field::Ref:
        o << i->_name << "(" << i->_sname << ")";     
        break;
      case InstrBase::Field::RValue:
        if (i->_sname) {
          o << i->_name << "(" << i->_sname << ")";     
        } else {
          o << i->_name << "(" << i->_field->name() << ")";
        }
        break;
      default:
        o << *i;
        break;
      }
      o << " ";
    }
  }


  void Instr::print_intro(ostream &o,const gc_string &pfx) const
  {
    o << " (rank: " << asm_rank() << ")";
    if (!assemble()) {
      o << " (no-assemble)";
    }        
    if (shorthand()) {
      o << " (shorthand)";
      if (!disassemble()) {
        o << " (no-disassemble)";
      }
    } else if (!is_used() && !is_pseudo() && _instrs.empty()) {
      // If not used by the core, then say so.  Ignore this if we have nested
      // instructions, since it might be the case that a nested instruction is
      // used.  TODO: Check to see if any of the nested instructions are used.
      o << " (not used by core)";
    }
    // Do not output width for contaner for nested instructions.
    if (!has_variable_field()) {
      o << "\n" << pfx << "Width:  " << _width;
      if (_width != _fetch_width) {
        o << "\n" << pfx << "Fetch width:  " << _fetch_width;
      }
    }
    o << "\n";
  }

  void Instr::print_alias(ostream &o,const gc_string &pfx) const
  {
    if (!_alias.empty()) {
      o << pfx << "Alias:  ";
      bool first = true;
      ForEach(_alias,a) {
        list_sep(o,first);
        (*a)->print(o,*this);
      }
      o << "\n";
    }
  }

  // This returns the usage info of the target of an alias, if this is an alias.
  // Otherwise, returns the instruction's own usage info.  The exception is that
  // we can have cases where an alias contains additional usage information,
  // e.g. if it has a field that uses the CIA or fixed values.  In this case,
  // the alias stores all usage information, including the target's, and its own
  // info.
  const UsageInfo &Instr::usageinfo() const
  {
    if (is_simple_alias() && _usageinfo.empty()) {
      return _alias.front()->_target->usageinfo();
    } else {
      return InstrBase::usageinfo();
    }
  }

  Instr*  Instr::gen_instruction(const gc_string& name, SubInstrHash& subinstrs, Core &core) 
  {
    auto instr = new Instr(*this,name);

    instr->_has_subinstrs = true;

    PMap renames;
    // Substitute a SubInstrGrp with its representative member
    ForEach(instr->_init_all,i) {
      Field *f = get_field(*i);
      if (f && !f->isOpcode() && !f->isOpcodeImp() && f->_name) { 
        auto sit = subinstrs.find(f->_name);
        if (sit != subinstrs.end()) {
          const char* oname = f->_name;
          *i = f = f->clone();
          f->_name = sit->second->name().c_str();
          f->_subinstr = _subinstrgrps[sit->first];
          renames.insert(make_pair(oname,new PtreeSubInstrCall(strdup(f->_name),
                                                               *sit->second,
                                                               *f->_subinstr,
                                                               sit->second->usageinfo())));
          instr->add_attrs(sit->second->attrs(),true);
        }
      }
    }

    SubstWalker sw(_env,renames);

    instr->_action = sw.Translate(instr->_action);

    // Modify syntax - replace Sububgroups with their reps.  
    if (instr->_syntax) {
      Syntax &syntax = *(instr->_syntax);
      for(unsigned i = 1; i < syntax._orig->Number(); ++i) {
        Ptree *p = (*syntax._orig)[i];
        auto sit = subinstrs.find(p->ToString());
        if (sit != subinstrs.end()) {
          (*syntax._orig)[i] = Ptree::Make("%s",sit->second->name().c_str());
        }
      }
    }
    
    // Attributes should be inherited
    instr->add_attrs(this->attrs(),true);
    return instr;
  }


  // Computes names for the cross-product of all sub-instruction groups.
  //
  // The algorithm computes the cross-product of all of the sequences of
  // subinstructions (the list in each group).  The algorithm is basically:
  //
  // Given ( (e1, e2, ...) . rest )
  //
  // Compute the cross-product of rest, then prepend e1, e2, etc.
  StrList compute_subinstr_names(const gc_string &pfx,SubInstrGrpHash::const_iterator si_start,SubInstrGrpHash::const_iterator si_end)
  {
    if (si_start == si_end) {
      return StrList();
    } else {
      auto si_tmp = si_start;
      advance(si_tmp,1);

      // Compute the cross-product of everything else.
      StrList cp_rest = compute_subinstr_names("",si_tmp,si_end);

      // For the current element, prepend each element to this cross product.
      const SubInstrGrp &cur = *(si_start->second);
      StrList result;
      ForEach(cur,i) {
        gc_string r = pfx + "_" + (*i)->name();
        if (cp_rest.empty()) {
          result.push_back(r);
        } else {
          ForEach(cp_rest,j) {
            result.push_back(r + *j);
          }
        }
      }
      return result;
    }
  }

  void Instr::expand_subinstructions(Core &core,MsgLog &ml,InstrHash &generated, InstrHash &dummies)
  {
    if( is_succint() ) {
      typedef gc_vector<SubInstrGrp::SubInstrIterator> SubInstrIterators;
      typedef gc_vector<SubInstrGrp*> SubInstrGroups;

      // cartesian product size;
      unsigned instr_cnt = 1; 
      ForEach(_subinstrgrps,i) {
        i->second->finalize(core);
        instr_cnt *=  i->second->size();
      }

      if (!_names.empty()) {
        // If the user has specified instruction names, then there must be a 1-1
        // correspondence of these names to the total number of generated
        // instructions.
        if (instr_cnt != _names.size()) {
          PError(loc(),"Instruction 'names' count different from the size of the cross-product of all sub-instructions.");
        }
      } else {
        // No names were specified, so synthesize the list ourselves.
        auto tmp_names = compute_subinstr_names(_name,_subinstrgrps.begin(),_subinstrgrps.end());
        _names.insert(_names.end(),tmp_names.begin(),tmp_names.end());
      }

      if (!_instrs.empty()) {
        PError(loc(),"Nested instruction can not be used with subinstructions.");
      }

      // generate the instructions
      uint sz = _subinstrgrps.size();
      SubInstrIterators begins(sz);
      SubInstrIterators ends(sz);
      SubInstrIterators iters(sz);
      StrVect           grp_names(sz);
  
      int index = sz-1;
      ForEach(_subinstrgrps,i) {
        begins[index]    = (i->second->begin());
        ends[index]      = (i->second->end());
        grp_names[index] = i->first;
        --index;
      }
      assert(begins.size() == iters.size());
      copy(begins.begin(),begins.end(),iters.begin());
    
      InstrHash new_instrs;
      // Genererate the cross product in lexicographic order      
      ForRange(instr_cnt,id) {
        // Generte an instruction with the current subinstr combination, if legal.
        if (_names[id] != "reserved") {
          SubInstrHash members;
          ForRange(sz,i) {
            members.insert(make_pair(grp_names[i],*iters[i]));
          }
          auto instr = gen_instruction(_names[id],members,core);
	  
          // Now check if need to apply modification from dummies (in the right order)	 
          gc_map <unsigned,Instr* > mods;	  
	  
          // Check generated instr name 
          if (dummies.find(_names[id]) != dummies.end()) {
            auto inst = dummies.find(_names[id])->second;
            mods.insert(make_pair(inst->_dummy,inst));
          }
	  
          // Check subinstr groups names	  
          ForRange(sz,i) {
            gc_string name = (*iters[i])->name();
            if (dummies.find(name) != dummies.end()) {
              auto inst = dummies.find(name)->second;
              mods.insert(make_pair(inst->_dummy,inst));
            }	    
          }
	  
          // Apply modifications
          ForEach(mods,i) {
            instr->apply_modifications(*(i->second));
          }
          generated.insert(make_pair(instr->name(),instr));
          new_instrs.insert(make_pair(instr->name(),instr));
        }
      
        // Generate next lexicographic combination
        ForRange(sz,i) {
          if (++iters[i] != ends[i]) {
            ForRange(i,k) {
              iters[k] = begins[k];
            }
            break;
          }
        }	
      }

      // Now cut syntax list for each of instructions
      bool process_sub_syntax  =  (_syntax != NULL);
      bool process_sub_dsyntax = (_dsyntax != NULL);
      if (_syntax && is_syntax_list(_syntax)) {
        process_syntax_list(_syntax, new_instrs, false);
        process_sub_syntax = false;	
      }
      if (_dsyntax && is_syntax_list(_dsyntax)) {
        process_syntax_list(_dsyntax, new_instrs, true);
        process_sub_dsyntax = false;	
      }
      // Update each instruction with info about whether their syntax gc_strings
      // should be processed.  Skip reserved items, which will have null values.
      ForEach(new_instrs,i) {
        if (i->second) {
          i->second->_process_sub_syntax = process_sub_syntax;
          i->second->_process_sub_dsyntax = process_sub_dsyntax;
        }
      }
    }
  }

  void Instr::finalize(Core &core,MsgLog &ml,InstrHash &generated, InstrHash &dummies) 
  {
    // Apply any modifications to nested instructions, in case this affects
    // aspect expansion or something like that.
    ForEach(_instrs,i) {
      Instr *instr = (i->second);
      gc_map <unsigned,Instr* > mods;	  
      if (dummies.find(instr->name()) != dummies.end()) {
        auto tmp = dummies.find(instr->name())->second;
        mods.insert(make_pair(tmp->_dummy,tmp));
      }
      //Apply modifications
      ForEach(mods,i) {
        instr->apply_modifications(*(i->second));
      }
    }

    // Finalizes the base.  Skip if this contains sub-instructions.
    if (!is_succint()) {
      finalize(core,ml);
    }

    // Finally, make sure that all excluded sources/targets were valid.
    ForEach(_source_excludes,iter) {
      if (!iter->_handled) {
        PError(iter->_src,"Excluded source " << *iter << " is not a valid source for instruction '" << _name << "'.");
      }
    }
    _source_excludes.clear();

    ForEach(_target_excludes,iter) {
      if (!iter->_handled) {
        PError(iter->_src,"Excluded target " << *iter << " is not a valid target for instruction '" << _name << "'.");
      }
    }
    _target_excludes.clear();

    // Then finalize any nested instructions.
    ForEach(_instrs,i) {
      Instr *instr = (i->second);
      instr->add_encap_data(ml,*this);
      instr->finalize_nested(core);
    }
    generated.insert(_instrs.begin(),_instrs.end());
  }

  // Add the outer instruction attributes and blk info
  void Instr::add_encap_data(MsgLog &ml, const Instr &instr) 
  {
    add_attrs(instr.attrs(),true);

    auto &blks = instr.blks();
    bool used = (blks.end() != std::find(blks.begin(), blks.end(), get_blk()));
    if ((!get_blk().empty()) && (!used)) {
      PError(loc(),"Different blk information for outer and nested instructions.");
    } else if (get_blk().empty()) {
      // In order to properly handle block lists in the parent,
      // the parent's block(s) can only be copied when there
      // were no blocks specified in the child.
      _blks.clear();
      if (!instr.get_blk().empty()) {
        _blks.push_back(instr.get_blk());
      }
    }

  }


  void  Instr::apply_modifications(Instr& source) {
    
    // Merge aspects.
    add_aspects(source);

    // Add attributes
    add_attrs(source.attrs(),true);	
    
    // Replace width
    if (source._width) {
      _width = source._width;
    }
    
    // Add fields
    if (!source._all.empty()) {
      _all.insert(_all.end(),source._all.begin(),source._all.end());
    }    

    // Add assembler rules.
    if (!source._asmrules.empty()) {
      _asmrules.insert(_asmrules.end(),source._asmrules.begin(),source._asmrules.end());
    }
    
    // Replace action
    if (source._action) {
      _action = source._action;
    }    

    // Replace syntax
    if (source._syntax) {
      _syntax = new Syntax(*source._syntax);
    } 
    
    // and dsyntax
    if (source._dsyntax) {
      _dsyntax = new Syntax(*source._dsyntax);
    }  
    _remove |= _remove || source._remove;
  }
  
  bool Instr::has_variable_field() const {
    ForEach(_all,i) {                         
      if (i->_field && i->_field->is_variable()) { 
        return true;
      }
    }
    return false;
  }

  void Instr::process_width(unsigned prefix_bits, bool need_encoding) 
  {
    if (has_variable_field()) {
      return;
      // Do not process widht of a instruction which have variable fields
    }
    // Prefix instruction do not have prefix bits.
    unsigned pb = is_prefix() ? 0 : prefix_bits;
    // Default instruction size is 4 bytes.  Set this if not already set.
    bool all_pseudo = true;
    if (!_width) {
      if (_assembler && !_action) {
        return;
      }
      unsigned width = 0;
      ForEach(_all,i) {
        const InstrField &ifield = *(i->_field);
        if (ifield.is_variable()) {
          PError(loc(),"The width of instruction with variable fields must be specified.");
        } else if(ifield.is_pseudo()) {
          width += ifield.width();
        } else if (ifield.highbit() > _width) {
          all_pseudo = false;
          _width = ifield.highbit();
        }
      }
      if (_width == 0) { // all fields are pseudo, use cumulative width
        _width = width;
      }
      _width = ((_width - pb)/8 + 1)*8 + pb; 
    }   
    _fetch_width = _width - pb;

  }

  void Instr::check_blk(const Core& core) 
  {
    if (!_type_name.empty()) {
      if (_prefix) {
        PError(loc(),"Prefix can not use type.");
      }
      if (!(_type = core.get_instr(_type_name))) {
        PError(loc(),"Unknown pseudo instruction: " << _type_name );
      }
      else if (!_type->is_pseudo()) {
        PError(loc(),"Non-pseudo type instruction.");
      }
    }

    if (_prefix) {
      if (!get_blk().empty()) {
        PError(loc(),"Prefix associated with a block.");
      }
      if (_pseudo) {
        PError(loc(),"Prefix can not be pseudo.");
      }
      add_attr(AttrItem(Data::get_attr(PrefixInstrTable,AttrParams::NoParam)));
    } else { // Non Prefix 
      ForEach(_fields,iter) { // Prefixes have fields correspondning to different instructions
        const InstrField *ifield = iter->_field;
        if (ifield) { // only prefix fields may be associated with more than one block
          if (!ifield->blks().empty() && ifield->blks().front() != get_blk())
            PError(loc(),"Inconsistent blocks.");
        }
      }
    }
  }

  void Instr::finalize_tmpfields_alias(const Core &core, InstrFieldHash &genrtdFields) 
  {
    TmpAlias *tmpalias = (_tmpalias.empty()) ? 0 : _tmpalias.front();
    unsigned afields_size = (tmpalias ? tmpalias->_fields.size() : 0);
    unsigned all_size = count_non_tmps(_init_all);

    // Note: We don't check for unmapped pseudo fields if we have nested
    // instructions since there's a semantic conflict between pseudo instructions
    // used with nested instruction aliases.
    InstrBase::finalize_tmpfields(core,genrtdFields,_prefix,_instrs.empty());

    int added = count_non_tmps(_init_all) - all_size;

    if (afields_size && added > 0) { // add alias expression for fields not occuring in alias
      const Instr *target = core.get_instr(tmpalias->_name.c_str());
      if (!target) {
        PError(loc(),"Unknown alias target:  " << tmpalias->_name);
      }
      StrSet tnames;
      ForEach(target->_fields,f) {
        if (f->_name) {
          tnames.insert(f->_name);
        }
      }

      Fields  &afields = tmpalias->_fields;
      StrVect &afnames = tmpalias->_field_names;
      ForRange(((unsigned)added),i) {
        Field  &f = *get_field(_init_all[all_size+i]);
        if (f._type != Field::Ref) {
          continue;
        }
        const char* tname = _tmpfields->get_inv_name(f._name);
        if (tnames.find(tname) == tnames.end()) { // dont add if not in target
          continue;
        }

        bool found = false;
        ForRange(afields_size,k) {
          if (afields[k]._name && !strcmp(afields[k]._name, f._name)) {
            found = true;
            break;
          }
        }
        if (!found) { // add only if not in current fields
          afields.push_back(Field(tname));
          afnames.push_back(tname);
        }
      }
    }
  }

  void Instr::finalize_tmpfields(const Core &core, InstrFieldHash &genrtdFields)
  {
    if (is_complex_alias()) return;
    
    finalize_tmpfields_alias(core, genrtdFields);

    ForEach(_instrs,i) {
      (i->second)->finalize_tmpfields_alias(core,genrtdFields);
    }
  }

  // Substitute any SubInstr with its constituent InstrFields
  void Instr::transfer_fields(Core &core,MsgLog &ml)
  {
    if (_has_subinstrs) {
      auto i = _init_all.begin();
      while (i != _init_all.end()) {
        if (Field *f = get_field(*i)) {
          if (f->_name) {
            if (const SubInstr *subinstr = const_core.get_subinstr(f->_name)) {
              if (!subinstr->get_blk().empty() && subinstr->get_blk() != get_blk()) {
                PError(loc(),"Inconsistent blocks.");
              }
              // Transfer all fields up to, but not including,.this one.
              BFields tmp_all(_init_all.begin(),i);
              // Add in the sub-instruction items.  This is done to the temporary
              // list so that we end up with the proper ordering, e.g. a
              // sub-instruction might appear in the middle of an encoding list.
              ForEach((subinstr->_init_all),a_iter) {
                tmp_all.push_back((*a_iter)->clone());
                tmp_all.back()->_subinstr = f->_subinstr;
              }
              // Then copy over remaining items.
              tmp_all.insert(tmp_all.end(),i+1,_init_all.end());
              // Then swap out the original with the new.
              _init_all.swap(tmp_all);
              // Start over, for safety purposes.
              i = _init_all.begin();
              continue;
            }
          }
        }
        ++i;
      }
    }
  }

  void Instr::process_reserved_bits_value(Core &core, MsgLog &ml )
  {
    // Handle the reserve-bits value.
    if (_reserved_bits_value == RsrvNotSet) {
      _reserved_bits_value = core.reserved_bits_value();
    }
    switch (_reserved_bits_value) {
    case 0:
    case 1:
      break;
    case RsrvNotSet:
      _reserved_bits_value = 0;
      break;
    default:
      AddError(ml,loc(),"The reserved-bits value may only be 0 or 1.");
    }
  }


  // Used before we've fully processed the instruction to check to see if we
  // have micro ops.  We only do this because of ordering problems.  Error checking
  // on bad instruction fields is delayed to InstrBase::process_fields.
  bool Instr::initialMicroOpCheck(const Core &core)
  {
    ForEach(_fields,i) {
      const Field &f = *i;
      if (f._name) {
        if (const InstrField *field = core.get_instrfield(f._name)) {
          if (field->type() == InstrField::tInstr) {
            return true;
          }
        }
      }
    }
    return false;
  }
  
  void Instr::finalize(Core &core, MsgLog &ml )
  {
    assert(_env);

    distr_fields(_fields,_opcs,0,_init_all,_tmpalias.empty(),false);

    bool actionless = _nested || is_alias() || is_pseudo(); // TBD - nested

    _little_endian = (core.instrEndianness() == Core::Little);

    process_reserved_bits_value(core,ml);

    if (!_action && !actionless && !is_prefix() && !initialMicroOpCheck(core)) {
      if (!_assembler) {
        PError(loc(),"Instruction has no action or assembler function and is not an alias, prefix, pseudo, or micro-op based.");
      }
      if (_has_subinstrs) {
        PError(loc(),"Assembler only instruction cannot have subinstructions.");
      }
      if (get_nested().size()) {
        PError(loc(),"Assembler only instruction cannot have nested instructions.");
      }
      if (_all.size()) {
        PError(loc(),"Assembler only instruction cannot have fields.");
      }
      if (width()) {
        PError(loc(),"Assembler only instruction cannot have width.");
      }
      if (is_prefix()) {
        PError(loc(),"Assembler only instruction cannot be prefix instruction.");
      }
      if (is_pseudo()) {
        PError(loc(),"Assembler only instruction cannot be pseudo instruction.");
      }
      if (_names.size()) {
        PError(loc(),"Assembler only instruction cannot have names.");
      }
      if (_syntax) {
        Instr::process_syntax(core);
      }
      if (is_alias()) {
        PError(loc(),"Assembler only instruction cannot be alias.");
      }
      return;
    }
    if (_assembler) {
      if (is_pseudo()) {
        PError(loc(),"Pseudo instructions can not have assembler behavior.");
      }
      if (is_alias()) {
        PError(loc(),"Alias instructions can not have assembler behavior.");
      }
    }
    check_blk(core);

    // If a next-table value is specified, make sure that it's for a valid
    // table.
    if (_next_table) {
      CheckCode cc(_env,core,false,false);
      cc.check_code_filter(_next_table);
      ForEach(cc.escapes(),i) {
        if (!const_core.get_instrfield(i->second)) {
          PError(i->first,"Unknown variable/resouce found in next-table hook: " << i->second);
        }
      }
    }

    // If we have a post-fetch-logic enable, check the code.
    if (_enable_post_fetch == PostFetchFunc) {
      LogPError(ml,{ CheckCode cc(_env,core); cc.check_code(_enable_post_fetch_func); });
    }

    if (_has_subinstrs) {
      // This will handle the renaming of field values, where an opcode is
      // supplied.  We rename them directly to their opcode value.
      PMap opvals;
      ForEach(_opcs,i) {
        opvals.insert(make_pair(i->_name,uint2p(i->_value)));
      }
      SubstWalker sw(_env,opvals);
      _action = sw.Translate(_action);

      if (_process_sub_syntax) {
        process_subsyntax(_syntax,core,false);
      }
      if (_process_sub_dsyntax) {
        process_subsyntax(_dsyntax,core,true);
      }
    }
  
    process_user_src_trg_info(core,ml);

    InstrBase::finalize(ml,core,actionless,is_alias(),assembler(), &_source_excludes, &_target_excludes);
   
    // If the syntax specifier has fields that don't exist in our encoding
    // (they're pseudo fields used for assembly only), and we don't have a
    // specific disassembly gc_string, then we cannot disassemble, since we don't
    // know how to reconstruct those fields.
    if (_syntax && !_dsyntax) {
      ForEach(_syntax->_items,iter) {
        if (is_alias()) {
          ForEach(_alias,a_iter) {
            if (iter->first._field && (*a_iter)->_target && ((*a_iter)->_target->getFieldOrParent(iter->first._field->name()) < 0)) {
              _disassemble = false;
            }
          }
        } else {
          if (iter->first._field && (getFieldOrParent(iter->first._field->name()) < 0)) {
            _disassemble = false;
          }
        }
      }
    }
   
    if(!_action && is_alias()) {
      // We store the union of all usageinfo if this is an alias with multiple
      // targets.
      bool mt = (_alias.size() > 1);
      ForEach(_alias,a_iter) {
        Alias *alias = *a_iter;
        if (!alias->_target) continue;
        
        _branch_type = alias->_target->_branch_type;

        // If we have any fixed values, then this might affect usage information.
        // Scan and store if different from the target.
        bool scan = false;
        ForEach(alias->_fields,i) {
          if ( i->_type == InstrBase::Field::Value) {
            scan = true;
          }
        }
        if (scan) {
          // Set up values, then scan target's action code.
          CheckCode cc(_env,core,true);
          cc._vw.set_excludes(&_source_excludes, &_target_excludes);
          cc._vw.set_instr(alias->_target);
          cc._vw.set_alias_values(alias);
          InstrAspectPred a;
          cc._vw.set_aspects_pred(&a);
          cc.set_is_instr_code(true);
          cc.check_code_filter(alias->_target->action());
          // Merge in usageinfo if our own isn't empty (due to pc-relative fields)
          // or if we discovered something new that differs from the target's
          // original usageinfo, due to the use of values.
          if (!_usageinfo.empty() || mt || !(cc.usageinfo() == alias->_target->usageinfo())) {
            _usageinfo.update(cc.usageinfo());
          }        
        } else if (!_usageinfo.empty() || mt) {
          // If we have usage information and we're an alias, e.g. we have instruction
          // fields which reference the CIA, then we merge in the info from the target
          // of the alias.
          _usageinfo.update(alias->_target->usageinfo());
        }
      }
    }

    // Scan the assembler function for unknown resources
    if (_assembler) {
      const AsmParmHash &asm_parms = core.asm_config().parms();
      CheckCode cc(_env,core,false,false);
      cc.check_code_filter(_assembler);
      if (get_arity(_assembler) != 0) {
        PError(loc(),"The assembler action function takes 0 arguments for non-complex aliases.");
      }
      ForEach(cc.escapes(),i) {
        if (asm_parms.find(i->second) != asm_parms.end()) {
          // Assembler parm are allowed
          continue;
        }
        const InstrField *field = const_core.get_instrfield(i->second);
        if (!field || (!field->is_prefix() && getField(i->second) < 0)) {
          PError(i->first,"Unknown variable/resouce found in assembler code: " << i->first);
        }
      }
    }

    // Handle complex-alias action code.
    if (_alias_action) {
      LogPError(ml,{
          CheckCode cc(_env,core,true,false);
          cc.set_is_asm_code(true);
          cc.check_code(_alias_action);
          if (!arg_is_type(_assembler,"InstrArgs",0)) {
            PError(loc(),"Argument to alias action function must be 'InstrArgs'.");
          }
        });
    }

    // Check that all assembler rules exist

    const AsmConfig &asmcfg = core.asm_config();
    auto const &asm_rules = asmcfg.rules(); 

    ForEach(_asmrules,r) {
      auto it = asm_rules.find(*r);
      if (it == asm_rules.end()) {
        PError(loc(),"Unknown assembler rule found: " << *r);
      }
      if (it->second->type() == AsmRule::Packet) {
        PError(loc(),"Packet rule " << *r << " can not be used for an instruction.");
      }
    }


    ForEach(_all,i) {
      const InstrField &tfield = *(i->_field);
      const InstrField &ifield = (tfield.interface() ? *(tfield.interface()) : tfield);
      if (_pseudo) {
        if (ifield.is_prefix() && !ifield.index()) {
          PError(loc(),"Non indexed prefix field used in pseudo instruction.");
        }
      } else {
        if (i->_type != Field::Resrvd && i->_type != Field::RValue && ifield.is_prefix() != _prefix) {
          PError(loc(),"Inconsistent prefix information:  " 
                 << name() << " is a prefix instruction, but " << tfield.name() << " is not a prefix field.");
        }
      }
      ifield.set_used();
      if (ifield.action()) {
        _disassemble = true;
      }
    }

    if (_dependencies) {
      _dependencies->finalize(ml,core,this);
    }

  }

  void parse_used_resource(UsedReg &ur,const Core &core,Ptree *src)
  {
    // It should be either a register or a register file, possibly
    // with a field specified.
    Ptree *name, *index;
    if ( PtreeUtil::Match(src,"[ %? ( %? ) ]",&name,&index) ) {
      // This should be a register-file with a specified index.
      if (const RegFile *rf = core.get_regfile(name->ToString())) {
        ur.setreg(rf);
        // We might have a range here...
        Ptree *b,*e;
        if (PtreeUtil::Match(index,"[ %? , %? ]",&b,&e)) {
          ur._begin = b;
          ur._end = e;
        } else {
          ur._begin = index;
        }
      } else {
        PError(name,"Resource " << name << " is not a register file.");
      }
    } else {
      const char *name = get_ident(src,"a register/register-file name");
      if (const RegFile *rf = core.get_regfile(name)) {
        ur.setreg(rf);
      } else if (const Reg *reg = core.get_reg(name)) {
        ur.setreg(reg);
      } else {
        PError(src,"Unknown register or register-file " << src);
      }
    }
  }

  // Process information associated with fields whose bits exist in both the
  // instruction's bits and in the prefix's bits.  We only do this if the
  // instruction's bits are actually split between the instruction itself and a
  // prefix.
  void Instr::finalize_split_fields() 
  {
    if (_width == _fetch_width || !_type) {
      return;
    }

    ForEach(_fields,j) {
      InstrField *f = const_cast<InstrField*>(j->_field);
      // The field is split
      if (f && f->highbit() >= fetch_width()) {
        gc_string target_pfield;
        ForEach(_type->_fields,k) {
          const InstrField *tf = k->_field;
          // Right now, we assume that only high-bits go into a prefix and that
          // only one prefix is ever used.  This allows us to simply check to
          // see if the highbit intersects with the bits of the target field.
          // If it does, then we've found the relevant prefix field that
          // contain's this field's upper bits.
          if (tf && tf->bits().intersect(BitPair(f->highbit(),f->highbit()))) {
            target_pfield = (tf->interface() ? tf->interface()->name() : tf->name());
          }
        }
        if (target_pfield.empty()) {
          PError(loc(),"No target prefix field found when analyzing field " << f->name());
        }
        if (!f->splitInfo()){
          f->setSplitInfo(new InstrField::SplitInfo(target_pfield));
        }
        f->splitInfo()->_widths.insert(_width);
      }
    }
  }


  // Process explicit source/target keys.
  void Instr::process_user_src_trg_info(const Core &core,MsgLog &ml)
  {
    // This iterates over the list of excludes, removing them from the source list
    // if they exist.
    ForEach(_orig_source_excludes,i) {
      LogPError(ml,process_exclude(_source_excludes,core,*i));
    }
    _orig_source_excludes.clear();

    // Same for targets.
    ForEach(_orig_target_excludes,i) {
      LogPError(ml,process_exclude(_target_excludes,core,*i));
    }
    _orig_target_excludes.clear();

    // Now process the explicit user-specified sources and targets, if they exist.
    ForEach(_orig_sources,i) {
      LogPError(ml,process_user_resource(_usageinfo._src_regs,core,*i));
    }
    ForEach(_orig_targets,i) {
      LogPError(ml,process_user_resource(_usageinfo._trg_regs,core,*i));
    }
  }
  
  // Processes an element in the source exclude list, as described above.
  void Instr::process_exclude(ResExcludes &excludes,const Core &core,Ptree *src)
  {
    ResExclude ex(src);

    parse_used_resource(ex,core,src);

    // Add to the list to be excluded.
    excludes.insert(ex);
  }

  void Instr::process_user_resource(UsedRegs &usedregs,const Core &core,Ptree *src)
  {
    UsedReg ur;
    parse_used_resource(ur,core,src);
    usedregs.insert(ur);
  }

  //
  void Instr::finalize_nested(const Core &core) 
  {
    // Nested instructions are finalized at two stages once as nested
    // from the parent instruction and once as alias after all instructions
    // have finished.

    _nested = true;
    
    if (!_instrs.empty()) {
      PError(loc(),"Too much nesting.");
    }
    if (_pseudo) {
      PError(loc(),"Nested pseudo instruction.");
    }
    if (_prefix) {
      PError(loc(),"Nested prefix instruction.");
    }
    if (!_subinstrgrps.empty()) {
      PError(loc(),"Nested instruction can not have subgroups.");
    }
  }

  //builds syntax from subinstruction syntaxes
  void Instr::process_subsyntax(Syntax* s, const Core& core, bool dsyntax ) 
  { 
    Syntax &syntax = *s;
    unsigned i = 1;
    unsigned num = syntax._orig->Number();
    gc_string::iterator siter = syntax._fmt.begin();
    gc_string::iterator send  = syntax._fmt.end();
    gc_string  nfmt;
    PtreeArray *norig = new PtreeArray(syntax._orig->Number());
    norig->Append((*syntax._orig)[0]);
    while (siter != send) {
      bool skip = false;
      if (*siter == '%') {
        if (*(siter+1) == 'f' || *(siter+1) == 'p') {
          if (i > num) { // let finalize diagnose error 
            break;
          }
          Ptree *p = (*syntax._orig)[i];
          ++i;
          auto fp = get_reg_ref(p,"syntax field name.",false,env());
          if (const SubInstr *subinstr = core.get_subinstr(fp.first)) {
            Syntax* subsyntax;
            if(dsyntax){	    
              if (!subinstr->_dsyntax) {
                PError(loc(),"No syntax specified for sub-instruction.");
              }
              subsyntax = (subinstr->_dsyntax);
            } else {
              if (!subinstr->_syntax) {
                PError(loc(),"No syntax specified for sub-instruction.");
              }
              subsyntax = (subinstr->_syntax);	    
            }
            for(unsigned j=1; j < subsyntax->_orig->Number(); ++j) {
              norig->Append((*subsyntax->_orig)[j]);
            }
            nfmt += subsyntax->_fmt;
            ++siter;
            skip = true;
          } else {
            norig->Append(p);
          }
        } 
      } 
      if (!skip) {
        nfmt += *siter;
      }
      ++siter;
    }
    syntax._fmt = nfmt;
    syntax._orig = norig;
  }

  unsigned Instr::getNFixed() const {
    unsigned ret = 0;
    if (_syntax) {      
      ForEach( _syntax->_items, i) {
        Field fp = i->first;
        if (fp._type == Instr::Field::Value) {
          ret++;	
        }	
      }    
    } 
    return ret;     
  }

  // If we have a syntax list, then cut it into individual syntaxes.
  void Instr::process_syntax_list(Syntax* s, InstrHash& generated, bool dsyntax) 
  {
    uint32_t index = 0;
    ForRange(_names.size(),i) {
      auto instr = generated[_names[i]];	  
      if (!instr) {
        continue;
      }
	
      auto items = new PtreeArray;
      auto syntax = new InstrBase::Syntax;
      const char* tmp;
      Ptree *p = (*s->_orig)[index];
      if ((tmp = get_str(p,0))) {
        syntax->_fmt = tmp;
        //dummy append, need because all others have it. 
        items->Append(p);
      }
      while (index < s->_orig->Number()-1) {
        index++;
        p = (*s->_orig)[index];
        gc_string str = p->ToString();	        
        gc_string::size_type loc = str.find("%", 0 );
        if (p && loc == gc_string::npos) {
          items->Append(p);
        } 
        else {
          break;
        }	  
      }
      syntax->_orig = items;
      if (dsyntax) {
        instr->_dsyntax = syntax;  
      } else {
        instr->_syntax = syntax;  
      }      
    }
  }

  // This converts the original syntax description (a ptree gc_string and a list of fields)
  // into the Instr::Syntax data structure
  void Instr::process_syntax(Core& core)
  {
    if (_nested && !_alias.empty() && !_syntax) {
      Alias *alias = _alias.front();
      // Nested instruction without syntax inherits it from the outer instruction.
      if (Syntax* s = alias->_target->_syntax) {
        _syntax = new Syntax;
        _syntax->_fmt = s->_fmt;
        ForEach(s->_items,i) {
          gc_string name = i->first._name;
          // Now need to find corresponding instrfield
          ForEach(alias->_fields,ii) {
            if (ii->_name == name) {
              Field fld(ii->_sname);
              int ix = getField(ii->_sname);
              Instr::Field tfp = getField(ix);
              const InstrField &tfield = *(tfp._field);
              // do not forget about nested fields.
              const InstrField *encap_field = tfield.encap_field();
              fld._field = tfp._field;
              fld._outer = encap_field;
              _syntax->_items.push_back(make_pair(fld,i->second));
              tfield.set_used();
              if (encap_field) encap_field->set_used();
              break;
            }
          }
        }
      }     
    } 
    else if(_syntax) {
      InstrBase::process_syntax(core);
      Syntax &syntax = *_syntax;
      Ptree *errloc = (syntax._orig->Number()) ? (*syntax._orig)[0] : loc();
      uint32_t count = process_format(_syntax,_name,true);
      //Check validity of syntax
      gc_set<gc_string> simple_items;
      ForRange(syntax._items.size(),i){        
        auto item = syntax._items[i].first;
        //must be a simple identifier
        if (i < count) {
          if (item._type == Field::Expr){
            PError(errloc,"Control code may correspond only to simple identifier.");
          }
          simple_items.insert(item._field->name());
        }
        //check that expression can be built from starting indentifiers
        else {
          if (item._type == Field::Ref) {	  
            PError(errloc,"Fewer format codes were found than fields listed in the syntax field.");
          } 
          else if (item._type == Field::Expr) {
            SimpleVarWalker vw(_env);
            vw.Translate(item._expr);	  
            VarList vars = vw.get_vars();
            ForEach(vars,var) {
              auto str = var->first->ToString();
              if (simple_items.count(str) == 0 && !validAliasEscape(str) ) {
                PError(errloc,"Expression "<< var->first->ToString() 
                       << " is not handled by any control code.");
              }
            }	    
          } else if (item._type == Field::Alias) {
            if (!item._field || !item._name) {
              PError(errloc,"Wrong instruction field alias target'" << item._name << "'.");
            }
          } else {
            PError(errloc,"Expression must use simple references.");
          }
        }   
      }      
      
      // If we made it to here, we can discard the PtreeArray
      // and store the new format gc_string.
      _syntax->_orig = 0;
    }    
  }
  
  void Instr::process_dsyntax(MsgLog &ml,Core& core) {
    if (!_disassemble) {
      _dsyntax = 0;
      return;
    }
    if (_dsyntax) {       
      InstrBase::ptree_to_syntax(core,_dsyntax);
      uint32_t num_codes = process_format(_dsyntax,_name,false);
      if(num_codes != _dsyntax->_items.size()) {
        PError(loc(),"Invalid dsyntax string.");
      }      
    } 
    else if (_syntax && _syntax->have_to_invert()) {
      invert_syntax(ml,core);   	    
    }
    //now check validity
    if (_dsyntax) {    
      InstrBase::process_dsyntax(ml,core);
    }
  }
  
  //need to know subinstructoins' syntax defined by a list
  bool Instr::is_syntax_list(const Syntax* syntax) {  
    for (unsigned i = 1; i < syntax->_orig->Number(); ++i) {
      Ptree *p = (*syntax->_orig)[i];
      gc_string str = p->ToString();
      gc_string::size_type loc = str.find("%", 0 );
      if (loc != gc_string::npos) {
        return true;
      }  
    }
    return false;  
  }

  RNumber Instr::adjustForLittleEndian(RNumber x,unsigned w,bool swap_for_le) const
  {
    if (is_mixed_endian() && w) {
      x <<= (w - width());
    }
    
    // If little-endian, do a word-swap.
    return (_little_endian && swap_for_le) ? swap_words(x) : x;
  }

  bool Instr::is_mixed_endian() const
  {
    return !is_little_endian() && BitPair::is_little_endian();
  }
  
  RNumber Instr::getOpcode(unsigned width,bool swap_for_le) const
  {
    return getOpcode(width,(shorthand() ? _alias.front() : 0),swap_for_le);
  }

  // Returns an RNumber with all opcode values set.
  RNumber Instr::getOpcode(unsigned width,const Instr::Alias *alias,bool swap_for_le) const
  {
    RNumber opcode(0,width);
    // If this is a shorthand, then we get the value from the target of the alias,
    // plus add in any immediate values.
    if (alias) {
      opcode = alias->_target->getOpcode(width,swap_for_le);
      ForEach(alias->_fields,fiter) {
        if (fiter->_type == Field::Value) {
          RNumber tmp(fiter->_field->setValue(fiter->_value,width),width);
          opcode |= adjustForLittleEndian(tmp,width,swap_for_le);  // Adjust for little endianness
        }
      }
      // Note: Don't adjust for little-endian b/c that was already done in the
      // calls to the alias targets.
      return opcode;
    } else {
      for (unsigned i = 0; i != sizeOpcs(); ++i) {
        // Split fields for opcodes are not allowed, so we don't have to handle them here.
        const Field ifp = getOpc(i);
        RNumber tmp(0,width);
        const InstrField &f = *(ifp._field);
        tmp = f.setValue(ifp._value,width);
        opcode |= tmp;
      }

      return adjustForLittleEndian(opcode,width,swap_for_le);
    }
  }

  RNumber Instr::getReservedMask(unsigned width) const
  {
    return adjustForLittleEndian(getReservedMask(width,(shorthand() ? _alias.front() : 0)),0,true);
  }

  // Returns an RNumber with 1s for all reserved bits (reserved fields and
  // missing fields).
  RNumber Instr::getReservedMask(unsigned width,const Instr::Alias *alias) const
  {
    RNumber rmask(0,width);
    rmask.setAll();

    if (alias) {
      // Doesn't work properly for multiple shorthand targets, so fix later.
      return alias->_target->getReservedMask(width);
    } else {
      ForRange(sizeAll(),i) {
        const Field ifp = getAll(i);
        // Skip reserved fields; clear all else.
        if (ifp._field->is_reserved()) continue;
        auto const &mask = ifp._field->mask();
        // Clear bits defined by the field.
        rmask = rmask & ~mask;
      }
      if (is_mixed_endian()) {
        rmask <<= (width - _width);
      }
    }
    return rmask;
  }


  void Instr::dump(ostream &o,const gc_string &pfx) const
  {
    if (_nested) { // Nested instructions are printed with owner instruction
      return;
    }

    InstrBase::dump(o,pfx);
  
    if (!_doc_op.empty()) {
      o << pfx << "Operation:  " << _doc_op << "\n";
    }

    if (!_subinstrgrps.empty()) {
      o << pfx << "Subinstructions:  \n";
      ForEach(_subinstrgrps,i) {
        i->second->dump(o,pfx);
      }
    }
  
    if (!_names.empty()) {
      o << pfx << "Names:  ";
      printSequence(o,_names,true);
    }
   
    if (_prefix) {
      o << pfx << "Prefix:  " << _prefix << "\n";
    }

    if (_pseudo) {
      o << pfx << "Pseudo:  " << _pseudo << "\n";
    }

    if (_type) {
      o << pfx << "Type:  " << _type_name << "\n";
    }

    if (_reserved_bits_value) {
      o << pfx << "Reserved bits value:  " << (int)_reserved_bits_value << "\n";
    }

    switch (_enable_post_fetch) {
    case PostFetchFalse:
      o << pfx << "Post-fetch logic disabled.\n";
      break;
    case PostFetchFunc:
      o << pfx << "Post-fetch enable:  " << _enable_post_fetch_func << "\n";
      break;
    default:
      ;
    }

    if (_next_table) {
      o << pfx << "Next Table:  " << _next_table << '\n';
    }
   
    if (_assembler) {
      o << pfx << "Assembler:  " << _assembler << "\n";
    }

    if (_alias_action) {
      o << pfx << "Alias Action:  " << _alias_action << "\n";
    }    
 
    if (!_asmrules.empty()) {
      o << pfx << "Assembler rules:  ";
      printSequence(o,_asmrules,true);
    }

    if (_dependencies) {
      o << pfx << "Dependencies:\n";
      _dependencies->dump(o,indent(pfx));
    }
   
    if (!_instrs.empty()) {
      gc_string npfx = indent(pfx);
      o << pfx << "Nested insructions:  \n";
      ForEach(_instrs,i) {
        i->second->_nested = false;
        i->second->dump(o,npfx);
        i->second->_nested = true;
      }
    }
    o << pfx << "-------------------------------\n";
  }


  //
  //  Information about subinstructiongroups
  //


  SubInstrGrp::SubInstrGrp(Environment *env,Ptree *loc,const gc_string &name) :
    Base(env,loc),
    Named(name)
  {
  }


  void SubInstrGrp::finalize(const Core &core) 
  {
    ForEach(_names,i) {
      if ( const SubInstr *subinstr = core.get_subinstr(*i) ) {
        _subinstrs.push_back(const_cast<SubInstr*>(subinstr));
      } else {
        PError(loc(),"Unkown subinstruction " << *i);
      }
    }
  }

  void SubInstrGrp::dump(ostream &o, const gc_string &pfx) const
  {
    o << pfx << "Name:  " << name() << "\n";   
    o << pfx << "Subinstructions:  "; 
    printSequence(o,_names,true);
  }

  //
  // Information about exceptions.
  // 

  ExceptionField::ExceptionField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
    GenericField(env,loc,name)
  {
  }
  
  Exception::Exception(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _priority(0),
    _action(0),
    _enable(0),
    _sensitivity(EdgeSensitive)
  {
  }

  gc_string Exception::get_exception_type() const
  {
    return name() + "_t";
  }

  void Exception::finalize(Core &core,MsgLog &ml)
  {
    assert(_env);

    Attrs::finalize();

    if (!_action) {
      PError(loc(),"No action specified for this exception.");
    }

    ForEach(_fields,i) {
      i->second->finalize(ml);
    }

    LogPError(ml,{
        // Scan the action function for unknown resources.
        CheckCode cc(_env,core,true);
        cc.check_code(_enable);
      });

    // If we have fields, then we expect the action function to take a single
    // argument- a copy of the exception data type.  Otherwise, it should be of
    // arity 0.
    
    LogPError(ml,{
        // Scan the action function for unknown resources.
        CheckCode cc(_env,core,false);
        cc.check_code(_action);
        if (_fields.empty()) {
          unsigned a = get_arity(_action);
          if ( a != 0) {
            AddError(ml,_action,"Expected a function of arity 0 for an exception handler but got a function of arity " << a);
          }
        } else {
          if (!arg_is_type(_action,get_exception_type().c_str(),0)) {
            AddError(ml,_action,"Expected exception handler to take a first argument of type " << get_exception_type());
          }
        }
      });

  }

  void Exception::dump(ostream &o,const gc_string &pfx) const
  {
    gc_string pfx1 = indent(pfx);

    o << pfx << "Name:  " << _name;
    if (_priority) {
      o << " (priority:  " << _priority << ")";
    }
    if (_sensitivity == LevelSensitive) {
      o << " (level sensitive)"; 
    }
    o << "\n";
    Attrs::dump(o,pfx);

    if (!_fields.empty()) {
      o << pfx << "Fields:\n";
      ForEach(_fields,i) {
        i->second->dump(o,pfx1);
      }
    }

    if (_enable) {
      o << pfx << "Enable:  " << _enable << "\n";
    }

    o << pfx << "Action:  " << _action << "\n";

    dump_doc(o,pfx,true);

    o << pfx << "-------------------------------\n";
  }

  //
  // Information about the MMU.
  // 

  MMU::MMU(Environment *env,Ptree *loc) :
    Attrs(env,loc),
    _data_prioritized(false),
    _instr_prioritized(false),
    _interleaved_fetch(true),
    _interleaved_reads(true),
    _interleaved_writes(true)
  {
  }

  MmuLookup *MMU::get_top_mmulookup(const gc_string &name)
  {
    auto iter = _top_lookups.find(name);
    return (iter == _top_lookups.end()) ? 0 : iter->second;
  }

  MmuLookup *MMU::get_any_mmulookup(const gc_string &name)
  {
    auto iter = _all_lookups.find(name);
    return (iter == _all_lookups.end()) ? 0 : iter->second;
  }

  bool MMU::is_lookup_enum(const gc_string &name) const
  {
    ForEach(_all_lookups,i) {
      if (name == i->second->enum_name()) {
        return true;
      }
    }
    return false;
  }

  // Checks that all lookup objects have unique priorities.  Returns false if not.
  // Updates the two priority booleans.
  bool check_priorities (const MmuLookupHash &lookups,bool &instr_p,bool &data_p)
  {
    // First, push everything into a vector, then sort the vector on priorities.
    vector<MmuLookup *> instr,data;
    ForEach(lookups,i) {
      if (!i->second->is_interface()) {
        if (i->second->type() == MmuLookup::Instr || i->second->type() == MmuLookup::Both) {
          instr.push_back(i->second);
        }
        if (i->second->type() == MmuLookup::Data || i->second->type() == MmuLookup::Both) {
          data.push_back(i->second);
        }
      }
    }
    sort(instr.begin(),instr.end(),SortPriority());
    auto u_i = unique(instr.begin(),instr.end(),CompPriority());

    sort(data.begin(),data.end(),SortPriority());
    auto u_d = unique(data.begin(),data.end(),CompPriority());

    instr_p = (u_i == instr.end());
    data_p  = (u_d == data.end());
    return (instr_p && data_p);
  }

  void check_arg_is_addr(HelperInfo &info,Environment *env,Core &core,unsigned num_args,
                         bool return_okay,const char *msg)
  {
    if (info._func) {
      CheckCode cc(env,core,return_okay);
      cc.check_code(info._func);
      info.update(cc.usageinfo());
      ForRange(num_args,arg) {
        if (!arg_is_addr(info._func,arg)) {
          PError(info._func,"Argument " << arg+1 << " of " << msg << " function must be of type 'addr_t'.");
        }
      }
    }
  }

  void check_child_lookups(MsgLog &ml,const MmuLookupHash &lookups,
                           Core &core,const StrVecList &lnames)
  {
    int count = 0;
    ForEach(lookups,i) {
      LogPError(ml,i->second->finalize(ml,core,lnames,count++));
    }
  }

  void populate_child_lookups(MsgLog &ml,MMU &mmu,const MmuLookupHash &lookups)
  {
    ForEach(lookups,i) {
      LogPError(ml,i->second->populate_global_lookups(ml,mmu));
    }
  }

  void MMU::populate_global_lookups(MsgLog &ml)
  {
    populate_child_lookups(ml,*this,_top_lookups);
  }

  void MMU::check_miss_handler(MsgLog &ml,Core &core,HelperInfo &miss_func,const char *msg)
  {
    LogPError(ml,{
        if (miss_func) {
          CheckCode cc(_env,core);
          cc.check_code(miss_func);
          if (!arg_is_addr(miss_func._func,0)) {
            AddError(ml,miss_func._func,"Argument 1 of " << msg << " miss-handler must be of type addr_t.");
          }
          if (!arg_is_uint(miss_func._func,1)) {
            AddError(ml,miss_func._func,"Argument 2 of " << msg << " miss-handler must be of type unsigned int.");
          }
        }
      });
  }

  void MMU::finalize(MsgLog &ml,Core &core)
  {
    Attrs::finalize();

    StrVecList lnames;
  
    check_child_lookups(ml,_top_lookups,core,lnames);

    // We know that the functions will be transformed, thus resulting in
    // different ptrs, so we figure out if they're identical first and then
    // treat them that way.
    if (_instr_enable._func == _data_enable._func) {
      LogPError(ml,{ CheckCode cc(_env,core); cc.check_code(_instr_enable); });
      _data_enable = _instr_enable;
    } else {
      LogPError(ml,{ CheckCode cc(_env,core); cc.check_code(_instr_enable); });
      LogPError(ml,{ CheckCode cc(_env,core); cc.check_code(_data_enable); });
    }

    LogPError(ml,{
        if (_multi_hit) {
          CheckCode cc(_env,core);
          cc.check_code(_multi_hit);
          if (!arg_is_type(_multi_hit._func,"TransType",0)) {
            AddError(ml,_multi_hit._func,"Argument 1 of multi-hit-handler must be of type TransType.");
          }
          if (!arg_is_addr(_multi_hit._func,1)) {
            AddError(ml,_multi_hit._func,"Argument 2 of multi-hit-handler must be of type addr_t.");
          }
          if (!arg_is_uint(_multi_hit._func,2)) {
            AddError(ml,_multi_hit._func,"Argument 3 of multi-hit-handler must be of type unsigned int.");
          }
        }
      });

    LogPError(ml,{
        if (_final_hit) {
          CheckCode cc(_env,core);
          cc.check_code(_final_hit);
          if (!arg_is_type(_final_hit._func,"TransType",0)) {
            AddError(ml,_final_hit._func,"Argument 1 of multi-hit-handler must be of type TransType.");
          }
          if (!arg_is_addr(_final_hit._func,1)) {
            AddError(ml,_final_hit._func,"Argument 2 of multi-hit-handler must be of type addr_t.");
          }
          if (!arg_is_uint(_final_hit._func,2)) {
            AddError(ml,_final_hit._func,"Argument 3 of multi-hit-handler must be of type unsigned int.");
          }
          if (!arg_is_uint(_final_hit._func,3)) {
            AddError(ml,_final_hit._func,"Argument 4 of multi-hit-handler must be of type unsigned int.");
          }
        }
      });

    check_miss_handler(ml,core,_instr_miss,"instruction translation");
    check_miss_handler(ml,core,_data_miss,"data translation");
    check_miss_handler(ml,core,_load_miss,"load translation");
    check_miss_handler(ml,core,_store_miss,"store translation");

    LogPError(ml,check_arg_is_addr(_aligned_write,_env,core,2,false,"misaligned-write"));

    LogPError(ml,check_arg_is_addr(_misaligned_write,_env,core,2,false,"misaligned-write"));

    LogPError(ml,check_arg_is_addr(_misaligned_read,_env,core,2,false,"misaligned-read"));

    LogPError(ml,check_arg_is_addr(_pre_read,_env,core,2,false,"pre-read"));

    LogPError(ml,check_arg_is_addr(_post_read,_env,core,2,false,"post-read"));

    LogPError(ml,check_arg_is_addr(_pre_write,_env,core,2,false,"pre-write"));

    LogPError(ml,check_arg_is_addr(_post_write,_env,core,2,false,"post-write"));

    // We don't allow pre hooks if interleaving is on, since the definition if
    // the pre hook is that they execute after translations but before the
    // memory access, which can't be the case with interleaving.
    if (_interleaved_reads && _pre_read) {
      AddError(ml,loc(),"A pre-read hook is not allowed when interleaved reads are enabled.");
    }
    if (_interleaved_writes && _pre_write) {
      AddError(ml,loc(),"A pre-write hook is not allowed when interleaved writes are enabled.");
    }

    if (!check_priorities(_top_lookups,_instr_prioritized,_data_prioritized)) {
      if (!multi_hit_search()) {
        AddError(ml,loc(),"Not all lookup objects have unique priorities and no multi-hit or final-hit handler was specified.");
      }
    }

    if ((_instr_prioritized || _data_prioritized) && _multi_hit && !_final_hit) {
      AddWarning(ml,loc(),"Priorities and a multi-hit handler were specified.  The multi-hit handler will be ignored.");
    }

    if (_top_lookups.empty()) {
      AddError(ml,loc(),"No lookups were defined for the MMU.");
    }
  }

  void MMU::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "MMU:\n";

    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    Attrs::dump(o,pfx1);

    o << '\n' << pfx1 << "Interleaved fetch :  " << _interleaved_fetch
      << '\n' << pfx1 << "Interleaved reads :  " << _interleaved_reads
      << '\n' << pfx1 << "Interleaved writes:  " << _interleaved_writes << "\n";

    if (_instr_enable._func == _data_enable._func) {
      o << '\n' << pfx1 << "Enable:  ";
      if (_instr_enable) {
        _instr_enable.print(o,pfx2);
      } else {
        o << "<always>";
      }
    } else {
      o << '\n' << pfx1 << "Instr enable:  ";
      if (_instr_enable) {
        _instr_enable.print(o,pfx2);
      } else {
        o << "<always>";
      }
      o << '\n' << pfx1 << "Data enable:  ";
      if (_data_enable) {
        _data_enable.print(o,pfx2);
      } else {
        o << "<always>";
      }
    }

    if (_multi_hit) {
      o << '\n' << pfx1 << "Multi-hit action:  ";
      _multi_hit.print(o,pfx2);
    }

    if (_final_hit) {
      o << '\n' << pfx1 << "Final-hit action:  ";
      _final_hit.print(o,pfx2);
    }

    if (_instr_miss._func == _data_miss._func && !_load_miss && !_store_miss) {
      if (_instr_miss) {
        o << '\n' << pfx1 << "Miss action:  ";
        _instr_miss.print(o,pfx2);
      }
    } else {
      if (_instr_miss) {
        o << '\n' << pfx1 << "Instr miss action:  ";
        _instr_miss.print(o,pfx2);
      }
      if (_data_miss) {
        o << '\n' << pfx1 << "Data miss action:  ";
        _data_miss.print(o,pfx2);
      }
      if (_load_miss) {
        o << '\n' << pfx1 << "Load miss action:  ";
        _load_miss.print(o,pfx2);
      }
      if (_store_miss) {
        o << '\n' << pfx1 << "Store miss action:  ";
        _store_miss.print(o,pfx2);
      }
    }

    if (_aligned_write) {
      o << '\n' << pfx1 << "Aligned-write action:  ";
      _aligned_write.print(o,pfx2);
    }

    if (_misaligned_write) {
      o << '\n' << pfx1 << "Misaligned-write action:  ";
      _misaligned_write.print(o,pfx2);
    }

    if (_misaligned_read) {
      o << '\n' << pfx1 << "Misaligned-read action:  ";
      _misaligned_read.print(o,pfx2);
    }

    if (_pre_read) {
      o << '\n' << pfx1 << "Pre-read action:  ";
      _pre_read.print(o,pfx2);
    }

    if (_post_read) {
      o << '\n' << pfx1 << "Post-read action:  ";
      _post_read.print(o,pfx2);
    }

    if (_pre_write) {
      o << '\n' << pfx1 << "Pre-write action:  ";
      _pre_write.print(o,pfx2);
    }

    if (_post_write) {
      o << '\n' << pfx1 << "Post-write action:  ";
      _post_write.print(o,pfx2);
    }

    if (!_top_lookups.empty()) {
      ForEach(_top_lookups,i) {
        i->second->dump(o,pfx1);
      }
    }

    if (!_doc.empty()) {
      o << pfx1 << _doc << "\n";
    }

    o << pfx << "-------------------------------\n";
  }

  //
  // Information about an MMU lookup test set.
  //

  MmuTestSet::MmuTestSet(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name)
  {}

  MmuTestSet *MmuTestSet::clone() const
  {
    return new MmuTestSet(*this);
  }

  void MmuTestSet::finalize(MsgLog &ml,MmuLookup &lookup,Core &core,const StrVecList &lnames)
  {
    core.set_mmu_handling(true);
    LogPError(ml,{ 
        CheckCode cc(_env,core); 
        cc.check_code(_enable); 
        if (!arg_is_type(_enable._func,"TransType",0)) {
          PError(_enable._func,"Expected an argument of type TransType for test-set enable predicate.");
        }
      });

    ForEach(_tests,t) {
      LogPError(ml,*t = lookup.check_test(*t,core,lnames));    
    }
    core.set_mmu_handling(false);
  }

  void MmuTestSet::dump(std::ostream &o,const gc_string &pfx,bool print_name) const
  {
    gc_string pfx1;
    if (print_name) {
      o << pfx << "Test Set:  " << _name;

      pfx1 = indent(pfx);
    } else {
      pfx1 = pfx;
    }

    gc_string pfx2 = indent(pfx1);

    Attrs::dump(o,pfx1);

    if (_enable) {
      o << '\n' << pfx1 << "Enable:  ";
      _enable.print(o,pfx2);
    }

    ForEach(_tests,i) {
      o << '\n' << pfx1 << *i;
    }
  }

  //
  // Information about an MMU lookup type.
  // 

  MmuLookup::MmuLookup(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _id(0),
    _type(Both),
    _priority(0),
    _interface(false),
    _iparent(0),
    _array_entries(0),
    _array_assoc(0),
    _size(-1),
    _sizescale(1),
    _sizeoffset(0),
    _sizeshift(0),
    _sizetype(None),
    _pageshift(0),
    _lower_bound(0),
    _upper_bound(0),
    _bound_mask(0),
    _tmp_test_order(0),
    _entry_mem_size(0)
  {
  }

  gc_string MmuLookup::type_name() const
  {
    return _name + "_t";
  }

  gc_string MmuLookup::enum_name() const
  {
    return _name + "_id";
  }

  StrVect MmuLookup::all_type_names() const
  {
    StrVect n;
    for (const MmuLookup *iter = this; iter; iter = iter->iparent()) {
      n.push_back(iter->type_name());
    }
    return n;
  }

  // Perform a deep-copy on the test sets so that defmods on a single lookup
  // won't affect others.
  void MmuLookup::copy_tests(const MmuLookup &x)
  {
    _tests.clear();
    ForEach(x._tests,i) {
      _tests.insert(make_pair(i->first,i->second->clone()));
    }
  }

  void MmuLookup::copy(const MmuLookup &x)
  {
    Environment *tmp_env = _env;
    gc_string tmp_name = _name;
    Ptree *tmp_loc = _loc;
    bool tmp_int = _interface;

    *this = x;
    copy_tests(x);

    _env = tmp_env;
    _name = tmp_name;
    _loc = tmp_loc;
    _interface = tmp_int;
    _iparent = &x;
  }

  const MmuField *MmuLookup::find_field(const gc_string &name) const
  {
    MmuFieldHash::const_iterator x = _set_fields.find(name);
    if (x != _set_fields.end()) {
      return x->second;
    } else {
      x = _way_fields.find(name);
      if (x != _way_fields.end()) {
        return x->second;
      } else {
        return 0;
      }
    }
  }

  const MmuField *MmuLookup::find_field(Opencxx::Ptree *value,const char *msg) const
  {
    if (const char *name = get_simple_ident(value,0)) {
      if (auto f = find_field(name)) {
        return f;
      }
    }
    if (msg) {
      PError(value,"Bad field name for " << msg);
    } else {
      return 0;
    }
  }

  // Compare two field hashes.
  bool operator==(const MmuFieldHash &x,const MmuFieldHash &y)
  {
    auto xiter = x.begin();
    auto yiter = y.begin();
    for ( ; xiter != x.end(); ++xiter, ++yiter) {
      if (yiter == y.end()) {
        // y is too small case.
        return false;
      }
      const gc_string &s1 = xiter->first;
      const gc_string &s2 = yiter->first;
      if ( s1 != s2 ) {
        return false;
      }
      const MmuField &f1 = *(xiter->second);
      const MmuField &f2 = *(yiter->second);
      if ( !(f1 == f2)) {
        return false;
      }
    }
    if (yiter != y.end()) {
      // y is too large case.
      return false;
    }
    return true;
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a lookup across cores.
  bool MmuLookup::operator==(const MmuLookup &x) const
  {
    if (_array_entries != x._array_entries) {
      return false;
    }
    if (_array_assoc != x._array_assoc) {
      return false;
    }

    if (!(_set_fields == x._set_fields)) {
      return false;
    }
    if (!(_way_fields == x._way_fields)) {
      return false;
    }

    return true;
  }

  struct FindInStrVect {
    gc_string _n;

    FindInStrVect(const gc_string &n) : _n(n) {};

    bool operator()(const StrVect &v) {
      return find(v.begin(),v.end(),_n) != v.end();
    }
  };

  // Checks code for unknown resources.  Fields are allowed.
  bool MmuLookup::check_code(Ptree *func,CheckCode &cc,bool preds_okay,const StrVecList *lnames)
  {
    if (func) {
      StrSet exceptions;
      cc.check_code_filter(func);
      ForEach(cc.escapes(),i) {
        if ((i->second == "ea" || i->second == "Instr" || i->second == "Data" || i->second == "Load" || i->second == "Store") && preds_okay) {
          // Special predicates that we allow.
          continue;
        }
        if (find_field(i->second) == 0) {
          if (!lnames || (find_if(lnames->begin(),lnames->end(),FindInStrVect(i->second)) == lnames->end())) {
            PError(i->first,"Unknown variable/resource found in action code:  " << i->first);
          }   
        }
      }
      return cc.has_parms();
    } else {
      return false;
    }
  }

  bool MmuLookup::check_code(Ptree* &result,Ptree *func,Core &core,bool return_okay,bool reg_targets_okay,
                             bool preds_okay,const StrVecList *lnames)
  {
    CheckCode cc(_env,core,return_okay,reg_targets_okay);
    bool rc = check_code(func,cc,preds_okay,lnames);
    result = cc.translated();
    return rc;
  }

  bool MmuLookup::check_code(HelperInfo &info,Core &core,bool return_okay,bool reg_targets_okay,
                             bool preds_okay,const StrVecList *lnames)
  {
    CheckCode cc(_env,core,return_okay,reg_targets_okay);
    bool rc = check_code(info._func,cc,preds_okay,lnames);
    info.update(cc.usageinfo());
    info._func = cc.translated();
    return rc;
  }

  // Checks that the function's arguments match the list of lookup names we have
  // stored.  Adds errors to the message log.  Returns the number of arguments
  // checked, i.e.  the return value is the first argument that's not part of this
  // list.
  // extra_args is the count of extra arguments to be expected, in addition to the number of
  // arguments required by the size of lnames.
  int MmuLookup::check_lookup_args(MsgLog &ml,Ptree *func,const StrVecList &lnames,
                                   int extra_args,const char *msg)
  {
    LogPError(ml,get_func(func,lnames.size()+extra_args,msg));
    int c = 0;
    ForEach (lnames,i) {
      if (!arg_is_type(func,*i,c) && !(!c && arg_is_type(func,"TransType",c))) {
        AddError(ml,func,"Argument " << c+1 << " of " << msg << " must be of type(s) '" << getSequence(*i) << "'.");
      }
      c++;
    }
    return c;
  }

  void MmuLookup::check_lookup_func(MsgLog &ml,HelperInfo &info,Core &core,
                                    const StrVecList &lnames,const char *msg)
  {
    if (info._func) {
      LogPError(ml,{ 
          CheckCode cc(_env,core,false);
          cc.check_code(info);
          int h = check_lookup_args(ml,info._func,lnames,2,msg);
          if (!arg_is_addr(info._func,h)) {
            AddError(ml,info._func,"Argument " << h+1 << " of " << msg << " must be of type addr_t.");
          }
          ++h;
          if (!arg_is_uint(info._func,h)) {
            AddError(ml,info._func,"Argument " << h+1 << " of " << msg << " must be of type unsigned.");
          }
        });
    }
  }

  Ptree *MmuLookup::check_test(Ptree *test,Core &core,const StrVecList &lnames)
  {
    const char *type = get_ident(get_func_call_name(test)," test name.");
    if (!strcmp(type,"Bounded")) {
      // Must have two or three arguments.  Arguments may contain fields.
      int numargs = get_func_call_numargs(test);
      if (!(numargs == 2 || numargs == 3)) {
        PError(test,"The Bounded test takes two or three argumnts.");
      }
      Ptree *arg2;
      Ptree *arg  = get_func_call_arg(test,0);
      check_code(arg2,arg,core,false,true,true);
      _lower_bound = arg2;
      if (arg != arg2) {
        test = PtreeUtil::Subst(arg2,arg,test);
      }

      arg  = get_func_call_arg(test,1);
      check_code(arg2,arg,core,false,true,true);
      _upper_bound = arg2;
      if (arg != arg2) {
        test = PtreeUtil::Subst(arg2,arg,test);
      }

      arg  = get_func_call_arg(test,2);
      if (arg) {
        check_code(arg2,arg,core,false,true,true);
        _bound_mask = arg2;
        if (arg != arg2) {
          test = PtreeUtil::Subst(arg2,arg,test);
        }
      }
    }
    else if (!strcmp(type,"Check")) {
      // Must have a single argument.  Argumnt may contain fields.
      int numargs = get_func_call_numargs(test);
      if (numargs != 1) {
        PError(test,"The Check test takes a single argumnt.");
      }
      Ptree *arg  = get_func_call_arg(test,0);
      Ptree *arg2;
      check_code(arg2,arg,core,false,true,true);
      if (arg != arg2) {
        test = PtreeUtil::Subst(arg2,arg,test);
      }
    }
    else if (!strcmp(type,"Compare")) {
      // Must have 2 or more args.  First arg is a field, remaining arguments may not
      // contain fields.
      int numargs = get_func_call_numargs(test);
      if (numargs < 2) {
        PError(test,"The Compare test takes 2 or more arguments.");
      }
      if (find_field(get_func_call_arg(test,0),0) == 0) {
        PError(test,"The first argument to the Compare test must be a field name.");
      }
      for (int i = 1; i != numargs; ++i) {
        Ptree *arg  = get_func_call_arg(test,i);
        Ptree *arg2;
        check_code(arg2,arg,core,false,true,true,&lnames);
        if (arg != arg2) {
          test = PtreeUtil::Subst(arg2,arg,test);
        }
      }
    }
    else if (!strcmp(type,"AddrIndex")) {
      // Must have two arguments which are bit indices.
      int numargs = get_func_call_numargs(test);
      if (numargs != 2) {
        PError(test,"The AddrIndex test takes 2 arguments.");
      }      
      int start = get_uint(get_func_call_arg(test,0)," first argument of AddrIndex test.",env());
      int stop = get_uint(get_func_call_arg(test,1)," second argument of AddrIndex test.",env());
      if (stop < start) {
        PError(test,"Bit range must be in increasing order.");
      }
      // Now see if the range fits the array.  We also only allow this for a set-assoc of 1.
      if (_array_assoc != 1) {
        PError(test,"The AddrIndex test is only allowed for arrays with a set-associativity of 1.");
      }
      unsigned size = 1 << (stop - start + 1);
      if (size > _array_entries) {
        PError(test,"Bit range for AddrIndex test is larger than the size of the array.");
      }
      // The core's ea_mask must be constant in order to use this test (for now).
      if (!core.ea_mask().is_const()) {
        PError(test,"The AddrIndex test is only allowed for cores with constant effective-address masks.");
      }
    }
    else if (!strcmp(type,"AddrComp")) {
      // This test takes a single argument that identifies a field.
      int numargs = get_func_call_numargs(test);
      if (numargs != 1) {
        PError(test,"The AddrComp test takes 1 argument.");
      }
      if (find_field(get_func_call_arg(test,0),0) == 0) {
        PError(test,"The first argument to the AddrComp test must be a field name.");
      }
      if (_sizetype == None) {
        PError(test,"No page-size defined but AddrComp test was specified.");
      }
    }
    else {
      PError(test,"Unknown lookup matching test:  " << test);
    }
    return test;
  }

  void MmuLookup::check_sharing_okay() const
  {
    // We don't allow a TLB to be shared if it has no array, since in that case,
    // there's nothing to be shared.
    if (!has_array()) {
      PError(loc(),"Lookup " << name() << " cannot be shared because it has no array defined.");
    }
  }

  void MmuLookup::populate_global_lookups(MsgLog &ml,MMU &mmu)
  {
    // Add ourselves to the global lookup hash in the MMU.  Check for any conflicts.
    if ( !mmu._all_lookups.insert(make_pair(name(),this)).second ) {
      AddError(ml,loc(),"Lookup conflicts with another lookup of the same name");
    } else {
      populate_child_lookups(ml,mmu,_lookups);
    }
  }

  unsigned MmuLookup::min_page_size() const
  {
    switch (sizetype()) {
    case MmuLookup::Const:
      // Constant value, so just return that.
      return pagesize();
    case MmuLookup::BitSize:
      // Assume a size of 0 and calculate the size from that.
      return ~(~((1ULL << (sizescale() * sizeoffset())) - 1) << sizeshift()) + 1;
    case MmuLookup::LeftMask:
      return ~(~0ULL << sizeshift()) + 1;
    default:
      return 0;
    }
  }

  void MmuLookup::finalize(MsgLog &ml,Core &core,const StrVecList &lnames,unsigned id)
  {
    _id = id;

    Attrs::finalize();

    StrVecList mylnames = lnames;
    mylnames.push_front(all_type_names());

    // The names of set fields and way fields may not overlap.
    ForEach(_set_fields,i) {
      if (_way_fields.count(i->first)) {
        AddError(ml,loc(),"Name conflict between set field and way field of name '" << i->first << "'.");
      }
    }

    // Handle the page-size stuff:  If we have a pagesize name, find it and
    // store its index.
    LogPError(ml,{
        if (_sizefield) {
          _sizefield._d = find_field(_sizefield._p,"page size.");
        }
      });

    // Handle the real-page.
    LogPError(ml,{
        if (_realpage) {
          _realpage._d = find_field(_realpage._p,"real-page.");
        }
      });

    if (_sizetype == BitSize || _sizetype == LeftMask) {
      if (!_sizefield) {
        AddError(ml,loc(),"Size is not constant and no size field was specified.");
      }
    }

    ForEach(_set_fields,i) {
      i->second->finalize(ml);
    }
    ForEach(_way_fields,i) {
      i->second->finalize(ml);
    }

    if (!_interface) {
      // Page-size is required if we're a leaf-node.  If realpage is specified,
      // then page-size must be specified.
      if (_lookups.empty()) {
        if (_realpage._d != 0) {
          if (_sizetype == None) {
            AddError(ml,loc(),"No page-size specified for this lookup object.");
          } else {
            if (_sizetype != Const && !_sizefield) {
              AddError(ml,loc(),"The page-size is not constant but no field was specified.");
            }
          }
        }
      }

      // Each test must be a recognized type.  We also want to check each parameter
      // to make sure it doesn't reference unknown things.
      ForEach(_tests,t) {
        t->second->finalize(ml,*this,core,mylnames);
      }

      // If we have a Bounded test, then that's not compatible with the concept
      // of address translation.
      if (bounded() && (_sizetype != None)) {
        AddError(ml,loc(),"The use of a Bounded test along with a size-type is not supported.");
      }

      // Now process the test order, if specified.  If no order is specified, then
      // we only allow a single test set.  Otherwise, each test, except the last,
      // must have an enable.  The last one cannot have an enable, since it's the
      // "else" clause.
      if (_tmp_test_order) {
        ForEach((*_tmp_test_order),i) {
          const gc_string &tname = *i;

          IfFind(iter,_tests,tname) {
            _test_order.push_back(iter->second);
          } else {
            AddError(ml,loc(),"Unknown test-set " << tname);
          }
        }

        // Each item, except the last, should have an enable predicate.
        if (_test_order.empty()) {
          AddError(ml,loc(),"Empty test order list.");
        }
        auto last = _test_order.end();
        last--;
        for (auto iter = _test_order.begin(); iter != last; ++iter) {
          if (!(*iter)->enable()) {
            AddError(ml,loc(),"Test-set " << (*iter)->name() << " has no enable predicate.");
          }
        }

        // We don't need this anymore.
        _tmp_test_order = 0;
      } else {
        if (valid_translating_lookup()) {
          if (_tests.empty()) {
            AddError(ml,loc(),"No tests were defined for this lookup.");
          }
          else if (_tests.size() == 1) {
            _test_order.push_back(_tests.begin()->second);
          } else {
            AddError(ml,loc(),"More than one test set exists for this lookup, but no test order was defined.");
          }
        }
      }
    }

    // If we have a lookup which is not fully associative, then we must have a constant
    // size.
    if (sizetype() == Const) {
      if (!is_power_2(pagesize())) {
        AddError(ml,loc(),"Page size must be a power of 2.");
      }
    } else if (!fully_assoc() && (sizetype() != None)) {
      AddError(ml,loc(),"Lookups which are not fully associative must have constant-sized translations.");
    }

    LogPError(ml,{
        if (_multi_hit) {
          CheckCode cc(_env,core);
          cc.check_code(_multi_hit);
          if (!arg_is_type(_multi_hit._func,"TransType",0)) {
            AddError(ml,_multi_hit._func,"Argument 1 of multi-hit-handler must be of type TransType.");
          }
          if (!arg_is_addr(_multi_hit._func,1)) {
            AddError(ml,_multi_hit._func,"Argument 2 of multi-hit-handler must be of type addr_t.");
          }
          if (!arg_is_uint(_multi_hit._func,2)) {
            AddError(ml,_multi_hit._func,"Argument 3 of multi-hit-handler must be of type unsigned int.");
          }
        }
      });

    LogPError(ml,{
        if (_final_hit) {
          CheckCode cc(_env,core);
          cc.check_code(_final_hit);
          if (!arg_is_type(_final_hit._func,"TransType",0)) {
            AddError(ml,_final_hit._func,"Argument 1 of final-hit-handler must be of type TransType.");
          }
          if (!arg_is_addr(_final_hit._func,1)) {
            AddError(ml,_final_hit._func,"Argument 2 of final-hit-handler must be of type addr_t.");
          }
          if (!arg_is_uint(_final_hit._func,2)) {
            AddError(ml,_final_hit._func,"Argument 3 of final-hit-handler must be of type unsigned int.");
          }
          if (!arg_is_uint(_final_hit._func,3)) {
            AddError(ml,_final_hit._func,"Argument 4 of final-hit-handler must be of type unsigned int.");
          }
        }
      });
  
    // Check for unknown resources for hit and miss handlers.
    {
      core.set_mmu_handling(true);
      StrVecList hitlnames = mylnames;
      hitlnames.push_front(StrVect(1,"TransType"));
      check_lookup_func(ml,_hit,core,hitlnames,"hit-handler function");
      core.set_mmu_handling(false);
    }

    LogPError(ml,{
        check_code(_valid,core,true,false,false);
      });

    // Check the miss handler.
    LogPError(ml,{
        if (_miss) {
          CheckCode cc(_env,core);
          cc.check_code(_miss);
          if (!arg_is_type(_miss._func,"TransType",0)) {
            AddError(ml,_miss._func,"Argument 1 of miss-handler must be of type TransType.");
          }
          if (!arg_is_addr(_miss._func,1)) {
            AddError(ml,_miss._func,"Argument 2 of miss-handler must be of type addr_t.");
          }
          if (!arg_is_uint(_miss._func,2)) {
            AddError(ml,_miss._func,"Argument 3 of miss-handler must be of type unsigned int.");
          }
        }
      });

    // Check the reset handler.
    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_reset);
      });

    // If we don't have an array, and we're not an interface or have a priority
    // of 'ignore', then we must have a miss handler, which will always be
    // executed.
    if (!_interface && valid_priority()) {
      if (!has_array() && !_miss) {
        AddError(ml,loc(),"Lookup has no array defined and has no miss handler defined.");
      }
    }

    // Check the permission checkers.
    core.set_mmu_handling(true);
    check_lookup_func(ml,_exec_perm,core,mylnames,"execute permission function");
    check_lookup_func(ml,_load_perm,core,mylnames,"data-load permission function");
    check_lookup_func(ml,_store_perm,core,mylnames,"data-store permission function");
    core.set_mmu_handling(false);

    // Handle child nodes.
    check_child_lookups(ml,_lookups,core,mylnames);

    // Child lookup objects must have unique priorities- we don't support
    // multi-hit handlers here.
    bool i,d;
    if (!_interface && !check_priorities(_lookups,i,d)) {
      AddError(ml,loc(),"All child lookup objects must have unique priorities.");
    }

    // We must have an integral number of sets and those must be a power of 2 if we're not
    // fully associatiove.
    if (has_array()) {
      if (_array_assoc) {
        if (_array_entries % _array_assoc) {
          AddError(ml,loc(),"The number of sets (entries/assoc) must be an integral number.");
        }
        if (!is_power_2(sets())) {
          AddError(ml,loc(),"The number of sets must be a power of 2 if the lookup array is not fully associative.");
        }
      }
    }

    if (_mem_read) {
      CheckCode cc(_env,core,true);
      if (!arg_is_uint(_mem_read._func,0)) AddError(ml,loc(),"First argument to mmu lookup mem_read function must be an unsigned integer.");
      if (!arg_is_uint(_mem_read._func,1)) AddError(ml,loc(),"Second argument to mmu lookup mem_read function must be an unsigned integer.");
      if (!arg_is_addr(_mem_read._func,2)) AddError(ml,loc(),"Third argument to mmu lookjup mem_read function must be addr_t.");
      cc.check_code(_mem_read);
    }

    if (_mem_write) {
      CheckCode cc(_env,core,true);
      if (!arg_is_uint(_mem_write._func,0)) AddError(ml,loc(),"First argument to mmu lookup mem_write function must be an unsigned integer.");
      if (!arg_is_uint(_mem_write._func,1)) AddError(ml,loc(),"Second argument to mmu lookup mem_write function must be an unsigned integer.");
      if (!arg_is_addr(_mem_write._func,2)) AddError(ml,loc(),"Third argument to mmu lookjup mem_write function must be addr_t.");
      cc.check_code(_mem_write);
    }

    if ((_mem_read || _mem_write) && !_entry_mem_size) {
      AddError(ml,loc(),"A memory read and/or write routine was specified but no entry_mem_size parameter was set.");
    }
  }

  void MmuLookup::dump(ostream &o,const gc_string &pfx) const
  {
    o << '\n' << pfx << "Lookup:  " << _name;
  
    if (_interface) {
      o << " (interface)";
    }

    if (_iparent) {
      o << " (parent: " << _iparent->name() << ")";
    }

    o << "\n";

    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    o << pfx1 << "Type:  ";
    switch (_type) {
    case Both:
      o << "both\n";
      break;
    case Instr:
      o << "instr\n";
      break;
    case Data:
      o << "data\n";
      break;
    }

    Attrs::dump(o,pfx1);

    o << pfx1 << "Priority:  ";
    if (!valid_priority()) {
      o << "ignored";
    } else {
      o << _priority;
    }
    o << "\n";
    switch (_sizetype) {
    case None:
      break;
    case Const:
      o << pfx1 << "Size:  " << _size << " bytes.\n";
      break;
    case BitSize:
      o << pfx1 << "Size:  2^(" << _sizescale << " * " << _sizefield().name() << " + " << _sizeoffset << ") << " << _sizeshift << "\n";
      break;
    case LeftMask:
      o << pfx1 << "Size:  " << _sizefield().name() << " field (left-justified mask) << " << _sizeshift << "\n";
      break;
    }

    o << pfx1 << "Page Number Shift:  " << _pageshift << "\n";

    if (_realpage) {
      o << pfx1 << "Real-page Field:  " << _realpage().name() << "\n";
    }

    o << pfx1 << "Min Page Size:  " << dec << min_page_size() << " (bytes)\n";

    if (has_array()) {
      o << pfx1 << "Array:  " << _array_entries << " entries, ";
      if (_array_assoc == 0) {
        o << "fully associative.\n";
      } else {
        o << _array_assoc << "-way set-associative.\n";
      }
    }

    if (!_set_fields.empty()) {
      o << pfx1 << "Set Fields:\n";
      ForEach(_set_fields,i) {
        i->second->dump(o,pfx2);
      }
      o << "\n";
    }

    if (!_way_fields.empty()) {
      o << pfx1 << "Way Fields:\n";
      ForEach(_way_fields,i) {
        i->second->dump(o,pfx2);
      }
      o << "\n";
    }

    bool print_name = !(_tests.size() == 1 && _tests.begin()->second->name() == DefaultTestSet);
    if (!_test_order.empty()) {
      o << pfx1 << "Tests:";
      ForEach(_test_order,i) {
        o << "\n";
        (*i)->dump(o,pfx2,print_name);
      }
      o << "\n";
    } else if (!_tests.empty()) {
      // If we're an interface, then we don't have a test-order yet, so we just
      // print from the hash.
      o << pfx1 << "Tests:";
      ForEach(_tests,i) {
        o << "\n";
        i->second->dump(o,pfx2,print_name);
      }
      o << "\n";    
    }

    if (_exec_perm) {
      o << '\n' << pfx1 << "Execute permission:  ";
      _exec_perm.print(o,pfx2);
    }
    if (_load_perm) {
      o << '\n' << pfx1 << "Load permission:  ";
      _load_perm.print(o,pfx2);
    }
    if (_store_perm) {
      o << '\n' << pfx1 << "Store permission:  ";
      _store_perm.print(o,pfx2);
    }

    if (_valid) {
      o << '\n' << pfx1 << "Valid Predicate:  ";
      _valid.print(o,pfx2);
    }

    if (_multi_hit) {
      o << '\n' << pfx1 << "Multi-hit action:  ";
      _multi_hit.print(o,pfx2);
    }

    if (_final_hit) {
      o << '\n' << pfx1 << "Final-hit action:  ";
      _final_hit.print(o,pfx2);
    }

    if (_miss) {
      o << '\n' << pfx1 << "Miss handler:  ";
      _miss.print(o,pfx2);
    }

    if (_hit) {
      o << '\n' << pfx1 << "Hit handler:  ";
      _hit.print(o,pfx2);
    }

    if (_reset) {
      o << '\n' << pfx1 << "Reset handler:  ";
      _reset.print(o,pfx2);
    }

    if (_mem_read) {
      o << '\n' << pfx1 << "Memory read handler:  ";
      _mem_read.print(o,pfx2);
    }

    if (_mem_write) {
      o << '\n' << pfx1 << "Memory write handler:  ";
      _mem_write.print(o,pfx2);
    }

    if (_entry_mem_size) {
      o << '\n' << pfx1 << "Memory size:  " << mem_size() << '\n';
    }

    if (!_lookups.empty()) {
      ForEach(_lookups,i) {
        i->second->dump(o,pfx1);
      }
    }

    dump_doc(o,pfx,true);

    o << pfx << "-------------------------------\n";
  }

  //
  // Information about a generic field.
  // 

  GenericField::GenericField(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _bits(0),
    _resetvalue(0),
    _int_reset(true)
  {
  }

  void GenericField::finalize(MsgLog &ml)
  {
    Attrs::finalize();

    if (!_int_reset) {
      // If we don't have a numeric value, then check to make sure that it points
      // to an enum identifier.
      Bind *b = 0;
      _env->Lookup(_resetvalue,b);
      if (!dynamic_cast<BindEnumName*>(b)) {
        AddError(ml,_resetvalue,"Identifier does not match any registered enum values.");
      }
    } else {
      if (!_resetvalue) {
        _resetvalue = strconst("0");
      }
    }
  }

  void GenericField::dump(std::ostream &o,const gc_string &pfx) const
  {
    gc_string pfx2 = indent(pfx);
    o << pfx << name() << ":  ";
    if (!_bits) {
      o << "default size.\n";
    } else {
      o << _bits << " bits.\n";
    }
    o << pfx2 << "Reset value:  " << _resetvalue << "\n";
    Attrs::dump(o,pfx2);
    dump_doc(o,pfx,true);
  }

  bool operator==(const GenericField &x,const GenericField &y)
  {
    return (x._bits == y._bits) && PtreeUtil::Eq(x._resetvalue,y._resetvalue);
  }

  MmuField::MmuField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
    GenericField(env,loc,name)
  {
  }

  //
  // Information about the cache.
  //

  CacheField::CacheField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
    GenericField(env,loc,name)
  {
  }

  Cache::Cache(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _id(0),
    _orig_name(name),
    _type(UnifiedCache),
    _level(0),
    _size(0),
    _linesize(0),
    _sets(0),
    _ways(0),
    _enable(0),
    _hit(0),
    _hit_pred(0),
    _miss(0),
    _invalidate_line(0),
    _read_line(0),
    _replace(0),
    _line_access(0),
    _sets_func(0),
    _ways_func(0),
    _write_through(0),
    _miss_enable(0),
    _next(0)
  {
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a cache across cores.  As such, we ignore things such as hook functions
  // and concentrate instead on the shape of the data structure, i.e. cache size, etc.
  bool Cache::operator==(const Cache &x) const
  {
    if ( _type != x._type ) return false;

    if ( _level != x._level ) return false;

    if ( _size != x._size ) return false;

    if ( _linesize != x._linesize ) return false;

    if ( _sets_list != x._sets_list ) return false;

    if ( _ways_list != x._ways_list ) return false;

    return true;
  }

  void Cache::finalize(Core &core,MsgLog &ml,unsigned id)
  {
    _id = id;

    Attrs::finalize();

    StrSet excepts;
    excepts.insert(CurrentCacheName);

    if (!_linesize) {
      PError(loc(),"No linesize set for cache.");
    }

    // We allow the user to either set the size of the cache or to have that calculated by
    // setting the number of sets.  It's possible for both to be set, in which case we assume that
    // the user is setting a value because the actual cache size is smaller than the maximum possible
    // combination of sets and ways, e.g. only certain combinations of sets/ways are possible.
    if (_ways_list.empty()) {
      PError(loc(),"No set-associativity specified for the cache.");
    }
    _ways = *max_element(_ways_list.begin(),_ways_list.end());

    if (!_size && _sets_list.empty()) {
      PError(loc(),"No size set for cache and a set-count was not explicitly specified.");
    } else if (_sets_list.empty()) {
      _sets = (fully_assoc()) ? 1 : (_size / (_ways * _linesize));
      _sets_list.push_back(_sets);
    } else {
      _sets = *max_element(_sets_list.begin(),_sets_list.end());
      // Each set value must be an even power of 2.
      ForEach(_sets_list,i) {
        if (!is_power_2(*i)) {
          AddError(ml,loc(),"Set value of " << *i << " is not a power of 2.");
        }
      }
      if (!_size) {
        // Size not set- calculate based upon max values.
        _size = _sets * _ways * _linesize;
      } else {
        // Sanity check:  Size must be less than max possible value.
        if (_size > (_sets * _ways * _linesize)) {
          AddError(ml,loc(),"Size must be less than maximum possible set/way combination.");
        }
      }
    }

    // The size must be an even number of words and must be a power of 2.
    if (!is_power_2(_size)) {
      AddError(ml,loc(),"Size must be a power of 2.");
    }
    if (_size % 4) {
      AddError(ml,loc(),"Size must be an even number of words.");
    }

    // We don't allow any set fields to have the same name as way fields so that
    // we can set fields w/o having to disambiguate the two.

    ForEach(_set_fields,i) {
      i->second->finalize(ml);
    }
    ForEach(_way_fields,i) {
      i->second->finalize(ml);

      if ( _set_fields.count(i->first) ) {
        AddError(ml,loc(),"Way-field '" << i->first << "' conflicts with set-field of the same name.");
      }
    }  

    // Cache line size must be an even number of words
    // and the total size must be an even number of cache lines.
    if (!is_power_2(_linesize)) {
      AddError(ml,loc(),"Cache linesize must be a power of 2.");
    }
    if (_linesize % 4) {
      AddError(ml,loc(),"Cacheline size must be an even number of words.");
    }
    if (_size % ( ((fully_assoc()) ? 1 : _ways) * _linesize)) {
      AddError(ml,loc(),"Cache size must be an even number of cache lines * ways.");
    }

    // If we have multiple sets or multiple ways, then we must have a corresponding set function.
    if ( multi_sets() && !_sets_func) {
      AddError(ml,loc(),"Multiple numbers of sets specified, but no specifier function was provided.");
    }

    if ( multi_ways() && !_ways_func) {
      AddError(ml,loc(),"Multiple numbers of ways specified, but no specifier function was provided.");
    }

    // Check various handlers.
    if (_enable) {
      LogPError(ml,{ 
          CheckCode cc(_env,core); 
          cc.check_code(_enable,excepts); 
          int arity = get_arity(_enable);
          if (! ( arity == 1 || arity == 2)) {
            PError(_enable,"The cache-enable function takes either 1 or 2 arguments.");
          }
          if (!arg_is_type(_enable,"CacheAccess",0)) {
            AddError(ml,_enable,"Argument 1 of the cache-enable function must be of type CacheAccess.");
          }        
          if (arity == 2) {
            if (!arg_is_addr(_enable,1)) {
              AddError(ml,_enable,"Argument 2 of the cache-enable function must be of type addr_t.");
            }        
          }
        });
    }
    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_hit,excepts);
        if (!arg_is_type(_hit,"CacheAccess",0)) {
          AddError(ml,_hit,"Argument 1 of cache-hit function must be of type CacheAccess.");
        }
        if (!arg_is_addr(_hit,1)) {
          AddError(ml,_hit,"Argument 2 of cache-hit-handler must be of type addr_t.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core,true);
        cc.check_code(_hit_pred,excepts);
        if (!arg_is_type(_hit_pred,"CacheAccess",0)) {
          AddError(ml,_hit_pred,"Argument 1 of hit-predicate function must be of type CacheAccess.");
        }
        if (!arg_is_uint(_hit_pred,1)) {
          AddError(ml,_hit_pred,"Argument 2 of hit-predicate function must be of type unsigned-integer.");
        }
        if (!arg_is_uint(_hit_pred,2)) {
          AddError(ml,_hit_pred,"Argument 3 of hit-predicate function must be of type unsigned-integer.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_miss,excepts);
        if (!arg_is_type(_miss,"CacheAccess",0)) {
          AddError(ml,_miss,"Argument 1 of cache-miss function must be of type CacheAccess.");
        }
        if (!arg_is_addr(_miss,1)) {
          AddError(ml,_miss,"Argument 2 of cache-miss-handler must be of type addr_t.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_invalidate_line,excepts);
        if (!arg_is_type(_invalidate_line,"CacheAccess",0)) {
          AddError(ml,_invalidate_line,"Argument 1 of invalidate-line function must be of type CacheAccess.");
        }
        if (!arg_is_uint(_invalidate_line,1)) {
          AddError(ml,_invalidate_line,"Argument 2 of invalidate-line function must be of type unsigned-integer.");
        }
        if (!arg_is_uint(_invalidate_line,2)) {
          AddError(ml,_invalidate_line,"Argument 3 of invalidate-line function must be of type unsigned-integer.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core);
        cc.check_code(_replace,excepts);
        if (!arg_is_type(_replace,"CacheAccess",0)) {
          AddError(ml,_replace,"Argument 1 of replacement-algorithm function must be of type CacheAccess.");
        }
        if (!arg_is_uint(_replace,1)) {
          AddError(ml,_replace,"Argument 2 of replacement-algorithm function must be of type unsigned-integer.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_line_access,excepts);
        if (!arg_is_type(_line_access,"CacheAccess",0)) {
          AddError(ml,_line_access,"Argument 1 of line-access function must be of type CacheAccess.");
        }
        if (!arg_is_uint(_line_access,1)) {
          AddError(ml,_line_access,"Argument 2 of line-access function must be of type unsigned-integer.");
        }
        if (!arg_is_uint(_line_access,2)) {
          AddError(ml,_line_access,"Argument 3 of line-access function must be of type unsigned-integer.");
        }
        if (!arg_is_addr(_line_access,3)) {
          AddError(ml,_line_access,"Argument 4 of line-access function must be of type addr_t.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_read_line,excepts);
        if (!arg_is_type(_read_line,"CacheAccess",0)) {
          AddError(ml,_read_line,"Argument 1 of read-line function must be of type CacheAccess.");
        }
        if (!arg_is_uint(_read_line,1)) {
          AddError(ml,_read_line,"Argument 2 of read-line function must be of type unsigned-integer.");
        }
        if (!arg_is_uint(_read_line,2)) {
          AddError(ml,_read_line,"Argument 3 of read-line function must be of type unsigned-integer.");
        }
        if (!arg_is_addr(_read_line,3)) {
          AddError(ml,_read_line,"Argument 4 of read-line function must be of type addr_t.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core);
        cc.check_code(_write_through,excepts);
        if (!arg_is_type(_write_through,"CacheAccess",0)) {
          AddError(ml,_write_through,"Argument 1 of write-through predicate must be of type CacheAccess.");
        }
        if (!arg_is_addr(_write_through,1)) {
          AddError(ml,_write_through,"Argument 2 of write-through predicate must be of type addr_t.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core);
        cc.check_code(_miss_enable,excepts);
        if (!arg_is_type(_miss_enable,"CacheAccess",0)) {
          AddError(ml,_miss_enable,"Argument 1 of miss-enable predicate must be of type CacheAccess.");
        }
        if (!arg_is_addr(_miss_enable,1)) {
          AddError(ml,_miss_enable,"Argument 2 of miss-enable predicate must be of type addr_t.");
        }
        if (!arg_is_uint(_miss_enable,2)) {
          AddError(ml,_miss_enable,"Argument 3 of miss-enable predicate must be of type unsigned-integer.");
        }
      });
    LogPError(ml,{
        CheckCode cc(_env,core);
        cc.check_code(_sets_func,excepts);
      });
    LogPError(ml,{
        CheckCode cc(_env,core);
        cc.check_code(_ways_func,excepts);
      });
  }

  gc_string Cache::typestr() const
  {
    return typestr(_type);
  }

  gc_string Cache::type_enum_str() const
  {
    return type_enum_str(_type);
  }

  void Cache::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << name() << " (level " << _level << ", " << typestr() << ")\n";
    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    o << pfx1 << "Size    :  " << _size << " bytes\n"
      << pfx1 << "Linesize:  " << _linesize << " bytes\n"
      << pfx1 << "Sets    :  ";
    printSequence(o,_sets_list,true);
    o << pfx1 << "Ways    :  ";
    printSequence(o,_ways_list,true);

    if (!_set_fields.empty()) {
      o << pfx1 << "Set Fields:\n";
      ForEach(_set_fields,i) {
        i->second->dump(o,pfx2);
      }
    }

    if (!_way_fields.empty()) {
      o << pfx1 << "Way Fields:\n";
      ForEach(_way_fields,i) {
        i->second->dump(o,pfx2);
      }
    }

    if (_enable) {
      o << pfx1 << "Enable function:  " << _enable << "\n\n";
    }
    if (_hit_pred) {
      o << pfx1 << "Hit predicate:  " << _hit_pred << "\n\n";
    }
    if (_hit) {
      o << pfx1 << "Hit handler:  " << _hit << "\n\n";
    }
    if (_miss) {
      o << pfx1 << "Miss handler:  " << _miss << "\n\n";
    }
    if (_invalidate_line) {
      o << pfx1 << "Invalidate action:  " << _invalidate_line << "\n\n";
    }
    if (_read_line) {
      o << pfx1 << "Read action:  " << _read_line << "\n\n";
    }
    if (_replace) {
      o << pfx1 << "Replacement algorithm:  " << _replace << "\n\n";
    }
    if (_line_access) {
      o << pfx1 << "Line access action:  " << _line_access << "\n\n";
    }
    if (_write_through) {
      o << pfx1 << "Write-through predicate:  " << _write_through << "\n\n";
    }
    if (_miss_enable) {
      o << pfx1 << "Miss-enable predicate:  " << _miss_enable << "\n\n";
    }
    if (_sets_func) {
      o << pfx1 << "Number-of-sets:  " << _sets_func << "\n\n";
    }
    if (_sets_func) {
      o << pfx1 << "Number-of-ways:  " << _ways_func << "\n\n";
    }

    Attrs::dump(o,pfx1);
    dump_doc(o,pfx,true);
  }

  gc_string Cache::typestr(CacheType type)
  {
    return get_cache_type_str(type);
  }

  gc_string Cache::type_enum_str(CacheType type)
  {
    switch (type) {
    case InstrCache:
      return "InstrCache";
    case DataCache:
      return "DataCache";
    default:
      return "UnifiedCache";
    }
  }

  CacheType Cache::get_type(const gc_string &n)
  {
    return get_cache_type(n);
  }

  //
  // Info for memories.
  //

  MemoryHook::MemoryHook(Opencxx::Environment *env, Opencxx::Ptree *loc, Opencxx::Ptree *hook) :
    Base(env,loc),
    _action(hook)
  {
  }


  void MemoryHook::dump(ostream &o, const gc_string &pfx) const
  {
    o << pfx << _action << "\n";
  }


  void MemoryHook::finalize(Core &core, MsgLog &ml, bool is_write) 
  {
    using namespace PtreeUtil;

    LogPError(ml,{
        // When checking these functions, we allow the use of the memory-hook
        // access size specifier, which will be specified in the code as a
        // template parameter.
        CheckCode cc(_env,core);
        cc.check_code(_action);
        // First argument is the size.
        if (!arg_is_uint(_action,0)) {
          PError(_action,"First argument to read/write hook must be an unsigned integer.");
        }

        Ptree *args = get_func_args(_action);
        Ptree *size_arg = Second(First(args))->Car();

        if (!size_arg) {
          PError(_action,"No name was specified for the memory hook size argument.");
        }
        
        // Second argument is a bits object with a size that's the name of the size parameter.
        if (!arg_is_bits(_action,1,size_arg->ToString())) {
          PError(_action,"Second argument to memory hook must be a bits object of size '" << size_arg << "'.");
        }

      });

  }

  Memory::Memory(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string& name) :
    Attrs(env,loc),
    Named(name),
    _id(0),
    _instr_mem(false),
    _prefetch_mem(false),
    _size(0),
    _addr_unit(0),
    _parent(0),
    _offset(0),
    _read(0),
    _instr_read(0),
    _write(0)
  {
  }

  void	Memory::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Name:  " << _name << " (" << _id << ")";
    
    if(_instr_mem) {
      o << "\n" << pfx << "Instruction memory" ;
    }
    
    if (_prefetch_mem) {
      o << "\n" << pfx << "Prefetch memory";
    }
    
    o << "\n" << pfx << "Size:   " << _size;
    o << "\n" << pfx << "Addressing unit: " << _addr_unit;

    if(_parent) {
      o << "\n" << pfx << "Parent name:  " << _parent->_name;
      o << "\n" << pfx << "Offset:  " << _offset;
    }
    o << "\n";

    if(_read) {
      o << pfx << "Read Hook:\n";
      _read->dump(o,pfx);
    }

    if(_instr_read) {
      o << pfx << "Instruction Read Hook:\n";
      _instr_read->dump(o,pfx);
    }

    if(_write) {
      o << pfx << "Write Hook:\n"; 
      _write->dump(o,pfx);
    }

    Attrs::dump(o,pfx);
    o << "\n";
  }

  void	Memory::finalize(Core &core, MsgLog &ml,unsigned id) 
  {
    _id = id;

    // Global memory may have only read/write hooks
    if(name() == MemoryName) {
      if(_size || _addr_unit || _parent_name != "" || _offset) {
        PError(loc(),"Global Mem supports only read/write hooks.");
      }
    } else if (_prefetch_mem) {
      if(!_size) {
        PError(loc(),"Size not set for memory.");
      }
      if (_parent_name != "") {
        PError(loc(),"Pre-fetch memory can not have parent memory.");
      }
      if (_read || _write) {
        PError(loc(),"Pre-fetch memory can not have read/write hook.");
      }
      if (_size%4) {
        PError(loc(),"Pre-fetch memory size must be divisible by four.");
      }
    }  else {
      if(!_size) {
        PError(loc(),"Size not set for memory.");
      }
      if(!_addr_unit) {
        PError(loc(),"Addressinng unit not set for memory.");
      }
      if(!is_power_2(_addr_unit)) {
        PError(loc(),"Addressing unit must be a power of 2.");
      }
    
      if(_parent_name != "")
        {
          if(!(_parent = core.get_memory(_parent_name))) {
            PError(loc(), "Parent memory does not exits.");
          }
          unsigned parent_size = _parent->_size;
          if(_offset+_size > parent_size) {
            PError(loc(),"Size out of parent range.");
          }
          if(_offset%_addr_unit != 0) {
            PError(loc(),"Offset must be divisible by address unit.");
          }
        }
    }

    if (_write) {
      _write->finalize(core,ml,true);
    }

    if (_read) {
      _read->finalize(core,ml,false);
    }

    // If we don't have a specific instruction read hook, then we'll use the
    // data read hook for instructions as well.
    if (!_instr_read) {
      _instr_read = _read;
    } else {
      _instr_read->finalize(core,ml,false);
    }
  }

  // This equivalence is defined in terms of what's needed to be able to
  // share a register.
  bool Memory::operator==(const Memory &x) const
  {
    if(_parent) {
      if( _parent != x._parent && _parent != &x)
        return false;
    }
    else if(x._parent && x._parent != this) {
      return false;
    }
    return (_read == x._read) && (_write == x._write);
  }

  //
  // Info about event buses.
  //

  EventBusField::EventBusField(Environment *env,Ptree *loc,const gc_string &name) :
    GenericField(env,loc,name)
  {
  }

  // This constructs a type signature.  Each type has a single-letter identifier.
  // For enums, we enumerate the enums as "e{val1,val2,...}".
  gc_string EventBusField::signature() const
  {
    gc_ostringstream ss;
    ss << 'b' << _bits;
    return ss.str();
  }

  EventBus::EventBus(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _action(0)
  {
  }

  bool EventBus::operator==(const EventBus &x) const
  {
    return (_signature == x._signature && PtreeUtil::Eq(_action._func,x._action._func));
  }

  void EventBus::finalize(Core &core,MsgLog &ml)
  {
    // Finalize each field and generate the signature, which is simply a
    // concatenation of each field's type.
    ForEach(_fields,f) {
      f->second->finalize(ml);
      _signature.append(f->second->signature());
    }

    if (!_action) {
      PError(loc(),"No action specified for this bus.");
    }

    LogPError(ml,{
        CheckCode cc(_env,core,false);
        cc.check_code(_action);
        gc_string t = name() + "_t";
        if (!arg_is_type(_action._func,t.c_str(),0)) {
          AddError(ml,_action._func,"Argument 1 of event-bus action function must be of type " << t << ".");
        }
      });

  }

  void EventBus::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << name() << "\n";

    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    if (!_fields.empty()) {
      o << pfx1 << "Fields:\n";
      ForEach(_fields,i) {
        i->second->dump(o,pfx2);
      }
    }

    if (_action) {
      o << pfx1 << "Action:  " << _action << "\n\n";
    }

    Attrs::dump(o,pfx1);
    dump_doc(o,pfx,true);
  }



  //
  // Info about external resources.
  //

  ExtResource::ExtResource(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name)
  {
  }

  bool ExtResource::operator==(const ExtResource &x) const
  {
    return (_objtype == x._objtype && _external == x._external);
  }

  // Currently, nothing to finlize.
  void ExtResource::finalize(Core &core,MsgLog &ml)
  {
  }

  void ExtResource::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << name() << "\n";

    gc_string pfx1 = indent(pfx);

    o << pfx1 << "Object type:  " << _objtype << '\n'
      << pfx1 << "External   :  " << _external << '\n'
      << pfx1 << "Reset      :  " << _reset << '\n';

    if (!_constr_args.empty()) {
      o << pfx1 << "Constr args: ";
      printSequence(o,_constr_args,true);
    }

    Attrs::dump(o,pfx1);
    dump_doc(o,pfx,true);
  }


  
  //
  // Info about groups.
  //
  
  Group::Group(Environment *env,Ptree *loc,const gc_string &name) :
    Attrs(env,loc),
    Named(name),
    _check_parent(false),
    _checked(false)
  {};

  void finalize(const Core &,MsgLog &) {}

  void Group::dump(std::ostream &o,const gc_string &pfx) const {
    o << pfx << "Name:  " << _name << "\n";     
    o << pfx << "Type:  " << _type << "\n";
    if (!_items.empty()) {
      o << pfx << "Items: ";
      ForEach(_items,i) {
        o << *i << " ";
      }
      o << "\n";
    }     
    Attrs::dump(o,pfx);
    dump_doc(o,pfx,true);
    o << pfx << "-------------------------------\n";
  }
  
  void Group::add_item(Opencxx::Ptree* p) {
    _raw_items.insert(p);
  }
  
  void Group::finalize(const Core& core,MsgLog &ml) 
  {
    if (_items.empty()) {
      if (Data::strictMode()) {
        AddWarning(ml,loc(),"Group '" << name() << "' is empty.");
      }
      // Note: We allow users to specify arbitary types when no elements are
      // there, in order to use them as attribute groups.
      if (_type.empty()) {
        _type = "none";
      }
      return;
    }
    
  }

  //
  // Info about assembler
  //

 
  AsmConfig::AsmConfig(Environment *env,Ptree *loc) :
    Base(env,loc),
    _default(true),
    _queue_size(1),
    _queue_offset(0),
    _explicit_regs(false), 
    _explicit_operand_regex_end(false)
  {};


  void AsmConfig::checkInstrTable(Core &core,MsgLog &ml,const StrList &instrtables,const char *msg)
  {
    StrSet tmp;
    
    ForEach(instrtables,i) {
      if (tmp.count(*i) > 0) {
        AddError(ml,loc(),"Duplicate value '" << *i << "' in instruction tables.");      
      }
      tmp.insert(*i);
      int id = Data::get_attr(*i);
      if(core.instrtables().find(id) == core.instrtables().end()) {
        AddError(ml,loc(),"Unknown instruction table '" << *i << "' in " << msg << ".");      
      }
    }
  }
  

  void AsmConfig::finalize(Core &core,MsgLog &ml) 
  {
    if (_queue_size < 1) {
      _queue_size = 1;
    }

    if (_queue_offset >= _queue_size) {
      AddError(ml,loc(),"Queue offset must be less than the queue size.");
    }
    
    _default = (_comments.empty() && _line_comments.empty() && _line_seps.empty());

    if (!_group_seps.empty() && !core.packetized_encoding()) {
      PError(loc(),"Packet grouping is defined only for architectures with packetized encodings.");      
    }  
    // Populate with the defaults, if needed.
    if (_comments.empty()) {
      _comments.push_back("#");
    } 
    
    if (_line_comments.empty()) {
      _line_comments.push_back("#");
    }

    // Check for duplicates
    StrSet tmp;
    StrSet all_comments;
    StrSet non_instr_seps;

    ForEach(_comments,i) {
      if (tmp.count(*i) > 0) {
        AddError(ml,loc(),"Duplicate value '" << *i << "' in comment strings.");      
      }
      tmp.insert(*i);       
      all_comments.insert(*i);
    }

    tmp.clear();
    ForEach(_line_comments,i) {
      if (tmp.count(*i) > 0) {
        AddError(ml,loc(),"Duplicate value '" << *i << "' in line comment strings.");      
      }
      tmp.insert(*i);
      all_comments.insert(*i);
    }

    // Check duplicates and that line separator is not a comment
    tmp.clear();   
    ForEach(_line_seps,i) {
      if (tmp.count(*i) > 0) {
        AddError(ml,loc(),"Duplicate value '" << *i << "' in line separators.");      
      }
      if (all_comments.count(*i)) {
        AddError(ml,loc(),"Line separator '" << *i << "' is also a comment.");      
      }
      tmp.insert(*i);
      non_instr_seps.insert(*i);
    }
    
    // Check group separators.
    ForEach(_group_seps,i) {
      non_instr_seps.insert(i->first);
      non_instr_seps.insert(i->second);
      ForEach(_group_seps,j) { 
        if (i != j) {
          if (i->first == j->first && i->second == j->second) {
            AddError(ml,loc(),"Duplicate value ('" << i->first << "','" << i->second
                     << "') in packet group separators.");      
          }
        }
      }
    }

    // Instruction separators shouldn't duplicate comments or group separators.
    non_instr_seps.insert(all_comments.begin(),all_comments.end());
    ForEach(_instr_seps,i) {
      if (non_instr_seps.count(*i)) {
        AddError(ml,loc(),"Duplicate value used as an instruction separator and as a comment, packet-separator, or line separator.");
      }
      if ( i->size() > 1 ) {
        AddError(ml,loc(),"Instruction separators may only be single characters.");
      }
    }

    // Check instrtables.
    checkInstrTable(core,ml,_asm_instrtables,"assembler instruction table list");
    checkInstrTable(core,ml,_disasm_instrtables,"disassembler instruction table list");

    ForEach(_parms,i) {
      i->second->finalize(core,ml);
    }    

    LoggedHash(_rules,ml,core);
  }
  
  const StrList AsmConfig::all_group_seps() const {
    StrList ret;
    ForEach(_group_seps,i) {
      ret.push_back(i->first);
      if (i->first != i->second) {
        ret.push_back(i->second);
      }
    }
    return ret;
  }
  
  void AsmConfig::dump(std::ostream &o,const gc_string &pfx) const 
  {
    
    if (!_default || !_group_seps.empty() || !_parms.empty() || _queue_size > 1 || _explicit_regs) {
      o << pfx << "\nAssember configuration:\n";
    }
    
    gc_string pfx1 = indent(pfx);
    
    if (!_default) {
      if (!_comments.empty()) {
        o << pfx1 << "Comment strings: ";
        ForEach(_comments,i) {
          o << "'" << *i << "' ";
        }
      }
      if (!_line_comments.empty()) {
        o << "\n" << pfx1 << "Line comment strings: " ;
        ForEach(_line_comments,i) {
          o << "'" << *i << "' ";
        }
      }
      if (!_line_seps.empty()) {
        o << "\n" << pfx1 << "Line separators: " ;
        ForEach(_line_seps,i) {
          o << "'" << *i << "' ";
        }
      }
      if (!_instr_seps.empty()) {
        o << "\n" << pfx1 << "Instruction separators: " ;
        ForEach(_instr_seps,i) {
          o << "'" << *i << "' ";
        }
      }
      o << "\n" << pfx1 << "Explicit regnames: " << _explicit_regs << "\n";
    } 
    
    if (_queue_size > 1) {
      o << "\n" << pfx1 << "Queue size: " << _queue_size << "\n";
    }
    
    if (!_group_seps.empty()) {
      o << pfx1 << "Packet grouping: " ;
      ForEach(_group_seps,i) {
        if (i->first != i->second) {
          o << "('" << i->first << "','" << i->second << "') ";
        } else {
          o << "'" << i->first << "' ";
        }
      }
      o << "\n";
    } 

    if (!_asm_instrtables.empty()) {
      o << pfx1 << "Assembler instruction tables: ";
      printSequence(o,_asm_instrtables,true);
    }

    if (!_disasm_instrtables.empty()) {
      o << pfx1 << "Disassembler instruction tables: ";
      printSequence(o,_disasm_instrtables,true);
    }        

    if (!_parms.empty()) {
      o << pfx1 << "Parms: ";
      ForEach(_parms,i) {
        i->second->dump(o,pfx1);
      }
    }
 
    if (!_symbol_chars.empty()) {
      o << pfx1 << "Symbol characters: '";
      printSequence(o,_symbol_chars,true);
    }

    if (!_operand_symbols_excluded.empty()) {
      o << pfx1 << "Operand symbols excluded: '";
      printSequence(o,_operand_symbols_excluded,true);
    }    
    
    if (!_rules.empty()) {
      o << pfx << "Assembler rules:\n";
      printHashOrdered(o,pfx1,_rules);
    }
 

  }

  
  AsmParm::AsmParm(Environment *env, Ptree *loc,const gc_string &name) : 
    Base(env,loc), Named(name), _value((uint64_t)-1), _constant(false), _type(NoType) {}

  void AsmParm::finalize(const Core &core, MsgLog &ml) 
  {
    LogPError(ml,{
        if (_type == NoType) {
          PError(loc(),"Parm does not have a type specified.");
        } else if (_type == Label) {
          if (_value != ((uint64_t) -1)) {
            PError(loc(),"Label parm can not have a default value.");
          }
          if (_constant) {
            PError(loc(),"Label parm can not be constant.");
          }
        } else { // Integer
          if (_value == ((uint64_t) -1)) {
            PError(loc(),"Integer parm does not have default value.");
          }
        }
      });
  }
 
  void AsmParm::dump(ostream &o, const gc_string &pfx) const
  {
    o << pfx << "Name :  " << name()    << "\n"
      << pfx << "Value:  " << _value    << "\n"
      << pfx << "Const:  " << _constant << "\n";

    dump_doc(o,pfx,true);
  }

  AsmRule::AsmRule(Opencxx::Environment *env,Opencxx::Ptree *loc, const gc_string &name) :
    Base(env,loc),Named(name),_type(NoType),_dest(false),_action(0) {}

  void AsmRule::finalize(Core &core, MsgLog &log)
  {
    if (!_action) {
      PError(loc(),"Rule without action code.");
    }
 
    StrSet excepts;
    excepts.insert("error");
    excepts.insert("info");
    excepts.insert(core.blocks().begin(),core.blocks().end());
    ForEach(core.instrfields(),i) {
      if (!i->second->interface()) {
        excepts.insert(i->first);
      }
    }

    if(arg_is_type(_action,"InstrBundle",0)) {
      _type = Packet;
    } else if (arg_is_type(_action,"InstrInfo",0)) {
      _type = Instr;
    } else {
      PError(loc(),"Argument to packet assembler rule must be 'InstrBundle' for a packet rule or 'InstrInfo' for an instruction rule.");
    }

    CheckCode cc(_env,core,false,false);
    cc.set_is_asm_code(true);
    cc.check_code_filter(_action);

    auto escapes = cc.escapes();
    ForEach(escapes,i) {
      if (_type == Packet) {
        if(i->second == "get_dest" || i->second == "intersect_dest") {
          _dest = true;
        } else if (i->second == "size") {
          continue;
        }
      } else if (!excepts.count(i->second)) {
        PError(i->first,"Unknown variable/resource found in action code:  " << i->first);
      }
    }
  }  

  void AsmRule::dump(std::ostream &o, const gc_string &pfx) const
  {
    o << pfx << "Name :  " << name() << "\n"
      << pfx << "Type  : " << (type() == Instr ? "instruction" : "packet") << "\n"
      << pfx << _action << "\n";
    dump_doc(o,pfx,true);
  }


}

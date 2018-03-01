//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
//
// This program compares two DAT or UVP files and reports any differences.
//

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <cstdarg>
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>

#include "rnumber/RNumber.h"

#include "helpers/AnyOption.h"
#include "helpers/pfxstream.h"
#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"

#include "DatParser.h"
#include "UvpParser.h"
#include "UvpParseTypes.h"

using namespace std;
using namespace rnumber;

// Set to true for copious bison debug information when parsing UVPs.  For this
// to print debug info in the scanner/tokenizer, you'll also need to have
// "%option debug" uncommented in uvptokenizer.ll.
#define UvpDebug false
namespace adl {

  typedef vector<string>     StrList;
  typedef set<string>        StrSet;
  typedef set<Token>         TokSet;
  typedef map<string,string> StrMap;

  int MaxFailCount        = 10;        // Max fails to report before we stop reporting.
  int  Verbose            = 0;         // Verbosity level for message reporting.
  bool Quiet              = false;     // If true, report nothing (useful for
                                       // when scripts are using this tool).
  bool ParseOnly          = false;     // If true, just parse the data files,
                                       // then exit if parsing was OK.
  bool IgnoreExtraMem     = false;     // If true, then ignore memory data which
                                       // exists in the expects file but not in
                                       // the found file.  Useful for Raptor
                                       // tests, which might have extra D cards
                                       // caused by, e.g. cache operations.
  bool IgnoreExtraTlbs    = false;     // If true, then ignore tlb (T-card) data which
                                       // exists in the expects file but not in
                                       // the found file.  Useful for Raptor
                                       // tests, which might have extra T cards.
  bool IgnoreExtraFields  = false;     // If true, then ignore any extra fields
                                       // which are in the expects file but not in
                                       // the found file.
  bool IgnoreExtraFoundFields = true;  // We generally ignore extra fields in
                                       // the found file, but this allows for
                                       // the feature to be toggled.
  bool IgnoreITime        = false;     // If true, ignore issue-time information.
  bool IgnoreCTime        = false;     // If true, ignore completion-time information.
  bool IncrementalScan    = false;     // If true, then rather than reading in
                                       // all expects and all found data into a
                                       // single data structure, we scan
                                       // incrementally through all of the
                                       // cores.
  bool FullPathErrors     = true;      // If true, report the full paths for
                                       // filenames in an error message.  If
                                       // false, report just the basename.
                                       // Useful for regressions, where files
                                       // might be located in different
                                       // directories- in that case, set this to
                                       // false so that the expects regression
                                       // won't miscompare just because relative
                                       // paths are different.
  bool CompareGlobalCount = false;     // If true, compare a UVPs global-count
                                       // field.  If false, then don't.
  bool IgnoreTrace        = false;     // If true, ignore trace information-
                                       // just compare final results.
  bool CheckAll           = false;     // If true, then the found file must have
                                       // all intermediate data that the expects
                                       // file does.  Normally (when false), we
                                       // just require that if the found file
                                       // has a type of intermediate data, then
                                       // we check it, otherwise we ignore that
                                       // type.  So, for example, a found file
                                       // which doesn't have cache data can
                                       // still be compared against an expects
                                       // file that does have it.  If this is
                                       // true, then the found file would have
                                       // to have the cache data.  This also
                                       // checks for the existance of extra
                                       // intermediate results, such as extra
                                       // register updates which are not
                                       // expected.
  TokSet CheckAllCards;                // Similar to CheckAll, but allows for individual control
                                       // over what cards to check.  We always add "M" cards to this
                                       // list.
  bool IgnoreExcptNames   = true;      // If true, then exception names are ignored.
  uint32_t MaxCommands    = 10000;     // Size of chunk for incremental
                                       // algorithm
  double MaxDelta         = 0.0;       // If non-zero, report errors only if
                                       // time difference delta exceeds this
                                       // value.
  int MinTimeError        = 0;         // Minimum time difference for reporting
                                       // an instruction timing error, if we
                                       // fail a test due to exceeding the
                                       // max-time delta.
  int global_id           = 0;         // Global instruction counter
  int InstrTimingOffset   = 0;         // Instruction index to start checking
                                       // for timing (if we're doing that).
  StrSet IgnoreRegs;                   // Lists any registers that should be
                                       // ignored during the comparison.
  StrSet IgnoreCaches;                 // Lists any caches that should be
                                       // ignored during the comparison.
  TokSet IgnoreFields;                 // Lists any fields that should be
                                       // ignored during the comparison.
  TokSet IgnoreCards;                  // Lists any cards that should be ignored
                                       // during comparison.
  RNumber RegMask;                     // If non-zero, use the value as a mask
                                       // when comparing registers.  We only
                                       // compare the portions that are non-zero
                                       // in the mask.
  bool PrintAsm = true;                // If true, error messages print the
                                       // assembly string for the instruction.
  bool RemoveConsecutiveMem = false;   // If true, then we remove consecutive ir
                                       // memory elements, leaving just one
                                       // unique item.  If false, then all are
                                       // added.
  bool ShowThreadCount = true;         // Always show a thread-local instruction count.
  StrMap CoreNameMap;                  // For mapping/translating core names.

  static const char *FetchTokenStr = "fetch";
  static const char *WriteTokenStr = "write";

  static StrToken FetchToken(FetchTokenStr,strlen(FetchTokenStr),false);
  static StrToken WriteToken(WriteTokenStr,strlen(WriteTokenStr),false);
  
#define Msg(x) { if (!Quiet) { cout << x << "\n"; } }

#define VStream cerr
#define V1(x)   { if (Verbose >= 1)   { VStream << x << "\n"; } }
#define V2(x)   { if (Verbose >= 2)   { VStream << x << "\n"; } }
#define VN(n,x) { if (Verbose >= (n)) { VStream << x << "\n"; } }

  // Predicated versions of the above.
#define V1p(x,y)   { if (Verbose >= 1 && (x))   { VStream << y << "\n"; } }
#define V2p(x,y)   { if (Verbose >= 2 && (x))   { VStream << y << "\n"; } }
#define VNp(n,x,y) { if (Verbose >= (n) && (x)) { VStream << y << "\n"; } }

  bool vlevel(int l)
  {
    return (Verbose >= l);
  }

  struct RegEntry {
    RegEntry() : _strval(false) {}
    RegEntry(const RNumber &rn) : _val((RegMask != 0) ? (rn & RegMask) : rn), _strval(false) {};
    RegEntry(Pairs &pairs) : _strval(false) 
    {
      // First, try to get an integer value.
      _val = get_rn(pairs,"d","register data.");
      _dt  = get_str(pairs,"dt");
      // If not, then check for whether it is a zero or non-parsable string
      if (_val == 0) {
        string str = get_str(pairs,"d",0);
        unsigned int  n=0;
        char c = str[n];
        while (c == ' ' ||  c == '\t') { // skip white spaces
          c = str[++n];
        }
        if (c == '\0') { // done
          return;
        }
        if (c == '+' || c == '-') { // skip leading sign
          c = str[++n];
        }
        if (c == '0') { 
          c = str[++n];
          if (tolower(c) == 'x' || tolower(c) == 'b') {
            c = str[++n];
          }
        }
        while (c != '\0' && isdigit(c)) {
          c = str[++n];
        }
        if (c != '\0') { // it is a string
          _enum = str;
          _strval = true;
        }
      } else {
        // We have a numerical value, so mask it if we have a comparison mask.
        // That way, we can just directly compare the values later on.
        if (RegMask != 0) {
          _val = _val & RegMask;
        }
      }
    }

    bool operator==(const RegEntry &x) const 
    {
      if (_strval != x._strval) {
        return false;
      }
      if (_strval) {
        return (_enum == x._enum);
      }
      // Ignore data dependency if either is empty.
      if (!_dt.empty() && !x._dt.empty()) {
        if (_dt != x._dt) {
          return false;
        }
      }
      return _val == x._val;
    }

    bool operator!=(const RegEntry &x) const
    {
      return !(*this == x);
    }

    bool operator<(const RegEntry &x) const
    {
      // We consider enums to be less than integer values.
      if (_strval && !x._strval) {
        return true;
      }
      else if (!_strval && x._strval) {
        return false;
      }
      else if (_strval) {
        return (_enum < x._enum);
      }
      else {
        return _val < x._val;
      }
    }

    RNumber  _val;
    string   _enum;
    string   _dt;
    bool     _strval;
  };

  // Using a map, rather than a hash, in order to get deterministic output for
  // error messages.
  typedef map<string,RegEntry> RegHash;

  typedef pair<addr_t,uint32_t> AddrItem;
  typedef vector<AddrItem> AddrHash;
  typedef string CoreName;

  struct AddrCompare {
    bool operator()(const AddrItem &x,const AddrItem &y) {
      return x.first < y.first;
    }

    bool operator()(const AddrItem &x,addr_t y) {
      return x.first < y;
    }

    bool operator()(addr_t x,const AddrItem &y) {
      return x < y.first;
    }
  };

  struct IntPairComp {
    bool operator()(const TokPair &x,const TokPair &y) const {
      int c = x.first.cmp(y.first);
      if (c == 0) {
        // First element is the same, so now we must compare the
        // second element.  If we can convert it to an integer, then
        // we do an integer comparison.
        uint64_t xd;
        if (!atoi64_exact(x.second,xd)) {
          return x.second < y.second;
        } else {
          uint64_t yd = atoi64(y.second,0);
          return xd < yd;
        }
      } else {
        return c < 0;
      }
    }
  };

  typedef  unordered_map<string,AddrHash > MemHash;

  struct Card {
    string           _type;
    Pairs            _data;

    Card() {};
    Card(const string &t,const Pairs &d) : _type(t), _data(d)
    {
      check_ignores();
    };
    Card(const Token &t,const Pairs &d) : _type(t.str()), _data(d)
    {
      check_ignores();
    };
    Card(const Token &t,Pairs &d,bool) : _type(t.str())
    {
      _data.swap(d);
      check_ignores();
    };

    Pairs::iterator find_key(const char *n) {
      return _data.find_key(n);
    }

    bool not_found(const Pairs::iterator &x) const {
      return _data.not_found(x);
    }

    bool was_found(const Pairs::iterator &x) const {
      return _data.was_found(x);
    }

    void remove(const char *key) {
      ForEach(_data,i) {
        if ( i->first == key ) {
          _data.erase(i);
          break;
        }
      }
    }

    void remove(Pairs::iterator i) {
      _data.erase(i);
    }

    // Remove any fields that should be ignored.
    void check_ignores()
    {
      if (!IgnoreFields.empty()) {
        auto iter = _data.begin();
        for ( ; iter != _data.end(); ) {
          if ( IgnoreFields.count(iter->first) ) {
            auto tmp = iter++;
            _data.erase(tmp);
          } else {
            ++iter;
          }
        }
      }
    }

    bool operator==(const Card &x) const {
      if (_type != x._type) {
        return false;
      }
      if (_data.size() != x._data.size()) {
        return false;
      }
      Pairs::const_iterator xiter = x._data.begin();
      ForEach(_data,i) {
        if (is_empty_pair(*i)) {
          ++xiter;
          continue;
        }
        if (i->first != xiter->first) {
          return false;
        } else {
          // Try to convert to a 64-bit integer.  If that fails, compare the strings.
          uint64_t xd;
          if (!atoi64_exact(i->second,xd)) {
            if (i->second != xiter->second) {
              return false;
            }
          } else {
            uint64_t yd = atoi64(xiter->second,0);
            if (xd != yd) {
              return false;
            }
          }
        }
        ++xiter;
      }
      return true;
    }
  };

  ostream &operator<<(ostream &o,const Card &c)
  {
    o << "[ " << c._type << ": " << c._data << " ]";
    return o;
  }

  void display(const Card &c)
  {
    cerr << c;
  }

  struct Frame  : public vector<Card> {
    Frame(unsigned id = 0):_id(id){};
    void set_name(const Token &n) { _n = n; };

    unsigned _id;
    Token    _n;
  };

  // Stores information about intermediate results for a single instruction.
  // Keys are command types, e.g. "M" for memory, etc.  The value is a pair,
  // where the second element is a frame, or list of intermediate results of a
  // given type.  The first element of the frame is used as a counter in order
  // to track what intermediate result element we're comparing.
  typedef  unordered_map<string,pair<unsigned,Frame> > IrHash;

  typedef list<Frame> Trace;

  struct Key {
    string _name;
    addr_t _set;  // May also be used to store an address, in some situations.
    int    _way;

    Key(const string &n,addr_t s,int w) : _name(n), _set(s), _way(w) {};

    bool operator==(const Key &x) const {
      return (_name == x._name && _set == x._set && _way == x._way);
    };

    bool operator<(const Key &x) const {
      if (_name != x._name) return _name < x._name;

      if (_set != x._set) return _set < x._set;

      return _way < x._way;     
    }

  };

  // For display purposes, if set, we use this for field descriptions.
  struct KeyTemplate {
    string _set_name;
    bool   _set_hex;
    string _way_name;
    bool   _way_hex;
    KeyTemplate(const string &sn,bool sh,const string &wn,bool wh) : 
      _set_name(sn), 
      _set_hex(sh),
      _way_name(wn),
      _way_hex(wh)
    {};
  };

  static const KeyTemplate *GlobalKeyTemplate = 0;

  // An RAII structure for the global Key template.  Use this for setting the
  // global, rather than doing it directly, so that if an exception occurs, the
  // global is still cleared.
  struct KeyTemplateUse {
    KeyTemplateUse(const KeyTemplate *kt) {
      GlobalKeyTemplate = kt;
    }
    ~KeyTemplateUse() {
      GlobalKeyTemplate = 0;
    }
  };

  ostream &operator<<(ostream &o,const Key &k)
  {
    o << "(" << k._name << ",";
    if (GlobalKeyTemplate) {
      o << GlobalKeyTemplate->_set_name << ":";
      if (GlobalKeyTemplate->_set_hex) {
        o << hex;
      } else {
        o << dec;
      }
    }
    o << showbase << k._set << ",";
    if (GlobalKeyTemplate) {
      o << GlobalKeyTemplate->_way_name << ":";
      if (GlobalKeyTemplate->_way_hex) {
        o << hex;
      } else {
        o << dec;
      }
    }
    o << k._way << ")";
    return o;
  }

}

namespace std {
  
  // Hash/set support for strings.
  template<> struct hash<adl::Key>
  {
    size_t operator()(const adl::Key &x) const { 
      hash<string> hs;
      hash<int>    hi;
      return hs(x._name) + hi(x._set) + hi(x._way);
    }
  };
  
}

namespace adl {

  typedef  unordered_map<Key,Card> KeyHash;

  // This structure stores atell of the types of data that we know about,
  // including trace information.
  struct BasicData {
    Trace    _trace;
    RegHash  _regs;
    MemHash  _mem;
    KeyHash  _cache;
    KeyHash  _mmu;
    unsigned _id;
    string   _name;

    // We always add on a single frame in order to handle trace cards that might
    // occur before the first instruction, such as annotations.
    BasicData():_id(0) {
      _trace.push_back(Frame(0));
    }

    bool empty() const
    {
      assert (!_trace.empty());
      return _trace.front().empty() && _regs.empty() && _mem.empty() && _cache.empty() && _mmu.empty();
    }
  };

  // Stores data about a single context instance.
  struct CtxData : public BasicData {};

  // Stores information about a single type of context.
  struct CtxDataArray {
    vector<CtxData> _data;
    unsigned        _cur;

    CtxDataArray() : _data(1), _cur(0) {};

    CtxData &cur() { return _data[_cur]; };
    unsigned size() const { return _data.size(); };

    bool empty() const 
    {
      ForEach(_data,i) {
        if (!i->empty()) return false;
      }
      return true;
    }
  };

  typedef  unordered_map<string,CtxDataArray> CtxHash;

  // Stores data about a single core, which might contain context information,
  // as well as outer-level, non-context related information.
  struct CoreData : public BasicData {

    CtxHash  _contexts;

    CtxData *_cur_ctx;   

    CoreData() : _cur_ctx(0){}

    bool empty() const 
    {
      if (!BasicData::empty()) return false;

      ForEach(_contexts,i) {
        if (!i->second.empty()) return false;
      }

      return true;
    }

  };

  template <class T>
  ostream &print_hash(ostream &o,const T &rh)
  {
    o << "{\n";
    ForEach(rh,i) {
      o << i->first << " => " << i->second << "\n";
    }
    o << "}\n";
    return o;
  }

  template <class T>
  ostream &print_array(ostream &o,const T &t)
  {
    o << "{\n";
    unsigned count = 0;
    ForEach(t,i) {
      o << "#" << dec << count++ << " => " << *i << "\n";
    }
    o << "}\n";
    return o;
  }

  template <class T>
  ostream &print_list(ostream &o,const T &t)
  {
    o << "{\n";
    ostream_iterator<typename T::value_type> out(o,"\n");
    copy(t.begin(),t.end(),out);
    o << "}\n";
    return o;
  }

  ostream &operator<<(ostream &o,const Frame &t)
  {
    return print_list(o,t);
  }

  ostream &operator<<(ostream &o,const IrHash &ir)
  {
    o << "{\n";
    ForEach(ir,i) {
      o << i->first << " => " << hex << showbase << i->second.second << dec << noshowbase << "\n";
    }
    o << "}\n";
    return o;
  }

  ostream &operator<<(ostream &o,const Trace &t)
  {
    return print_list(o,t);
  }

  ostream &operator<<(ostream &o, const RegEntry &re) 
  {
    if (re._strval) {
      o << re._enum;
    } else {
      o << re._val;
    }
    if (!re._dt.empty()) {
      o << " (dt=" << re._dt << ")";
    }
    return o;
  }

  ostream &operator<<(ostream &o,const RegHash &rh)
  {
    o << "{\n";
    ForEach(rh,i) {
      o << i->first << " => " << hex << showbase << i->second << dec << noshowbase << "\n";
    }
    o << "}\n";
    return o;
  }

  ostream &operator<<(ostream &o,const AddrHash &ah)
  {
    o << "{\n";
    ForEach(ah,i) {
      o << hex << showbase << i->first << " => " << i->second << dec << noshowbase << "\n";
    }
    o << "}\n";
    return o;
  }

  ostream &operator<<(ostream &o,const CtxDataArray &cda)
  {
    return print_array(o,cda._data);
  }

  ostream &operator<<(ostream &o,const MemHash &mh)
  {
    return print_hash(o,mh);
  }

  ostream &operator<<(ostream &o,const KeyHash &kh)
  {
    return print_hash(o,kh);
  }

  void display(const KeyHash &kh) {
    cerr << kh;
  }

  ostream &operator<<(ostream &o,const CtxHash &ch)
  {
    return print_hash(o,ch);
  }

  ostream &operator<<(ostream &o,const BasicData &bd)
  {
    o << "Trace " << bd._trace
      << "Regs " << bd._regs
      << "Memories " << bd._mem
      << "Cache " << bd._cache
      << "Mmu " << bd._mmu;
    return o;
  }

  ostream &operator<<(ostream &o,const CtxData &cd)
  {
    o << "{\n";
    operator<<(o,(BasicData&)cd);
    o << "}\n";
    return o;
  }

  ostream &operator<<(ostream &o,const CoreData &cd)
  {
    o << "{\n";
    operator<<(o,(BasicData&)cd);
    o << "Contexts " << cd._contexts
      << "}\n";
    return o;
  }

  typedef  map<string,CoreData> CoreMap;

  ostream &operator<<(ostream &o,const CoreMap &cm)
  {
    return print_hash(o,cm);
  }

  string uitoa(unsigned x) 
  {
    ostringstream s;
    s << x;
    return s.str();
  }

  string btoa(bool x)
  {
    return (x) ? "true" : "false";
  }

  string dtoa(double x)
  {
    ostringstream s;
    s << x;
    return s.str();
  }

  string regname(Pairs &pairs)
  {
    string name = get_str(pairs,"n","register name.");
    int32_t index = get_int32(pairs,"i",0);
  
    if (index >= 0) {
      name += uitoa(index);
    }
    return name;
  }


  void addRegEntry(RegHash &regs,Pairs &pairs)
  {
    string   name = regname(pairs);
    RegEntry data(pairs);
    if (!IgnoreRegs.count(name)) {
      regs[name] = data;
    }
  }

  bool is_compressed_dat(const string &fn)
  {
    return getExtension(fn) == "dlz4";
  }

  bool is_uncompressed_dat(const string &fn)
  {
    return getExtension(fn) == "dat";
  }

  bool is_fast_dat(const string &fn)
  {
    return getExtension(fn) == "fdat";
  }

  bool is_uvp(const string &fn)
  {
    return getExtension(fn) == "uvp";
  }

  bool is_dat(const string &fn)
  {
    return is_uncompressed_dat(fn) || is_compressed_dat(fn) || is_fast_dat(fn);
  }

  // General data model base class.
  struct TraceData {

    TraceData(bool is_expects) :
      _is_expects(is_expects),
      _handled_cards(0),
      _first_test(false),
      _id(0),
      _handled_test(false),      
      _testcount(0),
      _diff_done(true)
    {
      reset();
    }

    bool      _is_expects;
    uint32_t  _handled_cards;
    bool      _first_test;
    unsigned  _id;
    bool      _handled_test;
    unsigned  _testcount;
    bool      _diff_done;
   
    CoreMap   _cores;
    CoreData *_cur_core;
    CoreData *_mem;
   
    unsigned _tgt_test; 
    
    virtual ~TraceData() {};

    //true if current test is read.
    virtual bool finished() const = 0;

    bool handled_test() { return _handled_test;};
    // Derived classes should override this to do their parsing here.
    // Return false then current test is parsed
    virtual bool read_file() = 0;
    
    // This  read specifined by MaxCommands numger of trace entries
    // Returns false then test is finished
    virtual bool read_chunk() = 0;

    virtual void reset() {
      _id = 0;
      _cores.clear();
      _cores.insert(make_pair("",CoreData()));
      _mem = _cur_core = &(_cores.begin()->second);
      _handled_test = false;
      _diff_done = true;
    }
    
    // This sorts the memory, which is stored as a vector.
       
    void finalize_mem()
    {
      ForEach(_cores,i) {
        CoreData &cd = i->second;
        ForEach(cd._mem,j) {
          AddrHash &ah = j->second;
          sort(ah.begin(),ah.end(),AddrCompare());
        }
      }
    }

    // Update the current core by finding the specified name.
    void update_core(const char *n)
    {
      if (*n == ':') {
        ++n;
      }
      string s(n);
      if (!CoreNameMap.empty()) {
        IfFind(x,CoreNameMap,s) {
          s = x->second;
        }
      }
      _cur_core = &_cores[s];
      _cur_core->_name = s;
    }

    // This updats context information for a given core.
    void update_context(const string &cname,unsigned cnum)
    {
      CtxHash::iterator citer = _cur_core->_contexts.insert(make_pair(cname,CtxDataArray())).first;
      CtxDataArray &carray = citer->second;
      if (cnum >= carray.size()) {
        carray._data.resize(cnum+1);
      }
      carray._cur = cnum;
      _cur_core->_cur_ctx = &(carray.cur());
      _cur_core->_cur_ctx->_name = string(cname);
    }

    // Specifies that results should not be directed towards a context at this point.
    void update_no_context()
    {
      _cur_core->_cur_ctx = 0;
    }
  };
  

    
  // The parser for UVP files.
  struct UvpHandler : private UvpParser, public TraceData
  {
    UvpHandler(const string &fn,bool is_expects) :
      UvpParser(fn,UvpDebug,false),
      TraceData(is_expects),
      _filename(fn)
    {
      setFullPathErrors(FullPathErrors);
    }
    
    string _filename;
    
    // We keep track of the current path as we go.
    StrList _path;
    
    virtual void reset()
    {
      TraceData::reset();
    }

    virtual bool finished() const { return is_finished(); };

    // We don't support incremental UVP parsing.
    virtual bool read_chunk() { assert(0); };

    // We know if we're done if we try parsing again and get EOF.
    virtual bool read_file()
    {      
      _handled_cards = 0;
      while (!_handled_test && !is_finished()) {
        parse();
      }
      finalize_mem();
      return !is_finished();      
    }
    
    string get_path_name()
    {
      string r;
      bool first = true;
      ForEach(_path,x) {
        if (!first) {
          r += ':';
        }
        first = false;
        r += *x;
      }
      return r;
    }

    // Note: We treat all elements of the UVP file as cores.  So even an MT UVP is
    // treated as a set of cores.  This is unlike a DAT, which differentiates
    // between cores and contexts.
    void handle_block_begin(const YYLTYPE &loc,unique_ptr<UvpToken> type,unique_ptr<UvpToken> name)
    {
      string type_str = type->str();
      string name_str = name->str();

      //cout << "Block begin:  " << type_str << ": " << name_str << endl;
      if (*type.get() == "test") {
        ++_testcount;
        _id = 0;	
        V1("Parsing test " << _testcount << ":  " << ((_is_expects) ? "Expect data." : "Found data."));
      }

      if (!_handled_test) {
        if ( *type.get() == "test" ) {
          _handled_test = true;
        }
      } else {
        _path.push_back(name.get()->str());
        update_core(get_path_name().c_str());
      } 
      
      _cur_core->_name = get_path_name();
    }

    void handle_block_end(const YYLTYPE &loc)
    {
      string dd = get_path_name();

      //cout << "Block end" << endl;
      if (!_path.empty()) {
        _path.pop_back();
      }
      update_core(get_path_name().c_str());      
    }

    void addRegEntry(const UvpCmd &cmd)
    {
      string name  = get_str(cmd,"nm","register name.");
      RNumber data = get_rn(cmd,"d","register data.");

      if (!IgnoreRegs.count(name)) {
        _cur_core->_regs[name] = data;
      }
    }

    void addTraceFrame(unsigned id)
    {
      _cur_core->_trace.push_back(Frame(id));
    }

    void setFrameName(const Token &n)
    {
      _cur_core->_trace.back().set_name(n);
    }

    Card &addTraceEntry(Card &&card)
    {
      BasicData &ct = *_cur_core;
      ct._trace.back().push_back(card);
      return ct._trace.back().back();
    }

    typedef vector<string> StrSet;

    // Copies a UVP type to the target Card type, flattening any aggregate
    // object.
    void copy_simple(Card &trg,const Token &key,const UvpValue *value)
    {
      if ( const UvpToken *ut = dynamic_cast<const UvpToken*>(value) ) {
        // For simple tokens, we can just copy them over.
        trg._data.push_back(make_pair(key,*ut));
      }
      else if (const UvpList *ul = dynamic_cast<const UvpList*>(value) ) {
        // For list objects, we enumerate each element with the form key<n>=value[n].
        const int bufsize = 50;
        char buf[bufsize];
        int index = 0;
        ForEach((*ul),i) {
          int l = min(bufsize,snprintf(buf,bufsize,"%.*s%d",key.size(),key.data(),index++));
          copy_simple(trg,Token(buf,l,true),i->get());
        }
      }
      else if (const UvpMap *um = dynamic_cast<const UvpMap*>(value)) {
        // For map objects, we flatten each element with the form key:mapkey=map[key]
        const int bufsize = 50;
        char buf[bufsize];
        ForEach((*um),i) {
          // We only handle maps with simple keys.
          if ( const UvpToken *ut = dynamic_cast<const UvpToken*>(&(i->first))) {
            int l = min(bufsize,snprintf(buf,bufsize,"%.*s:%.*s",key.size(),key.data(),ut->size(),ut->data()));
            copy_simple(trg,Token(buf,l,true),i->second.get());
          }
        }
      }
    }

    // Copies a series of keys from the src UvpMap object to the target Pairs object.
    void copy_to_card(Card &trg,const UvpMap &src,const StrSet &src_list)
    {    
      ForEach(src,i) {
        // Skip the item if it's not in the souce list or it's to be ignored.
        if ( !binary_search(src_list.begin(),src_list.end(),i->first) || IgnoreFields.count(i->first) ) {
          continue;
        }
        copy_simple(trg,i->first,i->second.get());
      }
    }

    // Copies UVP types to a Card, flattening aggregate objects.
    void copy_to_card(Card &trg,const UvpMap &src)
    {    
      ForEach(src,i) {
        if (IgnoreFields.count(i->first)) {
          continue;
        }
        copy_simple(trg,i->first,i->second.get());
      }
    }

    // Copies UVP types to a Card, flattening aggregate objects and ignoring
    // global ignores and specific ignores.
    void copy_to_card_with_ignores(Card &trg,const UvpMap &src,const StrSet &ignore_list)
    {    
      ForEach(src,i) {
        if (IgnoreFields.count(i->first) || binary_search(ignore_list.begin(),ignore_list.end(),i->first)) {
          continue;
        }
        copy_simple(trg,i->first,i->second.get());
      }
    }

    // Copies tlb fields stored in the pair with the key "d" to a Pairs object.
    // Finalizes the pairs object when done, so that it can be compared.
    void copy_tlb_fields(Pairs &trg,const UvpMap &src)
    {
      const UvpList &fields = find_key<UvpList>(src,"d","list","TLB fields.");
      ForEach(fields,i) {
        const UvpList &fpair = get_uvptype<UvpList>(**i,"list","TLB field pair.");
        UvpList::const_iterator liter = fpair.begin();
        Token name,value;
        if (liter != fpair.end()) {
          name = get_uvptype<StrToken>(**liter,"string","TLB field name");
          ++liter;
          if (liter != fpair.end()) {
            value = get_uvptype<NumToken>(**liter,"number","TLB field value");
          } else {
            RError("Expected a two-element list for a TLB field pair " << name);
          }
        } else {
          RError("Empty list found for TLB field pair.");
        }
        if (!IgnoreFields.count(name)) {
          trg.push_back(make_pair(name,value));
        }
      }
      trg.finalize();
    }

    StrSet init_src_list(const char *first ...)
    {
      StrSet s;
      va_list ap;
      const char *p = first;
      va_start(ap,first);
      s.push_back(first);
      while ( (p = va_arg(ap,const char *)) ) {
        s.push_back(p);
      }
      va_end(ap);
      sort(s.begin(),s.end());
      return s;
    }

    void handle_cmd(const YYLTYPE &loc,unique_ptr<UvpCmd> cmd_p)
    {
      //cout << "Location:  " << loc << endl;

      const UvpCmd &cmd = *cmd_p.get();

      //cout << "Cmd:  " << cmd << endl;

      static StrSet instr_src_list = init_src_list("ea","ra","d","op","bt","ta",((CompareGlobalCount) ? "gc" : 0),(const char *)0);
      static StrSet tlb_ir_ignores = init_src_list("d",(const char *)0);

      //cout << "  Handled" << endl;
     
      try {
        
        if (_handled_cards==0) {
          _handled_cards = 1;
        }

        // If we should ignore this card, then accept it and continue.
        if (IgnoreCards.count(*(cmd._cmd))) {
          return;
        }

        // Now try and handle the command.  We ignore result stuff and we only care
        // about execution (trace) stuff if it's a non-executed instruction, in
        // which case we use it to initialize memory.
        if ( *(cmd._port) == "I" ) {

          // Ignore init card stuff.

        } else if (*(cmd._port) == "E" ) {
          ++_handled_cards;
          
          if ( *(cmd._cmd) == "I" || *(cmd._cmd) == "RI" || *(cmd._cmd) == "A" || *(cmd._cmd) == "F") {
          
            // Skip if this is a non-executed or prefetch instruction.
            string cmdtype = cmd._cmd->str();
            StrToken type;
            auto type_iter = cmd._parms.find_key("t");
            if (cmd._parms.was_found(type_iter)) {
              auto t = get_uvptype<StrToken>(*type_iter->second,"string","instruction type");
              // Ignore non-execute instructions, since they're basically just memory inits.
              // Ignore prefetch instructions b/c ADL doesn't execute a prefetch path.
              // Ignore affix instructions b/c they're memory inits and not part of the trace.
              // Ignore jam-writes because we don't generate them via the scripting interface.
              if (t == "N" || t == "P" || t == "A") {
                return;
              }
              // Hack: We don't generate the exitHalt type, so replace it with
              // write to facilitate easier comparisons.
              if (t == "exitHalt") {
                type = WriteToken;
              } else if (t == "write" || t == "ijam") {
                type = t;
              }
            }

            if (cmdtype == "F") {
              // Just to make sure we uniquely identify E:F cards.
              type = FetchToken;
            }

            // Change the command type to be the same, so that we'll compare
            // properly.  Otherwise, an E:F card won't compare against an E:A
            // card, for example.
            cmdtype = "I";

            if(_is_expects) {
              global_id++;
            }  
            
            addTraceFrame(global_id);

            auto v = cmd._parms.find_key("asm");
            if (cmd._parms.was_found(v)) {
              auto asm_str = get_uvptype<StrToken>(*v->second,"string","assembly string");
              setFrameName(asm_str);
            }          
          
            Card &card = addTraceEntry(Card(cmdtype,Pairs()));

            if (!type.empty()) {
              card._data.push_back(make_pair("t",type));
            }

            // Copy over relevant values.
            copy_to_card(card,cmd._parms,instr_src_list);

            // Now handle the intermediate results.  Unfortunately, this requies fairly intimate knowledge
            // of the file format b/c we have to convert over to a DAT-like model.
            // Now extract the intermediate results and add each item to the frame.
            auto iter = cmd._parms.find_key("ir");
            if (!cmd._parms.not_found(iter)) {
              const UvpList &ir = get_uvptype<UvpList>(*iter->second,"list","ir entry list");
              ForEach(ir,i) {
                auto const &elm = get_uvptype<UvpMap>(**i,"map","ir entry");
                string type = find_key<StrToken>(elm,"t","string","ir entry type").str();
                if (type == "reg") {
                  Card &card = addTraceEntry(Card(string("R"),Pairs()));
                  card._data.push_back(make_pair("d",find_key<NumToken>(elm,"d","number","reg data")));
                  card._data.push_back(make_pair("n",find_key<StrToken>(elm,"rn","string","reg name")));
                }
                else if (type == "load" || type == "store") {
                  Card &card = addTraceEntry(Card(string("M"),Pairs()));
                  copy_to_card(card,elm);
                } else if (type == "intr") {
                  if (!IgnoreExcptNames) {
                    // Add this entry unless we're ignoring exception names.
                    Card &card = addTraceEntry(Card(string("E"),Pairs()));
                    card._data.push_back(make_pair("n",find_key<StrToken>(elm,"msg","string","exception name")));
                  }
                } else if (type == "tlb") {
                  // TLB intermediate results require us to take the fields values
                  // stored in the 'd' pair and flatten that, adding that to our
                  // map.
                  Card &card = addTraceEntry(Card(string("T"),Pairs()));
                  copy_to_card_with_ignores(card,elm,tlb_ir_ignores);
                  copy_tlb_fields(card._data,elm);
                }
              }
            }        

          } else {
            cerr << printLocation(loc) << ":  Warning:  Unknown UVP command:  " << cmd._port << " : " << cmd._cmd << ".\n";
          }
        
          return;
        } else if ( *(cmd._port) == "R" ) {
          if ( *(cmd._cmd) == "R" ) {
            // Register initialization.
            addRegEntry(cmd);
          }
          else if ( *(cmd._cmd) == "D" ) {
            // Data memory initialization.
            addr_t addr = get_uint64(cmd,"ra","address.");
            uint32_t data = get_uint32(cmd,"d","data.");
            // UVPs only suppport global memory.
            _mem->_mem["Mem"].push_back(make_pair(addr,data));
          }
          else if ( *(cmd._cmd) == "T" ) {
            // TLB initializations.
            string name = get_str(cmd,"n","TLB name.");
            uint32_t way  = get_uint32(cmd,"way","TLB way.");
            addr_t ep = 0;
            if (!IgnoreFields.count("ep")) {
              ep = get_uint64(cmd,"ep","effectve page.");
            }

            auto ip = _cur_core->_mmu.insert(make_pair(Key(name,ep,way),Card(*(cmd._cmd),Pairs())));
            Pairs &pairs = ip.first->second._data;

            copy_tlb_fields(pairs,cmd._parms);

            if (pairs.empty()) {
              RError("No TLB lookup key/value pairs were found on this line.");
            }
          
          } else if ( *(cmd._cmd) == "RES" ) {

            addTraceFrame(0);

            Card &card = addTraceEntry(Card(*(cmd._cmd),Pairs()));

            copy_to_card(card,cmd._parms);

          } else {
            cerr << printLocation(loc) << ":  Warning:  Unknown UVP command:  " << cmd._port << " : " << cmd._cmd << ".\n";
          }

        } else if ( *(cmd._port) == "M") {

          // Skip messages.

        } else {
          cerr << printLocation(loc) << ":  Warning:  Unknown UVP command:  " << cmd._port << " : " << cmd._cmd << ".\n";
        }

      }
      catch (exception &err) {
        RError(printLocation(loc) << ": " << err.what());
      }

      return;
    }  
  
    void handle_parm(const YYLTYPE &loc,unique_ptr<UvpToken> name,unique_ptr<UvpValue> value)
    {
      // Do nothing for bare parameters.
    }
  };

  // The parser class for DAT files.
  struct DatHandler : private DatParser, public TraceData
  {
    // We can only handle 1k max-line sizes for compressed files.
    // We allocate uniformly sized blocks in order to improve performance.
    enum { LineBufSize = 1024 };

    DatHandler(const    string &fn,
               bool     is_expects,
               bool     inc_scan,  
               bool     flt_mode = false,
               string   tgt_core = "",
               string   tgt_ctx  = "",
               unsigned tgt_ctx_num = 0 ,
               unsigned tgt_test = 0) : 
      DatParser(fn,is_compressed_dat(fn),inc_scan),
      TraceData(is_expects),
      _fn(FullPathErrors ? fn : base_name(fn)),
      _first_test(true),
      _test_start(false),
      _record_trace(false),
      _record_results(false),
      _flt_mode(flt_mode),
      _tgt_core(tgt_core),
      _tgt_ctx(tgt_ctx),
      _tgt_ctx_num(tgt_ctx_num),
      _tgt_test(tgt_test)
    {
      setLineBufSize(LineBufSize);
    }
    string _fn;
    bool   _first_test;
    bool   _test_start;
    bool   _record_trace;
    bool   _record_results;

    // For incremental scanning:
    bool     _flt_mode;
    string   _tgt_core;
    string   _tgt_ctx;
    unsigned _tgt_ctx_num;
    unsigned _tgt_test;

    // For storing uncompressed data, if applicable.
    list<unique_ptr<const char[]> > _clines;

    // If we're compressed, then we have to copy the line data
    // so that we won't clobber it.  We don't have to worry about
    // this for mmap'd files, since the entire file exists in memory.
    pair<const char*,unsigned> getline_compressed()
    {
      auto l = DatParser::getline_compressed();
      _clines.push_back(unique_ptr<const char[]>(l.first));
      return l;
    }

    bool finished() const { return _finished; };

    virtual CoreName core() { return "";};
        
    virtual void reset() {
      _test_finished = false;
      TraceData::reset();    
      _clines.clear();
    }

    // We determine whether we're done during the parsing of a test, so
    // we always return that we're not finished.  A separate call to
    // finished() must be made.
    virtual bool read_file()
    {  
      parse();
      finalize_mem();
      return true;      
    }

    virtual bool read_chunk()
    { 
      if (!_diff_done) {
        return true;
      }   
      _clines.clear();
      _handled_cards = 0;
      _stopped = false;	
      _diff_done = false;
      return !parse()&&!finished();
    }


    BasicData &cur_target()
    {
      if (_cur_core->_cur_ctx) {
        return *(_cur_core->_cur_ctx);
      } else {
        return *_cur_core;
      }
      
    }

    // Returns true if we're in trace mode.
    bool check_record_trace()
    {
      return _record_trace;
    }

    void addRegEntry(Pairs &pairs)
    {
      adl::addRegEntry(cur_target()._regs,pairs);
    }

    void addTraceFrame(unsigned id)
    {
      cur_target()._trace.push_back(Frame(id)); 
    }

    void setFrameName(const Token &n)
    {
      cur_target()._trace.back().set_name(n);
    }

    Card &addTraceEntry(Card &&card)
    {
      BasicData &ct = cur_target();
      ct._trace.back().push_back(move(card));
      return ct._trace.back().back();      
    }

    Card &addTraceEntry(const Card &card)
    {
      BasicData &ct = cur_target();
      ct._trace.back().push_back(card);
      return ct._trace.back().back();      
    }

    bool handleCard(int ln,const Token &type)
    {         
      if (type == "TEST") {   
        _testcount++;
        _record_trace = _record_results = false;
        _id = 0;
        _test_start = true;
        // We allow a leading TEST card, but stop once we encounter that.
        if (!_flt_mode && !_first_test) {
          _stopped = true;
          _test_finished = true;
          _first_test = false;
          return false;
	  
        }
        _first_test = false;
        return true;
      }

      if (_test_start) {
        V1("Parsing test " << _testcount << ":  " << ((_is_expects) ? "Expect data." : "Found data."));
        _test_start = false;
      }

      // If we managed to get here before we saw TEST card
      if (_first_test) {
        _testcount = 1;
        _first_test = false;
      }

      if (type == "TRACE") {
        _record_trace = true;
        return true;
      }
      else if (type == "RESULTS") {
        _record_results = true;
        _record_trace = false;
        return true;
      }
      else if (type == "INIT") {
        _record_results = false;
        _record_trace = false;
        return true;
      }
      else if (type == "CORE") {
        Token name = get_str("n","core name");
        update_core(name.data());
        return true;
      }
      else if (type == "CTX") {
        Token name = get_str("n","context name");
        uint32_t cn  = get_uint32("cn","context number");
        string update = adl::get_str(_pairs,"u",0);
        update_context(name.str(),cn);
        return true;
      }
      else if (type == "NOCTX") {
        update_no_context();
        return true;
      }      
      if (!(_record_trace | _record_results)) {
        return true;
      }
      
      if (_flt_mode) {
        //this core should read all results for diff_final
        if (_tgt_core == "" && _tgt_ctx == "") {
          if (_tgt_test != _testcount) {
            return true;
          } else {
            //we read trace only when needed
            _record_trace   = _record_trace && (_cur_core->_name == "");
          }	
        } else {
          //take only core
          if (_tgt_ctx == "") {
            if (_tgt_test != _testcount ||
                _tgt_core != _cur_core->_name ||
                _cur_core->_cur_ctx != 0)
              {
                return true;
              }	
          } else {
            if ( !_cur_core->_cur_ctx      ||
                 _tgt_test    != _testcount ||
                 _tgt_core    != _cur_core->_name ||
                 _tgt_ctx     != _cur_core->_cur_ctx->_name||
                 _tgt_ctx_num != _cur_core->_contexts[ _cur_core->_cur_ctx->_name]._cur)
              {
                return true;
              }	
          }
        }       
      }
      
      _handled_cards += (_handled_cards==0);
	
      // If we should ignore this card, then accept it and continue.
      if (IgnoreCards.count(type)) {
        return true;
      }
      
      if (type == "I" || type == "ERW") {
        if(_is_expects) {
          global_id++;
        }
        addTraceFrame(global_id);
      }
      else if (type == "INSTR") {
        // Special case- don't report the asm string, since that 
        // can vary easily.
        if (check_record_trace()) {
          Card &c = addTraceEntry(Card(type,_pairs,true));

          auto v = c.find_key("asm");
          if (c.was_found(v)) {
            setFrameName(v->second);
            c.remove(v);
          }
        }
        ++_handled_cards;
	
        if (IncrementalScan && _handled_cards > MaxCommands) {
          _stopped = true;
        } else {
          _stopped = false;
        }
      }
      else if (type == "RD") {
        addRegEntry(_pairs);
      }
      else if (type == "MD") {
        
        string name = get_str("n","memory name.");
        addr_t addr = get_uint64("ra","address.");
        uint32_t data = get_uint32("d","data.");         
	
        // Note: All memory of name "Mem" is stored in the same core, since memory
        // is considered global.
        if (name == "Mem") {
          _mem->_mem[name].push_back(make_pair(addr,data));
        } else {
          _cur_core->_mem[name].push_back(make_pair(addr,data));
        }
      }
      else if (type == "CD") {
        string name  = get_str("n","cache name.",true);
        uint32_t set   = get_uint32("set","cache set.",true);
        uint32_t way   = get_uint32("way","cache way.",true);
        if (!IgnoreCaches.count(name)) {
          _cur_core->_cache[Key(name,set,way)] = Card(type,_pairs);
        }
      }
      else if (type == "B") {
        if (check_record_trace()) {
          Card c(type,_pairs);
          c._data.push_back(make_pair("t","brk"));
          addTraceEntry(c);
        }
      }
      else if (type == "TD") {
        string name = get_str("n","TLB lookup name.",true);
        uint32_t set =  get_uint32("set","TLB lookup set value.",true);
        uint32_t way =  get_uint32("way","TLB lookup way value.",true);      
        _cur_core->_mmu[Key(name,set,way)] = Card(type,_pairs);
      }
      else {
        // Anything else is simply pushed onto the current frame if in
        // trace mode, or else ignored w/a warning.
        if (check_record_trace() || (_flt_mode && _tgt_core == "")) {
          if (type == "R") {
            string action = get_str("a",0);
            if (action == "read") {
              // Skip reads for now- only compare register writes.
              return true;
            }
          }
          else if (type == "C") {
            string name  = get_str("n",0);
            // Skip specified caches, if relevant.
            if (IgnoreCaches.count(name)) {
              return true;
            }
          }
          addTraceEntry(Card(type,_pairs));
        } else  {
          Msg(_fn << ":" << ln << ":  Warning:  Unknown data type " << type);
        }
      }

      return true;
    }
  
    void error(int ln,const std::string &msg)
    {
      Msg(_fn << ":" << ln << ":  " << msg);
    }

  };

  AnyOption options;

  template <class E>
  struct FindKey {};

  template<> struct FindKey< pair<addr_t,uint32_t> >
  {
    bool operator()(addr_t x,const pair<addr_t,uint32_t> &y) {
      return x < y.first;
    }
    bool operator()(const pair<addr_t,uint32_t> &x,addr_t y) {
      return x.first < y;
    }
  };

  template <class C> struct find_element {
    const typename C::value_type *operator()(const C &c,typename C::value_type::first_type key)
    {
      auto range = equal_range(c.begin(),c.end(),key,FindKey<typename C::value_type>());
      if ( range.first != range.second ) {
        return &(*range.first);
      } else {
        return 0;
      }  
    }
  };

  template <class K,class V> struct find_element< unordered_map<K,V> > {
    typedef  unordered_map<K,V> container;
    const typename container::value_type *operator()(const container &c,typename container::value_type::first_type key)
    {
      auto iter = c.find(key);
      if ( iter != c.end() ) {
        return &(*iter);
      } else {
        return 0;
      }  
    }
  };

  // This compares hashes using set_difference to find what's not the same in the two.
  template <class C>
  void compare_hashes(const C &exp,const C &fnd,const string &msg,const KeyTemplate *tmpl,bool ignore_extra = false)
  {
    KeyTemplateUse ktu(tmpl);
    bool errors = false;
    typedef vector<typename C::value_type> Missing;
    Missing missing;
    back_insert_iterator<Missing> result(missing);
    set_difference(exp.begin(),exp.end(),fnd.begin(),fnd.end(),result);
    if (!missing.empty()) {
      ForEach(missing,mi) {
        find_element<C> fe;
        auto fnd_item = fe(fnd,mi->first);
        if (fnd_item) {
          Msg(msg << showbase << hex << ":  Expected " << mi->first << " = " << mi->second << ", Found " << fnd_item->second);
          errors = true;
        } else if (!ignore_extra) {
          Msg(msg << showbase << hex << ":  Expected " << mi->first << " = " << mi->second << ", Not found in found file.");
          errors = true;
        }
      }
      if (errors) {
        RError(msg);
      }
    }
  }

  // This compares hashes by searching the found set for each element in the
  // expected set.  Thus, extra elements in the found set are ignored.  The
  // requirement is just that everything that's expected must exist (unless
  // ignore_extra_exp is true) and the value must match.
  template <class C>
  void compare_hashes2(const C &exp,const C &fnd,const string &msg,const KeyTemplate *tmpl,bool ignore_extra_exp = false)
  {
    KeyTemplateUse ktu(tmpl);
    bool errors = false;
    ForEach(exp,e) {
      auto f = fnd.find(e->first);
      if (f == fnd.end()) {
        if (!ignore_extra_exp) {
          Msg("\n" << msg << ":  Did not find a value for " << e->first);
          errors = true;
        }
      } else {
        if (! (e->second == f->second) ) {
          Msg("\n" << msg << hex << ":\nExpected " << e->first << " = " << e->second << "\nFound " << f->second << dec);
          errors = true;
        }
      }
    }
    if (errors) {
      RError(msg);
    }
  }

  typedef list<string> ErrorList;

  // This is implemented as a functor so that string creation is delayed unless we
  // absolutely need it.
  struct CountStr {
    CountStr(Token &n,bool d,unsigned id,unsigned lc) : _name(n), _dat(d), _id(id), _local_count(lc) {};
    Token     _name;
    bool      _dat;
    unsigned  _id;
    unsigned  _local_count;
  };

  ostream &operator<<(ostream &o,const CountStr &cs)
  {
    o << cs._id;
    if (!cs._name.empty() && PrintAsm) {
      o << " '" << cs._name << "'";
    }
    if (ShowThreadCount) {
      o << " (local count " << cs._local_count << ")";
    }
    return o;
  }

  // Predicate for whether we should compare issue timing data.
  bool should_check_itime(int instr_index)
  {
    if (IgnoreITime || instr_index < InstrTimingOffset) {
      return false;
    } else {
      return true;
    }
  }

  // Predicate for whether we should compare completion timing data.
  bool should_check_ctime(int instr_index)
  {
    if (IgnoreCTime || instr_index < InstrTimingOffset) {
      return false;
    } else {
      return true;
    }
  }

  // There 's some gcc 3.4.3 weirdness, where thre is a llabs conflict between
  // /usr/include and the compiler's headers.  So, I just redefine it here.
  inline int64_t abs(int64_t x) { return x >= 0 ? x : -x; }

  void check_time(int instr_index,int64_t f_time,int64_t e_time,
                  int64_t &adjust_time,int64_t &total_time_error,
                  int64_t &start_time,int64_t &total_time,int &fails,
                  ErrorList &time_errors,const CountStr &count_str,
                  const string &cname,const string &msg,int &last_fail_index)
  {
    // If we have an instruction offset, and we hit that value (which means that
    // we start checking), then the first time we simply adjust our time values
    // to equalize them.  Otherwise, we do our timing check.
    if (!InstrTimingOffset || (instr_index != InstrTimingOffset)) {
      // Our compare time is the found time + the timing adjustment.  The timing
      // adjustment is used to adjust the current found time, based upon prior
      // delta error.  This way, we don't get tons of errors once we're off- we
      // just report the first error and then continue.
      int64_t cmp_time = f_time+adjust_time;
      if ( cmp_time != e_time) {
        if (MaxDelta) {
          if ( abs(cmp_time - e_time) >= MinTimeError) {
            string s;
            MkStr(s,dec << cname << ":  Instr " << count_str << " (+" << (instr_index - last_fail_index) << "):  Incorrect " << msg << ":  Found " << f_time << " (adjusted to " 
                  << cmp_time << "), expected " << e_time << " (" << (cmp_time - e_time) << ")");
            time_errors.push_back(s);
          }
          total_time_error += abs(cmp_time-e_time);
        } else {
          Msg(dec << cname << ":  Instr " << count_str << " (+" << (instr_index - last_fail_index) << "):  Incorrect " << msg << ":  Found " << f_time << " (adjusted to " 
              << cmp_time << "), expected " << e_time << " (" << (cmp_time - e_time) << ")");
          ++fails;
        }
        // Update our last-fail counter.
        last_fail_index = instr_index;
      }
    } else {
      // First time through: Get our start time, so that we ignore the time not
      // associated with what we're checking.
      start_time = e_time;
    }
    adjust_time = (e_time-f_time);
    total_time = e_time;
  }

  void final_check_time(int64_t total_time_error,int64_t total_time,int &fails,const ErrorList &time_errors,
                        const string &cname,const string &msg)
  {
    if (total_time_error) {
      double error_pctg = (double)total_time_error / (double)total_time;
      if (error_pctg > MaxDelta) {
        // We've exceeded our threshold, so it's an error.
        ForEach(time_errors,iter) {
          Msg(*iter);
        }
        Msg(cname << "  Error:  " << (error_pctg*100) << "% (" << total_time_error << " cycles off out of " << total_time << ") error exceeded threshold for " << msg);
        ++fails;
      } else {
        // Not a hard-error, but we'll print a warning so that it can be grepped.
        Msg(cname << ":  Warning:  Time differences were found for " << msg << ", but were less than the allowed time delta.\n" 
            "              " << dec << total_time_error << " cycles off out of " << total_time << " cycles (" << (error_pctg*100) << "% error).");
      }
    }
  }

  // Returns true if the 'ra' and 'ea' values match.  Returns false if they do
  // not, or if the relevant tags are not found.
  bool same_addr (Card &x,Card &y)
  {
    auto x_ra_iter = x.find_key("ra");
    if (x.was_found(x_ra_iter)) {
      addr_t x_ra = atoi64(x_ra_iter->second,0);
      auto y_ra_iter = y.find_key("ra");
      if (y.was_found(y_ra_iter)) {
        addr_t y_ra = atoi64(y_ra_iter->second,0);
        if (x_ra != y_ra) {
          return false;
        }
      } else {
        return false;
      }
    }
    auto x_ea_iter = x.find_key("ea");
    if (x.was_found(x_ea_iter)) {
      addr_t x_ea = atoi64(x_ea_iter->second,0);
      auto y_ea_iter = y.find_key("ea");
      if (y.was_found(y_ea_iter)) {
        addr_t y_ea = atoi64(y_ea_iter->second,0);
        if (x_ea != y_ea) {
          return false;
        }
      } else {
        return false;
      }
    }
    return true;
  }

  // Return true if we should strictly check for a specific type of
  // card/command.
  bool checkAll(const char *card)
  {
    return (CheckAll || CheckAllCards.count(card));
  }

  bool checkAll(const string &card)
  {
    return checkAll(card.c_str());
  }

  void diff_ir(BasicData &exp, BasicData &fnd, const string &cname,
               bool length_check, bool DAT, bool strict_check=true)
  {
    // If there's no expected trace data, then we just exit.
    
    if ((fnd._trace.empty() && !exp._trace.empty()) && length_check && strict_check) {          
      Msg(cname << ":  More instructions were found in expected trace than found trace.");
      RError("Intermediate results mismatch.");
    }    
    
    auto exp_b = exp._trace.begin();
    auto exp_e = exp._trace.end();

    //need to save in order to know which we can erase
    auto fnd_b = fnd._trace.begin();
    auto fnd_e = fnd._trace.end();

    auto exp_begin = exp_b;
    auto fnd_begin = fnd_b;

    if (ParseOnly) {
      exp._trace.clear();    
      fnd._trace.clear();
      return;
    }  

    int64_t      start_itime = 0, start_ctime = 0;
    int64_t      total_itime = 0, total_ctime = 0;
    int64_t      total_itime_error = 0, total_ctime_error = 0;
    int64_t      adjust_itime = 0, adjust_ctime = 0;
    ErrorList    itime_errors, ctime_errors;

    unsigned &id = exp._id;

    int fails = 0;    

    int last_issue_fail_index = 0;
    int last_compl_fail_index = 0;
    
    for ( ; exp_b != exp_e; ++exp_b, ++fnd_b, ++id ) {
      if (fnd_b == fnd_e) {
        if (strict_check && length_check) {          
          Msg(cname << ":  More instructions were found in expected trace than found trace.");
          RError("Intermediate results mismatch.");
        } else {
          break;
        }
      }     
      
      Frame &exp_f = *exp_b;
      Frame &fnd_f = *fnd_b;

      CountStr count_str(exp_f._n,DAT,exp_f._id,id);

      int64_t f_itime = 0, f_ctime = 0;
      int64_t e_itime = 0, e_ctime = 0;
      bool has_itime = false, has_ctime = false;

      IrHash values;

      RegHash exp_regs;
      RegHash fnd_regs;

      bool exp_eot = false, fnd_eot = false;

      // Initial processing: Hash registers in order to collapse multiple
      // entries and handle time values in a special manner.
      ForEach(fnd_f,fiter) {
        if (fiter->_type == "RES") {
          fnd_eot = true;
        }
        if (fiter->_type == "ITIME") {
          f_itime = get_uint64(fiter->_data,"t");
          has_itime = true;
        }
        else if (fiter->_type == "CTIME") {
          f_ctime = get_uint64(fiter->_data,"t");
          has_ctime = true;
        }
        else if (fiter->_type == "R") {
          addRegEntry(fnd_regs,fiter->_data);
        }
        else if (fiter->_type == "M" && RemoveConsecutiveMem) {
          // By default, squash consecutive memory accesses of the same type.
          // This can be disabled.  We take the last, so that for stores, we get
          // the last update.
          auto &vals = values[fiter->_type].second;
          if (vals.empty() || !same_addr(vals.back(),*fiter)) {
            // Nothing yet, or different address, then add item.
            vals.push_back(*fiter);
          } else {
            // Otherwise, it's the same address, so overwrite with new
            // information so that we keep the last item.
            vals.back() = *fiter;
          }
        } else {
          values[fiter->_type].second.push_back(*fiter);
        }
      }

      // We also hash the expected registers in order to collapse multiple entries.
      ForEach(exp_f,eiter) {
        if (eiter->_type == "RES") {
          exp_eot = true;
        }
        if (eiter->_type == "R") {
          addRegEntry(exp_regs,eiter->_data);
        }
      }

      V2("Instruction " << dec << count_str << ":  Expected:\n\n" << exp_f << "\n\nFound:  Non-regs:  " << values << "\n\nRegs:  " << fnd_regs << "\n\n");
      cout << showbase;

      // These checks are a hack for UVPs, since the last instruction in a UVP is
      // a RES card, so it can fool the length checking in an off-by-one
      // situation.
      if (length_check) {
        if (exp_eot && !fnd_eot) {
          Msg(cname << ":  More instructions were found in found trace than expected trace.");
          RError("Intermediate results mismatch.");
        }
        if (fnd_eot && !exp_eot) {
          Msg(cname << ":  More instructions were found in expected trace than found trace.");
          RError("Intermediate results mismatch.");
        }
      }
  
      unordered_set<const RegHash::value_type *> processed_regs;
      // First, check the registers by iterating over the hashes we've created.
      ForEach(exp_regs,eiter) {
        const string   &n = eiter->first;
        const RegEntry &d = eiter->second;

        auto f_iter = fnd_regs.find(n);
        if (f_iter == fnd_regs.end()) {
          if (!IgnoreExtraFields) {
            Msg(cname << ":  Instr " << count_str << ":  Register " << n << " update not found:  Expected " << hex << d << dec);
            ++fails;
          }
        } else {
          if (f_iter->second != d) {
            Msg(cname << ":  Instr " << count_str << ":  Register " << n << " mismatch:  Found " << hex << fnd_regs[n] << ", expected " << d << dec);
            ++fails;
          } else {
            processed_regs.insert(&*f_iter);
          }
        }
      }
      if (checkAll("R")) {
        // If we're checking everything, then check for any extra updates in the found list.
        ForEach(fnd_regs,f_iter) {
          if (!processed_regs.count(&*f_iter)) {
            Msg(cname << ":  Instr " << count_str << ":  Unexpected register update found:  " << f_iter->first << " = " << hex << f_iter->second << dec);
            ++fails;
          }
        }
      }

      // Now go through and check the expected ir against the hashes.
      ForEach(exp_f,eiter) {
        try {
          if ( eiter->_type == "ITIME" && has_itime && should_check_itime(exp_f._id)) {
            e_itime = get_uint64(eiter->_data,"t");
            check_time(exp_f._id,f_itime,e_itime,adjust_itime,
                       total_itime_error,start_itime,total_itime,fails,
                       itime_errors,count_str,cname,"issue-time",last_issue_fail_index);
          }
          else if ( eiter->_type == "CTIME" && has_ctime && should_check_ctime(exp_f._id)) {
            e_ctime = get_uint64(eiter->_data,"t");
            check_time(exp_f._id,f_ctime,e_ctime,adjust_ctime,
                       total_ctime_error,start_ctime,total_ctime,fails,
                       ctime_errors,count_str,cname,"completion-time",last_compl_fail_index);
          }
          else if ( eiter->_type == "R") {
            // Ignore because we've already checked the registers.
          }
          else {
            // Generic case:  Compare values for equality.  Skip if the found-instruction doesn't have
            // any intermediate results of this type.  If CheckAll is set, then indicate an error if we
            // don't have intermediate results of this type.  Also, we force memory ops to be in the check-all
            // mode, since we never want to let an instruction no-op a memory operation.  Mainly, we just want to
            // ignore things like cache and tlb activity.
            pair<unsigned,Frame> &ir_f = values[eiter->_type];          
            if (checkAll(eiter->_type) && ir_f.second.empty()) {
              Msg(cname << ":  Instr " << count_str << ":  No intermediate results of type " << eiter->_type << " found.");
              ++fails;
            } else {
              if (ir_f.first < ir_f.second.size()) {
                int fiter = ir_f.first++;
                Card &found  = ir_f.second[fiter];
                Card &expect = *eiter;

                Pairs missing;
                back_insert_iterator<Pairs> result(missing);
                if (IgnoreExtraFoundFields) {
                  set_difference(expect._data.begin(),expect._data.end(),found._data.begin(),found._data.end(),result,IntPairComp());
                } else {
                  set_symmetric_difference(expect._data.begin(),expect._data.end(),found._data.begin(),found._data.end(),result,IntPairComp());
                }
                if (!missing.empty()) {
                  if (IgnoreExtraFields) {
                    // If we're ignoring missing fields, then remove anything
                    // that's in the missing list, but doesn't have a key in the
                    // found list.  This will just leave elements where there was
                    // a value mismatch.
                    auto iter = missing.begin();
                    for ( ; iter != missing.end(); ) {
                      if (found._data.not_found(found._data.find_key(iter->first))) {
                        auto tmp = iter;
                        missing.erase(tmp);
                        // For safety, start over.
                        iter = missing.begin();
                      } else {
                        ++iter;
                      }
                    }
                    if (missing.empty()) {
                      continue;
                    }
                  }
                  Msg(cname << ":  Instr " << count_str << ":  IR element " << fiter+1 << " of type " << eiter->_type << ":  Bad tags:  " << missing << "\n"
                      << "  Found data:  " << found._data);
                  ++fails;
                }
              } else if (checkAll(eiter->_type)) {
                Msg(cname << ":  Instr " << count_str << ":  Missing expected IR:  " << eiter->_data);
                ++fails;
              }
            }
          }
        }
        catch (runtime_error &err) {
          RError("Instr " << count_str << ":  " << err.what());
        }

        if (MaxFailCount && fails > MaxFailCount) {
          Msg("Exceeded max fail count of " << MaxFailCount);
          goto Exit;
        }
      }
    }

  Exit:

    if (MaxDelta && !IgnoreITime) {
      // First, remove time not associated with the checking window.
      total_itime -= start_itime;
      // Now check the time.
      final_check_time(total_itime_error,total_itime,fails,itime_errors,cname,"issue-time");
    }

    if (MaxDelta && !IgnoreCTime) {
      // First, remove time not associated with the checking window.
      total_ctime -= start_ctime;
      // Now check the time.
      final_check_time(total_ctime_error,total_ctime,fails,ctime_errors,cname,"completion-time");
    }

    if (fails) {
      RError("Differences were found when checking intermediate results.");
    }
   
    //clean trace, we dont need it
    exp._trace.erase(exp_begin,--exp_b);    
    fnd._trace.erase(fnd_begin,--fnd_b);
    id--;

  }

  // If the found hash has no data for its final cache results, then remove from
  // the expects hash to avoid a miscompare.
  void check_for_cache_data(KeyHash &exp,KeyHash &fnd)
  {
    // Do we have any cache data?
    bool has_data = false;
    ForEach(fnd,f) {
      if (f->second.was_found(f->second.find_key("d"))) {
        has_data = true;
      }
    }
    if (!has_data) {
      // No data found, so remove from expects.
      ForEach(exp,e) {
        e->second.remove("d");
      }
    }
  }

  // This compares final result data for a BasicData object, which could be either
  // a core or a context.
  void diff_data(BasicData &exp,BasicData &fnd)
  {
    KeyTemplate cache_tmpl("set",false,"way",false), tlb_tmpl("ep",true,"way",false);

    // Check non-context registers.
    compare_hashes2(exp._regs,fnd._regs,"Register mismatch",0);

    // Check caches.
    check_for_cache_data(exp._cache,fnd._cache);
    compare_hashes2(exp._cache,fnd._cache,"Cache-line mismatch",&cache_tmpl);
    if (checkAll("CD")) {
      // If we want an exect comparison with cache data cards, then compare in
      // the reverse order.
      compare_hashes2(fnd._cache,exp._cache,"Cache-line mismatch",&cache_tmpl);
    }

    // Check TLBs.
    compare_hashes2(exp._mmu,fnd._mmu,"TLB-entry mismatch",&tlb_tmpl,IgnoreExtraTlbs);

    // Memory checking:  Check all expected memory arrays.
    ForEach(exp._mem,j) {
      
      const AddrHash &exp_mem = j->second;
      
      auto fi = fnd._mem.find(j->first);
      if (fi == fnd._mem.end()) {
        if (IgnoreExtraMem) {
          // If we're ignoring extra memory, then we allow the case that there is
          // no memory at all- the extreme case of there being extra memory in the
          // initial file that's not in the final file.
          continue;
        }
        RError("No data in 'found' file for memory '" << j->first << "'.");
      }
      const AddrHash &fnd_mem = fi->second;
    
      compare_hashes(exp_mem,fnd_mem,"Memory '"+j->first+"' mismatch",0,IgnoreExtraMem);
    }
  }
  
  // Diffs intermediate results
  // in filter mode checks only specified core,context etc,
  // if it is a last diff to be done length check is turned on  
  void diff_intermediate(TraceData *expects, TraceData *found, 
                         bool length_check,
                         bool DAT,
                         bool flt_mode=false,
                         string   tgt_core="",
                         string   tgt_ctx="",
                         unsigned tgt_ctx_num = 0,
                         bool     last_diff = true) 
  {  
    // If we're not checking intermediate results, then return immediately.
    if (IgnoreTrace) {
      return;
    }
    int fails = 0;
    expects->_diff_done = true;
    ForEach(expects->_cores,i) {
      if (flt_mode && i->second._name != tgt_core) {
        continue;
      }
      try {
        CoreData &exp_core = i->second;

        auto fi = found->_cores.find(i->first);
        if (fi == found->_cores.end()) {
          if (!exp_core.empty()) {
            RError("No data in 'found' file for core '" << i->first << "'.");
          }
          continue;
        }
        CoreData &fnd_core = fi->second;

        // Check intermediate results for each context.
        
        ForEach(exp_core._contexts,exp_citer) {
          // 'tgt_ctx == ""' means 'check only core'
          if (flt_mode && tgt_ctx == "") {
            continue;
          }
          CtxDataArray &exp_ctxs = exp_citer->second;

          auto fnd_citer = fnd_core._contexts.find(exp_citer->first);
          if (fnd_citer == fnd_core._contexts.end()) {
            continue;
            RError("No data in 'found' file for context '" << exp_citer->first << "'.");
          }
          CtxDataArray &fnd_ctxs = fnd_citer->second;

          ostringstream ss;
          ForRange(exp_ctxs._data.size(),x) {
            ss.str("");
            ss << i->first << ":" << exp_citer->first << x;
            if (flt_mode && tgt_ctx_num != x) {
              continue;
            }           
            try {
              if (x >= fnd_ctxs.size()) {
                RError("No element " << x << " for 'found' file context '" << exp_citer->first);
              }
              CtxData &exp = exp_ctxs._data[x];
              CtxData &fnd = fnd_ctxs._data[x];

              diff_ir(exp,fnd,ss.str(),length_check,DAT,last_diff);	      
              found->_diff_done = true;

            }
            catch (runtime_error &err) {
              Msg(ss.str() << ":  " << err.what());
              ++fails;
            }
          }

        }
        // Check intermediate results if the expects file has them.

        //dont diff core if came to diff context
        if (flt_mode && tgt_ctx != "") {
          continue;
        }
        diff_ir(exp_core,fnd_core,i->first,length_check,DAT,last_diff);
        found->_diff_done = true;

      }
      catch (runtime_error &err) {
        if (i->first.empty()) {
          throw;
        } else {
          Msg(i->first << ":  " << err.what());
          ++fails;
        }
      }
    }
    if (fails) {
      RError("Intermediate result mismatches were found.");
    }
  }  
     

  // This is the same for incremental diff and for usual because
  // we save all results  in the outer "core"
  void diff_final(TraceData *expects, TraceData *found) {
    ForEach(expects->_cores,i) {
      try {
        CoreData &exp_core = i->second;

        auto fi = found->_cores.find(i->first);

        if (fi == found->_cores.end()) {
          if (!exp_core.empty()) {
            RError("No data in 'found' file for core '" << i->first << "'.");
          }
          continue;
        }

        CoreData &fnd_core = fi->second;

        // Check contexts.
        ForEach(exp_core._contexts,exp_citer) {
          CtxDataArray &exp_ctxs = exp_citer->second;

          auto fnd_citer = fnd_core._contexts.find(exp_citer->first);
          if (fnd_citer == fnd_core._contexts.end()) {
            RError("No data in 'found' file for context '" << exp_citer->first << "'.");
          }
          CtxDataArray &fnd_ctxs = fnd_citer->second;

          ForRange(exp_ctxs._data.size(),x) {
            if (x >= fnd_ctxs.size()) {
              RError("No element " << x << " for 'found' file context '" << exp_citer->first);
            }
            CtxData &exp = exp_ctxs._data[x];
            CtxData &fnd = fnd_ctxs._data[x];
            
            try {
              diff_data(exp,fnd);
            }
            catch (runtime_error &err) {
              RError ("Context " << exp_citer->first << "[" << x << "]:  " << err.what());
            }
          }
        }

        // Check the data on the core itself.
        diff_data(exp_core,fnd_core);

      }
      catch (runtime_error &err) {
        if (i->first.empty()) {
          throw;
        } else {
          RError("Core " << i->first << ":  " << err.what());
        }
      }
    }
  }
    
  void diff(const string &src1,const string &src2,bool length_check,int max_tests)
  {
    unique_ptr<TraceData> expects;
    unordered_map <string,shared_ptr<TraceData> > founds;
    bool DAT = false;
    if (is_dat(src1) && is_dat(src2)) {
      expects.reset(new DatHandler(src1,true,IncrementalScan));
      DAT = true;
    } else if (is_uvp(src1) && is_uvp(src2)) {
      if (IncrementalScan) {
        RError("Incremental parsing not supported for UVP files.");
      }
      expects.reset(new UvpHandler(src1,true));
    } else {
      RError("Unknown or mismatched input file types.  Only DAT/DAT or UVP/UVP comparisons are allowed.");
    }

    int testcount = 0; 
    
    if (IncrementalScan) {
      //cerr << "inc" << endl;
      try {
        do {
          testcount++;
          if (max_tests && (testcount > max_tests)) {
            break;
          }
          global_id = 0;
          bool read = true;
          while(read) {
            read = expects->read_chunk();
            bool last_check = (read == 0);	    
            ForEach(expects->_cores,i) {	  
              CoreData &core = (i->second);
              if (founds.find(core._name) == founds.end()) {
                unique_ptr<TraceData> h;
                assert(DAT);
                h.reset(new DatHandler(src2,false,IncrementalScan,true,core._name,"",0,testcount));
                founds.insert(make_pair(core._name,move(h)));	  
              }
              TraceData* found = founds.find(core._name)->second.get();
              found->read_chunk();
              diff_intermediate(expects.get(),found,length_check,DAT,true,core._name,"",0,last_check);
              ForEach(core._contexts,ii) {
                if(!DAT) {
                  RError("Only Dat-files can have contextes");
                  return;
                }
                CtxDataArray &ctx = ii->second;
                string ctx_name = ii->first;	      
                ForRange(ctx._data.size(),x) {
                  string search_name = core._name + ":" + ctx_name + ":" + uitoa(x);
                  if (founds.find(search_name) == founds.end()) {
                    unique_ptr<TraceData> h(new DatHandler(src2,false,IncrementalScan,true,core._name,ctx_name,x,testcount));
                    founds.insert(make_pair(search_name,move(h)));	  
                  }	   
                  TraceData* found = founds.find(search_name)->second.get();
                  found->read_chunk();
                  diff_intermediate(expects.get(),found,length_check,DAT,true,core._name,ctx_name,x,last_check);
                }

              }	    
            }     
          }

          expects->finalize_mem();
          TraceData* outer = founds.find("")->second.get();
          outer->finalize_mem();
          diff_final(expects.get(),outer);
          expects->reset();
          founds.clear(); 	

        } while (!expects->finished());
      }
      catch (runtime_error &err) {
        RError("Test " << testcount << ":  " << err.what());
      }
      V1("Succeeded."); 
    
    } else {
      unique_ptr<TraceData> found;
      if (DAT) {
        found.reset(new DatHandler(src2,false,false));
      } else  {
        found.reset(new UvpHandler(src2,false));
      } 
      try {      
        do {
          ++testcount;
          
          global_id = 0;

          if (max_tests && (testcount > max_tests)) {
            break;
          }

          if (!expects->read_file()) {
            break;
          }
          found->read_file();

          if (expects->_handled_cards == 0) {
            RError("No data found in expects file " << src1);
          }

          if (expects->_handled_cards == 0) {
            RError("No data found in found file " << src2 );
          }

          V1("Done parsing.");

          if (vlevel(2)) {
            opfxstream oln(VStream.rdbuf(),2,'{','}');
            oln << "Expected:\n\n"
                << expects->_cores << "\n\n"
                << "Found:\n\n"
                << found->_cores << "\n\n";
          }
          diff_intermediate(expects.get(),found.get(),length_check,DAT);
          diff_final(expects.get(),found.get());
          expects->reset();
          found->reset();
        } while (!expects->finished());  
      }
      catch (runtime_error &err) {
        RError("Test " << testcount << ":  " << err.what());
      }
      V1("Succeeded.");
    }        
  }

  // Given a command-line option, this looks for multiple values and inserts them
  // into the specified string set.  If the option does not exist, then no action
  // is taken.
  template <class Set>
  void addOptionStrings(Set &set,const std::string &option)
  { 
    try {
      auto res = options.getValues(option);
      ForEach(res,i) {
        set.insert(*i);
      }
    }
    catch (runtime_error &err) { ; }
  }

  // Opposite of addOptionStrings- removes elements from a set if listed.
  template <class Set>
  void delOptionStrings(Set &set,const std::string &option)
  { 
    try {
      auto res = options.getValues(option);
      ForEach(res,i) {
        auto iter = set.find(*i);
        if (iter != set.end()) {
          set.erase(iter);
        }
      }
    }
    catch (runtime_error &err) { ; }
  }

  void setupCoreNameMap()
  {
    auto v = options.getValues("map-core-name");
    ForEach(v,i) {
      string s = i->c_str();
      unsigned n = s.find(':');
      if (n == string::npos) {
        RError("Bad core-name translation option:  " << n << ".  Expected 'initial:final'.");
      }
      CoreNameMap.insert(make_pair(s.substr(0,n),s.substr(n+1)));
    }
  }

}

int main(int argc, const char** argv)
{
  using namespace adl;

  try {
    options.addUsage(":  [options] <expect-file> <found-file>");
    options.setFlag(  "help","h",                   "Display help.");
    options.setFlag  ("quiet","q",                  "Quiet mode.");
    options.setFlag  ("ir-length-check",            "Perform trace length-checking:\nThe expected and found traces should have the same length.");
    options.setFlag  ("ignore-extra-mem",           "Ignores memory if the address is not in the found file.");
    options.setFlag  ("ignore-extra-tlbs",          "Ignores T-cards if the set/way pair is not in the found file.");
    options.setFlag  ("ignore-extra-fields",        "Ignores fields which are in the expects file but not in the found file.  The default is "+btoa(IgnoreExtraFields)+".");
    options.setFlag  ("ignore-extra-found-fields",  "Ignores fields which are in the found file but not in the expects file.  The default is "+btoa(IgnoreExtraFoundFields)+".");
    options.setFlag  ("ignore-timing",              "Ignores issue and completion data.  Default is false");
    options.setFlag  ("ignore-itime",               "Ignores issue time data.  Default is "+btoa(IgnoreITime)+".");
    options.setFlag  ("ignore-ctime",               "Ignores completion time data.  Default is "+btoa(IgnoreCTime)+".");
    options.setFlag  ("parse-only",                 "Exit after parsing input files.");
    options.setFlag  ("inc-scan",                   "Use incremental scanning for parsers. Suitable for very large files, for small files can be slower than usual.");;
    options.setFlag  ("full-path-errors","fpe",     "Use full paths for error messages.");
    options.setFlag  ("compare-global-count","cgc", "When comparing UVPs, compare the global count (gc) instruction key.\n"
                                                    "The default is "+btoa(CompareGlobalCount)+" (ignore this key).");
    options.setFlag  ("ignore-ir",                  "Do not check intermediate results (the trace).");
    options.setFlag  ("ignore-exception-names",     "Ignore the names of exceptions in intermediate results for UVPs.\n"
                                                    "The default is "+btoa(IgnoreExcptNames)+" (do not compare the names).");
    options.setFlag  ("check-all",                  "Check all command types, even if the found-file does not have them (which will create an error).\n"
                                                    "This also checks that no extra elements of a given type exist, such as extra, unexpected register updates.\n"
                                                    "The default is "+btoa(CheckAll)+" (ignore if found file does not have that type, or the update is extra).");
    options.setFlag  ("print-asm",                  "If true, error messages include the assembly string, to aid in debugging.\n"
                                                    "The default is "+btoa(PrintAsm)+".");
    options.setFlag  ("remove-consecutive-mem",     "If true, consecutive equivalent intermediate-result memory cards are removed, leaving just a single element.\n"
                      "If false, then all are added.  The default is "+btoa(RemoveConsecutiveMem)+".  This option may be negated.");
    options.setFlag  ("local-count",                "Always show a thread-local instruction count.");
    options.setOption("max-cmd",                    "For an incremental scan, this is the number of commands to compare in a single chunk.\n"
                                                    "Default is "+uitoa(MaxCommands)+".");
    options.setOption("verbose","v",                "Verbosity level.  Level 0 is off, level 1 is minimal, level 2 is very verbose.");
    options.setOption("max-tests",                  "If non-zero, check only the specified number of tests.");
    options.setOption("fail-count",                 "If non-zero, end checking if the specified number of fails is found.\n"
                                                    "This option may be turned off by either setting the value to 0 or using a 'no' prefix.\n"
                                                    "The default is "+uitoa(MaxFailCount)+".");
    options.setOption("max-delta",                  "If non-zero, report an error if the difference in time vs. the total time in the test exceeds\n"
                                                    "the specified percentage.  The default is "+dtoa(MaxDelta)+".\n"
                                                    "Note:  This value represents the error percentage.  In other words, a value of 0.10 means that\n"
                                                    "       the cumulative total of time differences in the file must be less than 10% of the total amount of\n"
                                                    "       time of the entire test (the final completion time).");
    options.setOption("min-time-error","mte",       "Specify the minimum time difference for reporting errors on a test which fails due to time differences.\n"
                                                    "The default is 0:  Report all errors.");
    options.setOption("instr-timing-offset","ito",  "Specify the instruction index at which to start checking timing, if so enabled.  This is checked against\n"
                                                    "the ID values in the instruction trace.  When encountered, the expected and found times are synchronized\n"
                                                    "and comparisons start ater this point.  The default is "+uitoa(InstrTimingOffset)+".");
    options.setOptionalValue("fail-count");
    options.setOption("ignore-reg","ir",            "Ignore the specified register, if it exists, e.g. --ignore-reg=FOO.  This option may be repeated.");
    options.setOption("ignore-cache",               "Ignore the specified cache, if it exists, e.g. --ignore-cache=L1i.  This option may be repeated.");
    options.setOption("ignore-field","if",          "Ignore the specified field, if it exists, e.g. --ignore-field=ta.\n"
                                                    "Top-level fields in a command may be ignored, as well as some special cases, such as elements of \n"
                                                    "the 'd' key in a TLB final result.  This option may be repeated.");
    options.setOption("ignore-card","ic",           "Ignore the specified card/command, if it exists, e.g. --ignore-card=I.  This option may be repeated.\n"
                                                    "For DAT files, this corresponds to the name of the card.  This UVP files, this corresponds to the name\n"
                                                    "of the command; the port value is ignored, e.g. to ignore E:I cards, use --ignore-card=I");
    options.setOption("check-all-card",             "Check all commands of the type specified, even if the found-file does not have them (which will create an error).\n"
                                                    "This also checks that no extra elements of a given type exist, such as extra, unexpected register updates.\n"
                                                    "By default, memory cards fall into this category.  Note:  Using the --check-all option is equivalent to listing\n"
                                                    "all card types using this option.");
    options.setOption("no-check-all-card",          "Remove cards from the list to be strictly checked.  This is the opposite of --check-all-cards.");
    options.setOption("reg-mask",                   "Specify a register-comparison mask.  If 0, the mask is ignored.  This allows the user to specify\n"
                                                    "that only a portion of the registers, such as the lower 32-bits, should be compared.\n"
                                                     "Default is 0.");
    options.setOption("map-core-name",              "Create a mapping for core names.  Format is --map-core-name=initial:final.  Whenever a CORE\n"
                                                    "card is encountered, if the name matches <initial>, it will be translated to <final>.\n"
                                                    "This option may be repeated.");

    options.useCommandArgs(argc,argv);


    if (!options.processCommandLine()) {
      return 1;
    }

    if (options.getFlag("h")) {
      options.printUsage();
      return 0;    
    };

    Verbose                = options.getOption("v",Verbose);
    int  max_tests         = options.getOption("max-tests",0);
    
    MaxCommands            = options.getOption("max-cmd",(int)MaxCommands);
    IncrementalScan        = options.getFlag("inc-scan",IncrementalScan);
    ParseOnly              = options.getFlag("parse-only",ParseOnly);
    Quiet                  = options.getFlag("q",Quiet);
    IgnoreExtraMem         = options.getFlag("ignore-extra-mem",IgnoreExtraMem);
    IgnoreExtraTlbs        = options.getFlag("ignore-extra-tlbs",IgnoreExtraTlbs);
    IgnoreExtraFields      = options.getFlag("ignore-extra-fields",IgnoreExtraFields);
    IgnoreExtraFoundFields = options.getFlag("ignore-extra-found-fields",IgnoreExtraFoundFields);
    IgnoreITime            = options.getFlag("ignore-itime",IgnoreITime);
    IgnoreCTime            = options.getFlag("ignore-ctime",IgnoreCTime);
    IgnoreExcptNames       = options.getFlag("ignore-exception-names",IgnoreExcptNames);
    FullPathErrors         = options.getFlag("fpe",FullPathErrors);
    CompareGlobalCount     = options.getFlag("cgc",CompareGlobalCount);
    IgnoreTrace            = options.getFlag("ignore-ir",IgnoreTrace);
    CheckAll               = options.getFlag("check-all",CheckAll);
    bool length_check      = options.getFlag("ir-length-check",true);
    PrintAsm               = options.getFlag("print-asm",PrintAsm);
    RemoveConsecutiveMem   = options.getFlag("remove-consecutive-mem",RemoveConsecutiveMem);
    ShowThreadCount        = options.getFlag("local-count",ShowThreadCount);
    MaxDelta               = options.getOption("max-delta",MaxDelta);
    MinTimeError           = options.getOption("mte",MinTimeError);
    InstrTimingOffset      = options.getOption("ito",InstrTimingOffset);
    string regmask         = options.getOption("reg-mask");

    if (!regmask.empty()) {
      // A non-numerical value will parse to 0, meaning that the mask is ignored.
      RegMask = RNumber(regmask);
    }

    bool ignore_timing = options.getFlag("ignore-timing",false);
    if (ignore_timing) {
      IgnoreITime = IgnoreCTime = ignore_timing;
    }

    if (MaxDelta) {
      if (MaxDelta < 0 || MaxDelta > 1.0) {
        cout << "The maximum time delta (--max-delta) value must be a percentage ( >= 0 && <= 1.0).\n";
        options.printUsage();
        return 1;
      }
    }

    if (options.haveOption("fail-count")) {
      if (options.getFlag("fail-count")) {
        MaxFailCount      = options.getOption("fail-count",MaxFailCount);
      } else {
        MaxFailCount = 0;
      }
    }

    // Setup the core-name translation map.
    setupCoreNameMap();

    // See if we have any ignore-regs.  Ignore if we don't.
    addOptionStrings(IgnoreRegs,"ignore-reg");

    // See if we have any caches to ignore.
    addOptionStrings(IgnoreCaches,"ignore-cache");

    // See if we have any ignore-fields.  Ignore if we don't.
    addOptionStrings(IgnoreFields,"ignore-field");

    // See if we have any ignore-cards.  Ignore if we don't.
    addOptionStrings(IgnoreCards,"ignore-card");

    // Look for cards that we should always check.  We always add in memory
    // cards by default.
    CheckAllCards.insert("M");
    addOptionStrings(CheckAllCards,"check-all-card");
    delOptionStrings(CheckAllCards,"no-check-all-card");

    if (options.getArgc() < 2) {
      cout << "Two source files must be specified.\n";
      options.printUsage();
      return 1;
    }

    string src1 = options.getArgv(0);
    string src2 = options.getArgv(1);

    // If the files are too large we will set incremental scan to true,
    struct stat stat1,stat2;
    if (stat(src1.c_str(), &stat1) != 0) {
      RError ("Could not stat input file:  " << src1);
    }
    if (stat(src2.c_str(), &stat2) != 0) {
      RError ("Could not stat input file:  " << src2);
    }

    if (IncrementalScan) {
      V1("Incremental scanning mode will be used.");
    }

    diff(src1,src2,length_check,max_tests);
  }
  catch (exception &err) {
    Msg("Error:  " << err.what());
    return 1;
  }

  return 0;
}

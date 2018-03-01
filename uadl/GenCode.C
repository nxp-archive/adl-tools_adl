//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//


#include <assert.h>
#include <set>
#include <vector>
#include <fstream>
// TODO: remove after removing RError
#include <stdexcept>
#include <iostream>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/Environment.h"
#include "opencxx/Walker.h"

#include "adl_config.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/FilenameUtils.h"
#include "helpers/gc_hash_map.h"
#include "helpers/pfxstream.h"
#include "helpers/BitTwiddles.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "iss/MiscTypes.h"
#include "iss/GenInstrPacket.h"
#include "iss/GenMems.h"

#include "GenCode.h"
#include "UadlCommon.h"
#include "UadlParser.h"
#include "GenInstClass.h"
#include "UadlWalker.h"

using namespace std;
using namespace adl;
using namespace uadl;
using namespace Opencxx;
using namespace PtreeUtil;

namespace uadl {

  void FileItem::destroy()
  {
    delete _o;
    _o = 0;
    delete _of;
    _of = 0;
  }

  void FileMap::destroy()
  {
    ForEach( (*this), i) {
      i->second->destroy();
    }
    clear();
  }

  gc_string getHeaderMacroName(const UadlIr &ir)
  { 
    return "_" + getuArchClassName(ir) + "_H"; 
  }

  gc_string getImplMacroName(const UadlIr &ir)
  { 
    return "_" + getuArchClassName(ir) + "_IMPLEMENTATION_H"; 
  }

  const gc_string &tmName(const gc_string &name) 
  { 
    return name; 
  }

  gc_string getMemWrapType(const gc_string &name) 
  {
    return name + "_memwrap_t";
  }

  gc_string getCacheType(const gc_string &name)
  {
    return name + "_cache_t";
  }

  gc_string getMemObj(const gc_string &name)
  {
    return name + "_obj";
  }

  gc_string getMemObj(const MemoryBase &mem)
  {
    return getMemObj(mem.name());
  }

  gc_string getSemaphoreType()
  {
    return "Semaphore";
  }

  gc_string getFlagType()
  {
    return "Flag";
  }

  gc_string getStageLoggerArray()
  {
    return "_stage_loggers";
  }

  gc_string getPrologue(const UadlIr &ir,const GenConfig &config)
  {
    const gc_string prologue = gc_string() +
      "//\n" +
      "// ADL/Uadl version " + ADL_PACKAGE_VERSION + "\n" +
      "//\n" +
      "// Cycle-accurate/approximate model generated for " + ir.uArchName() + "\n" +
      (!config._preamble.empty() ? gc_string() +
       "// " + config._preamble + "\n" : "") +
      "//\n\n";
   
    return prologue;
  }

  void genImplPrologue1(ostream &oc,const UadlIr &ir,const GenConfig &config,bool first_file)
  {
    oc << getPrologue(ir,config);
    // This allows us to be able to either compile the ISS separately or
    // together as a single file.
    if (first_file) {
      oc << "#ifdef UADL_SEPARATE_COMPILE\n"
         << "#  define ADL_HEADER\n"
         << "#endif\n";
    } else {
      oc << "#define ADL_HEADER\n";
    }

    oc << "#include \"" << base_name(ir.ISS_HFilename().c_str()) << "\"\n"
       << "#define " << getImplMacroName(ir) << "\n"
       << "#include \"" << base_name(ir.uArchHFilename().c_str()) << "\"\n"
       << "\n"
       << "using namespace std;\n"
       << "using namespace adl;\n"
       << "using namespace " << getIssNamespace(config._nameSpace) << ";\n"
       << "\n"
       << "\n";
  }

  void genImplPrologue2(ostream &oc,const GenConfig &config)
  {
    oc << "namespace " << config._nameSpace << " {\n"
       << "\n"
       << "const bool Tracing = " << (config._tracing ? "true" : "false") << ";\n\n";
  }

  void genImplEpilogue(ostream &oc,const GenConfig &config)
  {
    oc << "\n"
       << "} // namespace " << config._nameSpace << "\n";
  }

  gc_string get_ctor_parm(const gc_string &a)
  {
    return "p_" + a;
  }

  gc_string getIssType(const Thread *thread,const GenConfig &config)
  {
    return (!thread) ? getIssNamespace(config._nameSpace) : (getIssType(thread->parent(),config) + "::" + thread->item().name());
  }

  class GenCode : public gc {
  public:    
    GenCode(const UadlIr &ir, const Thread &thread, GenInstClassIF *genic,
            ostream &outH, ostream &outC,const GenConfig &config) :
      _ir(ir),
      _thread(thread),
      _res(thread.res()),
      _core(*_ir.thread().core()),
      _fetcher(0),
      _oh(outH),
      _oc(outC),
      _config(config),
      _realExtMemMode(config._extMemMode),
      _has_shared_stages(has_shared_stages(ir)),
      _iss_type(getIssType(&ir.thread(),config)),
      _genic(genic)
    {
      // issMemMode only if safeMode
      if (!_config._safeMode) 
        _config._issMemMode = false;

      // Effective extMemMode, only if !issMemMode, because we use an internal
      // pseudo memory for handling latency information.  _realExtMemMode stores
      // if external-memory was ever selected.
      _config._extMemMode = _config._extMemMode && !_config._issMemMode;

      _uname = getuArchClassName(thread);

      if (thread.core()) {
        _fetcher = _res.fetcher();
        assert(_fetcher);
        _fname = getFetchName(*_fetcher);
      }

      if (!_genic) {
        _genic = new GenInstClassIF(ir,_ir.thread(),outH,_of,outC,config,_iss_type);
      }
    };

  protected:
    const UadlIr    &_ir;
    const Thread    &_thread;
    const Resources &_res;
    const Core      &_core;           // For convenience, the ADL core.
    const FetchUnit *_fetcher;        // For convenience, the fetch unit.
    ostream         &_oh;
    ostream         &_oc;
    ostringstream    _of;             // For interface functor output.
    ostringstream    _os;             // For end-of-header output.
    GenConfig        _config;
    bool             _realExtMemMode; // If we really are using an external memory
                                      // or not.  May differ from the effective
                                      // flag based upon iss-mem-mode option.
    bool             _has_shared_stages;
    ClassData        _cd;

    gc_string        _uname;          // For convenience, the uarch class name.
    gc_string        _fname;          // For convenience, the name of the fetch unit.
    gc_string        _iss_type;       // Type name for the ISS class.
    FileMap          _icfiles;        // Instruction class -> filename mapping, if applicable.

    pair<unsigned,unsigned> _mask_info; // Information about mask data storage (max-count, max width).

    unsigned         _max_reg_id;     // Highest register ID in the system.

    adl::StrList     _run_stmts;      // Code to be added to run(), right before simulation starts.
    adl::StrList     _pre_stmts;      // Pre-cycle statements.
    adl::StrList     _post_stmts;     // Post-cycle statements.
    adl::StrList     _post2_stmts;    // Post-cycle statements, second phase.
    adl::StrList     _post_sim_stmts; // Post-simulation cleanup statements.
    adl::StrList     _child_args;     // Arguments to pass to thread constructors.

    GenInstClassIF   *_genic;

    struct StageLogItem {
      gc_string _name;
      unsigned  _index;

      StageLogItem(const gc_string &name,unsigned index) : _name(name), _index(index) {};

      bool operator<(const StageLogItem &x) const {
        if (_name != x._name) 
          return (_name < x._name);

        return _index < x._index;
      }
    };

    typedef gc_map<StageLogItem,unsigned> StageLoggerMap;
    StageLoggerMap   _stage_loggers;

  public:

    bool have_threads() const
    {
      return _ir.top().is_group();
    }

    bool leaf_node() const
    {
      return !_thread.is_group();
    }

    bool leaf_node(const Thread &thread) const
    {
      return !thread.is_group();
    }

    bool top_node() const
    {
      return !_thread.parent();
    }

    bool top_node(const Thread &thread) const
    {
      return !thread.parent();
    }

    bool cachesHaveData() const
    {
      return !_config._safeMode && !_config._tagsOnlyCaches;
    }

    void generate()
    {
      ClassData cd;

      // output prologue
      genPrologue();
      _oh << "\n\n";
      _oc << "\n\n";

      genISS(_thread);

      genImplPrologue2(_oc,_config);

      setupIcFiles();
      _genic->generate(_icfiles);

      // Main architecture class.
      genArch();

      genInterface();

      genEpilogue();

      cleanupIcFiles();
    }

    bool haveExceptions() const
    {
      return !_core.exceptions().empty();
    }

    bool is_shared(const gc_string &name)
    {
      return (_thread.parent()) ? (bool)_thread.parent()->res().findResourceByName(name) : false;
    }

    bool is_shared(const Resource &r)
    {
      return is_shared(r.name());
    }
    
    pair<gc_string,gc_string> getuArchParent()
    {
      pair<gc_string,gc_string> p;
      if (_config._systemc && top_node()) {
        p.second = "UadlArchSystemC";
        p.first  = "public sc_module, public " + p.second;
      } else {
        p.second = "UadlArch";
        p.first  = "public " + p.second;
      }
      return p;
    }

    gc_string getMemoryType(const gc_string &name) 
    {
      return (_config._extMemMode) ? "MemoryDelegate" : (name + "_mem_t");
    }

    gc_string issCacheName(const gc_string &name)
    { 
      return name + (_config._issMemMode ? "_perf" : "") + "_cache"; 
    }

    // Given a thread, return a string path to the first leaf node.
    gc_string getPathToLeaf(const Thread &thread)
    {
      if (leaf_node(thread)) {
        return gc_string();
      } else {
        const Thread &first_child = *thread.threads().begin()->second;
        gc_string n = first_child.name() + "->" + getPathToLeaf(first_child);
        return n;
      }
    }

    bool fetch_bp() const
    {
      return _fetcher && !_fetcher->branchPredictors().empty();
    }

    typedef gc_map<gc_string,StrSet> IcInstrs;
    typedef gc_list<IcInstrs::iterator> IcInstrIters;

    IcInstrIters findIntersecting(IcInstrs &icInstrs)
    {
      auto iter = icInstrs.begin();
      auto next = iter;
      ++next;

      IcInstrIters matching;
      matching.push_back(iter);

      for ( ; next != icInstrs.end(); ++next) {
        StrSet tmp;
        set_intersection(iter->second.begin(),iter->second.end(),
                         next->second.begin(),next->second.end(),
                         inserter(tmp,tmp.begin()));
        if (!tmp.empty()) {
          matching.push_back(next);
        }
      }
      return matching;
    }

    // If we're putting instruction classes into separate files, then setup the file mapping here.
    void setupIcFiles()
    {
      if (_config._sep_ics) {
        // First, construct a series of sets.  Each instruction class maps to
        // its set of instructions.  Then, we group instruction classes together
        // based upon whether any instruction classes intersect, in terms of
        // their instructions.  If they do, they have to be in the same file so
        // that the factory function can see the instruction class.
        IcInstrs icInstrs;

        ForEach(_ir.instClasses(),i) {
          const InstClass &ic = *(i->second);
          auto ip = icInstrs.insert(make_pair(ic.name(),StrSet()));
          StrSet &iset = ip.first->second;
          ForEach(ic.instrs(),j) {
            iset.insert(j->_name);
          }
        }

        int count = 0;
        gc_ostringstream ofss;

        // For each class, find all others which intersect, and group those.
        // Then remove those items and start over until we have no classes left.
        FileItem *f = 0;
        while ( !icInstrs.empty() ) {
          auto ics = findIntersecting(icInstrs);

          ofss << _config._basename << "-" << count++ << _config._fn_suffix << ".cc";

          f = new FileItem;
          f->_of = new ofstream(ofss.str().c_str());
          if (!*f->_of) RError("Error: Cannot write to file " << ofss.str() << "\n");
          f->_o = new opfxstream(f->_of->rdbuf(),2,'{','}');

          // Write the name of the file so that uadl2model will know about it.
          cout << "Instruction Class File:   " << ofss.str() << '\n';
            
          genImplPrologue1(*f->_o,_ir,_config,false);
          genImplPrologue2(*f->_o,_config);
            
          genNoMove(*f->_o);
            
          ofss.str("");

          ForEach(ics,iter) {
            const gc_string &icname = (*iter)->first;
            _icfiles[icname] = f;
          }
            
          ForEach(ics,i) {
            icInstrs.erase(*i);
          }
        }
      } else {
        genNoMove(_oc);
      }
    }

    void cleanupIcFiles()
    {
      ForEach(_icfiles,i) {
        FileItem &f = *i->second;
        if (!f._finished) {
          genImplEpilogue(*f._o,_config);
          f._finished = true;
        }
      }
      _icfiles.destroy();
    }

    void genNoMove(ostream &oc)
    {
      gc_string cname = getInstBaseName(_ir);
      oc << "static bool " << getNullActionFunc() << "(" << cname << " *," << _uname << " &,Logger *,unsigned) ATTRIBUTE_UNUSED;\n"
         << "static bool " << getNullActionFunc() << "(" << cname << " *," << _uname << " &,Logger *,unsigned) {\n"
         << "return true;\n"
         << "}\n\n";
    }

    // The base object for each instruction class.  Each object has an inquire and an action
    // function pointer.  Lock step stages will use both, whereas non-lock-step will just use
    // the action functions, which both inquire and perform the action.
    // NOTE:  Only this class may have members which have data elements.  All derived instruction
    // classes *must* *not* take any additional space, as we allocate all instruction classes from
    // a common pool in order to reduce space.
    void genInstClassBase()
    {
      gc_string cname = getInstBaseName(_ir);
      gc_string aname = getInstAllocName(_ir);

      _mask_info = _genic->get_mask_info();
     
      _oh << "struct InstrPacket;\n"
          << "struct " << _uname << ";\n"
          << "struct " << getDummyOpName() << ";\n"
          << "struct " << aname << ";\n\n"
          << "struct " << cname << " : public InstrBase {\n"
          << cname << "() : _move(0), _action(0), _flush(0),";
      if (_config._safeMode) {
        _oh << "_target_addr(0),";
      }
      if (!_res.branchPredictors().empty()) {
        _oh << "_pred_result(PredNone),\n";
      }
      if (have_threads()) {
        _oh << " _uarch(0),";
      }
      _oh << " _instr(0)";
      if (_config._ic_enabled) {
        _oh << ", _next(0), _ra(0)";
      }
      if (_has_shared_stages) {
        _oh << ", _stage(0)";
      }
      _oh << "\n{}\n\n"
          << "typedef bool (*StateFunc)(" << cname << " *," << _uname << " &,Logger *,unsigned);\n"
          << "typedef void (*FlushFunc)(" << cname << " *," << _uname << " &,Logger *);\n"
          << "\n"
          << "addr_t addr() const;\n\n";
      _oc << "addr_t " << cname << "::addr() const\n"
          << "{\n"
          << "return _instr->_addr;\n"
          << "}\n\n";

      _oh << "unsigned size() const;\n\n";
      _oc << "unsigned " << cname << "::size() const\n"
          << "{\n"
          << "return _instr ? _instr->_size : 0;\n"
          << "}\n\n";

      _oh << "unsigned instr_id() const;\n\n";
      _oc << "unsigned " << cname << "::instr_id() const\n"
          << "{\n"
          << "return _instr->_id;\n"
          << "}\n\n";

      if (have_threads()) {
        _oh << "int thread_id() const;\n\n";
        _oc << "int " << cname << "::thread_id() const\n"
            << "{\n"
            << "return _uarch->getChildId();\n"
            << "}\n\n";
      } else {
        _oh << "int thread_id() const { return -1; }\n\n";
      }

      _oh << "ReqInfo rinfo() const { return ReqInfo(iid(),thread_id()); };\n\n";
        
      if (InstClass::get_ic_attrs().empty()) {
        _oh << "bool has_attr(unsigned a) const { return false; }\n\n";
      } else {
        _oh << "bool has_attr(unsigned a) const;\n\n";
        _os << "inline bool " << cname << "::has_attr(unsigned a) const\n"
            << "{\n"
            << "return " << _uname << "::instrClassHasAttr(instr_class(),a);\n"
            << "}\n\n";
      }

      _oh << "bool is_fetch_exception() const { return !_instr; }\n\n"

          << "bool has_exceptions() const;\n\n";
      _oc << "bool " << cname << "::has_exceptions() const\n"
          << "{\n"
          << "return _instr && _instr->has_exceptions();\n"
          << "};\n\n";

      _oh << "bool move(" << _uname << "&uarch,Logger *logger,unsigned position) {\n"
          << "return (_move)(this,uarch,logger,position);\n"
          << "}\n"
          << "\n"
          << "bool action(" << _uname << "&uarch,Logger *logger,unsigned position) {\n"
          << "return (_action)(this,uarch,logger,position);\n"
          << "}\n"
          << "void flush(" << _uname << "&uarch,Logger *logger) {\n"
          << "(_flush)(this,uarch,logger);\n"
          << "}\n"
          << "\n"
          << "const adl::OpInfo &op(unsigned index) const;\n"
          << "const uint32_t *opc() const;\n";

      _oh << "std::pair<adl::BranchType,bool> branch_info(" << _uname << "&uarch) const;\n\n";
      _oc << "std::pair<adl::BranchType,bool> " << cname << "::branch_info(" << _uname << "&uarch) const\n"
          << "{\n"
          << "return _instr->branch_info(*uarch._iss);\n"
          << "}\n\n";

      _oh << "bool is_branch(" << _uname << "&uarch) const;\n\n";
      _oc << "bool " << cname << "::is_branch(" << _uname << "&uarch) const\n"
          << "{\n"
          << "const pair<adl::BranchType, bool> x = _instr->branch_info(*uarch._iss);\n"
          << "\n"
          << "return x.first;\n"
          << "}\n\n";

      _oh << "bool branch_taken(" << _uname << "&uarch) const;\n\n";
      _oc << "bool " << cname << "::branch_taken(" << _uname << "&uarch) const\n"
          << "{\n"
          << "const pair<adl::BranchType, bool> x = _instr->branch_info(*uarch._iss);\n"
          << "\n"
          << "return (x.first && x.second);\n"
          << "}\n\n";

      _oh << "addr_t branch_next_addr(" << _uname << "&uarch) const;\n\n";
      _oc << "addr_t " << cname << "::branch_next_addr(" << _uname << "&uarch) const\n"
          << "{\n"
          << "if (branch_taken(uarch)) {\n"
          << "return branch_target();\n"
          << "} else {\n"
          << "return addr()+size();\n"
          << "}\n"
          << "}\n\n";

      if (_config._safeMode) {
        _oh << "addr_t branch_target() const { return _target_addr; };\n";
      } else {
        _oh << "addr_t branch_target() const;\n\n";      
        _oc << "addr_t " << cname << "::branch_target() const\n"
            << "{\n"
            << "return _instr->branch_target();\n"
            << "}\n\n";
      }

      // When we set the branch target, we update the state of the prediction
      // result.  In the branch-hook, we call set_branch_finish, which updates
      // the prediction result if the result wasn't already set, in order to
      // handle the not-taken case.
      if (!_res.branchPredictors().empty()) {
        if (_config._safeMode) {
          // For handling branch prediction in safe-mode models.
          _oh << "void set_branch_target(addr_t bt) {\n"
              << "if (_pred_result == PredTaken && bt == _target_addr) {\n"
              << "_pred_result = PredCorrect;\n"
              << "} else {\n"
              << "_pred_result = PredIncorrect;\n"
              << "}\n"
              << "_target_addr = bt;\n"
              << "};\n"

              << "void set_branch_finish() {\n"
              << "switch (_pred_result) {\n"
              << "case PredCorrect:\n"
              << "case PredIncorrect:\n"
              << "break;\n"
            // If no predition set at all, then we'll mark it as incorrect, so
            // that we'll get a flush, but this should never happen.
              << "case PredNone:\n"
              << "case PredTaken:\n"
              << "_pred_result = PredIncorrect;\n"
              << "break;\n"
              << "case PredNotTaken:\n"
              << "_pred_result = PredCorrect;\n"
              << "break;\n"
              << "}\n"
              << "}\n";
        } else if (!_res.branchPredictors().empty()) {
          // Branch prediction for normal-mode models.
          _oh << "void set_branch_finish(" << _uname << "&uarch) {\n"
              << "((UadlArch&)uarch).incrBranchCount();\n"
              << "bool bt = branch_taken(uarch);\n"
              << "bool pt = (_pred_result == PredTaken);\n"
              << "if (bt == pt) {\n"
              << "if (!bt || branch_target() == _target_addr) {\n"
              << "_pred_result =  PredCorrect;\n"
              << "((UadlArch &)uarch).incrCorrectPredCount();\n"
              << "} else {\n"
              << "_pred_result = PredIncorrect;\n"
              << "}\n"
              << "} else {\n"
              << "_pred_result = PredIncorrect;\n"
              << "}\n"
              << "}\n\n";
        }

        // Either way, we store prediction data at issue time.
        _oh << "void set_prediction_data(bool taken,addr_t trg) {\n"
            << "_pred_result = (taken) ? PredTaken : PredNotTaken;\n"
            << "_target_addr = trg;\n"
            << "}\n\n"

            << "bool pred_correct() const { return _pred_result == PredCorrect; };\n";
      } else if (_config._safeMode) {
        _oh << "void set_branch_target(addr_t bt) { _target_addr = bt; };\n";
      }

      if (_has_shared_stages) {
        _oh << "bool is_in_stage(const PipelineStageBase *s) const { return _stage == s; };\n\n"
            << "bool is_alive() const { return _stage; };\n\n"
            << "void set_dead() { _stage = 0; };\n\n";
      }

      _oh << "\n";

      if (_mask_info.first != 0) {
        // We have masks, so we have to have a pointer to the mask storage.
        _oh << getMaskType(_mask_info.second) << " _masks[" << _mask_info.first << "];\n";
      }

      _oh << "StateFunc    _move;\n"
          << "StateFunc    _action;\n"
          << "FlushFunc    _flush;\n\n";

      // In safe-mode, we store branch targets, since the model is updated
      // immediately.
      if (_config._safeMode || !_res.branchPredictors().empty()) {
        _oh << "addr_t      _target_addr;\n";
      }
      if (!_res.branchPredictors().empty()) {
        _oh << "PredResult  _pred_result;\n";
      }

      // If we have threads, then we have to store the uarch object in each
      // instruction, so we know what thread we're dealing with.
      if (have_threads()) {
        _oh << _uname << " *_uarch;\n\n";
      }

      // If not using the instruction cache, then we never have an instruction
      // packet when the instruction object is deallocated.  With a cache, we
      // do, since the whole point is to cache this stuff.
      if (!_config._ic_enabled) {
        _oh << "union {\n";
      }
      _oh << "InstrPacket *_instr;\n"
          << aname << " *_next;\n";
      if (_config._ic_enabled) {      
        _oh << "addr_t _ra;\n";
      } else {
        _oh << "};\n";
      }
      // If the design has shared stages then we must track the current stage as
      // the instruction moves through the design.
      if (_has_shared_stages) {
        _oh << "PipelineStageBase *_stage;\n";
      }

      _oh << "};\n\n";

      gc_string pfx = (!_config._sep_ics) ? "inline " : "";
      
      _oc << pfx << "const adl::OpInfo &" << cname << "::op(unsigned index) const { return _instr->op(index); };\n\n"

          << pfx << "const uint32_t *" << cname << "::opc() const { return _instr->opc(); };\n\n";

      // If we're using virtual dispatch then we need to add extra storage for
      // references to the operands.  This class is used by the allocator so
      // that we make sure that we have enough space to store the largest
      // object.
      _oh << "class " << aname << " : public " << cname << "{\n";
      if (_config._vd_enabled) {
        size_t maxOps = 0;
        ForEach(_ir.instClasses(),i) {
          maxOps = max(maxOps,i->second->operands().size() + i->second->funcs().size());
        }
        _oh << "void *_space[" << maxOps << "];\n";
      }
      _oh << "};\n\n";

    }

    void genArch()
    {
      const gc_string IssClass = getIssClassName(_thread);
      const gc_string cname = getInstBaseName(_ir);

      // class header
      auto pinfo = getuArchParent();

      _oh << "class "  << _uname << ";\n\n"
          << "struct " << cname << ";\n\n";

      // Create various memory wrapper objects here.  Do this before any threads
      // b/c they might use the shared resources.
      genMemoryObjs();
      genCacheObjs();

      // The base instruction class object.  Only generate for the thread
      // leaf-node.
      if (leaf_node()) {
        genInstClassBase();
      }

      // Write any nested architecture objects.  Since all threads are
      // symmetrical, we only need to create one of them at a given level.  We
      // then instantiate the specified number of actual objects.
      if (!leaf_node()) {
        GenCode gc(_ir,*_thread.threads().begin()->second,_genic,_oh,_oc,_config);
        gc.genArch();
        _os << gc._os.str();
      }
 
      // BEGIN: header file
      _oh << "class " << _uname << " : " << pinfo.first << " {\n"
          << "public:\n\n"
          << "typedef " << cname << " InstrType;\n\n"
          << _uname << " &thisCore() { return *this; };\n\n";
 
      genIcAttrs();
      genResources();
      genHelpers();

      // Insert any functor declarations.
      _oh << _of.str();

      if (leaf_node()) {
        genInstFactories();
        genIssInterface();
        genExternExceptions();
        genIssue();
        genInstPool();
        genFetchServices();
      }
      
      genStall();
      genIssueInstructions();
      genUpdateTracing();

      if (top_node()) {
        genBreakDeadlock();
      }
      genCheckForDeadlock();

      // We need a reference to the top-most element of the design.  For
      // consistency, we always add this reference in.
      gc_string top_uname = getuArchClassName(_ir.top());
      _cd._member_vars.push_back(top_uname + " &top");

      if (top_node()) {
        _cd._constr_inits.push_back("top(*this)");

        _cd._member_vars.push_front(IssClass + " *_iss");
        _cd._constr_inits.push_front("_iss(new " + IssClass + "(*this,id))");
      } else {
        _cd._constr_args.push_back(top_uname + " &top_p");
        _cd._constr_inits.push_back("top(top_p)");

        _cd._constr_args.push_back(IssClass + " *iss");
        _cd._member_vars.push_front(IssClass + " *_iss");
        _cd._constr_inits.push_front("_iss(iss)");
      }
      if (top_node()) {
        _child_args.push_back("*this");
      } else if (!leaf_node()) {
        _child_args.push_back("top_p");
      }

      if (leaf_node()) {
        _cd._constr_stmts.push_back("_iss->_uarch = this");
      }

      // Write any nested architecture objects.  Since all threads are
      // symmetrical, we only need to create one of them at a given level.  We
      // then instantiate the specified number of actual objects.
      if (!leaf_node()) {

        if (_config._extMemMode) {
          _child_args.push_front("memory");
        }
        _child_args.push_front("startTime");
        _child_args.push_front("out");
        _child_args.push_front("thread_id");
        _child_args.push_front("id");

        // Child-class initialization.
        gc_string ca;
        bool first = true;
        ForEach(_child_args,i) {
          list_sep(ca,first);
          ca += *i;
        }

        // Add on the instances of this type.
        ForEach(_thread.threads(),i) {
          _cd._member_vars.push_back(getuArchClassName(*i->second) + " " + i->first);
          _cd._reset_stmts.push_back(i->first+".reset()");
          gc_string child_iss_class = getIssClassName(*i->second);
          _cd._constr_inits.push_back(i->first+"("+ca+",static_cast<"+child_iss_class+"*>(_iss->"+i->first+"))");
        }
      }

      _oh << "\n"
          << "// access/query methods\n"
          << "virtual adl::IssNode& iss();\n"
          << "virtual const adl::IssNode& iss() const;\n"
          << IssClass << "& get_iss();\n";

      _oc << "adl::IssNode& " << _uname << "::iss() { return *_iss; }\n"
          << "const adl::IssNode& " << _uname << "::iss() const { return *_iss; }\n"
          << IssClass << "& " << _uname << "::get_iss() { return *_iss; }\n\n";

      if (leaf_node()) {
        _oh << "unsigned getChildId() const { return _thread_id; }\n\n";

        if (have_threads()) {
          _oh << "int thread_id() const { return _thread_id; }\n\n";
        } else {
          _oh << "int thread_id() const { return -1; }\n\n";
        }
      }
      
      genScheduler();
      genArchSupport(pinfo.second);
      genStallGetters();

      _oh << "};\n\n";
    }
 
    void liveCheckBegin()
    {
      if (_has_shared_stages) {
        _oc << "if (instr->is_alive()) {\n";
      }
    }

    void liveCheckEnd()
    {
      if (_has_shared_stages) {
        _oc << "instr->set_dead();\n"
            << "}\n";
      }
    }

    unsigned getInstrShift()
    {
      unsigned min_instr_width = numeric_limits<int>::max();
      unsigned min_bytes = 0;

      const InstrHash &instrs = _core.instrs();

      // Collect statistics about instructions.
      ForEach(instrs,i) {
        const Instr     &instr = *(i->second);
        if (!instr.is_alias()) {
          min_instr_width = min(min_instr_width,instr.width());
        }
      }

      min_bytes = (min_instr_width/8);

      return (is_power_2(min_bytes)) ? log2(min_bytes) : 0;
    }

    // Generate logic for allocating instruction class objects.  We use a
    // pool-based allocator for performance.
    void genInstPool()
    {
      // We need enough items in the pool to satisfy the size requirements of
      // all stages in the core.  We tack on some extra for handling fetch.
      int count = 0;
      ForEach(_res.pipelineStages(),i) {
        count += i->second->size();
      }
      count += 2; // Hack- add some extra for fetch.

      gc_string cname = getInstBaseName(_ir);
      gc_string aname = getInstAllocName(_ir);

      if (_config._ic_enabled) {
        // We want an instruction cache, so create it here.
        unsigned instr_shift = getInstrShift();
        assert(is_power_2(_config._ic_page_size));
        unsigned cache_shift = log2(_config._ic_page_size) + instr_shift;

        gc_ostringstream ss;
        ss << "InstructionCache<" << aname << "," << _config._ic_size << "," 
           << cache_shift << "," << _config._ic_page_size << "," 
           << instr_shift << "> _instr_cache";
        _cd._member_vars.push_back(ss.str());
        _cd._reset_stmts.push_back("_instr_cache.reset()");        

        _cd._member_vars.push_back("unsigned _instr_alloc_count");
        _cd._constr_inits.push_back("_instr_alloc_count(0)");
        _cd._reset_stmts.push_back("_instr_alloc_count = 0");

        // Only insert real instructions into the cache, so that we don't have
        // to check for the instruction packet's existence later.
        _oh << "unsigned instr_alloc_count() const { return _instr_alloc_count; }\n\n"
            << "void free_instr(" << cname << " *instr) {\n";
        // If we're tracking deadlock, then on an instruction commit, clear the
        // counter, since committing instructions implies forward progress.
        if (_config._deadlock) {
          _oh << "_deadlockCount = 0;\n";
        }
        if (_config._safeMode) {
          // In safe mode, clear the last-instruction pointer if it's us, so that
          // we don't reference a freed instruction.
          _oh << "if (instr == _last_instr) _last_instr = 0;\n";
        }
        _oh << "if (instr->_instr) {\n"
            << "_instr_cache.insert(instr->_ra,(" << aname <<"*)instr);\n"
            << "} else {\n"
            << "delete instr;\n"
            << "}\n"
            << "--_instr_alloc_count;\n"
            << "}\n\n";
      } else {
        // If no instruction cache, then just create a pool-based allocator for
        // the packets.
        gc_ostringstream ss;
        ss << "adl::PacketPoolCounted<" << aname << "," << count << "> _instr_pool";
        _cd._member_vars.push_back(ss.str());
        _cd._reset_stmts.push_back("_instr_pool.reset()");

        _oh << "unsigned instr_alloc_count() const { return _instr_pool.num_alloced(); }\n\n"
            << "void free_instr(" << cname << " *instr) {\n";
        // If we're tracking deadlock, then on an instruction commit, clear the
        // counter, since committing instructions implies forward progress.
        if (_config._deadlock) {
          _oh << "_deadlockCount = 0;\n";
        }
        if (_config._safeMode) {
          // In safe mode, clear the last-instruction pointer if it's us, so that
          // we don't reference a freed instruction.
          _oh << "if (instr == _last_instr) _last_instr = 0;\n";
        }
        _oh << "_instr_pool.free((" << aname << "*)instr);\n"
            << "}\n\n";
      }

      _oh << "void commit(" << cname << "*instr);\n";
      _oc << "void " << _uname << "::commit(" << cname << "*instr)\n"
          << "{\n";
      liveCheckBegin();
      if (!_config._safeMode) {
        _oc << "addr_t ia = instr->addr();\n";
      }
      _oc << "_iss->free_packet(instr->_instr);\n"
          << "free_instr(instr);\n";
      if (!_config._safeMode) {
        // Note: For safe-mode, we increment the commit count at execution time,
        // so that the cid shown in the trace will be correct.  For
        // non-safe-mode, we can't really know the cid, since we don't know what
        // the pipeline behavior will be, so we just increment at the actual
        // commit.
        _oc << "incrCommitCount();\n"
            << "check_breakpoints(ia);\n";
      }
      liveCheckEnd();
      _oc << "}\n\n";

      _oh << "void commit_checked(" << cname << "*instr);\n";
      _oc << "void " << _uname << "::commit_checked(" << cname << "*instr)\n"
          << "{\n";
      liveCheckBegin();
      if (!_config._safeMode) {
        _oc << "bool check_bp = false;\n"
            << "addr_t ia;\n";
      }
      _oc << "if (instr->is_fetch_exception()) {\n"
          << "_iss->log_commit(instr->_fedata);\n"
          << "instr->discard_fetch_exception();\n"
          << "} else {\n";
      if (!_config._safeMode) {
        _oc << "check_bp = true;\n"
            << "ia = instr->addr();\n";
      }
      _oc << "_iss->free_packet(instr->_instr);\n"
          << "}\n"
          << "free_instr(instr);\n";
      if (!_config._safeMode) {
        _oc << "incrCommitCount();\n"
            << "if (check_bp) {\n"
            << "check_breakpoints(ia);\n"
            << "}\n";
      }
      liveCheckEnd();
      _oc << "}\n\n";

      _oh << "void discard(" << cname << "*instr);\n";
      _oc << "void " << _uname << "::discard(" << cname << "*instr)\n"
          << "{\n";
      liveCheckBegin();
      _oc << "cancel_all_mem(*instr);\n"
          << "free_all_mem(*instr);\n"
          << "_iss->discard_packet(instr->_instr);\n"
          << "free_instr(instr);\n";
      liveCheckEnd();
      _oc << "}\n\n";
    }

    void genPreCycle()
    {
      _oh << "void preCycle();\n";

      _oc << "void " << _uname << "::preCycle()\n"
          << "{\n";
        
      // If a pre-cycle hook exists in the ISS, call it.
      if (_core.pre_cycle_handler()) {
        _oc << "_iss->pre_cycle_handler();\n";
      }

      // Write any pre-cycle statements needed by other stuff.
      ForEach(_pre_stmts,i) {
        _oc << *i << ";\n";
      }

      // Child-pre-cycle statements.
      ForEach(_thread.threads(),i) {
        _oc << i->first << ".preCycle();\n";
      }

      // Fetcher, if we have one.
      if (_fetcher) {
        _oc << _fname << ".receiveInstrData();\n"
            << "if (logPipeline()) {\n"
            << _fname << ".instrBufReport();\n"
            << "}\n";
      }
      _oc << "}\n\n";
    }

    void genPostCycle()
    {
      _oh << "void postCycle();\n";

      _oc << "void " << _uname << "::postCycle()\n"
          << "{\n";

      if (_fetcher) {
        _oc << "if (_iss->activeCount()) " << _fname << ".sendFetchRequests();\n";
      }

      // Write any post-cycle statements needed by other stuff.
      ForEach(_post_stmts,i) {
        _oc << *i << ";\n";
      }

      // Child-post-cycle statements.
      ForEach(_thread.threads(),i) {
        _oc << i->first << ".postCycle();\n";
      }

      if (_core.post_cycle_handler()) {
        _oc << "_iss->post_cycle_handler();\n";
      }

      // Write any post-cycle statements needed by other stuff.
      ForEach(_post2_stmts,i) {
        _oc << *i << ";\n";
      }

      if (top_node()) {
        _oc << "if (tracing_on() && _out) {\n"
            << "report_and_clear(*_out,string());\n"
            << "}\n";
      }

      _oc << "incrTime();\n"
          << "}\n\n";
    }

    void genCanMove()
    {
      _oh << "bool can_move() const { return !_stall; }\n\n";
    }

    // Helper functions for branch prediction stuff.
    void genPredictorHelpers()
    {
      gc_string cname = getInstBaseName(_ir);

      // Helper function for branch prediction.  This simply now queries the
      // instruction's prediction data, rather than actually talking to the
      // predictor.
      gc_string lp_arg;
      if (_res.branchPredictors().size() > 1) {
        _oh << "template <class PredictorType>\n"
            << "bool last_branch_prediction_correct(" << cname << " &instr,PredictorType &bp)\n";
      } else {
        _oh << "bool last_branch_prediction_correct(" << cname << " &instr)\n";
      }
      _oh << "{\n";
      if (!_res.branchPredictors().empty()) {
        _oh << "return instr.pred_correct();\n";
      } else {
        _oh << "return false;\n";
      }
      _oh << "}\n\n";
    }

    // Generate the iss interaction interfaced (exec, commit, etc.)
    void genIssInterface()
    {
      const PipeVect &stages = _ir.sortedStages();
      gc_string cname = getInstBaseName(_ir);

      // Refers to the currently executing instruction.
      _cd._member_vars.push_back(cname + " *_cur_instr");
      _cd._constr_inits.push_back("_cur_instr(0)");
      _cd._reset_stmts.push_back("_cur_instr = 0");

      if (_config._safeMode) {
        _oh << "void decode_update(bool succeed,unsigned size) {\n"
            << _fname << ".decodeUpdate(succeed,size);\n"
            << "}\n\n";
      }
      
      _oh << "void set_cur_instr(" << cname << " *x) {\n"
          << "_cur_instr = x;\n";
      if (_config._safeMode && !_res.branchPredictors().empty()) {
        // For safe mode models, we update branch prediction here, right
        // before we execute the handler, so that we make decisions based upon
        // updated predictor data.
        _oh << "x->set_prediction_data(" << _fname << ".predTaken()," << _fname << ".predTrg());\n";
      }
      _oh << "};\n";

      // exec() routine.  In addition to the generic routine, we also include
      // stage-specific routines which flush the pipeline on a halt in
      // non-safe-mode.
      if (_config._safeMode) {
        // Next instruction in pipe.  Used to create a linked list of
        // instructions so that branches can easily determine the next
        // instruction behind themselves, in order to make sure that they're
        // correctly resolved.
        _cd._member_vars.push_back(cname + " *_last_instr");
        _cd._constr_inits.push_back("_last_instr(0)");
        _cd._reset_stmts.push_back("_last_instr = 0");

        // When a taken-branch occurs, we go into speculative mode.  We stop
        // executing instructions, but continue fetching as if no change in
        // control occurred.  When a flush occurs, we then start fetching from
        // the target-address.  The flush flag exists so that, if we clear the
        // speculative flag, we'll still be able to signal that a flush should
        // occur.
        _cd._member_vars.push_back("bool _speculative");
        _cd._constr_inits.push_back("_speculative(false)");
        _cd._reset_stmts.push_back("_speculative = false");

        // This stores the branch target, in case we need to restore the PC of
        // the ISS externally.
        _cd._member_vars.push_back("addr_t _branch_target");
        _cd._constr_inits.push_back("_branch_target(0)");
        _cd._reset_stmts.push_back("_branch_target = 0;");

        _cd._member_vars.push_back("unsigned _spec_tag");
        _cd._constr_inits.push_back("_spec_tag(0)");
        _cd._reset_stmts.push_back("_spec_tag = 0");

        _oh << "void set_branch_target(addr_t addr);\n\n";
        _oc << "void " << _uname << "::set_branch_target(addr_t addr) {\n"
            << "_branch_target = addr;\n"
            << "_cur_instr->set_branch_target(addr);\n"
            << "}\n\n";

        _oh << "void signalException(" << cname << " *instr);\n";
        _oc << "void " << _uname << "::signalException(" << cname << " *instr) {\n"
            << "if (" << _fname << ".logger()) " << _fname << ".logger()->logMessage(\"commiting exception\");\n"
            << "if (_speculative || !instr) return;\n"
            << "instr->set_branch_target(get_pc());\n"
            << "_speculative = true;\n"
            << "_spec_tag = instr->iid();\n"
            << "}\n\n";

        // True if the branch was predicted correctly, so that the next
        // instruction to issue is correct.
        _oh << "addr_t nextInstrIssueAddr() const {\n"
            << "return " << _fname << ".nextIssueAddr();\n"
            << "}\n\n"

        // In safe-mode, when checking speculative, we make sure it's not the
        // current instruction that's doing the checking.
            << "bool is_executing(const InstrBase &instr) const {\n"
            << "return (!_speculative || (_spec_tag == instr.iid()));\n"
            << "}\n\n"

            << "bool is_current_speculative_instr(const InstrBase &instr) const{\n"
            << "return (_speculative && (_spec_tag == instr.iid()));\n"
            << "}\n\n"

        // For safe-mode, if we're not speculative, then just set the
        // speculative flag accordingly.  Otherwise, only set it if our tag
        // matches.
            << "void set_speculative(const InstrBase &instr,bool s = true) {\n"
            << "if (!_speculative) {\n"
            << "_speculative = s;\n"
            << "_spec_tag = instr.iid();\n"
            << "} else if (_spec_tag == instr.iid()) {\n"
            << "_speculative = s;\n"
            << "}\n"
            << "}\n\n"

            << "void exec(" << cname << "&instr,Logger *logger) {\n"
            << "if (logger) {\n"
            << "logger->logMessage(\"exec\");\n"
            << "};\n"
            << "}\n"

            << "void exec_branch(" << cname << "&instr,Logger *logger) {\n"
            << "if (logger) {\n"
            << "logger->logMessage(\"exec\");\n"
            << "std::pair<adl::BranchType,bool> bi = instr.branch_info(*this);\n"
            << "if (bi.first) logger->logBranch(bi.second,instr.branch_target());\n"
            << "};\n"
            << "}\n"

            << "void exec_mem(" << cname << "&instr,Logger *logger) {\n"
            << "if (logger) { logger->logMessage(\"exec\"); };\n"
            << "}\n"

            << "void exec_mem_no_ma(" << cname << "&instr,Logger *logger) {\n"
            << "if (logger) { logger->logMessage(\"exec\"); };\n"
            << "}\n";

      } else {
        // Just here to avoid errors in a model.
        _oh << "void set_speculative(const InstrBase &,bool = true) {}\n\n"

            << "void exec(" << cname << "&,Logger *logger);\n";
        _oc << "void " << _uname << "::exec(" << cname << "&instr,Logger *logger)\n"
            << "{\n";
        _oc << "instr._instr->exec(*_iss);\n"
            << "if (logger) {\n"
            << "logger->logMessage(\"exec\");\n"
            << "std::pair<adl::BranchType,bool> bi = instr.branch_info(*this);\n"
            << "if (bi.first) logger->logBranch(bi.second,(bi.second) ? instr.branch_target() : 0);\n"
            << "}\n"
            << "}\n\n";

        // We have to finalize branches, so that prediction info is stored.
        _oh << "void exec_branch(" << cname << "&instr,Logger *logger) {\n"
            << "exec(instr,logger);\n";
        if (!_res.branchPredictors().empty()) {
          _oh << "instr.set_branch_finish(*this);\n";
        }
        _oh << "}\n";

        // Special exec for dealing with instructions which access memory.
        _oh << "void exec_mem(" << cname << "&,Logger *logger);\n";
        _oc << "void " << _uname << "::exec_mem(" << cname << "&instr,Logger *logger)\n"
            << "{\n"
            << "if (logger) { logger->logMessage(\"exec\"); };\n"
            << "set_cur_instr(&instr);\n"
            << "instr._status = instr._instr->exec(*_iss);\n"
            << "}\n\n";

        _cd._member_vars.push_back ("bool _ignore_mem");
        _cd._constr_inits.push_back("_ignore_mem(false)");
        _cd._reset_stmts.push_back("_ignore_mem = false");

        // Same as above, but shut off memory request collection.  Used when
        // coalescing multiple memory accesses.
        _oh << "void exec_mem_no_ma(" << cname << "&,Logger *logger);\n";
        _oc << "void " << _uname << "::exec_mem_no_ma(" << cname << "&instr,Logger *logger)\n"
            << "{\n"
            << "if (logger) { logger->logMessage(\"exec\"); };\n"
            << "struct IgnoreMem {\n"
            << "IgnoreMem(" << _uname << " &x) : _x(x) { _x._ignore_mem = true; };\n"
            << "~IgnoreMem() { _x._ignore_mem = false; };\n"
            << _uname << " &_x;\n"
            << "} ignore_mem(*this);\n\n"
            << "instr._status = instr._instr->exec(*_iss);\n"
            << "}\n\n";
      }

      genPredictorHelpers();

      // For setting/getting the program counter (next-instruction-address).
      _oh << "addr_t get_pc() const;\n";
      _oc << "addr_t " << _uname << "::get_pc() const {\n"
          << "return " << getRegRead(*_core.nia()) << ".uint64();\n"
          << "}\n\n";
      
      _oh << "void set_pc(addr_t a);\n";
      _oc << "void " << _uname << "::set_pc(addr_t a) {\n"
          << "_iss->" << _core.nia()->name() << "_write_simple(a);\n"
          << "}\n\n";
      
      // Breakpoint check.
      _oh << "void check_breakpoints(addr_t ea);\n";
      _oc << "void " << _uname << "::check_breakpoints(addr_t ea)\n"
          << "{\n"
          << "_iss->check_breakpoints(ea);\n"
          << "};\n\n";

      _oh << "void clear_exceptions(" << cname << " &instr);\n";
      _oc << "void " << _uname << "::clear_exceptions(" << cname << " &instr)\n"
          << "{\n"
          << "if (!instr.is_fetch_exception()) {\n"
          << "instr._instr->clear_exceptions();\n"
          << "}\n"
          << "}\n\n";

      // Called to execute the actions associated with an exception.
      _oh << "void commit_exceptions(" << cname << " &instr,Logger *logger);\n";
      _oc << "void " << _uname << "::commit_exceptions(" << cname << " &instr,Logger *logger)\n"
          << "{\n";
      if (_config._safeMode) {
        // In safe-mode, if we're speculative, then don't update anything,
        // e.g. we might have already executed an exception, or this might be on
        // the untaken side of a branch, etc.
        _oc << "if (_speculative) return;\n";
      }
      if (_ir.hasExceptions()) {
        _oc << "if (logger) logger->logMessage(\"commiting exception\");\n"
            << "if (instr.is_fetch_exception()) {\n"
            << "commit_fetch_exception(instr);\n"
            << "} else {\n";
        if (!_config._safeMode) {
          // Update the CIA and NIA as appropriate.
          _oc << "_iss->" << _core.cia()->name() << "_write(instr._instr->addr());\n";
          if (_core.cia() != _core.nia()) {
            _oc << "set_pc(instr._instr->addr() + instr._instr->size());\n";
          }
        }
        _oc << "instr._instr->commit_exceptions(*_iss);\n"
            << "}\n";
        if (_config._safeMode) {
          _oc << "instr.set_branch_target(get_pc());\n";
        }
      }
      _oc << "};\n\n";

      // Cancels all memory accesses related to a given instruction.
      _oh << "void cancel_all_mem(" << cname << " &instr);\n";
      _oc << "void " << _uname << "::cancel_all_mem(" << cname <<" &instr)\n"
          << "{\n";
      if (_ir.firstLevelDataMem()) {
        const gc_string dataMemory = getMemObj(*_ir.firstLevelDataMem());
        _oc << "UadlMemAccess *ma = instr.mem();\n"
            << "while (ma) {\n"
            << "if (ma->valid_request_id()) {\n"
            << dataMemory << ".cancel(ma->request_id(),true);\n"
            << "}\n"
            << "ma = ma->_next;\n"
            << "}\n";
      }
      _oc << "}\n\n";
      
      // For commiting fetch exceptions.
      _oh << "void commit_fetch_exception(" << cname << " &instr);\n";
      _oc << "void " << _uname << "::commit_fetch_exception(" << cname << "&instr)\n"
          << "{\n";
      if (_ir.hasExceptions()) {
        if (!_config._safeMode) {
          _oc << "FetchExceptionData &fedata = *instr._fedata;\n"
              << "_iss->" << _core.cia()->name() << "_write(fedata._addr);\n"
              << "_iss->log_current_instr(&fedata);\n"
              << "_iss->genExceptionsInternal(fedata._exdata,fedata._exitems);\n";
        }
        // In safe-mode, the behavior is taken care of by decode(), so we just
        // clear the fact that we're in a fetch exception.
        _oc << "_fetchExceptionInProgress = false;\n";
      }
      _oc << "}\n\n";

      // These are helpers which simplify reading and writing by wrapping the
      // exec call with the appropriate pre and post actions for reading or
      // writing.
      _oh << "template <class M>\n"
          << "void exec_and_read(" << cname << "&instr,Logger *logger,M &m) {\n"
          << "m.read(*this,instr,logger);\n"
          << "exec_mem(instr,logger);\n"
          << "m.post_read(*this,instr,logger);\n"
          << "}\n\n"

          << "template <class M,class I>\n"
          << "void exec_and_read(I& instr,Logger *logger,M &m,unsigned count) {\n"
          << "m.read(*this,instr,logger);\n"
          << "exec_mem_no_ma(instr,logger);\n"
          << "for (unsigned i = 1; i < count-1; ++i) {\n"
          << "if (!instr.exec_done() && m.next(*this,instr,logger)) {\n"
          << "exec_mem_no_ma(instr,logger);\n"
          << "} else {\n"
          << "break;\n"
          << "}\n"
          << "}\n"
          << "if (!instr.exec_done() && m.next(*this,instr,logger)) {\n"
          << "exec_mem(instr,logger);\n"
          << "}\n"
          << "m.post_read(*this,instr,logger);\n"
          << "}\n\n"

          << "template <class M>\n"
          << "void exec_and_write(" << cname << "&instr,Logger *logger,M &m) {\n"
          << "m.pre_write(*this,instr,logger);\n"
          << "exec_mem(instr,logger);\n"
          << "m.write(*this,instr,logger);\n"
          << "}\n\n"

          << "template <class M,class I>\n"
          << "void exec_and_write(I& instr,Logger *logger,M &m,unsigned count,unsigned final_size = 0) {\n"
          << "m.pre_write(*this,instr,logger);\n"
          << "exec_mem_no_ma(instr,logger);\n"
          << "for (unsigned i = 1; i < count-1; ++i) {\n"
          << "if (!instr.exec_done() && m.next(*this,instr,logger)) {\n"
          << "exec_mem_no_ma(instr,logger);\n"
          << "} else if (final_size > 0) {\n"
          << "m.set_size(*this,instr,logger,final_size);\n"
          << "break;\n"
          << "}\n"
          << "}\n"
          << "if (!instr.exec_done() && m.next(*this,instr,logger)) {\n"
          << "exec_mem(instr,logger);\n"
          << "} else if (final_size > 0) {\n"
          << "m.set_size(*this,instr,logger,final_size);\n"
          << "}\n"
          << "m.write(*this,instr,logger);\n"
          << "}\n\n";

      // Special stage-specific exec's which flush the pipeline on a halt.
      ForEach(_res.pipelineStages(),s_iter) {
        const PipelineStage &stage = *s_iter->second;
        const gc_string &sname = stage.name();
        _oh << "void exec_" << sname << "(" << cname << "&instr,Logger *logger,unsigned position);\n";
        _oc << "void " << _uname << "::exec_" << sname << "(" << cname << "&instr,Logger *logger,unsigned position)\n"
            << "{\n"
            << "exec(instr,logger);\n";
        if (!_config._safeMode) {
          // We only need to flush normal-mode models, since in safe-mode models
          // the halt() has occurred in issue() and already shut of the model.
          _oc << "if (_iss->activeCount() == 0) flush_" << sname << "(instr,logger,position);\n";
        }
        _oc << "}\n\n";

        _oh << "void exec_mem_" << sname << "(" << cname << "&instr,Logger *logger,unsigned position);\n";
        _oc << "void " << _uname << "::exec_mem_" << sname << "(" << cname << "&instr,Logger *logger,unsigned position)\n"
            << "{\n"
            << "exec_mem(instr,logger);\n";
        if (!_config._safeMode) {
          // We only need to flush normal-mode models, since in safe-mode models
          // the halt() has occurred in issue() and already shut of the model.
          _oc << "if (_iss->activeCount() == 0) flush_" << sname << "(instr,logger,position);\n";
        }
        _oc << "}\n\n";
      }    
     
      // flush() routine.  A flush routine is generated for each pipeline stage
      // so that we can hardcode in the actions, which is basically to retire
      // every instruction in the previous pipeline stages.  Common logic is then put into
      // flush_issue();
      // Not:  In safe-mode, we can't do arbitrary flushes because updates are immediately
      // committed.  Thus, by default flushes are ignored- we can only flush speculatively
      // issued instructions behind mispredicted branches.
      _oh << "void flush_issue_stage(Logger *logger,unsigned position,bool given_addr = false,addr_t addr = 0);\n";
      _oc << "void " << _uname << "::flush_issue_stage(Logger *logger,unsigned position,bool given_addr,addr_t addr)\n"
          << "{\n"
          << "if (!" << getIssueName() << ".empty()) {\n"
          << cname << "*instr = " << getIssueName() << ".instr();\n"
          << "if (!instr->is_fetch_exception()) " << _fname << ".removeInstr(instr->_instr->size());\n"
          << getIssueName() << ".remove_instr();\n"
          << "discard(instr);\n"
          << "}\n"
          << "flush_issue(logger,given_addr,addr);\n"
          << "}\n\n";

      // We'll use this as a fast way to look up a stage, to see if it's local
      // to this thread.
      gc_set<const PipelineStage *> local_stages;
      ForEach(_res.pipelineStages(),i) {
        local_stages.insert(i->second);
      }

      bool hit_issue = false;
      ForEach(stages,r_iter) {
        const PipelineStage &stage = **r_iter;
        // Only process stages known by this thread.
        if (!local_stages.count(&stage)) continue;
        const gc_string &sname = stage.name();
        if (stage.issue()) {
          hit_issue = true;
        }
        // For non-taken branches- we also update the NIA.
        _oh << "void flush_" << sname << "(" << cname << "&,Logger *logger,unsigned position);\n";
        _oc << "void " <<_uname << "::flush_" << sname << "(" << cname << "&ic,Logger *logger,unsigned position)\n"
            << "{\n";
        // Since the 'flush' function is meant to flush the current pipeline, it clears the speculative flag
        // so that the NIA won't be updated.
        if (_config._safeMode) {
          _oc << "_speculative = false;\n";
        }
        // Update the NIA to the next address after this instruction.
        _oc << "if (ic.iid() >= _resetFetchId) {\n";
        if (!_config._safeMode) {
          _oc << "set_pc(ic._instr->addr() + ic._instr->size());\n";
        }
        _oc << "}\n"
            << "taken_flush_" << sname << "(ic,logger,position);\n"
            << "}\n\n";

          // For taken branches- no NIA update in normal mode.  For safe-mode,
          // update to the target address.
        _oh << "void taken_flush_" << sname << "(" << cname << "&,Logger *logger,unsigned position,bool given_addr = false,addr_t addr = 0);\n";
        _oc << "void " <<_uname << "::taken_flush_" << sname << "(" << cname << "&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)\n"
            << "{\n";
        if (_ir.hasExceptions()) {
          _oc << "_fetchException = 0;\n";
        }
        _oc << "if (ic.iid() >= _resetFetchId) {\n"
            << "if (!given_addr && ic._instr) {\n";
        if (!_config._safeMode) {
          // Non-safe-mode: Get branch info and then decide on whether to use
          // the target address or not.
          _oc << "if (ic.branch_taken(*this)) {\n"
              << "set_pc(ic.branch_target());\n"
              << "} else {\n"
              << "set_pc(ic._instr->addr() + ic._instr->size());\n"
              << "}\n";
        }
        _oc << "}\n"
            << "}\n";
        if (_config._safeMode) {
          // Just to be safe, always clear these flags in safe-mode, since a
          // flush means that we're basically resetting everything.
          _oc << "_speculative = false;\n";
        }
        // Flush all instructions behind this one in the pipe, if relevant.
        // Check for a matching thread-id if we're in a shared stage and we have
        // a multi-threaded model.
        bool mtcheck = is_shared(stage);
        if (mtcheck) {
          _oc << "unsigned orig_size = " << sname << ".size();\n";
        }
        if (stage.size() > 1 && stage.scheduler() != sIndependent) {
          if (stage.scheduler() == sDependent) {
            // If it's a dependent stage, then we know that if its action code
            // is executing, it's at the front.  However, the position variable
            // represents it original position, so that's why we reset it here.
            _oc << "position = 0;\n";
          }
          _oc << "for (int i = position+1; i < " << stage.size() << "; ++i) {\n"
              << "if (" << cname << " *instr = " << sname << ".remove_instr(i" << (mtcheck ? ",getChildId()" : "") << ")) {\n";
          liveCheckBegin();
          _oc << "instr->flush(*this,logger);\n";
          if (hit_issue) {
            // Anything before the issue stage must be released from the fetch
            // unit.  For multi-issue, we need to do this in the current stage as
            // well.
            _oc << _fname << ".removeInstr(instr->size());\n";
          }
          _oc << "discard(instr);\n";
          liveCheckEnd();
          _oc << "}\n"
              << "}\n";
        }
        if (mtcheck) {
          // We have to put the stage in a consistent state, since some
          // instructions might be removed from the middle of the stage.
          _oc << sname << ".cleanup(orig_size);\n";
        } else {
          _oc << sname << ".update_tail();\n";
        }
        // Then flush all stages behind this one.
        bool hit_issue2 = false;
        for (auto s_iter = r_iter + 1; s_iter != stages.end(); ++s_iter) {
          const PipelineStage &stage2 = **s_iter;
          if (!local_stages.count(&stage2)) continue;
          const gc_string &sname2 = stage2.name();
          hit_issue2 = stage2.issue();
          bool mtcheck2 = is_shared(stage2);
          if (mtcheck2) {
            // For mt, only remove instructions which match the flushing instructin's thread id.
            _oc << "{\n"
                << "unsigned orig_size = " << sname2 << ".size();\n"
                << "for (unsigned i = 0; i != " << stage2.size() << "; ++i) {\n"
                << "if (" << cname << " *instr = " << sname2 << "[i]) {\n";
            liveCheckBegin();
            _oc << "if (" << sname2 << ".remove_instr(i,getChildId())) {\n"
                << "(instr->_flush)(instr,*this,logger);\n";
            if (hit_issue || hit_issue2) {
              // Anything before the issue stage must be released from the fetch unit.
              _oc << _fname << ".removeInstr(instr->size());\n";
            }
            _oc << "discard(instr);\n";
            liveCheckEnd();
            _oc << "}\n";
            if (_has_shared_stages) {
              _oc << "else {\n"
                  << sname2 << ".remove_instr(i);\n"
                  << "}\n";
            }
            _oc << "}\n"
                << "}\n"
                << sname2 << ".cleanup(orig_size);\n"
                << "}\n";
          } else {
            _oc << "for (unsigned i = 0; i != " << stage2.size() << "; ++i) {\n"
                << "if (" << cname << " *instr = " << sname2 << "[i]) {\n";
            liveCheckBegin();
            _oc << "(instr->_flush)(instr,*this,logger);\n";
            if (hit_issue || hit_issue2) {
              // Anything before the issue stage must be released from the fetch unit.
              _oc << _fname << ".removeInstr(instr->size());\n";
            }
            _oc << "discard(instr);\n";
            liveCheckEnd();
            _oc << "}\n"
                << "}\n"
                << sname2 << ".reset();\n";
          } 
        }
        _oc << "flush_issue_stage(logger,position,given_addr,addr);\n"
            << "}\n\n";
      }

      // A flush called in the init stage just needs to flush issue.
      _oh << "void taken_flush_" << getIssueName() << "(" << cname << "&,Logger *logger,unsigned,bool given_addr = false,addr_t addr = 0);\n";
      _oc << "void " << _uname << "::taken_flush_" << getIssueName() << "(" << cname << "&,Logger *logger,unsigned,bool given_addr,addr_t addr)\n"
          << "{\n"
          << "flush_issue(logger,given_addr,addr);\n"
          << "}\n\n";

      _oh << "void flush_issue(Logger *logger,bool given_addr,addr_t addr);\n";
      _oc << "void " << _uname << "::flush_issue(Logger *logger,bool given_addr,addr_t addr)\n"
          << "{\n"
          << "if (logger) logger->logFlush(\"pipeline\");\n"
          << "_iss->log_flush_pending();\n"
          << _fname << ".flush((given_addr) ? addr : get_pc());\n"
          << "++_flushCount;\n";
      if (_ir.hasExceptions()) {
        _oc << "_fetchExceptionInProgress = false;\n";
      }
      _oc << "}\n\n";
    }

    // Generate deadlock checking logic.
    void genCheckForDeadlock()
    {
      if (_config._deadlock == 0) {
        return;
      }

      if (top_node()) {
        _cd._member_vars.push_back("unsigned _deadlockCount");
        _cd._constr_inits.push_back("_deadlockCount(0)");
        _cd._reset_stmts.push_back("_deadlockCount = 0");
        _child_args.push_back("_deadlockCount");
        
        _oh << "void checkForDeadlock() {\n"
            << "if (_deadlockCount > _timeOut) {\n"
            << "breakDeadlock();\n"
            << "}\n"
            << "}\n\n";
      } else {
        add_shared_ref("unsigned","_deadlockCount");
      }
    }

    // This function breaks a deadlock by removing the last instruction in the
    // pipe.  However, this is only valid for safe-mode models, since all
    // functionality is performed by the ISS all at once.  Otherwise, it just
    // causes the simulation to exit.
    //
    // Only generated if dead-lock detection is enabled.
    void genBreakDeadlock()
    {
      if (_config._deadlock == 0) {
        return;
      }

      char dbuf[1000];
      sprintf(dbuf,"%d",_config._deadlock);
      gc_string dstr = dbuf;
      _cd._reset_stmts.push_back("_timeOut = " + dstr);
      _cd._constr_stmts.push_back("_timeOut = " + dstr);

      _oh << "void breakDeadlock();\n\n";
      _oc << "void " << _uname << "::breakDeadlock()\n"
          << "{\n";

      if (_config._safeMode) {
        const PipeVect &stages = _ir.sortedStages();
        gc_string cname = getInstBaseName(_ir);
        
        _oc << "if (!_failOnDeadlock) {\n";

        gc_string uarch = (have_threads()) ? "*instr->_uarch" : "*this";
        gc_string dptr  = (have_threads()) ? "instr->_uarch->" : "";
        
        ForEach(stages,s_iter) {
          const PipelineStage &stage = **s_iter;
          gc_string sname = stage.name();
          
          _oc << "for (unsigned i = 0; i != " << stage.size() << "; ++i) {\n";

          if (stage.scheduler() != sIndependent) {
            _oc << "Logger *logger" << " = " << getStageLoggerCall(sname,-1) << ";\n"
                << "if (" << cname << " *instr = " << sname << ".head()) {\n"
                << "if (logger) logger->logMessage(\"discarding instruction to break deadlock.\");\n"
                << "instr->flush(" << uarch << ",logger);\n"
                << sname << ".remove_instr();\n"
                << dptr << "discard(instr);\n"
                << "_deadlockCount = 0;\n"
                << "return;\n"
                << "}\n";
          } else {
            _oc << "Logger *logger" << " = " << getStageLoggerCallAbs(sname,0,"i") << ";\n"
                << "if (" << cname << " *instr = " << sname << ".get(i)) {\n"
                << "if (logger) logger->logMessage(\"discarding instruction to break deadlock.\");\n"
                << "instr->flush(*this,logger);\n"
                << sname << ".remove_instr_abs(i);\n"
                << "discard(instr);\n"
                << "_deadlockCount = 0;\n"
                << "return;\n"
                << "}\n";
          }

          _oc << "}\n";
        }
        
        _oc << "}\n";
      }

      // If we've gotten to here, then we either didn't find an instruction to remove, or we're
      // failing on deadlock.
      _oc << "handleDeadlock();\n"
          << "}\n\n";
    }

    // Generates logic for handling external exceptions.
    void genExternExceptions()
    {
      if (_ir.hasExceptions()) {

        // We need to update the ISS, so that level-sensitive exceptions trigger
        // if their conditions are met.
        _pre_stmts.push_back("_iss->updateLevelSensitiveExceptions()");

        // We use the ISSs pending flag to store whether we have any kind of
        // active, external exception.  We've set up the generator so that the
        // ISS does not cancel a pending exception when the real exception is
        // enabled.  This allows us to remove the extra mask in the uadl model,
        // compared to how this was implemented in the original Uadl.
        _oh << "bool hasExternException();\n\n";
        _oc << "bool " << _uname << "::hasExternException()\n"
            << "{\n"
            << "return _iss->hasCurrentPendingExceptions();\n"
            << "}\n\n";

        _oh << "bool hasPendingExternException();\n\n";
        _oc << "bool " << _uname << "::hasPendingExternException()\n"
            << "{\n"
            << "return _iss->hasPendingExceptions();\n"
            << "}\n\n";

        _oh << "void genExceptions(unsigned flag);\n\n";
        _oc << "void " << _uname << "::genExceptions(unsigned flag)\n"
            << "{\n"
            << "_iss->setPendingExceptions(flag);\n"
            << "_iss->setExternException(flag);\n"
            << "}\n\n";

        _oh << "void cancelExceptions(unsigned flag);\n\n";
        _oc << "void " << _uname << "::cancelExceptions(unsigned flag)\n"
            << "{\n"
            << "_iss->cancelExceptions(flag);\n"
            << "}\n\n";

        // I don't think we need to flush the pipeline, because we guard the
        // call to this function with a check to see if the pipeline is empty.
        // We do need to flush the fetch unit.
        _oh << "void raiseExternException();\n\n";
        _oc << "void " << _uname << "::raiseExternException()\n"
            << "{\n";
        if (!_config._safeMode) {
          _oc << "// Hack for genExceptions() to get correct ISS state.\n"
              << "_iss->" << _ir.NIAName() << "_write(" << _fname <<".nextInstrAddr());\n";
        }
        _oc << "// logging hack- we want to immediately see the logging effects.\n"
            << "_iss->log_direct(true);\n"
            << "_iss->genExceptions(_iss->getExceptionFlag());\n"
            << "_iss->clearEdgeSensitivePendingExceptions();\n"
            << "_iss->log_direct(false);\n"
            << "flush_issue(" << _fname << ".logger(),false,0);\n"
            << "}\n\n";
      }
    }

    // Generates issue support logic.
    void genIssue()
    {
      gc_string cname = getInstBaseName(_ir);

      gc_string iname = getIssueName();
      _cd._member_vars.push_back("IssueStage<" + getInstBaseName(_ir) + "," + (have_threads() ? "true" : "false") + "> " + iname);
      gc_string t_iname(_fname.size()+2,' ');
      _cd._constr_inits.push_back(iname + "(\"" + t_iname + "\")");
      _cd._reset_stmts.push_back(iname + ".reset()");

      // If no issue stage was explicitly identified, then just use the last stage
      // in the scheduling order.
      const PipelineStage *issueStage = _res.issueStage();
      if (!issueStage && !_ir.sortedStages().empty()) {
        issueStage = _ir.sortedStages().back();
      }

      _oh << "void log_instr_read(addr_t ea,addr_t ra);\n";
      _oc << "void " << _uname << "::log_instr_read(addr_t ea,addr_t ra) {\n"
          << "_iss->log_instr_read(ea,ra);\n"
          << "}\n";

      // Before issuing, we make sure that there's space in the first real stage.
      _oh << "bool issueInquire();\n";
      _oc << "bool " << _uname << "::issueInquire()\n"
          << "{\n"
          << "return " << iname << ".empty() && " << issueStage->name() << ".has_space();\n"
          << "}\n\n";

      // fetch aspect
      // issueInstruction()
      _oh << cname << "* issueInstruction();\n";
      _oc << cname << " *" << _uname << "::issueInstruction()\n"
          << "{\n"
          << cname << " *instr = 0;\n"
          << "\n"
          << "if (FetchData fd = " << _fname << ".instrData(_iss->tracing_on())) {\n";
     
      if (_config._ic_enabled && !_config._safeMode) {
        _oc << "if ((instr = _instr_cache.find(fd._ra))) {\n"
            << "if ((fd._size < instr->size()) || !"
            << "_iss->quick_decode(fd._ea,fd._data,0)"
            << ") return 0;\n"
            << "instr->reset(_issueId++,_commits);\n"
            << "instr->_instr->quick_reset(*_iss);\n"
            << "_instr_alloc_count++;\n"
            << "} else {\n";
      }

      if (_config._safeMode) {
        _oc << "instr = _iss->decode(fd._ea,fd._size,_speculative,*this);\n";
      } else {
        _oc << "instr = _iss->decode(fd._ea, fd._data, 0, fd._size, *this);\n";
      }

      if (_config._ic_enabled) {
        _oc << "}\n";
      }

      if (!_config._safeMode) {
        _oc << _fname << ".decodeUpdate(instr, instr ? instr->size() : 0);\n";
      }

      // Even if we have just one fetch size, we might get a 0 due to the
      // max-instruction-count being exceeded.
      _oc << "if (!instr) return 0;\n";

      if (_config._ic_enabled) {
        _oc << "instr->_ra = fd._ra;\n";
      }
      if (!_res.branchPredictors().empty() && !_config._safeMode) {
        // Safe mode models handle this in set_cur_instr.
        _oc << "instr->set_prediction_data(" << _fname << ".predTaken()," << _fname << ".predTrg());\n";
      }

      if (_config._safeMode) {
        _oc << "if (is_executing(*instr)) {\n"
            << "incrCommitCount();\n"
            << "try {\n"
            << "check_breakpoints(get_pc());\n"
            << "}\n"
            << "catch (SimInterrupt) {\n"
            << "setBreakpoint();\n"
            << "}\n"
            << "}\n";
      }

      _oc << "} else if (!" << _fname << ".empty()) {\n"
          << "return 0;\n"
          << "}\n";

      if (_ir.hasExceptions()) {
        if (!_config._safeMode) {
          _oc << "else if (hasFetchException()) {\n"
              << "_fetchExceptionInProgress = true;\n"
              << cname << " *fe = " << getFetchExceptionFunc() << "(0);\n"
              << "fe->set_fetch_exception(_fetchException,_fetchExceptionHandle," << _fname << ".nextFetchAddr(),_fetchLogData);\n"
              << "_fetchLogData = 0;\n"
              << "_fetchException = 0;\n"
              << "return fe;\n"
              << "}\n";
        } else {
          _oc << "else if (hasFetchException() && !fetchExceptionInProgress()) {\n"
              << "if (!_speculative) {\n"
              << "instr = _iss->decode(Fetcher.nextFetchAddr(),0,_speculative,*this);\n"
              << "}\n"
              << "_fetchExceptionInProgress = true;\n"
              << cname << " *fe = " << getFetchExceptionFunc() << "(0);\n"
              << "_fetchLogData = _iss->log_get_pending_queue();\n"
              << "fe->set_fetch_exception(_fetchException,_fetchExceptionHandle," << _fname << ".nextFetchAddr(),_fetchLogData);\n"
              << "_fetchLogData = 0;\n"
              << "_fetchException = 0;\n"
              << "return fe;\n"
              << "}\n";
        }
      }

      _oc << "\n"
        << "return instr;\n"
        << "}\n\n";

      // Miscellaneous logic for when an instruction has "issued" (according to
      // the stage marked as 'issue').
      _oh << "void issue(InstrPacket *instr);\n";
      _oc << "void " << _uname << "::issue(InstrPacket *instr)\n"
          << "{\n";
      // If we have an issue stage, then logging is performed when the stage
      // calls this function.  Otherwise, we handle it immediately in
      // issueInstructions.
      if (_res.issueStage()) {
        _oc << "_iss->log_instr_issue_time(instr,currTime());\n";
      }
      _oc << _fname << ".removeInstr(instr->size());\n"
          << "}\n\n";
    }

    // Creates the stall logic for lock-step pipelines (the stalled()
    // predicate).  The flag always lives at the top-level and is propagated
    // down to constituent threads.
    void genStall()
    {
      if (top_node()) {
        _cd._member_vars.push_back("bool _stall");
        _cd._constr_inits.push_back("_stall(false)");
        _cd._reset_stmts.push_back("_stall = false");
        _child_args.push_back("_stall");
      } else {
        add_shared_ref("bool","_stall");
      }

      _oh << "bool stalled() const { return _stall; };\n\n";
    }

    // Return true if the threads' fetch units are interleaved.
    bool interleavedIssue(const Thread &thread)
    {
      if (leaf_node(thread)) {
        return thread.res().fetcher()->interleaved();
      } else {
        return interleavedIssue(*thread.threads().begin()->second);
      }
    }

    void genLeafIssueInstructions()
    {
      // Get instructions from the fetch unit and then execute them if we get an
      // instruction.
      gc_string iname = getIssueName();
      gc_string fpred = (fetch_bp()) ? (_fname + ".predTaken()," + _fname + ".predTrg()") : "false,0";
      bool interleaved = _thread.res().fetcher()->interleaved();

      bool has_ex = _ir.hasExceptions();

      if (_config._safeMode) {
        // Hold off on execution if we haven't reached the fast-forward time
        // yet.
        _oc << "if (currTime() < _startTime) return false;\n";
      }

      // Then, if there's a leftover from issue, run that.  This is usually
      // unlikely to happen.
      _oc << "if (!" << iname << ".empty() && " << iname << ".instr()->action(*this,((Tracing && logPipeline()) ? " << iname << ".logger(" << fpred << "): 0),0)) {\n"
          << iname << ".remove_instr();\n"
          << "}\n";

      // If we have exceptions, then add support for external exceptions.  For
      // now, if an external exception is present, hold off on issue and wait
      // until the pipeline is empty before raising the exception.  We might
      // have to make this more configurable in the future.
      if (has_ex) {
        _oc << "if (!hasExternException()) {\n";
      }

      _oc << ((interleaved) ? "if" : "while") << " (_iss->is_active() && !fetchExceptionInProgress() && issueInquire()) {\n"
          << "if ((" << iname << ".add_instr(issueInstruction()))) {\n";
      if (!_res.issueStage()) {
        _oc << "_iss->log_instr_issue_time(" << iname << ".instr()->_instr,currTime());\n";
      }
      _oc << "if (" << iname << ".instr()->action(*this,((Tracing && logPipeline()) ? " << iname << ".logger(" << fpred << "): 0),0)) {;\n"
          << iname << ".remove_instr();\n";
      if (interleaved) {
        _oc << "return true;\n";
      }
      _oc << "}\n"
          << "} else {\n";
      if (!interleaved) {
        _oc << "break;\n";
      }
      _oc << "}\n"
          << "}\n";
      if (has_ex) {
        _oc << "} else if (instr_alloc_count() == 0) {\n"
            << "raiseExternException();\n"
            << "}\n";
      }

      if (interleaved) {
        _oc << "return false;\n";
      } else {
        _oc << "return true;\n";
      }
    }

    void genThreadGroupIssueInstructions()
    {
      if (interleavedIssue(_thread)) {
        unsigned tc = _thread.threads().size();
        ForRange(tc,count) {
          _oc << "bool i" << count << ";\n";
        }
        _oc << "do {\n";
        unsigned count = 0;
        ForEach(_thread.threads(),i) {
          _oc << "i" << count << " = " << i->first << ".issueInstructions();\n";
          ++count;
        }          
        bool first = true;
        _oc << "} while (";
        ForRange (tc,count) {
          pfx_sep(_oc," || ",first);
          _oc << "i" << count;
        }
        _oc << ");\n";
      } else {
        ForEach(_thread.threads(),i) {
          _oc << i->first << ".issueInstructions();\n";
        }
      }
      _oc << "return true;\n";
    }

    void genIssueInstructions()
    {
      _oh << "bool issueInstructions();\n";
      _oc << "bool " << _uname << "::issueInstructions()\n"
          << "{\n";
        
      if (leaf_node()) {
        genLeafIssueInstructions();
      } else {
        genThreadGroupIssueInstructions();
      }

      _oc << "}\n\n";

      if (!leaf_node()) {
        _oh << "virtual uint64_t instrCount() const;\n\n";
        _oc << "uint64_t " << _uname << "::instrCount() const\n"
            << "{\n"
            << "return ";
        bool first = true;
        ForEach(_thread.threads(),i) {
          pfx_sep(_oc," + ",first);
          _oc << i->first << ".instrCount()";
        }
        _oc << ";\n"
            << "}\n\n";
      }
    }

    // Grabbing the right logger is always relative, since the stages act as
    // queues.  Thus position 0 can actually be at index 0.
    gc_string getStageLoggerCall(const gc_string &sn,int pos)
    {
      gc_ostringstream ss;

      ss << "((Tracing && logPipeline()) ? " << getStageLoggerArray() << ".get(";
      if (pos >= 0) {
        ss << getStageLoggerIndex(sn,0) << "+" << sn << ".pos(" << pos << ")";
      } else {
        ss << getStageLoggerIndex(sn,0) << "+" << sn << ".headp()";
      }
      ss << ") : 0)";
      return ss.str();
    }

    // Same as above, but allows for a variable to specify the relative
    // position.
    gc_string getStageLoggerCall(const gc_string &sn,const gc_string &pos)
    {
      gc_ostringstream ss;

      ss << "((Tracing && logPipeline()) ? " << getStageLoggerArray() << ".get("
         << getStageLoggerIndex(sn,0) << "+" << sn << ".pos(" << pos << ")"
         << ") : 0)";
      return ss.str();
    }

    // For getting absolute loggers.
    gc_string getStageLoggerCallAbs(const gc_string &sn,int indexn,const char *index = 0)
    {
      gc_ostringstream ss;

      ss << "((Tracing && logPipeline()) ? " << getStageLoggerArray() << ".get(";
      if (index) {
        ss << getStageLoggerIndex(sn,0) << "+" << index;
      } else {
        ss << getStageLoggerIndex(sn,indexn);
      }
      ss << ") : 0)";
      return ss.str();
    }    

    // Code for a dependent-style scheduler.
    void genStageDepActionCall(const gc_string &sn,unsigned count,bool shared,unsigned pos)
    {
      if (count) {
        _oc << "if (!" << sn << ".empty()";
        if (shared) {
          _oc << " && (" << sn << ".head()->is_in_stage(&" << sn << "))";
        }
        _oc << ") {\n"
            << "if (" << sn << ".head()->action(";
        if (have_threads()) {
          _oc << "*" << sn << ".head()->_uarch";
        } else {
          _oc << "*this";
        }
        _oc << ",logger" << pos << "," << pos << ")) {\n";
        genStageDepActionCall(sn,count-1,shared,++pos);
        _oc << "} else {\n"
            << "_stall = true;\n"
            << "}\n"
            << "}\n";          
      }
    }

    // Code for an independent-queue-style scheduler.
    void genStageIndepActionCall(const gc_string &sn,unsigned count,bool shared,unsigned pos)
    {
      if (count) {
        _oc << "if (" << sn << "[sindex]";
        if (shared) {
          _oc << " && (" << sn << "[sindex]->is_in_stage(&" << sn << "))";
        }
        _oc << ") {\n"
            << "if (!" << sn << "[sindex]->action(";
        if (have_threads()) {
          _oc << "*" << sn << "[sindex]->_uarch";
        } else {
          _oc << "*this";
        }
        _oc << ",logger" << pos << ",sindex)) {\n"
            << "_stall = true;\n"
            << "++sindex;\n"
            << "}\n";
        genStageIndepActionCall(sn,count-1,shared,++pos);
        _oc << "}\n";
      }
    }

    void genStageExecLogic(const PipeVect &stages,PipeVect::const_iterator siter)
    {
      if (siter == stages.end()) {
        return;
      }

      const PipelineStage &stage = **siter;
      const gc_string &sn = stage.full_name();
        
      if (stage.scheduler() == sLockStep && stage.size() > 1) {
        gc_string cname = getInstBaseName(_ir);
        // Lock step multi-issue means that we evaluate all outer guards, then
        // execute inner actions if all pass.
        _oc << "{\n";
        ForRange(stage.size(),i) {
          _oc << "Logger *logger" << i << " ATTRIBUTE_UNUSED = " << getStageLoggerCall(sn,i) << ";\n"
              << "bool r" << i << " = (!" << sn << "[" << i << "] || ";
          if (_has_shared_stages) {
            _oc << "(" << sn << "[" << i << "]->is_in_stage(&" << sn << ")) || ";
          }
          _oc << sn << "[" << i << "]->action(";
          if (have_threads()) {
            _oc << "*" << sn << "[" << i << "]->_uarch";
          } else {
            _oc << "*this";
          }
          _oc << ",logger" << i << "," << i << "));\n";
        }
        _oc << "if (";
        bool first = true;
        ForRange(stage.size(),i) {
          pfx_sep(_oc," && ",first);
          _oc << "r" << i;
        }
        _oc << ") {\n";
        ForRange(stage.size(),i) {
          _oc << "if (" << sn << ".head()) " << sn << ".head()->move(";
          if (have_threads()) {
            _oc << "*" << sn << ".head()->_uarch";
          } else {
            _oc << "*this";
          }
          _oc << ",logger" << i << "," << i << ");\n";
        }
        _oc << "} else {\n"
            << "_stall = true;\n"
            << "}\n";
        _oc << "}\n";
      } else if ((stage.scheduler() == sDependent) || (stage.scheduler() == sLockStep && stage.size() <= 1)) {
        _oc << "{\n";
        // Note: We fetch loggers for all elements of the stage, even though we
        // only execute according to the bandwidth count, so that the logger
        // will report all instructions in the stage, even if they're not
        // touched.
        ForRange(stage.size(),i) {
          _oc << "Logger *logger" << i << " ATTRIBUTE_UNUSED = " << getStageLoggerCall(sn,i) << ";\n";
        }
        // Not multi-issue lock step.  Evaluate 'bandwidth' items, stopping on a stall.
        genStageDepActionCall(sn,stage.bandwidth(),stage.shared(),0);
        _oc << "}\n";
      } else if (stage.scheduler() == sIndependent) {
        _oc << "{\n";
        ForRange(stage.size(),i) {
          _oc << "Logger *logger" << i << " ATTRIBUTE_UNUSED = " << getStageLoggerCall(sn,i) << ";\n";
        }
        _oc << "unsigned sindex = 0;\n";
        // Independent scheduling.
        genStageIndepActionCall(sn,stage.bandwidth(),stage.shared(),0);
        _oc << "}\n";
      }
      // Proceed to next stage.
      genStageExecLogic(stages,++siter);
    }

    // Generates the cycle function, which simulates one cycle of the
    // processor's behavior.  Note that we unroll all loops through code
    // generation in order to maximize the chances that the host processor's
    // branch predictor can correctly guess what to do.
    void genScheduler()
    {
      genPreCycle();
      genPostCycle();
      genCanMove();

      // Only the top-level item gets the scheduler.
      if (!top_node()) return;

      _oh << "void cycle();\n";

      _oc << "void " << _uname << "::cycle()\n"
          << "{\n"
          << "try {\n"
          << "preCycle();\n"
          << "_stall = false;\n";

      // When tracking deadlock, bump the deadlock counter first.  If we commit
      // an instruction, then it will be cleared, since this implies forward
      // progress.
      if (_config._deadlock) {
        _oc << "++_deadlockCount;\n";
      }

      // The stage scheduling order has already been worked out by the
      // front-end, so we just use that order here.  If lock-step scheduling is
      // used then we execute all outer guards first, then only execute inner
      // code if no stall occurs.  In both cases, we execute all stages and make
      // the stalled() predicate available for querying.
      const PipeVect &stages = _ir.sortedStages();
      genStageExecLogic(stages,stages.begin());
      gc_string iname = getIssueName();

      if (_config._deadlock) {
        _oc << "checkForDeadlock();\n";
      }

      _oc << "issueInstructions();\n"
          << "postCycle();\n"
          << "}\n"
          << "catch (SimInterrupt) {\n"
          << "postCycle();\n"
          << "throw;\n"
          << "}\n"
          << "}\n\n";

      _oh << "void simulate();\n";
      _oc << "void " << _uname << "::simulate()\n"
          << "{\n"
          << "while (active() && (currTime() < _endTime )) {\n"
          << "cycle();\n"
          << "}\n"
          << "}\n\n";
    }

    void genFetchTranslate()
    {
      // fetchTranslate()
      _oh << "bool fetchTranslate(addr_t ea, addr_t &ra);\n";
      _oc << "bool " << _uname << "::fetchTranslate(addr_t ea, addr_t &ra)\n"
          << "{\n";
      if (_ir.hasExceptions()) {
        _oc << "try {\n";
      }
      _oc << "_iss->prefetch_translate(ra, ea);\n"
          << "return true;\n";
      if (_ir.hasExceptions()) {
        _oc << "}\n"
            << "catch (" << _iss_type << "::Exception e) {\n"
            << "return false;\n"
            << "}\n";
      }      
      _oc << "}\n\n";

      _oh << "bool fetchTranslate(addr_t ea, addr_t &ra, unsigned frontPadSize, unsigned size,byte_t *data);\n";
      _oc << "bool " << _uname << "::fetchTranslate(addr_t ea, addr_t &ra, unsigned frontPadSize, unsigned size,byte_t *data)\n"
          << "{\n"
        
          << "PrefetchReturnType prefetchStatus = adl::prNotActive;\n"
          << (_ir.hasExceptions() ?
              "  const uint64_t oldExceptionFlag = _iss->getExceptionFlag();\n" : "")
          << "\n";

      if (_ir.hasExceptions()) {
        _oc << "try {\n";
      }

      _oc << "if (frontPadSize) {\n"
          << "addr_t addr = ea + frontPadSize;\n"
          << "\n"
          << "// Invalidate MMU cache to force a new TLB lookup.\n"
          << "_iss->checkInstrPerms();\n"
          << "prefetchStatus = _iss->prefetch_translate(addr, addr);\n"
          << "}\n"
          << "\n"
          << "if (prefetchStatus != adl::prException) {\n"
          << "switch (size) {\n";

      assert(_fetcher);
      const vector<unsigned> &fetchSizes = _fetcher->fetchSizes();
      for (unsigned i = 0; i < fetchSizes.size(); i++) {
        unsigned size = fetchSizes[i];

        _oc << "case " << size << ":\n"
            << "do {\n";

        if (!_config._safeMode) {
          _oc << "prefetchStatus = _iss->prefetch<" << size << ">(ea);\n";
        }
        else {
          _oc << "prefetchStatus = _iss->prefetch<" << size << ", true>\n"
              << "(data, ra, ea);\n";
        }

        _oc << "} while (prefetchStatus == adl::prSkip);\n"
            << "\n"
            << "break;\n"
            << "\n";
      }

      _oc << "default: assert(false); // should never reach here\n"
          << "}\n"
          << "\n"
          << "assert(prefetchStatus != adl::prNotActive);\n"
          << "}\n"
          << "\n";

      if (_ir.hasExceptions()) {
        _oc << "}\n"
            << "catch (" << _iss_type << "::Exception e) {\n";
        if (_config._safeMode) {
          _oc << "_fetchException = oldExceptionFlag ^ (unsigned)e;\n"
              << "_iss->cancelExceptions(_fetchException);\n";
        } else {
          _oc << "_fetchExceptionInProgress = false;\n"
              << "_fetchException = oldExceptionFlag ^ (unsigned)e;\n"
              << "_fetchExceptionHandle = _iss->getExceptionData(_fetchException);\n"
              << "_fetchLogData = _iss->log_get_pending_queue();\n"
              << "_iss->cancelExceptions(_fetchException);\n";
        }
        // This signals to the fetch unit that a fetch exception has occurred.
        // This value is not important- we're just trying to avoid a
        // conditional in the common path.
        _oc << "throw 0;\n"
             << "}\n"
             << "\n";
      }

      if (!_config._safeMode) {
        _oc << "ra = _iss->memory_request(0)._ra;\n"
            << "\n";
      }

      _oc << "return true;\n"
          << "}\n"
          << "\n\n";
    }

    void genFetchServices()
    {

      if (_ir.hasExceptions()) {
        // Fetch exception support stuff.
        _cd._member_vars.push_back("unsigned _fetchException");
        _cd._constr_inits.push_back("_fetchException(0)");
        _cd._reset_stmts.push_back("_fetchException = 0");

        _cd._member_vars.push_back("bool _fetchExceptionInProgress");
        _cd._constr_inits.push_back("_fetchExceptionInProgress(false)");
        _cd._reset_stmts.push_back("_fetchExceptionInProgress = false");

        _cd._member_vars.push_back("adl::LogType _fetchLogData");
        _cd._constr_inits.push_back("_fetchLogData(0)");
        _cd._reset_stmts.push_back("_fetchLogData = 0");

        _cd._member_vars.push_back("adl::ExHandle _fetchExceptionHandle");

        _oh << "bool hasFetchException() const { return _fetchException; };\n"
            << "bool fetchExceptionInProgress() const { return _fetchExceptionInProgress; };\n"
            << "void signalFetchException() { _fetchExceptionInProgress = true; }\n";
      } else {
        _oh << "bool hasFetchException() const { return false; };\n"
            << "bool fetchExceptionInProgress() const { return false; };\n";
      }

      _oh << "void reset_fetch();\n";
      _oc << "void " << _uname << "::reset_fetch()\n"
          << "{\n"
          << "if (_resetFetch) {\n"
          << "_resetFetch = false;\n"
          << _fname << ".reset(get_pc());\n"
          << "}\n"
          << "}\n\n";

      _oh << "void force_reset_fetch();\n";
      _oc << "void " << _uname << "::force_reset_fetch()\n"
          << "{\n"
          << _fname << ".reset(get_pc());\n"
          << "}\n\n";

      _run_stmts.push_back("reset_fetch()");

      _oh << "virtual addr_t ea(addr_t addr);\n";

      _oc << "addr_t " << _uname << "::ea(addr_t addr) { return _iss->mask_ea(addr); }\n\n";

      genFetchTranslate();

      _oh << "void updateInstrCacheStatus(CacheStatusHint &hint,addr_t addr)";
      if (_config._safeMode && _ir.firstLevelInstMem()->isCache()) {
        _oh << ";\n\n";
        _oc << "void " << _uname << "::updateInstrCacheStatus(CacheStatusHint &hint,addr_t addr)\n"
            << "{\n"
            << "_iss->update_instr_cache_status(hint._cacheStatus,CacheIFetch,addr);\n"
            << "}\n\n";
      } else {
        _oh << " {};\n\n";
      }

      // fetchCacheEnabled()
      _oh << "bool fetchCacheEnabled(addr_t addr);\n";
      _oc << "bool " << _uname << "::fetchCacheEnabled(addr_t addr)\n"
          << "{\n";

      const gc_string instrMem = tmName(_ir.firstLevelInstMem()->name());
      if (_config._safeMode && _ir.firstLevelInstMem()->isCache()) {
        _oc << "return _iss->" << instrMem << "_cache_enabled(adl::CacheIFetch,addr);\n";
      }
      else {
        _oc << "return false;\n";
      }

      _oc << "}\n\n";

      _oh << "\n";
    }

    // Given a stage and a position, return the position in the model's array
    // that stores pointers to all loggers.
    unsigned getStageLoggerIndex(const gc_string &sn,unsigned index)
    {
      auto iter = _stage_loggers.find(StageLogItem(sn,index));
      assert(iter != _stage_loggers.end());
      return iter->second;
    }

    void genUpdateTracing()
    {
      const PipeVect &stages = _ir.sortedStages();

      // All of the loggers for the individual pipeline positions are kept in a
      // single array for easy access, though they're "owned" by their
      // respective stage.
      unsigned stage_logger_count = 0;
      _stage_loggers.clear();
      ForEach(stages,i) {
        const PipelineStage &stage = **i;
        ForRange(stage.size(),j) {
          _stage_loggers.insert(make_pair(StageLogItem(stage.full_name(),j),stage_logger_count++));
        }
      }

      _cd._member_vars.push_back("StageInstrLoggers " + getStageLoggerArray());
      gc_string init;
      MkGStr(init,getStageLoggerArray() << "(" << stage_logger_count << ")");
      _cd._constr_inits.push_back(init);

      _oh << "void update_tracing();\n";
      _oc << "void " << _uname << "::update_tracing()\n"
          << "{\n"
          << "unsigned tflags ATTRIBUTE_UNUSED = (_out) ? LogAll : LogNone;\n";

      if (!top_node()) {
        // If we're not the top-node, then get the current set of tracing flags
        // so that all nodes are consistent.
        _oc << "_flags = top.tracing_flags();\n";
      }

      ForEach(_thread.threads(),i) {
        _oc << i->first << ".update_tracing();\n";
      }

      if (_fetcher) {
        _oc << "if (logFetch()) {\n"
            << _fname << ".createLogger(_logclient,tflags);\n"
            << "} else {\n"
            << _fname << ".removeLogger();\n"
            << "}\n"
            << "if (logPipeline()) {\n"      
            << getIssueName() << ".createLogger(_logclient,tflags);\n"
            << "} else {\n"
            << getIssueName() << ".removeLogger();\n"
            << "}\n";
      }

      // We log all stages together, at the top.
      if (top_node()) {

        _oc << "if (logPipeline()) {\n";
        ForEach(stages,i) {
          const PipelineStage &stage = **i;
        _oc << stage.full_name() << ".createLogger(_logclient,tflags);\n";
        ForRange(stage.size(),j) {
          _oc << getStageLoggerArray() << ".set(" << getStageLoggerIndex(stage.full_name(),j) << "," << stage.full_name() << ".logger(" << j << "));\n";
        }
        }
        
        _oc << "} else {\n";

        ForEach(stages,i) {
          const PipelineStage &stage = **i;
          _oc << stage.full_name() << ".removeLogger();\n";
        }
        _oc << getStageLoggerArray() << ".reset();\n"
            << "}\n";
      }

      ForEach(_res.caches(),i) {
        const gc_string managerName = getMemObj(*i->second);

        if (is_shared(*i->second)) continue;
        _oc << "if (logMemory()) {\n"
            << managerName << ".createLogger(_logclient,tflags);\n"
            << "} else {\n"
            << managerName << ".removeLogger();\n"
            << "}\n";        
      }

      if (!_config._extMemMode) {
        ForEach (_res.memories(), i) {
          const gc_string managerName = getMemObj(*i->second);

          if (is_shared(*i->second)) continue;
          _oc << "if (logMemory()) {\n"
              << managerName << ".createLogger(_logclient,tflags);\n"
              << "} else {\n"
              << managerName << ".removeLogger();\n"
              << "}\n";
        }
      }

      ForEach(_res.customResources(),i) {
        const CustomResource &cr = *i->second;
        gc_string n = cr.name();

        if (is_shared(*i->second)) continue;
        _oc << "if (logOther()) {\n"
            << n << ".createLogger(_logclient,tflags);\n"
            << "} else {\n"
            << n << ".removeLogger();\n"
            << "}\n";
      }
     
      _oc << "}\n\n";

      if (top_node()) {
        _oh << "void report_and_clear(std::ostream &o,const std::string &pfx);\n";
        _oc << "void " << _uname << "::report_and_clear(ostream &o,const string &pfx)\n"
            << "{\n"
            << "*_out << \"time \" << dec << currTime() << \":\\n\";\n"
            << "report_and_clear_top(o,pfx);\n";

        RevForEach(stages,i) {
          const PipelineStage &stage = **i;
          _oc << stage.full_name() << ".report_and_clear(o,pfx);\n";
        }

        _oc << "*_out << '\\n';\n"
            << "report_and_clear_bottom(o,pfx);\n"
            << "}\n\n";
      }

      _oh << "void report_and_clear_top(std::ostream &o,const std::string &pfx);\n";
      _oc << "void " << _uname << "::report_and_clear_top(ostream &o,const string &pfx)\n"
          << "{\n";
      if (leaf_node()) {
        if (_config._safeMode) {
          _oc << "if (_speculative) {\n"
              << "*_out << pfx << \"[\" << _iss->name() << \" speculative on iid=\" << dec << _spec_tag << \"]\\n\";\n"
              << "}\n";
        }
        if (_ir.hasExceptions()) {      
          _oc << "if (_iss->hasPendingExceptions()) {\n"
              << "*_out << pfx << \"[external exception]\\n\";\n"
              << "}\n";
        }     
      }
      if (_fetcher) {
        _oc << _fname << ".report_and_clear(o,pfx);\n"
            << getIssueName() << ".report_and_clear(o,pfx);\n";
      }
      ForEach(_thread.threads(),i) {
        _oc << i->first << ".report_and_clear_top(o,pfx);\n";
      }
      _oc << "}\n\n";

      _oh << "void report_and_clear_bottom(std::ostream &o,const std::string &pfx);\n";
      _oc << "void " << _uname << "::report_and_clear_bottom(ostream &o,const string &pfx)\n"
          << "{\n";
        
      ForEach(_thread.threads(),i) {
        _oc << i->first << ".report_and_clear_bottom(o,pfx);\n";
      }

      ForEach(_res.caches(),i) {
        if (!is_shared(*i->second)) {
          const gc_string managerName = getMemObj(*i->second);
          _oc << managerName << ".report_and_clear(o,pfx);\n";
          _oc << "*_out << '\\n';\n";
        }
      }
      
      if (!_config._extMemMode) {
        ForEach (_res.memories(), i) {
          if (!is_shared(*i->second)) {
            const gc_string managerName = getMemObj(*i->second);
          
            _oc << managerName << ".report_and_clear(o,pfx);\n";
            _oc << "*_out << '\\n';\n";
          }
        }
      }
      
      ForEach(_res.customResources(),i) {
        const CustomResource &cr = *i->second;
        if (!is_shared(cr)) {
          gc_string n = cr.name();
        
          _oc << n << ".report_and_clear(o,pfx);\n";
          _oc << "*_out << '\\n';\n";
        }
      }
      _oc << "}\n\n";
    }

    void genSystemC()
    {
      // Only generate if we're a top-level item (the true core, not a thread)
      // and we have SystemC enabled.
      if (_config._systemc == scNone || !top_node()) {
        return;
      }

      _oh << "void clock_action();\n";

      gc_string type;
      if (_config._systemc == scMethod) {
        // SystemC SC_METHOD interface: Basically, we just add a clock and
        // attach clock_action() to execute on it.
        type = "SC_METHOD";
        _oc << "void " << _uname << "::clock_action()\n"
            << "{\n"
            << "try {\n"
            << "if (active()) {\n"
            << "cycle();\n"
            << "}\n"
            << "}\n"
            << "catch (SimInterrupt) {\n"
            << "setBreakpoint();\n"
            << "}\n"
            << "}\n\n";
      }
      else if (_config._systemc == scThread) {
        // For threads, it's basically the same thing, except that we use a
        // loop.
        type = "SC_THREAD";
        _oc << "void " << _uname << "::clock_action()\n"
            << "{\n"
            << "while (true) {\n"
            << "wait();\n"
            << "try {\n"
            << "if (active()) {\n"
            << "cycle();\n"
            << "}\n"
            << "}\n"
            << "catch (SimInterrupt) {\n"
            << "setBreakpoint();\n"
            << "}\n"
            << "}\n"
            << "}\n\n";
      }

      // At the start of simulation, call pre_run() so that the fetch unit, so
      // that any necessary resources are sync'd up.
      _oh << "virtual void start_of_simulation();\n";
      _oc << "void " << _uname << "::start_of_simulation()\n"
          << "{\n"
          << "preRun();\n"
          << "}\n\n"
          << "SC_HAS_PROCESS(" << _uname << ");\n\n";

      _cd._constr_args.push_front("sc_module_name name");
      _cd._constr_inits.push_front("sc_module(name)");
      _cd._constr_stmts.push_back(type + "(clock_action)");
      _cd._constr_stmts.push_back("dont_initialize()");
      _cd._constr_stmts.push_back("sensitive << _clock.pos()");
    }

    // If we have a post-simulation hook, we create an RAII class to generate the
    // call upon exit from a simple_run_* function.
    void writePostSim()
    {
      if (!_config._postSimHook || !top_node()) return;
      
      const gc_string IssClass = getIssClassName(_thread);

      _oc << "struct PostSim {\n"
          << "PostSim(" << IssClass << " *core) : _core(core) {};\n"
          << "~PostSim() { _core->postSim(); }\n"
          << IssClass << " *_core;\n"
          << "} __post_sim__(_iss);\n\n";
    }

    void genArchSupport(const gc_string &parentClass)
    {
      if (leaf_node()) {
        _cd._member_vars.push_front("unsigned _thread_id");
        _cd._constr_inits.push_front("_thread_id(thread_id++)");
        if (have_threads()) {
          gc_ostringstream ss;
          ss << "_id_offset = " << _max_reg_id << " * _thread_id";
          _cd._constr_stmts.push_back(ss.str());
        }

        _cd._member_vars.push_front("bool _breakpoint");
        _cd._constr_inits.push_front("_breakpoint(false)");
        _cd._reset_stmts.push_front("_breakpoint = false");
      }

      // List member variables.
      _oh << "\n";
      _cd.writeMemberVars(_oh);
      _oh << "\n";

      // Write the constructor for the class.
      if (_config._extMemMode) {
        _cd._constr_args.push_front("UadlMemoryIf *memory");
      }
      _cd._constr_args.push_front("ttime_t startTime");
      _cd._constr_args.push_front("std::ostream &out");
      _cd._constr_args.push_front("unsigned &thread_id");
      _cd._constr_args.push_front("unsigned &id");

      // Base-class initialization.
      _cd._constr_inits.push_front(parentClass+"(startTime,out)");

      // Let levels for elements in the memory hierarchy.
      if (_ir.firstLevelDataMem()) {
        const gc_string dataMem = getMemObj(_ir.firstLevelDataMem()->name());
        _cd._constr_stmts.push_back(dataMem + ".set_level(0)");
      }
      if (_ir.firstLevelInstMem()) {
        const gc_string instrMem = getMemObj(_ir.firstLevelInstMem()->name());
        _cd._constr_stmts.push_back(instrMem + ".set_level(0)");
      }


      genSystemC();

      _oh << "virtual ~" << _uname << "();\n"
          << _uname;
      _cd.writeConstrArgs(_oh);
      _oh << ";\n";

      _oc << _uname << "::~" << _uname << "() {\n";
      if (top_node()) {
        _oc << "delete _iss;\n";
      }
      _oc << "}\n\n";

      _oc << _uname << "::" << _uname;
      _cd.writeConstrArgs(_oc);
      _cd.writeConstrInits(_oc);
      _oc << "\n{\n";
      _cd.writeConstrStmts(_oc);
      _oc << "};\n\n";

      // Note: Make sure we call the parent class's reset first, so that the ISS
      // is reset first.
      _oh << "void reset();\n";      
      _oc << "void " << _uname << "::reset()\n"
          << "{\n";
      _oc << parentClass << "::reset();\n";
      _cd.writeResetStmts(_oc);
      _oc << "}\n\n";

      _oh << "bool active() const;\n";
      _oc << "bool " << _uname <<"::active() const\n"
          << "{\n"
          << "if (breakpoint()) return false;\n";
      if (!leaf_node()) {
        _oc << "return ";
        bool first = true;
        ForEach(_thread.threads(),i) {
          pfx_sep(_oc," || ",first);
          _oc << i->first << ".active()";
        }
        _oc << ";\n";
      } else {
        _oc << "return (_iss->activeCount() || instr_alloc_count()";
        ForEach(_res.memories(), i) {
          _oc << " || " << getMemObj(i->second->name()) + ".isActive()";
        }
        ForEach(_res.caches(),i) {
          _oc << " || " << getMemObj(i->second->name()) + ".isActive()";
        }
        _oc << ");\n";
      }

      _oc << "}\n\n";

      _oh << "virtual bool is_active() const;\n";
      _oc << "bool " << _uname << "::is_active() const\n"
          << "{\n"
          << "return active();\n"
          << "}\n\n";

      // When setting and clearing breakpoints, we work directly with the
      // base-class flag if we're a leaf node, otherwise we propagate the change
      // to the leaf node.
      _oh << "virtual void setBreakpoint();\n";
      _oc << "void " << _uname << "::setBreakpoint()\n"
          << "{\n";
      if (!leaf_node()) {
        ForEach(_thread.threads(),i) {
          _oc << i->first << ".setBreakpoint();\n";
        }
      } else {
        _oc << "_breakpoint = true;\n";
      }
      _oc << "}\n\n";

      _oh << "virtual void clearBreakpoint();\n";
      _oc << "void " << _uname << "::clearBreakpoint()\n"
          << "{\n";
      if (!leaf_node()) {
        ForEach(_thread.threads(),i) {
          _oc << i->first << ".clearBreakpoint();\n";
        }
      } else {
        _oc << "_breakpoint = false;\n";
      }
      _oc << "}\n\n";

      _oh << "bool breakpoint() const;\n";
      _oc << "bool " << _uname << "::breakpoint () const\n"
          << "{\n"
          << "return ";
      if (!leaf_node()) {
        bool first = true;
        ForEach(_thread.threads(),i) {
          pfx_sep(_oc," || ",first);
          _oc << i->first << ".breakpoint()";
        }
      } else {
        _oc << "_breakpoint\n";
      }
      _oc << ";\n"
          << "}\n\n";

      _oh << "virtual bool hasBreakpoint() const;\n";
      _oc << "bool " << _uname << "::hasBreakpoint() const\n"
          << "{\n"
          << "return breakpoint();\n"
          << "}\n\n";



      _oh << "virtual void preRun();\n";
      _oc << "void " << _uname << "::preRun()\n"
          << "{\n";
      // Write any run statements needed by other stuff.
      ForEach(_thread.threads(),i) {
        _oc << i->first << ".preRun();\n";
      }
      ForEach(_run_stmts,i) {
        _oc << *i << ";\n";
      }
      _oc << "_hadReset = false;\n"
          << "}\n\n";

      // Only the top-level item get proceed() and run() functions.
      if (top_node()) {
        _oh << "virtual void proceed();\n";
        _oc << "void " << _uname << "::proceed()\n"
            << "{\n"
            << "try {\n"
            << "cycle();\n"
            << "}\n"
            << "catch (SimInterrupt) {\n"
            << "setBreakpoint();\n"
            << "}\n"
            << "}\n\n";
        
        _oh << "virtual void run(ttime_t endTime);\n";
        _oc << "void " << _uname << "::run(ttime_t endTime)\n"
            << "{\n";
        writePostSim();
        _oc << "try {\n"
            << "preRun();\n"
            << "_endTime = endTime;\n"
            << "simulate();\n"
            << "}\n"
            << "catch (SimInterrupt) {\n"
            << "setBreakpoint();\n"
            << "}\n"
            << "}\n\n";

        _oh << "virtual void run();\n";
        _oc << "void " << _uname << "::run()\n";
        _oc << "{\n";
        writePostSim();
        _oc << "try {\n"
            << "preRun();\n"
            << "simulate();\n"
            << "}\n"
            << "catch (SimInterrupt) {\n"
            << "setBreakpoint();\n"
            << "}\n"
            << "}\n\n";
      }

      _oh << "virtual void postSim();\n";
      _oc << "void " << _uname << "::postSim()\n"
          << "{\n";      
      // Write any post-simulation statements.
      ForEach(_thread.threads(),i) {
        _oc << i->first << ".postSim();\n";
      }
      ForEach(_post_sim_stmts,i) {
        _oc << *i << ";\n";
      }

      // Optional: Make sure that no more memory requests are still allocated
      // (somewhere).  Note: Might be useful as a sanity check, but doesn't seem
      // to be doing much right now, so I'm commenting it out.
      //_oc << "assert(_mem_access_pool.none_allocated());\n";

      _oc << "}\n\n";

    }

    void genRecursiveStallGetter(const string &name)
    {
      _oc << "unsigned " << _uname << "::" << name << "() const {\n"
          << "return ";
      bool first = true;
      ForEach(_thread.threads(),i) {
        pfx_sep(_oc," + ",first);
        _oc << i->first << "." << name << "()";
      }
      _oc << ";\n"
          << "}\n\n";
    }

    unsigned getPortId(CacheType type,const MemoryBase &mb)
    {
      if (mb.ports().size() > 1) {
        // If there is more than one port, then find a data port.  If not found, then use the default port.
        ForEach(mb.ports(),i) {
          if ( i->second->type() == type ) {
            return i->second->id();
          }
        }
      }
      return mb.defaultPort()->id();
    }

    // Generate the getter functions for reading the various stall counters.
    void genStallGetters()
    {
      _oh << "unsigned fetchStalls() const;\n";
      if (!leaf_node()) {
        genRecursiveStallGetter("fetchStalls");
      } else {
        _oc << "unsigned " << _uname << "::fetchStalls() const { return " << _fname << ".fetchStalls(); }\n\n";
      }

      if (auto dfirst = _ir.firstLevelDataMem()) {
        _oh << "unsigned dmemStalls() const;\n";
        if (!leaf_node()) {
          genRecursiveStallGetter("dmemStalls");
        } else {
          _oc << "unsigned " << _uname << "::dmemStalls() const { return " << getMemObj(dfirst->name()) << ".memStalls("
              << getPortId(DataCache,*dfirst) << "); }\n\n";
        }
      }

      if (auto ifirst = _ir.firstLevelInstMem()) {
        _oh << "unsigned imemStalls() const;\n";
        if (!leaf_node()) {
          genRecursiveStallGetter("imemStalls");
        } else {
          _oc << "unsigned " << _uname << "::imemStalls() const { return " << getMemObj(ifirst->name()) << ".memStalls("
              << getPortId(InstrCache,*ifirst) << "); }\n\n";
        }
      }

    }

    void genMemPortsDef(ostream &o,const MemoryBase &mem,bool use_name) 
    {
      ForEach(mem.ports(), i) {
        if (i->first == MemoryBase::defaultPortName()) break;

        auto p =  i->second;
        if (use_name) {
          o << getMemObj(mem) << ".";
        }
        o << "addPort(\"" << p->name() << "\"," 
          << p->commonBandwidth() << ", "
          << p->addrBandwidth() << ", " 
          << p->dataBandwidth() << ", " 
          << p->readLatency() << ", " 
          << p->writeLatency() << ", "
          << p->maxRequest() << ","
          << p->allowBusWait()
          << ");\n";
      }
    }

    void genMemSetParms(ostream &o,const Memory &mem)
    {
      o << getMemObj(mem) << ".setMemParms(\n"
        << mem.addrBandwidth() << ",\n"
        << mem.dataBandwidth() << ",\n"
        << mem.readLatency() << ",\n"
        << mem.writeLatency() << ",\n"
        << mem.maxRequest() << ",\n"
        << (mem.allowBusWait() ? "true" : "false") << ",\n"
        << (mem.preemptive() ? "true" : "false") << ",\n"
        << (mem.readOnly() || _config._safeMode ? "true" : "false")
        << ");\n";
    }

    // Create the memory objects.
    void genMemoryObjs()
    {
      ForEach (_res.memories(), i) {
        const Memory &mem = *i->second;

        const gc_string parent = (_config._issMemMode && _realExtMemMode) ? "ExtLatencyMem" : "Memory";
        const gc_string cname = getMemoryType(mem.name());
        const gc_string wname = getMemWrapType(mem.name());
          
        if (!is_shared(mem)) {

          // First, create the memory object itself if not using external memory.

          if (!_config._extMemMode || _config._issMemMode) {
            // Parameter 'debug' is only used by ExtLatencyMem
            _oh << "class " << cname << " : public " + parent << " {\n"
                << "public:\n"
                << cname << "(UadlArch &arch, bool debug) :\n"
                << parent << "(\n"
                << "arch,\n"
                << "\"" + mem.name() << "\",\n"
                << mem.addrBandwidth() << ",\n"
                << mem.dataBandwidth() << ",\n"
                << mem.readLatency() << ",\n"
                << mem.writeLatency() << ",\n"
                << mem.maxRequest() << ",\n"
                << (mem.allowBusWait() ? "true" : "false") << ",\n"
                << (mem.preemptive() ? "true" : "false") << ",\n"
                << (mem.readOnly() || _config._safeMode ? "true" : "false") << ",\n"
                << (!_config._issMemMode ? "true" : "false")
                << (_config._issMemMode && _realExtMemMode ? ",debug\n" : "")
                << ")"
                << (mem.preemptive() ? ",_arch(arch)" : "")
                << "{\n";
            genMemPortsDef(_oh,mem,false);
            _oh << "}\n\n";

            _oh << "\n"
                << "virtual ~" + cname + "() {}\n";
          
            if (mem.preemptive() || !_config._issMemMode) {
              _oh << "protected:\n";
            }
          
            if (!_config._issMemMode) {
              _oh << "virtual byte_t* hasData(addr_t addr, unsigned &size) const;\n";
              _oc << "byte_t *" << cname << "::hasData(addr_t addr, unsigned &size) const {\n"
                  << "return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr);\n"
                  << "}\n\n";
            }

            _oh << "};\n\n";
          }
        }

        // The wrapper is only needed for leaf-node objects.
        if (leaf_node()) {
          gc_vector<MemoryPort *> ports;
          ForEach (mem.ports(),i) {
            if (i->second->name() != mem.defaultPortName()) {
              ports.push_back(i->second);
            }
          }
          // Now we create a wrapper around the interface template.  If ports
          // exist, then this object also contains members to represent the
          // ports.
          gc_string sm = (_config._safeMode ? "true" : "false");
          gc_string ibname = getInstBaseName(_ir);
          _oh << "struct " << wname << " : public MemoryT<" << cname << "," << _uname << "," << ibname << "," << sm <<  ",0>\n"
              << "{\n"
              << wname << "(" << cname << " &mem) : MemoryT<" << cname << "," << _uname << "," << ibname << "," << sm <<  ",0>(mem)";
          ForEach (ports,i) {
            const MemoryPort &port = **i;

            _oh << ", " << port.name() << "(mem)";
          }          
          _oh << " {}\n\n";

          ForEach (ports,i) {
            const MemoryPort &port = **i;

            _oh << "MemoryT<" << cname << "," << _uname << "," << ibname << "," << sm <<  "," << port.id() << "> " << port.name() << ";\n";
          }
          _oh << "};\n\n";
        }
      }
    }

    gc_string getBlockType(const gc_string &b)
    {
      if (b == "true") {
        return "bcwTrue";
      } else if (b == "serialized") {
        return "bcwSerialized";
      }
      return "bcwFalse";
    }
    
    void genCacheObj(const Cache &cache)
    {
      if (is_shared(cache)) return;

      const gc_string ct = getCacheType(cache.name());

      gc_string basename = (cache.custom_model()) ? cache.custom_class() : "Cache";
    
      // The cache class declaration.
      _oh << "class " << ct << " : public " << basename << " {\n"
          << "public:\n"
          << "typedef std::vector<bool> BVect;\n"
          << "typedef std::vector<BVect> HasData;\n"
          << ct << "(" << _uname << " &uarch, UadlMemoryIf &memory);\n"
          << "virtual ~" << ct << "() {}\n"
          << "\n"
          << "void resize();\n"
          << "\n"
          << "void reset_has_data();\n"
          << "\n"
          << "  // access/query methods\n"
          << (cache.linefillWidth() ?
              "  virtual unsigned linefillWidth(addr_t ea,addr_t ra);\n"
              "\n"                      : "")
          << (cache.storeBufEnable() ?
              "  // access/query methods\n"
              "  virtual bool storeBufEnabled();\n"
              "\n"                      : "")
          << "bool is_write_through(addr_t addr);\n\n"
          << "protected:\n"
          << (cache.preemptive() ? 
              "virtual void preemptFetch();\n"
              "\n"                  : "")
          << "virtual bool lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const;\n"
          << (cachesHaveData() ?
              "  virtual byte_t *cachePosition(unsigned set, unsigned way, unsigned offset) const;\n"
              : "")
          << "virtual addr_t getTagAddr(unsigned set, unsigned way);\n"
          << "\n"
          << "virtual void enableCacheLineData(unsigned set, unsigned way) {\n"
          << "_hasData[set][way] = true;\n"
          << "}\n\n"
          << "virtual void disableCacheLineData(unsigned set, unsigned way) {\n"
          << "_hasData[set][way] = false;\n"
          << "}\n\n"
          << "virtual bool cacheLineHasData(unsigned set, unsigned way) const {\n"
          << "return _hasData[set][way];\n"
          << "}\n"
          << "\n"
          << "  // ADL cache interface\n"
          << "  virtual void operationBegin(uadl::MemoryRequest &request);\n"
          << "  virtual void operationEnd(const uadl::MemoryRequest &request) const;\n"
          << "  virtual void clearValid(unsigned set, unsigned way);\n"
          << "\n"
          << "private:\n"
          << "  " << _uname << " &_uarch;\n"
          << "\n"
          << "  HasData _hasData;\n"
          << "};\n"
          << "\n";

      // The constructor function.
      const adl::Cache &adl_cache = *_core.get_cache(cache.name());
      const MemoryBase &mem = *dynamic_cast<const MemoryBase*>(_res.findResourceByName(cache.nextLevelMemory()));

      // Create an object to store any configuration parameters.
      Ptree *config = AdlGenSym();
      _oc << "static CacheConfigMap::value_type " << config << "[] = {\n";
      bool first = true;
      int ccount = 0;
      ForEach(cache.config(),i) {
        pfx_sep(_oc,",\n",first);
        _oc << "CacheConfigMap::value_type(\"" << i->first << "\",CacheConfigItem(";
        if (i->second.is_str()) {
          _oc << "\"" << i->second.str() << "\"";
        } else {
          _oc << i->second.val();
        }
        _oc << "))";
        ++ccount;
      }
      _oc << "\n};\n\n"

          << ct << "::" << ct << "(" << _uname << " &uarch, UadlMemoryIf &memory) :\n"
          << basename << "(\n"
          << "    uarch,\n"
          << "    \"" << cache.name() << "\",\n"
          << "    memory,\n"
          << "    " << mem.findPortByName(cache.nextLevelMemoryPort())->id() << ",\n"
          << "    " << adl_cache.linesize() << ",\n"
          << "    " << cache.addrBandwidth() <<            ", // address bandwidth\n"
          << "    " << cache.dataBandwidth() <<            ", // data bandwidth\n"
          << "    " << cache.maxRequest() <<               ", // max requests\n"
          << "    " << cache.allowBusWait() <<             ", // allow bus wait\n"
          << "    " << cache.preemptive() <<               ", // pre-emptive\n"
          << "    " << "CacheConfigMap(&" << config << "[0],&" << config << "[" << ccount << "]),\n"
          << "    " << cachesHaveData() <<               "  // with-data\n"
          << "  ),\n"
          << "  _uarch(uarch)\n"
          << "{\n";
      genMemPortsDef(_oc,cache,false);
      _oc << "}\n\n";

      // Called before each simulation cycle.
      //const adl::Core &core = *IR.archIR().get_core(IR.archName());
      const gc_string leafPath = getPathToLeaf(_thread);
      const gc_string ISSCache = leafPath + issCacheName(cache.name());

      _oc << "void " << ct << "::reset_has_data()\n"
          << "{\n"
          << "if (_uarch.hadReset()) {\n"
          << "// set up data array status if this is the first time step\n"
          << "unsigned num_sets = _uarch._iss->" << ISSCache << "_get_sets();\n"
          << "unsigned num_ways = _uarch._iss->" << ISSCache << "_get_ways();\n"
          << "_hasData = HasData(num_sets,BVect(num_ways,false));\n"
          << "for (unsigned set = 0; set < num_sets; set++) {\n"
          << "for (unsigned way = 0; way < num_ways; way++) {\n"
          << "_hasData[set][way] = _uarch._iss->" << ISSCache << "._array[set][way].valid;\n"
          << "}\n"
          << "}\n"
          << "}\n"
          << "}\n\n"

          << "bool " << ct << "::lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const\n"
          << "{\n"
        // NOTE: always use CacheLogRead
          << "const bool hit = _uarch._iss->" << ISSCache << "_find(adl::CacheLogRead, (int&)set, (int&)way, addr);\n"
          << "if (hit) offset = _uarch._iss->" << ISSCache << "_get_offset(addr);\n"
          << "return hit;\n"
          << "}\n\n";

      if (cachesHaveData()) {
        _oc << "byte_t* " << ct << "::cachePosition(unsigned set, unsigned way, unsigned offset) const\n"
            << "{\n"
            << "return _uarch._iss->" << ISSCache << "_data_array(set, way) + offset;\n"
            << "}\n\n";
      }

      _oc << "addr_t " << ct << "::getTagAddr(unsigned set, unsigned way)\n"
          << "{\n"
          << "return _uarch._iss->" << ISSCache << "_get_tag(set, way);\n"
          << "}\n\n"

          << "void " << ct << "::resize() {\n"
          << "unsigned num_sets = _uarch._iss->" << ISSCache << "_get_sets();\n"
          << "unsigned num_ways = _uarch._iss->" << ISSCache << "_get_ways();\n"
          << "_hasData = HasData(num_sets,std::vector<bool>(num_ways));\n"
          << "}\n"
          << "\n"

      // operationBegin()
          << "void " << ct << "::operationBegin(uadl::MemoryRequest &request)\n"
          << "{\n"
          << "Cache::operationBegin(request);\n"
          << "\n"
          << "if (_uarch._iss->" << ISSCache << "_begin(\n"
          << "        request.type, request.action,\n"
          << (_config._safeMode ?
              "        request.hint._cacheStatus,\n"
              :
              "        adl::CacheStatus(),\n")
          << "        request.set, request.way, request.addr, request.replaceLine,\n"
          << "        hasCommand()\n"
          << "    ))\n"
          << "{\n"
          << "request.offset = _uarch._iss->" << ISSCache << "_get_offset(request.addr);\n"
          << "} else {\n"
          << "request.action = adl::CacheNone;\n"
          << "}\n"
          << "request.lookupDone = true;\n"
          << "}\n\n"

      // operationEnd()
          << "void " << ct << "::operationEnd(const uadl::MemoryRequest &request) const\n"
          << "{\n"
          << "if (request.action != adl::CacheNone) {\n"
          << "_uarch._iss->" << ISSCache << "_end(request.type,request.action, request.set, request.way,request.addr, hasCommand());\n"
          << "}\n"
          << "}\n\n"
          << "\n"
          << "void " << ct << "::clearValid(unsigned set, unsigned way)\n"
          << "{\n"
          << "_uarch._iss->" + ISSCache + "._array[set][way].valid = false;\n"
          << "}\n\n";
      
      // preemptFetch()
      if (cache.preemptive()) {
        _oc << "void " << ct << "::preemptFetch()\n"
            << "{\n"
            << "_uarch." << _fname << ".preemptFetch();\n"
            << "}\n\n";
      }
      
      // linefillWidth()
      if (auto e =  cache.linefillWidth()) {
        _oc << "unsigned " << ct << "::linefillWidth(addr_t ea,addr_t ra)\n"
            << "{\n"
            << "return _uarch._iss->" << leafPath << e().name() << "(ea,ra) / 8;\n"
            << "}\n\n";
      }

      // storeBufEnabled()
      if (auto e =  cache.storeBufEnable()) {
        _oc << "bool " << ct << "::storeBufEnabled()\n"
            << "{\n"
            << "return _uarch._iss->" << leafPath << e().name() << "();\n"
            << "}\n\n";
      }

      // is_write_through()
      _oc << "bool " << ct << "::is_write_through(addr_t addr)\n"
          << "{\n"
          << "return _uarch._iss->" << leafPath << cache.name() << ".is_write_through(adl::CacheWrite,addr);\n"
          << "}\n\n";
    }

    // Create all cache objects.
    void genCacheObjs()
    {
      ForEach(_res.caches(),i) {
        const Cache &c = *i->second;
        genCacheObj(c);
      }
    }

    // Creates the ISS wrapper class.
    void genISS(const Thread &thread)
    {
      const gc_string ISSClass = getIssClassName(thread);
      const gc_string iss_type = getIssType(&thread,_config);

      if (!leaf_node(thread)) {
        genISS(*thread.threads().begin()->second);
      }

      if (!top_node(thread)) {
        // Just a hack declaration to deal with the fact that we can't forward declare nested types.
        _oh << "class " << ISSClass << ";\n\n";
        _os << "class " << ISSClass << " : public " << iss_type << " {};\n\n";
      } else {
        // The main, top-level ISS item.
        _oh << "class " << ISSClass << ";\n\n";

        _os << "struct InstrPacket : " << getIssType(&_ir.thread(),_config) << "::InstrPacket {};\n"
            << "\n"
            << "class " << ISSClass << " : public " << iss_type << " {\n"
            << "public:\n"
            << ISSClass << "(" << _uname << " &uarch,unsigned &id) :\n"
            << iss_type << "(\"" + _ir.archName() + "\", 0, id";
        if (!thread.is_group()) {
          _os << ", 0";
        }
        _os << ", &uarch.currTime())\n"
            << "{}\n"
            << "virtual ~" << ISSClass << "() {}\n"
            << "\n"
            << "};\n"
            << "\n";
      }

      if (leaf_node(thread)) {

        // Synchronize the fetch unit upon activation of the core.
        _oc << "void " << iss_type << "::synchronizeActivation()\n"
            << "{\n"
            << "_uarch->force_reset_fetch();\n"
            << "}\n\n";

        if (!_config._safeMode) {
          // Handles logging requests from the ISS, since we can have dirty data
          // within our own memory subsystem.
          _oc << "uint32_t " << iss_type << "::getMemoryData32(CacheAccess type, addr_t addr)\n" 
              << "{\n"
              << "byte_t data[4] = {0,0,0,0};\n"
              << "\n"
              << "_uarch->getMemoryData(type, addr, 4, data);\n"
              << "return byte_read32(data, 0);\n"
              << "}\n\n";
        }
        
        _oc << "void " << iss_type << "::handle_memory_request(unsigned memId, const adl::MemAccess &ma) {\n"
            << "_uarch->handle_memory_request(memId,ma);\n"
            << "};\n\n";

        if (_config._safeMode) {
          _oc << "void " << iss_type << "::enable_mem_logging(bool e) {\n"
              << "_uarch->_log_mem = e;\n"
              << "};\n\n"
              << "void " << iss_type << "::" << getBrTrgUpdateName() << "(addr_t target,unsigned size)\n"
              << "{\n"
              << "if (_uarch->is_executing(*_uarch->_cur_instr)) {\n"
              << "_uarch->set_branch_target(target);\n";
            // If we don't have a predictor, then we can safely set speculative
            // to true, because the default is false.
          if (thread.res().branchPredictors().empty()) {
            _oc << "_uarch->_speculative = true;\n";
            _oc << "_uarch->_spec_tag = _uarch->_cur_instr->iid();\n";
          }
          _oc << "}\n"
              << "}\n\n"

              << "void " << iss_type << "::" << getBrHookName() << "()\n"
              << "{\n"
              << "if (_uarch->is_executing(*_uarch->_cur_instr)) {\n";
          // For safe-mode, if we have a predictor, then we set speculative on a
          // mis-predict.  We do this here, rather than in the
          // branch-target-update function, because with a predictor we can run
          // into a corner case where we predict taken, but we don't take the
          // branch, thus the branch_target_update function is not called,
          // meaning we never set the speculative flag to true.
          if (!thread.res().branchPredictors().empty()) {
            _oc << "_uarch->_cur_instr->set_branch_finish();\n";
            // Only set speculative if we have a predictor associated with
            // fetch.  Otherwise, it's up to the user's code to handle this
            // stuff in the issue stage.
            if (!thread.res().fetcher()->branchPredictors().empty()) {
              _oc << "_uarch->_speculative = !_uarch->last_branch_prediction_correct(*_uarch->_cur_instr);\n"
                  << "_uarch->_spec_tag = _uarch->_cur_instr->iid();\n";
            }
          }
          _oc << "}\n";
          // Call the user hook, if one exists.
          if (_ir.branch_hook()) {
            _oc << "_uarch->" << getBrUserHookName() << "(*_uarch->_cur_instr);\n";
          }
          _oc << "}\n\n";
        }

        // If we're in iss-mem-mode and we are dealing with a true external
        // memory, then we create this handler to handle latency information form
        // the outside world.
        //
        // TBD:  Support for multiple memories.  Was in there, but isn't really being used,
        // so I'm not adding it back until it's needed.
        if (_config._safeMode && _config._issMemMode && _realExtMemMode) {
          _oc << "void " << iss_type << "::handle_memory_latency(unsigned memId, CacheAccess type, addr_t addr, unsigned size, const MemHandler::Latencies &latency)\n"
              << "{\n"
              << "if (!latency.empty()) {\n"
              << "uadl::UadlMemoryIf &mem = _uarch->" << getMemObj(_ir.res().memories().begin()->first) << ";\n"
              << "\n"
              << "static_cast<uadl::ExtLatencyMem&>(mem).addLatency(type, addr, size, latency);\n"
              << adl::getMemHandlerName(getGlobalMemRef()) << "->clear_latency();\n"
              << "}\n"
              << "}\n\n";
        }
      }
    }

    gc_string getCreatorFunc(const gc_string &iname)
    {
      return "get_instr_" + makeFriendlyName(iname);
    }

    gc_string getFetchExceptionFunc()
    {
      return getCreatorFunc(FetchExceptionStr);
    }

    typedef gc_hash_multimap<gc_string,pair<const InstClass *,const InstItem *> > InstrClassHash;

    void writeInstrDispatch(ostream &oc,const gc_string &icname,const InstClass &ic,const InstItem &instr,bool incr_issue_id)
    {
      if (_config._ic_enabled) {
        // If using the instruction cache, then we just use new to create any
        // new instruction objects that we need.
        oc << icname << " *ic = new " << icname << ";\n"
           << "_instr_alloc_count++;\n"
           << "ic->reset(";
        if (have_threads()) {
          oc << "this,";
        }
        oc << "_issueId" << ((incr_issue_id) ? "++" : "") << ",_commits," << _iss_type << "::" << ic.getTypeName() << ",p);\n";
      } else if (_config._vd_enabled) {
        // Virtual dispatch: Call the in-place constructor with references to
        // the static operand variables.
        oc << icname << " *ic = new (_instr_pool.alloc()) " << icname << "(";
        if (have_threads()) {
          oc << "this,";
        }
        oc << "_issueId" << ((incr_issue_id) ? "++" : "") << ",_commits," << _iss_type << "::" << ic.getTypeName() << ",p";
        gc_string oname,fname;
        ForEach(ic.operands(),i) {
          if (instr.getOperand(*i)) {
            oname = getInstrOpName(instr._name,ic.name(),*i);
          } else {
            oname = getInstrOpBaseName(ic.name(),*i);
          }
          oc << "," << getOpVar(oname);
        }
        ForEach(ic.funcs(),i) {
          if (instr.getFunc(*i)) {
            fname = getInstrOpName(instr._name,ic.name(),*i);
            oc << "," << getOpVar(fname);
          }
        }
        oc << ");\n";
      } else {
        // Template dispatch w/o instruction cache: Allocate from the
        // instruction pool, then call reset.
        oc << icname << " *ic = (" << icname << "*)_instr_pool.alloc();\n"
           << "ic->reset(";
        if (have_threads()) {
          oc << "this,";
        }
        oc << "_issueId" << ((incr_issue_id) ? "++" : "") << ",_commits," << _iss_type << "::" << ic.getTypeName() << ",p);\n";
      }

      
      // Perform mask setup here.
      const MaskMap &masks = _genic->get_mask_map();
      ForEach(instr._operands,o_iter) {
        ForEach(o_iter->second,opv_iter) {
          const Operand *op = &(*opv_iter);
          IfFind(m_iter,masks,op) {
            writeMaskInit(oc,m_iter->second,*op);
          }
        }
      }

      oc << "return ic;\n";
    }

    void writeUnimplInstrDispatch(ostream &oc,const gc_string &name)
    {
      oc << "throw unimpl_instr_exception(\"Instruction '" << name << "' is not implemented in this model.\");\n";
    }

    struct InstElm {
      gc_string        _name;
      const InstClass *_ic;
      const InstItem  *_ii;
      InstElm(const gc_string &name,const InstClass *ic,const InstItem *ii) : _name(name), _ic(ic), _ii(ii) {};
    };
    
    typedef gc_list<InstElm> IcNames;

    ostream &getIcFile(const IcNames &icnames)
    {
      if (!_config._sep_ics || icnames.empty()) {
        return _oc;
      } else {
        // Classes should be grouped together into the same file if shared by a
        // single instruction, so we can just look at the first item.
        auto iter = _icfiles.find(icnames.begin()->_ic->name());
        assert(iter != _icfiles.end());
        return *iter->second->_o;
      }
    }

    void genInstFactory(const gc_string &iname,const InstrClassHash &ic_hash,bool incr_issue_id)
    {
      gc_string iclass = getInstBaseName(_ir);
      gc_string name = getCreatorFunc(iname);

      // Do we have a class for this?  If so, use it.  Otherwise, return 0 for
      // now.  Build up a list of instruction classes and pointers to the
      // InstItem.  We use the latter for when there are operand constraints.
      IcNames icnames;
      if (!iname.empty()) {
        auto ip = ic_hash.equal_range(iname);
        for ( ; ip.first != ip.second; ++ip.first) {
          auto ic = ip.first->second.first;
          auto instItem = ip.first->second.second;
          gc_string icbase = getInstClassName(*ic);
          gc_string icname = icbase;
          if (!(ic->operands().empty() && ic->funcs().empty()) && !_config._vd_enabled) {
            icname += "<";
            bool first = true;
            ForEach((ic->operands()),o_iter) {
              list_sep(icname,first);
              if (instItem->getOperand(*o_iter)) {
                icname += getInstrOpName(iname,ic->name(),*o_iter);
              } else {
                icname += getDummyOpName();
              }
            }
            ForEach((ic->funcs()),f_iter) {
              list_sep(icname,first);
              if (instItem->getFunc(*f_iter)) {
                icname += getInstrOpName(iname,ic->name(),*f_iter);
              }
            }
            icname += ">";
          }
          icnames.push_back(InstElm(icname,ic,instItem));
        }
      }

      ostream &oc = getIcFile(icnames);

      // For each instruction, write the creator function.  I hate having to
      // use a void* as an argument, but I can't forward declare the
      // instruction packet b/c it's a nested type.  Ugh.
     
      _oh << iclass << " *" << name << "(void *);\n";
      
      oc << "// Factory function for instruction \"" << iname << "\".\n"
         << iclass << " *" << _uname << "::" << name << "(void *x)\n"
         << "{\n"
         << "InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);\n";
      if (icnames.empty()) {
        writeUnimplInstrDispatch(oc,name);
      } else {
        const InstElm *default_ic = 0;
        ForEach (icnames,ic_iter) {
          const gc_string &icname   = ic_iter->_name;
          const InstClass *ic       = ic_iter->_ic;
          const InstItem  *instItem = ic_iter->_ii;
          bool has_opvs = false;
          if (instItem && instItem->has_opvs()) {
            // We have operand-value constraints, so list them here and
            // conditionalize creation based upon them.
            has_opvs = true;
            bool first = true;
            oc << "if (";
            ForEach( (instItem->_opvalues), o_iter) {
              pfx_sep(oc," && ",first);
              oc << "(_iss->get_" << o_iter->first << "(p->_opc[0]) == " << dec << o_iter->second << ")";
            }
            oc << ") {\n";
          } else {
            // We have to handle the default case last.
            default_ic = &(*ic_iter);
            continue;
          }
          writeInstrDispatch(oc,icname,*ic,*instItem,incr_issue_id);
          if (has_opvs) {
            oc << "}\n";
          }
        }
        // If a default case exists, then it's listed last.  Otherwise, we have
        // to throw an unimplemented-instruction exception.
        if (default_ic) {
          writeInstrDispatch(oc,default_ic->_name,*default_ic->_ic,*default_ic->_ii,incr_issue_id);
        } else {
          writeUnimplInstrDispatch(oc,name);
        }
      }
      oc << "}\n\n";
    }

    // Create the factory functions which create instruction-class objects.
    void genInstFactories()
    {
      const InstrHash &instrs = _core.instrs();

      // First, for easy lookup, construct a hash mapping instruction names to
      // instruction classes.  This is a multi-map, since an instruction might
      // have operand constraints which map it to one class, and then a default
      // version in another class.
      InstrClassHash ic_hash;
      ForEach(_ir.instClasses(),i) {
        const InstClass &ic = *(i->second);
        ForEach(ic.instrs(),i_iter) {
          ic_hash.insert(make_pair(i_iter->_name,make_pair(&ic,&(*i_iter))));
        }
      }

      _oh << "//\n"
          << "// Instruction creation factory functions.\n"
          << "//\n";
      ForEach(instrs,i) {
        auto instr = i->second;
        if (!instr->is_used()) {
          continue;
        }
        genInstFactory(instr->name(),ic_hash,true);
      }
      // For the decode-miss creator.
      genInstFactory(DecodeMissStr,ic_hash,true);
      
      if (_ir.hasExceptions()) {
        genInstFactory(FetchExceptionStr,ic_hash,false);
      }

      _oh << "\n";
    }

    void add_shared_ref(const gc_string &type,const gc_string &var)
    {
      _cd._member_vars.push_back(type + " &" + var);
      gc_string cparm = get_ctor_parm(var);
      _cd._constr_inits.push_back(var + "(" + cparm + ")");
      _cd._constr_args.push_back(type + " &" + cparm);

      if (!leaf_node()) {
        _child_args.push_back(var);
      }
    }

    // Create the pipeline stages in the design.
    void genStages()
    {
      gc_string tuname = getuArchClassName(_ir.thread());

      ForEach(_res.pipelineStages(),i) {
        const PipelineStage &stage = *(i->second);
        gc_ostringstream ss;
        ss << "PipelineStage<" << stage.size() << "," << stage.rigid() << "," << tuname << "," 
           << getInstBaseName(_ir) << "," << have_threads() << ">";
        gc_string type = ss.str();
        gc_string name = tmName(stage.name());

        if (!is_shared(stage)) {
          _cd._member_vars.push_back(type + " " + name);
          _cd._constr_inits.push_back(name + "(getPath(_iss,\".\") + \"" + name + "\")");
          _cd._reset_stmts.push_back(name + ".reset()");
          _child_args.push_back(name);
        } else {
          add_shared_ref(type,name);
        }
      }
      _oh << "\n";
    }

    // Create the data structure which stores instruction class attributes.
    void genIcAttrs()
    {
      gc_string cname = getInstBaseName(_ir);

      auto const &ic_attrs = InstClass::get_ic_attrs();
      if (ic_attrs.empty()) {
        _oh << "static bool instrClassHasAttr(unsigned id,unsigned a) { return false; }\n\n";
      } else {
        _cd._member_vars.push_back("static unsigned _ic_attrs[]");
        _oh << "static bool instrClassHasAttr(unsigned id,unsigned a) {\n"
            << "return _ic_attrs[id] & a;\n"
            << "}\n\n";

        _oc << "unsigned " << _uname << "::_ic_attrs[] = {\n"
            << "0, // unknownClass\n";
        // Id of 0 is for the unknown class.  Instruction class id's are
        // implicit, using their order in the data model.
        ForEach (_ir.instClasses(),i) {
          const InstClass &ic = *i->second;
          auto const &attrs = ic.get_attrs();
          unsigned v = 0;
          ForEach(attrs,a) {
            IfFind(av,ic_attrs,*a) {
              v |= av->second;
            }
          }
          _oc << hex << "0x" << v << ", // " << ic.getTypeName() << "\n";
        }
        _oc << dec << "0, // maxClass\n"
            << "};\n\n";
      }

      // Then create the string-lookup function.
      _oh << "// Returns 0 if attribute not found.\n"
          << "static unsigned getInstrClassAttrByName(const std::string &);\n\n";

      _oc << "unsigned " << _uname <<"::getInstrClassAttrByName(const std::string &attr)\n"
          << "{\n";
      // This is intended only for use at startup, so nothing fancy in terms of implementation...
      auto const &attrs = InstClass::get_ic_attrs();
      ForEach(attrs,i) {
        _oc << "if (attr == \"" << i->first << "\") return 0x" << hex << i->second << ";\n" << dec;
      }
      _oc << "return 0;\n"
          << "}\n\n";
    }

    // Generate any helper functions.  We only need to do this in leaf nodes.
    void genHelpers()
    {
      if (!leaf_node()) return;

      UadlWalker walker(_ir.env(),_config._ld_okay,_config._safeMode,_ir,_iss_type);

      // First, emit any user type declarations.
      ForEach(_ir.type_decls(),i) {
        _oh << walker.translateCode(_ir.env(),*i) << "\n\n";
      }

      // The user-defined branch hook, if it exists.
      if (_config._safeMode && _ir.branch_hook()) {
        _oh << "void " << getBrUserHookName() << "(" << get_func_args(_ir.branch_hook()) << ");\n\n";
        _oc << "void " << _uname << "::" << getBrUserHookName() << "(" << get_func_args(_ir.branch_hook()) << ")\n"
            << "{\n"
            << walker.translateCode(_ir.env(),get_func_body(_ir.branch_hook()))
            << "}\n\n";
      }

      if (!_ir.helpers().empty()) {
        _oc << "//\n"
            << "// User-defined helper functions.\n"
            << "//\n\n";
      }

      // Then generate the helpers.  We have to write out declarations and
      // definitions, because the definition must be in the implementation file,
      // since that file has the include for the iss.
      ForEach(_ir.helpers(),i) {
        Ptree *helper = walker.translateCode(_ir.env(),i->second._func);

        Ptree *decl    = List(First(helper),Second(helper),Third(helper));
        Ptree *helper2 = List(First(helper),Second(helper),
                              Ptree::Make("%s::%p",_uname.c_str(),Third(helper)),
                              Nth(helper,3));

        _oh << decl << ";\n\n";
        _oc << helper2 << "\n\n";
      }
    }

    // Create the various resources in the core.
    void genResources()
    {
      genBranchPredictor();
      genCustomResources();
      genStages();
      genMemories();
      genCaches();
      genRegisters();
      genSemaphores();
      genFlags();
      genForwardingPaths();
      genFetch();
    }

    void genSparseCanAlloc(const RegFile &rf,bool with_mask)
    {
      _oh << "bool can_alloc(" << _uname << " &uarch,InstrBase &ic,Logger *logger, unsigned index, const char *msg";
      if (with_mask) {
        _oh << ", " << getMaskType(rf) << " mask";
      }
      _oh << ") const {\n"
          << "switch (index) {\n";

      ForEach( rf.entries(), i ) {
        const RegEntry &re = *i->second;
        const RegName  &rn = re.reg();
        
        if (rn) {
          _oh << "case " << i->first << ":\n"
              << "return uarch." << getRegName(*rn()) << ".can_alloc(uarch,ic,logger,";
          if (rn.has_index()) {
            _oh << rn.index() << ",";
          } else if (rn.has_child_id()) {
            _oh << "uarch.getChildId(),";
          }
          _oh << "msg";
          if (with_mask) {
            _oh << ",mask";
          }
          _oh << ");\n";
        }        
      }
      
      // If it doesn't exist, then we assume it can be allocated, since this
      // will be handled as an error condition later.
      _oh << "default:\n"
          << "return true;\n"
          << "}\n"
          << "}\n\n";
    }

    void genSparseAlloc(const RegFile &rf,const char *action,bool with_mask)
    {
      _oh << "void " << action << "(" << _uname << " &uarch, InstrBase &ic, Logger *logger, unsigned index";
      if (with_mask) {
        _oh << ", " << getMaskType(rf) << " mask";
      }
      _oh << ") const {\n"
          << "switch (index) {\n";

      ForEach( rf.entries(), i ) {
        const RegEntry &re = *i->second;
        const RegName  &rn = re.reg();
        
        if (rn) {
          _oh << "case " << i->first << ":\n"
              << "uarch." << getRegName(*rn()) << "." << action << "(uarch,ic,logger";
          if (rn.has_index()) {
            _oh << "," << rn.index();
          } else if (rn.has_child_id()) {
            _oh << ",uarch.getChildId()";
          }
          if (with_mask) {
            _oh << ",mask";
          }
          _oh << ");\n"
              << "break;\n";
        }        
      }
      
      _oh << "}\n"
          << "}\n\n";
    }

    void genSparseLastAlloc(const RegFile &rf)
    {
      _oh << "tag_t last_allocator(" << _uname << " &uarch, unsigned index) const {\n"
          << "switch (index) {\n";

      ForEach( rf.entries(), i ) {
        const RegEntry &re = *i->second;
        const RegName  &rn = re.reg();
        
        if (rn) {
          _oh << "case " << i->first << ":\n"
              << "uarch." << getRegName(*rn()) << ".last_allocator(uarch,index);\n"
              << "break;\n";
        }        
      }
      _oh << "}\n"
          << "return InvalidTag;\n"
          << "}\n\n";
    }

    void genSparseGetValidMask(const RegFile &rf)
    {
      _oh << "mask_t rm_later_partial_alloc(" << _uname << " &uarch, unsigned index,mask_t mask,tag_t tag) const {\n"
          << "switch (index) {\n";

      ForEach( rf.entries(), i ) {
        const RegEntry &re = *i->second;
        const RegName  &rn = re.reg();
        
        if (rn) {
          _oh << "case " << i->first << ":\n"
              << "return uarch." << getRegName(*rn()) << ".rm_later_partial_alloc(uarch,index,mask,tag);\n";
        }        
      }
      _oh << "}\n"
          << "return 0;\n"
          << "}\n\n";
    }

    void genSparseName(const RegFile &rf)
    {
      _oh << "std::string name(" << _uname << " &uarch,unsigned index) const {\n"
          << "switch (index) {\n";
      ForEach(rf.entries(),i) {
        const RegEntry &re = *i->second;
        const RegName  &rn = re.reg();
        
        if (rn) {
          _oh << "case " << i->first << ":\n return ";
          if (rn.has_index()) {
            _oh << "uarch." << getRegName(*rn()) << ".name(uarch," << rn.index() << ");\n";
          } else if (rn.has_child_id()) {
            _oh << "uarch." << getRegName(*rn()) << ".name(uarch,uarch.getChildId());\n";
          } else {
            _oh << "uarch." << getRegName(*rn()) << ".name(uarch,0);\n";
          }
        }        
      }
      _oh << "}\n"
          << "return \"" << rf.name() << "\";\n"
          << "}\n\n";
    }

    void genSparseId(const RegFile &rf)
    {
      _oh << "RegId id(" << _uname << " &uarch,unsigned index) const {\n"
          << "switch (index) {\n";

      ForEach( rf.entries(), i ) {
        const RegEntry &re = *i->second;
        const RegName  &rn = re.reg();
        
        if (rn) {
          _oh << "case " << i->first << ":\n return ";
          if (rn.has_index()) {
            _oh << "uarch." << getRegName(*rn()) << ".id(uarch," << rn.index() << ");\n";
          } else if (rn.has_child_id()) {
            _oh << "uarch." << getRegName(*rn()) << ".id(uarch,uarch.getChildId());\n";
          } else {
            _oh << "uarch." << getRegName(*rn()) << ".id(uarch,0);\n";
          }
        }        
      }
      _oh << "}\n"
          << "return RegInvalidId;\n"
          << "}\n\n";

    }
    
    typedef gc_map<gc_string,gc_string> CplxTypes;

    // Generates a sparse-register-file object.  Currently, this is implemented
    // as switch statements to select the actual element.  We might want to
    // experiment, in the future, with alternatives, such as an array of objects
    // w/virtual functions, etc.
    void genSparseRegFile(const RegFile &rf,CplxTypes &cplxTypes)
    {
      gc_string rtype = getSparseType(rf);

      _oh << "struct " << rtype << " {\n"
          << "const char *name() const { return \"" << rf.name() << "\"; }\n\n";

      genSparseName(rf);

      genSparseId(rf);

      genSparseCanAlloc(rf,false);
      genSparseCanAlloc(rf,true);

      genSparseAlloc(rf,"allocate",false);
      genSparseAlloc(rf,"allocate",true);

      genSparseLastAlloc(rf);
      genSparseGetValidMask(rf);

      genSparseAlloc(rf,"deallocate",false);
      genSparseAlloc(rf,"deallocate",true);
        
      _oh << "};\n\n";

      // Insert an entry, specifying whether the thread-id is needed or not.
      cplxTypes.insert(make_pair(rf.name(),rtype));
    }

    void genAliasCanAlloc(const RegFile &rf,const RegFileHook &rh,bool with_mask)
    {
      _oh << "bool can_alloc(" << _uname << " &uarch,InstrBase &ic, Logger *logger, unsigned index, const char *msg";
      if (with_mask) {
        _oh << ", " << getMaskType(rf) << " mask";
      }
      _oh << ") const {\n"
          << "assert(index < MaxIndex);\n"
          << "return uarch." << getRegName(*rh.alias()()) << ".can_alloc(uarch,ic,logger,_indices[index],msg";
      if (with_mask) {
        _oh << ",mask";
      }
      _oh << ");\n"
          << "}\n\n";
    }

    void genAliasAlloc(const RegFile &rf,const RegFileHook &rh,const char *action,bool with_mask)
    {
      _oh << "void " << action << "(" << _uname << " &uarch, InstrBase &ic, Logger *logger, unsigned index";
      if (with_mask) {
        _oh << ", " << getMaskType(rf) << " mask";
      }
      _oh << ") const {\n"
          << "assert(index < MaxIndex);\n"
          << "uarch." << getRegName(*rh.alias()()) << "." << action << "(uarch,ic,logger,_indices[index]";
      if (with_mask) {
        _oh << ",mask";
      }
      _oh << ");\n"
          << "}\n\n";
    }

    void genAliasLastAlloc(const RegFile &rf,const RegFileHook &rh)
    {
      _oh << "tag_t last_allocator(" << _uname << " &uarch, unsigned index) const {\n"
          << "return uarch." << getRegName(*rh.alias()()) << ".last_allocator(uarch,_indices[index]);\n"
          << "}\n\n";
    }

    void genAliasGetValidMask(const RegFile &rf,const RegFileHook &rh)
    {
      _oh << "mask_t rm_later_partial_alloc(" << _uname << " &uarch, unsigned index,mask_t mask,tag_t tag) const {\n"
          << "return uarch." << getRegName(*rh.alias()()) << ".rm_later_partial_alloc(uarch,_indices[index],mask,tag);\n"
          << "}\n\n";
    }

    // Aliased indices, where one register file maps to different indices of
    // another file really just matters for reads, since we can always write (we
    // don't model WAW hazards right now).
    void genAliasedRegFile(const RegFile &rf,CplxTypes &cplxTypes)
    {
      gc_string rtype = getSparseType(rf);
      cplxTypes.insert(make_pair(rf.name(),rtype));

      auto const &rh = *rf.readhook();
      auto const &mapping = rh.aliasindices();

      _oh << "struct " << rtype << " {\n"
          << "enum { MaxIndex = " << dec << mapping.size() << " };\n\n"
          << "const char *name() const { return \"" << rf.name() << "\"; };\n\n"
          << "std::string name(" << _uname << " &uarch,unsigned index) const { return uarch." << getRegName(*rh.alias()()) << ".name(uarch,_indices[index]); };\n\n"
          << "RegId id(" << _uname << " &uarch,unsigned index) const { return uarch." << getRegName(*rh.alias()()) << ".id(uarch,_indices[index]); };\n\n";

      genAliasCanAlloc(rf,rh,false);
      genAliasCanAlloc(rf,rh,true);

      genAliasAlloc(rf,rh,"allocate",false);
      genAliasAlloc(rf,rh,"allocate",true);

      genAliasLastAlloc(rf,rh);
      genAliasGetValidMask(rf,rh);

      genAliasAlloc(rf,rh,"deallocate",false);
      genAliasAlloc(rf,rh,"deallocate",true);
      
      _oh << "const char      *_name;\n"
          << "static unsigned _indices[];\n"
          << "};\n\n";

      _oc << "unsigned " << _uname << "::" << rtype << "::_indices[] = { ";
      bool first = true;
      ForEach(mapping,i) {
        list_sep(_oc,first);
        _oc << *i;
      }
      _oc << " };\n\n";
    }

    void genComplexRegFiles(CplxTypes &cplxTypes)
    {
      ForEach(_res.registerFiles(),i) {
        const RegisterFile &rf = *i->second;
        if (!rf.is_reg() && !is_shared(rf)) {
          const RegFile *regfile = _core.get_regfile(rf.name());
          assert(regfile);
          if (regfile->sparse()) {
            genSparseRegFile(*regfile,cplxTypes);
          }
          else if (regfile->has_readhook() && !regfile->readhook()->aliasindices().empty()) {
            genAliasedRegFile(*regfile,cplxTypes);
          }
        }
      }
    }

    void genRegisters()
    {
      CplxTypes cplxTypes;

      genComplexRegFiles(cplxTypes);

      gc_ostringstream ss;

      _max_reg_id = 0;
      ForEach(_res.registerFiles(),i) {
        const RegisterFile &r = *i->second;
        if (r.is_reg()) {
          const Reg &reg = *_core.get_reg(r.name());

          _max_reg_id = max(_max_reg_id,reg.id());
          gc_string rname = getRegName(reg);
          gc_string rtype = "RegResource<" + getMaskType(reg) + ">";
          
          if (!is_shared(r)) {
            _cd._member_vars.push_back(rtype + " " + rname);
            _cd._reset_stmts.push_back(rname + ".reset()");
            _cd._constr_inits.push_back(rname + "(\"" + reg.name() + "\")");
            _child_args.push_back(rname);
          } else {
            add_shared_ref(rtype,rname);
          }
        } else {
          const RegFile &rf = *_core.get_regfile(r.name());

          _max_reg_id = max(_max_reg_id,rf.id());
          gc_string rname = getRegName(rf);
          gc_string rtype;

          bool shared = is_shared(r);
          bool cmplx = false;

          IfFind(x,cplxTypes,rf.name()) {
            rtype = x->second;
            cmplx = true;
          } else {
            ss << "RegFileResource<" << getMaskType(rf) << "," << rf.size() << ">";
            rtype = ss.str();
            ss.str("");
          }

          if (!shared) {
            _cd._member_vars.push_back(rtype + " " + rname);          
            if (!cmplx) {
              _cd._reset_stmts.push_back(rname + ".reset()");
              _cd._constr_inits.push_back(rname + "(\"" + rf.name() + "\")");
            }
            _child_args.push_back(rname);
          } else {
            add_shared_ref(rtype,rname);
          }
        }
      }
    }

    void genForwardingPaths()
    {
      ForEach (_res.forwardingPaths(),i) {
        auto const &fp = *i->second;

        const gc_string fn = tmName(fp.name());
        const gc_string ft = getForwardingPathType(_ir,_config._safeMode,fp);

        if (!is_shared(fp)) {
          _cd._member_vars.push_back(ft + " " + fn);

          gc_ostringstream ss;
          ss << fn << "(\"" << fp.name() << "\")";
          _cd._constr_inits.push_back(ss.str());
          
          _cd._reset_stmts.push_back(fn + ".reset()");
          
          _post_stmts.push_back(fn + ".postCycle()");

          _child_args.push_back(fn);
        } else {
          add_shared_ref(ft,fn);
        }
      }
    }

    void genSemaphores()
    {
      ForEach (_res.semaphores(),i) {
        const Semaphore &s = *i->second;

        const gc_string sn = tmName(s.name());
        const gc_string st = getSemaphoreType();

        if (!is_shared(s)) {
          _cd._member_vars.push_back(st + " " + sn);

          gc_ostringstream ss;
          ss << sn << "(\"" << s.name() << "\"," << s.count() << ")";
          _cd._constr_inits.push_back(ss.str());
          
          _cd._reset_stmts.push_back(sn + ".reset()");

            _child_args.push_back(sn);
        } else {
          add_shared_ref(st,sn);
        }
      } 
    }

    void genFlags()
    {
      ForEach (_res.flags(),i) {
        const Flag &f = *i->second;

        const gc_string fn = tmName(f.name());
        const gc_string ft = getFlagType();

        if (!is_shared(f)) {
          _cd._member_vars.push_back(ft + " " + fn);

          gc_ostringstream ss;
          ss << fn << "(\"" << f.name() << "\")";
          _cd._constr_inits.push_back(ss.str());
          
          _cd._reset_stmts.push_back(fn + ".reset()");

          _child_args.push_back(fn);
        } else {
          add_shared_ref(ft,fn);
        }
      }
    }

    void genCaches()
    {
      RevForEach(_res.sortedCaches(),i) {
        const Cache &c = **i;

        const gc_string ct = getCacheType(c.name());
        const gc_string cn = getMemObj(c);

        const gc_string in = getInstBaseName(_ir);

        const gc_string wname  = tmName(c.name());
          
        const gc_string wbase  = (c.custom_model()) ? c.custom_class() : "Cache";
        const gc_string wtype  = wbase + "T<" + ct + "," + _uname + "," + in + "," + (_config._safeMode ? "true" : "false") + ">";

        if (!is_shared(c)) {

          _cd._member_vars.push_back(ct + " " + cn);
          _cd._constr_inits.push_back(cn + "(*this," + getMemObj(c.nextLevelMemory()) + ")");
          _cd._reset_stmts.push_back(cn + ".reset()");
                  
          _run_stmts.push_back(cn + ".reset_has_data()");
          _pre_stmts.push_back(cn + ".preCycle()");

          _child_args.push_back(cn);
        } else {
          add_shared_ref(ct,cn);
        }

        if (leaf_node()) {
          _cd._member_vars.push_back(wtype + " " + wname);                                          
          _cd._constr_inits.push_back(wname + "(" + cn + ")");
        }
      }

      ForEach(_res.sortedCaches(),i) {
        const Cache &c = **i;

        if (!is_shared(c)) {
            const gc_string cn = getMemObj(c);

            _post_stmts.push_back(cn + ".postCycle()");
            _post2_stmts.push_back("if (logMemory()) " + cn + ".logAllRequests()");
            _post_sim_stmts.push_back(cn + ".postSim()");
        }
      }

    }

    void genMemories()
    {
      gc_string cname = getInstBaseName(_ir);
      const gc_string instrMem = getMemObj(*_ir.firstLevelInstMem());

      // Memory
      ForEach (_res.memories(), i) {
        const gc_string &mem   = getMemObj(*i->second);
        const gc_string &mtype = getMemoryType(i->second->name());
        const gc_string wname  = tmName(i->second->name());
        const gc_string wtype  = getMemWrapType(i->second->name());

        if (!is_shared(*i->second)) {
          _cd._member_vars.push_back(mtype + " " + mem);
          
          if (!_config._extMemMode) {
            _cd._constr_inits.push_back(mem + "(*this, logWarnings())");
          } else {
            _cd._constr_inits.push_back(mem + "(memory)");
          }
                   
          _cd._reset_stmts.push_back(mem + ".reset()");
          
          if (!_config._extMemMode || _config._extCycleMem) {
            _pre_stmts.push_back(mem + ".preCycle()");
            _post_stmts.push_back(mem + ".postCycle()");
            _post2_stmts.push_back("if (logMemory()) " + mem + ".logAllRequests()");
          }

          _child_args.push_back(mem);
        } else {
          add_shared_ref(mtype,mem);
        }

        // We only need the wrapper for the leaf-node objects, since that's what
        // instructions act upon.
        if (leaf_node()) {
          _cd._member_vars.push_back(wtype + " " + wname);
          _cd._constr_inits.push_back(wname + "(" + mem + ")");
        }
      }

      if (leaf_node()) {
        const adl::Cache *L1d = 0;
        if (_ir.firstLevelDataMem() && _ir.firstLevelDataMem()->isCache()) {
          L1d = _core.get_cache(_ir.firstLevelDataMem()->name());
        }

        // Will store memory transactions.
        unsigned bufSize = 2 * sizeof(uint64_t);
        if (L1d && L1d->linesize() > bufSize) {
          bufSize = L1d->linesize();
        }

        bool have_data_caches = _core.has_caches(DataCache);

        _oh << "typedef adl::PacketPool<UadlMemAccess," << dec << _config._memPoolSize << "> MemAccessPool;\n\n"
            << "void alloc_mem_access_data();\n\n";
        _oc << "void " << _uname << "::alloc_mem_access_data()\n"
            << "{\n";
        if (!_config._safeMode) {
          // We don't store data if we're in safe-mode, since it's not needed.
          _oc << "for (MemAccessPool::iterator iter = _mem_access_pool.begin(); iter != _mem_access_pool.end(); ++iter) {\n"
              << "(*iter)->alloc(" << bufSize << ");\n"
              << "}\n";
        } else if (have_data_caches) {
          // If we have data caches and we're in safe-mode, then allocate cache hint information.
          _oc << "for (MemAccessPool::iterator iter = _mem_access_pool.begin(); iter != _mem_access_pool.end(); ++iter) {\n"
              << "(*iter)->_hint = new CacheStatusHint;\n"
              << "}\n";
        }
        _oc << "}\n\n";

        // Size is a guess- maybe we'll need to increase this at some point.
        _cd._member_vars.push_back("MemAccessPool _mem_access_pool");
        _cd._reset_stmts.push_back("_mem_access_pool.reset()");
        _cd._constr_stmts.push_back("alloc_mem_access_data()");

        // This adds memory requests to the instruction.
        _oh << "UadlMemAccess &add_memory_request(InstrBase &instr);\n\n";
        _oc << "UadlMemAccess &" << _uname << "::add_memory_request(InstrBase &instr)\n"
            << "{\n"
            << "return *instr.mem();\n"
            << "}\n\n";

        // This sets a memory result.
        _oh << "void set_memory_result(InstrBase &instr);\n\n";
        _oc << "void " << _uname << "::set_memory_result(InstrBase &instr)\n"
            << "{\n";
        if (!_config._safeMode) {
          // Not used in safe-mode, because memory is handled immediately.
          _oc << "UadlMemAccess *ma = instr.mem();\n"
              << "_iss->set_memory_result(0,ma->data(),ma->offset());\n";
        }
        _oc << "}\n\n";
           
        _oh << "void free_mem(InstrBase &instr);\n\n";
        _oc << "void " << _uname << "::free_mem(InstrBase &instr)\n"
            << "{\n"
            << "if (instr.mem()) {\n"
            << "UadlMemAccess *ma = instr.get_mem_and_clear();\n";
        _oc << "_mem_access_pool.free(ma);\n"
            << "}\n"
            << "}\n\n";

        _oh << "void free_all_mem(InstrBase &instr);\n\n";
        _oc << "void " << _uname << "::free_all_mem(InstrBase &instr)\n"
            << "{\n"
            << "if (instr.mem()) {\n"
            << "_mem_access_pool.free_all(instr.mem(),instr.mtail());\n"
            << "}\n"
            << "}\n\n";

        // Generate the handler for doing the real memory access.  In safe-mode,
        // this handles the complete memory request.  In normal mode, this just
        // records the request.
        _cd._member_vars.push_back("bool _log_mem");
        _cd._reset_stmts.push_back("_log_mem = true");
        _cd._constr_inits.push_back("_log_mem(true)");

        _oh << "void enableMemLogging(bool e) { _log_mem = e; };\n\n";

        gc_string cur_mem = tmName(_res.memories().begin()->first);

        _oh << "void handle_memory_request(unsigned memId,const adl::MemAccess &m);\n";
        _oc << "void " << _uname << "::handle_memory_request(unsigned memId,const adl::MemAccess &m)\n"
            << "{\n"
            << "if (!_log_mem";
        if (!_config._safeMode) {
          _oc << " || _ignore_mem";
        }
        _oc << ") return;\n"
            << "UadlMemAccess *ma = _mem_access_pool.alloc();\n";
        if (have_data_caches && _config._safeMode) {
          _oc << "CacheStatusHint *hint = ma->_hint;\n"
              << "_iss->update_data_cache_status(hint->_cacheStatus,m._type,m._ra);\n";
          if (L1d) {
            _oc << "hint->_storeBufEnabled = " << getMemObj(L1d->name()) << ".storeBufEnabled();\n";
          }
        }
        _oc << "ma->reset(_memId++,m);\n";
        if (_ir.mem_access()) {
          // Add a call to the memory-access hook, if it exists.
          _oc << "_iss->" << _ir.mem_access()._d->name() << "(*ma);\n";
        }
        _oc << "_cur_instr->add_mem(ma);\n"
            << "}\n\n";

        if (!_config._safeMode) {
          // For logging: Examine our memory hierarchy for data, including the data
          // potentially attached to an instruction.
          _oh << "void getMemoryData(byte_t *data,UadlMemAccess *ma,addr_t addr,unsigned size);\n";

          // The use of max() for computing s is a little weird: The reason is
          // that _nb normally stores the precise number of bytes for a write,
          // which can be smaller than the size if it's a misaligned store (and
          // thus modifying just a part of a word.  However, the user can modify
          // _nb, e.g. to group writes, meaning that more data may be available in
          // this memory access than we would think.
          _oc << "void " << _uname << "::getMemoryData(byte_t *data,UadlMemAccess *ma,addr_t addr,unsigned size)\n"
              << "{\n"
              << "unsigned data_size;\n"
              << "if (!ma || !ma->has_valid_data()) return;\n\n"
              << "unsigned s;\n"
              << "if (ma->_nb > size) {\n"
              << "s = ma->_nb;\n"
              << "} else {\n"
              << "s = size;\n"
              << "}\n"
              << "if (addr >= ma->_ra && addr < ma->_ra + s) {\n"
              << "unsigned offset = addr - ma->_ra;\n"
              << "data_size = min(ma->_ra + ma->_nb - addr,(addr_t)size);\n"
              << "memcpy(data,ma->_data+offset,data_size);\n"
              << "} else if (addr < ma->_ra && addr + s > ma->_ra) {\n"
              << "unsigned offset = ma->_ra - addr;\n"
              << "data_size = min((addr_t)ma->_nb,addr + size - ma->_ra);\n"
              << "memcpy(data+offset,ma->_data+ma->_offset,data_size);\n"
              << "}\n"
              << "}\n\n";
        
          _oh << "void getMemoryData(adl::CacheAccess type, addr_t addr, unsigned size, byte_t *data);\n";
          _oc << "void " << _uname << "::getMemoryData(CacheAccess type, addr_t addr, unsigned size, byte_t *data)\n"
              << "{\n";

          // Normal mode: Any instruction might have dirty data, so we have to
          // look at everything in the pipeline.
          _oc << "if (type == adl::CacheLogRead) {\n";

          if (_ir.firstLevelDataMem()) {
            const gc_string dataMemory = getMemObj(*_ir.firstLevelDataMem());

            _oc << "// get data from memory/cache\n"
                << "if (" << dataMemory << ".getMemoryData(type, addr, size, data)) {\n"
                << "// augment with instruction memory data\n"
                << "\n";
            ForEach (_ir.sortedStages(),siter) {
              const PipelineStage &stage = **siter;
              ForRange(stage.size(),i) {
                _oc << "if (" << cname << " *instr = " << stage.name() << "[" << i << "]) {\n"
                    << "getMemoryData(data,instr->mem(),addr,size);\n"
                    << "}\n";
              }
            }
            _oc << "} else {\n"
                << instrMem << ".getMemoryData(type, addr, size, data);\n"
                << "}\n";
          }
        
          _oc << "} else {\n"
              << instrMem << ".getMemoryData(type, addr, size, data);\n"
              << "}\n"        
              << "}\n\n";
        }
      }

      if (_realExtMemMode) {
        _oh << "void setIssMemHandler(adl::MemHandler *);\n";
        _oc << "void " << _uname << "::setIssMemHandler(adl::MemHandler *memhandler)\n"
            << "{\n"
            << "_iss->setMemHandler(memhandler);\n"
            << "}\n\n";
      }
      
      // setExtMemory()
      if (_config._extMemMode) {
        _oh << "bool hasExternalMem() const { return true; }\n\n";
        
        _oh << "void setExtMemory(UadlMemoryIf &mem);\n";
        _oc << "void " << _uname << "::setExtMemory(UadlMemoryIf &mem)\n"
            << "{\n";
        if (leaf_node()) {
          ForEach(_res.memories(), i) {
            _oc << getMemObj(i->second->name()) << ".setMemory(mem);\n";
            genMemPortsDef(_oc,*i->second,true);
            genMemSetParms(_oc,*i->second);
          }
        } else {
          ForEach(_thread.threads(),i) {
            _oc << i->first << ".setExtMemory(mem);\n";
          }
        }
        _oc << "}\n\n";
        
        assert(!_res.memories().empty());
        _oh << "bool hasExtMemSet() const;\n\n";
        _oc << "bool " << _uname << "::hasExtMemSet() const\n"
            << "{\n";
        if (leaf_node()) {
          _oc << "return " << getMemObj(_res.memories().begin()->second->name()) << ".getMemory();\n";
        } else {
          _oc << "return " << _thread.threads().begin()->first << ".hasExtMemSet();\n";
        }
        _oc << "}\n\n";
      }
    }

    gc_string get_bp_datatype(const BranchPredictor &bp)
    {
      if (bp.custom()) {
        // Custom algorithm means that we use the algo name as the type.
        return bp.algorithm();
      } else {
        // Internally recognized algorithm- and prefix.
        return "BranchPredictor" + bp.algorithm();
      }
    }

    gc_string get_bp_name(const BranchPredictor &bp)
    {
      return bp.name();
    }

    gc_string get_bpdata_name(const BranchPredictor &bp)
    {
      if (bp.has_reg_compares()) {
        return bp.name() + "_data_t";
      } else {
        return "PredictData";
      }
    }

    gc_string get_compare_datatype_base(const BranchPredictor::RegCompare &r)
    {
      int size = 32;
      if (r._rf) {
        if (r._rf->width() >= 64) {
          size = 64;
        } 
      } else {
        if (r._rb->width() >= 64) {
          size = 64;
        }
      }

      return (size = 32) ? "uint32" : "uint64";
    }

    gc_string get_compare_datatype(const BranchPredictor::RegCompare &r)
    {
      return get_compare_datatype_base(r) + "_t";
    }

    gc_string get_compare_dataname(const BranchPredictor::RegCompare &r)
    {
      gc_string n = '_' + r._reg;
      if (r._rf) {
        n += '_' + r._field;
      }
      return n;
    }

    gc_string get_compare_accessor(const BranchPredictor::RegCompare &r)
    {
      return getRegRead(*r._rb,r._rf) + "." + get_compare_datatype_base(r) + "()";
    }

    gc_string get_enable_class(const BranchPredictor &bp)
    { 
      return bp.name() + "_enable_t"; 
    }
  
    gc_string get_enable_instance(const BranchPredictor &bp)
    { 
      return "_" + bp.name() + "_enable"; 
    }

    void genBranchPredictorData(const BranchPredictor &bp)
    {
      // Register compare class.
      if (bp.has_reg_compares()) {
        gc_string n = get_bpdata_name(bp);
        _oh << "struct " << n << " : public PredictData {\n";
        ForEach(bp.reg_compares(),r_iter) {
          const BranchPredictor::RegCompare &r = *r_iter;
          _oh << get_compare_datatype(r) << " " << get_compare_dataname(r) << ";\n";
        }
        _oh << "\n"
            << n << "() {};\n"
            << n << "(" << _uname << " &uarch,addr_t addr);\n\n";

        _oc << _uname << "::" << n << "::" << n << "(" << _uname << " &uarch,addr_t addr) :\n"
            << "                PredictData(uarch,addr)";

        ForEach(bp.reg_compares(),r_iter) {
          const BranchPredictor::RegCompare &r = *r_iter;
          _oc << ",\n                " << get_compare_dataname(r) << "(uarch." << get_compare_accessor(r) << ")";
        }
        _oc << "\n{};\n\n";

       _oh << "bool operator==(const " << n << " &x) const {\n"
            << "return ";
        bool first = true;
        ForEach(bp.reg_compares(),r_iter) {
          pfx_sep(_oh," && ",first);
          gc_string fn = get_compare_dataname(*r_iter);
          _oh << "(" << fn << " == x." << fn << ")";
        }
        _oh << "\n             && PredictData::operator==(x);\n"
          << "};\n"
          << "};\n\n";
      }
    }

    void genBranchPredictorDecl(const BranchPredictor &bp,unsigned id)
    {
      gc_string cname = getInstBaseName(_ir);

      gc_string e_arg;
      if (bp.enable()) {
        _cd._member_vars.push_back(get_enable_class(bp) + " " + get_enable_instance(bp));
        _cd._constr_inits.push_back(get_enable_instance(bp)+"(*this)");
        e_arg = get_enable_class(bp);
      } else {
        e_arg = "BPredEnableDefault";
      }

      gc_string bpname = get_bp_name(bp);

      gc_ostringstream ss;
      ss << get_bp_datatype(bp) << '<';
      if (bp.custom()) {
        bool first = true;
        ForEach(bp.args()._tmpl_args,j) {
          if (j->_builtin) {
            list_sep(ss,first);
            if (j->_name == "ModelType") {
              ss << _uname;
            }
            else if (j->_name == "InstrType") {
              ss << cname;
            }
            else if (j->_name == "BPredEnable") {
              ss << e_arg;
            }
            else if (j->_name == "BPData") {
              ss << get_bpdata_name(bp);
            }
          } else {
            IfFind (iter,bp.parms(),j->_name) {
              list_sep(ss,first);
              ss << iter->second;
            }
          }
        }
      } else {
        ss << _uname << "," << cname << "," << get_bpdata_name(bp) << "," << e_arg;
      }
      ss << ">";

      gc_string bptype = ss.str();

      // If we have additional register-comparison criteria, then we must construct
      // a data type to store this data.
      if (!is_shared(bp)) {
        genBranchPredictorData(bp);

        // Enable predicate class.
        if (auto e =  bp.enable()) {
          const gc_string enableClassName = get_enable_class(bp);

          _oh << "struct " << enableClassName << " {\n" 
              << enableClassName << "(" << _uname << " &uarch) : _uarch(uarch) {}\n" 
              << "bool operator()();\n"
              << "private:\n" 
              << _uname << "&_uarch;\n" 
              << "};\n\n";
        
          _oc << "bool " << _uname << "::" << enableClassName << "::operator()() { return _uarch._iss->" << e().name() << "(); }\n\n";
        }

        _cd._member_vars.push_back(bptype + " " + bpname);

        gc_ostringstream ss;
        ss << bpname << "(*this," << id << "," << (bp.enable() ? get_enable_instance(bp) : "BPredEnableDefault()");
        if (!bp.custom()) {
          ss << ","   << bp.size() << ",\n" << bp.counterWidth();
        } else {
          // Not the default type, so we specify any keys which match
          // constructor arguments.
          ForEach(bp.args()._constr_args,j) {
            IfFind (iter,bp.parms(),j->_name) {
              ss << ',' << iter->second;
            }
          }
        }
        ss << ")";
        _cd._constr_inits.push_back(ss.str());
        _cd._reset_stmts.push_back(bpname + ".reset()");
        _child_args.push_back(bpname);
      } else {
        add_shared_ref(bptype,bpname);
      }
    }

    gc_string writePredict(const gc_string &bp,
                      const gc_string &addr,const gc_string &target,
                      const gc_string &size)
    {
      gc_ostringstream ss;      
      ss << bp << ".predict(*this," << addr << "," << target << "," << size << ")";
      return ss.str();
    }

    void genBranchPredictor()
    {
      unsigned count = 0;
      ForEach(_res.branchPredictors(),i) {
        genBranchPredictorDecl(*i->second,count++);
      }

      if (leaf_node()) {
        // For more than one predictor, we only check the enable for the first,
        // but we chain together queries.
        gc_string bp = (fetch_bp()) ? tmName(_fetcher->branchPredictors().front()) : gc_string();

        _oh << "bool branchPredictEnabled() {\n";
        if (!bp.empty()) {
          _oh << "return " << bp << ".enabled();\n";
        } else {
          _oh << "return false;\n";
        }      
        _oh << "}\n\n";
        
        // Note that this function now simply calls the fetch-unit predictor, if
        // one was declared.
        _oh << "Prediction branchPredict(addr_t addr,addr_t &target, unsigned &size)\n"
            << "{\n";
        
        // We have to create this stub, since the fetch unit calls it, though if
        // no branch predictor exists, then the fetch unit won't actually call it.
        // However, the compiler still expects it to be there.
        if (!bp.empty()) {
          if (_fetcher->branchPredictors().size () > 1) {
            _oh << "Prediction p;\n";
            ForEach(_fetcher->branchPredictors(),bp_iter) {
              _oh << "p = " << writePredict(*bp_iter, "addr", "target", "&size") << ";\n";
              if (bp_iter+1 != _fetcher->branchPredictors().end()) {
                _oh << "if (p != Miss && p != NoPrediction) return p;\n";
              } else {
                _oh << "return p;\n";
              }
            }
          } else {
            _oh << "return " << writePredict(bp, "addr", "target", "&size") << ";\n";
          }
        } else {
          _oh << "return NoPrediction;\n";
        }
        _oh << "}\n\n";
      }
    }

    // For each custom resource, instantiate the class with the appropriate
    // arguments from the input file.
    void genCustomResources()
    {
      gc_string cname = getInstBaseName(_ir);

      ForEach(_res.customResources(),i) {
        const CustomResource &cr = *i->second;
        gc_string n = cr.name();

        gc_ostringstream ss;
        ss << cr.type();
        if (cr.is_template()) {
          ss << "<";
          bool first = true;
          ForEach(cr.args()._tmpl_args,j) {
            if (j->_builtin) {
              list_sep(ss,first);
              if (j->_name == "ModelType") {
                ss << _uname;
              }
              else if (j->_name == "InstrType") {
                ss << cname;
              }
            } else {
              IfFind (iter,cr.parms(),j->_name) {
                list_sep(ss,first);
                ss << iter->second;
              }
            }
          }
          ss << ">";
        }

        gc_string type = ss.str();
        
        if (!is_shared(cr)) {

          _cd._member_vars.push_back(type + " " + n);

          // Add constructor arguments if we have any set.
          ss.str("");
          ss << n << "(*this";
          ForEach(cr.args()._constr_args,j) {
            IfFind (iter,cr.parms(),j->_name) {
              ss << ',' << iter->second;
            }
          }
          ss << ")";
          _cd._constr_inits.push_back(ss.str());

          _cd._reset_stmts.push_back(n + ".reset()");

          if (cr.hasPreCycle()) {
            _pre_stmts.push_back(n + "." + PreCycleName + "()");
          }
          if (cr.hasPostCycle()) {
            _post_stmts.push_back(n + "." + PostCycleName + "()");
          }
          if (cr.hasPostSim()) {
            _post_sim_stmts.push_back(n + "." + PostSimName + "()");
          }

          _child_args.push_back(n);
        } else {
          add_shared_ref(type,n);
        }
      }
    }

    void genFetch()
    {
      // Fetch units can't currently be shared.
      if (!leaf_node()) return;

      assert(_fetcher);

      // fetch unit
      gc_ostringstream fetchUnitParas;

      fetchUnitParas << _uname << ", " << _fetcher->entrySize() 
                     << ", " << _fetcher->fetchSizes().size() 
                     << ", " << _fetcher->minFetchEntries() 
                     << ", " << _ir.minInstrSize()
                     << ", " << _ir.maxInstrSize() 
                     << ", " << _fetcher->maxFetchCount() << "ul"
                     << ", " << _ir.instrSizeGCD()
                     << ", " << fetch_bp()
                     << ", " << _fetcher->lazyFetch() 
                     << ", " << _fetcher->flushFetch()
                     << ", " << _fetcher->canCancel() 
                     << ", " << _fetcher->reuseData()
                     << ", " << _config._safeMode 
                     << ", " << _config._issMemMode;

      _cd._member_vars.push_back("DefaultFetchUnit<" + fetchUnitParas.str() + "> " + _fname);
      _cd._reset_stmts.push_back(_fname + ".full_reset()");

      // fetch unit (must after memories and branch predictors)
      const gc_string instrMem = getMemObj(*_ir.firstLevelInstMem());
      const unsigned fetchMemPort = _ir.firstLevelInstMem()->findPortByName
        (_fetcher->fetchMemoryPort())->id();

      fetchUnitParas.str("");
      fetchUnitParas << _fname << "("
                     << "*this,"
                     << "getPath(_iss,\".\")+\"" << _fetcher->name() << "\","
                     << _fetcher->nEntries() << ","
                     << instrMem << ","
                     << fetchMemPort << ","
                     << "_fetchSizes,"
                     << "&" << getMemObj(*_ir.lastLevelInstrMem())
                     << ")";
      _cd._constr_inits.push_back(fetchUnitParas.str());

      _cd._member_vars.push_back("static FetchSizes _fetchSizes");

      vector<unsigned> fetchSizes;
      ForEach(_fetcher->fetchSizes(),i) {
        fetchSizes.push_back(*i);
      }
      sort(fetchSizes.begin(),fetchSizes.end(),greater<unsigned>());
      ostream_iterator<unsigned> oiter(_oc,", ");
      _oc << "static unsigned initFetchSizes[] = { ";
      copy(fetchSizes.begin(),fetchSizes.end(),oiter);
      _oc << " };\n";
      _oc << "FetchSizes " << _uname << "::_fetchSizes(&initFetchSizes[0],&initFetchSizes[" << fetchSizes.size() <<"]);\n\n";
    }

    void genPrologue()
    {
      const gc_string headerMacroName = getHeaderMacroName(_ir);
      const gc_string prologue = getPrologue(_ir,_config);

      // BEGIN: header file prologue
      _oh << prologue
          << "#ifndef " << headerMacroName << "\n"
          << "#define " << headerMacroName << "\n"
          << "\n"
        // Note: Do not directly include ModelInterface.h because it depends upon some
        // defines done within the ISS header file.  Also, include systemc.h *after*
        // the ISS header because the SystemC header is redefining min, which screws
        // stuff up.
          << "#define UADL_MODEL_NAMESPACE " << _config._nameSpace << "\n"
          << "\n"
          << "#include <string.h>\n"
          << "#include <set>\n"
          << "#include <list>\n"
          << "#include <vector>\n"
          << "#include <string>\n"
          << "#include <iostream>\n"
          << "\n"
          << "#include \"helpers/Macros.h\"\n";

      genBitsDecls(_oh,_core.is_little_endian());

      // In debug mode, we do range-checking of the circular queues.
      if (_config._debugMode) {
        _oh << "#define QueueDebugCheck true\n\n";
      }

      _oh << "\n"
          << "#include \"iss/PacketPool.h\"\n"
          << "\n"
          << "\n"
          << "#include <uadl/" << (_config._systemc ? "UadlArchSystemC.h" : "UadlArch.h") << ">\n"
          << "#include \"uadl/UadlMemory.h\"\n"
          << "#include \"uadl/DefaultFetchUnit.h\"\n"
          << "#include \"uadl/PipelineStage.h\"\n"
          << "#include \"uadl/RegResource.h\"\n";

      if (!_ir.res().semaphores().empty()) {
        _oh << "#include \"uadl/Semaphore.h\"\n";
      }

      if (!_ir.res().flags().empty()) {
        _oh << "#include \"uadl/Flag.h\"\n";
      }

      if (!_ir.res().forwardingPaths().empty()) {
        _oh << "#include \"uadl/ForwardingPath.h\"\n";
      }

      if (!_ir.res().caches().empty()) {
        _oh << "#include \"uadl/Cache.h\"\n";
      }

      // If we have any custom cache models, then we have to include their headers here.
      ForEach(_ir.res().caches(),i) {
        const Cache &cache = *i->second;
        if (cache.custom_model()) {
          _oh << "#include \"" << cache.custom_header() << "\"\n";
        }
      }

      // Do we have any branch predictors?  If so, then include the header.  Are any custom algorithms?  If so,
      // inclue their header files as well.
      if (!_ir.res().branchPredictors().empty()) {
        _oh << "#include \"uadl/BranchPredictor.h\"\n";
        ForEach(_ir.res().branchPredictors(),bp) {
          if ( bp->second->custom() ) {
            _oh << "#include \"" << bp->second->header() << "\"\n";
          }
        }
      }

      // Include any custom resources.
      ForEach(_ir.res().customResources(),cr) {
        _oh << "#include \"" << cr->second->header() << "\"\n";
      }

      _oh << "\n"
          << "namespace adl {\n"
          << "struct OpInfo;\n"
          << "struct IssNode;\n"
          << "}\n\n"
          << "namespace uadl {\n"
          << "class UadlMemoryIf;\n"
          << "}\n"
          << "\n"
          << "namespace " << _config._nameSpace << " {\n"
          << "\n";

      if (_config._nameSpace != getUadlNamespace()) {
        _oh << "using namespace uadl;\n";
      }

      genImplPrologue1(_oc,_ir,_config,true);
    }


    void genInterface()
    {
      const gc_string uArchParentClass = _config._systemc ? "UadlArchSystemC" : "UadlArch";

      // createArch()
      _oc << uArchParentClass << " *createArch(const string &name, unsigned &id,UadlMemoryIf *memory = NULL,ttime_t startTime = 0)\n"
          << "{\n";

      if (!_config._extMemMode) {
        _oc << "if (memory) {\n"
            << "cerr << \"Warning:  An external memory was specified but the model was created to use internal memory.\\n\";"
            << "}\n";
      }

      _oc << "\n"
          << "unsigned thread_id = 0;\n"
          << "return new " << _uname << "(";
      if (_config._systemc) {
        _oc << "name.c_str(),\n";
      }
      _oc << "id,thread_id,std::cout,startTime";
      if (_config._extMemMode) {
        _oc << ",memory";
      }
      _oc << ");\n"
          << "}\n" 
          << "\n\n";
    }

    void genEpilogue()
    {
      // BEGIN: header file epilogue
      _oh << "\n"
          << "} // namespace " << _config._nameSpace << "\n"
          << "\n"
          << "#endif // " << getHeaderMacroName(_ir) << "\n"
          << "\n"
          << "#ifdef " << getImplMacroName(_ir) << "\n\n"
          << "namespace " << _config._nameSpace << "{\n\n"
          << _os.str()
          << "\n"
          << "} // namespace " << _config._nameSpace << "\n\n"
          << "#endif // " << getImplMacroName(_ir) << "\n\n";
      // END: header file epilogue

      genImplEpilogue(_oc,_config);
    }

  };


  GenCodeIF::GenCodeIF(const UadlIr &ir, ostream &outH, ostream &outC,
                       const GenConfig &config) :
    _impl(new GenCode(ir,ir.top(),0,outH,outC,config))
  {
  }

  ImplCall(GenCodeIF,generate);
  
}

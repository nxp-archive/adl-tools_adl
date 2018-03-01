//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This class stores configuration information for a time-tagged ISS.
// Right now, we support a single configuration database; queries are
// made against this database for timing information, etc.
//

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "opencxx/parser/Ptree.h"

#include "helpers/fe_exceptions.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"

#include "fe/Helpers.h"
#include "fe/Data.h"

#include "MiscTypes.h"
#include "ConfigParser.h"
#include "ConfigDB.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  // fn:  Configuration file name.
  // Returns false if there's a parsing error.
  void ConfigDB::parse(const string &fn,const Data &data,const AnyOption &options,bool check_aux_funcs)
  {
    if (!fn.empty()) {
      ConfigParser parser(*this,data,check_aux_funcs);

      // First, use the standard parser configuration to get any defines and includes.
      standardParserSetup(parser,options,string());

      // We don't ever want dependency files, however.
      parser.setDepFile(string());
      parser.setDepTarget(string());
    
      if (!parser.parse(fn)) {
        RError("Error parsing ISS configuration file.");
      }
    
    }
  }

  void  ConfigDB::extern_mems(StrSet &externs) const
  {
    extern_mems(_top,externs);
    externs.erase("Mem");
  }

  void  ConfigDB::extern_mems(const Node &cur, StrSet &externs) const
  {
    ForEach(cur._children,i) {
      extern_mems(*(i->second), externs);
    }

    if (cur._coreConfig) {
      const StrSet &local_externs = cur._coreConfig->extern_mems();
      externs.insert(local_externs.begin(),local_externs.end());
    } else if (cur._sysConfig) {
      const StrSet &local_externs = cur._sysConfig->extern_mems();
      externs.insert(local_externs.begin(),local_externs.end());
    }
  }


  const ConfigDB::Node *ConfigDB::walk(Path::const_iterator p,Path::const_iterator e,
                                       const Node &cur) const
  {
    if (p == e) {
      return &cur;
    }
    Node::Nodes::const_iterator i = cur._children.find(*p);
    if (i == cur._children.end()) {
      return 0;
    } else {
      return walk(++p,e,*(i->second));
    }
  }

  ConfigDB::Node *ConfigDB::walk_insert(Path::const_iterator p,Path::const_iterator e,Node &cur)
  {
    if (p == e) {
      return &cur;
    }
    auto ip = cur._children.insert(make_pair(*p,(Node*)0));
    if (ip.second) {
      ip.first->second = new Node();
    }
    return walk_insert(++p,e,*(ip.first->second));
  }

  // Given a path, return a pointer to a core configuration object.
  CoreConfig *ConfigDB::getCoreConfig(const Path &path)
  {
    if(const Node* n=walk(path.begin(),path.end(),_top)) {
      return n->_coreConfig;
    }
    return 0;
  }

  const CoreConfig *ConfigDB::getCoreConfig(const Path &path) const
  {
    return const_cast<ConfigDB*>(this)->getCoreConfig(path);
  }

  const CoreConfig *ConfigDB::getFirstCoreConfigInternal(const Node *cur) const
  {
    if (cur->_coreConfig) {
      return cur->_coreConfig;
    } else if (!cur->_children.empty()) {
      return getFirstCoreConfigInternal(cur->_children.begin()->second);
    } else {
      return 0;
    }
  }

  const CoreConfig *ConfigDB::getFirstCoreConfig() const
  {
    return getFirstCoreConfigInternal(&_top);
  }

  CoreConfig &ConfigDB::makeCoreConfig(const Core &core)
  {
    auto ip = _top._children.insert(make_pair(core.name(),new Node(new CoreConfig(&core))));
    if (ip.first->second->_coreConfig->core() != &core) {
      RError("Core pointer mismatch when creating a new CoreConfig.");
    }
    return *(ip.first->second->_coreConfig);
  }

  CoreConfig &ConfigDB::makeCoreConfig(const Path &path,const Core &core)
  {
    auto n = walk_insert(path.begin(),path.end(),_top);
    auto ip = n->_children.insert(make_pair(core.name(),new Node(new CoreConfig(&core))));
    if (ip.first->second->_coreConfig->core() != &core) {
      RError("Core pointer mismatch when creating a new CoreConfig.");
    }
    return *(ip.first->second->_coreConfig);
  }

  // Given a path, return a pointer to a system configuration object.
  SysConfig *ConfigDB::getSysConfig(const Path &path)
  {
    if(const Node* n=walk(path.begin(),path.end(),_top)) {
      return n->_sysConfig;
    }
    return 0;
  }

  const SysConfig *ConfigDB::getSysConfig(const Path &path) const
  {
    return const_cast<ConfigDB*>(this)->getSysConfig(path);
  }

  void ConfigDB::finalize(MsgLog &ml,bool check_aux_funcs)
  {
    _top.finalize(ml,check_aux_funcs);
  }

  // Dump database information to the specified stream.
  void ConfigDB::dump(ostream &o) const
  {
    gc_string pfx = indent("  ");
    o << pfx << "Trace mode          :  " << _trace_mode << "\n"
      << pfx << "Debug mode          :  " << _debug_mode << "\n"
      << pfx << "Verbose mode        :  " << _verbose_mode << "\n"
      << pfx << "Line directives     :  " << _ld_okay << "\n"
      << pfx << "Time-tagged         :  " << _time_tagged << "\n"
      << pfx << "System calls enabled:  " << _syscall_enabled << "\n"
      << pfx << "External memory     :  " << _extern_mem << "\n"
      << pfx << "Log TD commit       :  " << _log_td_commit << "\n";
    _top.dump(o,pfx);
  }

  void ConfigDB::Node::finalize(MsgLog &ml,bool check_aux_funcs)
  {
    if(_coreConfig) {
      _coreConfig->finalize(ml,check_aux_funcs);
    }
    ForEach(_children,i) {
      i->second->finalize(ml,check_aux_funcs);
    }
  }

  void ConfigDB::Node::dump(ostream &o,const gc_string &pfx) const
  {
    if (_coreConfig) {
      _coreConfig->dump(o,pfx);
    }
    if (_sysConfig) {
      _sysConfig->dump(o,pfx);
    }
    if (!_children.empty()) {
      gc_string pfx2 = indent(pfx); 
      ForEach(_children,i) {
        o << pfx << i->first << ":\n";
        i->second->dump(o,pfx2);
      }
    }
  }



  void BaseConfig::set_extern_mem(const gc_string &n, bool ext) 
  {
    _memories.insert(make_pair(n,ext));
    if (ext) {
      _extern_mems.insert(n);
    }
  
  }

  bool BaseConfig::extern_mem(const gc_string &n) const
  {
    Memories::const_iterator i = _memories.find(n);
    if(i == _memories.end()) {
      return false;
    }
    return (i->second);
  }

  void BaseConfig::dump(std::ostream &o, const gc_string &pfx1) const
  {
    if(!_memories.empty()) {
      gc_string pfx2 = indent(pfx1);
      ForEach(_memories,i) {
        o << pfx1 << "Memory " << i->first << ":\n"
          << pfx2 << "External:  " << i->second << "\n";
      }
    }
  }

  template <typename G,typename C>
  const G &CoreConfig::get_group(const C &c,const gc_string &n) const
  {
    typename C::const_iterator i = c.find(n);
    if (i == c.end()) {
      RError("Unknown configuration group " << n);
    }
    return i->second;
  }

  template <typename G,typename C>
  const G *CoreConfig::get_group_from_instr(const C &c,const G *default_grp,
                                            const InstrMap &instr_mapping,const Instr &instr) const
  {
    // First, see if the instruction itself is registered.
    InstrMap::const_iterator iter;
    if ( (iter = instr_mapping.find(instr.name())) != instr_mapping.end()) {
      return &get_group<G,C>(c,iter->second);
    }

    // Barring that, look up each instruction class it's a member of.
    ForEach(instr.attrs(),i) {
      if (const char *cn = Data::get_attr_name(i->first)) {
        if ( (iter = instr_mapping.find(cn)) != instr_mapping.end()) {
          return &get_group<G,C>(c,iter->second);
        }
      }
    }

    // If nothing else, see what group contains the default-instruction element
    // and assign it to that.
    return default_grp;
  }

  const CoreConfig::Group *CoreConfig::get_group(const Instr &instr) const
  {
    return get_group_from_instr<Group,Groups>(_groups,_default_grp,_instr_mapping,instr);
  }

  // If we don't have a default graphite group, create one here, so that we
  // always have at least one group when we're queried.
  void CoreConfig::check_for_graphite_default() const
  {
    if (!_default_graphite_grp) {
      auto ip = _graphite_groups.insert(make_pair(getGraphiteDefaultGroup(),GraphiteGroup()));
      ip.first->second.set_name(getGraphiteDefaultGroup());
      ip.first->second.set_id(_last_graphite_id++);
      _default_graphite_grp = &ip.first->second;
    }
  }

  const CoreConfig::GraphiteGroups &CoreConfig::graphite_groups()
  {
    check_for_graphite_default();

    return _graphite_groups;
  }

  const CoreConfig::GraphiteGroup &CoreConfig::get_graphite_group(const Instr &instr) const
  {
    check_for_graphite_default();

    auto c = get_group_from_instr<GraphiteGroup,GraphiteGroups>(_graphite_groups,_default_graphite_grp,_graphite_instr_mapping,instr);
    assert(c);
    return *c;
  }

  void CoreConfig::Group::finalize(Core &core,MsgLog &ml,CoreConfig &cfg,bool check_aux_funcs)
  {
    // Add to the instruction -> group-name map.
    ForEach(_items,i) {
      cfg._instr_mapping[*i] = _name;
    }
    _items.clear();

    // Handle necessary aux function addition/checking.
    if (_allow_issue && check_aux_funcs) {
      AuxFunc *af = core.add_aux_func(_allow_issue._p,"bool","Allow-issue hook for time-tagged ISS.");
      Ptree *f = af->func();
      if (!arg_is_uint(f,0)) {
        PError(f,"Argument to allow-issue hook must be of type unsigned.");
      } 
      _allow_issue._d = af;
    }
  }

  void CoreConfig::GraphiteGroup::finalize(MsgLog &ml,CoreConfig &cfg,unsigned id)
  {
    _id = id;

    // Add to the instruction -> group-name map.
    ForEach(_items,i) {
      cfg._graphite_instr_mapping[*i] = _name;
    }
    _items.clear();
  }

  // Lookup a latency for an instruction (by name).
  // Returns -1 if not found.
  int CoreConfig::get_latency(const Instr &instr,int def) const
  {
    if (const Group *grp = get_group(instr)) {
      return grp->latency();
    } else {
      return def;
    }
    
    // If all else fails, return the default value.
    return def;
  }

  void CoreConfig::finalize(MsgLog &ml,bool check_aux_funcs)
  {
    if (_core) {
      // OK, yeah, this is a hack- we say it's constant, but then here, we
      // modify it.  Trying to change this, though, is kind of a mess, which is
      // the reason why I just hack in a const_cast here.
      Core &core = const_cast<Core&>(*_core);

      ForEach(_groups,i) {
        i->second.finalize(core,ml,*this,check_aux_funcs);
      }

      ForEach(_graphite_groups,i) {
        i->second.finalize(ml,*this,_last_graphite_id++);
      }

      if (check_aux_funcs) {
        core.check_aux_funcs(ml);
      }
    }
  }

  // Dump database information to the specified stream.
  void CoreConfig::dump(std::ostream &o,const gc_string &pfx1) const
  {
    gc_string pfx2 = indent(pfx1);
    gc_string pfx3 = indent(pfx2);
    o << pfx1 << "Generate MMU        :  " << _gen_mmu << "\n"
      << pfx1 << "Generate Caches     :  " << _gen_caches << "\n"
      << pfx1 << "Issue width         :  " << _issue_width << "\n";
    if (_mem_split_size) {
      o << pfx1 << "Mem split size      :  " << _mem_split_size << "\n";
    }
    if (!_reg_callbacks.empty()) {
      o << pfx1 << "Register callbacks  :  " << _reg_callbacks << "\n";
    }
    if (!_reg_read_callbacks.empty()) {
      o << pfx1 << "Register read callbacks:  " << _reg_read_callbacks << "\n";
    }
    if (!_except_callbacks.empty()) {
      o << pfx1 << "Exception callbacks :  " << _except_callbacks << "\n";
    }
    if (!_bb_end_instrs.empty()) {
      o << pfx1 << "Basic-block end instructions:  " << _bb_end_instrs << "\n";
    }
    if (!_opt_bb_end_instrs.empty()) {
      o << pfx1 << "Optional basic-block end instructions:  " << _opt_bb_end_instrs << "\n";
    }
    if (!_dyn_caches.empty()) {
      o << pfx1 << "Dynamic caches      :  " << _dyn_caches << "\n";
    }
    if (!_dyn_tlbs.empty()) {
      o << pfx1 << "Dynamic TLBs        :  " << _dyn_caches << "\n";
    }

    ForEach(_groups,i) {
      o << pfx1 << "Group " << i->first << ":";
      if (&i->second == _default_grp) {
        o << " (default)";
      }
      o << "\n" << dec
        << pfx2 << "Latency:  " << i->second.latency() << "\n";
      if (!i->second.stages().empty()) {
        o << pfx2 << "Stages:  ";
        bool first = true;
        ForEach(i->second.stages(),s) {
          list_sep(o,first);
          const Stage &stage = *s;
          o << stage.name();
          if (!stage.tag().empty()) {
            o << ":" << stage.tag();
          }
        }
        o << "\n";
      }
      o << pfx2 << "Items  :\n";
      ForEach(_instr_mapping,j) {
        if (j->second == i->first) {
          o << pfx3 << j->first << "\n";
        }
      }
    }
    ForEach(_graphite_groups,i) {
      o << pfx1 << "Group " << i->first << " (" << i->second.id() << ")";
      if (&i->second == _default_graphite_grp) {
        o << " (default)";
      }
      o << ":\n";
      o << pfx2 << "Items  :\n";
      ForEach(_graphite_instr_mapping,j) {
        if (j->second == i->first) {
          o << pfx3 << j->first << "\n";
        }
      }
    }
    if (!_cache_miss_latency.empty()) {
      o << pfx1 << "Cache miss latencies:\n";
      ForEach(_cache_miss_latency,i) {
        o << pfx2 << i->first << " = " << i->second << "\n";
      }
    }
    BaseConfig::dump(o,pfx2);
  }

}

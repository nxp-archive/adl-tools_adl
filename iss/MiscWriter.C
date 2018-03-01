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

#include <sstream>
#include <iostream>
#include <stdexcept>

#include "helpers/Macros.h"

#include "MiscWriter.h"
#include "ModelInterface.h"
#include "NodeHelpers.h"

using namespace std;

namespace adl {

  // Attribute used to designate that an MMU field is considered a translation
  // attribute and should be reported in the UVP "ta" field.
  static const string TranslationAttr = "translation_attr";

  // Attribute which implies that a register's or exception's intermediate
  // results should not be printed.
  static const char *SuppressIr = "indeterminate";

  // Attribute which implies that a register is the cia 
  //  static const char *Cia = "cia";

  // Attribute which implies that all reporting of the register or exception
  // should not be printed.
  static const char *SuppressAll = "unarchitected";

  // Attribute which implies that the current instruction should be squashed, i.e.
  // not displayed in the UVP.
  static const char *SquashInstr = "squash_instr";

  // Attribute which implies that an exception is asynchronous and should be printed as an "E : A"
  // card, rather than just as an intermediate result.
  static const char *AsynchExcept = "asynchronous";

  static const string UvpPathDelim = "::";

  // Walks the model hierarchy and builds our mirror hierarchy of CoreInfos.
  CoreInfoPtr buildHierarchy( CoreInfoPtr parent, IssNode &node, string parent_path) 
  {
    // Construct a CoreInfo for this node.
    unsigned ps = parent_path.size()+UvpPathDelim.size();
    string path = add_to_path(parent_path,node,UvpPathDelim);
    CoreInfoPtr ci(new CoreInfo(&node));
    ci->setParent(parent);
    ci->setNameData(node.name(),node.type(),path,ps);
    if (parent) {
      parent->add_child(ci);
    }

    // If we have children, construct them, too.
    ForEach(node.children(),iter) {
      buildHierarchy(ci, **iter, path );
    }

    // If any contexts are marked as threads, then add those as children as well.
    if (const CtxInfo *ctx = getThreadContext(node)) {
      ci->setThreadInfo(ctx->_id);
      ForRange(ctx->_num_contexts,i) {
        CoreInfoPtr c(new CoreInfo(&node,ctx->_id,i));
        ostringstream ss;
        ss << ctx->_name[0] << i;
        unsigned ps = path.size()+UvpPathDelim.size();
        string tpath = add_to_path(path,ss.str(),UvpPathDelim);
        c->setNameData(ss.str(),ctx->_name,tpath,ps);
        c->setParent(ci);
        ci->add_child(c);
      }
    }
    return ci;
  }

  // Entrypoint function for building a hierarchy.
  CoreInfoPtr buildHierarchy( IssNode &node )
  {
    return buildHierarchy(CoreInfoPtr(),node,"");
  }

  struct SortAttrObj {
    bool operator()(const AttrObj &x,const AttrObj &y) {
      return x._order < y._order;
    }
  };

  int get_suffix_index(const string &n)
  {
    size_t s = n.find_first_of("0123456789");
    if (s != string::npos) {
      return strtoul(n.c_str()+s,0,0);
    }
    return 0;
  }

  CoreInfo::CoreInfo(IssNode* node,int tid,int tinst) : 
    _print_ta(true),
    _path_start(0),
    _trg(node,tid,tinst)
  {
	  DBPrint("Creating CoreInfo " << this << '\n');
	  // Setup various stuff about the node.
    assert(node);
	  setup(*node);
  }

  CoreInfo::~CoreInfo()
  {
	  DBPrint("Cleaning up CoreInfo " << this << '\n');
  }

  void CoreInfo::reset()
  {
    _instrs.clear();

    _regs.clear();
    _init_trans.clear();
    _final_trans.clear();

    ForEach(_children,i) {
      (*i)->reset();
    }
  }

  void CoreInfo::setNameData(const std::string &n,const std::string &t,const std::string &p,unsigned ps) 
  { 
    _name = n; 
    _type = t; 
    _path = p; 
    _path_start = ps;
  };
  
  // This records what fields are considered translation attributes for 
  // the MMU fields.  These are then stored in _transAttrs so that we can
  // look them up quickly during the simulation.
  void CoreInfo::setup(const IssNode &node) 
  {
    // Grab handles to the various attributes in which we're interested.
    static int suppress_ir   = node.getAttrData().get_attr(SuppressIr);
    static int suppress_all  = node.getAttrData().get_attr(SuppressAll);
    static int squash_instr  = node.getAttrData().get_attr(SquashInstr);
    static int asynch_except = node.getAttrData().get_attr(AsynchExcept);

    // Setup the translation-attribute lookup table here.
    const MmuInfo &info = _trg.node()->getMmuInfo();
    unsigned max_id = 0;
    ForEach(info,iter) {
      max_id = max(max_id,iter->_id);
    }
    _transAttrs.resize(max_id+1);
    _tlb_suppress.resize(max_id+1);

    // Do we have an MMU?  If not, then suppress printing of translation
    // attributes.
    if (info.empty()) {
      _print_ta = false;
    } else {

      ForEach(info,l_iter) {
        const MmuLookupInfo &li = *l_iter;
        int id = li._id;
    
        int count = 0;
        ForRange(li._fields.size(),f_iter) {
          const MmuFieldInfo &fi = li._fields[f_iter];

          // See if we should ignore this item.
          if (fi.has_attr(suppress_all)) {
            _tlb_suppress[id][f_iter] = isAll;
            continue;
          }
          else if (fi.has_attr(suppress_ir)) {
            _tlb_suppress[id][f_iter] = isIr;
            continue;
          }

          if (fi._setfield) {
            continue;
          }

          // See if this is a translation attribute.
          if (!fi._setfield) {
            ForEach(fi._attrs,attr) {
              string attr_name = node.getAttrData().get_attr(attr->first);
              if ( attr_name.compare(0,TranslationAttr.size(),TranslationAttr) == 0) {
                _transAttrs[id].push_back(AttrObj(get_suffix_index(attr_name),count,fi._width));
                // There should only be one of these attributes, so quit if we get
                // one.
                break;
              }
            }
          }
          ++count;
        }
        sort(_transAttrs[id].begin(),_transAttrs[id].end(),SortAttrObj());
      }

    }

    //    static int cia = getAttrData().get_attr(Cia);
    const RegInfos &rinfo = _trg.node()->getRegInfo();
    const ExceptionInfos &einfo = _trg.node()->getExceptionInfo();

    // Process all registers.
    ForEach(rinfo,i) {
      // We want to keep the highest level of suppression for this register.
      if ( i->has_attr(suppress_all) ) {
        _reg_suppress[i->_id] = isAll;
      }
      else if ( i->has_attr(suppress_ir) ) {
        _reg_suppress[i->_id] = isIr;
      }
      // If we have fields, store a pointer to the field description.  We use
      // this for printing comments during initial and final results.
      if ( !i->_fields.empty() ) {
        _reg_fields[i->_id] = &(i->_fields);
      }
    }

    // Hack.  This is a workaround because Raptor displays the EPN and
    // RPN fields as though they are each always size 54 bits.  No
    // BookeE designs here support size 0 (1K) pages so they use
    // don't use the full 54 bits.  For instance, for Altair the
    // EPN is 52 bits and the RPN is 32.  We have to shift these fields
    // to account for this.
    //
    // Pull out the EPN and RPN indices so that as we iterate over the
    // lookup data for each TLB entry, we can check if we are dealing
    // with one of these fields and if so shift them.  ugh.
    // This is done for each lookup in the MMU.
    ForEach(info,l_iter) {
      const MmuLookupInfo &li = *l_iter;
      _hacks.resize(li._id+1);
      HackFields &hack = _hacks[li._id];
      hack._shift = li._pageshift;
      ForRange(li._fields.size(),f_iter) {
        const MmuFieldInfo &fi = li._fields[f_iter];
        // get the index of the EPN & RPN lookup objects.
        if (fi._name == "EPN") {
          hack._epn = f_iter;
        } else if (fi._name == "RPN") {
          hack._rpn = f_iter;
        }
      }
    }

    // Process all exceptions.
    ForEach(einfo,i) {
      if ( i->has_attr(suppress_all) ) {
        _excpt_suppress[i->_id]._ir_action = isAll;
      } else if ( i->has_attr(suppress_ir) ) {
        _excpt_suppress[i->_id]._ir_action = isIr;
      }
      if ( i->has_attr(squash_instr) ) {
        _excpt_suppress[i->_id]._squash = true;
      }
      if ( i->has_attr(asynch_except) ) {
        _excpt_suppress[i->_id]._asynch = true;
      }
    }
  }

  IrSuppress ignore_type(const RegSuppressHash &ignores,unsigned id)
  {
    auto i = ignores.find(id);
    if (i != ignores.end()) {
      return i->second;
    } else {
      return isNone;
    }
  }

  IrSuppress CoreInfo::reg_suppress(unsigned id) const
  {
    return ignore_type(_reg_suppress,id);
  }

  ExcptData CoreInfo::excpt_data(unsigned id) const
  {
    auto i = _excpt_suppress.find(id);
    if (i != _excpt_suppress.end()) {
      return i->second;
    } else {
      return ExcptData();
    }
  }

  const RegFieldInfos *CoreInfo::reg_fields(unsigned id) const
  {
    auto i = _reg_fields.find(id);
    if (i != _reg_fields.end()) {
      return i->second;
    } else {
      return 0;
    }
  }


  InstrEntry *CoreInfo::add_instr (InstrEntryPtr &&e) 
  { 
    _instrs.push_back(move(e));
    return _instrs.back().get();
  }

  void CoreInfo::addrHack(FieldData &fd,unsigned id)
  {
    // Adjust EPN/RPN fields, if necessary for this lookup.
    HackFields &hf = _hacks[id];
    if (hf._epn >= 0) {
      fd[hf._epn]._value <<= hf._shift;
    }
    if (hf._rpn >= 0) {
      fd[hf._rpn]._value <<= hf._shift;
    }
  }

  void CoreInfo::remove_instrs()
  {
    _instrs.clear();
    ForEach(_children,i) {
      (*i)->remove_instrs();
    }
  }

  void CoreInfo::add_translation (TransEntryPtr &&t,bool final) 
  { 
    TransEntry *te;
    if (!final) {
      _init_trans.push_back(move(t));
      te = _init_trans.back().get();
    } else {
      _final_trans.push_back(move(t));
      te = _final_trans.back().get();
    }

    addrHack(te->_ld,te->id());
  }

  CoreInfo *CoreInfo::get_thread(const IssNode *parent)
  {
    if (_trg.ctx_id() >= 0) {
      return _children.at( parent->getActiveCtx(_trg.ctx_id()) ).get();
    } else {
      return 0;
    }
  }

  CoreInfo *CoreInfo::switch_thread(unsigned id,unsigned nc)
  {
    if (_trg.ctx_id() == (int)id) {
      // Switch to another thread, which will be a peer of this CoreInfo object,
      // unless we're in a parent node (identified by having an instance of -1),
      // in which case we dive down to get the relevant child node.
      if (!is_thread_instance()) {
        return _children.at( nc ).get();
      } else {
        return _parent->children().at(nc).get();
      }
    }
    return 0;
  }

  CoreInfo *CoreInfo::switch_to_parent()
  {
    if (is_thread_instance()) {
      return _parent.get();
    } else {
      return this;
    }
  }

  pair<addr_t,addr_t> CoreInfo::getProgramCounter() const
  {
    CtxState ctxstate(_trg,true);
    addr_t ea = node()->getProgramCounter();
    addr_t ra;
    if (const IssCore *c = core()) {
      if (!c->extInstrReadTranslate(ra,ea,true)) {
        // This used to be an error, but various cores exist which allow for
        // missing translations, so I'm just going to have this return ea=ra.
        // Tracediff should detect the issue if it's an error.
        ra = ea;
      }
    } else {
      ra = ea;
    }
    return make_pair(ea,ra);
  }

  void CoreInfo::setProgramCounter(addr_t a) const
  {
    CtxState ctxstate(_trg);
    _trg.node()->setProgramCounter(a);
  }

  void RaInfo::add_mapping (addr_t &ea,unsigned &ta,CoreInfo* c) 
  {
      bool unique = true;

      ForEach(_mappings,iter) {
        if (iter->owner() != c) {
          // there is more than one owner for this ra, set it to global ownership
          _owner = 0;
        } else if (( iter->ea() == ea) && (iter->ta() == ta) && (iter->owner() == c)){
          // this mapping already exists
          unique = false;
          break;
        }
      }
      if (unique) {
        if (_mappings.empty() && !_owner) {
          // set myself as the owner
          _owner = c;
        }
        _mappings.push_back(MapNode(ea,ta,c));      
      }
    }

  bool RaInfo::get_mapping (addr_t &ea,unsigned &ta,const CoreInfo* c) const 
  {
      // returns the the first ea/ta mapping info for the given owner if found
      ForEach(_mappings,iter) {
        if (iter->owner() == c) {
          ta = iter->ta();
          ea = iter->ea();
          return true;
        }
      }
      return false;
    }

}

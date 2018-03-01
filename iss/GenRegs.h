//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Interface to register generation code.
//

#ifndef _GEN_REGS_H_
#define _GEN_REGS_H_

#include "MiscTypes.h"

namespace adl {

  class opfxstream;
  class InstrWalker;
  class ClassData;
  class ConfigDB;
  class Reg;
  class RegFile;

  gc_string getRegObjName(const RegBase &r);
  gc_string getContextTypeName(const Context &ctx);
  gc_string getContextObjName(const Context &ctx);
  gc_string getContextObjType(const Context &ctx);
  gc_string getContextControl(const Context &ctxm,bool full_qual = true);
  gc_string getContextDbgControl(const Context &ctx,bool full_qual = true);
  gc_string getCtxChangeFunc(const Context &ctx);
  gc_string getCtxChangeBackFunc(const Context &ctx);
  gc_string getRegFieldGetter(const RegBase &r,const RegField &rf);
  gc_string getRegFieldSetter(const RegBase &r,const RegField &rf);
  gc_string getRegExtWriteFunc(const RegBase &r);
  gc_string getNiaSfx();

  // Used for conveying lists of registers.  The boolean
  // indicates shared/not-shared.
  template <class RT>
  struct RegEl {
    const RT      *_reg;
    bool           _shared;
    const Context *_ctx;

    RegEl(const RT *r,bool s,const Context *c) : _reg(r), _shared(s), _ctx(c) {};
    RegEl(const RT *r,bool s) : _reg(r), _shared(s), _ctx(0) {};
  };
  typedef gc_list<RegEl<Reg> > RegList;
  typedef gc_list<RegEl<RegFile> > RegFileList;

  struct Watch {
    gc_set<const RegBase *>  _regs;
    Opencxx::Ptree          *_watchfunc; // Called on external updates, e.g. setReg.
    Opencxx::Ptree          *_writefunc; // Called on register writes.
  
    Watch(const InstrWalker::RegBaseList &rl,Opencxx::Ptree *w) : 
      _regs(rl.begin(),rl.end()), _watchfunc(w), _writefunc(w) {};
    Watch(const InstrWalker::RegBaseList &rl,Opencxx::Ptree *wa,Opencxx::Ptree *wr) : 
      _regs(rl.begin(),rl.end()), _watchfunc(wa), _writefunc(wr) {};
    bool watches_reg(const RegBase *r) {
      return _regs.count(r);
    }
  };
  struct Watches : public gc_list<Watch> {};

  void genWatch(opfxstream &o,Watches &watches,InstrWalker &walker,Opencxx::Environment *env,Opencxx::Ptree *watch);

  typedef gc_hash_map<Opencxx::Ptree *,const Core *> PCHash;

  // Extended watch support- this is used to handle shared registers which might
  // have watches.  This means that on a write to a register in one core, we might
  // have to propagate that event to other cores.  These structures handle that
  // situation.
  struct ParentWatch {
    Opencxx::Ptree   *_watchfunc;
    PCHash            _child_watches;

    ParentWatch(Opencxx::Ptree *s) : _watchfunc(s) {};
  };

  typedef gc_hash_map<gc_string,ParentWatch> ParentWatches;

  struct Parent {
    const SysItem *_si;
    gc_string    _type;
    gc_string    _ref;
    gc_string    _arg;
    ParentWatches  _watches;

    Parent(const SysItem *si,const gc_string &t,Opencxx::Ptree *r) : 
      _si(si), _type(t), _ref(r->ToString()), _arg(_ref+"_arg") {};
  };
  typedef gc_vector<Parent> Parents;

#define DefaultCtxName "default"

  struct RegCtxData {
    const Context *_ctx;
    RegList        _regs;
    RegFileList    _regfiles;

    RegCtxData() : _ctx(0) {};
  };
  typedef gc_map<gc_string,RegCtxData> RegCtxMap;

  class GenRegsBase;
  class GenRegs;

  // Create a map of register and register file lists.  Each primary key in this map
  // represents a context, or 'default' for any non-context resource.
  void makeRegLists(RegCtxMap &regdata,const Core &core);

  struct GenRegsBaseIF {

    GenRegsBaseIF(opfxstream &o,ClassData &cd,const ConfigDB &config);
    GenRegsBaseIF(GenRegsBase *);

    // Just generate register data structures.
    void generateRegDS(const RegList &regs,const Core *core=0);
    void generateRegFileDS(const RegFileList &rf,const Core *core=0);

    // Generate the member variables.
    void generateRegVars(ClassData &,const RegList &regs);
    void generateRegFileVars(ClassData &,const RegFileList &rf);


    // Generate register assist/debug functions.
    void generateRegAssistFuncs(const RegCtxMap &regdata,const RegList &regs,const RegFileList &regfiles,
                                int *ignore_class,bool is_system);

  protected:
    GenRegsBase *_impl;

  };

  struct GenRegsIF : public GenRegsBaseIF {

    GenRegsIF(opfxstream &o,const Core &core,InstrWalker &walker,
              const gc_string &filename,RegBaseSet &mmu_regs,
              Parents &parents,Watches &watches,ClassData &cd,
              const ConfigDB &config, const CoreConfig &coreconfig);

    void setPacketWidth(unsigned w);
    
    // Entry point for generating register-file attribute enum.
    void generateRegFileAttrs();

    // Entry point for generating the registers.
    void generateRegResources();

  protected:
    GenRegs *_impl;
  };

}

#endif

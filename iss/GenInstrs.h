//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Interface to instruction generation code.
//

#ifndef _GEN_INSTRS_H_
#define _GEN_INSTRS_H_

#include <string>

namespace adl {

  class opfxstream;
  class InstrWalker;
  class ClassData;
  class ConfigDB;
  class GenInstrs;
  class CoreConfig;
  class Context;
  class Watches;
  class InstrSyms;
  class GenInstrPacketIF;
  class PfxFileArray;

  gc_string getCtxUpdateName();
  gc_string getContextUpdateVar();
  gc_string getCtxControlFunc(const Context &ctx);
  gc_string getCtxUpdateFunc(const Context &ctx);
  gc_string getCommitCheckName();
  gc_string getBreakpointCheckName();
  gc_string getCheckForBreakPointsName();
  gc_string getIssNamespace(const ConfigDB &config);

  void writeSupportFuncs(opfxstream &o,ClassData &cd,const ConfigDB &config,
                         bool is_core,bool has_post_sim,const gc_string &class_name);

  struct GenInstrsIF {

    GenInstrsIF(opfxstream &o,PfxFileArray *impls,opfxstream *jo,const Core &core,InstrWalker &walker,
                const gc_string &filename,Watches &watches,ClassData &cd,
                GenInstrPacketIF &genip,StrList &end_code,InstrSyms &instr_syms,
                const Path &path,const ConfigDB &config,const CoreConfig &coreconfig);

    void handlerSetup();
    void generateInstrAttrs();
    void generateInstrTables();
    void generateSubInstrs();
    void generateInstrHandlers();
    void generateSimSupportFuncs();
    void generateSysCallSupport();
    void generateInstrs();
    void generateHelpers();
    void generateHalt();
    void generateJitFwdDecls();

  protected:
  
    GenInstrs *_impl;
  };

}

#endif

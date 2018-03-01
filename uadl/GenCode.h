//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _CODE_GEN_H
#define _CODE_GEN_H

#include <fstream>

#include "UadlCommon.h"

namespace Opencxx {

  class Ptree;

}

namespace adl {

  class StrStrMap;
  class StrList;
  class opfxstream;
}

namespace uadl {

  class UadlIr;
  class Machine;
  class InstClass;
  class Action;
  class Actions;
  class Transaction;
  class Transactions;
  class CTransactions;
  class FieldQueries;
  class Guards;
  class PredMap;
  class BranchPredictor;
  class InquireFuncs;
  class GenCode;

  enum scType { scNone, scMethod, scThread };
  
  struct GenConfig {
    bool        _debugMode;      // True => Generate extra run-time checks.
    bool        _safeMode;       // Generate a safe-mode model.
    bool        _tagsOnlyCaches; // Caches only store tags.
    bool        _issMemMode;     // Use ISS-mem-mode (ISS uses its own memory).
    scType      _systemc;        // Generate SystemC interface.
    bool        _extMemMode;     // Generate external-memory interface.
    bool        _extCycleMem;    // Generate pre/post cycle code for an external memory.
    bool        _postSimHook;  // ISS has a post-simulation hook to be called.
    bool        _ld_okay;        // Generate line directives.
    bool        _tracing;        // Generate tracing code.
    bool        _sep_ics;        // Generate instruction classes in separate files.
    bool        _ic_enabled;     // Generate a cache for instruction objects.
    bool        _vd_enabled;     // Use virtual dispatch for instruction classes, rather than templates.
    int         _ic_size;        // Size of instruction cache (number of entries).
    int         _ic_page_size;   // Size of each page/entry.
    int         _deadlock;       // != 0 => create deadlock detection/breaking logic for safe-mode models.
    int         _memPoolSize;    // Number of items in memory pool.
    gc_string   _fn_suffix;
    gc_string   _basename;
    gc_string   _nameSpace;
    gc_string   _preamble;

    GenConfig() : 
      _debugMode(false),
      _safeMode(false),
      _tagsOnlyCaches(false),
      _issMemMode(false),
      _systemc(scNone),
      _extMemMode(false),
      _extCycleMem(false),
      _postSimHook(false),
      _ld_okay(false),
      _tracing(true),
      _sep_ics(false),
      _ic_enabled(false),
      _vd_enabled(false),
      _ic_size(1024),
      _ic_page_size(1024),
      _deadlock(0),
      _memPoolSize(256)         // Just an arbitrary default size.
    {};
  };

  struct FileItem : public gc {
    std::ofstream   *_of;
    adl::opfxstream *_o;
    bool             _started;
    bool             _finished;

    FileItem() : _of(0), _o(0), _started(false), _finished(false) {}
    void destroy();
  };
  struct FileMap : public gc_map<gc_string,FileItem*> {
    void destroy();
  };

  gc_string getPrologue(const UadlIr &ir,const GenConfig &config);

  void genImplPrologue(std::ostream &oc,const UadlIr &ir,const GenConfig &config);
  void genImplEpilogue(std::ostream &oc,const GenConfig &config);

  struct GenCodeIF {

    GenCodeIF(const UadlIr &IR,std::ostream &outh,std::ostream &outc,const GenConfig &config);

    // Output the entire model.
    void generate();

  protected:
    GenCode *_impl;
  };

}

#endif // _CODE_GEN_H

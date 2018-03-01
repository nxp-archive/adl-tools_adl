//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The Reader class is the base class used for reading initial state
// for stand-alone ISSs.
//

#include <stdexcept>
#include <iostream>
#include <sstream>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"

#include "ModelInterface.h"
#include "Reader.h"
#include "MemoryLog.h"
#include "StringHelpers.h"

using namespace std;

namespace adl {


  // Global option-processing object.
  AnyOption &options()
  {
    static AnyOption opts;
    return opts;
  }

  static bool WarnMemOverwrite = true;

  Reader::Reader (const std::string &filename,IssNode *root) :
    _root(root),
    _filename (filename),
    _init_memory(true)
  {
    WarnMemOverwrite = (options().getFlag("warn-mem-overwrite",WarnMemOverwrite));
  }

  void Reader::init_reg_internal (IssNode &n,const string &name,int index,const rnumber::RNumber &value) 
  { 
    if (!n.setReg(name,index,value)) {
      if (LaxMode) {
        cerr <<"Warning: Ignoring unknown register '" << n.name() <<": " << name << "'.\n";
      } else {
        RError("Error: Unknown register '" << n.name() <<": " << name << "'.");
      }
    }
  }

  void Reader::init_reg_internal (IssNode &n,const string &name,int index,uint64_t value)
  {
    if (!n.setReg(name,index,value)) {
      if (LaxMode) {
        cerr <<"Warning: Ignoring unknown register '" << n.name() <<": " << name << "'.\n";
      } else {
        RError("Error: Unknown register '" << n.name() <<": " << name << "'.");
      }
    }
  }

  void Reader::init_mmu_internal (IssNode &n,const string &lookup,unsigned set,unsigned way,const FieldData &ld)
  {
    n.setMmuLookup(lookup,set,way,ld);
  }

  void Reader::init_mem_internal (IssNode &n, const string &name,addr_t addr, uint32_t data, unsigned size,bool log_only)
  { 
    initialize_mem (n,name,addr,data,size,log_only || !_init_memory); 
  }

  void Reader::init_cache_internal(IssNode &n,const std::string &name,unsigned set,unsigned way,
                                   const FieldData &fd,const CacheLineData &cd)
  {
    n.setCaches(name,set,way,fd,cd);
  }

  uint64_t Reader::symbol(const char* sname) const 
  { 
    throw std::runtime_error("Reader: no symbol table information.");
  }   

  void Reader::setEntryPoint(const std::string &str)
  {
    if (str.empty() || !_root) return;

    addr_t entry_point;
    if (!getAddr(entry_point,str)) {
      try {
        entry_point = symbol(str.c_str());
      }
      catch (runtime_error &err) {
        cerr << err.what() << "  Will skip setting the entry point to '" << str << "'.\n";
        return;
      }
    }
    _root->setProgramCounter(entry_point);
    cerr << "Reader:  Setting program counter for " << _root->name() << " to the entry addr: 0x" << hex << entry_point << '\n' << dec;
  }

  // Called by the reader to setup memory and tell the writer
  void initialize_mem (IssNode &n, const std::string &name, addr_t addr,uint32_t data,unsigned size,bool log_only)
  {
    if (!log_only) {
      n.setMem(name,addr,data,size);
    }
    if (!memoryLog().set(n,name,addr) && WarnMemOverwrite) {
      cerr << "Warning:  Overwriting initialized memory at 0x" << hex << addr << '\n' << dec;
    }
  }

}

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

#ifndef READER_H
#define READER_H 

#include <fstream>

#include "helpers/stringfwd.h"

#include "ModelInterface.h"


namespace rnumber {
  class RNumber;
}

namespace adl {

  struct FieldData;
  struct CacheLineData;
  struct IssNode;
  struct IssCore;

  // Base reader class.
  class Reader {
  public:
    Reader (const std::string &filename,IssNode *root);
    virtual ~Reader(){};

    const std::string &filename() const { return _filename; };

    virtual void setRoot(IssNode *r) { _root = r; };

    // This should return a string describing the file-type it creates, e.g. "dat".
    virtual const char *format_str() const = 0;

    // Specify whether memory should be initialized or not during the reading
    // process.  If not, then do anything else, but just don't init memory.
    void initMemory(bool im) { _init_memory = im; };
  
    // If supported, lets the user toggle whether the entry-point listed in
    // the file should be obeyed.
    virtual void useFileEntryPoint(bool) {};
    
    // If supported, lets the user define custom entry-point (either an address or a symbol).
    virtual void setEntryPoint(const std::string &str);

    // Reads from the input file.  This should return false if nothin was accomplished because an
    // end-of-file condition was reached and true otherwise.
    virtual bool readFile() { return false; };

    // Returns true if no input is left.  This allows a reader to support the concept
    // of storing multiple independent data sets within a single file.
    virtual bool finished() const { return true; };

    // Returns the value of the symbol, or 0 if there is no symbol table or the symbol
    //  is not found.
    virtual uint64_t symbol(const char* sname) const;
    
    // Trace run functions
    
    virtual void initTraceReading() {};
    
    virtual bool hasTraceSupport() { return false; };
    
    virtual bool applyNext() { return true; };
    virtual bool applyNext(bool ignore_brpt) { return true; };
    
    // KEY_FUNC: applyPrev
    virtual bool applyPrev() { return true; };
    virtual bool applyPrev(bool ignore_brpt) { return true; };
    
    virtual uint64_t instrCount() { return 0; };

  protected:

    // these methods talk to the model via ModelInterface
    void init_reg_internal  (IssNode &n,const std::string &name,int index,const rnumber::RNumber &value);
    void init_reg_internal  (IssNode &n,const std::string &name,int index,uint64_t value);
    void init_mmu_internal  (IssNode &n,const std::string &lookup,unsigned set,unsigned way,const FieldData &ld);
    void init_cache_internal(IssNode &n,const std::string &name,unsigned set,unsigned way,const FieldData &fd,const CacheLineData &cd);
    void init_mem_internal  (IssNode &n,const std::string &name,addr_t addr, uint32_t data, unsigned size,bool log_only = false);
  
    IssNode       *_root;
    std::string    _filename;
    bool           _init_memory;
  };

}

#endif

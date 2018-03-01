//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Header file of the Writer class, the generic class used for writing intermediate
// and final test results in a stand-alone ISS.
//

#ifndef WRITER_H
#define WRITER_H 

#include "ModelInterface.h"
#include "LoggingMgr.h"

namespace adl {

struct IssNode;
struct FieldData;
struct MmuBase;
struct MemoryLog;

struct WriterLogger : public LoggingIface {

  WriterLogger();

  // for intermediate values
  virtual void logInstrRead(unsigned id,const char *name,addr_t ea,addr_t ra,uint32_t value);

protected:
  // Last instruction access- stores data from last call to log_instr_read.
  addr_t _instr_ea, _instr_ra;

};

class Writer : public LogParentIface {
public:
  Writer (const std::string &filename,IssNode &root,const MemoryLog &memlog);
  virtual ~Writer() { }

  const std::string &filename() const { return _filename; };

  // This should return a string describing the file-type it creates, e.g. "dat".
  virtual const char *format_str() const = 0;

  virtual void post_initialize() {}
  virtual void pre_finalize () {}
  virtual void post_finalize () {}

  // Called when to indicate that, for a stream writer, it should flush its
  // buffers and consider the contents it has already written "done" for this
  // particular operation.  Subsequent write operations may follow.
  virtual void stream_done() {}

  virtual void log_initialized_translation (IssCore *c,const char *lookup,unsigned set,unsigned way,const FieldData &ld);

  // for passing initial & final values
  virtual void report_regfile (const IssNode *,unsigned id,const char *name,int index,const std::string &value,bool shared){}
  virtual void report_reg (const IssNode *,unsigned id,const char *name,const rnumber::RNumber &value,bool shared){}
  virtual void report_mmu (const IssNode *,const char *lookup,unsigned set,unsigned way,
                           const FieldData &fd,bool shared,addr_t epn, bool valid_epn, addr_t rpn, bool valid_rpn){}
  virtual void report_cache(const IssNode *,const char *name,CacheType type,unsigned level,addr_t addr,unsigned set,unsigned way,
                            bool valid,const FieldData &fd,bool shared,const byte_t *data,unsigned n) {}
  virtual void report_memory(IssNode *, unsigned id,const std::string &name, bool shared) {}

  virtual void writeResults(){}

protected:

  IssNode          &_root;

  const MemoryLog  &_initMemLog;
 
  std::string       _filename; 
};

}

#endif

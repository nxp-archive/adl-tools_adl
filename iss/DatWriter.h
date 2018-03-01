//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef DATWRITER_H
#define DATWRITER_H 

#include <list>
#include <map>
#include <vector>

#include "NodeHelpers.h"
#include "Writer.h"
#include "ReportExtras.h"

namespace rnumber {
  class RNumber;
}

namespace adl {

  struct MemoryLog;
  struct DatWriterLogger;

  typedef std::unique_ptr<MemoryLog> MemoryLogPtr;

  // This class implements routines needed for printing initial and final
  // results.
  class DatReporter : public ReportBase, public ReportExtras {
  public:
    DatReporter(std::ostream *o,const IssNode &root);

    // for passing initial & final values
    virtual void report_ctx_change(const IssNode *n,unsigned id,const char *name,unsigned context_num);
    virtual void report_ctx_off(const IssNode *n);
    virtual void report_reg (const IssNode *,unsigned id,const char *name,const rnumber::RNumber &value,bool shared);
    virtual void report_regfile (const IssNode *,unsigned id,const char *name,int index,const rnumber::RNumber &value,bool shared);
    virtual void report_mmu (const IssNode *,const MmuBase &mb,unsigned set,unsigned way,
                             const FieldData &ld,bool valid,bool shared,addr_t epn, bool valid_epn, addr_t rpn, bool valid_rpn);
    virtual void report_cache(const IssNode *,const char *name,CacheType type,unsigned level,addr_t addr,unsigned set,unsigned way,
                              bool valid,const FieldData &fd,bool shared,const byte_t *data,unsigned n);
    virtual void report_memory(IssNode *,unsigned id, const std::string &name, bool shared);

  protected:
    virtual void write_reg_value(unsigned id, const rnumber::RNumber &value) = 0;
    virtual void core_update(const IssNode *c,bool emit_card = true) {};

    std::ostream &o() { return *_o; };

    void write_mem();
    void write_mmu_line(const FieldData &ld);
    void write_mem_card(IssNode *n,const std::string &name,addr_t addr);

    std::ostream    *_o;             // The output stream.
    MemoryLog       *_memlog;        // Tracks used addresses- this object is not managed.
    const IssNode   &_root;
  };

  // This is an internal class which implements most of the real functionality
  // of the DAT writer.  It's split out from the main Writer class so that we
  // can support two modes of operation: All cores write to the same file and
  // each core writes to its own file.
  class DatWriterImpl : public DatReporter {
  public:
    DatWriterImpl(const std::string &filename,const std::string &path,IssNode &root,
                  const MemoryLog &initMemLog,bool compress = false, unsigned max_size = 0,
                  unsigned max_instrs = 0);
    DatWriterImpl(std::ostream &os,IssNode &root,const MemoryLog &initMemLog);
    virtual ~DatWriterImpl();

    LoggingIface *register_core(IssCore &core);

    void post_initialize();
    void post_finalize();
    void stream_done();

    void writeResults();

    typedef std::map<const IssNode *,std::shared_ptr<DatWriterLogger> > CoreMap;

  private:
    friend class DatWriterLogger;

    void populateCoreMap(const std::string &name,IssNode &node);

    void core_update(const IssNode *c,DatWriterLogger *dwl,bool emit_card = true);
    void update_core_info(const IssNode *c,DatWriterLogger *dwl,bool emit_card);

    virtual void core_update(const IssNode *c,bool emit_card = true);

    void write_reg_value(unsigned id, const rnumber::RNumber &value);
    void write_cache_type(CacheType type);
    void core_tag();
    void write_core_info();
    void log_used_address(unsigned id,const char *name,addr_t ra,uint32_t value);
    void logRegValue(unsigned id, const rnumber::RNumber &value);
    std::string get_filename();
    void make_new_stream();
    void check_file_limits();

    void write_regs();
    void write_memory();
    void write_mmu();
    void write_cache();

    std::string      _fn_base;
    std::string      _fn_ext;
    const MemoryLog &_initMemLog;
    MemoryLogPtr     _memlogp;

    bool             _compress;

    // For storing limits, used to slice up long trace files into multiple
    // files.
    unsigned         _max_size;
    unsigned         _max_instrs;

    unsigned         _testcount;
    unsigned         _instrcount;
    unsigned         _filecount;

    // Stores base counts for handling limits.
    unsigned         _base_pcount;
    unsigned         _base_icount;

    CoreMap          _cores;
    CTarget          _cur_core;      // pointer to the current core (the one executing instructions).
    CTarget          _update_core;   // pointer to the core from which we're getting updates.
    DatWriterLogger *_update_info;   // name of the update core.
  };

  // Main public interface class for the DAT writer.
  class DatWriter : public Writer {
  public:
    DatWriter(const std::string &filename,IssNode &root,const MemoryLog &initMemLog,bool compress = false);
    DatWriter(std::ostream &os,IssNode &root,const MemoryLog &initMemLog);
    virtual ~DatWriter();

    virtual LoggingIface *register_core(IssCore &core);

    virtual const char *format_str() const { return "dat"; };

    virtual void post_initialize();
    virtual void post_finalize();
    virtual void stream_done();

    virtual void writeResults();

  private:
    struct ImplFactory;

    void createImpls(const std::string &filename,const std::string &ext,
                     IssNode &node,const MemoryLog &initMemLog,
                     bool compress,unsigned max_size,unsigned max_impls);

    typedef std::unique_ptr<DatWriterImpl> DatWriterImplPtr;
    std::vector<DatWriterImplPtr> _impls;   
  };

}

#endif

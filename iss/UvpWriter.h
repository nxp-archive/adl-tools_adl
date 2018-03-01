//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef UVPWRITER_H
#define UVPWRITER_H 

#include <fstream>
#include <list>
#include <map>

#include "helpers/pfxstream.h"

#include "Writer.h"
#include "ReportExtras.h"

namespace adl {

  class RaInfo;
  class UvpLogger;
  class CoreInfo;
  struct Regs;
  class InstrEntry;
  struct MmuBase;
  struct MemoryLogger;
  struct FieldData;
  class RegSuppressHash;

  // We really only need 2 currently, but this is just in case.  This array is
  // sized so that we can store multiple translations, if required due to a
  // misaligned access.
  const unsigned MaxDataTrans = 3;

  class UvpWriter : public Writer, public ReportBase, public ReportExtras {
  
  public:
    UvpWriter(const std::string& filename,IssNode &root,const MemoryLog &initMemLog);
    UvpWriter(std::ostream &os,IssNode &root,const MemoryLog &initMemLog);
    virtual ~UvpWriter();

    const char *format_str() const { return "uvp"; };

    virtual LoggingIface *register_core(IssCore &core);

    virtual void post_initialize ();
    virtual void pre_finalize ();
    virtual void post_finalize ();
    virtual void stream_done();

    virtual void writeResults();
  
    // callbacks for the core to report register values, called at start to get inits and end to get finals
    virtual void report_ctx_change(const IssNode *n,unsigned id,const char *name,unsigned context_num);
    virtual void report_ctx_off(const IssNode *n);
    virtual void report_regfile (const IssNode *n,unsigned id,const char *name,int index,const rnumber::RNumber& value,bool shared);
    virtual void report_reg (const IssNode *n,unsigned id,const char *name,const rnumber::RNumber& value,bool shared);
    virtual void report_mmu (const IssNode *n,const MmuBase &mb,unsigned set,unsigned way,
                             const FieldData &ld,bool valid,bool shared,addr_t epn, bool valid_epn, addr_t rpn, bool valid_rpn);

    // Use these for settting the translation caching pointers, as they'll
    // properly delete an element if necessary.
    void setCurInstrTrans(const MmuBase *);
    void setCurDataTrans(int seq,const MmuBase *);

  protected:
    bool hasCurInstr() const { return _curInstr; };

  private:
    friend class UvpLogger;

    typedef std::vector<std::unique_ptr<UvpLogger> > UvpLoggers;

    void reset();
    void initTransInfo();
    void clearTransInfo();    

    CoreInfo* getCoreInfo (CoreInfo &ci,const IssCore &c);
    CoreInfo* getCoreInfo (const IssNode* n);

    void updateCurr(CoreInfo &ci,const IssCore &c);
    void updateCurr(const IssNode* n);

    const MmuBase *last_data_trans(int seq);
    unsigned get_ta(const MmuBase *trans);

    void write_regs(bool initial,const CoreInfo* ci);
    void write_instrs(const CoreInfo* ci);
    void write_mem(bool init,const CoreInfo* ci);
    void write_mmu(bool initial,const CoreInfo* ci);

    void write_instr(const InstrEntry &,unsigned &count,unsigned gcount,bool print_ta);
    void write_ir_reg_update(const std::string& name,const rnumber::RNumber& value);
    void write_ir_mem_update(addr_t ea,addr_t ra,bool print_ta,uint32_t wimg,uint32_t value,uint32_t type);
    void write_ir_interrupt (const std::string& name);
    void write_ir_annotation (const std::string& name);
    void write_ir_mmu_update(const RegSuppressHash &field_ignores,const std::string& name,addr_t epn,int way,const FieldData &fd);
    void write_ir_mmu_access(const std::string& name,int set,int way);

    void writeResCard(addr_t ea,addr_t ra,bool print_ta,unsigned wimg);
 
    void writeResults (const CoreInfo* ci);
    void displayResources (CoreInfo* c);
    void removeHalt(CoreInfo* c);

    typedef std::shared_ptr<CoreInfo> CoreInfoPtr;
    typedef std::unique_ptr<MemoryLogger> MemoryLoggerPtr;

    UvpLoggers             _loggers;       // Store pointers to logger objects for eventual cleanup.
    CoreInfoPtr            _root;          // The tree hierarchy of system/cores/threads.
    CoreInfo*              _curCore;       // Pointer to the current core (optimization for displaying resources).
    CoreInfo*              _updateCore;    // Pointer to the core from which we're getting updates.
    InstrEntry*            _curInstr;      // Pointer to the last instruction (for adding intermediate results).
    std::ostream          *_ostream;       // Pointer to stream for stream mode.
    std::ofstream          _o;             // Main output file.
    opfxstream             _os;            // the test stream, with indentation support.
    bool                   _testFinished;
    unsigned               _testcount;
    unsigned               _ircount;       // number of ir processed.
    std::string            _testname;
    MemoryLoggerPtr        _memQList;      // Holds all the used ra information for writing out the dcards.
    unsigned               _instr_count;   // Global count for determining instruction trace order.
    bool                   _use_std_log;   // If true, we read memory address

    // data from the address log stored
    // in the base class.

    const MmuBase*         _curInstrTrans;               // Pointer to last instruction translation used.
    const MmuBase*         _curDataTrans[MaxDataTrans];  // Array of data translations used.
  };

} 

#endif

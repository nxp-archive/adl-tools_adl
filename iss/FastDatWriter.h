//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a stripped down writer, used to create a very minimal trace.  It just
// recoreds the last n instruction addresses, where n is configurable via a
// command-line parameter.  This writer is mainly useful for MT simulations,
// where the act of full tracing causes the issue to go away.
//

#ifndef _FAST_DAT_WRITER_H_
#define _FAST_DAT_WRITER_H_

#include "iss/Writer.h"
#include "iss/DatWriter.h"

namespace adl {

  struct IssNode;
  struct FastDatLogger;

  typedef std::map<const IssCore *,FastDatLogger *> CoreMap;

  struct FileObj {
    std::ofstream   _out;

    FileObj(const std::string &fn) : _out(fn.c_str()) {};
  };

  struct FastDatLogger;
  
  struct FastDatWriter : public Writer, public FileObj, public DatReporter 
  {  
    FastDatWriter(const std::string &filename, IssNode &root, const MemoryLog &memlog);
    ~FastDatWriter();

    void populateCoreMap(IssNode &node,unsigned);

    virtual LoggingIface *register_core(IssCore &core);

    // Required by the Writer class: Returns a string identifying the writer's type.
    virtual const char *format_str(void) const { return "FDAT"; };

    virtual void post_initialize();
    virtual void writeResults();

  protected:
    void write_reg_value(unsigned id, const rnumber::RNumber &value);
    void writeResults(const IssCore &core,FastDatLogger &fdl);
    void post_initialize (const IssCore &core,FastDatLogger &fdl);
    void write_current_state(const IssCore &core);

    unsigned        _testcount;
    CoreMap         _cores;
  };  


}

#endif

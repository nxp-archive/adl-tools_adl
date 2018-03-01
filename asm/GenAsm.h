//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Wrapper classes for accessing assembler/disassembler generation.
//

#ifndef _GENASM_H_
#define _GENASM_H_

#include <iosfwd>
#include <vector>

#include "helpers/gc_string.h"
#include "helpers/gc_list.h"


const gc_string  EMPTY_STRING = "";

namespace adl {

  class Core;
  class Data;
  class Instr;
  class InstrList;
  class AnyOption;

  class GenerateAssemblerInt;
  class GenerateDisassemblerInt;


  // Interface class used for creating a recursive call to the disassembler.
  class DisassemblerCallIF {
  public:
    virtual bool disassembleMicroOps() const = 0;    
    virtual void operator()(std::ostream &o,const gc_string &arg,const gc_string &table_type,bool has_multiple_tables) = 0;
    
  };
  
  class GenDisassembler {
  public:
    // core:    Core disassembly generation.
    // use_cpp: Output code will use C++ I/O.
    // name_ws: The instruction's name will be padded with whitespace so that all operands line up in a column.
    // fname:   Base-name of file being created.  Only used for usage tracking, so can be empty if not relevant.
    // log_usage: Usage logging control string.  Empty = no logging.
    // dc:      Interface object for creating recursive calls to the disassembler.  If 0, the default will be used.
    GenDisassembler(const Core &core,bool use_cpp,bool name_ws, bool gen_pfx_logic,bool library_mode,
                    const gc_string &fname,const gc_string &log_usage,DisassemblerCallIF *dc);

    // Write a "leaf node" entry in the disassembly table.
    void writeDisTableEntry(std::ostream &o,const Instr &instr,const InstrList &shorthands);

    // Write a "leaf node" entry in the disassembly table accessors.
    void writeDisTableEntryAccessors(std::ostream &o,const Instr &instr,const InstrList &shorthands);

    // Main entry point.  If 'h' is non-null, then header_name should be the
    // name of the file corresponding to the 'h' stream.  This is for generating
    // debugger-assist information.
    void exec(std::ostream &o,std::ostream *h,const gc_string &header_name);

  private:
    class GenerateDisassemblerInt *_dis;
  };

  class GenAssembler {
  public:
    // fname:   Base-name of file being created.  Only used for usage tracking, so can be empty if not relevant.
    // log_usage: Usage logging control string.  Empty = no logging.
    GenAssembler(const Core &core, const gc_string &fname,const gc_string &log_usage);

    // Main entry point.
    void exec(std::ostream &cfile);
  private:
    class GenerateAssemblerInt *_asm;
  };

  struct DbgRegElm {
    gc_string _name;
    gc_string _pfx;
    int       _index;
    unsigned  _width;

    DbgRegElm(const gc_string &name,unsigned width) : _name(name), _index(-1), _width(width) {};
    DbgRegElm(const gc_string &name,const gc_string &pfx,unsigned index,unsigned width) : 
      _name(name), _pfx(pfx), _index(index), _width(width) {};

    bool has_index() const { return _index >= 0; };
  };

  typedef gc_list<DbgRegElm> DbgRegList;

  // Scans a design for registers listed as debug registers, and stores a list of name,prefix,width tuples.
  void checkForDebugRegs(DbgRegList &debug_regs,const Core &core);

  // Main entry-point for creating the binutils assembler/disassembler.
  void genAsmDisasm(const Data &data,const std::string &archpfx,const std::string &archname,const std::string &archpname,
                    const std::string &input,const AnyOption &options);

}

#endif

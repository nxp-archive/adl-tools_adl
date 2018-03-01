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

#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"
#include "helpers/AnyOption.h"
#include "helpers/stringhash.h"
#include "helpers/StringUtils.h"

#include "NodeHelpers.h"
#include "FastDatWriter.h"
#include "WriterUtils.h"
#include "MemoryLog.h"

using namespace std;
using namespace adl;
using namespace rnumber;

namespace adl {

  AnyOption &options();

  // By default, we save the last 10000 instructions.
  const int DefaultInstrCount = 10000;

  // By store up to 64 bits of opcode in the trace.
  const int MaxInstrSize = 8;

  static bool dummy ATTRIBUTE_UNUSED = 
    options().setOption  ("fdat-icount",    "Specify number of instructions to store for a fast-trace output file.\n"
                          "Default = "+itos(DefaultInstrCount));


  // A single instruction element.  We currently only support up to 64-bit
  // instructions.
  struct InstrItem {
    addr_t       _ea;
    uint64_t     _icount;
    Disassembler _disasm;
    unsigned     _num_half_bytes;
    uint32_t     _opc[MaxInstrSize];

    InstrItem() :
      _ea(0),
      _icount(0),
      _disasm(0)
    {
      memset(_opc,0,sizeof(_opc));
    }

    InstrItem(addr_t ea,uint64_t icount,Disassembler disasm,const uint32_t *opc,int num_half_bytes) :
      _ea(ea),
      _icount(icount),
      _disasm(disasm),
      _num_half_bytes(num_half_bytes)
    {
      memcpy(_opc,opc,min(num_half_bytes/2,MaxInstrSize));
    }
  
    bool valid() const { return _icount != 0; };

    void print(ostream &o)
    {
      if (valid()) {
        unsigned w = (_num_half_bytes+7)/8;
        RNumber opcode = RNumber(_opc,w,32*w);
        unsigned shift = (32*opcode.wordCount() - 4*_num_half_bytes);
        opcode >>= shift;
        if (_num_half_bytes) {
          opcode.resize(4*_num_half_bytes);
        }
        o << "I ea=0x" << hex << _ea << " id=" << dec << _icount << "\n"
          << "INSTR op=0x" << hex << setw(_num_half_bytes) << opcode << " asm=\"";
        _disasm(o,_ea,_opc);
        o << "\"\n\n";
      }
    }
  };

  typedef vector<InstrItem> InstrVect;

  struct FastDatLogger : public LoggingIface {

    string         _name;
    const IssCore *_core;
    InstrVect      _instrs;
    unsigned       _size;
    unsigned       _index;
    uint64_t       _icount;
    addr_t         _last_ea;

    FastDatLogger(const IssCore *core,unsigned size) :
      _name(getPathName(core,":")),
      _core(core),
      _size(size),
      _index(0),
      _icount(0)
    {
    }

    void reset()
    {
      _instrs.clear();
      _instrs.resize(_size);
      _index = 0;
      _icount = 0;
      _last_ea = 0;
    }
  
    unsigned next(unsigned index)
    {
      return (index + 1) % _size;
    }

    virtual void logInstrPrefetch(addr_t ea) 
    {
      _last_ea = ea;
    }

    virtual void logInstr(const uint32_t *opc,int num_half_bytes,const char* name, Disassembler dis,uint32_t flags) 
    {
      _instrs[_index] = InstrItem(_last_ea,++_icount,dis,opc,num_half_bytes);
      _index = next(_index);
    }    
  
    void writeTrace(ostream &o)
    {
      o << "TRACE\n\n";

      // Start with the next element beyond the current one and find a valid
      // entry.  Then loop to the current element, printing the data.
      for (unsigned index = next(_index); index != _index; index = next(index)) {
        _instrs[index].print(o);
      }
    }

  };


  // This just opens the file and makes sure that the operation succeeded.
  FastDatWriter::FastDatWriter(const std::string &filename, IssNode &root, const MemoryLog &memlog) :
    Writer(filename,root,memlog),
    FileObj(filename.c_str()),
    DatReporter(&_out,root),
    _testcount(0)
  { 
    unsigned size = options().getOption("fdat-icount",DefaultInstrCount);

    // Walk the hierarchy, recording full core names, hashed by node pointers.
    populateCoreMap(root,size);
  
    if (!_out) {
      RError("Unable to open filename " << filename);
    }

    // We're not going to modify it, so it's OK to store it in a non-const
    // pointer for this writer.
    _memlog = const_cast<MemoryLog*>(&memlog);
  }

  FastDatWriter::~FastDatWriter()
  {
    ForEach(_cores,citer) {
      delete citer->second;
    }
  }

  void FastDatWriter::populateCoreMap(IssNode &node,unsigned size)
  {
    if (const IssCore *core = dynamic_cast<const IssCore *>(&node)) {
      _cores.insert(make_pair(core,new FastDatLogger(core,size)));
    }
    // Then add any children.
    ForEach(node.children(),i) {
      populateCoreMap(**i,size);
    }
  }

  LoggingIface *FastDatWriter::register_core(IssCore &core)
  {
    // return a pointer to the logger for the given core
    return _cores.find(&core)->second;
  }  

  void FastDatWriter::post_initialize()
  {
    ForEach(_cores,citer) {
      FastDatLogger &fdl  = *citer->second;
      const IssCore &core = *fdl._core;

      post_initialize(core,fdl);
    }

    _out << flush;
  }

  void FastDatWriter::post_initialize (const IssCore &core,FastDatLogger &fdl)
  {
    fdl.reset();

    _out << setfill('0')
         << "\n"
         << "TEST id=" << _testcount++ << "\n"
         << "INIT\n\n"
         << "CORE n=" << fdl._name << "\n\n";
  
    write_current_state(core);

    _out << "\n" << flush;
  }

  void FastDatWriter::writeResults()
  {
    ForEach(_cores,citer) {
      FastDatLogger &fdl = *citer->second;
      const IssCore &core = *fdl._core;

      writeResults(core,fdl);
    }

    _out << flush;
  }

  void FastDatWriter::writeResults(const IssCore &core,FastDatLogger &fdl)
  {
    _out << "CORE n=" << fdl._name << "\n\n";

    fdl.writeTrace(_out);

    _out << "RESULTS\n\n";

    write_current_state(core);
  }

  void FastDatWriter::write_current_state(const IssCore &core)
  {
    core.showRegs(*this);

    core.showCaches(*this);    

    core.showMmu(*this);

    write_mem();
  }

  void FastDatWriter::write_reg_value(unsigned id, const RNumber &value)
  {
    _out << " d=0x" << hex << setfill('0') << setw(value) << value;
  }

}


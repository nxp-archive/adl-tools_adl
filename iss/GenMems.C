//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main generation code for all memory stuff.
//

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "helpers/gc_list.h"
#include "helpers/pfxstream.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/BitTwiddles.h"

#include "AttrsHelper.h"
#include "InstrWalker.h"
#include "ConfigDB.h"
#include "GenMems.h"
#include "GenMmu.h"
#include "GenCache.h"
#include "GlobalMemoryName.h"
#include "MiscTypes.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

static const char *attr_used = " ATTRIBUTE_USED ";

namespace adl {


  // Use the defines MEM_BIT_WIDTH and MEM_LOOKUP_LEVELS to define the memory
  // characteristics.

  // Number of bits that we care about.  The address type is defined in ModuleInterface.h.
#ifndef MEM_BIT_WIDTH
# define MEM_BIT_WIDTH 64
#endif

  // Number of lookup levels.  For example, 32/2 means that we care about 32 bits of the
  // address and have 2 levels of lookup- one tree node and then the lookup within the
  // page itself.  A level of 1 thus implies a single array of bytes.
#ifndef MEM_LOOKUP_LEVELS
# define MEM_LOOKUP_LEVELS 4
#endif

  // Use the SPARSE_MEM to determine when to use a SPARSE Memory
#ifndef SPARSE_MEM
# define SPARSE_MEM 1048576
#endif

  static gc_string DmiStoreAttr = "dmi_store";

  gc_string getMemoryName(const gc_string &name)
  {
    return (name == MemoryName) ? GlobalMemoryName : name;
  }

  bool isGlobalMem(const gc_string &name)
  {
    return (name == MemoryName);
  }

  bool isGlobalMemName(const gc_string &name)
  {
    return (name == GlobalMemoryName);
  }

  gc_string getMemoryName(const Memory &memory)
  {
    return getMemoryName(memory.name());
  }

  gc_string getMemHandlerType()
  {
    return "MemHandler";
  }

  gc_string getMemWrapType(const SysItem &x)
  {
    return "LoggingMemWrapper<" + x.name() + "> ";
  }

  gc_string getMemWrapName()
  {
    return "_logging_mem_wrapper";
  }

  gc_string getMemHandlerAccessType(bool is_data,bool is_logging_mem,bool extern_dmi,bool is_translating,const SysItem *item)
  {
    gc_string arg;
    if (extern_dmi) {
      arg = getExtDmiType();
    } else if (is_logging_mem && item) {
      arg = getMemWrapType(*item);
    } else {
      arg = getMemHandlerType();
    }   
    // if is_translating is true, this implies that we're doing a zero-time
    // instruction fetch when translating a basic block for the JIT or decode
    // cache.
    gc_string type = (is_data) ? "ExternDataAccess" : ( (is_translating) ? "ExternInstrTransAccess" : "ExternInstrAccess");
    return type + "<" + arg + "> ";
  }

  gc_string getMemHandlerRef(bool is_logging_mem,bool extern_dmi)
  {
    if (extern_dmi) {
      return "((" + getExtDmiType() + "*)&" + getExtDmiName() + ")";
    } else if (is_logging_mem) {
      return getMemWrapName();
    } else {
      return getMemHandlerName(getGlobalMemRef());
    }
  }

  gc_string getGlobalMemObj(bool is_extern,bool is_data,bool is_logging_mem,bool extern_dmi,const Core *core)
  {
    if (is_extern) {
      return "(" + getMemHandlerAccessType(is_data,is_logging_mem,extern_dmi,false,core) + "(" + getMemHandlerRef(is_logging_mem,extern_dmi) + "))";
    } else {
      return GlobalMemoryName;
    }
  }

  gc_string getExtMemAccessor(bool is_data,const gc_string &name)
  {
    gc_string pfx = (is_data) ? "_data_access_" : "_instr_access_";
    return pfx + name;
  }

  gc_string getExtDmiType( )
  {
    return "DmiMemHandlerType";
  }

  gc_string getExtDmiName()
  {
    return "_extern_dmi";
  }

  bool is_global_mem(unsigned id)
  {
    return id == 0;
  }

  gc_string getMemoryType(const Memory &memory,bool is_extern)
  {
    gc_ostringstream     ss;
    if(is_extern) {
      ss << (memory.instr_mem() ? "ExternInstrAccess" : "ExternDataAccess")
         << "<" << getMemHandlerType() << ">";
    } else if(memory.size() < SPARSE_MEM) {
      // prefetch memory have exta word handling cyclic accesses. 
      // The first and last word are identical
      unsigned  sz = (memory.prefetch_mem() ? memory.size() + 8 : memory.size());
      ss << "SimpleMemory<" << sz << ">";
    }
    else {
      ss << "SparseMemory<" << MEM_BIT_WIDTH << "," << MEM_LOOKUP_LEVELS << ">"; 
    }
    return ss.str();
  }

  gc_string getCacheType(const Memory &memory,bool is_extern) 
  {
    gc_ostringstream ss;
    ss << "SimpleCache< " << getMemoryType(memory,is_extern) << " & >";
    
    return ss.str();
  }

  gc_string getGlobalCacheType(bool is_extern,bool is_data,bool logging_mem,bool extern_dmi,bool debug,bool is_translating,const Core *core)
  {
    gc_string pfx, carg;
    if (logging_mem && core) {
      pfx = "LoggingMem";
      carg = "," + core->name();
    }
    if (core && core->typed_mem() && is_data && !debug && (logging_mem || is_extern)) {
      pfx += "Typed";
    } else if (!is_data && logging_mem) {
      pfx += "Instr";
    }
    if (is_extern || extern_dmi) {
      return "Simple" + pfx + "Cache<" + getMemHandlerAccessType(is_data,logging_mem,extern_dmi,is_translating,core) + carg + ">";
    } else {
      return "Simple" + pfx + "Cache<GlobalMemoryType&" + carg + ">";
    }
  }

  gc_string getGlobalMemRef()
  {
    return GlobalMemoryName;
  }
  
  gc_string getGlobalMemName(bool is_extern)
  {
    if (is_extern) {
      return getMemHandlerName(getGlobalMemRef());
    } else {
      return GlobalMemoryName;
    }
  }

  gc_string getMemAccessor(bool is_data,bool is_extern,bool extern_dmi,const gc_string &name)
  {
    if (is_extern || extern_dmi) {
      return getExtMemAccessor(is_data,name);
    } else {
      return name;
    }
  }

  gc_string getGlobalCacheVar(const gc_string &var,bool is_extern,bool is_data,bool is_logging_mem,bool extern_dmi,const Core *core)
  {
    gc_string carg;
    if (is_logging_mem && core) {
      carg = ",*this";
    }
    if ((is_extern || is_logging_mem) && core && core->typed_mem() && is_data) {
      carg += ",type";
    }
    return getGlobalCacheType(is_extern,is_data,is_logging_mem,extern_dmi,false,false,core) + "   " + var + "(" + getGlobalMemObj(is_extern||extern_dmi,is_data,is_logging_mem,extern_dmi,core) + carg + ");\n";
  }

  gc_string getTranslateType(const Memory &memory) 
  {
    gc_ostringstream ss;
    ss << "MemoryTranslate<" << log2(memory.addr_unit()) << "," << memory.offset() << "," << memory.size() << ">";
    
    return ss.str();
  }

  gc_string getDmiParmName(const gc_string &p)
  {
    return "_dmi_" + p;
  }

  gc_string getReadHookName(const gc_string &mname)
  {
    return mname + "_read_hook";
  }

  gc_string getWriteHookName(const gc_string &mname)
  {
    return mname + "_write_hook";
  }

  gc_string getInstrHookName(const gc_string &mname)
  {
    return mname + "_instr_read_hook";
  }

  class GenMemBase {
  public:
    GenMemBase (opfxstream &o,const StrSet &externs,ClassData &cd,
                const ConfigDB &config,bool is_system) :
      _o(o), 
      _externs(externs),
      _cd(cd),
      _config(config),
      _is_system(is_system)
    {};

  protected:
    opfxstream     &_o;

    const StrSet   &_externs;
    ClassData      &_cd;
    const ConfigDB &_config;
    bool            _is_system;

  public:

    bool mt_support() const
    {
      return _config.mt_support();
    }

    bool rw_locked_mem() const
    {
      return mt_support() && _config.mt_rw_locked_mem();
    }

    bool transactional_iss() const
    {
      return _config.iss_type() == tTransactional;
    }

    bool trans_non_logging_iss() const
    {
      return transactional_iss() && (_config.mem_if() != mLogging);
    }

    bool normal_iss() const
    {
      return _config.iss_type() == tNormal;
    }

    bool logging_mem() const
    {
      return _config.mem_if() == mLogging;
    }

    bool skip_caches_in_exec() const
    {
      return _config.skip_caches();
    }

    bool time_tagged() const
    {
      return _config.time_tagged();
    }

    bool trans_cache() const
    {
      return transactional_iss() && !logging_mem();
    }

    bool rollback() const
    {
      return _config.rollback_mode();
    }

    bool tlm2_endianness() const
    {
      return _config.tlm2_endianness();
    }

    bool ignore_mode() const
    {
      return _config.ignore_mode();
    }
    
    bool extern_mem(const gc_string &name) 
    {
      return (_externs.find(name) != _externs.end());
    }

    bool extern_mem(const Memory &mem) 
    {
      return extern_mem(mem.name());
    }

    virtual bool data_little_endian() const
    {
      return false;
    }

    // This version returns true if the external-memory option is true for the global memory.
    bool extern_mem()
    {
      return _config.extern_mem();
    }

    bool is_system() const
    {
      return _is_system;
    }

    bool extern_dmi()
    {
      // We don't use the DMI cache at the system level, since that's mainly
      // just for setup purposes.
      return _config.extern_dmi() && !_is_system;
    }

    // From a memory perspective, JIT and decode-cache models are the same-
    // we're aiming for higher performance.
    bool jit_support() const
    {
      return _config.jit_support() || _config.dc_support();
    }
    
    bool library_mode() const
    {
      return _config.library_mode();
    }

    bool have_dt() const
    {
      return _config.dep_tracking();
    }

    gc_string getMemPointsIterator(bool is_const)
    {
      return adl::getMemPointsIterator(library_mode(),is_const);
    }

    // Generate data structures for a memory
    void genMemoryDataStructure(const Memory &memory, bool shared)
    {
      // Ignore if the memory is external.
      if (extern_mem(memory)) return;
      
      gc_string mname = getMemoryName(memory);
      gc_string tname = getMemoryType(memory,extern_mem(memory));
      // Define the structure name

      // A Memory is either a byte array (if size <= 1M) or a sparse array.
      if (!shared) {
        if(memory.parent()) {
          gc_string pmname = getMemoryName(*memory.parent());
          _o << tname << " &" << mname << ";\n\n";
          _cd._constr_inits.push_back(mname + "(" + pmname + ")");
        } 
        else {
          _o << tname  << " " << mname << ";\n\n";
        }
      }
      else {
        _o << tname << " &" << mname << ";\n\n";
        _cd._constr_args.push_back(tname+" &__"+mname);
        _cd._constr_inits.push_back(mname+"(__" +mname+")");
      }
    }
    // Generate function return instrction and data memory names, currently we supprot one instruction and one
    // data memory.
    void genMemNameGetters(const MemoryList &memories) {
      // Instruction memory
      gc_string imem_name(MemoryName);
      // Data memory
      gc_string dmem_name(MemoryName);
      ForEach (memories,i) {
        const Memory &mem = *(i->_memory);
        unsigned  id = mem.id();
        if (is_global_mem(id) || mem.prefetch_mem()) {
          continue;
        }
       	if (mem.instr_mem()) {
          imem_name = getMemoryName(mem);
        } else {
          dmem_name = getMemoryName(mem);
        }
      }
      _o << "std::string getInstrMemName() const { return \"" << imem_name << "\"; }\n";
      _o << "std::string getDataMemName() const { return \"" << dmem_name << "\"; }\n";
    };

    void genSizedMemRead(unsigned size)
    {
      _o << "uint" << size << "_t mem_read" << dec << size << "(addr_t addr)\n"
         << "{\n"
         << "return ";
      if (data_little_endian()) {
        _o << "swap_uint" << size << "(";
      }
      if (extern_mem()) {
        _o << getMemHandlerName(getGlobalMemRef()) << "->read" << size << "(CacheRead,addr)";
      } else {
        _o << "adl::mem_read" << size << "(addr)";
      }
      if (data_little_endian()) {
        _o << ")";
      }
      _o << ";\n"
         << "}\n\n";
    }

    void genSizedMemWrite(unsigned size)
    {
      _o << "void mem_write" << dec << size << "(addr_t addr,uint" << size << "_t value";
      if (size > 8) {
        _o << ",unsigned nb";
      }
      _o << ")\n{\n";
      if (data_little_endian() && (size > 8)) {
        _o << "value = swap_uint" << size << "(value);\n";
      }
      if (extern_mem()) {
        _o << getMemHandlerName(getGlobalMemRef()) << "->write" << size << "(addr,value";
        if (size > 8) {
          _o << ",nb";
        }
        _o << ");\n";
      } else {
        _o << "adl::mem_write" << size << "(addr,value";
        if (size > 8) {
          _o << ",nb";
        }
        _o << ");\n";
      }
      _o << "}\n\n";
    }

    void genByteReadWrite()
    {
      _o << "uint32_t mem_read32(const byte_t *buf) const\n"
         << "{\n"
         << "return byte_read32(buf,0);\n"
         << "}\n\n"

         << "void mem_write32(byte_t *buf,uint32_t v,unsigned nb = BytesPerWord)\n"
         << "{\n"
         << "byte_write32(buf,0,v,nb);\n"
         << "}\n\n";
    }

    void genMemoryHelpers(const MemoryList &memories) 
    {
      // Memory initialziation method
      _o << "//\n"
         << "// Memory initialization method.\n"
         << "//\n"
         << "\n"
         << "template <class Translate, class Cache> void global_mem_set(Translate t,Cache c,addr_t addr, uint64_t data, unsigned size)\n"
         << "{\n"
         << "NullWriteLog	 l;\n"
         << "DPrint(std::hex << \"0x\" << addr << \": 0x\" << data << std::dec << \"\\n\");\n"
         << "mem_write_sized<Translate,Cache,NullWriteLog," << data_little_endian() << ">(t,c,l,addr,data,size);\n"
         << "}\n"
         << "\n"
         << "template <class Translate, class Cache> uint64_t global_mem_get(Translate t,Cache c,addr_t addr, unsigned size) const\n"
         << "{\n"
         << "NullReadLog	 l;\n"
         << "return mem_read_sized<Translate,Cache,NullReadLog," << data_little_endian() << ">(t,c,l,addr,size);\n"
         << "}\n"
         << "\n"
         << "void base_global_mem_set(addr_t ra,uint64_t data,unsigned size)\n"
         << "{\n"
         << "NullTranslate t;\n"
         << getGlobalCacheVar("c",extern_mem(),true,false,extern_dmi(),0)
         << "global_mem_set(t,c,ra,data,size);\n"
         << "}\n"
         << "\n"
         << "uint64_t base_global_mem_get(addr_t ra,unsigned size) const\n"
         << "{\n"
         << "NullTranslate t;\n"
         << getGlobalCacheVar("c",extern_mem(),true,false,extern_dmi(),0)
         << "return global_mem_get(t,c,ra,size);\n"
         << "}\n"
         << "\n";

      // generate the setting function
      _o << "void setMem(unsigned id, addr_t addr, uint64_t data, unsigned size) {\n";
      if (rw_locked_mem()) _o << "AdlSimLock<boost::mutex> mem_lock(" << getMemMutex() << ");\n";
      _o << "switch (id) {\n"
         << "case 0:\n"
         << "base_global_mem_set(addr,data,size);\n"
         << "break;\n";
      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        _o << "case " << memory.id() << ":\n"
           << getMemoryName(memory) << "_set(addr,data,size);\n"
           << "break;\n";
      }
      _o << "default:\n"
         << "RError(\"Non existing memory:  \" << id);\n"
         << "}\n"
         << "}\n\n";

      _o << "uint64_t getMem(unsigned id, addr_t addr,unsigned size) const {\n";
      if (rw_locked_mem()) _o << "AdlSimLock<boost::mutex> mem_lock(" << getMemMutex() << ");\n";
      _o << "switch (id) {\n"
         << "case 0:\n"
         << "return base_global_mem_get(addr,size);\n";
      // Generate the specific memory functions for local memories
      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        _o << "case " << memory.id() << ":\n"
           << "return " << getMemoryName(memory) << "_get(addr,size);\n";
      }
      _o << "default:\n"
         << "RError(\"Non existing memory:  \" << id);\n"
         << "}\n"
         << "}\n\n";

      _o << "unsigned getMemId(const std::string &name) const {\n"
         << "if (name == GlobalMemName) {\n"
         << "return 0;\n"
         << "}\n";
      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        _o << "else if (name == \"" << memory.name() << "\") {\n"
           << "return " << memory.id() << ";\n"
           << "}\n";
      }
      _o << "else {\n"
         << "RError(\"Non existing memory name:  \" << name);\n"
         << "}\n"
         << "}\n\n";

      // Generate the specific memory set functions for local memories
      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        gc_string mname  = getMemoryName(memory);
        gc_string cname  = getCacheType(memory,extern_mem(memory));
        gc_string xname  = getTranslateType(memory);

        gc_string mem_init = getMemAccessor(!memory.instr_mem(),extern_mem(memory),false,mname);

        _o <<"void  " << mname << "_set(addr_t ea, uint32_t data, unsigned size) {\n"
           << xname << " t;\n"
           << cname  << " c(" << mem_init << ");\n"
           << "NullWriteLog	   l;\n"
           << "DPrint(std::hex << \"0x\" << ea << \": 0x\" << data << std::dec << \"\\n\");\n"
           << "mem_write_sized<" << xname << "," << cname << ",NullWriteLog," << data_little_endian() << ">(t,c,l,ea,data,size);\n"
           << "}\n";	
      }
    
      // Generate the specific memory get functions for local memories
      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        gc_string mname  = getMemoryName(memory);
        gc_string mtype  = getMemoryType(memory,extern_mem(memory));
        gc_string cname  = getCacheType(memory,extern_mem(memory));
        gc_string xname  = getTranslateType(memory);

        gc_string mem_init = getMemAccessor(!memory.instr_mem(),extern_mem(memory),false,mname);

        _o <<"uint32_t  " << mname << "_get(addr_t ea,unsigned size) const {\n"
           << xname << " t;\n"
           << cname  << " c(const_cast<" << mtype << "&>(" << mem_init << "));\n"
           << "NullReadLog	   l;\n"
           << "return mem_read_sized<" << xname << "," << cname << ",NullReadLog," << data_little_endian() << ">(t,c,l,ea,size);\n"
           << "}\n";
      }

      // Generate the main-memory sized access functions.
      genSizedMemRead(64);
      genSizedMemRead(32);
      genSizedMemRead(16);
      genSizedMemRead(8);
      genSizedMemWrite(64);
      genSizedMemWrite(32);
      genSizedMemWrite(16);
      genSizedMemWrite(8);

      // Then the simple byte-read functions.
      genByteReadWrite();

      // Generate the introspection function.
      _o << "static MemInfos initMemInfo() {\n"
         << "MemInfos info;\n";
      ForEach(memories,i) {
        const Memory &mem = *(i->_memory);
        bool shared = i->_shared;
        _o << "info.push_back(" << getMemoryName(mem) << "_info(" << (shared ? "true" : "false") << "));\n";
      }
      _o << "sort_info(info);\n"
         << "return info;\n"
         << "}\n\n"
         << "static const MemInfos &baseMemInfo() {\n"
         << "static MemInfos info = initMemInfo();\n"
         << "return info;\n"
         << "}\n\n"
         << "const MemInfos &getMemInfo() const {\n"
         << "return baseMemInfo();\n"
         << "}\n\n";

      // Function for listing local memory accesses.
      _o << "void showMem(ReportBase& rb)  {\n";
      ForEach(memories,i) {
        const Memory &mem = *(i->_memory);
        _o << getMemoryName(mem) << "_show(rb);\n";
      }
      _o << "}\n\n";

      // Generate the specific memory listing functions
      ForEach(memories,i) {
        bool shared = i->_shared;
        const Memory &mem = *(i->_memory);
        _o << "void " << getMemoryName(mem) << "_show(ReportBase& rb)  {\n"
           << "rb.report_memory(this," << mem.id() << ",\"" << getMemoryName(mem) << "\"," << shared << ");\n" 
           << "}\n\n";
      }

      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        gc_string mname  = getMemoryName(memory);
        _o << "static MemInfo " << mname << "_info(bool shared) {\n"
           << "MemInfo info;\n"
           << "info._id = " << memory.id() << ";\n"
           << "info._name = \"" << memory.name() << "\";\n"
           << "info._shared = shared;\n"
           << "info._instr_mem = " << memory.instr_mem() << ";\n"
           << "info._extern = " << extern_mem() << ";\n"
           << "info._size = " << memory.size() << ";\n"
           << "info._addr_unit = " << memory.addr_unit() << ";\n"
           << "info._parent = \"" << (memory.parent() ? memory.parent()->name() : "Self") << "\";\n"
           << "info._offset = " << memory.offset() << ";\n";
        const AttrList &attrs = memory.attrs();
        ForEach(attrs,i) {
          AttrsHelper ah(&i->second);
          ah.gen_var(_o);
          _o << "info._attrs.push_back(std::get_pair(" 
             << i->first << "," << ah.gen_constr() << "));\n";
        }
        _o << "info.finalize();\n"
           << "return info;\n"
           << "}\n\n";
      }
    }

    void genMemoryDbgFuncs(const MemoryList &memories,const gc_string &name)
    {
      // Data-side memory access.
      _o << "bool debug_data_read (uint32_t &result,unsigned id,bool trans,addr_t addr,int size) const {\n";
      if (is_system()) {
        _o << "result = getMem(id,addr,size);\n"
           << "return true;\n";
      } else {
        _o << "try {\n";
        if (!memories.empty()) {
          _o << "switch (id) {\n"
             << "case 0:\n"
             << "result = " << GlobalMemoryName << "_debug_data_read(trans,addr,size);\n"
             << "return true;\n";
          ForEach(memories,i) {
            const Memory &memory = *(i->_memory);        
            _o << "case " << memory.id() << ":\n"
               << "result = " << getMemoryName(memory) << "_get(addr,size);\n"
               << "return true;\n";
          }
          _o << "default:\n"
             << "RError(\"Non existing memory:  \" << id);\n"
             << "}\n";
        } else {
          _o << "result = " << GlobalMemoryName << "_debug_data_read(trans,addr,size);\n"
             << "return true;\n";
        }
        _o << "} catch (...) {\n"
           << "return false;\n"
           << "}\n";
      }
      _o << "}\n"
         << "\n";

      _o << "bool debug_data_write (unsigned id,bool trans,addr_t addr,uint32_t value,int size) {\n";
      if (is_system()) {
        _o << "setMem(id,addr,value,size);\n"
           << "return true;\n";
      } else {
        _o << "try {\n";
        if (!memories.empty()) {
          _o << "switch (id) {\n"
             << "case 0:\n"
             << GlobalMemoryName << "_debug_data_write(trans,addr,value,size);\n"
             << "return true;\n";
          ForEach(memories,i) {
            const Memory &memory = *(i->_memory);        
            _o << "case " << memory.id() << ":\n"
               << getMemoryName(memory) << "_set(addr,value,size);\n"
               << "return true;\n";
          }
          _o << "default:\n"
             << "RError(\"Non existing memory:  \" << id);\n"
             << "}\n";
        } else {
          _o << GlobalMemoryName << "_debug_data_write(trans,addr,value,size);\n"
             << "return true;\n";
        }
        _o << "} catch (...) {\n"
           << "return false;\n"
           << "}\n";
      }
      _o << "}\n"
         << "\n";

      // Cores have their instruction-read function defined elsewhere.
      if (is_system()) {
        // Find the memory marked as an instruction memory.
        unsigned id = 0;
        ForEach(memories,i) {
          const Memory &memory = *(i->_memory);
          if (memory.instr_mem()) {
            id = memory.id();
            break;
          }
        }

        _o << "bool debug_instr_read(uint32_t &result,bool trans,addr_t addr,int size) const\n"
           << "{\n"
           << "result = getMem(" << id << ",addr,size);\n"
           << "return true;\n"
           << "}\n\n"

           << "bool debug_instr_write(bool trans,addr_t addr,uint32_t value,int size)\n"
           << "{\n"
           << "setMem(" << id << ",addr,value,size);\n"
           << "return true;\n"
           << "}\n\n";
      }
    }

    // Generate functions accessing memories.
    void genMemoryDS(const MemoryList &memories,const gc_string &name)
    {
      ForEach(memories,i) {
        const Memory &memory = *(i->_memory);
        bool shared = i->_shared;
        genMemoryDataStructure(memory,shared);
      }
      genMemoryHelpers(memories);
      genMemoryDbgFuncs(memories,name);
    }

    // Creates the member for storing the external-memory handler, if applicable.
    void genMemHandler(const SysItem &item,const System::Items *children,const MemoryList &memories)
    {
      gc_string daname, ianame;
      gc_vector<pair<gc_string,pair<int,unsigned> > > externs;
      bool main_mem_extern = false;
      
      if (extern_mem() || extern_dmi()) {
        gc_string datype = getMemHandlerAccessType(true,logging_mem(),extern_dmi(),false,&item);
        daname = getMemAccessor(true,true,extern_dmi(),getGlobalMemRef());

        gc_string iatype = getMemHandlerAccessType(false,logging_mem(),extern_dmi(),false,&item);
        ianame = getMemAccessor(false,true,extern_dmi(),getGlobalMemRef());

        _cd._member_vars.push_back(datype + " " + daname);
        _cd._member_vars.push_back(iatype + " " + ianame);

        // If we have an external DMI item, then we can set up the external access objects here.
        if (extern_dmi()) {
          _cd._constr_inits.push_back(daname + "(&" + getExtDmiName() + ")");
          _cd._constr_inits.push_back(ianame + "(&" + getExtDmiName() + ")");
        }

        if (extern_mem()) {
          externs.push_back(make_pair(getGlobalMemRef(),make_pair(false,0)));
          main_mem_extern = true;
        }
      }

      // Handle any external "local" memories.
      ForEach(memories,i) {
        if (extern_mem(*i->_memory)) {
          const Memory &mem = *i->_memory;
          gc_string type = getMemHandlerAccessType(!mem.instr_mem(),false,false,false,&item);
          
          gc_string name = getMemAccessor(!mem.instr_mem(),true,false,mem.name());
          
          _cd._member_vars.push_back(type + " " + name);

          externs.push_back(make_pair(mem.name(),make_pair(mem.instr_mem(),mem.id())));
        }
      }

      if (!externs.empty()) {
        gc_string mtype = getMemHandlerType();

        gc_string stype = getMemWrapType(item);
        gc_string sname = getMemWrapName();

        ForEach(externs,i) {
          gc_string mname = getMemHandlerName(i->first);
          
          _cd._member_vars.push_back(mtype + " *" + mname);
          _cd._constr_inits.push_back(mname + "(0)");
        }

        _o << "bool hasExternalMem(unsigned id = 0) const {\n"
           << "switch (id) {\n";
        ForEach(externs,i) {
          _o << "case " << i->second.second << ":\n"
             << "return true;\n";
        }
        _o << "default:\n"
           << "return false;\n"
           << "}\n"
           << "}\n\n"

           << "void setMemHandler(" << mtype << " *mh,unsigned id = 0) {\n"
           << "switch (id) {\n";
        ForEach(externs,i) {
          gc_string mname = getMemHandlerName(i->first);

          _o << "case " << i->second.second << " : {\n"
             << mname << " = mh;\n";

          if (isGlobalMemName(i->first)) {
            // We guard this condition with whether we have DMI or not b/c if we
            // have DMI, then the logging memory object lives within the DMI object,
            // rather than as a member of the ISS class.
            bool lm = (logging_mem() && !extern_dmi());

            if (lm) {
              _o << sname << ".setMemHandler(mh);\n";
            }
            if (extern_dmi()) {
              _o << getExtDmiName() << ".setMemHandler(mh);\n";
            }
            if (lm) {
              _o << daname << ".setHandler(&" << sname << ");\n"
                 << ianame << ".setHandler(&" << sname << ");\n";
            }
            else if (extern_dmi()) {
              // Already handled in constructor.
            }
            else {
              _o << daname << ".setHandler(" << mname << ");\n"
                 << ianame << ".setHandler(" << mname << ");\n";
            }
          } else {
            gc_string name = getMemAccessor(!i->second.first,true,false,i->first);
            _o << name << ".setHandler(" << mname << ");\n";
          }
          _o << "} break;\n";          
        }

        _o << "default:\n"
           << "throw std::runtime_error(\"Unknown index supplied to setMemHandler.\");\n"
           << "}\n";
        
        if (children) {
          ForEach((*children),i) {
            _o << i->first << "->setMemHandler(mh);\n";
          }
        }

        _o << "}\n\n"

           << "MemHandler *getMemHandler(unsigned id) const {\n"
           << "switch (id) {\n";
        ForEach(externs,i) {
          gc_string mname = getMemHandlerName(i->first);

          _o << "case " << i->second.second << ":\n"
             << "return " << mname << ";\n";
        }
        _o << "default:\n"
           << "return 0;\n"
           << "}\n"
           << "}\n\n";

        // For safe-mode models, we use this extra wrapper around the
        // mem-handler in order to propagate latency information to a
        // performance model.
        if (logging_mem() && main_mem_extern) {
          _cd._member_vars.push_back(stype + " " + sname);
          _cd._constr_inits.push_back(sname + "(*this)");
        }

      }
    }

  };

  class GenMems : public GenMemBase {
  public:
    GenMems(opfxstream &o,
            const Core &core,
            InstrWalker &walker,
            const gc_string &filename,
            const StrSet &externs,
            StrList &end_code,
            ClassData &cd,
            const ConfigDB &config,
            const CoreConfig &coreconfig) :
      GenMemBase(o,externs,cd,config,false),
      _core(core),
      _coreconfig(coreconfig),
      _walker(walker),
      _filename(filename),
      _end_code(end_code),
      _dmi_store_attr(Data::get_attr(DmiStoreAttr))
    {};

  private:
    const Core       &_core;
    const CoreConfig &_coreconfig;
    InstrWalker      &_walker;
    const gc_string     &_filename;
    StrList          &_end_code;

    typedef gc_vector<const ParmTrans *> ParmTransV;

    ParmTransV        _parm_trans;
    int               _dmi_store_attr;

  public:

    bool mt_locked_mem() const
    {
      return _config.mt_locked_mem();
    }

    bool dmi_cache_support() const
    {
      return _coreconfig.dmi_cache_support();
    }

    // Enable the external DMI to update the internal DMI on writes if it's
    // allowed by the user or we have tags-only caches, in which case we don't
    // store any data in the caches.
    bool external_dmi_write_update() const
    {
      return _coreconfig.external_dmi_write_update() || _config.tags_only_caches();
    }

    gc_string splitSizeArg() const
    {
      if (_coreconfig.mem_split_size() == 0) {
        return "";
      } else if (_coreconfig.mem_split_size() == 8) {
        return ",8";
      } else {
        return ",4";
      }
    }

    bool log_orig_access() const
    {
      return !normal_iss() && _coreconfig.log_orig_mem_access();
    }

    bool instr_little_endian() const
    {
      return _core.instrEndianness() == Core::Little;
    }

    virtual bool data_little_endian() const
    {
      return _core.dataEndianness() == Core::Little;
    }

    void writeCurLocation()
    {
      adl::writeCurLocation(_o,_config,_filename);
    }

    void writeLineDirective(Environment *env,Ptree *expr)
    {
      adl::writeLineDirective(_o,_config,env,expr);
    }

    Ptree *funcBody(Environment *e,Ptree *f)
    {
      return adl::funcBody(_walker,e,f);
    }

    void genFunc(const gc_string &fname,Environment *env,Ptree *func)
    {
      adl::genFunc(_o,_config,_walker,_filename,fname,env,func);
    }

    unsigned packetPoolSize() const
    {
      return _config.packet_pool_size();
    }

    bool interleaved_fetch() const
    {
      if (auto mmu = _core.mmu()) {
        return mmu->interleaved_fetch();
      } 
      return true;
    }
    
    bool interleaved_reads() const
    {
      if (auto mmu = _core.mmu()) {
        return mmu->interleaved_reads();
      } 
      return true;
    }

    bool interleaved_writes() const
    {
      if (auto mmu = _core.mmu()) {
        return mmu->interleaved_writes();
      } 
      return true;
    }

    gc_string getMaskCall(const AddrMask &am,bool in_class)
    {
      gc_ostringstream ss;

      if (am.is_const()) {
        ss << "& 0x" << hex << am.initial_value() << "ULL";
      } else if (in_class) {
        ss << "& " << am.name();
      } else {
        ss << "& _core." << am.name();
      }

      return ss.str();
    }

    // Get the name of a transactional cache to use.
    gc_string getTransCacheName(const gc_string &core_name,CacheType type,bool typed_mem)
    {
      switch (type) {
      case InstrCache:
        return "TransInstrPreFetchCache<" + core_name + ">" ;
      case DataCache:
        return "TransCache<" + core_name + ">" ;
      case UnifiedCache:
        assert(0);
      }
      assert(0);
    }

    gc_string getRecordCacheName(const gc_string &core_name,CacheType type,bool typed_mem)
    {
      switch (type) {
      case InstrCache:
        break;
      case DataCache:
        if (typed_mem) {
          return "RecordCacheTyped<" + core_name + ">" ;
        } else {
          return "RecordCache<" + core_name + ">" ;
        }
      case UnifiedCache:
        assert(0);
      }
      assert(0);
    }

    void writeReadOffsetFuncs()
    {
      _o << "addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };\n"
         << "addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };\n"
         << "addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };\n\n";
    }
    
    gc_string logging_mem_mem_update(int size,CacheType type,const gc_string &access_type,bool is_write)
    {
      gc_ostringstream ss;

      // We don't want to log this stuff for instruction caches when dealing
      // with an iss-mem-mode model.
      bool add_logging = logging_mem() && (type != InstrCache || _config.log_fetches());

      if (add_logging) {
        ss << "_core.handle_memory_request(0, MemAccess(ea,ra," << size << ",";
        if (is_write) {
          switch (size) {
          case 0:
            ss << "0";
            break;
          case 1:
            ss << "1";
            break;
          default:
            ss << "nb";
            break;
          }
        } else {
          switch (size) {
          case 8:
            ss << "(BytesPerDW - (ra & DWMask))";
            break;
          case 4:
            ss << "(BytesPerWord - (ra & WordMask))";
            break;
          case 2:
            ss << "(((ra & WordMask) == 3) ? 1 : 2)";
            break;
          case 1:
            ss << "1";
            break;
          case 0:
            ss << "0";
            break;
          }
        }
        ss << "," << access_type << "));";
      }

      return ss.str();
    }

    // This returns relevant types for dealing with memory- it doesn't deal with caches.
    void getMemAccessorTypes(gc_string &accessor_name, gc_string &dbg_accessor_name, gc_string &accessor_init,
                             gc_string &dbg_accessor_init,gc_string &base_src_name,gc_string &src_name,gc_string &rf_arg0,
                             const gc_string &core_name,CacheType type,bool is_translating)
    {
      bool is_data = false;

      switch (type) {
      case InstrCache: {
        is_data = false;
      } break;
      case DataCache: {
        is_data = true;
      } break;
      case UnifiedCache:
        assert(0);
      }

      gc_string base_name = getGlobalCacheType(extern_mem(),is_data,logging_mem(),extern_dmi(),false,is_translating,&_core);
      gc_string dbg_base_name = getGlobalCacheType(extern_mem(),is_data,false,extern_dmi(),true,false,&_core);

      base_src_name = src_name = getMemAccessor(is_data,extern_mem(),extern_dmi(),getGlobalMemRef());
      if (trans_cache()) {
        accessor_name = getTransCacheName(core_name,type,_core.typed_mem());
        accessor_init =  "(*this";
        if (_core.typed_mem() && is_data) {
          accessor_init += ",type";
        }
        accessor_init += ")";
        dbg_accessor_init = "(" + getMemAccessor(is_data,extern_mem(),extern_dmi(),getGlobalMemRef()) + ")";
      } else {
        accessor_name = base_name;          
        accessor_init = "(" + getMemAccessor(is_data,extern_mem(),extern_dmi(),getGlobalMemRef());
        if (logging_mem()) {
          accessor_init += ",*this";
        }
        if (_core.typed_mem() && is_data && (extern_mem() || logging_mem())) {
          accessor_init += ",type";
        }
        accessor_init += ")";

        dbg_accessor_init = "(" + getGlobalMemObj(extern_mem(),is_data,false,extern_dmi(),&_core) + ")";
      }        
      dbg_accessor_name = dbg_base_name;

      if (extern_mem()) {
        base_src_name += ".";
        src_name = "_core." + base_src_name;
        rf_arg0 = (type == InstrCache) ? "CacheILogRead, " : "CacheLogRead, ";
      } else {
        base_src_name += ".";
        src_name += ".";
      }      
    }


    // If we're a transactional ISS, then our memory interface doesn't actually
    // interface directly with memory because we need to be able to handle
    // arbitrary delays and such.  Instead, the memory routines post an ea
    // request, then return to the calling coroutine.  When they resume, they
    // grab the data from the temporary memory page.  However, the debug
    // interface must still interact directly with memory, since we want that to
    // be transparent and not involve any simulation time consumption.
    void writeCacheAccessor(gc_string &accessor_name, gc_string &dbg_accessor_name, gc_string &accessor_init,
                            gc_string &dbg_accessor_init,gc_string &base_src_name,gc_string &src_name,
                            gc_string &rf_arg0,gc_string &res_name,const gc_string &core_name,CacheType type)
    {
      gc_string read_type, write_type;
      gc_string base_name;
      bool is_data = false;

      switch (type) {
      case InstrCache: {
        base_name = "ICache";
        read_type = "CacheIFetch";
        is_data = false;
      } break;
      case DataCache: {
        base_name = "DCache";
        read_type  = "CacheRead";
        is_data = true;
      } break;
      case UnifiedCache:
        assert(0);
      }

      // Non transactional- optionally create cache objects, if relevant.
      if (_core.has_caches(type) && !_config.skip_caches()) {

        if (trans_cache()) {
          accessor_name = getTransCacheName(core_name,type,false);
        } else {
          accessor_name = base_name;
        }

        // The force-enable flag is set to 0 so that we read the cache exactly
        // as we would from the real access.
        rf_arg0 = (type == InstrCache) ? "false,CacheILogRead, " : "false,CacheLogRead, ";

        dbg_accessor_name = "Dbg" + base_name;

        const Cache &cache = *(_core.get_first_cache(type));

        dbg_accessor_init = accessor_init = "(*this)";

        base_src_name = getCacheName(cache);
        src_name = "_core." + base_src_name;
        res_name = base_src_name + "_";

        gc_string darg;
        if (dmi_cache_support() && &cache == _core.get_first_cache(DataCache)) {
          darg = "ea,";
        }

        if (!trans_cache()) {
          // We only need to generate this interface if we're not a
          // transactional ISS, since otherwise we'll tell the pipeline model
          // to service the request.
          _o << "struct " << accessor_name << " {\n"
             << accessor_name << "(" << core_name << "&c) : _core(c) {};\n"
             << core_name << " &_core;\n"
             << "uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { " << logging_mem_mem_update(8,type,read_type,false) << " return " << src_name << "_read64(" << read_type << "," << darg << "ra); };\n"
             << "uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { " << logging_mem_mem_update(4,type,read_type,false) << "return " << src_name << "_read32(" << read_type << "," << darg << "ra); };\n"
             << "uint16_t read16(unsigned seq,addr_t ea,addr_t ra) { " << logging_mem_mem_update(2,type,read_type,false) << "return " << src_name << "_read16(" << read_type << "," << darg << "ra); };\n"
             << "uint8_t  read8 (addr_t ea,addr_t ra)              { " << logging_mem_mem_update(1,type,read_type,false) << "return " << src_name << "_read8 (" << read_type << "," << darg << "ra); };\n"  
             << "uint8_t  read0 (addr_t ea,addr_t ra)              { " << logging_mem_mem_update(0,type,read_type,false) << "return " << src_name << "_read0 (" << read_type << "," << darg << "ra); };\n"  
             << "void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { " << logging_mem_mem_update(8,type,"CacheWrite",true) << " " << src_name << "_write64(" << darg << "ra,v,nb); };\n"
             << "void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { " << logging_mem_mem_update(4,type,"CacheWrite",true) << " " << src_name << "_write32(" << darg << "ra,v,nb); };\n"
             << "void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { " << logging_mem_mem_update(2,type,"CacheWrite",true) << " " << src_name << "_write16(" << darg << "ra,v,nb); };\n"
             << "void write8 (addr_t ea,addr_t ra,uint8_t  v)          {   " << logging_mem_mem_update(1,type,"CacheWrite",true) << " " << src_name << "_write8 (" << darg << "ra,v); };\n"
             << "void write0 (addr_t ea,addr_t ra,uint8_t  v)          {   " << logging_mem_mem_update(0,type,"CacheWrite",true) << " " << src_name << "_write0 (" << darg << "ra,v); };\n";
          if (logging_mem()) {
            _o << "void readpage(byte_t *data,unsigned n,addr_t addr,addr_t crit_addr) { " << src_name << "_readpage_full(" << read_type << ",data,n,addr,crit_addr); }\n";
          }
          writeReadOffsetFuncs();
          _o << "};\n\n";
        }
        // Note: The debug-access functor accesses the cache with the
        // force-enable flag set so that we always access data if it's in the
        // cache.
        _o << "struct " << dbg_accessor_name << " {\n"
           << dbg_accessor_name << "(const " << core_name << " &c) : _core(const_cast<" << core_name << "&>(c)) {};\n"
           << core_name << " &_core;\n"
           << "uint64_t read64(unsigned seq,addr_t ea,addr_t ra) const { return " << src_name << "_log_read64(true," << read_type << ",ra); };\n"
           << "uint32_t read32(unsigned seq,addr_t ea,addr_t ra) const { return " << src_name << "_log_read32(true," << read_type << ",ra); };\n"
           << "uint16_t read16(unsigned seq,addr_t ea,addr_t ra) const { return " << src_name << "_log_read16(true," << read_type << ",ra); };\n"
           << "uint8_t  read8(addr_t ea,addr_t ra)               const { return " << src_name << "_log_read8(true," << read_type << ",ra); };\n"
           << "uint8_t  read0(addr_t ea,addr_t ra)               const { return " << src_name << "_log_read0(true," << read_type << ",ra); };\n"
           << "void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { ";
        if (!trans_cache()) {
          _o << src_name << "_log_write64(true,ra,v,nb);";
        }
        _o << " };\n"
           << "void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { ";
        if (!trans_cache()) {
          _o << src_name << "_log_write32(true,ra,v,nb);";
        }
        _o << "};\n"
           << "void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { ";
        if (!trans_cache()) {
          _o << src_name << "_log_write16(true,ra,v,nb);";
        }
        _o <<"};\n"
           << "void write8 (addr_t ea,addr_t ra,uint8_t  v)          { "; 
        if (!trans_cache()) {
          _o << src_name << "_log_write8 (true,ra,v);";
        }
        _o << "};\n"
           << "void write0 (addr_t ea,addr_t ra,uint8_t  v)          { "; 
        if (!trans_cache()) {
          _o << src_name << "_log_write0 (true,ra,v);";
        }
        _o << "};\n";
        if (logging_mem()) {
          _o << "void readpage(byte_t *data,unsigned n,addr_t addr,addr_t crit_addr) { " << src_name << "_log_readpage_full(false," << read_type << ",data,n,addr,crit_addr); }\n";
        }
        writeReadOffsetFuncs();
        _o << "};\n\n";

        // The transactional ISS accesses the logging functions within the ISS class itself, so it doesn't need the
        // core accessor appended to it.
        if (trans_cache()) {
          src_name = "const_cast<" + _core.name() + "&>(*this)." + getCacheName(cache);
        }

        src_name += "_log_";
        base_src_name += "_log_";

      } else {
        getMemAccessorTypes(accessor_name,dbg_accessor_name,accessor_init,dbg_accessor_init,base_src_name,src_name,rf_arg0,core_name,type,false);
        res_name = base_src_name;
      }
    }

    void genExtTransCall(const gc_string &type,const gc_string &ra_mask)
    {
      _o << "if (" << type << "TranslateInternalNs(ra,ea,log)) {\n"
         << "ra = ra" << ra_mask << ";\n"
         << "return true;\n"
         << "} else {\n"
         << "return false;\n"
         << "}\n";
    }

    // Generate the memory wrapper object.
    void genMemWrappers()
    {
      gc_string cn = _core.name();

      gc_string ra_mask = getMaskCall(_core.ra_mask(),false);
      gc_string ea_mask = getMaskCall(_core.ea_mask(),false);

      gc_string ra_mask_internal = getMaskCall(_core.ra_mask(),true);
      gc_string ea_mask_internal = getMaskCall(_core.ea_mask(),true);

      gc_string instr_cache, dbg_instr_cache, ic_init, dbg_ic_init,
        data_cache, dbg_data_cache, dbg_dc_init, dc_init, d_src_name, i_src_name,
        d_base_src_name, i_base_src_name, dr_arg0, ir_arg0, ires_name, dres_name;

      writeCacheAccessor(instr_cache,dbg_instr_cache,ic_init,dbg_ic_init,i_base_src_name,i_src_name,ir_arg0,ires_name,cn,InstrCache);
      writeCacheAccessor(data_cache, dbg_data_cache, dc_init,dbg_dc_init,d_base_src_name,d_src_name,dr_arg0,dres_name,cn,DataCache);

      _cd._member_vars.push_back("addr_t _lastRealAddr");
      _cd._constr_inits.push_back("_lastRealAddr(0)");
      _cd._reset_stmts.push_back("_lastRealAddr = 0");

      gc_string noinline;

      _o << "\n"
         << "//\n"
         << "// Memory access routines.\n"
         << "//\n"
         << "\n"
         << "//\n"
         << "// Utility functions for translating memory and manipulating addresses.\n"
         << "//\n"
         << "addr_t lastRealAddr() const { return _lastRealAddr; };\n"
         << "\n"
         << "addr_t dataReadTranslate(addr_t ea,int seq) " << attr_used << "{\n";
      if (dmi_cache_support()) {
        _o << "_lastEffectiveAddr = ea;\n";
      }
      _o << "return _lastRealAddr = (dataReadTranslateInternal(ea,seq) " << ra_mask << ");\n"
         << "}\n"
         << "\n"
         << "template <size_t Nb>\n"
         << "addr_t dataReadTranslate(const bits<Nb> &ea) { return dataReadTranslate(ea.uint64(),0); }\n"
         << "\n"
         << "addr_t dataWriteTranslate(addr_t ea,int seq) " << attr_used << "{\n";
      if (dmi_cache_support()) {
        _o << "_lastEffectiveAddr = ea;\n";
      }
      _o << "return _lastRealAddr = (dataWriteTranslateInternal(ea,seq) " << ra_mask << ");\n"
         << "}\n"
         << "\n"
         << "template <size_t Nb>\n"
         << "addr_t dataWriteTranslate(const bits<Nb> &ea) { return dataWriteTranslate(ea.uint64(),0); }\n"
         << "\n"
         << "addr_t instrReadTranslate(addr_t ea,int seq) " << attr_used << "{\n"
         << "return _lastRealAddr = (instrReadTranslateInternal(ea,seq) " << ra_mask << ");\n"
         << "}\n"
         << "\n"
         << "template <size_t Nb>\n"
         << "addr_t instrReadTranslate(const bits<Nb> &ea) { return instrReadTranslate(ea.uint64(),0); }\n"
         << "\n"
        // For use by users in action code.  We force a no-inline for JIT models to reduce translated code size.
         << "addr_t dataReadTranslate(addr_t ea) " << attr_used << "{\n"
         << "return dataReadTranslate(ea,0);\n"
         << "}\n"
         << "\n"
         << "addr_t dataWriteTranslate(addr_t ea) " << attr_used << "{\n"
         << "return dataWriteTranslate(ea,0);\n"
         << "}\n"
         << "\n"
         << "addr_t instrReadTranslate(addr_t ea) " << attr_used << "{\n"
         << "return instrReadTranslate(ea,0);\n"
         << "}\n"
         << "\n"
         << "virtual bool extDataReadTranslate(addr_t &ra,addr_t ea,bool log = false) const {\n";
      genExtTransCall("dataRead",ra_mask);
      _o << "}\n"
         << "virtual bool extDataWriteTranslate(addr_t &ra,addr_t ea,bool log = false) const {\n";
      genExtTransCall("dataWrite",ra_mask);
      _o << "}\n"
         << "virtual bool extInstrReadTranslate(addr_t &ra,addr_t ea,bool log = false) const {\n";
      genExtTransCall("instrRead",ra_mask);
      _o << "}\n"
         << "\n"
         << "addr_t mask_ea(addr_t ea) { return ea " << ea_mask_internal << "; };\n"
         << "addr_t mask_ra(addr_t ra) { return ra " << ra_mask_internal << "; };\n"
         << "//\n"
         << "// Functors for performing translations.\n"
         << "//\n"
         << "struct InstrReadTranslate {\n"
         << "InstrReadTranslate(" << cn << " &c) : _core(c) {};\n"
         << "addr_t operator()(addr_t addr,int seq) {\n"
         << "return _core.instrReadTranslate(addr,seq);\n"
         << "}\n"
         << "addr_t mask_ea(addr_t ea) {\n"
         << "return ea " << ea_mask << ";\n"
         << "}\n"
         << "addr_t scale_ea(addr_t ea) { return ea; }\n"
         << "void aligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_read(addr_t ea,addr_t ra) { }\n"
         << "void post_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_write(addr_t ea,addr_t ra) { }\n"
         << "void post_write(addr_t ea,addr_t ra) { }\n"
         << cn << " &_core;\n"
         << "};\n"
         << "\n"
         << "struct DataReadTranslate {\n"
         << "DataReadTranslate(" << cn << " &c) : _core(c) {};\n"
         << "addr_t operator()(addr_t addr,int seq) {\n"
         << "return _core.dataReadTranslate(addr,seq);\n"
         << "}\n"
         << "addr_t mask_ea(addr_t ea) {\n"
         << "return ea " << ea_mask << ";\n"
         << "}\n"
         << "addr_t scale_ea(addr_t ea) { return ea; }\n"
         << "void aligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_read(addr_t ea,addr_t ra) {\n"
         << "_core.misaligned_read_handler(ea,ra);\n"
         << "}\n"
         << "void pre_read(addr_t ea,addr_t ra) {\n"
         << "_core.pre_read_handler(ea,ra);\n"
         << "}\n"
         << "void post_read(addr_t ea,addr_t ra) {\n"
         << "_core.post_read_handler(ea,ra);\n"
         << "}\n"
         << "void pre_write(addr_t ea,addr_t ra) { }\n"
         << "void post_write(addr_t ea,addr_t ra) { }\n"
         << cn << " &_core;\n"
         << "};\n"
         << "\n"
         << "struct DataWriteTranslate {\n"
         << "DataWriteTranslate(" << cn << " &c) : _core(c) {};\n"
         << "addr_t operator()(addr_t addr,int seq) {\n"
         << "return _core.dataWriteTranslate(addr,seq);\n"
         << "}\n"
         << "addr_t mask_ea(addr_t ea) {\n"
         << "return ea " << ea_mask << ";\n"
         << "}\n"
         << "addr_t scale_ea(addr_t ea) { return ea; }\n"
         << "void aligned_write(addr_t ea,addr_t ra) {\n"
         << "_core.aligned_write_handler(ea,ra);\n"
         << "}\n"
         << "void misaligned_write(addr_t ea,addr_t ra) {\n"
         << "_core.misaligned_write_handler(ea,ra);\n"
         << "}\n"
         << "void misaligned_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_read(addr_t ea,addr_t ra) { }\n"
         << "void post_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_write(addr_t ea,addr_t ra) {\n"
         << "_core.pre_write_handler(ea,ra);\n"
         << "}\n"
         << "void post_write(addr_t ea,addr_t ra) {\n"
         << "_core.post_write_handler(ea,ra);\n"
         << "}\n"
         << cn << " &_core;\n"
         << "};\n"
         << "\n"

         << "//\n"
         << "// No-side-effect versions of functors for performing translations.\n"
         << "//\n"
         << "struct InstrReadTranslateNs {\n"
         << "InstrReadTranslateNs(const " << cn << " &c) : _core(c) {};\n"
         << "addr_t operator()(addr_t addr,int seq) {\n"
         << "addr_t ra;\n"
         << "if (_core.extInstrReadTranslate(ra,addr,false)) {\n"
         << "return ra;\n"
         << "} else {\n"
         << "RError(\"Could not translate 0x\" << addr);\n"
         << "}\n"
         << "}\n"
         << "addr_t mask_ea(addr_t ea) {\n"
         << "return ea " << ea_mask << ";\n"
         << "}\n"
         << "addr_t scale_ea(addr_t ea) { return ea; }\n"
         << "void aligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_read(addr_t ea,addr_t ra) { }\n"
         << "void post_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_write(addr_t ea,addr_t ra) { }\n"
         << "void post_write(addr_t ea,addr_t ra) { }\n"
         << "const " << cn << " &_core;\n"
         << "};\n"
         << "\n"
         << "struct DataReadTranslateNs {\n"
         << "DataReadTranslateNs(const " << cn << " &c) : _core(c) {};\n"
         << "addr_t operator()(addr_t addr,int seq) {\n"
         << "addr_t ra;\n"
         << "if (_core.extDataReadTranslate(ra,addr,false)) {\n"
         << "return ra;\n"
         << "} else {\n"
         << "RError(\"Could not translate 0x\" << addr);\n"
         << "}\n"
         << "}\n"
         << "addr_t mask_ea(addr_t ea) {\n"
         << "return ea " << ea_mask << ";\n"
         << "}\n"
         << "addr_t scale_ea(addr_t ea) { return ea; }\n"
         << "void aligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_read(addr_t ea,addr_t ra) { }\n"
         << "void post_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_write(addr_t ea,addr_t ra) { }\n"
         << "void post_write(addr_t ea,addr_t ra) { }\n"
         << "const " << cn << " &_core;\n"
         << "};\n"
         << "\n"
         << "struct DataWriteTranslateNs {\n"
         << "DataWriteTranslateNs(const " << cn << " &c) : _core(c) {};\n"
         << "addr_t operator()(addr_t addr,int seq) {\n"
         << "addr_t ra;\n"
         << "if (_core.extDataWriteTranslate(ra,addr,false)) {\n"
         << "return ra;\n"
         << "} else {\n"
         << "RError(\"Could not translate 0x\" << addr);\n"
         << "}\n"
         << "}\n"
         << "addr_t mask_ea(addr_t ea) {\n"
         << "return ea " << ea_mask << ";\n"
         << "}\n"
         << "addr_t scale_ea(addr_t ea) { return ea; }\n"
         << "void aligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_write(addr_t ea,addr_t ra) { }\n"
         << "void misaligned_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_read(addr_t ea,addr_t ra) { }\n"
         << "void post_read(addr_t ea,addr_t ra) { }\n"
         << "void pre_write(addr_t ea,addr_t ra) { }\n"
         << "void post_write(addr_t ea,addr_t ra) { }\n"
         << "const " << cn << " &_core;\n"
         << "};\n"
         << "\n";

      // There's no point in translating if we don't have any real pages, since
      // we're doing debug accesses.
      bool has_real_pages = hasRealPages(_core);

      _o << "virtual uint32_t " << GlobalMemoryName << "_debug_data_read(bool trans,addr_t addr,int size) const\n"
         << "{\n"
         << "NullReadLog logger;\n"
         << dbg_data_cache << " cache" << dbg_dc_init << ";\n"
         << "if (trans && " << has_real_pages << ") {\n"
         << "DataReadTranslateNs translate(*this);\n"
         << "return mem_read_sized<DataReadTranslateNs," << dbg_data_cache << ",NullReadLog," << data_little_endian() << ">(translate,cache,logger,addr,size);\n"
         << "} else {\n"
         << "NullTranslate translate;\n"
         << "return mem_read_sized<NullTranslate," << dbg_data_cache << ",NullReadLog," << data_little_endian() << ">(translate,cache,logger,addr,size);\n"
         << "}\n"
         << "};\n"

         << "virtual void " << GlobalMemoryName << "_debug_data_write(bool trans,addr_t addr,uint32_t data,int size)\n"
         << "{\n"
         << "NullWriteLog logger;\n"
         << dbg_data_cache << " cache" << dbg_dc_init << ";\n"
         << "if (trans && " << has_real_pages << ") {\n"
         << "DataWriteTranslateNs translate(*this);\n"
         << "mem_write_sized<DataWriteTranslateNs," << dbg_data_cache << ",NullWriteLog," << data_little_endian() << ">(translate,cache,logger,addr,data,size);\n"
         << "} else {\n"
         << "NullTranslate translate;\n"
         << "mem_write_sized<NullTranslate," << dbg_data_cache << ",NullWriteLog," << data_little_endian() << ">(translate,cache,logger,addr,data,size);\n"
         << "}\n"
         << "};\n\n";

      gc_string dr_func, ir_func, dw_func, res_func;
      if (transactional_iss()) {
        // If this is a transactional ISS, then memory logging queries are
        // performed through this virtual function.  We provide a default implementation.
        // TODO: should be const (non-const now for ASTC)
        if (_coreconfig.virtual_mem_interface()) {
          _o << "virtual uint32_t getMemoryData32(CacheAccess type, addr_t addr)\n"
             << "{\n";
          if (ir_func == dr_func && ir_arg0 == dr_arg0) {
            _o << "return " << i_base_src_name << "read32(" << ir_arg0 << "addr);\n";
          } else {
            _o << "if (type == CacheILogRead) {\n"
               << "return " << i_base_src_name << "read32(" << ir_arg0 << "addr);\n"
               << "} else {\n"
               << "return " << d_base_src_name << "read32(" << dr_arg0 << "addr);\n"
               << "}\n";
          }
          _o << "}\n\n";
        } else {
          _o << "uint32_t getMemoryData32(CacheAccess type, addr_t addr);\n\n";
        }

        dr_func = "_core.getMemoryData32";
        ir_func = "_core.getMemoryData32";

        dr_arg0 = "CacheLogRead,";
        ir_arg0 = "CacheILogRead,";
      } else {
        dr_func = d_src_name+"read32";
        ir_func = i_src_name+"read32";
        dw_func = d_src_name+"write32";
        res_func= dres_name+"write32";
      }

      gc_string x_init = "(*this)";

      genMemoryFuncs(0,GlobalMemoryName,MemoryName,
                     data_cache,data_cache,instr_cache,
                     dc_init,dc_init,ic_init,
                     dbg_instr_cache,dbg_ic_init,
                     "DataReadTranslate","DataWriteTranslate","InstrReadTranslate",
                     x_init,x_init,x_init,
                     dr_func,dr_arg0,ir_func,ir_arg0,dw_func,res_func,
                     !_core.has_instr_memory(),_core.get_memory(MemoryName));     
    }
  
    // This generates a memory hook function and returns true if the function
    // exists or false otherwise.
    bool genMemoryHook(const MemoryHook *hook,const gc_string &hookname,bool dmi_check) 
    {
      if (!hook) {
        return false;
      }

      Ptree *func = hook->action();
      Ptree *args = funcArgs(func);
      Ptree *new_args = removeArgN(args,0);

      Ptree *size_arg  = First(args);
      Ptree *size_name = Second(size_arg)->Car();
        
      _o << "template <" << size_arg << ">\n"
         << "bits<" << size_name << "> " << hookname << " (" << new_args << ") ";
      writeLineDirective(hook->env(),func);
      if (dmi_check) {
        _walker.set_trans_parms(ptRecord);
      }
      _o << funcBody(hook->env(),func) << "\n";
      writeCurLocation();
      _o << "\n";

      if (dmi_check) {
        // If relevant, save the parameters we recorded.
        if (ParmTrans *tp = _walker.get_parm_trans()) {
          // Store the translated parms for later, when we create the DMI cache.
          _parm_trans.push_back(tp);
          // Look for any other parameters that we need to store.
          if (_dmi_store_attr >= 1) {
            ForEach(_core.parms(),i) {
              const Parm &p = *(i->second);
              if (p.has_attr(_dmi_store_attr)) {
                tp->insert(make_pair(p.name(),PItem(0,&p)));
              }
            }
          }
        }
      }
      _walker.set_trans_parms(ptNone);
      return true;
    }

    // Write the function which reads an integer for instruction decoding.
    void writeInstrReadFunc(const gc_string &pfx,IssType iss_type,bool has_instr_hook,const gc_string &name,
                            const gc_string &ilname,const gc_string &icname,const gc_string &ic_init,
                            const gc_string &xiname,const gc_string &xi_init)
    {
      gc_string read_call = "ma.read32(translate,cache,logger,translate.scale_ea(ea))";

      _o << "uint32_t instr_read32" << pfx << " (addr_t ea)\n"
         << "{\n";
      if (iss_type == tNormal) {
        // Normal ISS: Read 32 bits using the normal memory hierarchy.
        _o << xiname << " translate" << xi_init << ";\n"
           << ilname << " logger(*this);\n"
           << icname << " cache" << ic_init << ";\n"
           << "MemoryAccess<" << xiname << "," << icname << "," << ilname << "," << interleaved_reads() << "> ma;\n";
      } else {
        if (_config.trans_split_fetch()) {
          // Transactional ISS, but fetching directly using the address, so data might be split if misaligned.
          _o << xiname << " translate" << xi_init << ";\n"
             << ilname << " logger(*this);\n"
             << "TransInstrFetchCache<" << _core.name() << "> cache(*this);\n"
             << "MemoryAccess<" << xiname << ",TransInstrFetchCache<" << _core.name() << ">," << ilname << ",true> ma;\n";
        } else {
          // Transactional ISS: Read from the internal buffer.  We have to do
          // another read, for tracing purposes, though, in order to get the
          // proper instruction memory reads.
          _o << "if (Tracing && _trace_mode) {\n"
             << xiname << " translate" << xi_init << ";\n"
             << ilname << " logger(*this);\n"
             << "NullReadCache cache;\n"
             << "MemoryAccess<" << xiname << ",NullReadCache," << ilname << ",true> ma;\n"
             << read_call << ";\n"
             << "}\n";
          _o << "NullTranslate translate;\n"
             << "NullReadLog logger;\n"
             << "TransInstrFetchCache<" << _core.name() << "> cache(*this);\n"
             << "MemoryAccess<NullTranslate,TransInstrFetchCache<" << _core.name() << ">,NullReadLog,true> ma;\n";

          // Use 0 as an address, since we don't want alignment issues to throw
          // things off; we're just reading from a buffer, so alignment doesn't
          // matter.
          read_call = "ma.read32(translate,cache,logger,translate.scale_ea(0))";
        }
      }

      if (instr_little_endian()) {
        read_call = "swap_uint32(" + read_call + ")";
      }

      if (has_instr_hook) {
        _o << "return " << getInstrHookName(name) << "<32>(" << read_call << ").uint32();\n";
      } else {
        _o << "return " << read_call << ";\n";
      }
      _o << "}\n\n";
    }

    void writeMemCommitCode(const gc_string &name,const gc_string &sfx,const gc_string &addr,
                            const gc_string &NB,const gc_string &Nb,const gc_string &core,
                            const gc_string &queue,const gc_string &delay,
                            const gc_string &type_parm = gc_string(),const gc_string &type_arg = gc_string())
    {
      gc_string commitWrite = _core.name() + "::" + name + sfx;
      gc_string delay_p = (delay.empty()) ? "" : ","+delay;

      gc_string targ;
      if (!NB.empty()) {
        commitWrite += "<" + NB + ">";
        targ = NB + "*8";
      } else {
        targ = Nb;
      }
      gc_string t;
      if (!type_parm.empty()) {
        t = "Typed";
      }

      // The user shouldn't specify NB and Nb- one or the other.
      assert (! ( !NB.empty() && !Nb.empty() ) );
      
      _o << "typedef void (" << _core.name() << "::*Functor) (addr_t,const bits<" << targ << ">&" << type_parm << ");\n"
         << "Functor pCommit = &" << commitWrite << ";\n"
         << "static PacketPool<Commit" << t << "MemWrite<" << _core.name() << "*,Functor," << targ << "," << packetPoolSize() << ">, " << packetPoolSize() << ">   pool;\n"
         <<  "Commit" << t << "MemWrite<" << _core.name() << "*,Functor," << targ << "," << packetPoolSize() << "> *cmt = pool.alloc();\n"
         << "cmt->reset(" << core << ",pCommit," << addr << ",value" << type_arg << ",pool);\n"
         << queue << ".add(cmt" << delay_p << ");\n";
    }

    void writeRollbackMemSave(const gc_string &name,const gc_string &dr_func,const gc_string &dr_arg0)
    {
      if (rollback()) {
        _o << "if (pre && _core." << getRollbackEnableName() << ") {\n"
           << "bits<32> value = " << dr_func << "(" << dr_arg0 << " ra & ~WordMask);\n";
        writeMemCommitCode(name,"_restore_write","(ra&~WordMask)","","32","&_core","_core."+getRestoreStackName(),"");
        _o << "}\n";
      }
    }

    void writeRestoreFunc(const gc_string &name,const gc_string &dw_func)
    {
      if (rollback()) {
        _o << "void " << name << "_restore_write(addr_t ra,const bits<32> &x) {\n";
        if (!dw_func.empty()) {
          _o << dw_func << "(ra,x.uint32(),4);\n";
        }
        _o << "}\n\n";
      }
    }

    // Generates the direct-memory interface cache, which stores pointers to
    // memory blocks based upon effective-addresses in order to be fast.  This
    // only handles aligned accesses.
    void genDmiCache(const gc_string &name,const gc_string &type)
    {
      gc_string cn = getDmiCacheName(name,type);

      _cd._member_vars.push_back("DmiCache " + cn);
      _cd._reset_stmts.push_back(cn + ".reset()");
    }

    // Returns true if instruction attributes need to be checked by the dmi
    // cache.  We currently do not cache instructions which have instruction
    // attributes, in order to simplify the matching criteria.
    bool checkDmiUseInstrAttr()
    {
      bool has_multi_sets = false;
      if (const MMU *mmu = _core.mmu()) {
        ForEach(mmu->all_lookups(),i) {
          if (!i->second->is_interface()) {
            if ( i->second->tests().size() > 1) {
              has_multi_sets = true;
              break;
            }
          }
        }
      }

      const IntSet &instr_attrs = _core.instr_attrs();
      
      return (has_multi_sets && !instr_attrs.empty());
    }

    void genDmiParmUpdate(bool has_hook,unsigned p_index,const gc_string &cn)
    {
      if (has_hook) {
        if (_parm_trans.size() > p_index && !_parm_trans[p_index]->empty()) {
          // If we have virtualized parameters, then update them here.  If we
          // do, then we have to invalidate the normal translation load/store cache, since this
          // might set up a conflicting set.
          if (!(*_parm_trans[p_index]).empty()) {
            _o << "core.checkDataPerms();\n";
          }
          ForEach((*_parm_trans[p_index]),i) {
            _o << "core." << i->first << " = (" << getParmType(*(i->second._p)) << ")core." << cn << "._items[table_index]." << getDmiParmName(i->first) << ";\n";
          }
        }
      }
    }

    // We only do internal DMI updates if:
    // 1.  We don't have caches.  Otherwise, the cache updates the DMI cache.
    // 2.  We don't have an external DMI, otherwise the external DMI update causes an udpate here.
    // 3.  We don't have external memory.  Otherwise, the external memory interface must update the cache.
    bool update_dmi_internally(bool has_data_cache)
    {
      return !has_data_cache && !extern_dmi() && !(extern_mem() && dmi_cache_support());
    }

    // We only update latency values on a DMI hit if we don't have a data cache
    // and thus this DMI is pointing into external memory (including the
    // external DMI) and we are a time-tagged ISS.
    bool update_latencies(bool has_data_cache)
    {
      return !has_data_cache && time_tagged();
    }

    void genDmiCacheRead(const gc_string &name,unsigned size,bool has_data_cache,bool check_instr_attr,
                         bool has_read_hook,unsigned p_index)
    {
      gc_string cn = getDmiCacheName(name,"read");
      gc_string un = getDmiCacheUpdateName(name,"read");

      // For 8-bit accesses, we don't care about misaligned addresses.  For
      // everything else make sure that the lower bits (based upon access size)
      // are clear.  Thus, the comparison mask will look at the relevant lower
      // bits.
      gc_string mask;
      if (size == 1) {
        mask = "DmiCache::CompMask";
      } else {
        MkGStr(mask, "(DmiCache::CompMask | " << (size-1) << ")");
      }

      gc_string instr_attr_check;
      if (check_instr_attr) {
        instr_attr_check = "!core." + getInstrAttrGetter() + "() && ";
      }

      _o << "template <class Core,class Translate,class CacheAccess,class Logger,bool Interleaved>\n"
         << "struct dmi_read_internal<Core,Translate,CacheAccess,Logger," << size << ",Interleaved> {\n"
         << "mem_bits<" << size * 8 << "> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea) const " << attr_used << "\n"
         << "{\n"

        // We index into the table using the *last* address of the access, e.g. +
        // 3 for a 32-bit read, so that we catch any page-crossing situations.
         << "unsigned table_index = ((ea + " << size << " - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;\n"
        
        // Remember: The value we store in the mem pointer is the difference
        // between the guest ea and the host address, so that translation to the
        // true host address can be done simply through addition.  Note: We have
        // to be careful here with the types we use.  We have to force the mask
        // to be a long long in order for full 64b addresses to be handled
        // correctly.
         << "byte_t *p = (ea & ~((addr_t)DWMask)) + core." << cn << "._items[table_index]._mem;\n"

        // Then check to see if this address actually matches.  We have to watch
        // for instruction-attributes so that we don't improperly alias and we
        // look at the lsb in order to catch misaligned addresses, if we're not
        // accessing on a per-byte basis.
         << "if ( " << instr_attr_check << " (((ea ^ core." << cn << "._items[table_index]._ea) & " << mask << ") == 0)) {\n"
         << getAddrCheckCall(_core,"ea","make_addr(core."+cn+"._items[table_index]._ra,DmiCache::CompMask,ea)","core.","","",aLoad);
      genDmiParmUpdate(has_read_hook,p_index,cn);
      if (update_latencies(has_data_cache)) {
        _o << "core._latency += core." << getDmiReadLatency() << ";\n";
      }
      _o << "return mem_bits<" << size*8 << ">(byte_read" << size*8 << "(p,(ea & DWMask)));\n"
         << "} else {\n"
         << "read_internal<Translate,CacheAccess,Logger," << size << ",Interleaved> ri;\n"
         << "mem_bits<" << size*8 << "> x = ri(translate,cache,logger,ea);\n";
      // Update the cache.  In some situations, the read itself will update the
      // cache, so this update will be a no-op.  This is true for hardware data
      // caches, which can directly do the update themselves, and for when we
      // have an external dmi cache, in which case the user's DMI update updates
      // the internal DMI cache.  Also, we only do the update if no instruction
      // attributes are set, so as to only cache "normal" addresses.  Finally,
      // remember that the address we store is actually the difference between
      // the actual address and the ea, so that translation can be performed via
      // an add.
      if (update_dmi_internally(has_data_cache)) {
        _o << "core." << un << "(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));\n";
      }
      _o << "return x;\n"
         << "}\n"
         << "}\n"
         << "};\n\n";
    }

    void genDmiCacheWrite(const gc_string &name,unsigned size,bool has_data_cache,bool check_instr_attr,
                          bool has_write_hook,unsigned p_index)
    {
      gc_string cn = getDmiCacheName(name,"write");
      gc_string un = getDmiCacheUpdateName(name,"write");

      // For 8-bit accesses, we don't care about misaligned addresses.  For
      // everything else make sure that the lower bits (based upon access size)
      // are clear.  Thus, the comparison mask will look at the relevant lower
      // bits.
      gc_string mask;
      if (size == 1) {
        mask = "DmiCache::CompMask";
      } else {
        MkGStr(mask, "(DmiCache::CompMask | " << (size-1) << ")");
      }
      
      gc_string instr_attr_check;
      if (check_instr_attr) {
        instr_attr_check = "!core." + getInstrAttrGetter() + "() && ";
      }

      _o << "template <class Core,class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved>\n"
         << "struct dmi_write_internal<Core,Translate,CacheAccess,Logger," << size << ",Nb,Interleaved> {\n"
         << "void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,mem_bits<Nb> value) const" << attr_used << "\n"
         << "{\n"

        // We index into the table using the *last* address of the access, e.g. +
        // 3 for a 32-bit read, so that we catch any page-crossing situations.
         << "unsigned table_index = ((ea + " << size << " - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;\n"

        // Remember: The value we store in the mem pointer is the difference
        // between the guest ea and the host address, so that translation to the
        // true host address can be done simply through addition.
         << "byte_t *p = (ea & ~((addr_t)DWMask)) + core." << cn << "._items[table_index]._mem;\n"

        // Then check to see if this address actually matches.  We have to watch
        // for instruction-attributes so that we don't improperly alias and we
        // look at the lsb in order to catch misaligned addresses, if we're not
        // accessing on a per-byte basis.
         << "if ( " << instr_attr_check << "(((ea ^ core." << cn << "._items[table_index]._ea) & " << mask << ") == 0)) {\n"
         << getAddrCheckCall(_core,"ea","make_addr(core."+cn+"._items[table_index]._ra,DmiCache::CompMask,ea)","core.","","",aStore);
      if (update_latencies(has_data_cache)) {
        _o << "core._latency += core." << getDmiWriteLatency() << ";\n";
      }
      genDmiParmUpdate(has_write_hook,p_index,cn);
      if (has_write_hook) {
        _o << "value = core. template " << getWriteHookName(name) << "<" << size*8 << ">(value);\n";
      }
      _o << "byte_write" << size*8 << "(p,(ea & DWMask),value.uint" << ((size > 4) ? 64 : 32) << "());\n"
         << "} else {\n"
        // Didn't match, so call the normal access routine.
         << "write_internal<Translate,CacheAccess,Logger," << size <<"," << size*8 << ",Interleaved> wi;\n"
         << "ea = translate.mask_ea(ea);\n"
         << "addr_t ra = translate(ea,0);\n"
         << "translate.aligned_write(ea,ra);\n";
      if (has_write_hook) {
        _o << "value = core. template " << getWriteHookName(name) << "<" << size*8 << ">(value);\n";
      }
      _o << "wi(translate,cache,logger,ea,ra,value);\n";
        // Then do the update. See the read routine above for details about the
        // update.
      if (update_dmi_internally(has_data_cache)) {
        _o << "core." << un << "(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));";
      }
      _o << "}\n"
         << "}\n"
         << "};\n\n";
    }

    // Writes an update function for the DMI cache.  If we have instruction
    // attributes, we make sure that none are set when we cache the value, in
    // order to simplify the matching criteria.
    void genDmiCacheUpdate(const gc_string &name,const gc_string &type,bool check_instr_attr,unsigned p_index)
    {
      gc_string cn = getDmiCacheName(name,type);
      gc_string un = getDmiCacheUpdateName(name,type);
      gc_string in = getDmiCacheInvalidateName(name,type);

      _o << "// Update an entry in the DMI cache.\n"
         << "void " << un << "(addr_t ea,addr_t ra,byte_t *p) {\n";
      if (_config.debug_mode()) {
        _o << "if (" << getDmiReservedCheck(type) << "(ea)) return;\n";
      }
      if (check_instr_attr) {
        _o << "if (" << getInstrAttrGetter() << "()) return;\n";
      }
      _o << "unsigned table_index = (ea >> DmiCache::PageShift) & DmiCache::IndexMask;\n"
         << "addr_t ea_page = ea & DmiCache::CompMask;\n"
         << cn << "._items[table_index]._ea = ea_page;\n"
         << cn << "._items[table_index]._mem = (p - ea_page);\n";
      if (_core.addr_check_handler()) {
        _o << "addr_t ra_page = ra & DmiCache::CompMask;\n"
           << cn << "._items[table_index]._ra = ra_page;\n";
      }

      // Then list any virtualized parameters which need to be saved.
      if (_parm_trans.size() > p_index && !_parm_trans[p_index]->empty()) {
        ForEach((*_parm_trans[p_index]),i) {
          _o << cn << "._items[table_index]." << getDmiParmName(i->first) << " = " << i->first << ";\n";
        }
      }

      _o << "}\n\n";

      gc_string instr_attr_check;
      if (check_instr_attr) {
        instr_attr_check = "!" + getInstrAttrGetter() + "() && ";
      }

      // Note: We always invalidate the entry, even though the address might not
      // match, to handle the situation where aliasing in a hardware cache
      // causes a line to be replaced.  In that case, we must still invalidate
      // the DMI entry so that we don't accidentally think that the old address
      // is still in the hardware cache.

      _o << "// Invalidate an entry, if it exists, in the DMI cache.\n"
         << "void " << in << "(addr_t ea) {\n"
         << "unsigned table_index = (ea >> DmiCache::PageShift) & DmiCache::IndexMask;\n"
         << cn << "._items[table_index]._ea =  0x" << hex << ((addr_t)-1) << dec << "ull - (table_index  * DmiCache::PageSize);\n"
         << "}\n\n";
    }

    void genDmiCaches(const gc_string &name,bool has_read_hook,bool has_write_hook)
    {
      if (dmi_cache_support() && isGlobalMemName(name)) {

        bool has_data_cache = false;
        unsigned pagesize = DefaultDmiPageSize;
        // The page size of the DMI cache is the minimum of DefaultDmiPageSize,
        // the smallest translation, or the size of a cache line.
        if (_core.has_caches(DataCache)) {
          const Cache &cache = *(_core.get_first_cache(DataCache));
          has_data_cache = true;
          pagesize = min(pagesize,cache.linesize());
        }
        else if (const MMU *mmu = _core.mmu()) {
          ForEach( (mmu->top_lookups()), i) {
            pagesize = min(pagesize,i->second->min_page_size());
          }
        }

        // The amount to shift an address in order to eliminate the offset
        // portion.
        unsigned pageshift = log2(pagesize);

        if (!is_power_2(_config.dmi_cache_size())) {
          RError("DMI cache size must be a power of 2, but was set to " << _config.dmi_cache_size());
        }

        // The mask to use to eliminate the upper portion, in order to extract
        // the cache index.
        unsigned indexmask = _config.dmi_cache_size() - 1;

        // For masking out the offset and set portions of an address.
        addr_t mask = ((addr_t)-1) << (pageshift);

        bool check_instr_attr = checkDmiUseInstrAttr();

        // Note: The memory pointers stored in this cache are *not* the actual
        // pointer to memory, but are instead the difference between the guest ea and the 
        // host memory, so that the actual address is ea + _mem.
        _o << "struct DmiItem {\n" << dec
           << "addr_t  _ea;\n";
        if (_core.addr_check_handler()) {
          // If we have this handler, we need to store the ra, as it requires it.
          _o << "addr_t  _ra;\n";
        }
        _o << "byte_t *_mem;\n";

        // If we have any parameters which must be virtualized, then we write them out here.
        // First, we coalesce them into a unique set.
        StrSet parms;
        ForEach(_parm_trans,pt_iter) {
          ForEach ( (**pt_iter), i ) {
            parms.insert(i->first);
          }
        }
        ForEach(parms,i) {
          _o << "int " << getDmiParmName(*i) << ";\n";
        }

        _o << "};\n\n"
           << "struct DmiCache {\n"
           << "enum { PageSize  = " << pagesize << ",\n"
           << "       PageShift = " << pageshift << ",\n"
           << "       IndexMask = " << indexmask << ",\n"
           << "       CompMask  = " << hex << "0x" << mask << "ull,\n"
           << "};\n\n" << dec
           << "bool    _enable; // Note:  Only used for reset.\n"
           << "DmiItem _items[" << _config.dmi_cache_size() << "];\n"
           << "\n"
           << "DmiCache() { reset(); }\n"
           << "void enable() { _enable = true; };\n"
           << "void disable() { _enable = false; };\n"
           << "void reset() {\n"
           << "addr_t bad = 0x" << hex << ((addr_t)-1) << "ull;\n" << dec
           << "for (int i = 0; i != " << _config.dmi_cache_size() << "; ++i, bad -= PageSize) {\n"
          // This should be an impossible value, so we'll never match in the cache.
           << "_items[i]._ea = bad;\n"
           << "_items[i]._mem = 0;\n"
					 << "_enable = false;\n"
           << "}\n"
           << "}\n"
           << "};\n\n";

        _cd._member_vars.push_back("addr_t _lastEffectiveAddr");
        _cd._constr_inits.push_back("_lastEffectiveAddr(0)");
        _cd._reset_stmts.push_back("_lastEffectiveAddr = 0");

        genDmiCache(name,"write");
        genDmiCache(name,"read");

        if (update_latencies(has_data_cache)) {
          _cd._member_vars.push_back("unsigned " + getDmiReadLatency());
          _cd._constr_inits.push_back(getDmiReadLatency() + "(0)");

          _cd._member_vars.push_back("unsigned " + getDmiWriteLatency());
          _cd._constr_inits.push_back(getDmiWriteLatency() + "(0)");
        }

        // Wrapper classes for the DMI cache.  First, the generic version which
        // just ignores the DMI cache.  Then we have the specializations for
        // common sizes which do use the cache.
       _o << "template <class Core,class Translate,class CacheAccess,class Logger,size_t NB,bool Interleaved>\n"
          << "struct dmi_read_internal {\n"
          << "mem_bits<NB*8> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t addr) const {\n"
          << "read_internal<Translate,CacheAccess,Logger,NB,Interleaved> ri;\n"
          << "return ri(translate,cache,logger,addr);\n"
          << "}\n"
          << "};\n\n";

        // Note: Which element (the last parmater, 0 in this case) corresponds
        // to which hook is a little fragile- it's the order in which the hook
        // generation functions are called.

        genDmiCacheRead(name,8,has_data_cache,check_instr_attr,has_read_hook,0);
        genDmiCacheRead(name,4,has_data_cache,check_instr_attr,has_read_hook,0);
        genDmiCacheRead(name,2,has_data_cache,check_instr_attr,has_read_hook,0);
        genDmiCacheRead(name,1,has_data_cache,check_instr_attr,has_read_hook,0);

        genDmiCacheUpdate(name,"read",check_instr_attr,0);

        _o << "template <class Core,class Translate,class CacheAccess,class Logger,size_t NB,size_t Nb,bool Interleaved>\n"
           << "struct dmi_write_internal {\n"
           << "void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,const mem_bits<Nb> &value) " << attr_used << "{\n"
           << "write_internal<Translate,CacheAccess,Logger,NB,Nb,Interleaved> wi;\n"
           << "ea = translate.mask_ea(ea);\n"
           << "addr_t ra = translate(ea,0);\n"
           << "translate.aligned_write(ea,ra);\n";
        gc_string varg = "value";
        if (has_write_hook) {
          varg = "value2";
          _o << "mem_bits<Nb> " << varg << " = core. template " << getWriteHookName(name) << "<NB*8>(value);\n";
        }
        _o << "wi(translate,cache,logger,ea,ra," << varg << ");\n"
           << "}\n"
           << "};\n\n";

        genDmiCacheWrite(name,8,has_data_cache,check_instr_attr,has_write_hook,1);
        genDmiCacheWrite(name,4,has_data_cache,check_instr_attr,has_write_hook,1);
        genDmiCacheWrite(name,2,has_data_cache,check_instr_attr,has_write_hook,1);
        genDmiCacheWrite(name,1,has_data_cache,check_instr_attr,has_write_hook,1);

        genDmiCacheUpdate(name,"write",check_instr_attr,1);

        _o << "void " << getDmiCacheInvalidateName(name) << "(addr_t ea) {\n"
           << getDmiCacheInvalidateName(name,"read") << "(ea);\n"
           << getDmiCacheInvalidateName(name,"write") << "(ea);\n"
           << "}\n\n";
      }
    }

    // Generate a data memory access funcs
    void genMemoryFuncs(unsigned id,const gc_string &name,const gc_string &log_name,
                        const gc_string &dcrname,const gc_string &dcwname,const gc_string &icname,
                        const gc_string &dcr_init,const gc_string &dcw_init,const gc_string &ic_init,
                        const gc_string &dbg_icname,const gc_string &dbg_ic_init,
                        const gc_string &xrname,const gc_string &xwname,const gc_string &xiname,
                        const gc_string &xr_init,const gc_string &xw_init,const gc_string &xi_init,
                        const gc_string &dr_func,const gc_string &dr_arg0,
                        const gc_string &ir_func,const gc_string &ir_arg0,
                        const gc_string &dw_func,const gc_string &res_func,
                        bool is_instr_mem,const Memory *memory)
    {
      gc_string ilname  = "Log" + name + "InstrRead";
      gc_string rlname  = "Log" + name + "MemoryRead";
      gc_string wlname  = "Log" + name + "MemoryWrite";
      gc_string rwpname = "Log" + name + "WpRead";
      gc_string wwpname = "Log" + name + "WpWrite";

      gc_string cn        = _core.name();

      gc_string init_str, update_str, record_str;

      if (trans_non_logging_iss()) {
        init_str   = "_count(0), ";
        update_str = "ea = _info[_count].first; ra = _info[_count].second; ++_count;\n";
        record_str = "void set_addr(int seq,addr_t ea,addr_t ra) { _info[seq] = std::make_pair(ea,ra); }\n"
                     "std::pair<addr_t,addr_t> _info[4];\n"
                     "unsigned _count;\n";
      }

      // General logging functors.
      _o << "\n"
         << "struct " << rlname << " {\n"
         <<  rlname << "(" << cn << " &c) : " << init_str << "_core(c) {};\n"
         << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {\n"
         << "if (Tracing && (_core.trace_mode() & ttMemRead)) {\n"
         << update_str
         << "unsigned tf = _core.trace_mode();\n"
         << "_core.get_logger()->log_mem_read(" << id <<  ",\"" << log_name << "\",pre,seq,ea & ~WordMask,ra & ~WordMask," << dr_func << "(" << dr_arg0 << " ra & ~WordMask));\n"
         << "_core.set_trace_mode(tf);\n"
         << "}\n";
      if (_config.debug_mode()) {
        _o << "_core.check_read_watchpoints(ea,pre,num_bytes);\n";
      }
      _o << "};\n"
         << record_str
         << cn << " &_core;\n"
         << "};\n"
         << "\n" ;

      _o << "struct " << wlname << " {\n"
         << wlname << "(" << cn << " &c) : " << init_str << "_core(c) {};\n"
         << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {\n";
      // Add rollback support for memory operations, if applicable.
      writeRollbackMemSave(name,dr_func,dr_arg0);
      _o << "if (Tracing && (_core.trace_mode() & ttMemWrite)) {\n"
         << update_str
         << "unsigned tf = _core.trace_mode();\n"
         << "_core.get_logger()->log_mem_write(" << id <<  ",\"" << log_name << "\", pre, seq, ea & ~WordMask,ra & ~WordMask," << dr_func << "(" << dr_arg0 << " ra & ~WordMask));\n"
         << "_core.set_trace_mode(tf);\n"
         << "}\n";

      if (_config.debug_mode()) {
        _o << "_core.check_write_watchpoints(ea,pre,num_bytes,value);\n";
      }
      _o << "};\n"
         << record_str
         << cn << " &_core;\n"
         << "};\n" 
         << "\n";

      // For a transactional ISS, the first logger records ea/ra pairs so that
      // we don't have to worry about translating them again in the second pass.
      if (trans_non_logging_iss()) {
        _o << "template <class L>\n"
           << "struct RecordLog {\n"
           << "RecordLog(" << cn << " &c,L &l) : _core(c), _l(l), _count(0) {};\n"
           << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {\n"
           << "if (Tracing && (_core.trace_mode() & ttMemWrite)) {\n"
           << "_l.set_addr(_count++,ea,ra);\n"
           << "}\n"
           << "};\n"
           << cn << " &_core;\n"
           << "L &_l;\n"
           << "unsigned _count;\n"
           << "};\n\n";
      }

      // Watch-point only logging functors.  Only needed if we're generating a
      // JIT or trace-cache model.
      if (jit_support() && _config.debug_mode()) {
        _o << "\n"
           << "struct " << rwpname << " {\n"
           <<  rwpname << "(" << cn << " &c) : _core(c) {};\n"
           << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {\n"
           << "_core.check_read_watchpoints(ea,pre,num_bytes);\n"
           << "};\n"
           << cn << " &_core;\n"
           << "};\n"
           << "\n"

           << "struct " << wwpname << " {\n"
           << wwpname << "(" << cn << " &c) : _core(c) {};\n"
           << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {\n"
           << "_core.check_write_watchpoints(ea,pre,num_bytes,value);\n"
           << "};\n"
           << cn << " &_core;\n"
           << "};\n" 
           << "\n";
      }

      // We generate an instruction logger unless the user has explicitly stated
      // that this should not be done.  This implies that the external user,
      // e.g. a performance model, will take care of calling log_instr_read.
      _o << "struct " << ilname << " {\n"
         << ilname << "(" << cn << " &c) : _core(c) {};\n"
         << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {\n"
         << "if (Tracing && (_core.trace_mode() & ttInstrRead) && !pre) {\n"
         << "unsigned tf = _core.trace_mode();\n"
         << "_core.get_logger()->log_instr_read(" << id <<  ",\"" << log_name << "\", ea & ~WordMask,ra & ~WordMask," << ir_func << "(" << ir_arg0 << " ra & ~WordMask));\n"
         << "_core.set_trace_mode(tf);\n"
         << "}\n"
         << "};\n"
         << cn << " &_core;\n"
         << "};\n" 
         << "\n";
      if (!_config.log_instr_reads()) {
        // If the performance model will be calling the instruction-logging function, then this is the entry-point.
        _o << "void log_instr_read(addr_t ea,addr_t ra) {\n"
           << ilname << " logger(*this);\n"
           << "logger(false,0,ea,ra,4,0);\n"
           << "}\n\n"
           << "void log_clear_instr_reads() {\n"
           << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
           << "_logger->log_clear_instr_reads();\n"
           << "}\n"
           << "}\n\n";
      }

      bool has_read_hook = false, has_write_hook = false, has_instr_hook = false, is_prefetch_buffer = false;
      int  prefetch_size = 0;
      if (memory) {
        has_read_hook  = genMemoryHook(memory->read_hook(),getReadHookName(name),dmi_cache_support());
        has_write_hook = genMemoryHook(memory->write_hook(),getWriteHookName(name),dmi_cache_support());
        has_instr_hook = genMemoryHook(memory->instr_read_hook(),getInstrHookName(name),false);
        is_prefetch_buffer = memory->prefetch_mem();
        prefetch_size = memory->size();
      }

      // If we're storing info about the top-level, original memory access, then
      // we need the member variables and accessors.
      if (log_orig_access()) {
        _cd._member_vars.push_back("addr_t _orig_mem_addr");
        _cd._member_vars.push_back("unsigned _orig_mem_size");
        _cd._constr_inits.push_back("_orig_mem_addr(0)");
        _cd._constr_inits.push_back("_orig_mem_size(0)");
        _cd._reset_stmts.push_back("_orig_mem_addr = 0");
        _cd._reset_stmts.push_back("_orig_mem_size = 0");

        _o << "void set_orig_access_info(addr_t addr,unsigned nb) { _orig_mem_addr = addr; _orig_mem_size = nb; }\n\n"
           << "addr_t get_orig_access_addr() const { return _orig_mem_addr; }\n\n"
           << "unsigned get_orig_access_size() const { return _orig_mem_size; }\n\n";
      }

      // This exists for any non-normal ISS.  Safe-mode calls this to let the
      // performance know about memory requests, even though it gets its memory
      // directly from the outside.
      if (!normal_iss()) {
        _o << "// This must be implemented by the performance model for handling memory requests.\n";
        if (_coreconfig.virtual_mem_interface()) {
          _o << "virtual void handle_memory_request(u_int memId, const MemAccess &ma) = 0;\n\n"
             << "virtual void enable_mem_logging(bool) {};\n\n";
        } else {
          _o << "void handle_memory_request(u_int memId, const MemAccess &ma);\n\n"
             << "void enable_mem_logging(bool);\n\n";
        }

        // The do_memory_request function, for a transactional, blocking memory
        // interface, is when the memory action should be taken.  The access is
        // recorded via handle_memory_request.
        if (_config.mem_if() == mBlocking) {
          if (_coreconfig.virtual_mem_interface()) {
            _o << "virtual void do_memory_operation() = 0;\n\n";
          } else {
            _o << "void do_memory_operation();\n\n";
          }
        }

        // For safe-mode models, this is how we propagate latencies to the performance model.
        if (_coreconfig.ignore_latency()) {
          // In this case, we're ignoring latencies, e.g. for a safe-mode model
          // w/o the iss-mem-mode option.  This way, the overhead in
          // LoggingMemWrapper will be ignored.
          _o << "void handle_memory_latency(u_int memId, CacheAccess type,addr_t addr,u_int size,const MemHandler::Latencies &latency) {}\n\n";
        } else {
          _o << "// Overload this function in order to receive latency information for memory acesses.\n";
          if (_coreconfig.virtual_mem_interface()) {
            _o << "virtual void handle_memory_latency(u_int memId, CacheAccess type,addr_t addr,u_int size,const MemHandler::Latencies &latency) {};\n\n";
          } else {
            _o << "void handle_memory_latency(u_int memId, CacheAccess type,addr_t addr,u_int size,const MemHandler::Latencies &latency);\n\n";
          }
        }
      }

      if ((transactional_iss() && !logging_mem()) || skip_caches_in_exec()) {
        // If we have a transactional ISS then we must add support for the
        // coroutine-based memory interaction.
        _cd._member_vars.push_back("MemoryRequest<"+cn+","+uitogs(_config.trans_ifetch_accesses())+"> _memory_request");
        _cd._constr_inits.push_back("_memory_request(*this)");
        _cd._reset_stmts.push_back("_memory_request.reset()");

        // Are we using a blocking or non-blocking memory interface?  If
        // blocking, then don't do anything- this is handled via
        // handle_memory_request separately.  For blocking memory, we call a
        // virtual function.  Note: I intend to remove the blocking interface at
        // some point, since it's not used by uADL at all.
        _o << "void process_memory_request() {\n";
        if (_config.mem_if() == mBlocking) {
          // Blocking- call a virtual function.
          _o << "do_memory_operation();\n";
        } else if (_config.mem_if() == mNonBlocking) {
          // Non-blocking- use coroutines to swap.
          _o << "_co->resume();\n";
        }
        _o << "}\n\n"

           << "static unsigned max_mem_requests() { return " << _config.trans_ifetch_accesses() << "; };\n\n"
           << "const MemAccess &memory_request(unsigned seq) const { return _memory_request.access(seq); };\n\n"
           << "void set_memory_result(unsigned seq,byte_t *page,unsigned offset) { _memory_request.set_result(seq,page,offset); }\n\n";
      }
      
      gc_string rlogger, wlogger;
      if (is_prefetch_buffer) {
        rlname = "NullReadLog"; // do not log reads to this memory
        rlogger = rlname + " logger";
        wlname = "NullWriteLog";
        wlogger = wlname + " logger";
      } else {
        rlogger = rlname + " logger(*this)";
        wlogger = wlname + " logger(*this)";
      }

      // Generate the main read/write interface routines used by action code.

      genDmiCaches(name,has_read_hook,has_write_hook);

      // If DMI support is enabled, then we use it for the memory interface for the JIT.
      if (jit_support()) {
        gc_string f_pfx;
        gc_string f_sfx = getJitMemSuffix();

        // Do not log reads to this memory.  Check watch-points if we have
        // debug-mode enabled.
        gc_string rlname = (_config.debug_mode()) ? rwpname : "NullReadLog";
        gc_string rlogger = rlname + " logger(*this)";
        gc_string wlname = (_config.debug_mode()) ? wwpname : "NullWriteLog";
        gc_string wlogger = wlname + " logger(*this)";
        
        genReadWriteInterface(id,name,f_pfx,f_sfx,_config.trace_mode(),log_name,dcrname,dcwname,dcr_init,dcw_init,xrname,xwname,
                              xr_init,xw_init,rlogger,wlogger,rlname,wlname,is_prefetch_buffer,prefetch_size,
                              has_read_hook,has_write_hook,dmi_cache_support());
      }

      // No dmi usage here, since we want full logging when using the interpreter.
      genReadWriteInterface(id,name,gc_string(),gc_string(),!is_prefetch_buffer,log_name,dcrname,dcwname,
                            dcr_init,dcw_init,xrname,xwname,xr_init,xw_init,rlogger,wlogger,rlname,wlname,
                            is_prefetch_buffer,prefetch_size,has_read_hook,has_write_hook,false);

      if (is_instr_mem) {

        // The transactional ISS can read an arbitrary amount of instruction
        // info at a time, so we use the normal read_internal function, almost
        // the same as a data access.  The instr_read32 function then just
        // manipulates the next word that needs to be fetched.
        if (!normal_iss()) {
          if (!logging_mem()) {
            gc_string logger;
            gc_string logger_arg;
            if (_config.log_instr_reads()) {
              logger =  ilname;
              logger_arg = "(*this)";
            } else {
              logger = "NullReadLog";
            }
            _o << "template <size_t NB>\n"
               << "void instr_read (addr_t ea)\n"
               << "{\n"
               << xiname << " translate" << xi_init << ";\n"
               << logger << " logger " << logger_arg << ";\n"
               << icname << " cache" << ic_init << ";\n"
               << "read_internal<" << xiname << "," << icname << "," << logger << ",NB," << interleaved_fetch() << splitSizeArg() << "> ri;\n"
               << "ri(translate,cache,logger,ea);\n"
               << "};\n";
          } else {
            // A safe-mode ISS also has an arbitrary-size instruction-read, but
            // this actually reads data.  Use the standard instruction cache
            // name for normal safe-mode models, but for iss-mem-mode, since we
            // have real caches, use the debug cache in order to skip erroneous
            // cache accesses that will mess up logging.
            gc_string cname, c_init;
            if (logging_mem() && !_config.skip_caches()) {
              cname = dbg_icname;
              c_init = dbg_ic_init;
            } else {
              cname = icname;
              c_init = ic_init;
            }
            _o << "template <size_t NB>\n"
               << "void instr_read (byte_t *data,addr_t &ra,addr_t ea)\n"
               << "{\n"
               << "ra = swizzle_addr<NB>(instrReadTranslate(ea));\n"
               << cname << " cache" << c_init << ";\n"
               << "cache.readpage(data,NB,ra,ra);\n"
               << "};\n";
          }
        }

        if (logging_mem()) {
          // If in safe-mode, also write a simple read function.
          writeInstrReadFunc("",tNormal,has_instr_hook,name,ilname,icname,ic_init,xiname,xi_init);
        } else {
          writeInstrReadFunc("",_config.iss_type(),has_instr_hook,name,ilname,icname,ic_init,xiname,xi_init);
        }

        if (jit_support()) {
          gc_string last_ra = getJitLastRaMember();
          
          _cd._member_vars.push_back("addr_t " + last_ra);
          _cd._reset_stmts.push_back(last_ra + " = 0");
          _cd._constr_inits.push_back(last_ra + "(0)");

          // If compiling JIT support, we add in this version, which can fetch
          // instructions w/o going through the cache.

          gc_string cname = _core.name();
          _o << "struct JitInstrMemLog {\n"
             << "JitInstrMemLog(" << _core.name() << " &core) : _core(core) {};\n"
             << "void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) { if (seq == 0) { _core." << last_ra << " = ra; } };\n"
             << _core.name() << " &_core;\n"
             << "};\n\n";
          gc_string imemname, imeminit;
          if (isGlobalMemName(name)) {
            // If global memory, then we need to figure out the relevant parameters for accessing memory directly, w/o using the cache.
            gc_string imemdbgname, imemdbginit,bsname,sname,rf0;
            getMemAccessorTypes(imemname,imemdbgname,imeminit,imemdbginit,bsname,sname,rf0,cname,InstrCache,true);
          } else {
            // If a local memory, then we don't use caches, so it's no problem.
            imemname = icname;
            imeminit = ic_init;
          }
          writeInstrReadFunc("_nocache",tNormal,has_instr_hook,name,"JitInstrMemLog",imemname,imeminit,xiname,xi_init);
        }

        _o << "uint32_t debug_instr_read_internal(addr_t addr,bool trans,int size) const\n"
           << "{\n";
        if (is_global_mem(id)) {
          // Global memory might be translated, so we have to handle that case.
          _o << "NullReadLog logger;\n"
             << "if (trans) {\n"
             << xiname << "Ns translate" << xi_init << ";\n"
             << dbg_icname << " cache" << dbg_ic_init << ";\n"
             << "return mem_read_sized<" << xiname << "Ns," << dbg_icname << ",NullReadLog," << data_little_endian() << ">(translate,cache,logger,addr,size);\n"
             << "} else {\n"
             << "NullTranslate translate;\n"
             << dbg_icname << " cache" << dbg_ic_init << ";\n"
             << "return mem_read_sized<NullTranslate," << dbg_icname << ",NullReadLog," << data_little_endian() << ">(translate,cache,logger,addr,size);\n"
             << "}\n";
        } else {
          // If this isn't global memory, then we don't do any translations, so the function can be simpler.
          _o << "return " << name << "_get(addr,size);\n";
        }
        _o << "}\n\n"

           << "bool debug_instr_read(uint32_t &result,bool trans,addr_t addr,int size) const\n"
           << "{\n"
           << "try {\n"
           << "result = debug_instr_read_internal(addr,trans,size);\n"
           << "return true;\n"
           << "}\n"
           << "catch (...) {\n"
           << "return false;\n"
           << "}\n"
           << "}\n"
           << "\n" 
           << "bool debug_instr_write(bool trans,addr_t addr,uint32_t value,int size)\n"
           << "{\n"
           << "try {\n";
        if (is_global_mem(id)) {
          // Global memory might be translated, so we have to handle that case.
          _o << "NullWriteLog logger;\n"
             << "if (trans) {\n"
             << xiname << " translate" << xi_init << ";\n"
             << dbg_icname << " cache" << dbg_ic_init << ";\n"
             << "mem_write_sized<" << xiname << "," << dbg_icname << ",NullWriteLog," << data_little_endian() << ">(translate,cache,logger,addr,value,size);\n"
             << "} else {\n"
             << "NullTranslate translate;\n"
             << dbg_icname << " cache" << dbg_ic_init << ";\n"
             << "mem_write_sized<NullTranslate," << dbg_icname << ",NullWriteLog," << data_little_endian() << ">(translate,cache,logger,addr,value,size);\n"
             << "}\n";
        } else {
          // If this isn't global memory, then we don't do any translations, so the function can be simpler.
          _o << name << "_set(addr,value,size);\n";
        }
        _o << "return true;\n"
           << "}\n"
           << "catch (...) {\n"
           << "return false;\n"
           << "}\n"
           << "}\n"
           << "\n" ;
      }

      if (is_prefetch_buffer) {
        gc_string ctype = getCacheType(*memory,false);
        _o << "uint32_t prefetch_instr_read32 (addr_t ea)\n"
           << "{\n"
           << xiname << " translate" << xi_init << ";\n"
           << rlogger << ";\n"
           << ctype <<  "  cache" << ic_init << ";\n"
           << "MemoryAccess<" << xiname << "," << ctype << "," << rlname << "," << interleaved_reads() << "> ma;\n"
           << "return ma.read32(translate,cache,logger,(ea-_prefetch_start+_prefetch_idx)%" << prefetch_size << ");\n"
           << "}\n"
           << "\n";
      }

      writeRestoreFunc(name,res_func);

    }

    void writeIgnoreWriteCheck()
    {
      if (ignore_mode()) {
        _o << "if (" << getIgnoreEnableName() << ") return;\n";
      }
    }

    void writeIgnoreReadCheck()
    {
      if (ignore_mode()) {
        _o << "if (" << getIgnoreEnableName() << ") return 0;\n";
      }
    }

    void genMemReadCall(const gc_string &name,const gc_string &record_call,bool has_read_hook,const gc_string &dmi_core_arg,
                        const gc_string &xrvar,const gc_string &bswap)
    {
      _o << record_call;

      if (has_read_hook) {
        _o << "return " << getReadHookName(name) << "<NB*8>(ri(" << dmi_core_arg << xrvar << ",cache,logger,ea)" << bswap << ");\n";
      } else {
        _o << "return ri(" << dmi_core_arg << xrvar << ",cache,logger,ea)" << bswap <<";\n";
      }
    }
    
    void genMemReadFuncs(unsigned id,const gc_string &name,
                         const gc_string &f_pfx,const gc_string &f_sfx,
                         bool log_mem,const gc_string &log_name,
                         const gc_string &dcrname,const gc_string &dcr_init,
                         gc_string xrname,const gc_string &xr_init,
                         const gc_string &rlogger,const gc_string &rlname,
                         bool is_prefetch_buffer,int prefetch_size,bool has_read_hook,

                         const gc_string &read_type_parm,const gc_string &cw_parms,
                         const gc_string &cw_log_arg,
                         bool call_typed_logger,const gc_string &logger_read_expr,
                         const gc_string &cast_type_arg,const gc_string &dmi_prefix,
                         const gc_string &dmi_core_type,const gc_string &dmi_core_arg,
                         const gc_string &bswap)
    {
      _o << "template <size_t NB>\n"
         << f_pfx << " bits<NB*8> " << name << "_read" << f_sfx << " (addr_t ea" << read_type_parm << cw_parms << ")\n"
         << "{\n";

      gc_string xrvar = "translate";
      if (rw_locked_mem()) _o << "AdlSimLock<boost::mutex> mem_lock(" << getMemMutex() << ");\n";
      writeIgnoreReadCheck();
      _o << xrname  << " " << xrvar << xr_init << ";\n"
         << dcrname  << " cache" << dcr_init << ";\n"
         << rlogger << ";\n";
      if (log_mem) {
        _o << "if (Tracing && adl_unlikely(_trace_mode & ttCoreMemRead)) {\n";
        if (have_dt()) {
          _o << getDepVar() << ".check_src(DepItem::dpMem," << id << ",-1,++" << getMemSeqVar() << ");\n";
        }
        if (call_typed_logger) {
          _o << "if (" << logger_read_expr << ") {\n"
             << "_logger->log_core_mem_read_typed(" << id <<  ",\"" << log_name << "\"," << cw_log_arg << ",NB" << cast_type_arg << ");\n"
             << "} else {\n"
             << "_logger->log_core_mem_read(" << id <<  ",\"" << log_name << "\",ea,NB);\n"
             << "}\n";
        } else {
          _o << "_logger->log_core_mem_read(" << id <<  ",\"" << log_name << "\",ea,NB);\n";
        }
        _o << "}\n";
      }
      if (log_orig_access()) {
        _o << "set_orig_access_info(ea,NB);\n";
      }

      // The transactional ISS needs a second set of functors for performing the
      // initial recording operations.
      gc_string record_call;
      if (trans_non_logging_iss()) {
        gc_string dcrname_r = getRecordCacheName(_core.name(),DataCache,_core.typed_mem());
        gc_string rlname_r  = "RecordLog<" + rlname + ">";
        gc_string xrname_r  = xrname;
        xrname = "NullTranslate";
        xrvar = "translate_n";
        
        _o << dcrname_r << " cache_r" << dcr_init << ";\n"
           << rlname_r  << ' ' << "logger_r(*this,logger);\n"
           << xrname    << ' ' << xrvar << ";\n"
        
           << "read_internal<" << xrname_r << "," << dcrname_r << "," << rlname_r << ",NB," << interleaved_reads() << splitSizeArg() << "> ri_r;\n";

        record_call = "ri_r(translate,cache_r,logger_r,ea);\n";
      }

      _o << dmi_prefix << "read_internal<" << dmi_core_type << xrname << "," << dcrname << "," << rlname << ",NB," 
         << interleaved_reads() << splitSizeArg() << "> ri;\n";
      
      if (is_prefetch_buffer) {
        _o << "ea = (ea - _prefetch_start + _prefetch_idx)%" << prefetch_size << ";\n";
      }
      _o << "ea = translate.scale_ea(ea);\n";
      if (mt_support()) {
        gc_string f = (mt_locked_mem()) ? "true" : getAtomicFlag();
        _o << "if (ThreadedSim) {\n"
           << "LockMemory(" << getMemLocks() << "," << f << ",dataReadTranslate(ea));\n";
        genMemReadCall(name,record_call,has_read_hook,dmi_core_arg,xrvar,bswap);
        _o << "} else {\n";
      }

      genMemReadCall(name,record_call,has_read_hook,dmi_core_arg,xrvar,bswap);

      if (mt_support()) {
        _o << "}\n";
      }
      
     
      _o << "};\n"
         << "\n";
    }

    void genMemWriteCall(const gc_string &record_call,const gc_string &dmi_core_arg,
                         const gc_string &xwvar,const gc_string &ra_arg,const gc_string &bswap)
    {
      _o << record_call
         << "wi(" << dmi_core_arg << xwvar << ",cache,logger,ea," << ra_arg << "value" << bswap <<");\n";
    }
    
    // This writes the body of the actual write function, which might be in a
    // commit-write function, if relevant, or just in the normal body of the
    // write function.
    void genMemWriteBody(unsigned id,const gc_string &name,
                          const gc_string &f_pfx,const gc_string &f_sfx,
                          bool log_mem,const gc_string &log_name,
                          const gc_string &dcwname,const gc_string &dcw_init,
                          gc_string xwname,const gc_string &xw_init,
                          const gc_string &wlogger,const gc_string &wlname,
                          bool is_prefetch_buffer,bool has_write_hook,bool use_dmi,

                          const gc_string &write_type_parm,const gc_string &cw_parms,
                          const gc_string &cw_log_arg,const gc_string &cw_t_args,
                          const gc_string &commit_write_parm,const gc_string &type_arg,
                          bool call_typed_logger,const gc_string &logger_write_expr,
                          const gc_string &cast_type_arg,const gc_string &dmi_prefix,
                          const gc_string &dmi_core_type,const gc_string &dmi_core_arg,
                          const gc_string &ra_arg,const gc_string &bswap,bool delayed_write)
    {
      gc_string xwvar = "translate";

      _o << xwname  << ' ' << xwvar << xw_init << ";\n"
         << dcwname << " cache" << dcw_init << ";\n"
         << wlogger  << ";\n";

      if (rw_locked_mem()) _o << "AdlSimLock<boost::mutex> mem_lock(" << getMemMutex() << ");\n";
      if (log_mem) {
        _o << "if (Tracing && adl_unlikely(_trace_mode & ttCoreMemWrite)) {\n";
        if (have_dt()) {
          _o << getDepVar() << ".check_trg(DepItem::dpMem," << id << ",-1);\n";
        }
        if (call_typed_logger) {
          _o << "if (" << logger_write_expr << ") {\n"
             << "_logger->log_core_mem_write_typed(" << id <<  ",\"" << log_name << "\"," << cw_log_arg << ",NB" << cast_type_arg << ");\n"
             << "} else {\n"
             << "_logger->log_core_mem_write(" << id <<  ",\"" << log_name << "\",ea,NB);\n"
             << "}\n";
        } else {
          _o << "_logger->log_core_mem_write(" << id <<  ",\"" << log_name << "\",ea,NB);\n";
        }
        _o << "}\n";
      }
      if (log_orig_access()) {
        _o << "set_orig_access_info(ea,NB);\n";
      }

      // The transactional ISS needs a second set of functors for performing the
      // initial recording operations.
      gc_string record_call;
      if (trans_non_logging_iss()) {
        gc_string dcwname_r = getRecordCacheName(_core.name(),DataCache,_core.typed_mem());
        gc_string wlname_r  = "RecordLog<" + wlname + ">";
        gc_string xwname_r  = xwname;
        xwname = "NullTranslate";
        xwvar = "translate_n";
        
        _o << dcwname_r << " cache_r" << dcw_init << ";\n"
           << wlname_r  << ' ' << "logger_r(*this,logger);\n"
           << xwname    << ' ' << xwvar << ";\n"
        
           << "write_internal<" << xwname_r << "," << dcwname_r << "," << wlname_r << ",NB,NB*8,"
           << interleaved_reads() << splitSizeArg() << "> wi_r;\n";

        record_call = "wi_r(translate,cache_r,logger_r,ea," + ra_arg + "value" + bswap + ");\n";
      }

      _o << dmi_prefix << "write_internal<" << dmi_core_type << xwname << "," << dcwname << "," << wlname << ",NB,NB*8," 
         << interleaved_writes() << splitSizeArg() << "> wi;\n";
      if (!use_dmi) {
        _o << "ea = translate.mask_ea(translate.scale_ea(ea));\n"
           << "addr_t ra = translate(ea,0);\n"
           << "translate.aligned_write(ea,ra);\n";
      }
      if (has_write_hook && !use_dmi) {
        // We don't call the write hook for DMI, as that's done by the write
        // routines, in order to handle stored parameters.
        _o << "value = " << getWriteHookName(name) << "<NB*8>(value);\n";
      }
      if (mt_support()) {
        gc_string f = (mt_locked_mem()) ? "true" : getAtomicFlag();
        _o << "if (ThreadedSim) {\n"
           << "LockMemory(" << getMemLocks() << "," << f << ",dataWriteTranslate(ea));\n";
        genMemWriteCall(record_call,dmi_core_arg,xwvar,ra_arg,bswap);
        _o << "} else {\n";
      }

      genMemWriteCall(record_call,dmi_core_arg,xwvar,ra_arg,bswap);

      if (mt_support()) {
        _o << "}\n";
      }
    }

    void genMemWriteFuncs(unsigned id,const gc_string &name,
                          const gc_string &f_pfx,const gc_string &f_sfx,
                          bool log_mem,const gc_string &log_name,
                          const gc_string &dcwname,const gc_string &dcw_init,
                          const gc_string &xwname,const gc_string &xw_init,
                          const gc_string &wlogger,const gc_string &wlname,
                          bool is_prefetch_buffer,bool has_write_hook,bool use_dmi,

                          const gc_string &write_type_parm,const gc_string &cw_parms,
                          const gc_string &cw_log_arg,const gc_string &cw_t_args,
                          const gc_string &commit_write_parm,const gc_string &type_arg,
                          bool call_typed_logger,const gc_string &logger_write_expr,
                          const gc_string &cast_type_arg,const gc_string &dmi_prefix,
                          const gc_string &dmi_core_type,const gc_string &dmi_core_arg,
                          const gc_string &ra_arg,const gc_string &bswap,bool delayed_write)
    {
      bool need_commit_logic = false;
      bool wrote_par_logic = false;
      // First, handle the normal write, which is either a simple write, or
      // delayed for a parallel architecture.
      _o << "template <size_t NB>\n"
        << f_pfx << " void " << name << "_write" << f_sfx << " (addr_t ea,bits<NB*8> value" << write_type_parm << cw_parms << ")\n"
        << "{\n";
      writeIgnoreWriteCheck();
      if(!transactional_iss() && !is_prefetch_buffer && _core.parallel_execution()) {
        writeMemCommitCode(name,"_commit_write","ea","NB","","this",getCommitQueueName(),"0",commit_write_parm,cast_type_arg);
        _o << "}\n"
           << "\n";
        need_commit_logic = true;
        wrote_par_logic = true;
      } else {
        // Write a standard write body for a non-parallel architecture, even if
        // we have delayed writes, since we might have a mix of delayed and
        // non-delayed writes.
        genMemWriteBody(id,name,f_pfx,f_sfx,log_mem,log_name,dcwname,dcw_init,xwname,xw_init,
                        wlogger,wlname,is_prefetch_buffer,has_write_hook,use_dmi,
                        
                        write_type_parm,cw_parms,cw_log_arg,cw_t_args,commit_write_parm,type_arg,
                        call_typed_logger,logger_write_expr,cast_type_arg,dmi_prefix,dmi_core_type,
                        dmi_core_arg,ra_arg,bswap,delayed_write);
        _o << "};\n"
           << "\n";
      }

      if (delayed_write) {
        // If we have delayed writes, then we also need the delayed-write
        // function, regardless of it being a parallel architecture or not.
        _o << "template <size_t NB>\n"
           << f_pfx << " void " << name << "_write_delayed" << f_sfx << " (addr_t ea,bits<NB*8> value,unsigned delay" << write_type_parm << cw_parms << ")\n"
           << "{\n";
        writeIgnoreWriteCheck();
        writeMemCommitCode(name,"_commit_write","ea","NB","","this",getCommitQueueName(),"delay",commit_write_parm,cast_type_arg);
        _o << "}\n";

        // And we always need commit code for delayed writes.
        need_commit_logic = true;
      }

      if(need_commit_logic) {
        _o << "template <size_t NB>\n"
           << "void " << name << "_commit_write (addr_t ea,const bits<NB*8> &value" << write_type_parm << cw_parms << ")\n"
           << "{\n";
        genMemWriteBody(id,name,f_pfx,f_sfx,log_mem,log_name,dcwname,dcw_init,xwname,xw_init,
                        wlogger,wlname,is_prefetch_buffer,has_write_hook,use_dmi,
                        
                        write_type_parm,cw_parms,cw_log_arg,cw_t_args,commit_write_parm,type_arg,
                        call_typed_logger,logger_write_expr,cast_type_arg,dmi_prefix,dmi_core_type,
                        dmi_core_arg,ra_arg,bswap,delayed_write);
        _o  << "};\n"
            << "\n";
      }
    }
    
    void genReadWriteInterface(unsigned id,const gc_string &name,
                               const gc_string &f_pfx,const gc_string &f_sfx,
                               bool log_mem,const gc_string &log_name,
                               gc_string dcrname,gc_string dcwname,
                               const gc_string &dcr_init,const gc_string &dcw_init,
                               const gc_string &xrname,const gc_string &xwname,
                               const gc_string &xr_init,const gc_string &xw_init,
                               const gc_string &rlogger,const gc_string &wlogger,
                               const gc_string &rlname,const gc_string &wlname,
                               bool is_prefetch_buffer,int prefetch_size,
                               bool has_read_hook,bool has_write_hook,
                               bool use_dmi)
    {
      gc_string dmi_prefix, dmi_core_type, dmi_core_arg;
      gc_string ra_arg = "ra,";
      if (use_dmi) {
        dmi_prefix = "dmi_";
        dmi_core_type = _core.name() + ",";
        dmi_core_arg = "*this,";
        ra_arg.clear();
      }

      // If we have the 4-parameter form for memory, then that implies we need
      // the typed parms as well, though we don't have to pass those on.
      gc_string cw_parms, cw_args, cw_log_arg, cw_t_args;
      gc_string logger_read_expr, logger_write_expr;
      if (_core.crit_word_mem()) {
        cw_parms   = ",bool has_cw_addr = false,addr_t cw_addr = 0";
        cw_args    = ",has_cw_addr,cw_addr";        
        cw_log_arg = "((has_cw_addr) ? cw_addr : ea)";
        cw_t_args  = ",false,0";
        logger_read_expr = logger_write_expr = "has_cw_addr";
      } else {
        cw_log_arg = "ea";
      }

      bool call_typed_logger = false;
      gc_string read_type_parm, write_type_parm, type_arg, cast_type_arg, commit_write_parm;
      if (_core.typed_mem() || _core.crit_word_mem()) {
        read_type_parm = ",int type = CacheRead";
        write_type_parm = ",int type = CacheWrite";
        commit_write_parm = ",int";
        type_arg = ",type";
        cast_type_arg = ",(CacheAccess)type";
        call_typed_logger = true;

        if (_core.typed_mem() && _core.crit_word_mem()) {
          logger_read_expr += " || ";
          logger_write_expr += " || ";
        }

        if (_core.typed_mem()) {
          logger_read_expr  += "type != CacheRead";
          logger_write_expr += "type != CacheWrite";
        }
      }

      gc_string bswap;
      if (data_little_endian()) {
        bswap = ".swap_bytes()";
      }

      // Do we need delayed-write semantics for this memory?  This is separate
      // from the question of parallel architectures, even though they use the
      // same basic mechanism.
      bool delayed_write = _core.delayed_writes().count(log_name);

      genMemReadFuncs(id,name,f_pfx,f_sfx,log_mem,log_name,dcrname,dcr_init,xrname,xr_init,
                      rlogger,rlname,is_prefetch_buffer,prefetch_size,has_read_hook,

                      read_type_parm,cw_parms,cw_log_arg,call_typed_logger,logger_read_expr,cast_type_arg,dmi_prefix,
                      dmi_core_type,dmi_core_arg,bswap);

      genMemWriteFuncs(id,name,f_pfx,f_sfx,log_mem,log_name,dcwname,dcw_init,xwname,xw_init,
                       wlogger,wlname,is_prefetch_buffer,has_write_hook,use_dmi,

                       write_type_parm,cw_parms,cw_log_arg,cw_t_args,commit_write_parm,type_arg,
                       call_typed_logger,logger_write_expr,cast_type_arg,dmi_prefix,dmi_core_type,
                       dmi_core_arg,ra_arg,bswap,delayed_write);

      _o << "//\n"
         << "// Versions for handling addresses specified as bits objects.\n"
         << "//\n"
         << "template <size_t NB,size_t Nb1,size_t Nb2>\n"
         << "void " << name << "_write" << f_sfx << " (const bits<Nb1> &addr,const bits<Nb2> &value" << write_type_parm << ") {\n"
         << name << "_write" << f_sfx << "<NB>(addr.uint64(),value" << type_arg << cw_t_args << ");\n"
         << "};\n";

      if (delayed_write) {
        _o << "\n"
           << "template <size_t NB,size_t Nb1,size_t Nb2>\n"
           << "void " << name << "_write_delayed" << f_sfx << " (const bits<Nb1> &addr,const bits<Nb2> &value,unsigned delay" << write_type_parm << ") {\n"
           << name << "_write_delayed" << f_sfx << "<NB>(addr.uint64(),value,delay" << type_arg << cw_t_args << ");\n"
           << "};\n";
      }

      _o << "\n" 
         << "template <size_t NB,size_t Nb>\n" 
         << "bits<NB*8> " << name << "_read" << f_sfx << " (bits<Nb> addr" << read_type_parm << ") {\n"
         << "return " << name << "_read" << f_sfx << "<NB>(addr.uint64()" << type_arg << cw_t_args << ");\n"
         << "};\n"
         << "\n";

      if (_core.crit_word_mem()) {
        // Template versions for the 5-parm version.
        _o << "\n"
           << "template <size_t NB,size_t Nb1,size_t Nb2,size_t Nb3>\n"
           << "void " << name << "_write" << f_sfx << " (const bits<Nb1> &addr,const bits<Nb2> &value" << write_type_parm << ",bool has_cw_addr = false,const bits<Nb3> &cw_addr = 0) {\n"
           << name << "_write" << f_sfx << "<NB>(addr.uint64(),value" << type_arg << ",has_cw_addr,cw_addr.uint64());\n"
           << "};\n";

        if (delayed_write) {
          _o << "template <size_t NB,size_t Nb1,size_t Nb2,size_t Nb3>\n"
             << "void " << name << "_write_delayed," << f_sfx << " (const bits<Nb1> &addr,const bits<Nb2> &value,unsigned delay" << write_type_parm << ",bool has_cw_addr = false,const bits<Nb3> &cw_addr = 0) {\n"
             << name << "_write" << f_sfx << "<NB>(addr.uint64(),value,delay" << type_arg << ",has_cw_addr,cw_addr.uint64());\n"
             << "};\n";
        }

        _o << "\n" 
           << "template <size_t NB,size_t Nb,size_t Nb3>\n" 
           << "bits<NB*8> " << name << "_read" << f_sfx << " (bits<Nb> addr" << read_type_parm << ",bool has_cw_addr = false,const bits<Nb3> &cw_addr = 0) {\n"
           << "return " << name << "_read" << f_sfx << "<NB>(addr.uint64()" << type_arg << ",has_cw_addr,cw_addr.uint64());\n"
           << "};\n"
           << "\n";
      }
    }

    void genMemoryFuncs(const Memory &memory)
    {
      unsigned id   = memory.id();
      gc_string name   = getMemoryName(memory);
      gc_string cname  = getCacheType(memory,extern_mem(memory));
      gc_string xname  = getTranslateType(memory);

      gc_string x_init;

      bool is_extern = extern_mem(memory);
      
      gc_string base_name = getMemAccessor(!memory.instr_mem(),is_extern,false,memory.name());

      gc_string c_init = "(" + base_name + ")";
      
      gc_string dr_arg0;
      if (is_extern) {
        dr_arg0 = (memory.instr_mem()) ? "CacheILogRead, " : "CacheLogRead, ";
      }
      
      gc_string src_name   =  "_core." +  base_name ;
      gc_string read_func  =  src_name + ".read32";
      gc_string write_func =  src_name + ".write32";

      genMemoryFuncs(id,name,name,
                     cname,cname,cname,
                     c_init,c_init,c_init,
                     cname,c_init,
                     xname,xname,xname,
                     x_init,x_init,x_init,
                     read_func,dr_arg0,read_func,"",write_func,write_func,
                     memory.instr_mem(),&memory);
    }

    bool isGlobalMem(const gc_string &name) {
      return (name == MemoryName);
    }

    // Creates the type declaration for an external DMI cache, if applicable.
    void genExtDmiType()
    {
      if (extern_dmi()) {
        gc_string dtype = getExtDmiType();

        gc_string db_type;
        if (logging_mem()) {
          db_type = getMemWrapType(_core);
        } else if (extern_mem()) {
          db_type = "DmiExternalMem";
        } else {
          db_type = "DmiInternalMem";
        }

        // The DMI type can be overridden using a preprocessor define, to allow
        // integrators to override the cache size.
        _o << "#ifndef DMI_HANDLER_TYPE\n"
           << "# define DMI_HANDLER_TYPE DmiMemHandler<" << db_type << "," << _config.extern_dmi_entry_size() << "," 
           << _config.extern_dmi_size() << "," << time_tagged() << ">\n"
           << "#endif\n\n"
           << "typedef DMI_HANDLER_TYPE " << dtype << ";\n\n";
      }
    }

    // Creates stuff for the external DMI, if applicable.
    void genExtDmiSupport()
    {
      if (extern_dmi() || (dmi_cache_support() && extern_mem())) {

        gc_string name = GlobalMemoryName;
        gc_string dname = getExtDmiName();
        gc_string dtype = getExtDmiType();

        //        bool has_data_cache = _core.has_caches(DataCache);

        if (extern_dmi()) {
          _cd._member_vars.push_back(dtype + " " + dname);
          _cd._constr_inits.push_back(dname + "(this)");
          _cd._reset_stmts.push_back(dname + ".reset()");

          _o << "unsigned dmi_page_size() const { return " << getExtDmiType() << "::page_size(); }\n\n";
        }

        // True if we have an internal and/or external DMI cache.

        _o << "bool has_dmi_cache() const { return true; };\n"

        // These implement the control API for the external DMI cache.
           << "void update_dmi_cache_entry(CacheAccess ca,addr_t addr,byte_t *mem)\n"
           << "{\n";
        if (extern_dmi()) {
          _o << dname << ".update_cache_entry(ca,addr,mem);\n";
        }
        if (dmi_cache_support()) {
          // We only do an internal dmi update if we don't have a data cache.
          gc_string r_un = getDmiCacheUpdateName(name,"read");
          gc_string w_un = getDmiCacheUpdateName(name,"write");

          // If the supplied address doesn't match the last recorded real address
          // then this access came externally, which means we don't know the ea
          // and thus can't store it in the internal dmi cache, which hashes by
          // ea.
          _o << "addr_t ra = lastRealAddr();\n"
             << "if (ra != addr) return;\n"
             << "switch (ca) {\n"
             << "case CacheRead:\n"
             << "case CacheLogRead:\n"
             << r_un << "(_lastEffectiveAddr,lastRealAddr(),mem - (addr & ~DmiCache::CompMask));\n"
             << "break;\n";

          if (external_dmi_write_update()) {
            _o << "case CacheWrite:\n"
               << "case CacheFlush:\n"
               << "case CacheStore:\n"
               << "case CacheTouch:\n"
               << w_un << "(_lastEffectiveAddr,lastRealAddr(),mem - (addr & ~DmiCache::CompMask));\n"
               << "break;\n";
          }
          _o << "default:\n"
             << ";\n"
             << "}\n";
        }
        _o << "}\n\n"

           << "void invalidate_dmi_cache_entry(CacheAccess ca,addr_t addr)\n"
           << "{\n";
        if (extern_dmi()) {
          _o << dname << ".invalidate_cache_entry(ca,addr);\n";
        }
        if (dmi_cache_support()) {
          // Right now, any invalidates in the external DMI cache result in us
          // reseting the internal cache, because we don't necessarily know what
          // effective address this corresponds to.
          // We only do an internal dmi update if we don't have a data cache.
          _o << getDmiCacheName(name,"read") << ".reset();\n"
             << getDmiCacheName(name,"write") << ".reset();\n";
        }
        _o << "}\n\n"

           << "void invalidate_internal_dmi_cache()\n"
           << "{\n";
        if (dmi_cache_support()) {
          _o << getDmiCacheName(name,"read") << ".reset();\n"
             << getDmiCacheName(name,"write") << ".reset();\n";
        }
        _o << "}\n\n"

           << "void invalidate_dmi_cache()\n"
           << "{\n";
        if (extern_dmi()) {
          _o << dname << ".reset();\n";
        }
        _o << "invalidate_internal_dmi_cache();\n"
           << "}\n\n";
      }
    }

    void genSetLatencies()
    {
      if (time_tagged()) {

        bool has_data_cache = _core.has_caches(DataCache);
        // If we have an internal or external DMI, then we create the function
        // which allows an external user of the core to set read and write
        // latencies on DMI hits.  If we miss in the DMI cache (internal or
        // external) then latency is added by the system logic by calling
        // IssNode::addLatency().
        _o << "void setDmiLatencies(unsigned r,unsigned w)\n"
           << "{\n";
        if (dmi_cache_support() && !has_data_cache) {
          _o << getDmiReadLatency()  << " = r;\n"
             << getDmiWriteLatency() << " = w;\n";
        }
        if (extern_dmi()) {
          _o << getExtDmiName() << ".setDmiLatencies(r,w);\n";
        }
        _o << "}\n\n";
      }
    }

    // Generate the memory objects, if applicable.
    void genMemories()
    {
      // Generate external DMI type, if applicable.
      genExtDmiType();

      // Generate the memory wrapper object.
      genMemWrappers();

      // Generate external DMI support, if applicable.
      genExtDmiSupport();

      // Generate latency setup, if applicable.
      genSetLatencies();

      MemoryList memories;

      MemoryEl memEl;
      // Data structures for the memories.
      ForEach(_core.memories(),i) {
        if(!isGlobalMem(i->first)) {
          memories.push_back(MemoryEl(i->second,_core.is_shared(*(i->second))));
        }
      }
      genMemoryDS(memories,_core.name());

      // Generate external-memory support, if applicable.
      genMemHandler(_core,0,memories);     

      if (_config.debug_mode()) {
        _o << "void check_read_watchpoints(addr_t ea,bool pre,int num_bytes)\n"
           << "{\n"
           << "if (!WatchpointsEnabled) return;\n"
           << getMemPointsIterator(true) << " i = _read_watchpoints.find(ea & ~WordMask);\n"
           << "if (i != _read_watchpoints.end()) {\n"
           << "_logger->log_watchpoint(ea,pre,wpRead,num_bytes,0,i->second._handle,i->second._payload);\n"
           << "}\n"
           << "}\n\n"
           << "void check_write_watchpoints(addr_t ea,bool pre,int num_bytes,uint64_t value)\n"
           << "{\n"
           << "if (!WatchpointsEnabled) return;\n"
           << getMemPointsIterator(true) << " i = _write_watchpoints.find(ea & ~WordMask);\n"
           << "if (i != _write_watchpoints.end()) {\n"
           << "_logger->log_watchpoint(ea,pre,wpWrite,num_bytes,value,i->second._handle,i->second._payload);\n"
           << "}\n"
           << "}\n\n";
      }

      ForEach(memories,i) {
        genMemoryFuncs(*(i->_memory));
      }

      if (dmi_cache_support()) {
        gc_string drn = getDmiCacheName(GlobalMemoryName,"read");
        gc_string dwn = getDmiCacheName(GlobalMemoryName,"write");
        _o << "void " << getDmiCacheEnable() << " () {\n"
           << drn << ".enable();\n"
           << dwn << ".enable();\n"
           << "}\n\n"
           << "void " << getDmiCacheDisable() << " () {\n"
           << drn << ".disable();\n"
           << dwn << ".disable();\n"
           << "}\n\n";
      }

      genMemNameGetters(memories);
    }
  
  };

  void generateMemDS(opfxstream &o,const gc_string &name,const MemoryList &mems,
                     const StrSet &externs,ClassData &cd,const ConfigDB &config,bool is_system)
  {
    GenMemBase gm(o,externs,cd,config,is_system);

    gm.genMemoryDS(mems,name);
  }

  void generateMemHandler(opfxstream &o,const SysItem &item,const System::Items *children,
                          const MemoryList &mems,const StrSet &externs,ClassData &cd,
                          const ConfigDB &config,bool is_system)
  {
    GenMemBase gm(o,externs,cd,config,is_system);

    gm.genMemHandler(item,children,mems);
  }

  // Entry point for generating the memory data structures.
  void generateMemories(opfxstream &o,const Core &core,InstrWalker &walker,
                        const gc_string &filename,const StrSet &externs,
                        StrList &end_code,ClassData &cd,const ConfigDB &config, 
                        const CoreConfig &coreconfig )
  {
    GenMems gm(o,core,walker,filename,externs,end_code,cd,config,coreconfig);

    gm.genMemories();
  }

}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// Note: Much of this code was re-used from the AME (Zen cycle accurate simulator) written by Don Atkins

// The _External_ data structures are populated directly from the byte array read from
// the elf file.   They are then converted to the _Internal_ structures using the c2* functions.
// We currently initialize memory a word at a time which is pretty slow.  Eventually we'd like
// to use mmap to read the file in all at once and have a higher bandwidth way of setting up
// memory.  Much of this code is from the AME simulater elfistreamfmt class.  Although you would
// expect a ppc elf file to be in big endian, compiling to little endian is an option so I've
// left the code that handles endianness in here rather than assuming big.
//
//
// Notes: to compile to ppc elf, use the ppc cross-compiler:  ppc-elf-gcc -c hello.c -o hello.o -msim
//        to read an elf, use the gcc readelf program: readelf hello.o  (the dump-hex option is useful)
//        for an objdump:     ppc-elf-objdump -D hello.o
//
//
// notes on how ame uses the sym & str tables:
// Some objects in the simulator use the symbol table (if available)
// to either find a symbolic address or to display collected statistics
// on function names. For example, the debug object uses the symbol
// table to allow the user to set breakpoints using symbolic names
// (e.g. br main). Also, the 'prof' command displays code profile
// information after simulating showing the number of instructions
// executed within each function -- the symbol table is used to
// spool through the symbol names looking for function names and
// then setting up the statistics collection address ranges for
// defined functions.
//
// The string table holds the ascii characters to all character strings
// used in the ELF file -- symbols, file names, etc. The symbol table
// just provides an index into the string table to find the symbol
// name and the symbol value. 

#include <ctype.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"
#include "helpers/StringUtils.h"

#include "Reader.h"
#include "ModelInterface.h"
#include "ElfBaseReader.h"
#include "ElfReader.h"
#include "NodeHelpers.h"
#include "StringHelpers.h"

using namespace std;

namespace adl {

  // Add command-line options to the option database.  Note the use of logical &&
  // to ensure that we evaluate all of the additions.
  AnyOption &options();

  const bool UseEa = false;    // Default for using effective-address translation.
  const bool UsePa = true;     // Default for using ELF physical address translation.
  const bool UseFd = false;    // Default for using function descriptors.
  const bool LogBss = false;   // Default for logging the BSS sections.
  const bool AlwaysBe = false; // Default to byte swapping for little-endian ELF files.

  static bool dummy ATTRIBUTE_UNUSED = 
    options().setFlag("elf-ea",    "Causes the ELF reader to treat the addresses as effective addresses.  Default = "+btos(UseEa)+".") &&
    options().setFlag("elf-pa",    "Causes the ELF reader to translate logical addresses to physical addresses using\n"
                                   "the logical to physical information stored in the program headers.  This works for\n"
                                   "the GNU toolchain but not for the Green Hills toolchain.  Default = "+btos(UsePa)+".") &&
    options().setFlag("elf-use-fd","Use function descriptors for symbol lookup.  This is usually determined by the architecture\n"
                                   "but this option allows the user to override.  Option may be negated.") && 
    options().setFlag("elf-log-bss","Forces the ELF reader to log the BSS section.  Default = "+btos(LogBss)+".") &&
    options().setFlag("elf-be",     "Force the reader to always read as big-endian (no byte swapping of memory data).  Default = "+btos(AlwaysBe)+".") &&
    options().setOption("elf2node","e2n","Given a filename:node, set the pc of the given node to the entry address of the given elf file,\nand if the elf-ea option is set, then use translations from this node's MMU to map the EAs.");


  // The ADL-specific ELF reader class.
  class ElfReader : public Reader, public ElfReaderIface {
  public:
    ElfReader(const std::string& fn,IssNode *root,bool use_ea,bool use_pa,ElfUseFd use_fd,
              const std::string& elist,bool log_bss,bool always_be) :
      Reader(fn,root),
      _use_ea(use_ea),
      _elf2node_list(elist),
      _use_entry_point(true),
      _elfreader(createElfReader(fn,*this,log_bss,use_fd,use_pa,always_be))
    {
      if (root) {
        setMemoryNames();
      }
    }

    virtual void setRoot(IssNode *r)
    {
      Reader::setRoot(r);
      setMemoryNames();
    }

    void setMemoryNames()
    {
      _instr_mem_name = _root->getInstrMemName();
      _data_mem_name  = _root->getDataMemName();
    }

    virtual const char *format_str() const { return (_elfreader->type() == ELF32) ? "elf32" : "elf64"; };

    // We do implement this feature.  
    virtual void useFileEntryPoint(bool sep) 
    {
      _use_entry_point = sep;
    }
    
    // We also implement this feature.  
    virtual void setEntryPoint(const string &str) 
    {
      _entry_point = str;
    }

    // We check to see if the user has specified a particular core for this elf input file to be assigned to.
    // If so, then if the user has specified to use EA's, then we use that core's mmu for translation ea->ra 
    // for all addresses initialized in the elf file.  And we set the specified core's PC to the entry addr
    // in this elf input file.
    // If no particular core was specified, then we use any mmu for the ea->ra translations and we set all
    // cores PCs to the entry addr in this elf file.
    // The elf-context syntax is file:coreName,file:coreName:file:coreName...
    Target processPath() 
    {
      // Look for this filename in the list.
      size_t p = _elf2node_list.find(_filename);

      if (p != string::npos) {
        // Look for the ending tag.
        size_t b = _elf2node_list.find(':',p);
        if (b == string::npos) {
          RError("Malformed ELF node identifier for filename '" << _filename << "':  " << _elf2node_list);
        }
        size_t e = _elf2node_list.find(',',b);
        string name = _elf2node_list.substr(b+1,e-b-1);

        return parsePath(_root,name).first;
      }
      // The item must be a core (or context of a core) to do any translation.  If not, then translation
      // won't be performed.
      if (!_root->is_core() && _use_ea) {
        RError("Node " << getPathName(_root,":") << " for file  " << _filename << " must be a core in order to use effective-address mapping.");
      }
      return Target(_root);
    }

    //
    // symbol -- return value of symbol from elf symbol table
    //
    uint64_t symbol(const char* sname) const
    {
      return _elfreader->symbol(sname);
    };

    //
    // readFile
    // This is the main entry point to the ElfReader
    //
    virtual bool readFile()
    {
      //we don't want to log any tranlations used if we are treating addresses as EAs.
      unsigned old_trace_flags = _root->clear_tracing_r();

      // Determine the target for the read operation.
      _trg = processPath();
      CtxState state(_trg,true);

      // Do the actual file reading, if we're supposed to initialize memory.
      // Otherwise, there's really no point.
      if (_init_memory) {
        _elfreader->read();
      }

      addr_t entry_point = 0;
      if (_entry_point.empty()) {
        entry_point = _elfreader->entryAddr();
      } else {
        // If it's an address, then use directly, else consider it a symbol.
        if (!getAddr(entry_point,_entry_point)) {
          entry_point = _elfreader->symbol(_entry_point.c_str());
        }
      }

      entry_point = translateAddr(entry_point);

      // Set program entry address.
      DPrint("entry point: " << hex << entry_point << '\n');      
      setPC(entry_point);

      _root->set_tracing_flags_r(old_trace_flags);
      return true;
    }

    // Translate an address.  We have three possiblee techniques to use:
    // _use_ea:  Use the target node's MMU to translate the address.
    // _use_pa:  Use the program headers in the ELF file to do the translation.
    //    none:  Just treat logical addresses as physical.
    virtual addr_t translateAddr(addr_t addr)
    {
      addr_t ra;
      if (_use_ea) {
        // Translating addresses using the target core's MMU.
        if (!dynamic_cast<const IssCore*>(_trg.node())->extDataWriteTranslate(ra,addr)) {
          ra = addr;
        }
      } else {
        // Translating addresses using the ELF file's program headers.
        ra = _elfreader->getPhysAddr(addr);
      }
      return ra;
    }

    // Called when data memory is to be initialized.
    virtual void initData(addr_t addr,uint32_t data,bool log_only)
    {
      init_mem_internal(*_root,_data_mem_name,addr,data,32,log_only);
    }    

    // Called when code memory is to be initialized.
    virtual void initCode(addr_t addr,uint32_t data,bool log_only)
    {
      init_mem_internal(*_root,_instr_mem_name,addr,data,32,log_only);
    }

    //
    // setPC
    //
    bool setPC(addr_t a)
    {
      if (_use_entry_point || !_entry_point.empty()) {
        QPrint("ElfReader: setting program counter for " << _trg.node()->name() << " to the entry addr: 0x" << hex << a << dec);
        _trg.node()->setProgramCounter(a);
        return true;
      } else {
        return false;
      }
    }


    std::string               _instr_mem_name;
    std::string               _data_mem_name;

    bool                      _use_ea;               // whether to treat addresses as EAs or not
    std::string               _elf2node_list;        // optional colon delimited list of file:corename assignments
    bool                      _use_entry_point;      // whether to use the file's entrypoint or not.
    std::string               _entry_point;          // whether to use the file's entrypoint or not.

    Target                    _trg;                  // Current core target.

    unique_ptr<ElfBaseReader> _elfreader;
  };

  bool ElfReaderFactory::isElfFile(const string &fn)
  {
    return is_elf_file(fn);
  }

  // This is the factory function which instantiates the appropriate ElfReader based upon
  // the type of file.
  Reader* ElfReaderFactory::create(const std::string& fn,IssNode *root,const Args *args)
  {
    bool use_ea    = options().getFlag("elf-ea",UseEa);
    bool use_pa    = options().getFlag("elf-pa",UsePa);
    bool log_bss   = options().getFlag("elf-log-bss",LogBss);
    bool always_be = options().getFlag("elf-be",AlwaysBe);
    std::string elf2node_list = options().getOption("elf2node","");
    ElfUseFd use_fd = ElfFdArch;
    if (options().haveOption("elf-use-fd")) {
      use_fd = (options().getFlag("elf-use-fd")) ? ElfFdTrue : ElfFdFalse;
    }

    // Additionally, the extra flags may be specified in the args array.
    if (args) {
	  for (Args::const_iterator arg = args->begin(); arg != args->end(); ++arg) {
        if (*arg == "elf-ea") {
          use_ea = true;
        }
        else if (*arg == "no-elf-ea") {
          use_ea = false;
        }
        else if (*arg == "elf-pa") {
          use_pa = true;
        }
        else if (*arg == "no-elf-pa") {
          use_pa = false;
        }
        else if (*arg == "elf-log-bss") {
          log_bss = true;
        }
        else if (*arg == "no-elf-log-bss") {
          log_bss = false;
        }
        else if (*arg == "elf-use-fd") {
          use_fd = ElfFdTrue;
        }
        else if (*arg == "no-elf-use-fd") {
          use_fd = ElfFdFalse;
        }
        else if (*arg == "elf-be") {
          always_be = true;
        }
        else if (*arg == "no-elf-be") {
          always_be = false;
        }
      }
    }

    return new ElfReader(fn,root,use_ea,use_pa,use_fd,elf2node_list,log_bss,always_be);
  }

}

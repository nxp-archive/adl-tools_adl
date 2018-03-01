//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The ElfBaseReader implements an ELF reader, suitable for use by any program,
// not just ADL cores.  The actual object is created via a factory function,
// createElfReader() and will depend upon whether a 32-bit or 64-bit reader is
// needed (based upon the file contents).
//
// The reader has a basic interface for retrieving addresses of symbols, getting
// the program entry point, translating addresses, etc.  For setting memory, the
// user passes in a reference to an ElfReaderIface object.  This is an interface
// class which is called by the reader to set memory as it is read from the ELF
// file and to translate addresses, if relevant.
//

#ifndef _ELFBASEREADER_h_
#define _ELFBASEREADER_h_

#include "helpers/BasicTypes.h"

namespace adl {

  //
  // ELF reader interface class.  The actual class will be 32-bit or 64-bit
  // specific; this just provides an interface for querying the file.
  //
  struct ElfBaseReader {
    virtual ~ElfBaseReader() {};

    // Return the filename associated with this object.
    virtual const std::string &filename() const = 0;

    // Type of ELF file being parsed.
    virtual ElfClass type() const = 0;

    // Read the file, calling the interface class as memory is read.  Throws a
    // runtime_exception if errors are encountered.
    virtual void read() = 0;

    // Returns the address of the specified symbol using the ELF symbol table.
    virtual addr_t symbol(const char *name) const = 0;

    // Return the entry-point address from the ELF file.
    virtual addr_t entryAddr() const = 0;

    // Return true if virtual -> physical address mapping will be used (tells
    // the parser to go ahead and set up the mappings from the ELF file.
    virtual bool usePhysAddrs() const = 0;

    // Perform a virtual to physical mapping, returning the physical address.
    // Throws a runtime_exception if there is a problem.
    virtual addr_t getPhysAddr(addr_t vaddr) const = 0;
  };

  struct ElfReaderIface {
    virtual ~ElfReaderIface() {};

    // During processing, this is called to translate the address.  The default
    // just returns the address w/o any alteration, but a user's class can, for
    // example, call getPhysAddr() to translate using the ELF file's virtual ->
    // physical mapping.
    virtual addr_t translateAddr(addr_t addr) { return addr; };

    // As data is read from the ELF file, this function is called with the
    // address and data.  Data is always a 32-bit value.  If log_only is true,
    // then it means that this is just for logging purposes, e.g. the BSS
    // section, which is always 0 and thus doesn't really need to be
    // initialized.
    virtual void initData(addr_t addr,uint32_t data,bool log_only) = 0;
    // Same as above, but called for code sections.
    virtual void initCode(addr_t addr,uint32_t data,bool log_only) = 0;
  };

  // Returns true if the file is an ELF file.
  bool is_elf_file(const std::string &filename);

  // Creates the appropriate reader (32-bit or 64-bit) based upon the contents
  // of file 'fn'.  The iface object will be called as memory is read.
  //
  // fn:        Filename of the file to read.
  // iface:     Interface object used as a callback as memory is read.
  // log_bss:   If true, then the BSS section is read and the interface section is called with log_only=true
  //            for addresses in that section.
  // use_fd:    Use function descriptors for symbol lookup.  This is usually determined by the architecture
  //            but this option allows the user to override.
  // use_pa:    Use the logical -> physical address mapping if it appears to be valid (physical addresses 
  //            not all zeros).
  // always_be: Always treat as big-endian (no byte swapping of data that's loaded).
  ElfBaseReader *createElfReader(const std::string &fn,ElfReaderIface &iface,
                                 bool log_bss = false,ElfUseFd use_fd = ElfFdArch,
                                 bool use_pa = true,bool always_be = false);

}

#endif

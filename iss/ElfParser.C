//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <sstream>

#include "helpers/Macros.h"

#include "ElfParser.h"

#define ASCII_TEST_NUM 1024

using namespace std;

namespace adl {

static unsigned parser_ids = 0;

  ElfParser::ElfParser (const std::string &fn,ElfUseFd use_fd) : 
    _id(parser_ids++),
    _filename(fn),
		_file(fn.c_str(),ios::in|ios::binary),
    _section_map_table(0),
    _codeBuf(0),
    _strtab(0),
    _shstrtab(0),
    _symtab(0),
    _use_fd(use_fd),
    _fdtab(0),
    _fdaddr(0),
    _elfFormatFlag(false),
    _swap_byte_order(false),
    _file_is_big_endian(true),
    _class (ELFNONE)
  {  
    if (!_file) {
      RError ("Error opening input file '" << fn << "'.");
    }
	}

  ElfParser::~ElfParser()
  {
    delete [] _section_map_table;
    delete [] _codeBuf;
    delete [] _symtab;
    delete [] _strtab;
    delete [] _shstrtab;
  }

  void ElfParser::file_rewind() 
  { 
    if( _file.is_open() ) {
      _file.seekg( 0,std::ios::beg ); 
    }
  }

  // Does this elf file use function decsriptors?
  void ElfParser::check_use_fd()
  {
    // If set to the check-architecture value, then check our architecture.
    // Otherwise use value as specified by user.
    if (_use_fd == ElfFdArch) {
      switch (_header.e_machine) {
      case EM_PPC64:
        _use_fd = ElfFdTrue;
        break;
      default:
        _use_fd = ElfFdFalse;
        break;
      }
    }
  }
  
  //
  // readBytes: Read the next chunk of bytes from the current section to a
  // buffer.  The size of the chunk read is specified by gsize().  Reads that
  // amount or the remaining amount, whichever is less, and returns the amount
  // read.
  //
  int ElfParser::readBytes(char* buf) 
  {
    unsigned char* codePtr = &_codeBuf[_codeBufIndex];

    int loadsize = (gsize() >= _remainingSectionSize)?
      _remainingSectionSize:
      gsize() ;
    int sizediff = (gsize() - loadsize);
    
    if( loadsize==0 ) {
      setstate( std::ios::eofbit );
      return 0;
    }
    
    if( buf ) {
      // Get the bytes.
      memcpy(buf,codePtr,loadsize);
      // Clear out anything else that remains.
      if (sizediff) {
        memset(&buf[loadsize],0,sizediff);
      }
    }
    
    _remainingSectionSize -= loadsize;
    _codeBufIndex += loadsize;
    incraddr( +loadsize );
    incrnextaddr( +loadsize );
    return loadsize;
  }

  //
  // This searches for a section which contains the specified address.
  // Returns the index of the section (and sets up the section as the
  // current), or returns -1 if not found.  Note that a search will clobber
  // existing section information.
  //
  int ElfParser::findSection(addr_t addr)
  {
    init();
    for (int i = 0; i != getNumSections(); ++i) {
      getSecHeader(i);
      if ( (addr >= _secHeader.sh_addr) && (addr < (_secHeader.sh_addr + _secHeader.sh_size))
           && (_secHeader.sh_type == SHT_PROGBITS)) {
        return i;
      }
    }
    return -1;
  }

  //
  // This searches for a section by name.  Returns the index of the section or
  // -1 if not found.  Same rules as above apply.
  //
  int ElfParser::findSection(const char *name)
  {
    assert(name);
    if( !_shstrtab ) {
      throw std::runtime_error("No ELF section-header string table information exists.");
    }
      
    init();
    for (int i = 0; i != getNumSections(); ++i) {
      getSecHeader(i);
      const char *shname = shsym_name(_secHeader.sh_name);
      if ( strcmp(name,shname) == 0) {
        return i;
      }
    }
    return -1;
  }    

  // Look up a function descriptor by address.
  ElfFd ElfParser::get_function_descriptor(uint64_t v) const
  {
    ElfFd tmp;
    // The second condition is there as a sanity check to make sure that the
    // function-descriptor data is actually valid, since we currently guess
    // about whether function descriptors should be used.
    if (use_fd() && v > _fdaddr) {
      assert(_fdtab);
      const byte_t *data = &_fdtab[v-_fdaddr];
      tmp._addr = c2ll(data);
    } else {
      tmp._addr = v;
    }
    return tmp;
  }

  bool ElfParser::isFileAscii()
  {
    int   i;
    char  tdata;
    bool  binary=false;
    
    if( !_file.is_open() )  {
      RError ("Error opening input file.");
    }

    for( i=0; i<ASCII_TEST_NUM && !binary; i++ ) {
      if (_file.good()) {
        _file.read( (char*)&tdata, sizeof(tdata) );
      }
      if((tdata & 0x80) || (tdata==0x0)) {
        binary=true;
      }
    }
  
    // _file->close();                                                                                                                                                               
    return !binary;
  };

  // Returns true if this is an ELF file.
  bool is_elf_file(const std::string &filename)
  {
    std::ifstream file;
    file.open(filename.c_str(),ios::in);
    if (!file) {
      RError("Attempt to open ELF file '" << filename << "' failed.");
    }

    // read in the header to determine the elf format type
    char magic[4];
    file.read( (char*)&magic, sizeof(magic) );
    file.close();

    if ( (magic[EI_MAG0] == ELFMAG0) && (magic[EI_MAG1] == ELFMAG1) &&
         (magic[EI_MAG2] == ELFMAG2) && (magic[EI_MAG3] == ELFMAG3)) {
      return true;
    } else {
      return false;
    }
  }

  // This function returns the class-type for an ELF file.
  ElfClass get_elf_class(const std::string &filename)
  {
    std::ifstream file;
    file.open(filename.c_str(),ios::in);
    if (!file) {
      RError("Attempt to open ELF file '" << filename << "' failed.");
    }

    // read in the header to determine the elf format type
    struct Elf_External_Ehdr<1> ext_hdr;
    file.read( (char*)&ext_hdr, sizeof(ext_hdr) );
    file.close();
   
    // instantiate the correct implementation for either ELF32 or ELF64
    if( ext_hdr.e_ident[ EI_CLASS ]==ELFCLASS32) {
      return ELF32;
    } else  if( ext_hdr.e_ident[ EI_CLASS ]==ELFCLASS64) {
      return ELF64;
    } else {
      return ELFNONE;
    }
  }

  // Returns an ElfParser appropriate to the file specified, e.g. a 32-bit ELF
  // file will cause a 32-bit ElfParser to be created.  Throws a runtime_error
  // if the file does not exist or the file is not an ELF file.
  ElfParser *get_elf_parser(const std::string &filename)
  {
    auto ec = get_elf_class(filename);

    switch (ec) {
    case ELF32:
      return new ElfParserImpl<ELF32>(filename);
    case ELF64:
      return new ElfParserImpl<ELF64>(filename);
    default:
      RError("Object format for file '" << filename << "' is not binary ELF32 or binary ELf64 format");
    }
  }

}

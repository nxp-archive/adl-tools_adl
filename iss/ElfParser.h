//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This contains a class for loading ELF files.  It's templates on the file
// type, e.g. 32-bit vs. 64-bit ELF.
//

#ifndef _ELFPARSER_H_
#define _ELFPARSER_H_

#include <fstream>
#include <string.h>
#include <stdio.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <assert.h>
#ifndef _MSC_VER
# include <strings.h>
#endif

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

namespace adl {

  //---------------- Macro Definitions -------------------------------
  // Conversion between unsigned char array and other data types (sbo=swap byte order)
#define	LONG(m)		((((m)[0]&0xff)<<24)|(((m)[1]&0xff)<<16)|(((m)[2]&0xff)<<8)| ((m)[3]&0xff))
#define	SBO_LONG(m)		((((m)[3]&0xff)<<24)|(((m)[2]&0xff)<<16)|(((m)[1]&0xff)<<8)| ((m)[0]&0xff))

#define	LONG_LONG(m)		( (((m)[0]&0xffULL)<<56)|(((m)[1]&0xffULL)<<48)|(((m)[2]&0xffULL)<<40)|(((m)[3]&0xffULL)<<32)|(((m)[4]&0xffULL)<<24)|(((m)[5]&0xffULL)<<16)|(((m)[6]&0xffULL)<<8)| ((m)[7]&0xffULL))
#define	SBO_LONG_LONG(m)	( (((m)[7]&0xffULL)<<56)|(((m)[6]&0xffULL)<<48)|(((m)[5]&0xffULL)<<40)|(((m)[4]&0xffULL)<<32)|(((m)[3]&0xffULL)<<24)|(((m)[2]&0xffULL)<<16)|(((m)[1]&0xffULL)<<8)| ((m)[0]&0xffULL))

#define SHORT(m)        (((m)[0]<<8)|(m)[1])
#define SBO_SHORT(m)    (((m)[1]<<8)|(m)[0])



  // ELF Header Information
#define MAGIC_ELF ((ELFMAG0<<24)|(ELFMAG1<<16)|(ELFMAG2<<8)|(ELFMAG3))
#define SIZE_OF_MAGIC_ELF  16

#define SHT_SYMTAB      2               /* Link editing symbol table */
#define SHT_STRTAB      3               /* A string table */
#define SHT_RELA	    4        		/* Relocation entries with addends */
#define SHT_REL		    9        		/* Relocation entries, no addends */

  struct Elf_Internal_Ehdr {
    unsigned char   e_ident[SIZE_OF_MAGIC_ELF];  // ELF magic number
    uint64_t        e_entry;             // Entry address                 
    uint64_t        e_phoff;             // Program header offset          
    uint64_t        e_shoff;             // Section header offset         
    uint32_t        e_version;           // object version
    uint32_t        e_flags;             // processor flags
    unsigned short  e_type;              // object type
    unsigned short  e_machine;           // architecture type
    unsigned short  e_ehsize;            // ELF header size (bytes)
    unsigned short  e_phentsize;         // Program header size
    unsigned short  e_phnum;             // Program header count
    unsigned short  e_shentsize;         // Section header size
    unsigned short  e_shnum;             // Section header count
    unsigned short  e_shstrndx;          // Section header string index
  };

  // ELF Program Header
  struct Elf_Internal_Phdr {
    uint32_t  p_type;               // program segment type
    uint32_t  p_flags;             // flags
    uint64_t  p_offset;            // offset
    uint64_t  p_vaddr;             // virtual address
    uint64_t  p_paddr;             // physical address
    uint64_t  p_filesz;            // size of segment in file
    uint64_t  p_memsz;             // size of segment in memory
    uint64_t  p_align;             // alignment in file and memory
  };

  // ELF Section header
  struct Elf_Internal_Shdr {
    unsigned int  sh_name;             // index to section name
    unsigned int  sh_type;             // type of section
    uint64_t      sh_flags;            // flags                         
    uint64_t      sh_addr;             // virtual address                 
    uint64_t      sh_size;             // section size in bytes           
    uint64_t      sh_entsize;          // entry size, if section has table 
    uint32_t      sh_link;             // index to next section
    uint32_t      sh_info;             // additional information
    long long     sh_offset;           // file offset                     
    uint64_t      sh_addralign;        // alignment                       
    void *        assoc_section;       // associated section
    void *        contents;            // contents
  };

  // A common structure for describing an ELF symbol.
  struct ElfSym {
    const char *_name;                 // Symbol's name.
    unsigned    _sindex;               // Index of symbol's section.
    uint64_t    _value;                // Symbol's value.
    uint32_t    _size;                 // Symbol's size.
  };

  // ELF Symbol Table
  struct Elf32_Sym {
    uint32_t	    st_name;		/* Symbol name (string tbl index) */
    uint32_t	    st_value;		/* Symbol value */
    uint32_t	    st_size;		/* Symbol size */
    unsigned char	st_info;		/* Symbol type and binding */
    unsigned char	st_other;		/* Symbol visibility */
    uint16_t    	st_shndx;		/* Section index */
  };

  struct Elf64_Sym {
    uint32_t	    st_name;		/* Symbol name (string tbl index) */
    unsigned char	st_info;		/* Symbol type and binding */
    unsigned char   st_other;		/* Symbol visibility */
    uint16_t     	st_shndx;		/* Section index */
    uint64_t   	    st_value;		/* Symbol value */
    uint64_t	    st_size;		/* Symbol size */
  };

  // Base template for external symbol definitions.  We use specializations for
  // this type because the fields aren't exactly the same between the 32-bit and
  // 64-bit versions.
  template <size_t nw>
  struct Elf_External_Sym {};

  // A 32-bit external symbol.
  template<>
  struct Elf_External_Sym<1> {
    unsigned char	st_name[4];
    unsigned char	st_value[4];
    unsigned char	st_size[4];
    unsigned char   st_info;      
    unsigned char   st_other;
    unsigned char 	st_shndx[2]; 
  };

  // A 64-bit external symbol.
  template<>
  struct Elf_External_Sym<2> {
    unsigned char st_name[4];             /* Symbol name, index in string tbl */
    unsigned char st_info;                /* Type and binding attributes */
    unsigned char st_other;               /* No defined meaning, 0 */
    unsigned char st_shndx[2];            /* Associated section index */
    unsigned char st_value[8];            /* Value of the symbol */
    unsigned char st_size[8];             /* Associated symbol size */
  };

  // Relocation stuff.

  // A common structure for describing an ELF relocation.
  struct ElfRel {
    unsigned    _sym;                     // Index of relocation's symbol.
    uint64_t    _offset;
    unsigned    _type;
    int64_t     _addend;
  };

  // The start of a function descriptor.  We don't care about the TOC and environment
  // for our purposes.
  struct ElfFd {
    uint64_t _addr;
  };

  // Relocation table entry without addend (in section of type SHT_REL).
  template <size_t nw>
  struct Elf_Rel
  {
    unsigned char r_offset[4*nw];		/* Address */
    unsigned char r_info[4*nw];			/* Relocation type and symbol index */
  };
  
  // Relocation table entry with addend (in section of type SHT_RELA).
  template <size_t nw>
  struct Elf_Rela
  {
    unsigned char r_offset[4*nw];		/* Address */
    unsigned char r_info[4*nw];		    /* Relocation type and symbol index */
    unsigned char r_addend[4*nw];		/* Addend */
  };

  // Relocation entry r_info extraction helpers.
  template <size_t nw> 
  struct ElfRelInfo {};

  template<>
  struct ElfRelInfo<1> {
    unsigned sym (uint32_t r) { return ((r) >> 8); };
    unsigned type(uint32_t r) { return ((r) & 0xff); };
  };

  template<>
  struct ElfRelInfo<2> {
    unsigned sym (uint64_t r) { return ((r) >> 32); };
    unsigned type(uint64_t r) { return ((r) & 0xffffffff); };
  };
  
  // ELF external headers
  template <size_t nw>
  struct Elf_External_Ehdr {
    unsigned char e_ident[16];    // magic number
    unsigned char e_type[2];      // object file type
    unsigned char e_machine[2];   // machine type
    unsigned char e_version[4];   // version for object file
    unsigned char e_entry[4*nw];     // virtual address for entry
    unsigned char e_phoff[4*nw];     // program header offset
    unsigned char e_shoff[4*nw];     // section header offset
    unsigned char e_flags[4];     // processor-specific flags 
    unsigned char e_ehsize[2];    // size in bytes
    unsigned char e_phentsize[2]; // size of program header entry 
    unsigned char e_phnum[2];     // entry count for program header
    unsigned char e_shentsize[2]; // size of section header entry
    unsigned char e_shnum[2];     // entry count for section header
    unsigned char e_shstrndx[2];  // index to section header string table
  };

  // ELF external program header.  Note the difference in field order between
  // 32-bit and 64-bit!
  template <size_t nw>
  struct Elf_External_Phdr {};
  
  template <>
  struct Elf_External_Phdr<1> {
    unsigned char p_type[4];      // program segment type
    unsigned char p_offset[4];    // file offset
    unsigned char p_vaddr[4];     // virtual address
    unsigned char p_paddr[4];     // physical address
    unsigned char p_filesz[4];    // segment size in file
    unsigned char p_memsz[4];     // segment size in memory
    unsigned char p_flags[4];     // flags
    unsigned char p_align[4];     // alignment for file and memory
  };

  template <>
  struct Elf_External_Phdr<2> {
    unsigned char p_type[4];      // program segment type
    unsigned char p_flags[4];     // flags
    unsigned char p_offset[8];    // file offset
    unsigned char p_vaddr[8];     // virtual address
    unsigned char p_paddr[8];     // physical address
    unsigned char p_filesz[8];    // segment size in file
    unsigned char p_memsz[8];     // segment size in memory
    unsigned char p_align[8];     // alignment for file and memory
  };  

  // ELF external section header
  template <size_t nw>
  struct Elf_External_Shdr {
    unsigned char sh_name[4];     // index to section name
    unsigned char sh_type[4];     // section type
    unsigned char sh_flags[4*nw];    // section attributes
    unsigned char sh_addr[4*nw];     // virtual addr at execution 
    unsigned char sh_offset[4*nw];   // file offset 
    unsigned char sh_size[4*nw];     // section size (in bytes)
    unsigned char sh_link[4];     // link to next section
    unsigned char sh_info[4];     // section information 
    unsigned char sh_addralign[4*nw];// section alignment
    unsigned char sh_entsize[4*nw];  // entry size if section holds table 
  };


  // Fields for e_ident[] 
#define EI_MAG0		0     // ELF ident byte 0
#define ELFMAG0		0x7F  // ELF Magic number byte 0
#define EI_MAG1		1     // ELF ident byte 1
#define ELFMAG1		'E'   // ELF Magic number byte 1
#define EI_MAG2		2     // ELF ident byte 2
#define ELFMAG2		'L'   // ELF Magic number byte 2
#define EI_MAG3		3     // ELF ident byte 3
#define ELFMAG3		'F'   // ELF Magic number byte 3
#define EI_CLASS	4     // file class
#define ELFCLASSNONE	0     //   no assigned class 
#define ELFCLASS32	1     //   32-bit object
#define ELFCLASS64	2     //   64-bit object 

#define EI_DATA		5		// Data encoding 
#define ELFDATANONE	0		// Invalid data encoding 
#define ELFDATA2LSB	1		// 2's complement, little endian 
#define ELFDATA2MSB	2		// 2's complement, big endian 

#define EI_VERSION	6		// File version 

#define EI_PAD		7		// Start of padding bytes 


  // Values for e_type, which identifies the object file type 
#define ET_NONE		0		// No file type 
#define ET_REL		1		// Relocatable file 
#define ET_EXEC		2		// Executable file 
#define ET_DYN		3		// Shared object file 
#define ET_CORE		4		// Core file 
#define ET_LOPROC	0xFF00		// Processor-specific 
#define ET_HIPROC	0xFFFF		// Processor-specific 

  // Values for e_machine, which identifies the architecture 
#define EM_NONE		0	// No machine 
#define EM_M32		1	// AT&T WE 32100 
#define EM_SPARC	2	// SUN SPARC 
#define EM_386		3	// Intel 80386 
#define EM_68K		4	// Motorola m68k family 
#define EM_88K		5	// Motorola m88k family 
#define EM_860		7	// Intel 80860 
#define EM_MIPS		8	// MIPS R3000 (officially, big-endian only) 

#define EM_MIPS_RS4_BE 10	// MIPS R4000 big-endian 

#define EM_SPARC64     11	// SPARC v9 (not official) 64-bit 

#define EM_PARISC      15	// HPPA 
#define EM_PPC	       20	// PowerPC 
#define EM_PPC64       21 // PowerPC 64
#define EM_RCE	       39	// Motorola RCE 

  // Relocation types.

  // AMD x86-64 relocations.
#define R_X86_64_NONE		0	/* No reloc */
#define R_X86_64_64		1	/* Direct 64 bit  */
#define R_X86_64_PC32		2	/* PC relative 32 bit signed */
#define R_X86_64_GOT32		3	/* 32 bit GOT entry */
#define R_X86_64_PLT32		4	/* 32 bit PLT address */
#define R_X86_64_COPY		5	/* Copy symbol at runtime */
#define R_X86_64_GLOB_DAT	6	/* Create GOT entry */
#define R_X86_64_JUMP_SLOT	7	/* Create PLT entry */
#define R_X86_64_RELATIVE	8	/* Adjust by program base */
#define R_X86_64_GOTPCREL	9	/* 32 bit signed PC relative
					   offset to GOT */
#define R_X86_64_32		10	/* Direct 32 bit zero extended */
#define R_X86_64_32S		11	/* Direct 32 bit sign extended */
#define R_X86_64_16		12	/* Direct 16 bit zero extended */
#define R_X86_64_PC16		13	/* 16 bit sign extended pc relative */
#define R_X86_64_8		14	/* Direct 8 bit sign extended  */
#define R_X86_64_PC8		15	/* 8 bit sign extended pc relative */
#define R_X86_64_DTPMOD64	16	/* ID of module containing symbol */
#define R_X86_64_DTPOFF64	17	/* Offset in module's TLS block */
#define R_X86_64_TPOFF64	18	/* Offset in initial TLS block */
#define R_X86_64_TLSGD		19	/* 32 bit signed PC relative offset
					   to two GOT entries for GD symbol */
#define R_X86_64_TLSLD		20	/* 32 bit signed PC relative offset
					   to two GOT entries for LD symbol */
#define R_X86_64_DTPOFF32	21	/* Offset in TLS block */
#define R_X86_64_GOTTPOFF	22	/* 32 bit signed PC relative offset
					   to GOT entry for IE symbol */
#define R_X86_64_TPOFF32	23	/* Offset in initial TLS block */

#define R_X86_64_NUM		24

  // i386 relocs.

#define R_386_NONE	   0		/* No reloc */
#define R_386_32	   1		/* Direct 32 bit  */
#define R_386_PC32	   2		/* PC relative 32 bit */
#define R_386_GOT32	   3		/* 32 bit GOT entry */
#define R_386_PLT32	   4		/* 32 bit PLT address */
#define R_386_COPY	   5		/* Copy symbol at runtime */
#define R_386_GLOB_DAT	   6		/* Create GOT entry */
#define R_386_JMP_SLOT	   7		/* Create PLT entry */
#define R_386_RELATIVE	   8		/* Adjust by program base */
#define R_386_GOTOFF	   9		/* 32 bit offset to GOT */
#define R_386_GOTPC	   10		/* 32 bit PC relative offset to GOT */
#define R_386_32PLT	   11
#define R_386_TLS_TPOFF	   14		/* Offset in static TLS block */
#define R_386_TLS_IE	   15		/* Address of GOT entry for static TLS
					   block offset */
#define R_386_TLS_GOTIE	   16		/* GOT entry for static TLS block
					   offset */
#define R_386_TLS_LE	   17		/* Offset relative to static TLS
					   block */
#define R_386_TLS_GD	   18		/* Direct 32 bit for GNU version of
					   general dynamic thread local data */
#define R_386_TLS_LDM	   19		/* Direct 32 bit for GNU version of
					   local dynamic thread local data
					   in LE code */
#define R_386_16	   20
#define R_386_PC16	   21
#define R_386_8		   22
#define R_386_PC8	   23
#define R_386_TLS_GD_32	   24		/* Direct 32 bit for general dynamic
					   thread local data */
#define R_386_TLS_GD_PUSH  25		/* Tag for pushl in GD TLS code */
#define R_386_TLS_GD_CALL  26		/* Relocation for call to
					   __tls_get_addr() */
#define R_386_TLS_GD_POP   27		/* Tag for popl in GD TLS code */
#define R_386_TLS_LDM_32   28		/* Direct 32 bit for local dynamic
					   thread local data in LE code */
#define R_386_TLS_LDM_PUSH 29		/* Tag for pushl in LDM TLS code */
#define R_386_TLS_LDM_CALL 30		/* Relocation for call to
					   __tls_get_addr() in LDM code */
#define R_386_TLS_LDM_POP  31		/* Tag for popl in LDM TLS code */
#define R_386_TLS_LDO_32   32		/* Offset relative to TLS block */
#define R_386_TLS_IE_32	   33		/* GOT entry for negated static TLS
					   block offset */
#define R_386_TLS_LE_32	   34		/* Negated offset relative to static
					   TLS block */
#define R_386_TLS_DTPMOD32 35		/* ID of module containing symbol */
#define R_386_TLS_DTPOFF32 36		/* Offset in TLS block */
#define R_386_TLS_TPOFF32  37		/* Negated offset in static TLS block */
/* Keep this the last entry.  */
#define R_386_NUM	   38




  // If it is necessary to assign new unofficial EM_* values, please pick large
  //   random numbers (0x8523, 0xa7f2, etc.) to minimize the chances of collision
  //   with official or non-GNU unofficial values.  

  // Cygnus PowerPC ELF backend.  Written in the absence of an ABI.  
#define EM_CYGNUS_POWERPC 0x9025

  // Old version of PowerPC, this should be removed shortly. 
#define EM_PPC_OLD	17


  // Values for e_version 
#define EV_NONE		0		// Invalid ELF version 
#define EV_CURRENT	1		// Current version 

  // Values for program header, p_type field 
#define	PT_NULL		0		// Program header table entry unused 
#define PT_LOAD		1		// Loadable program segment 
#define PT_DYNAMIC	2		// Dynamic linking information 
#define PT_INTERP	3		// Program interpreter 
#define PT_NOTE		4		// Auxiliary information 
#define PT_SHLIB	5		// Reserved, unspecified semantics 
#define PT_PHDR		6		// Entry for header table itself 
#define PT_LOPROC	0x70000000	// Processor-specific 
#define PT_HIPROC	0x7FFFFFFF	// Processor-specific 

  // Program segment permissions, in program header p_flags field 
#define PF_X		(1 << 0)	// Segment is executable 
#define PF_W		(1 << 1)	// Segment is writable 
#define PF_R		(1 << 2)	// Segment is readable 
#define PF_MASKPROC	0xF0000000	// Processor-specific reserved bits 

  // Values for section header, sh_type field 
#define SHT_NULL	0		// Section header table entry unused 
#define SHT_PROGBITS	1		// Program specific (private) data 
#define SHT_SYMTAB	2		// Link editing symbol table 
#define SHT_STRTAB	3		// A string table 
#define SHT_RELA	4		// Relocation entries with addends 
#define SHT_HASH	5		// A symbol hash table 
#define SHT_DYNAMIC	6		// Information for dynamic linking 
#define SHT_NOTE	7		// Information that marks file 
#define SHT_NOBITS	8		// Section occupies no space in file 
#define SHT_REL		9		// Relocation entries, no addends 
#define SHT_SHLIB	10		// Reserved, unspecified semantics 
#define SHT_DYNSYM	11		// Dynamic linking symbol table 
#define SHT_LOPROC	0x70000000	// Processor-specific semantics, lo 
#define SHT_HIPROC	0x7FFFFFFF	// Processor-specific semantics, hi 
#define SHT_LOUSER	0x80000000	// Application-specific semantics 
#define SHT_HIUSER	0x8FFFFFFF	// Application-specific semantics 

  // Values for section header, sh_flags field 
#define SHF_WRITE	(1 << 0)	// Writable data during execution 
#define SHF_ALLOC	(1 << 1)	// Occupies memory during execution 
#define SHF_EXECINSTR	(1 << 2)	// Executable machine instructions 
#define SHF_MERGE	     (1 << 4)	/* Might be merged */
#define SHF_STRINGS	     (1 << 5)	/* Contains nul-terminated strings */
#define SHF_MASKPROC	0xF0000000	// Processor-specific semantics 


  // Values of note segment descriptor types for core files. 
#define NT_PRSTATUS	1		// Contains copy of prstatus struct 
#define NT_FPREGSET	2		// Contains copy of fpregset struct 
#define NT_PRPSINFO	3		// Contains copy of prpsinfo struct 

  // Values of note segment descriptor types for object files.  
  // (Only for hppa right now.  Should this be moved elsewhere?)  

#define NT_VERSION	1		// Contains a version string.  

  // These three macros disassemble and assemble a symbol table st_info field,
  // which contains the symbol binding and symbol type.  The STB_ and STT_
  // defines identify the binding and type.  Note that both 32 and 64-bit files
  // use the same one-byte representation.
#define ELF_ST_BIND(val)                (((unsigned int)(val)) >> 4)
#define ELF_ST_TYPE(val)                ((val) & 0xF)
#define ELF_ST_INFO(bind,type)          (((bind) << 4) + ((type) & 0xF))

#define STN_UNDEF	0		// undefined symbol index 

#define STB_LOCAL	0		// Symbol not visible outside obj 
#define STB_GLOBAL	1		// Symbol visible outside obj 
#define STB_WEAK	2		// Like globals, lower precedence 
#define STB_LOPROC	13		// Application-specific semantics 
#define STB_HIPROC	15		// Application-specific semantics 

#define STT_NOTYPE	0		// Symbol type is unspecified 
#define STT_OBJECT	1		// Symbol is a data object 
#define STT_FUNC	2		// Symbol is a code object 
#define STT_SECTION	3		// Symbol associated with a section 
#define STT_FILE	4		// Symbol gives a file name 
#define STT_LOPROC	13		// Application-specific semantics 
#define STT_HIPROC	15		// Application-specific semantics 

  // Special section indices, which may show up in st_shndx fields, among other places. 
#define SHN_UNDEF	0		    // Undefined section reference 
#define SHN_LORESERVE	0xFF00  // Begin range of reserved indices 
#define SHN_LOPROC	0xFF00		// Begin range of appl-specific 
#define SHN_HIPROC	0xFF1F		// End range of appl-specific 
#define SHN_ABS		0xFFF1		// Associated symbol is absolute 
#define SHN_COMMON	0xFFF2		// Associated symbol is in common 
#define SHN_HIRESERVE	0xFFFF		// End range of reserved indices 

  // relocation info handling macros 
#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((i) & 0xff)
#define ELF32_R_INFO(s,t)	(((s) << 8) + ((t) & 0xff))

#define ELF64_R_SYM(i)		((i) >> 32)
#define ELF64_R_TYPE(i)		((i) & 0xffffffff)
#define ELF64_R_INFO(s,t)	(((bfd_vma) (s) << 32) + (bfd_vma) (t))

  // Dynamic section tags 
#define DT_NULL		0
#define DT_NEEDED	1
#define DT_PLTRELSZ	2
#define DT_PLTGOT	3
#define DT_HASH		4
#define DT_STRTAB	5
#define DT_SYMTAB	6
#define DT_RELA		7
#define DT_RELASZ	8
#define DT_RELAENT	9
#define DT_STRSZ	10
#define DT_SYMENT	11
#define DT_INIT		12
#define DT_FINI		13
#define DT_SONAME	14
#define DT_RPATH	15
#define DT_SYMBOLIC	16
#define DT_REL		17
#define DT_RELSZ	18
#define DT_RELENT	19
#define DT_PLTREL	20
#define DT_DEBUG	21
#define DT_TEXTREL	22
#define DT_JMPREL	23
#define DT_LOPROC	0x70000000
#define DT_HIPROC	0x7fffffff

  // Base class for the parser.  Contains helper functions for manipulating files
  // and interface functions common to 32 and 64-bit file processing.
  class ElfParser {
  public:
    // filename:  Name of file to open.
    ElfParser(const std::string &filename,ElfUseFd use_fd = ElfFdArch);
    virtual ~ElfParser();

    const std::string &filename() const { return _filename; };

    unsigned id() const { return _id; };

    bool use_fd() const { return _use_fd == ElfFdTrue; };

    // Does this elf file use function decsriptors?  Set up the use-fd flag
    // appropriately.
    void check_use_fd();

    // Look up a symbol and return information about it.
    virtual ElfSym get_symbol(const char* sname) const = 0;

    // Look up a symbol by index and return info about it.
    virtual ElfSym get_symbol(unsigned index) const = 0;

    // Look up a function descriptor by address.
    ElfFd get_function_descriptor(uint64_t v) const;

    // Returns a vector of all symbols.  A symbol's index in the vector corresponds
    // to its index in the symbol table.
    typedef std::vector<ElfSym> ElfSyms;
    virtual ElfSyms get_symbols() const = 0;

    // Return a list of all relocations for a given section.  Note:  The section name
    // is the name of the section that the relocations apply to, rather than the name of
    // the relocation section itself.
    typedef std::vector<ElfRel> ElfRels;
    virtual ElfRels get_relocs(const char *sname,bool &is_rela) = 0;

    virtual bool eof() { return _file.eof(); }

    //
    // The symbol Table interface...
    //
  
    //
    // symbolTable
    //
    bool           symbolTable() { return _symtab?true:false; };
  
    //
    // symbolName
    //
    virtual const char* symbolName(int index) = 0;
  
    //
    // symbolValue() - given index, return symbol value
    //
    virtual uint64_t symbolValue(int index) = 0;

    //
    // symbolSize() - given index, return symbol size
    //
    virtual uint32_t symbolSize(int index) = 0;
  
    //
    // symbolType() - given index, return symbol type
    //
    virtual int symbolType(int index) = 0;


    // Returns true if the specified symbol's name matches the supplied
    // character string.
    bool symbolMatches(unsigned index,const char *sname) const
    {
      if( index
          && index<_strtab_size
          && _strtab[ index ]
          && strcmp( sname,(const char*)(_strtab+index))==0 ) {
        return true;
      } else {
        return false;
      }
    }

    //
    // readBytes: Read the next chunk of bytes from the current section to a
    // buffer.  The size of the chunk read is specified by gsize().  Reads that
    // amount or the remaining amount, whichever is less, and returns the amount
    // read.
    //
    int readBytes(char* buf);

    //
    // Returns a pointer to a section's data.  Returns 0 if addr -
    // addr+size-1 do not fall within the section's bounds.  If size is 0,
    // then the number if effectively ignored.  Note that addr is the absolute
    // address and not the offset within the section.
    //
    unsigned char *sectionData(addr_t addr,unsigned size = 0)
    {
      addr_t offset = addr - _secHeader.sh_addr;
      if ( offset >= _secHeader.sh_size || ( (offset+size) > _secHeader.sh_size) ) {
        return 0;
      }

      unsigned char* codePtr = &_codeBuf[_codeBufIndex];

      return &codePtr[offset];
    }
  
    //
    // bytesToUint32
    //
    // This is used for converting our char buffer to an integer that we can
    // initialize memory with.  We do pay attention to the little-endian value
    // here, because when we initialize the core's memory, swapping occurs for
    // little-endian designs.  So, we have to correct for that here by swapping
    // again.  In other words, we convert to a proper int, then let the model
    // set it to the appropriate value when we initialize its memory.
    uint32_t bytesToUint32(char* buf,bool le) 
    {
      return ( (le) ? SBO_LONG( buf ) : LONG( buf ) ); 
    }
    
    //
    // entryAddr:  Returns the entry address of the ELF file.
    //
    uint64_t entryAddr() const { 
      uint64_t v = _header.e_entry; 
      if (use_fd()) {
        return get_function_descriptor(v)._addr;        
      } else {
        return v;
      }
    }
  
    //
    // sectionInfo
    //
    bool  sectionInfo() 
    { 
      if ( getSecHeader() ) {
        return true;
      } else { 
        init(); 
        setstate( std::ios::eofbit ); 
        return false; 
      }
    };

    uint64_t sectionFlags() 
    { 
      return _secHeader.sh_flags; 
    }

    uint64_t sectionAddr() 
    { 
      return(_secHeader.sh_addr); 
    }

    uint64_t sectionSize() 
    { 
      return(_secHeader.sh_size); 
    }
  
    //
    // allocate:  Returns true if the section is an allocate-type section.
    //
    bool  allocate()  
    { 
      if ( (_secHeader.sh_flags&SHF_ALLOC) && (_secHeader.sh_size>0) ) {
        return true; 
      } else {
        return false; 
      }
    };
  
    //
    // code:  Returns true if the section is a code-type section.
    //
    bool code() 
    { 
      if ( (_secHeader.sh_type == SHT_PROGBITS) && (_secHeader.sh_flags&SHF_EXECINSTR) && (_secHeader.sh_size>0) ) {
        return allocate();
      } else {
        return false; 
      }
    };
    
    //
    //  data: Returns true if the section is data-type section
    //
    bool data() 
    {
      if ( (_secHeader.sh_type == SHT_PROGBITS) && !(_secHeader.sh_flags&SHF_EXECINSTR) && (_secHeader.sh_size>0) ) {
        return allocate();
      } else {
        return false; 
      }
    };
       
    //
    // bss: Returns true if the secion is bss-type section
    bool bss() {
      return (_secHeader.sh_type == SHT_NOBITS); 
    }

    //
    // ro_section:  Section is read-only.
    bool ro_section()
    {
      return (_secHeader.sh_flags&SHF_WRITE) == 0;
    }

    //
    // strings:  Returns true if the section is a read-only strings section.
    //
    bool string_section()
    {
      if( (_secHeader.sh_flags&SHF_WRITE) == 0 && (_secHeader.sh_flags&SHF_STRINGS) ) {
        return true; 
      } else {
        return false;
      }
    }

    //
    // This initiates an iteration through the sections in the file.  Call this
    // first, then call getSecHeader() to sequentially access sections.
    //
    void init()  
    { 
      _newSectionFlag=false;
      _numberOfSectionsRead=0;
      _remainingSectionSize=0;
      _addr = 0;
      _nextaddr = 0;
      clear();
    };
  
    // Returns the number of sections in the file.
    int getNumSections() const { return _header.e_shnum; };

    //
    // getSecHeader() - index into _extSectHdrTable (previously read-in)
    //                   and convert to 'secHeader' struct. 
    //                   Return 1 if OK, 0 otherwise.
    //
    int getSecHeader() {
      int index = _numberOfSectionsRead;
    
      if( index>=_header.e_shnum ) { 
        setstate(std::ios::eofbit);
        return false;
      }

      getSecHeader(index);

      _numberOfSectionsRead++;
      nextaddr( _secHeader.sh_addr );
    
      return(true);
    };

    // Get the section specified by index.  This updates the _secHeader
    // variable.
    virtual void getSecHeader(int index) = 0;

    //
    // This searches for a section which contains the specified address.
    // Returns the index of the section (and sets up the section as the
    // current), or returns -1 if not found.  Note that a search will clobber
    // existing section information.
    //
    int findSection(addr_t addr);

    //
    // This searches for a section by name.  Returns the index of the section or
    // -1 if not found.  Same rules as above apply.
    //
    int findSection(const char *name);

    // Get the program header specified by index.  This updates the _progHeader
    // variable.
    virtual void getProgHeader(int index) = 0;

    unsigned short c2us( const unsigned char *cptr ) const
    { return( (_swap_byte_order) ? SBO_SHORT( cptr ) : SHORT( cptr ) ); };
    uint32_t  c2ul( const unsigned char *cptr ) const
    { return( (_swap_byte_order) ? SBO_LONG( cptr ) : LONG( cptr ) ); };
    uint64_t c2ull( const unsigned char *cptr ) const { 
      return ( (_swap_byte_order) ? SBO_LONG_LONG( cptr ) : LONG_LONG( cptr ) );
    }

    short c2s( const unsigned char *cptr ) const
    { return( (_swap_byte_order) ? SBO_SHORT( cptr ) : SHORT( cptr ) ); };
    int32_t  c2l( const unsigned char *cptr ) const
    { return( (_swap_byte_order) ? SBO_LONG( cptr ) : LONG( cptr ) ); };
    int64_t c2ll( const unsigned char *cptr ) const { 
      return ( (_swap_byte_order) ? SBO_LONG_LONG( cptr ) : LONG_LONG( cptr ) );
    }

    // This converts a character string to an unsigned long.  If the ELF file
    // is 32-bit, then we expect the character string to be 4 bytes.  If a 64-bit
    // file, then the character string should be 8-bytes long and we truncate
    // the value to fit a long.
    uint32_t c2ul_s (const unsigned char *cptr) const
    {
      return (_class == ELF32) ? c2ul(cptr) : c2ull(cptr);
    }

    uint64_t c2ull_s (const unsigned char *cptr) const
    {
      return (_class == ELF32) ? c2ul(cptr) : c2ull(cptr);
    }

  protected:

    bool isFileAscii();
    void file_rewind();
      
    bool      newSection()  { return(_newSectionFlag); };
    int       getNumProgHdrs() const { return _header.e_phnum; };
    void  rewind() { init(); file_rewind(); };  
    bool  isFileBigEndian() {return _file_is_big_endian;};
    virtual void clear(std::ios_base::iostate state=(std::ios_base::iostate)(0)) { if( _file.is_open() ) _file.clear(state); }
    virtual void setstate(std::ios_base::iostate state=(std::ios_base::iostate)(0)) { clear(state);} 
    const char *sym_name(unsigned index) const { assert(_strtab); return (const char*)(_strtab+index); };
    const char *shsym_name(unsigned index) const { assert(_shstrtab); return (const char*)(_shstrtab+index); };

    // Same as above, except we produce a long long, so nothing is lost with
    // a 64-bit file and a 32-bit file just has some extra overhead.

    virtual addr_t get_addr() { return _addr; };
    virtual void   addr(uint64_t addr) { _addr=addr; };
    virtual void   incraddr(int incr) { _addr+=incr; };
    virtual addr_t nextaddr() { return _nextaddr; };
    virtual void   nextaddr(uint64_t addr) { _nextaddr=addr; };
    virtual void   incrnextaddr(int incr) { _nextaddr+=incr; };
    virtual void   gsize(int sz) { _gsize=sz; };
    virtual int    gsize() { return _gsize; };

    unsigned               _id;                   // Unique ID for this parser.
    std::string            _filename;             // Name of file we're parsing.
    std::ifstream          _file;                 // The input stream (file).
    Elf_Internal_Ehdr      _header;
    Elf_Internal_Shdr      _secHeader;
    Elf_Internal_Phdr      _progHeader;
    uint32_t*              _section_map_table;    // sh_offset mapping into codeBuf
    unsigned char*         _codeBuf;              // segment data
    int                    _codeBufIndex;
    unsigned char*         _strtab;               // string table
    unsigned char*         _shstrtab;             // section-header string table
    unsigned char*         _symtab;               // symbol table
    ElfUseFd               _use_fd;               // If we're using function descriptors.  In that case, when we look up
                                                  // symbols, we then look up the descriptor's target value.
    unsigned char*         _fdtab;                // function descriptor table, if used.
    addr_t                 _fdaddr;               // for calculating fd offset, if applicable.

    uint32_t               _codeBuf_size;         // total size of code sections
    uint32_t               _symtab_size;          // size in bytes of external symbol table
    uint32_t               _strtab_size;          // size in bytes of external string table
    uint32_t               _shstrtab_size;        // size in bytes of section-header string table
    int                    _numberOfSectionsRead;
    int                    _remainingSectionSize; // number of bytes remaining
    bool                   _elfFormatFlag;        // file is elf format
    bool                   _newSectionFlag;       // new section just started
    bool                   _swap_byte_order;      // big vs. little endian
    bool                   _file_is_big_endian; 
    addr_t                 _addr;                 // current address of data
    addr_t                 _nextaddr;             // next address 
    int                    _gsize;                // get size (in bytes)
    int                    _class;                // ELF32, ELF64 or NONE
  };

  // This function returns the class-type for an ELF file.  If the file is not
  // an ELF file, it returns ElfNone.  It throws a runtime_error if the file
  // cannnot be opened.
  ElfClass get_elf_class(const std::string &filename);

  // Returns an ElfParser appropriate to the file specified, e.g. a 32-bit ELF
  // file will cause a 32-bit ElfParser to be created.  Throws a
  // runtime_error if the file does not exist or the file is not an ELF file.
  ElfParser *get_elf_parser(const std::string &filename);

  //
  // The ElfParser class ------------------------------------------
  //
  // templated based on the filetype: ELF32 = 1 word, ELF64=2 words
  //
  //---------------------------------------------------------------
  template <size_t nw>
  class ElfParserImpl : public ElfParser {

  public:
    //
    // constructor
    //
    ElfParserImpl(const std::string& fn,ElfUseFd use_fd = ElfFdArch) :  
      ElfParser(fn,use_fd),
      _extProgHdrTable(0), 
      _extSectHdrTable(0)
    {
      switch (nw) {
      case 1:
      case 2:
        break;
      default:
        throw std::runtime_error("invalid format type for elf parser");
      }

      bool is_ascii = isFileAscii();
    
      if( is_ascii || (readHeader()!=MAGIC_ELF) ) {
        _file.close();
        throw std::runtime_error("object format is not binary ELF format");
      }
    
      _elfFormatFlag=true;
    
      if( !readProgHeaders() ) {
        _file.close();
        throw std::runtime_error("format error in elf object - error reading program header");
      }
			
      if( !readSecHeaders() ) {
        _file.close();
        throw std::runtime_error("no section header information - cannot get to code");
      }
    
      if( !readCode() ) {
        _file.close();
        throw std::runtime_error("format error in elf object - error reading code section");
      }
    
      readSymbols();  // OK if no symbols available

      readFuncDescriptors(); // Only if using.
    
      init(); // re-initialize pointers for other operations 
    };
  
    //
    // destructor
    //
    virtual ~ElfParserImpl() 
    { 
      _file.close();
      delete [] _extProgHdrTable;
      delete [] _extSectHdrTable;
    };
  
    //
    // get_symbol -- return value of symbol from elf symbol table
    //
    virtual ElfSym get_symbol(const char* sname) const 
    {
      if( !_symtab || !_strtab || !sname || !*sname ) {
        throw std::runtime_error("No ELF symbol table information exists.");
      }

      int incr = sizeof(Elf_External_Sym<nw>);

      for( unsigned num=0; num<_symtab_size; num+=incr )  {
        ExtSym  *esptr     = (ExtSym*)(_symtab+num);
        uint32_t st_name   = c2ul( esptr->sym.st_name );
        uint32_t st_sindex = c2us(esptr->sym.st_shndx);
        uint64_t st_value  = c2ull_s(esptr->sym.st_value);
        uint32_t st_size   = c2ul_s(esptr->sym.st_size);
        if (symbolMatches(st_name,sname)) {
          ElfSym sym;
          sym._name   = sym_name(st_name);
          sym._sindex = st_sindex;
          sym._value  = (use_fd()) ? get_function_descriptor(st_value)._addr : st_value;
          sym._size   = st_size;
          return sym;
        }
      }

      throw std::runtime_error ("ELF symbol not found:  " + std::string(sname));
    };

    // Look up a symbol by index and return info about it.
    virtual ElfSym get_symbol(unsigned index) const
    {
      if( !_symtab || !_strtab) {
        throw std::runtime_error("No ELF symbol table information exists.");
      }

      int incr = sizeof(Elf_External_Sym<nw>);

      ExtSym *esptr = (ExtSym*)(_symtab+(index*incr));

      ElfSym sym;
      sym._name  = sym_name( c2ul( esptr->sym.st_name ) );
      sym._sindex = c2us(esptr->sym.st_shndx);
      sym._value = c2ull_s(esptr->sym.st_value);
      if (use_fd()) {
        sym._value = get_function_descriptor(sym._value)._addr;
      }
      sym._size  = c2ul_s(esptr->sym.st_size);      
      return sym;
    }

    // Return all symbols within a vector.
    virtual ElfSyms get_symbols() const
    {
      if( !_symtab || !_strtab) {
        throw std::runtime_error("No ELF symbol table information exists.");
      }

      int incr = sizeof(Elf_External_Sym<nw>);

      ElfSyms syms;
      for( unsigned num=0; num<_symtab_size; num+=incr )  {
        ExtSym  *esptr   = (ExtSym*)(_symtab+num);
        uint32_t st_name = c2ul( esptr->sym.st_name );
        uint32_t st_shndx = c2us(esptr->sym.st_shndx);        
        uint64_t st_value= c2ull_s(esptr->sym.st_value);
        uint32_t st_size = c2ul_s(esptr->sym.st_size);
        ElfSym sym;
        sym._name   = sym_name(st_name);
        sym._sindex = st_shndx;
        sym._value  = (use_fd()) ? get_function_descriptor(st_value)._addr : st_value;
        sym._size   = st_size;
        syms.push_back(sym);
      }
      return syms;
    }

    // Return a list of all relocations.
    virtual ElfRels get_relocs(const char *sname,bool &is_rela)
    {
      RelTab reltab = readRelocs(sname);

      ElfRelInfo<nw> rinfo;

      int incr = (reltab._is_rela) ? sizeof(Elf_Rela<nw>) : sizeof(Elf_Rel<nw>);

      ElfRels rels;
      for (unsigned num = 0; num < reltab._tab_size; num+=incr ) {
        URel *rptr      = (URel*)(reltab._tab+num);
        uint64_t info   = c2ull_s( rptr->rel.r_info );
        unsigned sindex = rinfo.sym(info);
        unsigned type   = rinfo.type(info);

        ElfRel rel;
        rel._sym    = sindex;
        rel._offset = c2ull_s( rptr->rel.r_offset );
        rel._type   = type;
        if (reltab._is_rela) {
          rel._addend = c2ull_s( rptr->rela.r_addend );
        } else {
          rel._addend = 0;
        }
        rels.push_back(rel);
      }
      is_rela = reltab._is_rela;
      return rels;
    }

  
  protected:
    
    using ElfParser::getSecHeader;
    using ElfParser::getProgHeader;

    union URel {
      Elf_Rel<nw> rel;
      Elf_Rela<nw> rela;
    };

    union ExtSym {
      struct Elf_External_Sym<nw> sym;
      unsigned char chr[ sizeof(Elf_External_Sym<nw>) ];
    };
  
    const char* symbolName(int index) 
    {
      ExtSym* esptr;
      if( !(esptr = getExtSym(index) ) )
        return 0;
    
      return (const char*)_strtab + c2ul( esptr->sym.st_name );
    };
  
    uint64_t symbolValue(int index) 
    {
      ExtSym* esptr;
      if( !(esptr = getExtSym(index) ) )
        return 0;
    
      return c2ull_s( esptr->sym.st_value);
    };

    uint32_t symbolSize(int index) 
    {
      ExtSym* esptr;
      if( !(esptr = getExtSym(index) ) )
        return 0;
    
      return c2ul_s( esptr->sym.st_size);
    };
  
    int symbolType(int index) 
    {
      ExtSym* esptr;
      if( !(esptr = getExtSym(index) ) )
        return 0;
    
      return( (int)( ELF_ST_TYPE( (esptr->sym.st_info) )) );
    };

    //
    // getExtSym() - return pointer to symbol definition, 0 if not found
    //
    ExtSym* getExtSym(int index) 
    {
      if( !_symtab || !_strtab ) {
        std::cerr << "Warning:  No symbol table information available.\n";   
        return 0;
      }
    
      unsigned int offset = index * sizeof(Elf_External_Sym<nw>);
    
      if( offset < _symtab_size ) 
        return( (ExtSym*)(_symtab+offset) );
    
      std::cerr << "Warning:  Symbol not found for index: " << index << ".\n";   
      return 0;
    };
  

     //
    // readHeader() - read header as an array of 'unsigned char' then convert
    //			to 'header' struct.  Return first 4 chars of magic
    //			number as uint32_t.
    //
    uint32_t readHeader() 
    {
      struct Elf_External_Ehdr<nw> ext_hdr;
      int i;
    
      for( i=0; i<SIZE_OF_MAGIC_ELF; i++)
        _header.e_ident[i] = '\0';
    
      _file.seekg(0, std::ios::beg );
    
      if( _file.fail() )
        return 0;
    
      _file.read( (char*)&ext_hdr, sizeof(ext_hdr) );
      if( _file.fail() )
        return 0;
    
      if( ext_hdr.e_ident[ EI_DATA ]==ELFDATA2LSB )
        _file_is_big_endian = false;
      else _file_is_big_endian = true; // everything else is big -- for now
    
      if( ext_hdr.e_ident[ EI_CLASS ]==ELFCLASS32)
        _class = ELF32;   
    
      if( !_file_is_big_endian )
        _swap_byte_order=true;
    
      if( _file.good() ) {
        for( i=0; i<SIZE_OF_MAGIC_ELF; i++)
          _header.e_ident[i] = ext_hdr.e_ident[i];
        _header.e_machine  = c2us(ext_hdr.e_machine);
        _header.e_version  = c2ul(ext_hdr.e_version);
        _header.e_entry    = c2ull_s(ext_hdr.e_entry);
        _header.e_phoff    = c2ull_s(ext_hdr.e_phoff);
        _header.e_shoff    = c2ull_s(ext_hdr.e_shoff);
        _header.e_flags    = c2ul(ext_hdr.e_flags);
        _header.e_ehsize   = c2us(ext_hdr.e_ehsize);
        _header.e_phentsize= c2us(ext_hdr.e_phentsize);
        _header.e_phnum    = c2us(ext_hdr.e_phnum);
        _header.e_shentsize= c2us(ext_hdr.e_shentsize);
        _header.e_shnum    = c2us(ext_hdr.e_shnum);
        _header.e_shstrndx = c2us(ext_hdr.e_shstrndx);
      }
      // return magic number -- pack string data using LONG for both big and little endian
      return( LONG(_header.e_ident ) );
    };
  
  
    //
    // readProgHeaders() - read program headers as an array of 'unsigned char'
    //                    Return 1 if OK, 0 otherwise.
    //
    int readProgHeaders() 
    {
      int phsize = (_header.e_phnum * _header.e_phentsize);
      if( phsize==0 )      // no program header
        return(1);     // that's OK
    
      _file.seekg(_header.e_phoff, std::ios::beg );
      if( _file.fail() )        // read error from file
        return 0;              // is not OK
    
      if( _extProgHdrTable ) delete [] _extProgHdrTable;
    
      unsigned char* uca = new unsigned char[ phsize ];
      _extProgHdrTable = (Elf_External_Phdr<nw>*)uca;
    
      _file.read( (char *)uca, phsize );
      return _file.good();  // return current file status
    };

		
	
    //
    // readSecHeaders() - read section headers as an array of 'unsigned char'.
    //                     Return 1 if OK, 0 otherwise
    //
    int readSecHeaders() 
    {
      int shsize = (_header.e_shnum * _header.e_shentsize);
      if( shsize==0 )
        return 0;      // need section info to get to code, so error
    
      _file.seekg(_header.e_shoff, std::ios::beg );
      if( _file.fail() )        // read error from file
        return 0;              // error
    
      if( _extSectHdrTable ) delete [] _extSectHdrTable;
      if( _section_map_table ) delete [] _section_map_table;
    
      unsigned char* uca = new unsigned char[ shsize ];
      _extSectHdrTable = (Elf_External_Shdr<nw>*)uca;
      _section_map_table = new uint32_t[ _header.e_shnum ];
    
      _file.read( (char *)uca, shsize );
      return _file.good();  // return current file status
    };

    //
    // readCode() -  allocate _codeBuf and read code segments.
    //                   Return 1 if OK, 0 otherwise.
    //
    int readCode() 
    {
      int i;
      uint64_t sh_size;
      uint64_t sh_offset;
    
      if( _codeBuf ) delete [] _codeBuf;
      _codeBuf_size = 0;
    
      // Look at all section headers and add up total size.  We actually read
      // all progbits sections, including non-allocate, so that other tools can
      // read special sections, if necessary.
      for( i=0; i<_header.e_shnum; i++ ) { 
        bool progbits =  c2ul( _extSectHdrTable[i].sh_type ) == SHT_PROGBITS;
        uint64_t size = c2ull_s( _extSectHdrTable[i].sh_size );
        if(progbits)
          _codeBuf_size += size; 
      }
    
      // now allocate code buffer
      _codeBuf = new unsigned char[_codeBuf_size];
    
      // read in sections from file
      unsigned char* cbptr = _codeBuf;
      for( i=0; i<_header.e_shnum && _file.good(); i++ ) { 
        _section_map_table[i] = 0;
        bool progbits =  c2ul( _extSectHdrTable[i].sh_type ) == SHT_PROGBITS;
        if( progbits ) {
          sh_size   = c2ull_s( _extSectHdrTable[i].sh_size );
          sh_offset = c2ull_s( _extSectHdrTable[i].sh_offset ) ;
          _file.seekg(sh_offset, std::ios::beg); 
          if( _file.good() ) 
            _file.read( (char *)cbptr, sh_size );
          _section_map_table[i] = (uint32_t)(cbptr-_codeBuf);
          cbptr += sh_size;
        }
      }
    
      return _file.good();
    };
  
    //
    // read symbols from elf file
    //  -- assumes elf header and section headers have previously been read
    //     so zip through sections looking for SHT_SYMTAB and SHT_STRTAB
    //  -- Return True if symbol table found, False otherwise.
    //
    bool readSymbols() 
    {
      int shndx;    // section header index number
      int shlink=0; // link from _symtab to _strtab 
    
      if( !_file.is_open() )
        return false;
    
      std::streampos mymark;
      if( _symtab )   delete [] _symtab;
      if( _strtab )   delete [] _strtab;
      if( _shstrtab ) delete [] _shstrtab;
      _symtab=0;
      _strtab=0;
      _shstrtab=0;
      init();
    
      // get symbol table first
      while( !eof() && !_symtab ) { 
        getSecHeader();
        shndx = _numberOfSectionsRead-1; // one less than number already read
        if( !eof() && _secHeader.sh_type==SHT_SYMTAB && _secHeader.sh_size ) {
          _symtab_size = _secHeader.sh_size;
          _symtab = new unsigned char[ _symtab_size ];
          shlink = _secHeader.sh_link;
          mymark = _file.tellg();
          _file.seekg(_secHeader.sh_offset, std::ios::beg); 
          _file.read( (char *)_symtab, _symtab_size );
          _file.seekg(mymark, std::ios::beg); 
        }
      }
    
      clear(); // clear eof flag
    
      init();
      while( !eof() && !_strtab ) {
        getSecHeader();
        shndx = _numberOfSectionsRead-1; // one less than number already read
        if( !eof() && _secHeader.sh_type==SHT_STRTAB && _secHeader.sh_size) {
          if ( (shndx==shlink)
               && (shndx!=_header.e_shstrndx)
               ) { 
            // We're looking for symbol string table not section name string
            // table
            _strtab_size = _secHeader.sh_size;
            _strtab = new unsigned char[ _strtab_size ];
            mymark = _file.tellg();
            _file.seekg(_secHeader.sh_offset, std::ios::beg); 
            _file.read( (char *)_strtab, _strtab_size );
            _file.seekg(mymark, std::ios::beg); 
          } else if (shndx == _header.e_shstrndx) {
            // Here, we do want the section-header string table.
            _shstrtab_size = _secHeader.sh_size;
            _shstrtab = new unsigned char[ _shstrtab_size ];
            mymark = _file.tellg();
            _file.seekg(_secHeader.sh_offset, std::ios::beg); 
            _file.read( (char *)_shstrtab, _shstrtab_size );
            _file.seekg(mymark, std::ios::beg);             
          }
        }
      }
    
      return( (!_symtab || !_strtab) ? false : true );
    };

    // Read function descriptors if we're using them.
    void readFuncDescriptors()
    {
      // Should we be using function descriptors?  This is determined by machine
      // type.
      check_use_fd();
      if (use_fd()) {
        // We assume that the entry-point address is also a function descriptor
        // if we're using function descriptors.  This way, we don't have to care
        // about what section stores them- we just figure it out by finding a
        // descriptor and then storing the pointer to the section that it's in.
        addr_t ea = _header.e_entry;

        // Find the section that contains this symbol and then get a pointer to
        // the data that this symbol points to.
        int sindex;
        if ((sindex = findSection(ea)) < 0) {
          RError("Couldn't find section storing entry-point address function descriptor.");
        }

        // Store data location and offset address.
        _fdaddr = _secHeader.sh_addr;
        _fdtab = sectionData(_fdaddr,0);
        if (!_fdtab) {
          RError("Bad function descriptor value- doesn't fall within bounds of section.");
        }
      } 
    }

    struct RelTab {
      unsigned char *_tab;
      unsigned       _tab_size;
      bool           _is_rela;

      RelTab() : _tab(0), _tab_size(0), _is_rela(false) {};
    };

    //
    // read relocations from elf file for a given section.  Note that the
    // section header name is the name of the section referred to by the relocations,
    // rather than the name of the relocation section itself.
    //  -- assumes elf header and section headers have previously been read
    //     so zip through sections looking for SHT_SYMTAB and SHT_STRTAB
    //  -- Return True if a relocation table is found, False otherwise.
    //
    RelTab readRelocs(const char *sname) 
    {
      RelTab reltab;

      if (!_file.is_open()) {
        return reltab;
      }

      // First, find the section index that we're looking for.
      int section_index = findSection(sname);
      if (section_index < 0) {
        throw std::runtime_error("unknown section:  "+std::string(sname));
      }

      init();

      std::streampos mymark;

      while (!eof() && !reltab._tab) {
        getSecHeader();
        if ( !eof() && 
             (_secHeader.sh_type==SHT_REL || _secHeader.sh_type==SHT_RELA) &&
             ((int)_secHeader.sh_info == section_index) ) {
          reltab._is_rela = (_secHeader.sh_type == SHT_RELA);
          reltab._tab_size = _secHeader.sh_size;
          reltab._tab = new unsigned char[ reltab._tab_size ];
          mymark = _file.tellg();
          _file.seekg(_secHeader.sh_offset, std::ios::beg); 
          _file.read( (char *)reltab._tab, reltab._tab_size );
          _file.seekg(mymark, std::ios::beg);          
        }
      }

      return reltab;
    }

    virtual void getSecHeader(int index)
    {
      if (index >= _header.e_shnum) {
        RError("Section index (" << index << ") out of bounds (max " << _header.e_shnum << ").");
      }

      _secHeader.sh_name      = c2ul( _extSectHdrTable[index].sh_name );
      _secHeader.sh_type      = c2ul( _extSectHdrTable[index].sh_type );
      _secHeader.sh_flags     = c2ull_s(_extSectHdrTable[index].sh_flags);
      _secHeader.sh_addr      = c2ull_s(_extSectHdrTable[index].sh_addr);
      _secHeader.sh_offset    = c2ull_s(_extSectHdrTable[index].sh_offset);
      _secHeader.sh_size      = c2ull_s(_extSectHdrTable[index].sh_size);
      _secHeader.sh_link      = c2ul( _extSectHdrTable[index].sh_link);
      _secHeader.sh_info      = c2ul( _extSectHdrTable[index].sh_info);
      _secHeader.sh_addralign = c2ull_s( _extSectHdrTable[index].sh_addralign);
      _secHeader.sh_entsize   = c2ull_s( _extSectHdrTable[index].sh_entsize);
    
      _newSectionFlag=true;
      _remainingSectionSize = _secHeader.sh_size;
      _codeBufIndex = _section_map_table[index];
    }

    virtual void getProgHeader(int index)
    {
      if (index >= _header.e_phnum) {
        RError("Program header index (" << index << ") out of bounds (max " << _header.e_phnum << ").");
      }

      _progHeader.p_type   = c2ull_s( _extProgHdrTable[index].p_type );
      _progHeader.p_offset = c2ull_s( _extProgHdrTable[index].p_offset );
      _progHeader.p_vaddr  = c2ull_s( _extProgHdrTable[index].p_vaddr );
      _progHeader.p_paddr  = c2ull_s( _extProgHdrTable[index].p_paddr );
      _progHeader.p_filesz = c2ull_s( _extProgHdrTable[index].p_filesz );
      _progHeader.p_memsz  = c2ull_s( _extProgHdrTable[index].p_memsz );
      _progHeader.p_flags  = c2ull_s( _extProgHdrTable[index].p_flags );
      _progHeader.p_align  = c2ull_s( _extProgHdrTable[index].p_align );
    }
      
    Elf_External_Phdr<nw>* _extProgHdrTable;
    Elf_External_Shdr<nw>* _extSectHdrTable;
  };

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Implementation file for the generic ELF reader object.
//

#include <ctype.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "helpers/Macros.h"

#include "ElfParser.h"
#include "ElfBaseReader.h"

using namespace adl;
using namespace std;

namespace adl {

  // The non-templated ELF readering stuff goes here.
  class ElfBaseReaderImpl : public ElfBaseReader {
  public:
    ElfBaseReaderImpl(const std::string &fn,ElfReaderIface &iface,bool log_bss,bool use_pa,bool always_be) :
      _iface(iface),
      _log_bss(log_bss),
      _use_pa(use_pa),
      _always_be(always_be)
    {
    }

    // Performs a logical -> physical translation on an address, using the data
    // stored in the ELF file's program headers.  This does a binary search to
    // find the mapping.
    virtual addr_t getPhysAddr(addr_t vaddr) const
    {
      if (usePhysAddrs()) {
				pair<Segments::const_iterator,Segments::const_iterator> r = equal_range(_segments.begin(),_segments.end(),Segment(vaddr),SegmentOrder());
        if (r.first == r.second) {
          RError("Could not map ELF virtual address 0x" << hex << vaddr << " to a physical address.");
        } else {
          return vaddr - r.first->_vaddr + r.first->_paddr;
        }
      } else {
        return vaddr;
      }
    }

    // Return true if virtual -> physical address mapping will be used (tells
    // the parser to go ahead and set up the mappings from the ELF file.
    virtual bool usePhysAddrs() const { return _use_pa; };

  protected:

    ElfReaderIface        &_iface;                // Interface object for setting memory.
    bool                   _log_bss;              // Whether to log the BSS section or not.
    bool                   _use_pa;               // if not using EAs, true =>
                                                  // translate section addresses
                                                  // (logical) to physical using
                                                  // the program-header physical
                                                  // address.
    bool                   _always_be;            // Always treat as big-endian,
                                                  // rather than byte-swapping
                                                  // if a little-endian ELF is
                                                  // read.
    struct Segment {
      addr_t _vaddr;
      addr_t _paddr;
      addr_t _size;

      Segment(addr_t v) : _vaddr(v), _paddr(0), _size(1) {};
      Segment(addr_t v,addr_t p,addr_t s) : _vaddr(v), _paddr(p), _size(s) {};
    };
    typedef std::vector<Segment> Segments;

    struct SegmentOrder {
      bool operator()(const Segment &x,const Segment &y) {
        return (x._size) ? ((x._vaddr+x._size-1) < y._vaddr) : (x._vaddr < y._vaddr);
      };
    };

    Segments               _segments;
  };

  //
  // The ElfReader class ------------------------------------------
  //
  // templated based on the filetype: ELF32 = 1 word, ELF64=2 words
  //
  //---------------------------------------------------------------
  template<size_t nw>
  class ElfBaseReaderSized : public ElfBaseReaderImpl, public ElfParserImpl<nw> {

    typedef  ElfParserImpl<nw> Base;
    std::string _instr_mem_name;
    std::string _data_mem_name;
  public:
  
    //
    // constructor
    //
    ElfBaseReaderSized(const std::string& fn,ElfReaderIface &iface,bool log_bss,ElfUseFd use_fd,
                       bool use_pa,bool always_be) :
      ElfBaseReaderImpl(fn,iface,log_bss,use_pa,always_be),
      Base(fn,use_fd)
    {
      setup_segments();
    };
  
    // This function extracts the program headers and sets up an ordered array
    // of logical -> physical mappings.  These can then be quickly looked up
    // using getPhysAddr.  If we discover that all of the physical addresses are
    // 0, then we assume that they're invalid, and we turn off the mapping to be
    // on the safe side (only logical addresses will be used).  Typically, the
    // default is to have use-physical-addresses set to true, then let this code
    // discover if this should be turned off.  The GNU tools, for example, set
    // the physical addresses to the same value as the logical addresses by
    // default, so it'll just work.  Some tools set up an actual mapping, so
    // we'd want to use it, and some tools, e.g. GreenHills, set the physical
    // addresses to 0, so we will detect that and disable this mapping.
    void setup_segments()
    {
      if (!_use_pa) return;

      unsigned num_segments = Base::getNumProgHdrs();

      bool non_zero = false;

      _segments.clear();
      ForRange(num_segments,index) {
        Base::getProgHeader(index);
        _segments.push_back(Segment(Base::_progHeader.p_vaddr,Base::_progHeader.p_paddr,Base::_progHeader.p_memsz));
        non_zero = non_zero || (_segments.back()._paddr != 0);
      }

      if (non_zero) {
        // Found a non-zero physical address, so we assume the mappings are valid.
        sort(_segments.begin(),_segments.end(),SegmentOrder());
      } else {
        // All physical addresses are 0, so we assume that the mappings are not
        // valid, so disable the use of the mapping.
        _segments.clear();
        _use_pa = false;
      }
    }

    virtual const std::string &filename() const
    {
      return Base::filename();
    }

    virtual ElfClass type() const
    {
      return (ElfClass)nw;
    }

    addr_t symbol(const char* sname) const 
    {
      return Base::get_symbol(sname)._value;
    };

    virtual addr_t entryAddr() const
    {
      return Base::entryAddr();
    }

    //
    // readFile
    // This is the main entry point to the ElfReader
    //
    virtual void read()
    {
      Base::init();
      uint64_t addr=0;
      uint64_t section_size=0;
      bool little_endian=false;
    
      if( !Base::isFileBigEndian() )
        little_endian=true;

      addr=0;
    
      // read data and load into memory -- making sure memory has been allocated.
      Base::rewind();  // restart input stream
      Base::clear();

      bool is_code = false;
      char memptr[4];
      while( !Base::eof() ) {     
        // Look for initial address or new section and initialize memory with
        // that data.
        if (!advanceToData()) {
          break;
        }	

        // Section change- get section size.
        if( addr==0 || Base::newSection() ) {
          addr=Base::sectionAddr();
          section_size=Base::sectionSize();
	  
          if (Base::code()) {
            is_code = true;
          } else if (Base::data()) {
            is_code = false;
          } else {
            // This should not happen
            RError("Internal Elf reader error.");
          }
        }

        // Number bytes to get.
        Base::gsize( section_size > 4 ? 4 : section_size );        
        Base::readBytes(memptr);
      
        //unsigned value = *((unsigned *) &memptr[0]);
        uint32_t value = Base::bytesToUint32(memptr,little_endian && !_always_be);
      
        addr_t ra = _iface.translateAddr(addr);
        if (is_code) {
          _iface.initCode(ra,value,false);
        } else {
          _iface.initData(ra,value,false);
        }

        addr = Base::nextaddr();
      }
    }
    
    // Get the next data section.
    bool advanceToData() 
    {
      if( !Base::_file.is_open() ) {
        return false;
      }

      Base::_newSectionFlag=false;
      // look for data -- has to be non-zero size, SHT_PROGBITS or SHT_NOBITS (bss), and WRITE or ALLOCATE
      while( !((Base::_remainingSectionSize > 0) && (Base::code() || Base::data() ))) { 
        Base::getSecHeader();
        if (Base::bss() && _log_bss) {
          if (Base::_remainingSectionSize > 0) {
            // For a .bss section, just log these as used addresses.
            addr_t start = Base::_secHeader.sh_addr;
            start = _iface.translateAddr(start);
            addr_t stop = start + Base::_remainingSectionSize;  
            while (start < stop) {
              _iface.initData(start,0,true);
              start += 4;
            }
          }
        }
        if( Base::eof() )
          return false;
      }
      return true;
    };

  };
  
  // Factory function for creating the correct reader, based upon the file type.
  ElfBaseReader *createElfReader(const std::string &fn,ElfReaderIface &iface,
                                 bool log_bss,ElfUseFd use_fd,bool use_pa,bool always_be)
  {
    ElfClass ec = get_elf_class(fn);

    // Instantiate the correct implementation for either ELF32 or ELF64.
    switch (ec) {
    case ELF32:
      return ( new ElfBaseReaderSized<1>(fn,iface,log_bss,use_fd,use_pa,always_be));
    case ELF64:
      return (new ElfBaseReaderSized<2>(fn,iface,log_bss,use_fd,use_pa,always_be));
    default:
      RError("Object format for file '" << fn << "' is not binary ELF32 or binary ELf64 format");
    }    
  }

}

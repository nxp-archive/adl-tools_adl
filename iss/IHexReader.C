//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <assert.h>

#include <boost/iostreams/device/mapped_file.hpp> // for mmap

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"

#include "IHexReader.h"

using namespace std;

namespace adl {

  namespace {

    static int Hex2IntArray[256];

    bool populateHex2IntArray()
    {
      for (int i = 0; i != 256; ++i) {
        Hex2IntArray[i] = 0;
      }

      int c = 0;
      for (int i = '0'; i <= '9'; ++i) {
        Hex2IntArray[i] = c++;
      }

      c = 10;
      for (int i = 'a'; i <= 'f'; ++i) {
        Hex2IntArray[i] = c++;
      }

      c = 10;
      for (int i = 'A'; i <= 'F'; ++i) {
        Hex2IntArray[i] = c++;
      }

      return true;
    }

    static bool dummy ATTRIBUTE_UNUSED = populateHex2IntArray();

    // Simple lookup-table based way to convert a hex character to its corresponding integer value.
    int hex2int(unsigned c) 
    {
      return Hex2IntArray[c & 0xff];
    }

    // Given a starting and ending location, converts characters to a hex 
    // number.  Converts up to 8 hex digits (a 32bit number).
    uint32_t hexnum(const char *s,const char *e,uint8_t &crc)
    {
      uint32_t r = 0;

      while (s < e) {
        uint8_t b = (hex2int(*s++) << 4);
        b |= hex2int(*s++);

        crc += b;
        r <<= 8;
        r |= b;
      }

      return r;
    }

  }

  IHexReader::IHexReader (const std::string& filename,IssNode *root) : 
    Reader(filename,root),
    _use_entry_point(true)
  {
  }

  // We do implement this feature.  
  void IHexReader::useFileEntryPoint(bool sep)
  {
    _use_entry_point = sep;
  }
  
  bool IHexReader::readFile()
  {
    try {
      parse();
    }
    catch (exception &err) {
      RError(_filename + ":" + err.what());
    }
    return true;
  }

  void IHexReader::parse()
  {
    boost::iostreams::mapped_file mmap(_filename, boost::iostreams::mapped_file::readonly);
    auto f = mmap.const_data();
    auto l = f + mmap.size();

    // We only set the program counter if we get an appropriate record.
    bool got_pc = false;

    uintmax_t linecount = 1;
    uint8_t crc = 0;
    addr_t pc = 0,offset = 0;

    while (f) {

      crc = 0;

      // Search for the start of record character.
      while (f < l) {
        if (*f == '\n') ++linecount;
        if (*f++ == ':') break;
      }

      // Check for minimum record length.
      if (f + 10 > l) {
        RError(linecount << ": No valid record at line ");
      }

      int len = hexnum(f,f+2,crc);
      f += 2;

      addr_t addr = hexnum(f,f+4,crc);
      f += 4;

      int type = hexnum(f,f+2,crc);
      f += 2;

      // Record crc for the rest of the data.
      const char *t = f;
      while (t < l && t < (f + 2*len)) {
        hexnum(t,t+2,crc);
        t += 2;
      }
      int crcbyte = hexnum(t,t+2,crc);
      if (crc != 0) {
        RError(linecount << ": Bad CRC:  Got 0x" << hex << crcbyte << ", expected 0x" << (crcbyte - crc));
      }

      switch (type) {
      case 0:
        // Data record.  Extract up to 32-bits at a time of data and use it to
        // initialize memory.
        f = read_memory(f,l,len,offset+addr);
        break;
      case 1:
        // End-of-file record.
        goto Done;
        break;
      case 2:
        // Extended segment address.
        if (addr || len != 2) {
          RError(linecount << ": Bad extended segment address record (type " << type << ").");
        }
        offset = hexnum(f,f+4,crc) << 4;
        break;
      case 4:
        // Extended linear address.
        if (addr || len != 2) {
          RError(linecount << ": Bad extended linear address record (type " << type << ").");
        }
        offset = hexnum(f,f+4,crc) << 16;
        break;
      case 3:
        // Start segment address.
      case 5:
        // Start linear address.
        if (addr || len != 4) {
          RError(linecount << ": Bad start segment address record (type " << type << ").");
        }
        got_pc = true;
        pc = hexnum(f,f+8,crc);
        break;
      default:
        RError(linecount << ":  Unknown record type encountered:  " << type);
        break;
      }

      // Skip past the crc.
      f += 2;
    }

  Done:

    if (got_pc && _use_entry_point) {
      _root->setProgramCounter(pc);
    }

  }

  // Initialize memory.
  const char *IHexReader::read_memory(const char *f,const char *l,int len,addr_t addr)
  {
    uint8_t dummy;
    // Try to read 32-bit data, then do 8-bit writes for anything remaining.
    while (f < l && len >= 4) {
      uint32_t d = hexnum(f,f+8,dummy);
      f += 8;
      init_mem_internal(*_root,GlobalMemName,addr,d,32);
      addr += 4;
      len -= 4;
    }

    // Any remaining bytes.  If we have trailing nibbles, they're ignored, but
    // that shouldn't happen anyway.
    while (f < l && len) {
      uint32_t d = hexnum(f,f+2,dummy);
      f += 2;
      init_mem_internal(*_root,GlobalMemName,addr,d,8);
      addr += 1;
      --len;
    }

    return f;
  }

}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _ELFREADER_H_
#define _ELFREADER_H_

#include "helpers/stringfwd.h"

namespace adl {

  class IssNode;
  class Reader;
  struct Args;

  // Factory for creating ElfReaders, based upon the type of file being read.
  class ElfReaderFactory {
  public:
    ElfReaderFactory(){}
    virtual ~ElfReaderFactory() {}

    // Returns true if this is an elf file.  File must be readable- this checks
    // the magic number.
    bool isElfFile(const std::string &fn);

    // Returns the appropriate reader object for the specified ELF file.  We
    // assume that the file is actually an ELF file.
    Reader* create(const std::string& fn,IssNode *root,const Args *args = 0);
  };

}

#endif // _ELFREADER_H_

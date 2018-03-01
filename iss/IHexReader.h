//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef IHEXREADER_H
#define IHEXREADER_H 

#include <unordered_map>

#include "Reader.h"

namespace adl {

  // A reader for the Intel Hex format.
  class IHexReader : public Reader {  
  public:
    IHexReader (const std::string& filename,IssNode *root);
    virtual ~IHexReader () {}

    const char *format_str() const { return "hex"; };

    void useFileEntryPoint(bool sep) override;

    bool readFile();

  private:
    void parse();
    const char *read_memory(const char *f,const char *l,int len,addr_t addr);

    bool _use_entry_point;
  };

}

#endif

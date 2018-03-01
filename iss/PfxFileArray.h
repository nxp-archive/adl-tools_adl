//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _PFXFILEARRAY_H_
#define _PFXFILEARRAY_H_

#include <vector>
#include <string>

namespace adl {

  struct opfxstream;
  struct PfxItem;

  // This class stores an array of prefix-string files all pointing to files
  // with a common naming scheme: The input file name's basename is removed, a
  // -n, where n is the index, is added, then the file extension is added.
  struct PfxFileArray {

    PfxFileArray(const std::string &fn,unsigned count,unsigned indent,char start,char end,const std::string &sfx = std::string());
    ~PfxFileArray();

    opfxstream &operator[](unsigned index);
    const opfxstream &operator[](unsigned index) const { return const_cast<PfxFileArray&>(*this)[index]; };

    const std::string &filename(unsigned index);

    unsigned size() const { return _items.size(); };
    
  private:
    typedef std::vector<PfxItem *> PfxItems;

    PfxItems _items;
  };


}

#endif

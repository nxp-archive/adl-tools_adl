//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <assert.h>

#include "helpers/Macros.h"
#include "helpers/pfxstream.h"
#include "helpers/FilenameUtils.h"

#include "PfxFileArray.h"

using namespace std;

namespace adl {

  struct PfxItem {
    
    PfxItem(const char *fn,unsigned indent,char start,char end) :
      _fn(fn),
      _ofile(fn),
      _opfx(_ofile.rdbuf(),indent,start,end)
    {
      if (!_ofile) {
        RError("Unable to open output file '" << fn << "'.");        
      }
    }

    string     _fn;
    ofstream   _ofile;
    opfxstream _opfx;

  };

  pair<string,string> removeExtension(const string &fn)
  {
    size_t n = fn.find('.');
    if (n != string::npos) {
      return make_pair(fn.substr(0,n),fn.substr(n));
    } else {
      return make_pair(fn,string());
    }
  }

  PfxFileArray::PfxFileArray(const string &fn,unsigned count,unsigned indent,char start,char end,const string &sfx)
  {
    // Must have at least 1 items.
    assert(count);

    string dir   = dir_name(fn);
    if (!dir.empty()) {
      dir += '/';
    }

    pair<string,string> p = removeExtension(base_name(fn));

    ForRange(count,i) {
      ostringstream ss;
      ss << dir << p.first << "-" << i << p.second;
      _items.push_back(new PfxItem(ss.str().c_str(),indent,start,end));
    }
    
  }

  PfxFileArray::~PfxFileArray()
  {
    ForEach(_items,i) {
      delete (*i);
    }
  }

const string &PfxFileArray::filename(unsigned index)
{
  return _items.at(index)->_fn;
}

  opfxstream &PfxFileArray::operator[](unsigned index)
  {
    return _items.at(index)->_opfx;
  }

}

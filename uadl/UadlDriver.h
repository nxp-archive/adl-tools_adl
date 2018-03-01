//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _UADL_DRIVER_H
#define _UADL_DRIVER_H

#include <string>
#include <iosfwd>

#include "helpers/gc_string.h"

namespace adl {
  class AnyOption;
  class Data;
  class GenDriverIF;
}

namespace uadl {

  class UadlIr;

  class UadlDriver {
  public:
    UadlDriver();

    // parsing method
    UadlIr *parse(const adl::AnyOption &options,adl::Data &iss_ir,adl::GenDriverIF *iss_driver,
                   const gc_string &uADLFilename,const gc_string &iss_cfilename,const gc_string &uArchHFilename,
                  const gc_string &uArchCFilename,bool check_cr_args,bool full_paths);

    UadlIr *ir() const { return _ir; };

    // Dump the data model(s).  If dump_all is true, then dump the ADL data
    // model as well.  This may only be called after parse(), which is what
    // instantiates the data model.
    void dump(std::ostream &,bool dump_all) const;

  private:
    UadlIr *_ir;
  };

}

#endif // _UADL_DRIVER_H

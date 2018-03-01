//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"
#include "helpers/AnyOption.h"

#include "UadlDriver.h"
#include "UadlParser.h"

using namespace std;
using namespace adl;

namespace uadl {

  UadlDriver::UadlDriver() : _ir(0)
  {}

  UadlIr *UadlDriver::parse(const adl::AnyOption &options,adl::Data &iss_ir,adl::GenDriverIF *iss_driver,
                            const gc_string &uADLFilename,const gc_string &iss_cfilename,
                            const gc_string &uArchHFilename,const gc_string &uArchCFilename,
                            bool check_cr_args,bool full_paths)
  {
    _ir = new UadlIr(iss_ir, iss_driver, iss_cfilename, uArchHFilename, check_cr_args, full_paths);

    if (check_cr_args) {
      auto const &v = options.getValues("I");
      ForEach(v,i) {
        _ir->_incl_paths.push_back(i->c_str());
      }
    }

    UadlParser parser(_ir);

    bool fpe = standardParserSetup(parser,options,uArchCFilename.c_str());

    if (!parser.parse(uADLFilename.c_str())) {
      if (fpe) {
        RError("Could not parse " << uADLFilename);
      } else {
        RError("Could not parse " << base_name(uADLFilename.c_str()));
      }
    }

    return _ir;
  }

  void UadlDriver::dump(std::ostream &o,bool dump_all) const
  {
    assert(_ir);
    if (dump_all) {
      o << "===============\n"
        << "ADL Data Model:\n"
        << "===============\n\n";
      _ir->archIR().dump(o,"  ");
      o << "================\n"
        << "uADL Data Model:\n"
        << "================\n\n";
      _ir->dump(o,"  ");
    } else {
      _ir->dump(o,"");
    }
  }

}

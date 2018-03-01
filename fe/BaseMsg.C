//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A basic error message class.  An option allows for the file name to have
// only its basename printed.  This is useful for regression tests, where you
// want to run diff against a reference copy of the output.
//

#include <iostream>

#include "helpers/FilenameUtils.h"

#include "BaseMsg.h"

namespace adl {

  void BaseMsg::PrintOn(std::ostream& os) const
  {
    if (_location) {
      if (_fullpath) {
        os << _location.GetFilename();
      } else {
        os << base_name(_location.GetFilename());
      }
      os << ":" << _location.GetLineNo() << ": " << _msg;
    } else {
      os << "adl: " << _msg << std::endl;
    }
  }

}

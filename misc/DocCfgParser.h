//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Parser for documentation configuration information.
//

#ifndef _DOC_CFG_PARSER_H_
#define _DOC_CFG_PARSER_H_

#include "fe/BaseParser.h"
#include "fe/Types.h"

namespace adl {

  class DocCfg : public gc {
  public:
    // fn:    Configuration file name.  If empty, then no action taken.
    // Throws a runtime_error if there is a problem parsing the file.
    void parse(const std::string &fn,bool fullpatherrors = true);

    const PPairList &op_transforms() const { return _op_transforms; };
  private:
    friend class TransformHandler;
    
    PPairList _op_transforms;
  };

  // The parser class.
  class DocCfgParser : public BaseParser {
  public:
    DocCfgParser(DocCfg &);

  protected:
    // Creates the initial outer-handler.
    virtual Handler *make_initial_handler(Opencxx::Environment *);

    // This is the data model object.
    DocCfg   &_config;
  };

}

#endif

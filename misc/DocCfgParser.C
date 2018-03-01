//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Parser for documentation configuration information.
//

#include <sstream>
#include <iostream>

#include "fe/Helpers.h"
#include "fe/ParsingHelpers.h"
#include "fe/Handler.h"

#include "helpers/Macros.h"
#include "helpers/fe_exceptions.h"
#include "helpers/gc_string.h"
#include "helpers/AdlTranslatingWalker.h"

#include "DocCfgParser.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  class ConfigDB;
  class CoreConfig;

  void unknown_key(const char *key,Ptree *value)
  {
    PError(value,"Unrecognized key '" << key << "'.");
  }

  // Processes core configuration groups.
  class TransformHandler : public Handler {
  public:
    TransformHandler(Environment *env,Ptree *loc,
                     DocCfg &config) : 
      Handler(env,loc),
      _config(config)
    {};

    virtual void add_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"item")) {
        // we expect a ( src , result ) pair.
        Ptree *s,*r;
        if ( Match(value,"[ ( [ %? , %? ] ) ]",&s,&r)) {
          _config._op_transforms.push_back(make_pair(s,r));
        } else {
          PError(value,"Bad transform pair:  Expected ( src, result ).");
        }
      } else {
        unknown_key(key,value);
      }
    }

  private:

    DocCfg &_config;
  };

  // Outer handler class.
  class DocOuterCfgHandler : public Handler {
  public:
    DocOuterCfgHandler(Environment *env,Ptree *loc,
                    DocCfg &config) : 
      Handler(env,loc), 
      _config(config)
    {};

    virtual Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,
                                const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"op_transforms")) {
        return new TransformHandler(e,p,_config);
      } else {
        PError(p,"A define-block of type '" << type << "' is not allowed within this block.");
      }
    }

  private:
    DocCfg  &_config;
  };


  DocCfgParser::DocCfgParser(DocCfg &config) :
    _config(config)
  {
  }

  Handler *DocCfgParser::make_initial_handler(Environment *env)
  {
    return new DocOuterCfgHandler(env,0,_config);
  }


  // fn:  Configuration file name.
  // Returns false if there's a parsing error.
  void DocCfg::parse(const string &fn,bool fullpatherrors)
  {
    if (!fn.empty()) {
      DocCfgParser parser(*this);
      parser.setFullPathErrors(fullpatherrors);
    
      if (!parser.parse(fn)) {
        RError("Error parsing documentation configuration file.");
      }    
    }
  }


}

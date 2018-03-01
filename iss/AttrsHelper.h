//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Helper class for transfering attributes data from fe's data structure do
// simulator' data structure

#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "helpers/pfxstream.h"
#include "helpers/Macros.h"
#include "helpers/StringUtils.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

namespace adl {

  class AttrsHelper {
  public:
    
    AttrsHelper(const AttrParams* params):_params(params) {
      _var = std::string(AdlGenSym()->ToString());
    };
    
    //generate temp variable initialization
    void gen_var(opfxstream &o) const {
      switch (_params->_type) {
      case AttrParams::Int:
        o << "int " << _var << " = " << itos(_params->_int_list.front()) << ";\n" ;
        break;
      case AttrParams::IntList:	
        o << "std::vector<int> " << _var << ";\n";
        ForEach(_params->_int_list,i) {
          o << _var << ".push_back(" << itos(*i) << ");\n";
        }
        break;
      case AttrParams::Str:
        o << "std::string " << _var << " = \"" << _params->_str_list.front() <<  "\";\n"; 
        break;
      case AttrParams::StrList:	
        o << "std::vector<std::string> " << _var << ";\n";
        ForEach(_params->_str_list,i) {
          o << _var << ".push_back(\"" << *i << "\");\n";
        }
        break;
      default:
        o << ""; 	  
      }  
    };

    std::string gen_constr(bool noValue = false) const {
      if (noValue) { // place holder
        return "Attrs::Params(true)";
      } else if (_params->_type == AttrParams::NoParam) {
        return "Attrs::Params()";
      }
      else {
        return "Attrs::Params(" + _var + ")";
      }  
    };  
  private:
    const AttrParams* _params;
    std::string _var;
  }; 
};

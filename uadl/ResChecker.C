//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This class is used for checking a custom resource class.  It parses the class
// in order to determine the constructor and template arguments.
//

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <errno.h>
#include <iterator>
#include <list>
#include <stdexcept>
#include <unistd.h>

#include "opencxx/parser/CerrErrorLog.h"
#include "opencxx/parser/Lex.h"
#include "opencxx/parser/Parser.h"
#include "opencxx/parser/Program.h"
#include "opencxx/parser/ProgramFile.h"
#include "opencxx/parser/ErrorLog.h"
#include "opencxx/parser/TheErrorLog.h"
#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeDeclarator.h"
#include <opencxx/parser/token-names.h>
#include <opencxx/Environment.h>
#include <opencxx/GenericMsg.h>
#include <opencxx/Walker.h>
#include "opencxx/driver2.h"
#include "opencxx/OpencxxConfiguration.h"
#include "opencxx/UnknownCliOptionException.h"
#include "opencxx/MetacompilerConfiguration.h"

#include "helpers/FilenameUtils.h"

#include "ResChecker.h"
#include "UadlCommon.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace uadl {

  ostream &operator<<(ostream &o,const ArgItem &a) 
  {
    o << a._name;
    if (a._has_default) {
      o << " (has default)";
    }
    if (a._builtin) {
      o << " (built-in)";
    }
    return o;
  }


  ostream &operator<<(ostream &o,const ArgItems &a) 
  {
    ostream_iterator<ArgItem> oi(o,", ");
    copy(a.begin(),a.end(),oi);
    return o;
  }

  ostream &operator<<(ostream &o,const ResInfo &a) 
  {
    o << "template args:  " << a._tmpl_args << ", constr args:  " << a._constr_args;
    return o;
  }

  class MyWalker : public Walker
  {
  public:
    MyWalker(Parser *parser,const gc_string &cname) : 
      Walker(parser),
      _found(false),
      _cname(cname)
    {
    }

    Ptree* TranslateTemplateClass(Ptree* temp_def, Ptree* class_spec)
    {
      if (is_matching_class(class_spec)) {
        _in_class.push_back(true);
        Ptree *tmpl_args = Third(temp_def);
        storeArgs(_args._tmpl_args,tmpl_args);
      } else {
        _in_class.push_back(false);
      }
      Ptree *t2 = Walker::TranslateTemplateClass(temp_def,class_spec);
      _in_class.pop_back();
      return t2;
    }

    Ptree* TranslateClassSpec(Ptree* spec)
    {
      _in_class.push_back(is_matching_class(spec));
      Ptree *spec2 = Walker::TranslateClassSpec(spec);
      _in_class.pop_back();
      return spec2;
    }

    void processDecls(PtreeDeclarator *decl)
    {
      Ptree* args = 0;
      if (in_class()) {
        Ptree *fname = First(decl);
        if (Eq(fname,_cname.c_str())) {
          // Process the constructor.
          if(PtreeUtil::GetArgDeclList(decl, args)){
            Environment tmp_env(env,this);
            storeArgs(_args._constr_args, args);
          }
        } else if (Eq(fname,PreCycleName)) {
          _args._hasPreCycle = true;
        } else if (Eq(fname,PostCycleName)) {
          _args._hasPostCycle = true;
        } else if (Eq(fname,PostSimName)) {
          _args._hasPostSim = true;
        }
        
      }
    }

    virtual Ptree* TranslateDeclarator(bool record,PtreeDeclarator *exp)
    {
      processDecls(exp);
      return Walker::TranslateDeclarator(record,exp);
    }

    bool found() const { return _found; };

    ResInfo _args;

  protected:

    gc_string argName(Ptree *x)
    {
      if (x->IsLeaf()) {
        return x->ToString();
      } if (x->Cdr()) {
        return x->Cdr()->ToString();
      } else {
        return x->Car()->ToString();
      }
    }

    void storeArgs(ArgItems &arglist,Ptree* args)
    {
      Ptree* rest;

      if (args) {
        Ptree *a = args->Car();
        if(args->Cdr() != 0) {
          rest = args->Cdr()->Cdr();	// skip ","
          storeArgs(arglist,rest);
          rest = args->Cdr();
        }

        bool is_ellipsis = a->IsLeaf();		// a may be "..."
        if(is_ellipsis)
          /* do nothing */;
        else if(a->Car()->IsA(ntUserdefKeyword)){
          arglist.push_front(ArgItem(argName(Third(a)),Eq(Nth(a,3),"=")));
        }
        else if(a->Car()->IsA(REGISTER)){
          arglist.push_front(ArgItem(argName(Third(a)),Eq(Nth(a,3),"=")));
        }
        else{
          arglist.push_front(ArgItem(argName(Second(a)),Eq(Third(a),"=")));
        }
      }
    }

    bool in_class() const {
      return !_in_class.empty() && _in_class.back();
    }

    bool is_matching_class(Ptree *class_spec) {
      if (Eq(Second(class_spec),_cname.c_str())) {
        _found = true;
        return true;
      }
      return false;
    }

    bool         _found;
    vector<bool> _in_class;
    gc_string    _cname;
  };

  string filename(string fn,bool full_path_errors)
  {
    if (full_path_errors) {
      return fn;
    } else {
      return adl::base_name(fn);
    }
  }

  void process_file(ResInfo &args,const gc_string &src,const gc_string &cname,
                    const MetacompilerConfiguration& config,bool full_path_errors)
  {
    ifstream src_stream(src.c_str());
    if (!src_stream){
      ostringstream ss;
      ss << "Cannot open `" << src << "' for reading"
         << " (" << strerror(errno) << ")\n";
      throw runtime_error(ss.str());
    }

    // These are allocated on the heap b/c the environments refer to them, so
    // we want them to remain valid after the front-end runs.
    ProgramFile *src_prog = new ProgramFile(src_stream);
    Lex lexer(src_prog, config.WcharSupport(), false);
    Parser parser(&lexer, config.ErrorLog());
    MyWalker w(&parser,cname);
    
    Ptree* def;

    while (parser.rProgram(def)) {
      w.Translate(def);
    }

    if (parser.SyntaxErrorsFound()) {
      throw runtime_error(string("Parse errors found in ") + filename(src.c_str(),full_path_errors));
    }
    if (!w.found()) {
      throw runtime_error(string("Custom resource class '") + cname.c_str() + "' not found in file '" + filename(src.c_str(),full_path_errors) + "'.\n");
    }
    args = w._args;
  }

  bool parse_custom_resource(ResInfo &args,const gc_string &fn,const gc_string &cname,
                             bool full_path_errors)
  {
    CerrErrorLog errorLog(30);
    OpencxxConfiguration config(errorLog);

    // Turn off warnings, since it doesn't like the triple-quoted strings and
    // the import statements.
    config.AddCppOption("-w");
  
    string cppfile, occfile;
    try {
      //    cppfile = RunPreprocessor(fn.c_str(),config);
      process_file(args,fn,cname,config,full_path_errors);
    }
    catch (const TooManyErrorsException &) {
      cerr << "Too many errors." << endl;
      return false;
    }
    catch (const FatalErrorException &) {
      cerr << "Fatal error." << endl;
      return false;
    }
    catch (runtime_error &err) {
      cerr << "Error:  " << err.what();
      return false;
    }
    // Remove the intermediate file even with errors, since the error messages
    // reference the original file.
    unlink(cppfile.c_str());
    // If we had any errors, we exit.
    if (TheErrorLog().errorCount()) {
      return false;
    }

    return true;
  }

}


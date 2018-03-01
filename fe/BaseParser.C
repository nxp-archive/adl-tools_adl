//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main entry point for the front-end parsing library.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cerrno>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "opencxx/parser/CerrErrorLog.h"
#include "opencxx/parser/Lex.h"
#include "opencxx/parser/Parser.h"
#include "opencxx/parser/Program.h"
#include "opencxx/parser/ProgramFile.h"
#include "opencxx/parser/ErrorLog.h"
#include "opencxx/parser/TheErrorLog.h"
#include "opencxx/Class.h"
#include "opencxx/driver2.h"
#include "opencxx/OpencxxConfiguration.h"
#include "opencxx/UnknownCliOptionException.h"

#include "helpers/fe_exceptions.h"
#include "helpers/FilenameUtils.h"
#include "helpers/Macros.h"
#include "helpers/AnyOption.h"
#include "helpers/StringUtils.h"

#include "AdlWalker.h"
#include "BaseParser.h"
#include "Handler.h"
#include "Helpers.h"
#include "MsgLog.h"
#include "Types.h"
#include "BaseMsg.h"
#include "Data.h"

using namespace std;
using namespace Opencxx;

namespace Opencxx {

  // This should override the default version located in Plasma.
  // It allows the user to specify a temporary name to be used, rather than
  // the default based upon the source name.
  struct AdlMakeTempFilename : public gc, public MakeTempFilename {
    AdlMakeTempFilename(bool ut) : _use_temp(ut) 
    {
      if (_use_temp) {      
        _temp_name = "/tmp/occXXXXXXXX";
        int fd = mkstemp(&_temp_name[0]);
        close(fd);
        unlink (_temp_name.c_str());
      }
    };

    char* operator()(const char* src, const char* suffix)
    {
      string fn;
      if (_use_temp) {
        fn = _temp_name + suffix;
      } else {
        fn = adl::gen_default_name(src,suffix);
      }
      return strdup(fn.c_str());
    }

    bool _use_temp;
    string _temp_name;
  };
}

namespace adl {

  BaseParser::BaseParser() :
    _use_temp(true),
    _verbose(false),
    _handler(0),
    _parser(0)
  {
  }

  void BaseParser::addCppOption(const string &c)
  {
    _cpp_options.push_back(c);
  }

  bool BaseParser::parse(const string &fn)
  {
    CerrErrorLog errorLog(30);
    OpencxxConfiguration config(errorLog);

    auto af = new AdlMakeTempFilename(_use_temp);
    Opencxx::SetMakeTempFilename(*af);

    // Turn off warnings, since it doesn't like the triple-quoted strings and
    // the import statements.
    config.AddCppOption("-w");

    // Output dependency information, if so requested.
    if (!_depfile.empty()) {
      if (!_deptarget.empty()) {
        config.AddCppOption("-MT");
        config.AddCppOption(_deptarget);
      }
      config.AddCppOption("-MD");
      config.AddCppOption("-MP");
      config.AddCppOption("-MF");
      config.AddCppOption(_depfile);
    }
  
    // Add any remaining options.
    ForEach(_cpp_options,i) {
      config.AddCppOption(*i);
    }
  
    if (!_cpp.empty()) {
      config.SetCompilerCommand(_cpp);
    }

    if (_verbose) {
      config.SetVerboseMode(true);
    }

    string cppfile, occfile;
    try {
      cppfile = RunPreprocessor(fn.c_str(),config);
      occfile = process_file(cppfile,config);
    }
    catch (const TooManyErrorsException &) {
      cerr << "Too many errors." << endl;
      return false;
    }
    catch (const FatalErrorException &) {
      cerr << "Fatal error." << endl;
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

  void add_to_log(Parser &parser,MsgLog &log,bool fullpath)
  {
    ForEach(log,i) {
      SourceLocation location(parser.GetLocation(i->_loc));
      TheErrorLog().Report(BaseMsg(i->_severity,location,i->_msg,fullpath));
    }
  }

  SourceLocation BaseParser::getLocation(Ptree *x) const
  {
    if (_parser) {
      return _parser->GetLocation(x);
    } else {
      return SourceLocation();
    }
  }

  string BaseParser::process_file(const string &src,const MetacompilerConfiguration& config)
  {
    char* dest = OpenCxxOutputFileName(src.c_str());

    ifstream src_stream(src.c_str());
    if (!src_stream){
      ostringstream buf;
      buf << "cannot open `" << src << "' for reading"
          << " (" << strerror(errno) << ")";
      config.ErrorLog().Report(BaseMsg(Msg::Fatal, SourceLocation(), buf.str(), _fullpath));
    }

    // These are allocated on the heap b/c the environments refer to them, so
    // we want them to remain valid after the front-end runs.
    ProgramFile *src_prog = new ProgramFile(src_stream);
    Lex *lexer = new Lex(src_prog, config.WcharSupport(), config.RecognizeOccExtensions());
    _parser = new Parser(lexer, config.ErrorLog());
    MsgLog msglog;
    
    try {
      // Disable GC during this phase, since it's mainly just a creation phase.
      // We don't want to pay the overhead of the GC right now.
      GC_disable();
      
      // Do the actual parsing.
      parse_file(*_parser, config, msglog);

      // Re-enable the GC.
      GC_enable();

      // Does the second pass, which instantiates the data model.
      // We only do this if we have a pointer to the outer-handler, which might not
      // have been set if we got errors.
      if (_handler) {
        second_pass(msglog);
      }

      // When finished, erase the pointer to the object so that can
      // free all memory.
      _handler = 0;

    }
    catch (parse_error &e) {
      msglog.add(e);
    }
    catch (const TooManyErrorsException& e) {
      config.ErrorLog().Report(BaseMsg(Msg::Fatal, SourceLocation(), "too many errors", _fullpath));
    }

    // If there were any errors, add to the OpenC++ error log.
    if (!msglog.empty()) {
      add_to_log(*_parser,msglog,_fullpath);
    }

    return dest;
  }

  // Add any new syntax extensions here.
  void addSyntaxExtensions()
  {
    Class::RegisterNewClosureStatement("define");  
    Class::RegisterNewClosureStatement("defmod");  
    Class::RegisterNewClosureStatement("func");
    Class::RegisterNewClosureStatement("block");  
  }

  void BaseParser::setup(Environment *env)
  {
  }

  void BaseParser::add_function(Ptree *)
  {
  }

  void BaseParser::add_declaration(Ptree *)
  {
  }

  void BaseParser::second_pass(MsgLog &)
  {
  }

  // Does the parsing and walks the AST.  It is responsible for storing outer-scope
  // helper C++ code.
void BaseParser::parse_file(Parser &parser, const MetacompilerConfiguration& config,MsgLog &ml) 
  {
    AdlWalker *w = new AdlWalker(&parser,*this,ml);
    Ptree* def;

    w->setFullPathErrors(_fullpath);

    setup(w->get_env());

    addSyntaxExtensions();

    while (parser.rProgram(def)) {
      Ptree *exp = w->Translate(def);
      // The walker returns null for define/defmod blocks so that we can distinguish
      // them from C++ code.
      if (exp) {
        Ptree *decls = PtreeUtil::Third(exp);
        if (decls->IsA(ntDeclarator)) {
          add_function(exp);
        } else {
          add_declaration(exp);
        }
      }
      if (config.ShowProgram()) {
        def->Display(cout);
      }
    }

    if (parser.SyntaxErrorsFound()) {
      config.ErrorLog().Report(BaseMsg(Msg::Fatal, SourceLocation(),"parse errors, exiting", _fullpath));
    } else {
      _handler = w->get_outer();
    }
  }

  // This sets up the options used in the standard setup routine below.
  void standardParserOptionSetup(AnyOption &options)
  {
    options.setFlag("strict",              "Enable strict code check mode.");
    options.setFlag("warn-redefine",       "Warn the user if a define block overwrites an existing item.\nDefault is "+btos(Data::strictMode())+".");
    options.setFlag("full-path-errors",    "Display full-paths for filename locations in error messages.\nDefault is "+btos(Data::warnRedefineMode())+".");
    options.setFlag("orig-action",         "Displays action/hook-code without aspect expansion.");
    options.setOption("cpp",               "Specify the C preprocessor to be used.");
    options.setOption("depfile",           "Specify that dependency information should be written to this file.");
    options.setOption("deptarget",         "Specify the target of the dependency information.");
    options.setOption("define", "D",       "Specify a preprocessor define.");
    options.setOption("include", "I",      "Specify a preprocessor include directory.");
  }

  // This sets up the parser in a standard fashion based upon command-line options.
  bool standardParserSetup(BaseParser &parser,const AnyOption &options,const string &default_deptrg)
  {
    parser.setVerbose(options.getFlag("verbose"));
  
    Data::setStrict(options.getFlag("strict",Data::strictMode()));

    Data::setWarnRedefine(options.getFlag("warn-redefine",Data::warnRedefineMode()));

    Data::setOrigAction(options.getFlag("orig-action",Data::origAction()));

    bool fullpatherrors = options.getOption("full-path-errors",true);
    parser.setFullPathErrors(fullpatherrors);

    string cpp = options.getOption("cpp");
    if (!cpp.empty()) {
      parser.setCpp(cpp);
    }

    // Set up dependency tracking generation, if so requested.
    string df = options.getOption("depfile");
    if (!df.empty()) {
      parser.setDepFile(df);
      string dt = options.getOption("deptarget");
      parser.setDepTarget((dt.empty()) ? default_deptrg : dt);
    }

    // Add on any defines or includes.
    if (options.haveOption("D")) {
      ForEach(options.getValues("D"),i) {
        parser.addCppOption("-D"+*i);
      }
    }

    if (options.haveOption("I")) {
      ForEach(options.getValues("I"),i) {
        parser.addCppOption("-I"+*i);
      }
    }

    return fullpatherrors;
  }

}

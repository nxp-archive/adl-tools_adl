//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Base parser- runs OpenC++ and then walks the resulting Ptree,
// calling Handler objects.
//

#ifndef _BASE_PARSER_H_
#define _BASE_PARSER_H_

#include <string>
#include <vector>

namespace Opencxx {
  class MetacompilerConfiguration;
  class Parser;
  class Environment;
  class Ptree;
  class SourceLocation;
};

namespace adl {

  class Handler;
  class OuterHandler;
  class MsgLog;
  class PList;
  class AnyOption;

  // The parser class does all of the parsing, including running the preprocessor.
  class BaseParser {
  public:
    BaseParser();
    virtual ~BaseParser() {};

    // fn:  Filename to parse.
    // Returns false if there's a parsing error, else true.
    bool parse(const std::string &);

    void setUseTemp(bool t) { _use_temp = t; };
    void setCpp(const std::string &c) { _cpp = c; };
    void setDepFile(const std::string &d) { _depfile = d; };
    void setDepTarget(const std::string &d) { _deptarget = d; };
    void setVerbose(bool v) { _verbose = v; };
    void setFullPathErrors(bool fp) { _fullpath = fp; };

    // Add a preprocessor option.
    void addCppOption(const std::string &);

    // This must be specified by a derived class- creates the outer handler
    // object which will first receive parsing data.
    virtual Handler *make_initial_handler(Opencxx::Environment *) = 0;

    // Call this to retrieve source location information, given a piece of the AST.
    Opencxx::SourceLocation getLocation(Opencxx::Ptree *) const;

  protected:

    // This is called before the first pass.  It can be used to setup any initial
    // data model structures.
    virtual void setup(Opencxx::Environment *env);

    // Called each time an outer-most scope function is encountered.
    virtual void add_function(Opencxx::Ptree *);

    // Called at end of first-pass in order to store any declarations encountered.
    virtual void add_declaration(Opencxx::Ptree *);

    // This is called after a successful parse.
    virtual void second_pass(MsgLog &);

    std::string process_file(const std::string &src,const Opencxx::MetacompilerConfiguration& config);
    void parse_file(Opencxx::Parser &parser, const Opencxx::MetacompilerConfiguration& config,MsgLog &ml);

    // If true, use a temporary file name for intermediate files.
    bool          _use_temp;

    // Preprocessor to use.
    std::string   _cpp;

    // Extra preprocessor options.
    std::vector<std::string> _cpp_options;

    // Preprocessor dependency output file.
    std::string   _depfile;
  
    // If not empty, then this is used as the dependency target, if we
    // are going to output a dependency file.
    std::string   _deptarget;

    // Verbose mode switch.
    bool          _verbose;

   
    // Display fullpath information for error messages.
    bool          _fullpath;

    // Outer handler object storing the result of the first-pass.
    Handler      *_handler;

    // We store the parser so that we can retrieve source location information
    // later.
    Opencxx::Parser *_parser;
  };

  // This adds the options to the option-processing object which are then
  // handled by standardParserSetup().
  void standardParserOptionSetup(AnyOption &options);

  // This sets up the parser in a standard fashion based upon command-line options.
  // default_deptrg:  Default target to use when generating dependency information.
  // Returns true if full-path-errors is true, else false.
  bool standardParserSetup(BaseParser &parser,const AnyOption &options,const std::string &default_deptrg);

}

#endif


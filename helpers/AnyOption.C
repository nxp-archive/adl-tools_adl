//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

/*
 * AnyOption 1.1  
 *
 * kishan at hackorama dot com  www.hackorama.com JULY 2001 
 *
 * + Acts as a common facade class for reading 
 *   commandline options as well as options from
 *   an optionfile with delimited type value pairs 
 *
 * + Handles the POSIX style single character options ( -w )
 *   as well as the newer GNU long options ( --width )
 * 
 * + The option file assumes the traditional format of
 *   first character based comment lines and type value
 *   pairs with a delimiter , and flags which are not pairs
 * 
 *  	# this is a coment
 *  	# next line is an option value pair
 *  	width : 100
 *     	# next line is a flag 
 *      noimages   
 * 
 * + Supports printing out Help and Usage  
 * 
 * + Why not just use getopt() ? 
 *
 *   getopt() Its a POSIX standard not part of ANSI-C. 
 *   So it may not be available on platforms like Windows.
 *
 * + Why it is so long ?
 *
 *   The actual code which does command line parsing 
 *   and option file parsing are done in  few methods. 
 *   Most of the extra code are for providing a flexible
 *   common public interface to both a resourcefile and
 *   and command line supporting POSIX style and  
 *   GNU long option as well as mixing of both. 
 * 
 * + Please see "anyoption.h" for public method descriptions 
 *   
 */

/* Updated Auguest 2004 by Michael D Peters (mpeters at sandia.gov) 
 * to remove static local variables, allowing multiple instantiations 
 * of the reader (for using multiple configuration files).  There is
 * an error in the destructor when using multiple instances, so you
 * cannot delete your objects (it will crash), but not calling the 
 * destructor only introduces a small memory leak, so I
 * have not bothered tracking it down.
 *
 * Also updated to use modern C++ style headers, rather than
 * depricated iostream.h (it was causing my compiler problems)
*/

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdlib.h>
#include <iterator>
#include <string.h>

#include "Macros.h"
#include "AnyOption.h"

using namespace std;

struct EmptyStruct {};

#if defined(ADL_MINIMAL_BUILD) || defined(NO_GC)
# define GC EmptyStruct
#else
# define GC gc
#endif

namespace adl {

#define Verbose(x) { if (_verbose) { cerr << x << "\n"; } }
#define FVerbose(x,y,z) { if (_verbose) { cerr << x << ":" << y << ":  " << z; } }

  static string FlagSet   = "true";
  static string FlagUnset = "false";

  enum OptType { oNone, oRequire, oOptional };

  struct AnyOption::Option {
    Type    _type;
    OptType _opt;
    string  _abrev;
    string  _help;
    string  _set_value;
    Values  _values;

    Option(Type t,OptType o,const string &a,const string &h) : 
      _type(t), _opt(o), _abrev(a), _help(h) 
    {
    };

    void reset() { _values.clear(); };
  };

  AnyOption::AnyOption()
  {
    init();
  }

  AnyOption::AnyOption(unsigned argc, const char *argv[])
  {
    init();
    useCommandArgs(argc,argv);
  }

  AnyOption::~AnyOption()
  {
  }

  void AnyOption::init()
  {
    _max_legal_args = 0;
    _argc = 0;
    _argv = 0;
    _print_usage 	= true;
    _posix_style	= false;
    _verbose 	= true;
    _appname 	= 0;	
    _errors       = false;

    _opt_prefix_char     = '-';
    _file_delimiter_char = ':';
    _file_comment_char   = '#';
    _equalsign 	= '=';
    _comment       = '#' ;
    _delimiter     = ':' ;

    _long_opt_prefix = "--";

    _error_on_unknown = true;
  }

  void AnyOption::reset()
  {
    _args.clear();
    ForEach (_options,i) {
      i->second->reset();
    }
  }

  const string &AnyOption::flagSet() const
  {
    return FlagSet;
  }

  const string &AnyOption::flagUnset() const
  {
    return FlagUnset;
  }

  void AnyOption::setCommandPrefixChar( char prefix )
  {
    _opt_prefix_char = prefix;
  }

  void AnyOption::setCommandLongPrefix( const string &prefix )
  {
    _long_opt_prefix = prefix;
  }

  void AnyOption::setFileCommentChar( char comment )
  {
    _file_delimiter_char = comment;
  }

  void AnyOption::setFileDelimiterChar( char delimiter )
  {
    _file_comment_char = delimiter;
  }

  inline bool AnyOption::commandSet() const
  {
    return _argv;
  }

  inline bool AnyOption::fileSet() const
  {
    return !_filename.empty();
  }

  void AnyOption::noPosix()
  {
    _posix_style = false;
  }

  void AnyOption::errorOnUnknowns(bool e)
  {
    _error_on_unknown = e;
  }

  inline bool AnyOption::posix() const
  {
    return _posix_style;
  }

  void AnyOption::setVerbose()
  {
    _verbose = true ;
  }

  void AnyOption::useCommandArgs( int argc, const char **argv )
  {
    _argc = argc;
    _argv = argv;
    _appname = argv[0];
  }

  std::pair<unsigned,const char **> AnyOption::last_arg_data() const
  {
    return make_pair(_argc,_argv);
  }

  void AnyOption::dump_args(std::ostream &o) const
  {
    ForRange(_argc,i) {
      o << _argv[i] << ' ';
    }
  }


  void AnyOption::useFileName( const string &filename )
  {
    _filename = filename;
  }

  bool AnyOption::setOption( const string &opt, const string &h, Type t )
  {
    return addOption( opt, string(), t, h, oRequire );
  }

  bool AnyOption::setOption( const string &opt, const string &abrev, const string &h, Type t )
  {
    return addOption( opt, abrev, t, h, oRequire );
  }

  bool AnyOption::setFlag( const string &opt, const string &h, Type t )
  {
    return addOption( opt, string(), t, h, oNone );
  }

  bool AnyOption::setFlag( const string &opt, const string &abrev, const string &h, Type t )
  {
    return addOption( opt, abrev, t, h, oNone );
  }

  bool AnyOption::addOption( const string &name, const string &abrev, 
                             Type t, const string &h, int opt )
  {
    shared_ptr<Option> option(new Option(t,(OptType)opt,abrev,h));
    _options[ name ] = option;
    if (!abrev.empty()) {
      _options[ abrev ] = option;
    }
    return true;
  }

  bool AnyOption::setOptionalValue (const string &name,const std::string &set_value)
  {
    auto iter = _options.find(name);
    if (iter != _options.end() ) {
      iter->second->_opt = oOptional;
      iter->second->_set_value = set_value;
    } else {
      throw runtime_error("Option '" + name + "' is not a valid option.");
    }
    return true;
  }

  bool AnyOption::processOptions()
  {
    if (!processCommandLine()) {
      return false;
    }
    if (!processFile()) {
      return false;
    }
    return true;
  }

  bool AnyOption::processCommandLine()
  {
    _max_legal_args = _argc;
    return parseCommandLine();
  }

  bool AnyOption::processCommandLine(int max_args)
  {
    _max_legal_args = max_args;
    return parseCommandLine();
  }

  bool AnyOption::processCommandLine( unsigned argc, const char **argv )
  {
    useCommandArgs( _argc, _argv );
    _max_legal_args = _argc;
    return parseCommandLine();
  }
 
  bool AnyOption::processCommandLine( unsigned argc, const char **argv, int max_args )
  {
    _max_legal_args = max_args;
    useCommandArgs( argc, argv );  
    return parseCommandLine();
  }

  bool AnyOption::parseCommandLine()
  {
    if (!commandSet()) {
      return true;
    }
	 
    _errors = false;

    // Ignore first argv
    for ( int i = 1 ; i < _max_legal_args ; ++i ) {
      // Is it a GNU option?
      if( !strncmp(_argv[i],_long_opt_prefix.c_str(),_long_opt_prefix.size())) {
        i = parseGNU( _argv[i],_argv[i]+_long_opt_prefix.size(),i );
      } 
      // Is it a posix option?
      else if( _argv[i][0] ==  _opt_prefix_char ) {
        if( posix() ){ 
          i = parsePOSIX( _argv[i]+1,i );
        } else { 
          // Treat it as GNU option with a -
          i = parseGNU( _argv[i],_argv[i]+1,i );
        }
      } else { 
        // Not option but an argument keep index.
        _args.push_back(i);
      }
    }

    if (_errors) {
      printUsage();
      return false;
    }
    return true;
  }

  int AnyOption::handleUnknown(const char *orig,const char *arg,int aix,int split_at)
  {
    if (_error_on_unknown) {
      if (split_at > 0) {
        string tmp(arg,split_at);
        Verbose("Unknown command argument option : " << tmp);
      } else {
        Verbose("Unknown command argument option : " << arg);
      }
      _errors = true;
    }
    return aix;
  }

  // Parses posix options.  May consume an additional parameter
  // if needed.  Return value is the new index to use.
  int AnyOption::parsePOSIX( const char* arg, int aix )
  {
    for( const char *c = arg ; *c ; ++c ) { 
      Options::iterator oiter = _options.find(string(c,1));
      // Keep matching flags till an option is encountered.
      if( oiter != _options.end() ) {
        if (!oiter->second->_opt) {
          // This is a flag.
          setFlagOn(oiter,Cmd);
        } else {
          // This is a value.  If last char, argv[++i] is the value.
          if( !(*(++c)) ) { 
            if (isValue(aix+1)) {
              setValue(oiter,_argv[aix+1],Cmd);
              return aix+1;
            } else {
              setFlagOn(oiter,Cmd);
            }
          } else{
            // Else the rest of arg is the value.
            setValue(oiter,c,Cmd);
          }
        }
      } else if (_error_on_unknown) {
        Verbose( "Unknown command argument option : " << *c );
      }
    }
    return aix;
  }

  // Parses gnu options.  May consume an additional parameter
  // if needed.  Return value is the new index to use.
  int AnyOption::parseGNU( const char *orig, const char *arg, int aix )
  {
    int split_at = 0;

    // if has a '=' sign get value.  
    if (const char *e = strchr(arg,_equalsign)) {
      split_at = e - arg;
    }

    if( split_at > 0 ){ 
      // It is an option value pair, so we don't worry about testing the negate feature.
      string tmp(arg,split_at);
      Options::iterator oiter = _options.find(tmp);
      if (oiter != _options.end()) {
        setValue(oiter,arg+split_at+1,Cmd);
      } else {
        return handleUnknown(orig,arg,aix,split_at);
      }
    } else { 
      // regular options with no '=' sign.  The next item might be a value or
      // another option.
      Options::iterator oiter = _options.find(arg);
      if (oiter != _options.end()) {
        if (isValue(aix+1) && oiter->second->_opt) {
          setValue(oiter,_argv[aix+1],Cmd);
          return aix+1;
        } else {
          setFlagOn(oiter,Cmd);
        }
      } else {
        if ( (strlen(arg) > 3) && (strstr(arg,"no-") == arg)) {
          oiter = _options.find(&arg[3]);
          if (oiter != _options.end()) {
            // Found a negated version, so toggle it off.
            setFlagOff(oiter,Cmd);
            return aix;
          }
        }
        return handleUnknown(orig,arg,aix,split_at);
      }
    }
    return aix;
  }

  bool AnyOption::isValue(int i)
  {
    if (i >= _max_legal_args) {
      return false;
    } else {
      return ( (_argv[i][0] != _opt_prefix_char) && 
               strncmp(_argv[i],_long_opt_prefix.c_str(),_long_opt_prefix.size()));
    }
  }

  const AnyOption::Values &AnyOption::getValues (const string &option) const
  {
    Options::const_iterator i = _options.find(option);
    if (i != _options.end()) {
      return i->second->_values;
    } else {
      throw runtime_error("Option '" + option + "' is not a valid option.");
    }
  }

  string AnyOption::getOption( const string &option ) const 
  {
    Options::const_iterator i = _options.find(option);
    if (i != _options.end()) {
      if (!i->second->_values.empty()) {
        return i->second->_values.back();
      }
    }
    return string();
  }

  string AnyOption::getOption( const string &option, const string &def ) const
  {
    string res = getOption(option);
    if (res.empty()) {
      return def;
    } else {
      return res;
    }
  }

  int AnyOption::getOption (const std::string &option,int def ) const
  {
    string res = getOption(option);
    if (res.empty()) {
      return def;
    } else {
      char *err;
      return strtoul(res.c_str(),&err,0);
    }
  }

  uint64_t AnyOption::getOption (const std::string &option,uint64_t def ) const
  {
    string res = getOption(option);
    if (res.empty()) {
      return def;
    } else {
#     ifdef _MSC_VER
		return _atoi64(res.c_str());
#     else
      char *err;
      return strtoull(res.c_str(),&err,0);
#     endif
    }
  }

  double AnyOption::getOption (const std::string &option,double def ) const
  {
    string res = getOption(option);
    if (res.empty()) {
      return def;
    } else {
      char *err;
      return strtod(res.c_str(),&err);
    }
  }

  bool AnyOption::haveOption (const std::string &option) const
  {
    Options::const_iterator i = _options.find(option);
    return (i != _options.end()) && (!i->second->_values.empty());
  }

  // We only check that it's not set to FlagUnset so that options with optional values
  // are considered set as long as they haven't been negated.
  bool AnyOption::getFlag(const string &option ) const
  {
    Options::const_iterator i = _options.find(option);
    if (i != _options.end() && !i->second->_values.empty()) {
      return (i->second->_values.back() != FlagUnset);
    } else {
      return false;
    }
  }

  // We return an explicit true/false value if the option has a value (if set to
  // FlagUnset then it's false).  Otherwise, we return the default value.
  bool AnyOption::getFlag(const string &option,bool def ) const
  {
    Options::const_iterator i = _options.find(option);
    if (i != _options.end() && !i->second->_values.empty()) {
      if (i->second->_values.back() == FlagUnset) {
        return false;
      } else if (!i->second->_values.back().empty()) {
        return true;
      }
    }
    return def;
  }

  bool AnyOption::setValue( const std::string &option, const std::string &value )
  {
    Options::iterator oiter = _options.find(option);
    if (oiter != _options.end()) {
      setValue(oiter,value,Cmd);
      return true;
    } else {
      return false;
    }
  }

  bool AnyOption::setFlagOn( const std::string &option )
  {
    Options::iterator oiter = _options.find(option);
    if (oiter != _options.end()) {
      setFlagOn(oiter,Cmd);
      return true;
    } else {
      return false;
    }
  }

  bool AnyOption::setFlagOff( const std::string &option )
  {
    Options::iterator oiter = _options.find(option);
    if (oiter != _options.end()) {
      setFlagOff(oiter,Cmd);
      return true;
    } else {
      return false;
    }
  }

  void AnyOption::setValue( Options::iterator oiter, const string  &value, Type t )
  {
    Option &opt = *(oiter->second);
    if ( opt._opt == oNone ) {
      Verbose("Option '" << oiter->first << "' does not take a value.");
      _errors = true;
      return;
    }
    if (opt._type == File && t == Cmd) {
      Verbose("Option '" << oiter->first << "' is not a command-line option.");
      _errors = true;
    } 
    else if (opt._type == Cmd && t == File) {
      Verbose("Option '" << oiter->first << "' is not a resource file option.");
      _errors = true;
    }
    opt._values.push_back(value);
  }

  void AnyOption::setFlagOn( Options::iterator oiter, Type t )
  {
    Option &opt = *(oiter->second);
    if ( opt._opt == oRequire ) {
      Verbose("Option '" << oiter->first << "' takes a value and one was not specified.");
      _errors = true;
      return;
    }
    if (opt._type == File && t == Cmd) {
      Verbose("Option '" << oiter->first << "' is not a command-line flag.");
      _errors = true;
    } 
    else if (opt._type == Cmd && t == File) {
      Verbose("Option '" << oiter->first << "' is not a resource file flag.");
      _errors = true;
    }
    
    if (opt._set_value.empty()) {
      opt._values.push_back(FlagSet);
    } else {
      opt._values.push_back(opt._set_value);
    }
  }

  void AnyOption::setFlagOff( Options::iterator oiter, Type t )
  {
    Option &opt = *(oiter->second);
    if ( opt._opt == oRequire ) {
      Verbose("Option '" << oiter->first << "' takes a value and one was not specified.");
      _errors = true;
      return;
    }
    if (opt._type == File && t == Cmd) {
      Verbose("Option '" << oiter->first << "' is not a command-line flag.");
      _errors = true;
    } 
    else if (opt._type == Cmd && t == File) {
      Verbose("Option '" << oiter->first << "' is not a resource file flag.");
      _errors = true;
    }

    opt._values.push_back(FlagUnset);
  }

  int AnyOption::getArgc( )
  {
    return _args.size();
  }

  const char* AnyOption::getArgv( unsigned index )
  {
    if (index < _args.size()) {
      return (_argv[_args.at(index)]);
    } else {
      return 0;
    }
  }

  bool AnyOption::processFile()
  {
    if( !fileSet() ) {
      return false;
    }
    readFile();

    if (_errors) {
      printUsage();
      return false;
    }
    return true;
  }

  bool AnyOption::processFile( const string &fn )
  {
    useFileName(fn);
    return processFile();
  }

  string get_word(const string &line,size_t start,size_t end)
  {
    size_t kb = line.find_first_not_of(" \t");
    if (kb >= end) {
      return string();
    }
    size_t ke = line.find_last_not_of(" \t",end-1);
    return line.substr(kb,(ke-kb));
  }

  void AnyOption::readFile()
  {
    _errors = false;
    ifstream is(_filename.c_str());

    if (!is) {
      return;
    }

    // Loop over each line in the file, processing it.  Empty lines
    // and comments are skipped.  A valid line contains a key, the 
    // delimiter, and an optional value.  If a value is present, we call
    // setValue.  No value means we call setFlagOn.  Whitespace is
    // trimmed from the front and back of both items.
    string line;
    int lc = 1;
    size_t d;
    while (is) {
      getline(is,line);
      if (line.empty()) {
        continue;
      }
      if (line[0] == _comment) {
        continue;
      }

      // Find delimiter.
      if ( (d = line.find(_delimiter)) == string::npos) {
        FVerbose(_filename,lc,"Invalid format.");
        _errors = true;
        continue;
      }

      // Find the option.
      string opt = get_word(line,0,d);
      if (opt.empty()) {
        FVerbose(_filename,lc,"No option/flag found.");
        _errors = true;
        continue;
      }

      bool flag_set = true;
      Options::iterator oiter = _options.find(opt);
      if (oiter == _options.end()) {
        // Check for the negated option.
        if (opt.size() > 3 && opt.find("no-") == 0) {
          oiter = _options.find(opt.substr(3));
          flag_set = false;
        }
      }

      if (oiter == _options.end()) {
        if (_error_on_unknown) {
          FVerbose(_filename,lc,"Unknown option:  " << opt);
          _errors = true;
        }
        continue;
      }

      // Find the value.
      string value = get_word(line,d,line.size());

      if (value.empty()) {
        if (flag_set) {
          setValue(oiter,value,File);
        } else {
          FVerbose(_filename,lc,"Negated options with values are not supported.");
        }
      } else {
        if (flag_set) {
          setFlagOn(oiter,File);
        } else {
          setFlagOff(oiter,File);
        }
      }
    }
  }

  //
  // usage and help 
  //

  bool AnyOption::usage() const
  {
    return _print_usage;
  }

  void AnyOption::noUsage()
  {
    _print_usage = false;	
  }

  void AnyOption::printOpt(ostream &o,const string &name,bool is_opt) const
  {
    if (name.size() > 1) {
      o << _long_opt_prefix;
    } else {
      o << _opt_prefix_char;
    }
    o << name;
    if (is_opt) {
      o << "=<val>";
    }
  }

  void AnyOption::printUsage() const
  {
    printUsage(cout);
  }

  void AnyOption::printUsage(ostream &o) const
  {
    if( !usage() )
      return;

    o << "\n";
    if (_appname) {
      o << _appname;
    }
    ostream_iterator<string> out(o,"\n");
    copy(_usage.begin(),_usage.end(),out);
  
    o << "\nOptions:\n";
    ForEach(_options,i) {
      const Option &opt = *(i->second);
      const string &name = i->first;
      // Skip an abreviation.
      if (name == opt._abrev) { continue; }
      o << "  ";
      printOpt(o,name,opt._opt);
      if (!opt._abrev.empty()) {
        o << ", ";
        printOpt(o,opt._abrev,opt._opt);
      }
      if (opt._type == File) {
        o << "(rc file only) ";
      } else if (opt._type == Cmd) {
        o << "(cmd-line only) ";
      }
      o << ":\n";
      string pstring(6,' ');
      // Print the help string.  If we encounter a newline, insert a whitespace prefix.
      o << pstring;
      ForEach(opt._help,hiter) {
        o << *hiter;
        if (*hiter == '\n') {
          o << pstring;
        }
      }
      o << "\n\n";
    }
  }

  void AnyOption::addUsage( const string &l )
  {
    _usage.push_back(l) ;	
  }

}

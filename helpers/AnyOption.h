//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// General command-line parser for use by clients.  This can parse data from
// either the command-line or an options file or both.  Derive from this class
// in order to extend option processing, such as to pass command-line options
// not handled by AnyOption to another database.
//

#ifndef _ANYOPTION_H
#define _ANYOPTION_H

#include <iosfwd>
#include <string>

#ifndef _MSC_VER
# include "adl_config.h"
#endif

#include <memory>
# include <vector>
# include <unordered_map>
# define OptionVector std::vector
# define OptionHashMap std::unordered_map

#include "BasicTypes.h"

namespace adl {

  class AnyOption {
  public:
    typedef OptionVector<std::string> Values;

    AnyOption();
    AnyOption(unsigned argc,const char *argv[]);
    virtual ~AnyOption();

    //  
    //  Following set methods specify the  
    //  special characters and delimiters 
    //  if not set traditional defaults will be used
    //  
    void setCommandPrefixChar( char );   /* '-' in "-w" */
    void setCommandLongPrefix( const std::string & );  /* '--' in "--width" */
    void setFileCommentChar( char  );    /* '#' in shellscripts */
    void setFileDelimiterChar( char  );/* ':' in "width : 100" */

    // 
    // Provide the input for the options
    // like argv[] for commndline and the 
    // option file name to use.
    //
    void useCommandArgs( int argc, const char **argv );
    void useFileName( const std::string &fn );

    // Flush the values database.
    void reset();

    //
    // Turn off the POSIX style options 
    // this means anything starting with a '-' or "--"
    // will be considered a valid option 
    // which also means you cannot add a bunch of 
    // POSIX options chars together like "-lr"  for "-l -r".
    //
    void noPosix();

    //
    // If set to true (default), prints usage and returns false if unknown
    // options are encountered.
    // Otherwise, ignored unknown options and sets known options in argv to null
    // so that a second pass will ignore them.
    //
    void errorOnUnknowns(bool);

    //
    // Prints warning verbose if you set anything wrong .
    //
    void setVerbose();

    //
    // There are two types of options:
    //
    // Option:  Has an associated value ( -w 100 )
    // Flag  :  No value, just a boolean flag  ( -nogui )
    //          It also supports negation, i.e. you can add
    //          "no-" to the start of a flag to force it to be
    //          stored as a negated option.  Calling the version of
    //          getFlag that takes a default argument allows you
    //          to exercise this behavior.
    //
    // The options can be either a string ( GNU style )
    // or a character ( traditional POSIX style ) 
    // or both ( --width, -w )
    //
    // The options can be common to the commandline and 
    // the optionfile, or can belong only to either of 
    // commandline and optionfile
    //
    // Following set methods, handle all the above 
    // cases of options.
    //
    enum Type {Common,Cmd,File};

    bool setOption( const std::string &, const std::string &help, Type = Common );
    bool setOption( const std::string &, const std::string &, const std::string &help, Type = Common );
    bool setFlag( const std::string &, const std::string &help, Type = Common );
    bool setFlag( const std::string &, const std::string &, const std::string &help, Type = Common );

    // This allows an option to take an optional value, thus allowing for the
    // user to either supply a value or to negate/set the flag.  If a set-value
    // is supplied, then this is the value given to the option if the option is
    // simply set as a flag.  It defaults to the default value used for setting
    // a flag.
    bool setOptionalValue( const std::string &opt, const std::string &set_value = "");

    //
    // Process the options, registered using useCommandArgs() and useFileName();
    // Note that if error_on_unknown is false, then known command-line options
    // will be cleared so that second pass will ignore then.
    //  
    bool processOptions();  
    bool processCommandLine();
    bool processCommandLine( int max_args );
    bool processFile();

    //  
    //  Process the specified options 
    //  
    bool processCommandLine( unsigned argc, const char **argv );
    bool processCommandLine( unsigned argc, const char **argv, int max_args );
    bool processFile( const std::string &fn );
        
    // This returns the raw argc/argv pair that was last processed.
    std::pair<unsigned,const char **> last_arg_data() const;

    // Dump the raw arguments to a stream.
    void dump_args(std::ostream &) const;

    //  
    // Get the value of the options.  We return the last value that was specified.
    // Otherwise, use the getOptions function to get the full list. 
    // Will return an empty string if no value set. 
    //  
    // Returns the list of values.  Throws a runtime error if the option does not exist.
    const Values &getValues (const std::string &option) const;
    // Returns the value of an option.  If not given, then an empty string is returned.
    std::string getOption( const std::string &option ) const;
    // Returns 'default' if the option is not set.
    std::string getOption( const std::string &option, const std::string &def ) const;
    // Returns the option, converted to an integer.  If not set, returns the default
    // value.  We just do a simple conversion, so a non-integer value is converted to 0.
    int getOption (const std::string &option,int def ) const;
    uint64_t getOption (const std::string &option,uint64_t def ) const;
    // For 64-bit targets, we directly support unsigned long long so that
    // the use of ULL constants will work.
#   ifdef IS_64BIT
    unsigned long long getOption (const std::string &option,unsigned long long def ) const {
      return getOption(option,(uint64_t)def);
    }
#   endif
    double getOption (const std::string &option,double def ) const;
    // Returns true if the option or flag was specified on the command-line.
    bool haveOption( const std::string &option ) const;
    // Returns true if the specified flag is set, false if not set.
    bool getFlag( const std::string &option ) const;
    // Returns true if the flag is set, false if negated, or the default value
    // if not set.
    bool getFlag( const std::string &option,bool def) const;

    // Force in values for options.  Returns false if the option doesn't exist,
    // otherwise true.
    bool setValue( const std::string &option, const std::string & );
    bool setFlagOn( const std::string &option );
    bool setFlagOff( const std::string &option );

    //
    // Print Usage
    //
    void printUsage() const;
    void printUsage(std::ostream &) const;
    void addUsage( const std::string & );
    void noUsage();

    // Returns argv[0], which is usually the name of the executable.
    const char *getArgv0() const { return _argv[0]; };
        
    // Default flag-set/flag-not-set string values.
    const std::string &flagSet() const;
    const std::string &flagUnset() const;

    // 
    // Get the argument count and arguments sans the options.  Returns 0 if the
    // parameter doesn't exist.
    //
    int getArgc();
    const char *getArgv( unsigned index );

    // Override this method in a derived class to customize the handlinrg of
    // unknown options.  The return value should be the index of the next item
    // to process (generally just return 'aix').
    virtual int handleUnknown(const char *orig,const char *arg,int aix,int split_at);

    struct Option;
    typedef OptionHashMap<std::string,std::shared_ptr<Option> > Options;

  private:
    unsigned _argc;           // commandline arg count
    const char **_argv;       // commndline args
    std::string _filename;    // the option file
    const char* _appname;     // the application name from argv[0]

    OptionVector<int> _args;  // Remaining args (indices into _argv).

    int _max_legal_args;      // ignore extra arguments

    Options  _options;

    OptionVector<std::string> _usage;

    bool _error_on_unknown; // true => return false if unknown options are encountered.
                            // false => return true and null-out known options.
    bool _posix_style;      // enables to turn off POSIX style options.
    bool _verbose;          // silent|verbose
    bool _print_usage;      // usage verbose
    bool _print_help;       // help verbose

    bool _errors;
        
    char _opt_prefix_char;        //  '-' in "-w"
    std::string _long_opt_prefix; // '--' in "--width"
    char _file_delimiter_char;    // ':' in width : 100
    char _file_comment_char;      //  '#' in "#this is a comment"
    char _equalsign;
    char _comment;
    char _delimiter;

  private:
    void init();          

    bool parseCommandLine();

    bool isValue(int);

    void setValue( Options::iterator, const std::string &, Type );
    void setFlagOn( Options::iterator, Type );
    void setFlagOff( Options::iterator oiter, Type t );

    bool addOption( const std::string &, const std::string &, Type, 
                    const std::string &, int );

    bool commandSet() const;
    bool fileSet() const;
    bool posix() const;
    bool usage() const;

    int parsePOSIX( const char*, int );
    int parseGNU( const char *orig,const char *arg,int aix);

    void readFile();

    void printOpt(std::ostream &,const std::string &,bool) const;
  };

}

#endif


//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef DATREADER_H
#define DATREADER_H 

#include <unordered_map>

#include "Reader.h"
#include "DatParser.h"

namespace adl {

  // The main DAT file reader class.
  // Input comes from an mmap'd file, so we don't use the FileReader class.
  // All tokens are stored using the Token class above, in order to reduce the need
  // for copying.
  class DatReader : public Reader, public DatParser {  
  public:
    DatReader (const std::string& filename,IssNode *root,bool compressed = false);
    virtual ~DatReader () {}

    const char *format_str() const { return "dat"; };

    void setRoot(IssNode *root);

    bool readFile();

    bool finished() const { return _finished; };
    
    // Trace reader methods; currently not working
    
    void initTraceReading();
    
    bool hasTraceSupport() { return true; };
    
    bool applyNext();
    
    uint64_t instrCount();
    
    // End Trace reader methods

    struct CoreInfo {
      CoreInfo(IssNode *node) : _node(node) {};

      // For storing enumeration information for registers, i.e., strings
      // which are read/write for specified value
      typedef std::unordered_map<std::string,unsigned> EnumValues;
      typedef std::unordered_map<std::string,EnumValues>  RegEnumsHash;

      IssNode  		     *_node;
      RegEnumsHash      _reg_enums;
    };

    typedef std::unordered_map<std::string,CoreInfo>    CoreMap;
    typedef std::unordered_map<std::string,std::string> CoreNameMap;
    typedef CoreInfo::RegEnumsHash                      RegEnumsHash;

  private:

    virtual pair<const char*,unsigned> getline_compressed();
    void populateCoreMap(const std::string &name,IssNode &node);
    CoreMap::iterator lookupCore(std::string n);
    IssNode *getCore(const std::string &n);
    RegEnumsHash *getRegEnums(const std::string &n);
    IssCore *get_core_ptr();
    void get_cache_data(const char *k,CacheLineData &data,bool erase = false);
    WatchType get_watch_type(const char *k);
    void setup_core_name_map();

    virtual bool handleCard(int ln,const Token &type);
    virtual void error(int ln,const std::string &msg);
    
    // Trace reader
    
    bool handle_trace_card(uint64_t step, const Token &type);

    const char   *_last_line;
    
    bool _trace_mode;
    // End Trace reader
    
    CoreMap       _cores;
    IssNode   	 *_cur_core;
    RegEnumsHash *_cur_reg_enums;
    bool          _first_test;
    bool          _record;
    CoreNameMap   _core_name_map;
    
  };

}

#endif

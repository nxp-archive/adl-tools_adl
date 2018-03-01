//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This is a reader for reading in UVP files, which are produced by the Raptor
// testcase generator and used by testbenches in PowerPC-land.
//

#ifndef UVPREADER_H
#define UVPREADER_H 

#include <map>

#include "Reader.h"
#include "UvpParser.h"
#include "NodeHelpers.h"

namespace adl {

  struct MmuLookupInfo;

  struct UvpResValue {
    uint32_t _value;  // We support up to a 32-bit value for a RES card.
    int      _size;   // Size is determined from the input string.
    
    UvpResValue() : _value(0), _size(0) {};
    UvpResValue(uint32_t v) : _value(v), _size(32) {};
    UvpResValue(uint32_t v,int s) : _value(v), _size(s) {};
  };

  // The keys are ta values.  If a mask is specified, the mask is applied to
  // the ta value first, before comparisons are made.  If only one value
  // exists with a key of 0, then we just use that directly.
  struct UvpResValues : public std::map<unsigned,UvpResValue> {
    unsigned    _ta_res_mask;
    bool        _res_single_value;
    bool        _inited;

    UvpResValues(unsigned mask) : _ta_res_mask(mask), _res_single_value(false), _inited(false) {};
  };

  UvpResValues &uvp_res_values();

  // The main UVP file reader class.
  // Input comes from an mmap'd file, so we don't use the FileReader class.
  class UvpReader : public Reader, public UvpParser {  
  public:
    UvpReader (const std::string& filename,IssNode *root);
    virtual ~UvpReader () {}

    const char *format_str() const { return "uvp"; };

    bool readFile();

    bool finished() const { return is_finished(); };

    void setRoot(IssNode *root);

    void handle_block_begin(const YYLTYPE &loc,std::unique_ptr<UvpToken> type,std::unique_ptr<UvpToken> name);
    void handle_block_end(const YYLTYPE &loc);
    void handle_cmd(const YYLTYPE &loc,std::unique_ptr<UvpCmd> cmd);
    void handle_parm(const YYLTYPE &loc,std::unique_ptr<UvpToken> name,std::unique_ptr<UvpValue> value);

  private:
    void parse_res_value();
    void setup(IssNode &);
    void update_cur(const YYLTYPE &loc,const UvpToken &type,const UvpToken &name);
    std::string hack_up_name(const std::string &str);
    uint32_t hack_up_tlb(FieldData &fields,const std::string &name,addr_t ep);

	public:
    typedef std::map<char,std::string>                  StrMap;

    struct MmuLookupItem {
      int                  _addr_shift;  // For handling Raptor UVPs.
      const MmuLookupInfo *_lookup;      // Information about the lookup.

      MmuLookupItem(const MmuLookupInfo *l) : _addr_shift(0), _lookup(l) {};
    };
    typedef std::map<std::string,MmuLookupItem> MmuMap;

  private:

    StrMap      _regname_transforms;
    MmuMap      _mmulookups;

    Target      _cur;
    bool        _top;
    bool        _handled_test;
    bool        _log_result_mem;
  };

}

#endif


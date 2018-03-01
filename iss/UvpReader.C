//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This is a reader for reading in UVP files, which are produced by the Raptor
// testcase generator and used by testbenches in PowerPC-land.
//

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"
#include "helpers/BitTwiddles.h"
#include "helpers/AnyOption.h"

#include "rnumber/RNumber.h"

#include "StringHelpers.h"
#include "UvpReader.h"
#include "UvpParser.h"
#include "UvpParseTypes.h"

using namespace std;
using namespace rnumber;

typedef std::pair<const adl::UvpValue *,const adl::UvpValue *> UvpPair;
typedef std::pair<adl::UvpReader::MmuMap::iterator,bool> MmuMapInsertPair;

const int MaxWords = 5;

namespace adl {

  // Add command-line options to the option database.  Note the use of logical &&
  // to ensure that we evaluate all of the additions.
  AnyOption &options();

  static bool dummy = 
    options().setFlag  ("uvp-log-result-mem",    "Causes the UVP reader to log memory in the result sections, so that tracing is not required.\n"
                                                 "Default = false") &&
    options().setOption("uvp-res-value",         "Specify a value to be set for the RES card location in any UVP that is loaded.\n"
                                                 "This value may be either a constant or of the form ta=value[,ta=value...].  In\n"
                                                 "the latter case, the value set is determined by the ta value of the RES card.\n"
                                                 "If the --uvp-ta-mask option is set, then that mask is applied first before any\n"
                                                 "comparison is made against the ta field.  If a value of 0 is specified for this\n"
                                                 "option then the option will be ignored.") &&
    options().setOption("uvp-ta-mask",           "Specify a mask value to apply against the 'ta' field when calculating the appropriate\n"
                                                 "RES value to use.  A value of 0 implies that no masking should be performed.");

  UvpResValues &uvp_res_values()
  {
    static UvpResValues rv(options().getOption("uvp-ta-mask",0));
    return rv;
  }

  UvpReader::UvpReader(const string& filename,IssNode *root) :
    Reader(filename,root),
    UvpParser(filename,false /* Set to true to get copious bison debugger information */),
    _cur(root),
    _top(true),
    _handled_test(false),
    _log_result_mem(options().getFlag("uvp-log-result-mem",false))
  {
    setup(*root);
    parse_res_value();
  }

  void UvpReader::setRoot(IssNode *root)
  {
    UvpParser::reset();
    _cur = root;
    _regname_transforms.clear();
    _mmulookups.clear();
    setup(*root);
  }

  // Attempts to calculate the size of an integer constant.  Returns 32 by default.
  int calc_size(const string &in)
  {
    if (in.compare(0,2,"0x") == 0) {
      // It's a hex constant, so we can do a comparison.
      int nb = (in.size() - 2)/2;
      
      // We have the number of bytes.  Now convert to a length that's OK:  1,2,4.
      switch (nb) {
      case 1:
      case 2:
      case 4:
        break;
      case 3:
      default:
        nb = 4;
      }
      return nb*8;
    }
    return 32;
  }

  // Parse the res value, if one exists.
  void UvpReader::parse_res_value()
  {
    UvpResValues &rv = uvp_res_values();

    if (rv._inited) {
      return;
    }

    string in = options().getOption("uvp-res-value");

    if (!in.empty()) {
      // Only process if we have something.  Otherwise, this is ignored.

      // First, tokenize on commas.
      auto ta_pairs = tokenize(in,",");

      // For each result, tokenize on "=".
      ForEach(ta_pairs,mp) {
        auto ta_pair = tokenize(*mp,"=");
        // If we have two elements, then we expect that to be ta=value pair.
        // Otherwise, we expect it to be a simple value.  All simple values are
        // placed in to the "0" key, which means to use this unconditionally.
        if (ta_pair.size() == 1) {
          // Unconditional case.
          uint32_t v;
          if (!getUInt(v,ta_pair[0])) {
            RError("Bad UVP RES value:  Expected an integer, but got " << ta_pair[0]);
          }
          rv[0] = UvpResValue(v,calc_size(ta_pair[0]));
          rv._res_single_value = true;
        } else if (ta_pair.size() >= 2) {
          // ta=value case.
          uint32_t ta,v;
          if (!getUInt(ta,ta_pair[0])) {
            RError("Bad UVP RES value:  Expected an integer for a ta value, but got " << ta_pair[0]);
          }
          if (!getUInt(v,ta_pair[1])) {
            RError("Bad UVP RES value:  Expected an integer fora value, but got " << ta_pair[1]);
          }
          rv[ta] = UvpResValue(v,calc_size(ta_pair[1]));
        }
      }
    }
    rv._inited = true;
  }

  // For each register file that we know about, we setup a map from the leading
  // letter to the actual name.  Sparse register files are skipped b/c we don't
  // index into the register file in a UVP.
  //
  // We also populate the mmu lookup hash.
  //
  // Eventually, we should suppor heterogenous cores, but I'm feeling lazy today.
  void UvpReader::setup(IssNode &node)
  {
    if (node.is_core()) {
      const RegInfos &rinfo = node.getRegInfo();
      ForEach(rinfo,i) {
        if ( i->_size && !i->_sparse) {
          // This is a register file that's not sparse.
          _regname_transforms[ i->_name[0] ] = i->_name;
        }   
      }

      const MmuInfo &minfo = node.getMmuInfo();
      ForEach(minfo,i) {
        const MmuLookupInfo &li = *i;
        auto ip = _mmulookups.insert(make_pair(i->_name,MmuLookupItem(&li)));
        // Raptor shift hack.
        ip.first->second._addr_shift = li._pageshift;
      }

    } else {
      setup(*(node.children().front()));
    }  
  }

  void UvpReader::update_cur(const YYLTYPE &loc,const UvpToken &type,const UvpToken &name)
  {
    if (_top) {
      // Top-level item.  We just compare and fail if they differ.
      if ( !(name == _root->name())) {
        RError("Unrecognized top-level item:  " << (const UvpValue&)type << " => " << (const UvpValue&)name);
      }
      _cur.set_node(_root);
      _top = false;
    }
    else if (_cur.node()->is_core()) {
      // We are in a core, so this hierarchy level must refer to a context.
      _cur.set_ctx_id(_cur.node()->getCtxId(type.str()));
      string n = name.str();
      size_t index_start = n.find_last_not_of("0123456789");
      if (index_start == string::npos) {
        RError("Invalid context name:  Expected a name of the form <name><index>");
      }
      _cur.set_ctx_num(strtoul(&(n.c_str()[index_start+1]),0,0));
      _cur.setDbgCtx();
    } 
    else {
      // Not a core, so we must be a system:  Try to find a child.
      const IssChildren &ch = _cur.node()->children();
      ForEach(ch,i) {
        if ( (*i)->name().compare(0,name.size(),name.data(),name.size()) == 0) {
          _cur.set_node(*i);
          return;
        }
      }
      RError("Unrecognized child item:  " << (const UvpValue&)type << " => " << (const UvpValue&)name);
    }
  }

  // Main entry point for reading data.  We loop until we've gotten a first test.
  // Anything before this first test is ignored.
  // We also track whether we've read a test and return false if we're done and we didn't
  // do anything.
  bool UvpReader::readFile()
  {
    _handled_test = false;
    while (!_handled_test && !is_finished()) {
      parse();
    }
    return !is_finished();
  }

  // If this is a test, then toggle on the test flag.  If we're already in a test, then we try to
  // find the equivalent node in the hierarchy and store a pointer to it.
  void UvpReader::handle_block_begin(const YYLTYPE &loc,unique_ptr<UvpToken> type,unique_ptr<UvpToken> name)
  {
    try {
      if (!_handled_test) {
        if ( *type == "test" ) {
          _handled_test = true;
        }
      } else {
        update_cur(loc,*type,*name);
      }
    }
    catch (exception &err) {
      RError(printLocation(loc) << ": " << err.what());
    }
  }

  // Done with a block, so go up a level to the parent.  This means either exiting from a
  // context or moving up in the hierarchy.
  void UvpReader::handle_block_end(const YYLTYPE &loc)
  {
    try {
      _cur = _cur.parent();
      if (!_cur.node()) {
        _top = true;
        _cur.set_node(_root);
      }
    }
    catch (exception &err) {
      RError(printLocation(loc) << ": " << err.what());
    }
  }

  // Ugh: We have to hack up the TLB fields in a few ways b/c Raptor applies some
  // weird shifts to a few fields.  We also need to extract the set, since that's
  // not explicitly stated.  All in all, this makes the UVP format very PowerPC
  // (and Raptor!) specific.
  //
  // The current hacks are:
  // 
  // 1.  If the lookup is fixed in size, we have to extract the set by shifting by
  // the page-size and masking by the number of sets.  Otherwise, the set is
  // considered to be 0, since we assume fully-associative.
  //
  // 2.  The fields "EPN" and "RPN" are shifted 
  //
  uint32_t UvpReader::hack_up_tlb(FieldData &fields,const std::string &name,addr_t ep)
  {
    unsigned set = 0;

	MmuMap::iterator miter = _mmulookups.find(name);
    if (miter != _mmulookups.end()) {
      const MmuLookupItem &mitem = miter->second;
      const MmuLookupInfo &minfo = *(mitem._lookup);

      // If the page-size is fixed, then extract the set.
      if (minfo._pagesize) {

        // Get the size log(2).
        unsigned shift = count_trailing_zeros(minfo._pagesize);

        set = (ep >> shift) & (minfo._sets-1);
      }

      // Now go and modify the necessary fields.
      ForEach(fields,i) {
        if ( i->_name == "EPN" || i->_name == "RPN" ) {
          i->_value >>= mitem._addr_shift;
        }
      }

    } else {
      RError("Unknown TLB type:  " << name);
    }

    return set;
  }

  // Validates that the value is a pair and returns the two elements of the
  // pair.  Throws a runtime_error with pmsg if this is not the case.
  UvpPair get_pair(const UvpValue &v,const char *pmsg)
  {
    const UvpList &p = get_uvptype<UvpList>(v,"list",pmsg);
    auto iter = p.begin();
    if (iter != p.end()) {
      const UvpValue *e1 = iter->get();
      ++iter;
      if (iter != p.end()) {
        const UvpValue *e2 = iter->get();
        return make_pair(e1,e2);
      } else {
        RError("Expected a two-element list for " << pmsg);
      }
    } else {
      RError("Empty list found for " << pmsg);
    }
  }

  // Returns the size of the opcode in number of bits.
  // 32-bit is the default size.
  int get_size(const UvpCmd &cmd)
  {
    // By default, we consider opcodes to be 32-bit words, but if an
    // op_size parameter is specified, we use it instead.  The value
    // should be in terms of bits.
    int size = 32;
    auto size_iter = cmd._parms.find_key("op_size");
    if (!cmd._parms.not_found(size_iter)) {
      size = uvtoi32(*size_iter->second,"opcode size");
    }
    return size;
  }

  void UvpReader::handle_cmd(const YYLTYPE &loc,unique_ptr<UvpCmd> cmd_p)
  {
    const UvpCmd &cmd = *cmd_p;
    try {
      // Ignore anything before the first test.

      // Now try and handle the command.  We ignore result stuff and we only care
      // about execution (trace) stuff if it's a non-executed instruction, in
      // which case we use it to initialize memory.
      if ( *(cmd._port) == "I" ) {

        if ( *(cmd._cmd) == "R" ) {
          // Register initialization.
          string name  = get_str(cmd,"nm","register name.");
          RNumber data = get_rn(cmd,"d","register data.");
          init_reg_internal(*_cur.node(),name,0,data);
        }
        else if ( *(cmd._cmd) == "D" ) {
          // Data memory initialization.
          addr_t addr = get_uint64(cmd,"ra","address.");
          uint32_t data = get_uint32(cmd,"d","data.");
          init_mem_internal(*_cur.node(),GlobalMemName,addr,data,32);
        }
        else if ( *(cmd._cmd) == "T" ) {
          // TLB initializations.
          string name = get_str(cmd,"n","TLB name.");
          uint32_t way  = get_uint32(cmd,"way","TLB way.");
          addr_t ep   = get_uint64(cmd,"ep","effectve page.");
          const UvpList &fields = find_key<UvpList>(cmd._parms,"d","list","TLB fields.");
        
          FieldData fd;
          ForEach(fields,i) {
            auto p = get_pair(**i,"TLB field pair.");
            string name = uvtostr(*p.first,"TLB field name");
            addr_t value = uvtoi64(*p.second,"TLB field value");
            fd.push_back(FieldPair(name,value));
          }

          if (fd.empty()) {
            RError("No TLB lookup key/value pairs were found on this line.");
          }

          // Now to apply the miscellaneous hacks which are necessary in order to
          // massage what Raptor usually produces to what we can actually use.  We
          // also have to extract the set, since that's not explicit in the UVP
          // TLB card.
          uint32_t set = hack_up_tlb(fd,name,ep);
          //        cout << "Tlb Init:  n: " << name << ", s: " << set << ", w: " << way << ", ep: 0x" << hex << ep << ", fields:  " << fd << "\n\n";
          init_mmu_internal(*_cur.node(),name,set,way,fd);
        }
        else {
          cerr << printLocation(loc) << ":  Warning:  Unknown UVP command:  " << cmd._port << " : " << cmd._cmd << ".\n";
        }

      } else if ( *(cmd._port) == "E" ) {
        
        if ( *(cmd._cmd) == "I" ) {
          // Instruction memory initialization.
          int size = get_size(cmd);
          const UvpValue &ra = find_key(cmd,"ra","real address");
          if ( get_uvptype<UvpList>(ra) ) {
            // If we have a list for the real-address then we're dealing with a
            // misaligned instruction.  We expect the data and op_size values to also be lists.
            //auto p_ra = get_pair(ra,"misaligned real-address pair");
	    
            const UvpList &l_ra = get_uvptype<UvpList>(ra,"list","misaligned real-address pair");
            const UvpValue &d = find_key(cmd,"d","data");
            //addr_t ra1 = uvtoi64(*p_ra.first,"First real-address of a misaligned pair.");
            //addr_t ra2 = uvtoi64(*p_ra.second,"Second real-address of a misaligned pair.");
            
            addr_t ra_[MaxWords];
            unsigned ra_num = 0;
            ForEach(l_ra,i) {
              ra_[ra_num] =  uvtoi64(**i,"Real-address number element of a misaligned list.");
              ++ra_num;	    
            }
            if (!ra_num) {
              RError("Empty list found for misaligned real-address list");
            }
	    
            const UvpList &l_d = get_uvptype<UvpList>(d,"list","data");
            unsigned d_[MaxWords];
            unsigned d_num = 0;
            ForEach(l_d,i) {
              d_[d_num] = uvtoi32(**i,"Data value number element of a misaligned list.");;
              ++d_num;	    
            }            
            if (!d_num) {
              RError("Empty list found for data values for misaligned real-address list");
            }           
	    
            if (d_num != ra_num) {
              RError("Size of real-address list not equals size of data values of a misaligned list");
            }
	    
            // We assume that if we've gotten a list form, then the size of the
            // instruction is large enough so that some of it spills over into
            // the second word.  Otherwise, we wouldn't have a list form.
            // These sizes are in number of bits.
            //int sz1 = 8*(4 - (ra1 & 0x3));
            //int sz2 = size - sz1;
            
            int sz1 = 8*(4 - (ra_[0] & 0x3));
            ForRange(d_num,i) {
              if (i==0) {
                init_mem_internal(*_cur.node(),GlobalMemName,ra_[0],d_[0],sz1);
              } else {
                int remained = size - 32*(i-1) - sz1;
                int sz2 = remained > 32 ? 32 : remained; 
                init_mem_internal(*_cur.node(),GlobalMemName,ra_[i],d_[i],sz2);
              }    
            }

          } else {
            // The instruction is aligned.  We expect scalars for everything else.
            addr_t addr = uvtoi64(ra,"real address.");
            uint32_t data = get_uint32(cmd,"d","data.");
                        
            init_mem_internal(*_cur.node(),GlobalMemName,addr,data,size);            
          }
          
        }
        else if ( *(cmd._cmd) == "F") {
          // Ignore a fetch-card, since this just represents an aborted fetch.
          ;
        }
        else if ( *(cmd._cmd) == "RI" ) {
          // Ignore re-executed instructions.
          ;
        } else if ( *(cmd._cmd) == "A" ) {
          // Ignore asynchronous events.
          ;
        } else {
          cerr << printLocation(loc) << ":  Warning:  Unknown UVP command:  " << cmd._port << " : " << cmd._cmd << ".\n";
        }
        
      } else if ( *(cmd._port) == "R" ) {

        // Ignore result card stuff, except for memory, which we might log, if the option is set to true.
        if (_log_result_mem && *(cmd._cmd) == "D") {
          addr_t addr = get_uint64(cmd,"ra","address.");
          uint32_t data = get_uint32(cmd,"d","data.");
          init_mem_internal(*_cur.node(),GlobalMemName,addr,data,32,true);
        }
        else if ( *(cmd._cmd) == "RES") {
          UvpResValues &rv = uvp_res_values();
          if ( !rv.empty() ) {
            addr_t ra = get_uint64(cmd,"ra","real address.");
            // We have a result card and we have result-card values, so handle that here.
            if (rv._res_single_value) {
              // Use an unconditional value.
              const UvpResValue &x = rv[0];
              init_mem_internal(*_cur.node(),GlobalMemName,ra,x._value,x._size);
            } else {
              // Use a value based upon the ta.
              uint32_t ta = get_uint32(cmd,"ta","translation-attributes.");
              uint32_t mask = (rv._ta_res_mask) ? rv._ta_res_mask : ((uint32_t)-1);
              auto x = rv.find(ta&mask);
              if (x != rv.end()) {
                init_mem_internal(*_cur.node(),GlobalMemName,ra,x->second._value,x->second._size);              
              } else {
                cerr << printLocation(loc) << ":  Warning:  No appropriate RES value found for ta=0x" << hex << ta << dec << "\n";
              }
            }
          }
          
        }

      } else if ( *(cmd._port) == "M") {

        // Skip messages.

      } else {
        cerr << printLocation(loc) << ":  Warning:  Unknown UVP command:  " << cmd._port << " : " << cmd._cmd << ".\n";
      }

    }
    catch (exception &err) {
      RError(printLocation(loc) << ": " << err.what());
    }
  }

  void UvpReader::handle_parm(const YYLTYPE &loc,unique_ptr<UvpToken> name,unique_ptr<UvpValue> value)
  {
    // Bare parameters are currently ignored.
  }

}

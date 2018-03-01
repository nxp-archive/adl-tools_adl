//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <assert.h>

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"

#include "rnumber/RNumber.h"

#include "ModelInterface.h"
#include "DatReader.h"

using namespace std;
using namespace rnumber;

namespace adl {

  // Add command-line options to the option database.  Note the use of logical &&
  // to ensure that we evaluate all of the additions.
  AnyOption &options();

  static bool dummy = 
    options().setOption  ("dat-map-core-name", "Create a mapping for core names.  Format is --dat-map-core-name=initial:final.  Whenever a CORE\n"
                          "card is encountered, if the name matches <initial>, it will be translated to <final>.\n"
                          "This option may be repeated.");

  //
  // DatReader methods.
  //

  DatReader::DatReader (const std::string& filename,IssNode *root,bool compressed) : 
    Reader(filename,root),
    DatParser(filename,compressed),
    _last_line(0),
    _trace_mode(false),
    _first_test(true)
  {
    if (_root) {
      setRoot(_root);
    }

    setup_core_name_map();
    
    // TODO: set up trace run init
  }

  void DatReader::setRoot(IssNode *root)
  {
    Reader::setRoot(root);

    _cores.clear();
    populateCoreMap("",*_root);
    assert(!_cores.empty());
    _cur_core = root;
    _cur_reg_enums = getRegEnums(root->name());

    rewind();
  }

  // Setup the core name mappings, if any exist.  Each string should be of the
  // form "initial:final".
  void DatReader::setup_core_name_map()
  {	
    const AnyOption::Values &v = options().getValues("dat-map-core-name");
    ForEach(v,i) {
      string s = i->c_str();
      unsigned n = s.find(':');
      if (n == string::npos) {
        RError("Bad core-name translation option:  " << n << ".  Expected 'initial:final'.");
      }
      _core_name_map.insert(make_pair(s.substr(0,n),s.substr(n+1)));
    }
  }

  void DatReader::populateCoreMap(const string &name,IssNode &node)
  {
    string path = (name.empty()) ? node.name() : (name + ":" + node.name());
    if (node.is_core()) {
      auto pit = _cores.insert(make_pair(path,CoreInfo(&node)));
      // Process enums
      const RegInfos &rinfo = node.getRegInfo();
      ForEach(rinfo,ri) {
        CoreInfo::EnumValues  &ev = pit.first->second._reg_enums[ri->_name];
        ForEach(ri->_enums,e) {
          ev.insert(make_pair(e->second,e->first));
        }
      }
    } else {
      _cores.insert(make_pair(path,CoreInfo(&node)));
      ForEach(node.children(),i) {
        populateCoreMap(path,**i);
      }
    }
  }

  DatReader::CoreMap::iterator DatReader::lookupCore(string name)
  {
    if (name[0] == ':') {
      name = name.substr(1);
    }

    // No translation, we can just use the token as-is.
    if (!_core_name_map.empty()) {
      IfFind(x,_core_name_map,name) {
        name = x->second;
      }
    }

    return _cores.find(name);
  }

  IssNode *DatReader::getCore(const string &name)
  {
    auto iter = lookupCore(name);
    if (iter == _cores.end()) {
      if (LaxMode) {
        cerr <<"Warning: Ignoring unknown core '" << name << "'.\n";
        return 0;
      } else {
        RError("Bad core identifier '" << name << "'.");
      }
    } else {
      return iter->second._node;
    }
    
  }

  DatReader::RegEnumsHash *DatReader::getRegEnums(const string &name) 
  {
    auto iter = lookupCore(name);
    if (iter == _cores.end()) {
      if (LaxMode) {
        return 0;
      } else {
        RError("Bad core identifier '" << name << "'.");
      }
    } else {
      return &(iter->second._reg_enums);
    }
  }

  IssCore *DatReader::get_core_ptr()
  {
    if (IssCore *c = dynamic_cast<IssCore*>(_cur_core)) {
      return c;
    } else {
      RError("Node " << _cur_core->name() << " is not a core node.");
    }
  }


  void DatReader::get_cache_data(const char *k,CacheLineData &data,bool erase)
  {
    auto v = find_key(k);
    if (not_found(v)) {
      RError("Could not find cache-data type (key of '" << k << "').");
    }

    const char *b = v->second.data();
    const char *e = b + v->second.size();

    while(b < e) {
      char *err;
      uint64_t d = strtoul(b,&err,0);
      if (b == err) {
        RError("Bad cache data for element " << data.size());
      }
      data.push_back(d);
      b = err + 1;
    }

    cond_erase(v,erase);
  }

  WatchType DatReader::get_watch_type(const char *k)
  {
    auto v = find_key(k);
    if (not_found(v)) {
      RError("Could not find watch-point type (key of '" << k << "').");
    }

    const Token &t = v->second;

    if (t.empty()) {
      RError ("No watchpoint type specified.");
    } else if (t == "read") {
      return wpRead;
    } else if (t == "write") {
      return wpWrite;
    } else if (t == "access") {
      return wpAccess;
    } else {
      RError("Unknown watchpoint type '" << t << "'.");
    }
  }

  pair<const char*,unsigned> DatReader::getline_compressed()
  {
    // Delete the last line that we had read.
    delete _last_line;
    _last_line = 0;
    // Read the next line and store it.
    auto l = DatParser::getline_compressed();
    _last_line = l.first;
    return l;
  }

  bool DatReader::readFile()
  {
    _record = true;
    try {
      parse();
    }
    catch (exception &err) {
      RError(_filename + ":  " + err.what());
    }
    return true;
  }

  // Refer to adl/docs/dat-format.rst for information about the file format.
  bool DatReader::handleCard(int ln,const Token &type)
  {
    if (_trace_mode) 
    {
      // here "ln" actually means "step"
      return handle_trace_card(ln, type);
    }
    else
    {
        if (type == "TEST") {
          // We allow a leading TEST card, but stop once we encounter that.
          if (!_first_test) {
            return false;
          }
          _first_test = false;
          return true;
        }

        _first_test = false;
          
        if ((type == "RESULTS") || (type == "TRACE")) {
          _record = false;
          return true;
        }
        else if (type == "INIT") {
          _record = true;
          return true;
        }
        else if (type == "CORE") {
          string name = get_str("n","core name");
          _cur_core = getCore(name);
          _cur_reg_enums = getRegEnums(name);
          return true;
        } 
        else if (type == "CTX") {
          if (_cur_core) {
            Token name = get_str("n","context name");
            uint32_t cn  = get_uint32("cn","context number");
            _cur_core->setDbgCtx(name.str(),cn);
          }
          return true;
        }
        else if (type == "INIT") {
          // turn _recording back on, we have an interleaved style
          // dat (likely converted from a uvp).
          _record = true;
          return true;
        }
        else if (!_record) {
          return true;
        }
        else if (type == "CTIME" || type == "ICOUNT") {
          // Skip performance info.
          return true;
        }
        else if (type == "NOCTX") {
          // We can ignore this because the model knows internally what is and what is
          // not a context register.
          return true;
        }
        else if (type == "MD") {
          string name = get_str("n","memory name.");
          addr_t addr = get_uint64("ra","address.");
          uint32_t data = get_uint32("d","data.");
          auto s = find_key("s");
          int size = (not_found(s)) ? 32 : atoi32(s->second,"memory initialization byte size value.")*8;
              
          DPrint("m " << name << " 0x" << hex << addr << ":  0x" << data << ", bytes = " << size << "\n");
        
          // Use the current core, if we have one (not skipping an unknown core in
          // lax-mode), otherwise use the root.
          init_mem_internal((_cur_core) ? *_cur_core : *_root,name,addr,data,size);
          return true;
        }

        // If no current core defined (skipping due to lax-mode), 
        if (!_cur_core) return true;

        if (type == "RD") {
          string name   = get_str("n","register name.");
          uint32_t index = get_uint32("i",0);
          RNumber data;
          assert(_cur_reg_enums);
          auto iter = _cur_reg_enums->find(name);
          if (iter == _cur_reg_enums->end()) {
            data   = get_rn("d","register data.");
          } else {
            CoreInfo::EnumValues ev = iter->second;
            string val = get_str("d","register data.");
            auto e = ev.find(val);
            if (e == ev.end()) {
              data = RNumber(val);
            } else {
              data = RNumber(e->second);
            }
          }
          DPrint("r " << name << " " << dec << index << ": 0x" << hex << data << "\n");
          init_reg_internal(*_cur_core,name,index,data);
        } 
        else if (type == "TD") {
          string name = get_str("n","TLB lookup name.",true);
          uint32_t set =  get_uint32("set","TLB lookup set value.",true);
          uint32_t way =  get_uint32("way","TLB lookup way value.",true);
          FieldData ld;
            
          ForEach(_pairs,i) {
            if (is_empty_pair(*i)) {
              continue;
            }
            ld.push_back(FieldPair(i->first.str(),atoi64(i->second,"TLB field value")));
          }

          if (ld.empty()) {
            RError("No TLB lookup key/value pairs were found on this line.");
          }

          init_mmu_internal(*_cur_core,name,set,way,ld);
        }
        else if (type == "CD") {
          FieldData     fields;
          CacheLineData data;

          string name  = get_str("n","cache name.",true);
          uint32_t set   = get_uint32("set","cache set.",true);
          uint32_t way   = get_uint32("way","cache way.",true);
          get_cache_data("d",data,true);

          ForEach(_pairs,i) {
            if (is_empty_pair(*i)) {
              continue;
            }
            try {
              fields.push_back(FieldPair(i->first.str(),atoi64(i->second,"cache field value.")));
            }
            catch (exception &err) {
              RError("Cache field " << i->first << ":  " << err.what());
            }
          }

          init_cache_internal(*_cur_core,name,set,way,fields,data);
        }
        else if (type == "BP") {
          addr_t addr   = get_uint64("ea","address.");
          IssCore *c = get_core_ptr();
          c->setBreakpoint(addr);
        }
        else if (type == "WP") {
          addr_t addr   = get_uint64("ea","address.");
          WatchType wpt = get_watch_type("t");
          IssCore *c = get_core_ptr();
          c->setWatchpoint(addr,wpt);
        }
        else {
          cerr << _filename << ":" << ln << ":  Warning:  Unknown data card type " << type << "\n";
        }

        return true;
    }
  }

  void DatReader::error(int ln,const string &msg)
  {
    cerr << _filename << ":" << ln << ":  " << _cur_core->name() << ":  " << msg << endl;
  }
  
  // Trace reader methods
  
  uint64_t DatReader::instrCount()
  {
    return _step;
  }
  
  bool DatReader::applyNext()
  {
    // use an empty line as the next step signal
    return parse_one_step();
  }

  bool DatReader::handle_trace_card(uint64_t step,const Token &type)
  {
    if (type == "I") {
      uint64_t tic = get_uint64("tic","thread-local instruction count.");
      if (tic != step+1)  //the step (_step in DatParser) is not yet updated at this point
      {
        RError("Step (=" << step << ") is different from tic (=" << tic << ")");
      }
      return true;
    }
    else if (type == "M") {
      string name = get_str("n","memory name.");
      string mem_type = get_str("t","type(read//write//ifetch).");
      addr_t addr = get_uint64("ra","address.");
      uint32_t data = get_uint32("d","data.");  
      auto s = find_key("s");
      int size = (not_found(s)) ? 32 : atoi32(s->second,"memory initialization byte size value.")*8;

      init_mem_internal((_cur_core) ? *_cur_core : *_root,name,addr,data,size);        
      return true;
    }
    else if (type == "INSTR") {
      // if wanna use the other endianness
      return true;
    
    }
    else if (type == "R") {
      string name   = get_str("n","register name.");
      uint32_t index = get_uint32("i",0);
      RNumber data;
      assert(_cur_reg_enums);
      auto iter = _cur_reg_enums->find(name);
      if (iter == _cur_reg_enums->end()) {
        data   = get_rn("d","register data.");
      } else {
        CoreInfo::EnumValues ev = iter->second;
        string val = get_str("d","register data.");
        auto e = ev.find(val);
        if (e == ev.end()) {
          data = RNumber(val);
        } else {
          data = RNumber(e->second);
        }
      }
      init_reg_internal(*_cur_core,name,index,data);
      return true;
    
    }
    else if (type == "D") {
      return true;    
    }
    else if (type == "C") { // cache
      return true;
    }
    else if (type == "T") { // MMU/TLB
      return true;
    }
    else {
      return false;
    }
  } 

    void DatReader::initTraceReading()
  {
    _trace_mode = true;    
    init_cur_to_trace();      
  }
  
}

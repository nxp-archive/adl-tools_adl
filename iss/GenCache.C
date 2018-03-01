//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main implementation file for cache generation code.
//

#include <string>
#include <iostream>
#include <sstream>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "fe/Data.h"
#include "fe/Helpers.h"


#include "helpers/gc_list.h"
#include "helpers/pfxstream.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/StringUtils.h"

#include "AttrsHelper.h"
#include "InstrWalker.h"
#include "ConfigDB.h"
#include "GenCache.h"
#include "GenRegs.h"
#include "GenMems.h"
#include "MiscTypes.h"
#include "GlobalMemoryName.h"

using namespace Opencxx;
using namespace PtreeUtil;
using namespace std;

namespace adl {

  static const char *attr_used = " ATTRIBUTE_USED ";

  gc_string getCacheName(const Cache &cache)
  {
    return cache.name() + "_cache";
  }

  gc_string getCacheOrigName(const Cache &cache)
  {
    return cache.orig_name() + "_cache";
  }

  gc_string getCacheDynGuard(const Cache &cache)
  {
    return getCacheOrigName(cache) + "_dyn_guard";
  }

  gc_string getPerfCacheName(const Cache &cache)
  {
    return cache.name() + "_perf";
  }

  gc_string getFullPerfCacheName(const Cache &cache)
  {
    return cache.name() + "_perf_cache";
  }

  gc_string getCacheDataType(const Cache &cache)
  {
    return getCacheName(cache) + "_t";
  }

  gc_string getCacheArrayName(const Cache &cache)
  {
    return getCacheName(cache) + "._array";
  }

  gc_string getCacheWayName(const Cache &cache)
  {
    return getCacheName(cache) + "_line_obj";
  }

  gc_string getCacheSetName(const Cache &cache)
  {
    return getCacheName(cache) + "_set_obj";
  }

  gc_string makeCacheParmName(const gc_string &s)
  {
    return s + "_arg";
  }

  gc_string getCacheLineSize(const Cache &cache)
  {
    return getCacheOrigName(cache) + "_linesize";
  }

  gc_string getCacheLineMask(const Cache &cache)
  {
    return getCacheOrigName(cache) + "_linemask";
  }

  gc_string getCacheLineShift(const Cache &cache)
  {
    return getCacheOrigName(cache) + "_lineshift";
  }

  gc_string getCacheGetLineSize(const Cache &cache)
  {
    return getCacheName(cache) + "_get_linesize";
  }

  gc_string getCacheGetLineMask(const Cache &cache)
  {
    return getCacheName(cache) + "_get_linemask";
  }

  gc_string getCacheGetLineShift(const Cache &cache)
  {
    return getCacheName(cache) + "_get_lineshift";
  }

  gc_string getCacheNumSets(const Cache &cache)
  {
    return getCacheOrigName(cache) + "_num_sets";
  }

  gc_string getCacheNumWays(const Cache &cache)
  {
    return getCacheOrigName(cache) + "_num_ways";
  }

  gc_string getCacheGetWays(const Cache &cache)
  {
    return getCacheName(cache) + "_get_ways";
  }

  gc_string getCacheGetSets(const Cache &cache)
  {
    return getCacheName(cache) + "_get_sets";
  }

  gc_string getCacheType(const Cache &cache)
  {
    gc_string s = cache.typestr() + "Cache";
    s[0] = toupper(s[0]);
    return s;
  }

  class GenCacheBase {
  public:
    GenCacheBase (opfxstream &o,ClassData &cd,const ConfigDB &config,const CoreConfig *coreconfig) :
      _o(o), 
      _cd(cd),
      _config(config),
      _coreconfig(coreconfig),
      _gen_mode(gmNormal)
    {};

  protected:
    opfxstream       &_o;

    ClassData        &_cd;
    const ConfigDB   &_config;
    const CoreConfig *_coreconfig;

    enum GenMode { 
      gmNormal,      // Normal caches (stores tags and data).
      gmTagsOnly,    // Normal caches, but stores only tags.  Data comes from memory.
      gmPerf         // Performance caches for transactional ISSs.  No data is
                     // stored and they're skipped for logging operations, etc.
    };
    GenMode         _gen_mode;

  public:

    bool time_tagged() const
    {
      return _config.time_tagged();
    }

    bool log_td_commit() const
    {
      return _config.td_api() && _config.log_td_commit();
    }

    bool transactional_iss() const
    {
      return _config.iss_type() == tTransactional;
    }

    bool normal_iss() const
    {
      return _config.iss_type() == tNormal;
    }

    bool logging_mem() const
    {
      return _config.mem_if() == mLogging;
    }

    bool perf_mode() const
    {
      return _gen_mode == gmPerf;
    }

    bool tags_only_caches() const
    {
      return _config.tags_only_caches();
    }

    // The idea is that we track what's in the cache, but don't actually store
    // any data.  So, it's similar to the skip-caches concept, but we're
    // tracking tags ourself.  This is useful for multi-core modeling- you can
    // get cache traffic but you don't have to worry about a coherency protocol.
    bool tags_only_mode() const
    {
      return _gen_mode == gmTagsOnly;
    }

    bool skip_caches_in_exec() const
    {
      return _config.skip_caches();
    }

    // The idea is that we only have performance caches if we're in the
    // logging-memory-mode (the ISS does its own thing and a performance model
    // manages caches), but we're also not skipping caches (the ISS actually
    // goes through the caches for its own behavior).  This corresponds to
    // uADL's --iss-mem-mode.
    bool have_perf_caches() const
    {
      return logging_mem() && !skip_caches_in_exec();
    }

    // This implies that cache management will be handled externally.  This is
    // true for a plain transactional ISS not in iss-mem-mode (logging-memory
    // mode) or a performance cache
    bool trans_cache() const
    {
      return (transactional_iss() && !logging_mem()) || skip_caches_in_exec() || perf_mode();
    }

    bool extern_mem() 
    {
      return _config.extern_mem();
    }

    bool extern_dmi()
    {
      return _config.extern_dmi();
    }

    bool graphite_support() const
    {
      return _config.graphite_support();
    }

    // For our purposes here, JIT and decode-cache support are equivalent in
    // terms of needed support functions.
    bool jit_support() const
    {
      return _config.jit_support() || _config.dc_support();
    }

    bool is_dynamic(const Cache &cache) const 
    {
      return (_coreconfig && _coreconfig->dyn_caches().count(cache.name()));
    }

    bool multi_compile() const
    {
      return _config.num_compile_jobs() > 0;
    }

    gc_string headerOnly() const
    {
      return adl::headerOnly(multi_compile());
    }   

    void addSharedParm(const gc_string &type,const gc_string &name)
    {
      gc_string parm = makeCacheParmName(name);

      _cd._member_vars.push_back(type+" &"+name);
      _cd._constr_args.push_back(type+" &"+parm);
      _cd._constr_inits.push_back(name+"("+parm+")");
    }

    // Generate data structures for a cache.
    void genCacheDataStructure(const Cache &cache,bool shared,bool is_dyn,bool has_data,StrList *child_args)
    {
      gc_string ctype = getCacheDataType(cache);
      gc_string cname = getCacheName(cache);

      // A cache is an array of objects who have fields and a byte array.
      // Every cache has a valid flag in addition to possibly other fields.
      gc_string lname = getCacheWayName(cache);
      gc_string sname = getCacheSetName(cache);

      bool has_rpl = cache.replace();

      if (!shared) {
        if (child_args) {
          child_args->push_back(cname);
        }

        _o << "struct " << lname << "{\n"
           << "addr_t   tag;\n"    // Address tag.
           << "bool     valid;\n"  // If the line contains real data.
           << "bool     dirty;\n"  // If the line has been modified.
           << "bool     locked;\n" // Whether it's locked or not.
           << "bool     pad;\n";   // Extra padding so that the data array will be word aligned.

        if (trans_cache()) {
          _o << "addr_t   old_tag;\n" // old address tag.
             << "u_int    busy;\n";   // Transactional ISSs record whether a
                                      // line is currently involved in a
                                      // transaction.
        }

        // If a custom replacement algorithm exists, then we don't print out LRU fields.
        if (!has_rpl) {
          _o << "int      _n, _p;\n"; // Next/prev pointers for LRU linked list.
        }

        // List way fields here.
        ForEach(cache.way_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << genFieldType(cf) << " " << cf.name() << ";\n";
        }

        if (has_data) {
          if (is_dyn) {
            _o << "byte_t   *_data;\n"
               << lname << "() : _data(0) {}\n"
               << "~" << lname << "() { delete [] _data; };\n";
          } else {
            // Note: No constructor is created b/c the set object will call reset on
            // its ways.
            _o << "byte_t    _data[" << dec << cache.linesize() << "];\n\n"
               << "unsigned linesize() const { return " << cache.linesize() << ";};\n\n";
          }
        }

        if (is_dyn && !perf_mode()) {
          gc_string linesize = getCacheLineSize(cache);
          gc_string linesize_v = uitogs(cache.linesize());
          _cd._member_vars.push_back("unsigned "+linesize);
          _cd._constr_inits.push_back(linesize+"(" + linesize_v + ")");

          gc_string linemask = getCacheLineMask(cache);
          gc_string linemask_v = uitogs(cache.linesize()-1);
          _cd._member_vars.push_back("unsigned "+linemask);
          _cd._constr_inits.push_back(linemask+"(" + linemask_v + ")");

          gc_string lineshift = getCacheLineShift(cache);
          gc_string lineshift_v = uitogs(trailing_zeros(cache.linesize()));
          _cd._member_vars.push_back("unsigned "+lineshift);
          _cd._constr_inits.push_back(lineshift+"(" + lineshift_v + ")");

          if (child_args) {
            child_args->push_back(linesize);
            child_args->push_back(linemask);
            child_args->push_back(lineshift);
          }
        }

        _o << "void reset(bool realloc,unsigned linesize) {\n";
        if (is_dyn && has_data) {
          _o << "if (realloc) {\n"
             << "delete []_data;\n"
             << "_data = new byte_t[linesize];\n"
             << "}\n";
        }
        _o << "valid = false;\n"
           << "dirty = false;\n"
           << "locked = false;\n";
        if (trans_cache()) {
          _o << "old_tag = 0;\n"
             << "busy = 0;\n";
        }
        ForEach(cache.way_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << cf.name() << " = " << genFieldInit(cf) << ";\n";
        }
        if (has_data) {
          _o << "memset(_data,0,linesize);\n";
        }
        _o << "};\n"
           << "void init(unsigned linesize,const FieldData &fd,const CacheLineData &cd) {\n"
           << "tag     = getLookupField(fd, \"ra\",     0);\n";
        if (trans_cache()) {
          _o << "old_tag = 0;\n";
        }
        _o << "valid   = getLookupField(fd, \"valid\",  0);\n"
           << "dirty   = getLookupField(fd, \"dirty\",  0);\n"
           << "locked  = getLookupField(fd, \"locked\", 0);\n";
        ForEach(cache.way_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << cf.name() << " = getLookupField(fd, \"" << cf.name() << "\"," << genFieldInit(cf) << ");\n";
        }
        if (has_data) {
          _o << "for (unsigned i = 0; i != linesize; ++i) {\n"
             << "if (i == cd.size()) break;\n"
             << "byte_write32(_data,i*4,cd[i]);\n"
             << "}\n";
        }
        _o << "}\n"
           << "void show(FieldData &fd) const {\n"
           << "fd.push_back(FieldPair(\"valid\",valid));\n"
           << "fd.push_back(FieldPair(\"dirty\",dirty));\n"
           << "fd.push_back(FieldPair(\"locked\",locked));\n";
        ForEach(cache.way_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << "fd.push_back(FieldPair(\"" << cf.name() << "\"," << genFieldIntGet(cf) << "));\n";
        }
        _o << "}\n"
           << "};\n\n";

        int ways = cache.ways();
        // Each set is made up of a series of cache lines.
        _o << "struct " << sname << " {\n";
        if (!has_rpl) {
          _o << "unsigned _h, _t;\n";
        }
        // List set fields here.
        ForEach(cache.set_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << genFieldType(cf) << " " << cf.name() << ";\n";
        }
        _o << "unsigned _lock_count;\n";

        if (is_dyn) {
          _o << lname << " *_ways;\n\n"
             << sname << "() : _ways(0) {};\n"
             << "~" << sname << "() { delete [] _ways; }\n";
        } else {
          _o << lname << " _ways[" << dec << ways << "];\n\n"
             << sname << "() {};\n";
        }

        _o << "void reset_lru(int num_ways) {\n";
        if (!has_rpl) {
          _o << "for (int i = 0; i != num_ways; ++i) {\n"
             << "_ways[i]._n = ((i == (num_ways-1)) ? -1 : i+1);\n"
             << "_ways[i]._p = ((i == 0)        ? -1 : i-1);\n"
             << "}\n"
             << "_h = 0;\n"
             << "_t = num_ways-1;\n";
        }
        _o << "}\n"
           << "void reset(bool realloc,int num_ways,unsigned linesize) {\n";
        if (is_dyn) {
          _o << "if (realloc) {\n"
             << "delete [] _ways;\n"
             << "_ways = new " << lname << "[num_ways];\n"
             << "}\n";
        }
        _o << "reset_lru(num_ways);\n"
           << "for (int i = 0; i != num_ways; ++i) {\n"
           << "_ways[i].reset(realloc,linesize);\n"
           << "}\n";
        ForEach(cache.set_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << cf.name() << " = " << genFieldInit(cf) << ";\n";
        }
        _o << "_lock_count = 0;\n"
           << "};\n"
           << "void init(const FieldData &fd) {\n";
        ForEach(cache.set_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << cf.name() << " = getLookupField(fd, \"" << cf.name() << "\"," << genFieldInit(cf) << ");\n";
        }
        _o << "}\n"
           << "void show(FieldData &fd) const {\n";
        ForEach(cache.set_fields(),i) {
          const CacheField &cf = *(i->second);
          _o << "fd.push_back(FieldPair(\"" << cf.name() << "\"," << genFieldIntGet(cf) << "));\n";
        }
        _o << "}\n"
           << lname << " &operator[](unsigned i) { return _ways[i]; };\n"
           << "const " << lname << " &operator[](unsigned i) const { return _ways[i]; };\n"
           << "bool fully_locked() const { return _lock_count == " << ways << "; }\n"
           << "bool fully_unlocked() const { return _lock_count == 0; }\n"
           << "};\n\n";

        _o << "struct " << ctype << " {\n";

        // If we allow for multiple numbers of sets/ways, add member variables to store the current values.
        if ((cache.multi_sets() || is_dyn) && !perf_mode()) {
          gc_string num_sets = getCacheNumSets(cache);
          gc_string num_sets_v = uitogs(cache.sets());
          _cd._member_vars.push_back("unsigned "+num_sets);
          _cd._constr_inits.push_back(num_sets+"(" + num_sets_v + ")");

          if (child_args) {
            child_args->push_back(num_sets);
          }
        }

        if ((cache.multi_ways() || is_dyn) && !perf_mode()) {
          gc_string num_ways = getCacheNumWays(cache);
          gc_string num_ways_v = uitogs(cache.ways());
          _cd._member_vars.push_back("unsigned "+num_ways);
          _cd._constr_inits.push_back(num_ways+"(" + num_ways_v + ")");

          if (child_args) {
            child_args->push_back(num_ways);
          }
        }

        // Create the data structure array.  Note: Right now, we always have a
        // two-dimensional array, even if we have a fully-associative cache.  We
        // might want to fold out that extra dimension (I don't know if the compiler
        // is smart-enough to optimize that out).
        // Also note that the size of the array is the maximum possible size of the
        // set/way combinations.
        _o << sname << " *_array;\n\n";
        if (is_dyn) {
          _o << ctype << "() : _array(0) {}\n";
        } else {
          _o << ctype << "() : _array(new " << sname << "[" << dec << cache.sets() << "]) {}\n";
        }
        _o << "~" << ctype << "() { delete [] _array; };\n"

           << "void reset(bool realloc,int num_sets,int num_ways,unsigned linesize) {\n";
        if (is_dyn) {
          _o << "if (realloc) {\n"
             << "delete [] _array;\n"
             << "_array = new " << sname << "[num_sets];\n"
             << "}\n";
        }
        _o << "for (int i = 0; i != num_sets; ++i) {\n"
           << "_array[i].reset(realloc,num_ways,linesize);\n"
           << "}\n"
           << "}\n"
           << "};\n\n";

        _cd._member_vars.push_back(ctype+" "+cname);
      } else {
        addSharedParm(ctype,cname);

        if (is_dyn && !perf_mode()) {
          addSharedParm("unsigned",getCacheLineSize(cache));
          addSharedParm("unsigned",getCacheLineMask(cache));
          addSharedParm("unsigned",getCacheLineShift(cache));
          addSharedParm("unsigned",getCacheNumSets(cache));
          addSharedParm("unsigned",getCacheNumWays(cache));
        }
      }
    }

    void writeGetSetLogic(gc_string lineshift,gc_string num_sets)
    {
      _o << "return (addr >> " << lineshift << ") & (" << num_sets << "-1);\n";
    }

    // Create cache assist functions.
    void genCacheAccessFuncs(const Cache &cache,bool shared,bool is_dyn,bool has_data)
    {
      gc_string cname      = getCacheName(cache);
      gc_string array      = getCacheArrayName(cache);
      gc_string entry      = array + "[set][way]";
      gc_string set_entry  = array + "[set]";
      gc_string ways       = uitogs(cache.ways());
      gc_string sets       = uitogs(cache.sets());
      bool has_rpl      = cache.replace();
      unsigned linesize = cache.linesize();
      unsigned linemask = linesize-1;
      unsigned lineshift= trailing_zeros(linesize);

      _o << "unsigned " << getCacheGetWays(cache) << "() const {\n";
      if (!cache.multi_ways() && !is_dyn) {
        _o << "return " << cache.ways() << ";\n";
      } else {
        _o << "return " << getCacheNumWays(cache) << ";\n";
      }
      _o << "}\n\n";

      _o << "unsigned " << getCacheGetSets(cache) << "() const {\n";
      if (!cache.multi_sets() && !is_dyn) {
        _o << "return " << cache.sets() << ";\n";
      } else {
        _o << "return " << getCacheNumSets(cache) << ";\n";
      }
      _o << "}\n\n";

      if (is_dyn) {
        ways = getCacheGetWays(cache) + "()";
        sets = getCacheGetSets(cache) + "()";        
      }

      if (is_dyn) {
        _o << "unsigned " << getCacheGetLineSize(cache) << "() const {\n"
           << "return " << getCacheLineSize(cache) << ";\n"
           << "}\n"
          
           << "addr_t " << getCacheGetLineMask(cache) << "() const {\n"
           << "return " << getCacheLineMask(cache) << ";\n"
           << "}\n"
          
           << "unsigned " << getCacheGetLineShift(cache) << "() const {\n"
           << "return " << getCacheLineShift(cache) << ";\n"
           << "}\n";
      } else {
        _o << "unsigned " << getCacheGetLineSize(cache) << "() const {\n"
           << "return " << linesize << ";\n"
           << "}\n"
          
           << "addr_t " << getCacheGetLineMask(cache) << "() const {\n"
           << "return 0x" << hex << linemask << dec << ";\n"
           << "}\n"
          
           << "unsigned " << getCacheGetLineShift(cache) << "() const {\n"
           << "return " << lineshift << ";\n"
           << "}\n";
      }

      _o << "unsigned " << cname << "_get_offset(addr_t addr) const {\n"
         << "return (addr & " << getCacheGetLineMask(cache) << "());\n"
         << "}\n"
      
         << "unsigned " << cname << "_get_set(unsigned addr) const {\n";
      if (is_dyn) {
        writeGetSetLogic(getCacheGetLineShift(cache)+"()",getCacheGetSets(cache)+"()");
      } else if (cache.multi_sets()) {
        writeGetSetLogic(uitogs(lineshift),getCacheGetSets(cache)+"()");
      } else {
        writeGetSetLogic(uitogs(lineshift),uitogs(cache.sets()));
      }
      _o << "}\n\n"

         << "addr_t " << cname << "_get_tag(unsigned set,unsigned way) const {\n"
         << "return " << entry << ".tag;\n"
         << "}\n\n";

      if (trans_cache()) {
        _o << "addr_t " << cname << "_get_old_tag(unsigned set,unsigned way) const {\n"
           << "return " << entry << ".old_tag;\n"
           << "}\n\n";
      }

      // This function is virtual so that a derived class can catch the reset
      // signal and reallocate any data structures it might have.
      _o << "virtual void " << cname << "_reset(bool realloc) {\n"
         << cname << ".reset(realloc," << getCacheGetSets(cache) << "()," << getCacheGetWays(cache) << "()," << getCacheGetLineSize(cache) << "());\n"
         << "}\n"
         << "\n";
      if (!has_rpl) {
        _o << "void " << cname << "_reset_lru() {\n"
           << "int num_ways = " << getCacheGetWays(cache) << "();\n"
           << "for (unsigned i = 0; i != " << sets << "; ++i) {\n"
           << array << "[i].reset_lru(num_ways);\n"
           << "}\n"
           << "}\n"
           << "\n";
      }
      _o << "void " << cname << "_set(unsigned set,unsigned way,const FieldData &fd,const CacheLineData &cd) {\n"
         << "if (set >= " << sets << ") {\n"
         << "throw std::runtime_error(\"Bad set specified.\");\n"
         << "}\n"
         << "if (way >= " << ways << ") {\n"
         << "throw std::runtime_error(\"Bad way specified.\");\n"
         << "}\n"
         << entry << ".init(" << getCacheGetLineSize(cache) << "(),fd,cd);\n"
         << set_entry << ".init(fd);\n"
         << "}\n"
         << "\n";

      if (!perf_mode()) {
        _o << "void " << cname << "_show(ReportBase &rb) const {\n"
           << "FieldData fd;\n"
           << "for (unsigned set = 0; set != " << sets << "; ++set) {\n"
           << "for (unsigned way = 0; way != " << ways << "; ++way) {\n"
           << "fd.clear();\n"
           << "addr_t addr = (" << entry << ".valid) ? " << entry << ".tag : 0;\n"
           << set_entry << ".show(fd);\n"
           << entry << ".show(fd);\n"
           << "rb.report_cache(this,\"" << cache.name() << "\"," << getCacheType(cache) << "," << cache.level() << ",addr"
           << ",set,way," << entry << ".valid,fd," << shared << ",";
        if (has_data) {
          _o << entry << "._data,";
        } else {
          _o << "0,";
        }
        _o << getCacheGetLineSize(cache) << "());\n"
           << "}\n"
           << "}\n"
           << "}\n"
           << "\n"
           << "static CacheInfo " << cname << "_info(bool shared) {\n"
           << "CacheInfo info;\n"
           << "info._name = \"" << cache.name() << "\";\n"
           << "info._shared = shared;\n"
           << "info._type = " << cache.type_enum_str() << ";\n"
           << "info._level = " << cache.level() << ";\n"
           << "info._size = " << cache.size() << ";\n"
           << "info._linesize = " << cache.linesize() << ";\n";
        ForEach(cache.sets_list(),i) {
          _o << "info._sets.push_back(" << *i << ");\n";
        }
        ForEach(cache.ways_list(),i) {
          _o << "info._ways.push_back(" << *i << ");\n";
        }
        const AttrList &attrs = cache.attrs();
        ForEach(attrs,i) {
          AttrsHelper ah(&i->second);
          ah.gen_var(_o);
          _o << "info._attrs.push_back(std::make_pair(" 
             << i->first << "," << ah.gen_constr() << "));\n";
        }
        _o << "info.finalize();\n"
           << "return info;\n"
           << "}\n\n";
      }
    }

    void genCacheHelpers(const CacheList &caches)
    {
      // Generate a general reset function.
      _o << "void resetCaches(bool realloc) {\n";
      ForEach(caches,i) {
        bool shared = i->_shared;
        if (!shared) {
          _o << getCacheName(*(i->_cache)) << "_reset(realloc);\n";
        }
      }
      _o << "}\n\n";
      _cd._reset_stmts.push_back("resetCaches(init_reset)");

      // Generate the setting function.
      bool first = true;
      _o << "void setCaches(const std::string &name,unsigned set,unsigned way,const FieldData &fd,const CacheLineData &cd) {\n";
      ForEach(caches,i) {
        const Cache &cache = *(i->_cache);
        if (first) {
          _o << "if";
        } else {
          _o << "else if";
        }
        first = false;
        _o << "(name == \"" << cache.name() << "\") {\n"
           << getCacheName(cache) << "_set(set,way,fd,cd);\n"
           << "} ";
      }
      _o << "else {\n"
         << "throw std::runtime_error(\"A cache of this type does not exist at this level.\");\n"
         << "};\n"
         << "}\n\n";

      // Generate the reporting function.
      _o << "void showCaches(ReportBase &rb) const {\n";
      ForEach(caches,i) {
        const Cache &cache = *(i->_cache);
        if (!i->_perf) {
          _o << getCacheName(cache) << "_show(rb);\n";
        }
      }
      _o << "}\n\n";

      // Generate the introspection function.
      _o << "static CacheInfos initCacheInfo() {\n"
         << "CacheInfos info;\n";
      ForEach(caches,i) {
        const Cache &cache = *(i->_cache);
        bool shared = i->_shared;
        if (!i->_perf) {
          _o << "info.push_back(" << getCacheName(cache) << "_info(" << shared << "));\n";
        }
      }
      _o << "return info;\n"
         << "}\n\n"
         << "static const CacheInfos &baseCacheInfo() {\n"
         << "static CacheInfos info = initCacheInfo();\n"
         << "return info;\n"
         << "}\n\n"
         << "const CacheInfos &getCacheInfo() const {\n"
         << "return baseCacheInfo();\n"
         << "}\n\n";
    }
    
    // Generate functions for setting/displaying caches.
    void generateCacheDS(const CacheList &caches,StrList *child_args)
    {
      ForEach(caches,i) {
        const Cache &cache = *(i->_cache);
        bool shared   = i->_shared;
        bool is_dyn   = i->_dyn;
        bool has_data = i->_data;
        
        if (i->_perf) {
          _gen_mode = gmPerf;
        }
        genCacheDataStructure(cache,shared,is_dyn,has_data,child_args);
        genCacheAccessFuncs(cache,shared,is_dyn,has_data);

        _gen_mode = gmNormal;
      }
      if (!caches.empty()) {
        genCacheHelpers(caches);
      }
    }

    // For a transactional ISS, we generate a function to store various cache
    // state info.  The object itself is static, which is a bit of a hack, but
    // is needed for uADL currently, so that it can handle these objects in a
    // non-templated fashion.
    void generateCacheStatusSupport(const CacheList &caches,CacheType ctype)
    {
      string name = (ctype == InstrCache) ? "instr" : "data";

      if (!normal_iss()) {
        _o << "void update_" << name << "_cache_status(CacheStatus &c,CacheAccess ca,addr_t addr) {\n"
           << "c._valid = true;\n";
        ForEach(caches,i) {
          // We ignore performance caches, since they're equivalent to the functional cache.
          if (i->_perf) continue;

          const Cache &cache = *(i->_cache);
          if (cache.type() == ctype || cache.type() == UnifiedCache) {
            gc_string cname = getCacheName(cache);
            if (cache.level() >= MaxCacheStatusLevels) {
              RError("We do not support " << cache.level() << " level caches in the transactional ISS right now.");
            }
            _o << "c._enabled[" << cache.level()-1 << "] = " << cname << "_enabled(ca,addr);\n"
               << "c._miss_allocate[" << cache.level()-1 << "] = ";
            if (cache.miss_enable()) {
              _o << cname << "_allocate_pred(ca,addr," << cname << "_get_set(addr));\n";
            } else {
              _o << "true;\n";
            }
          }
        }
        _o << "}\n\n";
          
      }
    }

    CacheList getCacheGenList(const System &sys,const Resources &shared)
    {
      // Generate shared caches.
      CacheList caches;

      gc_map<gc_string,bool> is_dyn_map;

      // Caches have data by default, unless the configuration specifies that
      // we're only tracking tags.
      bool has_data = !tags_only_caches();

      ForEach(sys.shared_caches(),i) {
        const Cache *cache = sys.get_cache(i->first);
        const CoreConfig *first = _config.getFirstCoreConfig();
        // Look at the first core's config for whether or not we support dynamic
        // cache parameters.
        bool is_dyn = ((first) ? first->dyn_caches().count(cache->name()) : false);
        is_dyn_map[cache->name()] = is_dyn;

        caches.push_back(CacheGen(cache,shared._caches.count(i->first),false,is_dyn,has_data));
      }

      if (have_perf_caches()) {
        ForEach(sys.shared_caches(),i) {
          const Cache *cache = sys.get_cache(i->first);
          Cache *pcache = new Cache(*cache);

          bool is_dyn = is_dyn_map[i->first];

          pcache->setName(getPerfCacheName(*cache));
          caches.push_back(CacheGen(pcache,shared._caches.count(i->first),
                                    true,is_dyn,false));
        }
      }

      return caches;
    }

  };

  class GenCache : public GenCacheBase {
  public:
    GenCache(opfxstream &o,
             const Core &core,InstrWalker &walker,
             Watches &watches,const gc_string &filename,
             ClassData &cd,const ConfigDB &config,const CoreConfig &coreconfig) :
      GenCacheBase(o,cd,config,&coreconfig),
      _core(core), 
      _walker(walker),
      _watches(watches),
      _filename(filename)
    {};

  private:

    const Core       &_core;
    InstrWalker      &_walker;
    Watches          &_watches;
    const gc_string     &_filename;

  public:

    bool dmi_cache_support() const
    {
      assert(_coreconfig);
      return _coreconfig->dmi_cache_support();
    }

    IntOrFunc cache_miss_latency(const Cache &cname) const
    {
      assert(_coreconfig);

      IfFind(x,_coreconfig->cache_miss_latency(),cname.name()) {
        return x->second;
      }

      return 0;
    }

    bool has_cache_miss_latencies() const
    {
      assert(_coreconfig);

      return !_coreconfig->cache_miss_latency().empty();
    }

    Ptree *funcBody(Environment *e,Ptree *f)
    {
      return adl::funcBody(_walker,e,f);
    }

    void genFunc(const gc_string &fname,Environment *env,Ptree *func)
    {
      adl::genFunc(_o,_config,_walker,_filename,fname,env,func);
    }

    void genWatch(Environment *env,Ptree *watch)
    {
      adl::genWatch(_o,_watches,_walker,env,watch);
    }    

    // Write cache logging functionality.
    void writeCacheLogCode(const Cache &cache,const gc_string &action,const gc_string &access,const gc_string &set,
                           const gc_string &way,const gc_string &addr)

    {
      if (_config.trace_mode() && !perf_mode()) {
        _o << "if (Tracing && adl_unlikely(_trace_mode & ttCacheAction)) {\n"
           << "_logger->log_cache_action(\"" << cache.name() << "\"," << action << "," << access << ","
           << cache.level() << "," << set << "," << way << "," << hex << getCacheGetLineMask(cache) << dec << "()," << addr << ");\n"
           << "}\n";
      }
    }

    // Write a single constant-read accessor function.
    void writeCacheLogReadFunc(const Cache &cache,const gc_string &nname,const gc_string &narg0, unsigned type)
    {
      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);

      unsigned dtype = (type == 0) ? 8 : type;

      _o << "uint" << dtype << "_t " << cname << "_log_read" << type << "(bool force_enable,CacheAccess ca,addr_t addr) {\n";

      if (tags_only_mode()) {
        // In tags-only mode, for logging purposes we just read the next level.
        // We don't need to find it in the cache b/c this is a backdoor access.
        _o << "return " << nname << "read" << type << "(" << narg0 << " addr);\n";
      } else {
        _o << "if (!force_enable && !" << cname << "_enabled(ca,addr)) {\n"
           << "return " << nname << "read" << type << "(" << narg0 << " addr);\n"
           << "} else {\n"
           << "int set,way;\n"
           << "if (" << cname << "_find(ca,set,way,addr)) {\n";
        if (type == 0) {
          _o << "return 0;\n";
        } else {
          _o << "return byte_read" << type << "(" << array << "[set][way]._data,((unsigned)addr) & " << getCacheGetLineMask(cache) << "());\n";
        }
        _o << "} else {\n"
           << "return " << nname << "read" << type << "(" << narg0 << " addr);\n"
           << "}\n"
           << "}\n";
      }
      _o << "}\n\n";
    }

    // Writes a logging-only read-page function for a cache.
    void writeCacheLogReadPage(const Cache &cache,const gc_string &nname,const gc_string &narg0,bool has_next_cache)
    {
      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);
      gc_string sfx    = (has_next_cache) ? "_full" : "";

      _o << "void " << cname << "_log_readpage_full(bool force_enable,CacheAccess ca,byte_t *s,size_t n,addr_t addr,addr_t crit_addr) {\n";

      if (tags_only_mode()) {
        // In tags-only mode, for logging purposes we just read the next level.
        // We don't need to find it in the cache b/c this is a backdoor access.
        _o << "return " << nname << "readpage" << sfx << "(" << narg0 << "s,n,addr,crit_addr);\n";
      } else {
        _o << "if (!force_enable && !" << cname << "_enabled(ca,crit_addr)) {\n"
           << "return " << nname << "readpage" << sfx << "(" << narg0 << "s,n,addr,crit_addr);\n"
           << "} else {\n"
           << "int set,way;\n"
           << "if (" << cname << "_find(ca,set,way,crit_addr)) {\n"
           << "return byte_pagecopy(s,0," << array << "[set][way]._data,((unsigned)addr) & " << getCacheGetLineMask(cache) << "(),n);\n"
           << "} else {\n"
           << "return " << nname << "readpage" << sfx << "(" << narg0 << "s,n,addr,crit_addr);\n"
           << "}\n"
           << "}\n";
      }
      _o << "}\n\n";
    }

    // Write a single constant-write accessor function.  This pokes memory into
    // a cache if the address is valid, then continues to the next level of the
    // hierarchy.
    void writeCacheLogWriteFunc(const Cache &cache,const gc_string &nname, const gc_string &narg0, unsigned vtype,bool has_nb)
    {
      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);

      unsigned dtype = (vtype == 0) ? 8 : vtype;

      _o << "void " << cname << "_log_write" << vtype << "(bool force_enable,addr_t addr, uint" << dtype << "_t v";
      if (has_nb) {
        _o << ", unsigned nb";
      }
      _o << ") {\n";
      if (tags_only_mode()) {
        _o << nname << "write" << vtype << "(" << narg0 << "addr,v" << ( (has_nb) ? ",nb" : "") << ");\n";
      } else {
        _o << "if (force_enable || " << cname << "_enabled(CacheWrite,addr)) {\n"
           << "int set,way;\n"
           << "if (" << cname << "_find(CacheWrite,set,way,addr)) {\n";
        if (vtype != 0) {
          _o << "byte_write" << vtype << "(" << array << "[set][way]._data,((unsigned)addr) & "  << getCacheGetLineMask(cache) << "(),v";
          if (has_nb) {
            _o << ",nb";
          }
          _o << ");\n";
        }
        _o << "}\n"
           << "}\n"
           << nname << "write" << vtype << "(" << narg0 << "addr,v" << ( (has_nb) ? ",nb" : "") << ");\n";
      }
      _o << "}\n\n";
    }

    // If the next level is memory, then update internal DMI on a miss.  Don't
    // do this is we have external DMI, as it will get updated via the
    // external method.
    void getCacheNextReadStr(ostream &o,bool dmi,const gc_string &name,const gc_string &arg0,
                             const gc_string &un,unsigned type,unsigned dtype,bool n_is_mem)
    {
      if (dmi) {
        o << "uint" << dtype << "_t x = " << name << "read" << type << "(" << arg0 << "addr);\n"
               << un << "(addr,lastRealAddr()," << name << "last_page(lastRealAddr() & DmiCache::CompMask));\n"
               << "return x;\n";
      } else {
        o << "return " << name << "read" << type << "(" << arg0 << "addr);\n";
      }   
    }

    // Write a single cache read accessor function.  The basic logic is this:
    // If the cache is enabled, perform the read.  If we have an allocate
    // predicate, then the access function is conditional: false => read from
    // next level, true => read from cache.
    void writeCacheReadFunc(const Cache &cache,const gc_string &nname,const gc_string &mname,
                            const gc_string &narg0,const gc_string &marg0,bool n_is_mem,unsigned type)
    {
      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);

      bool dmi = dmi_cache_support() && &cache == _core.get_first_cache(DataCache);

      bool n_dmi  = (n_is_mem && dmi_cache_support() && !extern_dmi());
      bool to_dmi = (tags_only_caches() && dmi_cache_support() && !extern_dmi());

      unsigned dtype = (type == 0) ? 8 : type;
      gc_string un = getDmiCacheUpdateName(GlobalMemoryName,"read");

      gc_ostringstream n_read, to_read;
      getCacheNextReadStr(n_read,n_dmi,nname,narg0,un,type,dtype,n_is_mem);
      getCacheNextReadStr(to_read,to_dmi,mname,marg0,un,type,dtype,true);

      _o << "uint" << dtype  << "_t " << cname << "_read" << type << "(CacheAccess ca," << ((dmi) ? "addr_t ea," : "") << "addr_t addr) " << attr_used << "\n"
         << "{\n"
         << "if (!" << cname << "_enabled(ca,addr)) {\n"
         << n_read.str()
         << "} else {\n"
         << "int set,way;\n"
         << "bool replace;\n";
      if (dmi) {
        _o << "_lastEffectiveAddr = ea;\n";
      }

      // If we don't have a mis-enable, then the access function will always
      // load in the required data.  If we do have a miss-enable function, then
      // we might not load the data, e.g. an allocate-on-write-only cache.  If
      // we're in tags-only mode, then we do whatever, but we always read from
      // memory, since we don't store any data.
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "if (" << cname << "_access<true>(ca,set,way,addr,replace)) {\n";
      } else {
        _o << cname << "_access<true>(ca,set,way,addr,replace);\n";
      }

      if (tags_only_mode()) {
        // In tags-only mode, read from the next level (since we don't have any
        // data) after doing any cache access stuff.
        _o << to_read.str();
      } else {
        if (type == 0) {
          // A 0-sized read never returns any data.
          _o << "return 0;\n";
        } else {
          if (dmi) {
            // We only update the DMI cache from the top-level cache, since we can't
            // skip the top-cache on a memory access.
            // If we have a DMI cache, then update the cache here.
            _o << "uint" << type << "_t x = byte_read" << type << "(" << array << "[set][way]._data,((unsigned)addr) & " << getCacheGetLineMask(cache) << "());\n"
               << un << "(ea,addr," << array << "[set][way]._data);\n"
               << "return x;\n";
          } else {
            _o << "return byte_read" << type << "(" << array << "[set][way]._data,((unsigned)addr) & " << getCacheGetLineMask(cache) << "());\n";
          }
        }
        if (cache.miss_enable()) {
          _o << "} else {\n"
             << n_read.str()
             << "}\n";
        }
      }
      _o << "}\n"
         << "}\n";
    }

    // If the next level is memory, then update internal DMI on a miss.  Don't
    // do this is we have external DMI, as it will get updated via the
    // external method.
    void getCacheNextWriteStr(ostream &o,bool dmi,const gc_string &name,const gc_string &un,
                              unsigned vtype,bool n_is_mem,bool has_nb)
    {
      if (dmi) {
        o << name << "write" << vtype << "(addr,v" << ( (has_nb) ? ",nb" : "") << ");\n" 
          << un << "(addr,lastRealAddr()," << name << "last_page(lastRealAddr() & DmiCache::CompMask));\n";
      } else {
        o << name << "write" << vtype << "(addr,v" << ( (has_nb) ? ",nb" : "") << ");\n";
      }
    }

    // Write a single cache write acceessor function.  The basic logic is this:
    // If the cache is enabled, perform the read.  If we have an allocate
    // predicate, then the access function is conditional: false => read from
    // next level, true => read from cache.  If we have a write-through predicate,
    // then we write to the next level if that predicate returns true.
    void writeCacheWriteFunc(const Cache &cache,const gc_string &nname,const gc_string &mname,
                             bool n_is_mem,unsigned vtype,bool has_nb)
    {
      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);

      bool dmi = dmi_cache_support() && &cache == _core.get_first_cache(DataCache);


      bool n_dmi  = (n_is_mem && dmi_cache_support() && !extern_dmi());
      bool to_dmi = (tags_only_caches() && dmi_cache_support() && !extern_dmi());

      unsigned dtype = (vtype == 0) ? 8 : vtype;
      gc_string un = getDmiCacheUpdateName(GlobalMemoryName,"write");

      gc_ostringstream n_write, to_write;
      getCacheNextWriteStr(n_write,n_dmi,nname,un,vtype,n_is_mem,has_nb);
      getCacheNextWriteStr(to_write,to_dmi,mname,un,vtype,true,has_nb);

      _o << "void " << cname << "_write" << vtype << "(" << ((dmi) ? "addr_t ea," : "") << "addr_t addr, uint" << dtype << "_t v";
      if (has_nb) {
        _o << ", unsigned nb";
      }
      _o << ") " << attr_used << "\n"
         << "{\n"
         << "if (!" << cname << "_enabled(CacheWrite,addr)) {\n"
         << n_write.str()
         << "return;\n"
         << "} else {\n"
         << "int set,way;\n"
         << "bool replace;\n";
      if (dmi) {
        _o << "_lastEffectiveAddr = ea;\n";
      }

      // If we don't have a mis-enable, then the access function will always get
      // the required data.  If we do have a miss-enable function, then we might
      // not get the data, e.g. an allocate-on-write-only cache.  If we're in
      // tags-only mode, then we do whatever, but we always read from memory,
      // since we don't store any data.
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "if (" << cname << "_access<true>(CacheWrite,set,way,addr,replace)) {\n";
      } else {
        _o << cname << "_access<true>(CacheWrite,set,way,addr,replace);\n";
      }
      
      if (tags_only_mode()) {
        // In tags-only mode, since we don't have data, always write to the next
        // level after doing any access stuff.
        _o << to_write.str();
      } else {
        if (vtype != 0) {
          _o << "byte_write" << vtype << "(" << array << "[set][way]._data,((unsigned)addr) & " << getCacheGetLineMask(cache) << "(),v";
          if (has_nb) {
            _o << ",nb";
          }
          _o << ");\n";
        }
        if (cache.write_through()) {
          _o << "if (" << cname << "_write_through_pred(CacheWrite,addr)) {\n"
             << n_write.str()
             << "} else {\n";
        } else {
          _o << "{\n";
        }
        if (dmi) {
          // We only update the DMI cache from the top-level cache, since we can't
          // skip the top-cache on a memory access.  Also, we only insert it into
          // the write cache if we're going to mark it as dirty, in order to avoid
          // write-through issues.
          _o << un << "(ea,addr," << array << "[set][way]._data);\n";
        }
        _o << array << "[set][way].dirty = true;\n"
           << "}\n";
        if (cache.miss_enable()) {
          _o << "} else {\n"
             << n_write.str()
             << "}\n";
        }
      }
      _o << "}\n"
         << "}\n";
    }

    // This function creates the functions used by the transactional ISS for
    // beginning and ending cache operations.
    void writeCacheTransOps(const Cache &cache,bool has_data)
    {

      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);
      gc_string entry = array + "[set][way]";

      gc_string status_arg = (cache.miss_enable()) ? "cs," : "";
      
      if (!perf_mode() && has_data) {
        // Helper function for accessing the data array in a specific cache
        // line.  Only for true transactional ISSs- performance caches in
        // iss-mem-mode do not have data.
        _o << "byte_t *" << cname << "_data_array(int set,int way) {\n"
           << "return " << entry << "._data;\n"
           << "}\n\n";
      }

      {
        // The beginning of an operation does the lookup and specifies the
        // destination set/way and whether an eviction is required.  It returns
        // true if the operation should proceed, false if the operation for this
        // cache should be skipped and the next level of cache used.
        _o << "bool " << cname << "_begin(CacheAccess ca,CacheAction &action,const CacheStatus &cs,int &set,int &way,addr_t addr,bool &replace,bool direct = false) {\n"
           << "if (direct) {\n"
           << "switch (ca) {\n"
           << "case CacheTouch:\n"
           << cname << "_access<true>(ca,action," << status_arg << "set,way,addr,replace);\n"
           << "return true;\n"
            << "case CacheAlloc:\n"
           << cname << "_access<false>(ca,action," << status_arg << "set,way,addr,replace);\n"
           << "return true;\n"
           << "case CacheLockAddr:\n"
           << cname << "_access<true>(ca,action," << status_arg << "set,way,addr,replace);\n"
           << "return true;\n"
           << "case CacheInvalidate:\n"
           << "action = CacheNone;\n"
           << "if (set >= 0) {\n"
           << cname << "_invalidate(set,way);\n"
           << "}\n"
           << "return true;\n"
           << "case CacheFlush:\n"
           << "case CacheStore:\n"
           << "action = CacheNone;\n"
           << "if (set >= 0) {\n"
           << cname << "_flush_line(action,ca,set,way);\n"
           << "}\n"
           << "return true;\n"
           << "case CacheWrite:\n";
        if (cache.miss_enable()) {
          _o << "if (" << cname << "_access<false>(ca,action,cs,set,way,addr,replace)) {\n";
        } else {
          _o << cname << "_access<true>(ca,action,set,way,addr,replace);\n";
        }
        _o << entry << ".busy++;\n"
           << "return true;\n";
        if (cache.miss_enable()) {
          _o << "} else {\n"
             << "return false;\n"
             << "}\n";
        }
        _o << "default:\n"
           << "assert(0);\n"
           << "}\n"
           <<"} else {\n"
           << "bool enabled = (cs.valid()) ? cs.enabled(" << cache.level() << ") : " << cname << "_enabled(ca,addr);\n"
           << "if (!enabled) {\n"
           << "return false;\n"
           << "} else {\n";
        if (cache.miss_enable()) {
          _o << "if (" << cname << "_access<true>(ca,action,cs,set,way,addr,replace)) {\n";
        } else {
          _o << cname << "_access<true>(ca,action,set,way,addr,replace);\n";
        }
        _o << entry << ".busy++;\n"
           << "return true;\n";
        if (cache.miss_enable()) {
          _o << "} else {\n"
             << "return false;\n"
             << "}\n";
        }
        _o << "}\n"
           << "}\n"
           << "}\n\n";
      }

      {
        // The end operation is access-type dependent.  This will set
        // miscellaneous fields as necessary.
        _o << "void " << cname << "_end(CacheAccess ca,CacheAction action,int set,int way,addr_t addr,bool direct = false) {\n"
           << "if (direct && ca == CacheLockAddr) {\n"
           << cname << "_lock(set,way,true);\n"
           << "}\n"
           << "if (!direct) {\n"
           << "switch (action) {\n"
           << "case CacheMiss:\n"
           << "case CacheEvict:\n"
           // Hangsheng: tag update at _end() may destory consecutive linefill
           // << "if (!(direct && (ca == CacheFlush || ca == CacheStore))) {\n"
           // << entry << ".tag = addr & ~0x" << hex << linemask << dec << ";\n"
           // << "}\n"
           << entry << ".valid = true;\n"
           << entry << ".dirty = false;\n"
           << "break;\n"
           << "default:\n"
           << "break;\n"
           << "}\n"
           << "}\n"
           << "if (set >= 0 && way >= 0 && " << entry << ".busy) {\n"
           << entry << ".busy--;\n"
           << "}\n"
           << "switch (ca) {\n"
           << "case CacheWrite:\n";

        if (cache.write_through())
          _o << "if (!" << cname << "_write_through_pred(ca,addr))\n";

        _o << array << "[set][way].dirty = true;\n"
           << "break;\n"
           << "default:\n"
           << ";\n"
           << "}\n"
           << "}\n";
      }
    }

    void genCacheAPIObj(const Cache &cache)
    {
      int    id       = cache.id();
      gc_string name     = cache.name();
      gc_string tname    = cache.name() + "_cache_obj";
      gc_string cname    = getCacheName(cache);
      gc_string sname    = getCacheSetName(cache);
      gc_string wname    = getCacheWayName(cache);
      gc_string mcall    = "_core." + cname;
      gc_string marray   = "_core." + getCacheArrayName(cache);
      gc_string mentry   = marray + "[set][way]";
      gc_string tag      = mentry + ".tag";

      bool is_dyn = is_dynamic(cache);

      gc_string action_arg = (trans_cache()) ? "action," : "";
      gc_string action_var= (trans_cache()) ? "CacheAction action;\n" : "";

      _o << "struct " << tname << " {\n"
         << _core.name() << " &_core;\n"
         << tname << "(" << _core.name() << "&x) : _core(x) {}\n\n"

         << "unsigned num_sets() const { \n";
      if (cache.multi_sets() || is_dyn) {
        _o << "return _core." << getCacheNumSets(cache) << ";\n";
      } else {
        _o << "return " << cache.sets() << ";\n";
      }
      _o << "}\n"

         << "unsigned num_ways() const {\n";
      if (cache.multi_ways() || is_dyn) {
        _o << "return _core." << getCacheNumWays(cache) << ";\n";
      } else {
        _o << "return " << cache.ways() << ";\n";
      }
      _o << "}\n"

         << "unsigned get_set(addr_t ra) const " << attr_used << "{\n"
         << "return " << mcall << "_get_set(ra);\n"
         << "}\n"

         << "void check_valid_set(unsigned set) const " << attr_used << "{\n"
         << "if (set >= num_sets()) {\n"
         << "RError(\"Bad set value specified:  \" << set << \" (valid is (0..\" << num_sets()-1 << \").)\");\n"
         << "}"
         << "}\n"

         << "void check_valid_way(unsigned way) const " << attr_used << "{\n"
         << "if (way >= num_ways()) {\n"
         << "RError(\"Bad way value specified:  \" << way << \" (valid is (0..\" << num_ways()-1 << \").)\");\n"
         << "}"
         << "}\n"

         << "void check_valid(unsigned set,unsigned way) const " << attr_used << "{\n"
         << "check_valid_set(set);\n"
         << "check_valid_way(way);\n"
         << "}\n"

         << "bool enabled(CacheAccess ca,addr_t addr = 0) const " << attr_used << "{\n"
         << "return " << mcall << "_enabled(ca,addr);\n"
         << "}\n";

      if (!perf_mode()) {

        // We always want to inform an external performance model about cache
        // operations.
        bool ext_handler = !normal_iss();
        // Transactional models and hybrid models w/skipped caches everything
        // externally.  Otherwise, cache logic is done internally and the
        // performance model is simply informed about it.
        bool int_handler = (normal_iss() || (logging_mem() && !skip_caches_in_exec()));

        gc_string ext_start,ext_end,extra_args;
        if (trans_cache()) {
          ext_start = "_core._memory_request.cache_request";
        } else {
          ext_start =  "_core.handle_memory_request(0, MemAccess";
          ext_end   = ")";
        }

        _o << "void touch(addr_t ra) " << attr_used << "\n"
           << "{\n";
        if (int_handler) {
          _o << "int set = -1, way = -1;\n"
             << "bool replace;\n"
             << mcall << "_access<true>(CacheTouch,set,way,ra,replace);\n";
          extra_args = ",set,way";
        }
        if (ext_handler) {
          _o << ext_start << "(CacheTouch," << id << ",ra" << extra_args << ")" << ext_end << ";\n";
        }
        _o << "};\n"

          // This form of lock acts like a touch + lock, so we only create it
          // for non-perf caches and it has to be handled via the external
          // model, if it exists.
           << "void lock(addr_t ra) {\n";
        if (int_handler) {
          _o << "int set = -1, way = -1;\n"
             << "bool replace;\n"
             << mcall << "_access<true>(CacheLockAddr,set,way,ra,replace);\n"
             << mcall << "_lock(set,way,true);\n";
          extra_args = ",set,way";
        }
        if (ext_handler) {
          _o << ext_start << "(CacheLockAddr," << id << ",ra" << extra_args << ")" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void allocate(addr_t ra) {\n";
        if (int_handler) {
          _o << "int set = -1, way = -1;\n"
             << "bool replace;\n"
             << mcall << "_access<false>(CacheAlloc,set,way,ra,replace);\n";
          extra_args = ",set,way";
        }
        if (ext_handler) {
          _o << ext_start << "(CacheAlloc," << id << ",ra" << extra_args << ")" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void flush(addr_t ra) " << attr_used << "\n"
           << "{\n"

           << "int set = -1, way = -1;\n"
           << "if (" << mcall << "_find(CacheFlush,set,way,ra)) {\n"
           << "if (" << marray << "[set][way].dirty) {\n";
        if (int_handler) {
          _o << mcall << "_flush_line(CacheFlush,set,way);\n";
        }
        if (ext_handler) {
          _o << ext_start << "(CacheFlush," << id << "," << tag << ",set,way)" << ext_end << ";\n";
        }
        _o << "return;\n"
           << "}\n"
           << "}\n";
        if (ext_handler) {
          // If we have an external handler, then we want to always generate a
          // cache operation, even if it's a dummy operation, so that the timing
          // model can stay consistent.
          _o << ext_start << "(CacheFlush," << id << ",0,-1,-1)" << ext_end << ";\n";          
        }
        _o << "};\n"

           << "void flush(int set,int way) " << attr_used << "\n"
           << "{\n"

           << "check_valid(set,way);\n"
           << "if (" << marray << "[set][way].dirty) {\n";
        if (int_handler) {
          _o << mcall << "_flush_line(CacheFlush,set,way);\n";
        } 
        if (ext_handler) {
          _o << ext_start << "(CacheFlush," << id << "," << tag << ",set,way)" << ext_end << ";\n";
        }
        _o << "return;\n"
           << "}\n";
        if (ext_handler) {
          _o << ext_start << "(CacheFlush," << id << ",0,-1,-1)" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void store(addr_t ra) " << attr_used << "\n"
           << "{\n"

           << "int set = -1, way = -1;\n"
           << "if (" << mcall << "_find(CacheStore,set,way,ra)) {\n"
           << "if (" << marray << "[set][way].dirty) {\n";
        if (int_handler) {
          _o << mcall << "_flush_line(CacheStore,set,way);\n";
        } 
        if (ext_handler) {
          _o << ext_start << "(CacheStore," << id << "," << tag << ",set,way)" << ext_end << ";\n";
        }
        else {
          _o << marray << "[set][way].dirty = false;\n";
        }
        _o << "return;\n"
           << "}\n"
           << "}\n";
        if (ext_handler) {
          _o << ext_start << "(CacheStore," << id << ",0,-1,-1)" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void store(int set,int way) " << attr_used << "\n"
           << "{\n"

           << "check_valid(set,way);\n"
           << "if (" << marray << "[set][way].dirty) {\n";
        if (int_handler) {
          _o << mcall << "_flush_line(CacheStore,set,way);\n";
        } 
        if (ext_handler) {
          _o << ext_start << "(CacheStore," << id << "," << tag << ",set,way)" << ext_end << ";\n";
        }
        else {
          _o << marray << "[set][way].dirty = false;\n";
        }
        _o << "return;\n"
           << "}\n";
        if (ext_handler) {
          _o << ext_start << "(CacheStore," << id << ",0,-1,-1)" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void invalidate(addr_t ra) " << attr_used << "\n"
           << "{\n"

           << "int set = -1, way = -1;\n"
           << "if (" << mcall << "_find(CacheInvalidate,set,way,ra)) {\n";
        if (int_handler) {
          _o << mcall << "_invalidate(set,way);\n";
        } 
        if (ext_handler) {
          _o << ext_start << "(CacheInvalidate," << id << "," << tag << ",set,way)" << ext_end << ";\n";
        }
        _o << "return;\n"
           << "}\n";
        if (ext_handler) {
          _o << ext_start << "(CacheInvalidate," << id << ",0,-1,-1)" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void invalidate(int set,int way) " << attr_used << "\n"
           << "{\n"

           << "check_valid(set,way);\n";
        if (int_handler) {
          _o << mcall << "_invalidate(set,way);\n";
        }
        if (ext_handler) {
          _o << ext_start << "(CacheInvalidate," << id << "," << tag << ",set,way)" << ext_end << ";\n";
        }
        _o << "};\n"

           << "void fill(addr_t ra,unsigned char c) " << attr_used << "\n"
           << "{\n"
           << mcall << "_fill(ra,c);\n"
           << "}\n"

           << "bool read_from_next(CacheAccess ca,int set,int way,addr_t addr,bool do_load) " << attr_used << "\n"
           << "{\n";
        if (int_handler) {
          _o << "check_valid(set,way);\n"
             << "return " << mcall << "_read_from_next(ca,set,way,addr,do_load);\n";
        } 
        if (ext_handler) {
          _o << "return false;\n";
        }
        _o << "}\n"

           << "void read_from_mem(CacheAccess ca,int set,int way,addr_t addr) " << attr_used << "\n"
           << "{\n";

        if (!trans_cache()) {
          _o << "check_valid(set,way);\n"
             << mcall << "_read_from_mem(ca,set,way,addr);\n";
        }
        _o << "}\n";

      }

      _o << "bool is_locked(int set,int way) const {\n"
         << "check_valid(set,way);\n"
         << "return " << marray << "[set][way].locked;\n"
         << "}\n"

         << "bool is_dirty(int set,int way) const {\n"
         << "check_valid(set,way);\n"
         << "return " << marray << "[set][way].dirty;\n"
         << "}\n"

         << "bool is_valid(int set,int way) const {\n"
         << "check_valid(set,way);\n"
         << "return " << marray << "[set][way].valid;\n"
         << "}\n"

         << "bool is_write_through(CacheAccess ca,addr_t addr) const " << attr_used << "\n"
         << "{\n";

      if (cache.write_through()) {
        _o << "return " << mcall << "_write_through_pred(ca,addr);\n";
      } else {
        _o << "return false;\n";
      }
      _o << "}\n"

         << "bool fully_locked(int set) const {\n"
         << "check_valid_set(set);\n"
         << "return " << marray << "[set].fully_locked();\n"
         << "}\n"

         << "bool fully_unlocked(int set) const {\n"
         << "check_valid_set(set);\n"
         << "return " << marray << "[set].fully_unlocked();\n"
         << "}\n"

         << "bool fully_unlocked() const {\n"
         << "ForRange(num_sets(),set) {\n"
         << "if (!" << marray << "[set].fully_unlocked()) return false;\n"
         << "}\n"
         << "return true;\n"
         << "}\n"

         << "bool lock(int set,int way) " << attr_used << "\n"
         << "{\n"
         << "check_valid(set,way);\n"
         << "return " << mcall << "_lock(set,way,true);\n"
         << "}\n\n"

         << "bool unlock(int set,int way) " << attr_used << "\n"
         << "{\n"
         << "return " << mcall << "_lock(set,way,false);\n"
         << "}\n"

         << "bool find(int &set,int &way,addr_t addr) " << attr_used << "\n"
         << "{\n"
         << "return " << mcall << "_find(CacheNoAccess,set,way,addr);\n"
         << "}\n"

         << wname << " &operator()(unsigned set,unsigned way) {\n"
         << "check_valid(set,way);\n"
         << "return " << marray << "[set][way];\n"
         << "}\n"

         << sname << " &operator()(unsigned set) {\n"
         << "check_valid_set(set);\n"
         << "return " << marray << "[set];\n"
         << "}\n"

         << "};\n\n";

      _cd._member_vars.push_back(tname + " " + name);

      _cd._constr_inits.push_back(name + "(*this)");
    }

    // Generate a sets or way modification hook.
    void genCacheParmHook(const Cache &cache,const gc_string &parm,Ptree *func,
                          const UIntVect &value_list,const gc_string &diff_func = "")
    {
      // No need to do this for performance caches, since they use the same
      // setup functions as the real cache.
      if(perf_mode()) {
        return;
      }

      gc_string fname = parm + "_hook";

      gc_string dguard = (is_dynamic(cache)) ? getCacheDynGuard(cache) : "";

      genFunc("unsigned "+fname,cache.env(),func);
      Ptree *sym = AdlGenSym();
      _o << "void " << sym << " () {\n";
      if (!dguard.empty()) {
        // If the cache can be modified via the dynamic parameter API, then if
        // any parameter has been set, then we ignore model-specified changed.
        // In other words, the dynamic parms override the model.
        _o << "if (!" << dguard << ") {\n";
      }
      if (!diff_func.empty()) {
        _o << "unsigned old = " << parm << ";\n";
      }
      _o << parm << " = " << fname << "();\n"
         << "if (!(";
      bool first = true;
      ForEach(value_list,i) {
        pfx_sep(_o," || ",first);
        _o << "(" << parm << " == " << *i << ")";
      }
      _o << ")) {\n"
         << "RError(\"Invalid value for " << parm << " specified:  \" << " << parm << ");\n"
         << "}\n";
      if (!diff_func.empty()) {
        _o << "if (old != " << parm << ") {\n"
           << diff_func << "();\n"
           << "}\n";
      }
      if (!dguard.empty()) {
        _o << "}\n";
      }
      _o << "}\n\n";
      _watches.push_back(Watch(_walker.regs_referenced(),sym));        
    }

    // If we have internal DMI and external DMI, and we don't have tags-only
    // caches, meaning that we can have different data in the caches than
    // memory, then if we enable or disable a cache, we have to invalidate the
    // internal DMI cache, since it could now hold stale data (pointers to the
    // wrong thing).
    void genDmiInvalidateHook()
    {
      if (!tags_only_caches() && extern_dmi() && dmi_cache_support()) {
        _watches.push_back(Watch(_walker.regs_referenced(),Ptree::Make("invalidate_internal_dmi_cache")));
      }
    }

    // Miscellaneous utility functions for the caches.
    void genCacheUtilities(const Cache &cache,const gc_string &cname,const gc_string &array,
                           const gc_string &entry,const gc_string &action_parm,const gc_string &action_arg,
                           const gc_string &status_parm,const gc_string &linemask,bool has_rpl)
    {
      if (Ptree *enable = cache.enable()) {
        _o << "bool " << cname << "_enabled(";
        if (get_arity(enable) == 1) {
          _o << funcArgs(enable) << ",addr_t)";
        } else {
          _o << funcArgs(enable) << ")";
        }
        _o << " {\n "
           << funcBody(cache.env(),enable) << "\n";
        genDmiInvalidateHook();
      } else {
        _o << "bool " << cname << "_enabled(CacheAccess,addr_t) {\n "
           << "return true;\n";
      }
      _o << "}\n\n";

      if (Ptree *hit = cache.hit()) {
        _o << "void " << cname << "_hit_func(" << funcArgs(hit) << ") "
           << funcBody(cache.env(),hit) << "\n";
      }

      if (Ptree *miss = cache.miss()) {
        _o << "void " << cname << "_miss_func(" << funcArgs(miss) << ") "
           << funcBody(cache.env(),miss) << "\n\n";
      }

      if (cache.multi_sets()) {
        genCacheParmHook(cache,getCacheNumSets(cache),cache.sets_func(),cache.sets_list());
      }

      if (cache.multi_ways()) {
        gc_string diff_func = (!has_rpl) ? cname+"_reset_lru" : "";
        genCacheParmHook(cache,getCacheNumWays(cache),cache.ways_func(),cache.ways_list(),diff_func);    
      }

      if (Ptree *hp = cache.hit_pred()) {
        _o << "bool " << cname << "_hit_pred(" << funcArgs(hp) << ") "
           << funcBody(cache.env(),hp) << "\n\n";
      }

      _o << "bool " << cname << "_find(CacheAccess ca,int &set,int &way,addr_t addr) {\n"
         << "set = " << cname << "_get_set(addr);\n"
         << "for (unsigned i = 0; i != " << getCacheGetWays(cache) << "(); ++i) {\n"
         << "if (" << array << "[set][i].valid && " << array << "[set][i].tag == (addr & ~" << hex << linemask << dec << ")";
      if (cache.hit_pred()) {
        _o << " && " << cname << "_hit_pred(ca,set,way)";
      }
      _o << ") {\n"
         << "way = i;\n"
         << "return true;\n"
         << "}\n"
         << "}\n";
      _o << "return false;\n"
         << "}\n\n";

      // The access function is used throughout the cache and does the work of
      // looking up an address and returning the set and way.  If the DoLoad
      // template parameter is set, then if the line is not present in the
      // cache, it will be loaded.  If false, then a line will be allocated but
      // no load will occur.
      gc_string access_rt = (cache.miss_enable()) ? "bool" : "void";
      genFunc("void "+cname+"_access_hook",cache.env(),cache.line_access());
      _o << "template <bool DoLoad> " << access_rt << " " << cname << "_access(CacheAccess ca," << action_parm << status_parm << "int &set,int &way,addr_t addr,bool &replace) {\n";
      if (trans_cache()) {
        _o << "action = CacheHit;\n"
           << "replace = false;\n";
      }
      _o << "if (!" << cname << "_find(ca,set,way,addr)) {\n";
      if (cache.miss_enable()) {
        if (trans_cache()) {
          _o << "bool alloc = (cs.valid()) ? cs.miss_allocate(" << cache.level() << ") : " << cname << "_allocate_pred(ca,addr,set);\n";
        } else {
          _o << "bool alloc = " << cname << "_allocate_pred(ca,addr,set);\n";
        }
        _o << "if (!alloc) {\n"
           << "return false;\n"
           << "}\n";
      }
      if (trans_cache()) {
        _o << "action = CacheMiss;\n";
      }
      _o << "way = (DoLoad) ? " << cname << "_replace_and_load(" << action_arg << "ca,set,addr) : " << cname << "_replace_noload(" << action_arg << "ca,addr);\n";
      if (trans_cache()) {
        _o << entry << ".old_tag = " << entry << ".tag;\n"
           << entry << ".tag = addr & ~" << hex << linemask << dec << ";\n"
           << "if (" << entry << ".valid) { replace = true; }\n"
           << "else { " << entry << ".valid = true; }\n"
           << entry << ".dirty = ca == CacheWrite;\n";
      }
      if (cache.miss()) {
        _o << cname << "_miss_func (ca,addr);\n";      
      }
      _o << "} else {\n";
      writeCacheLogCode(cache,"CacheHit","ca","set","way","addr");
      if (cache.hit()) {
        _o << cname << "_hit_func (ca,addr);\n";      
      }
      _o << "}\n";
      if (cache.line_access()) {
        _o << cname << "_access_hook(ca,set,way,addr);\n";
      }
      if (cache.miss_enable()) {
        _o << "return true;\n";
      }
      _o << "}\n\n"
         << "bool " << cname << "_ro_access(CacheAccess ca,int &set,int &way,addr_t addr) {\n"
         << "if (!" << cname << "_find(ca,set,way,addr)) {\n";
      writeCacheLogCode(cache,"CacheMiss","ca","-1","-1","addr");
      if (cache.miss()) {
        _o << cname << "_miss_func (ca,addr);\n";      
      }
      _o << "return false;\n"
         << "} else {\n";
      writeCacheLogCode(cache,"CacheHit","ca","set","way","addr");
      if (cache.hit()) {
        _o << cname << "_hit_func (ca,addr);\n";      
      }
      _o << "return true;\n"
         << "}\n"
         << "}\n\n";

      genFunc("bool "+cname+"_write_through_pred",cache.env(),cache.write_through());
      genFunc("bool "+cname+"_allocate_pred",cache.env(),cache.miss_enable());
    }

    // Generate cache read/write logic for page accesses.  This is for normal caches (not for
    // transactional-iss stuff).
    void genCachePageReadWrite(const Cache &cache,const gc_string &cname,const gc_string &nname,const gc_string &mname,
                               const gc_string &entry,const gc_string &narg0,const gc_string &wp_narg0,const gc_string &marg0,
                               const gc_string &action_var,const gc_string &action_arg,
                               const gc_string &linemask,const gc_string &linesize,
                               bool has_next_cache,bool perf)
    {
      // This allows a higher-level cache to write its page directly to this
      // cache.  If a write_through hook exists, we query it to determine whether
      // we should write our data on or simply mark the page as dirty.  Two
      // versions exist: The first version will load a line if not present,
      // whereas the second version only allocates a line- it doesn't load if not
      // present.  Generally, the latter will be used when a higher-level cache
      // does an eviction and its line size is the same as the line size of the
      // next cache down the hierarchy.
      // The two versions are selected via the boolean template parameter.
      gc_string t_arg = (has_next_cache) ? "<DoLoad>" : "";
      _o << "template <bool DoLoad> void " << cname << "_writepage(CacheAccess ca,byte_t *s,size_t n,addr_t addr,addr_t crit_addr) {\n"
         << "if (!" << cname << "_enabled(ca,addr)) {\n"
         << nname << "writepage" << t_arg << "(" << wp_narg0 << "s,n,addr,crit_addr);\n"
         << "} else {\n"
         << "int set = -1, way = -1;\n"
         << "bool replace;\n"
         << action_var;
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "if (" << cname << "_access<DoLoad>(ca," << action_arg << "set,way,crit_addr,replace)) {\n";
      } else {
        _o << cname << "_access<DoLoad>(ca," << action_arg << "set,way,crit_addr,replace);\n";
      }

      if (tags_only_mode()) {
        _o << mname << "writepage(s,n,addr,crit_addr);\n";
      } else if (!perf) {
        _o << "byte_pagecopy(" << entry << "._data,((unsigned)addr) & " << hex << linemask << dec << ",s,0,n);\n";
      }

      if (cache.write_through()) {
        _o << "if (" << cname << "_write_through_pred(ca,addr)) {\n";
        if (!tags_only_mode()) {
          gc_string d = (perf) ? "0" : entry + "._data";
          if (!perf || has_next_cache) {
            _o << nname << "writepage" << t_arg << "(" << wp_narg0 << d << "," << linesize << "," << entry << ".tag,crit_addr);\n";
          }
        }
        _o << "} else {\n";
      } else {
        _o << "{\n";
      }
      _o << entry << ".dirty = true;\n"
         << "}\n";
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "} else {\n"
           << nname << "writepage" << t_arg << "(" << wp_narg0 << "s,n,addr,crit_addr);\n" 
           << "}\n";
      }
      _o << "}\n"
         << "}\n"
         << "\n"


        // This allows a higher-level cache to request a page.
         << "bool " << cname << "_readpage(CacheAccess ca,byte_t *s,size_t n,addr_t addr,addr_t crit_addr,bool do_load = true) {\n"
         << "int set = -1, way = -1;\n"
         << "bool replace;\n"
         << "if (!" << cname << "_enabled(ca,addr)) {\n";
      if (has_next_cache) {
        _o << "return " << nname << "readpage(" << narg0 << "s,n,addr,crit_addr,do_load);\n";
      } else {
        _o << nname << "readpage(" << narg0 << "s,n,addr,crit_addr);\n"
           << "return true;\n";
      }
      _o << "} else if (do_load) {\n"
         << action_var;
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "if (" << cname << "_access<true>(ca," << action_arg << "set,way,crit_addr,replace)) {\n";
      } else {
        _o << cname << "_access<true>(ca," << action_arg << "set,way,crit_addr,replace);\n";
      }
      if (tags_only_mode()) {
        _o << mname << "readpage(" << marg0 << "s,n,addr,crit_addr);\n";
      } else if (!perf) {
        _o << "byte_pagecopy(s,0," << entry << "._data,((unsigned)addr) & " << hex << linemask << dec << ",n);\n";
      }
      _o << "return true;\n";
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "} else {\n";
        if (has_next_cache) {
          _o << "return " << nname << "readpage(" << narg0 << "s,n,addr,crit_addr,do_load);\n";
        } else {
          _o << nname << "readpage(" << narg0 << "s,n,addr,crit_addr);\n"
             << "return true;\n";
        }
        _o << "}\n";
      }
      _o << "} else {\n"
         << "if (" << cname << "_ro_access(ca,set,way,crit_addr)) {\n";
      if (tags_only_mode()) {
        _o << mname << "readpage(" << marg0 << "s,n,addr,crit_addr);\n";
      } else if (!perf) {
        _o << "byte_pagecopy(s,0," << entry << "._data,((unsigned)addr) & " << hex << linemask << dec << ",n);\n";
      }
      _o << "return true;\n"
         << "} else {\n"
         << "return false;\n"
         << "}\n"
         << "}\n"
         << "};\n\n";

      // This allows for reading a line w/normal eviction.
      _o << "void " << cname << "_readpage_full(CacheAccess ca,byte_t *s,size_t n,addr_t addr,addr_t crit_addr) {\n"
         << "if (!" << cname << "_enabled(ca,addr)) {\n";
      if (has_next_cache) {
        _o << nname << "readpage_full(" << narg0 << "s,n,addr,crit_addr);\n";
      } else {
        _o << nname << "readpage(" << narg0 << "s,n,addr,crit_addr);\n";
      }
      _o << "} else {\n"
         << "int set = -1, way = -1;\n"
         << "bool replace;\n";
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "if (" << cname << "_access<true>(ca,set,way,crit_addr,replace)) {\n";
      } else {
        _o << cname << "_access<true>(ca,set,way,crit_addr,replace);\n";
      }
      if (tags_only_mode()) {
        _o << mname << "readpage(" << marg0 << "s,n,addr,crit_addr);\n";
      } else if (!perf) {
        _o << "byte_pagecopy(s,0," << entry << "._data,((unsigned)addr) & " << hex << linemask << dec << ",n);\n";
      }
      if (cache.miss_enable() && !tags_only_mode()) {
        _o << "} else {\n";
        if (has_next_cache) {
          _o << nname << "readpage_full(" << narg0 << "s,n,addr,crit_addr);\n";
        } else {
          _o << nname << "readpage(" << narg0 << "s,n,addr,crit_addr);\n";
        }
        _o << "}\n";

      }
      _o << "}\n"
         << "}\n\n";

      // For tags-only mode, this does a fake access to get the tags right, but
      // doesn't actually read in any data.
      if (tags_only_mode()) {

        _o << "void " << cname << "_readpage_null(CacheAccess ca,addr_t addr) {\n"
           << "if (!" << cname << "_enabled(ca,addr)) {\n";
        if (has_next_cache) {
          _o << nname << "readpage_null(ca,addr);\n";
        }
        _o << "} else {\n"
           << "int set = -1, way = -1;\n"
           << "bool replace;\n"
           << cname << "_access<true>(ca,set,way,addr,replace);\n"
           << "}\n"
           << "}\n\n";
      }
    }

    // Generate next-level read/write routines.
    void genCacheNextReadWrite(const Cache &cache,const gc_string &cname,const gc_string &nname,
                               const gc_string &entry,const gc_string &narg0,
                               const gc_string &linemask,const gc_string &linesize,
                               bool has_next_cache,bool perf,bool has_data)
    {
      if (!trans_cache()) {
        // The load routine's default behavior is to use the lower cache's load
        // routine, if we have a lower cache.  Otherwise, it loads from memory.
        // For speed, we work directly with the data pages, which means we can't
        // have a cache-line that's bigger than a memory page.  However, there's
        // no good way to check that, since it's independent of the generator's
        // operation.  If the user has supplied their own routine, then we use
        // that instead.
        _o << "bool " << cname << "_read_from_next(CacheAccess ca,unsigned set,unsigned way, addr_t addr, bool do_load = true) {\n";
        if (!perf) {
          // If we're in tags-only mode, there's no need to read anything, since the only data is in memory.
          if (!tags_only_mode()) {
            if (has_next_cache) {
              _o << "if (!" << nname << "readpage(" << narg0 << entry << "._data," << linesize << ",(addr & ~" << hex << linemask << dec << "),addr,do_load)) {\n"
                 << "return false;\n"
                 << "}\n";
            } else {
              _o << nname << "readpage(" << narg0 << entry << "._data," << linesize << ",(addr & ~" << hex << linemask << dec << "),addr);\n";
            }
          } else if (has_next_cache) {
            // We have a next cache and we're tags-only, so we at least have to
            // access the cache to get the tags correct.
            _o << nname << "readpage_null(ca,addr);\n";
          }
          writeCacheLogCode(cache,"CacheLoad","ca","set","way",entry+".tag");
        }
        _o << "return true;\n"
           << "}\n\n";

        if (Ptree *rl = cache.read_line()) {
          genFunc("void "+cname+"_load",cache.env(),rl);
        } else {
          _o << "void " << cname << "_load(CacheAccess ca,unsigned set,unsigned way, addr_t addr) {\n"
             << cname << "_read_from_next(ca,set,way,addr);\n"
             << "}\n\n";
        }
      } else if (has_data) {
        // Helper routine for data movement.
        if (has_next_cache) {
          _o << "bool " << cname << "_read_from_next(CacheAccess ca,unsigned set,unsigned way, unsigned n_set, unsigned n_way, addr_t addr) {\n";
          if (!perf) {
            _o << "byte_pagecopy(" << entry << "._data,0," << nname << "data_array(n_set,n_way),0," << linesize << ");\n";
            writeCacheLogCode(cache,"CacheLoad","ca","set","way","addr");
          }
          _o << "return true;\n"
             << "}\n\n";
        }

        _o << "bool " << cname << "_write_to_next(CacheAccess ca,unsigned set,unsigned way, unsigned n_set, unsigned n_way, addr_t addr) {\n";
        if (!perf) {
          if (has_next_cache) {
            _o << "byte_pagecopy(" << nname << "data_array(n_set,n_way),((unsigned)addr) & " << hex << linemask << dec << "," << entry << "._data,0," << linesize << ");\n";
          } else {
            _o << nname << "writepage(" << entry << "._data," << linesize << ",addr,addr);\n";
          }
        }
        _o << "return true;\n"
           << "}\n\n";
      }
    }

    // Generate cache fill operation code.
    void genCacheFill(const Cache &cache,const gc_string &cname,const gc_string &mname,
                      const gc_string &entry,const gc_string &linesize,bool sm_cache,bool perf)
    {
      _o << "void " << cname << "_fill(addr_t addr,unsigned char c) {\n";
      if (!trans_cache()) {
        _o << "int set = -1, way = -1;\n"
           << "bool replace;\n";
        if (cache.miss_enable()) {
          _o << "if (" << cname << "_access<false>(CacheWrite,set,way,addr,replace)) {\n";
        } else {
          _o << cname << "_access<false>(CacheWrite,set,way,addr,replace);\n";
        }
        if (tags_only_mode()) {
          _o << mname << "fillpage(c," << linesize << ",addr);\n";
        } else if (!perf) {
          _o << "byte_pagefill(" << entry << "._data,0,c," << linesize << ");\n";
        }
        _o << entry << ".dirty = true;\n";
        if (sm_cache) {
          // For safe-mode, inform the performance model about the action.
          _o << "handle_memory_request(0, MemAccess(CacheWrite," << cache.id() << ",addr));\n";
        }
        if (cache.miss_enable()) {
          _o << "}\n";
        }
      } else if (!perf_mode() ) {
        // This is treated as a memory request b/c we expect to receive a line
        // into which we can write data.  We do nothing for performance-only
        // caches.
        _o << "_memory_request.cache_request(CacheWrite," << cache.id() << ",addr);\n";
        if (skip_caches_in_exec()) {
          _o << mname << "fillpage(c," << linesize << ",addr);\n";
        } else {
          _o << "byte_pagefill(_memory_request.page(0),0,c," << linesize << ");\n";
        }
      }

      _o << "}\n\n";
    }

    void genCacheFlushInvalidate(const Cache &cache,const gc_string &cname,const gc_string &nname,const gc_string &array,
                                 const gc_string &entry,const gc_string &action_parm,const gc_string &wp_narg0,
                                 const gc_string &load_arg,const gc_string &linesize,bool perf)
    {
      bool dmi = dmi_cache_support() && &cache == _core.get_first_cache(DataCache);

      // The flush_line function evicts a line and writes it to the next cache
      // if necessary.  We statically determine if the next cache's line size is
      // equal to this cache's line size.  If so, then there's no need to
      // perform a load, since the data will be completely overwritten.
      gc_string doload;
      _o << "bool " << cname << "_flush_line(" << action_parm <<"CacheAccess ca,unsigned set,unsigned way) {\n";
      if (dmi) {
        // If we have a DMI cache, then we have to invalidate this entry, since it's no longer in the cache.
        gc_string un = getDmiCacheInvalidateName(GlobalMemoryName);
        _o << "if (" << entry << ".valid) {\n"
           << un << "(" << entry << ".tag);\n"
           << "}\n";
      }
      _o << "if (" << entry << ".valid && " << entry << ".dirty) {\n";
      if (!perf) {
        writeCacheLogCode(cache,"CacheEvict","ca","set","way",entry+".tag");
        if (!trans_cache() && !tags_only_mode()) {
          _o << nname << "writepage" << load_arg << "(" << wp_narg0 << entry << "._data," << linesize << "," << entry << ".tag," << entry << ".tag);\n";
        }
      }
      _o << entry << ".dirty = false;\n";
      if (trans_cache()) {
        _o << entry << ".old_tag = " << entry << ".tag;\n"
           << "action = CacheEvict;\n";
      }
      _o << "return true;\n"
         << "}\n"
         << "return false;\n"
         << "}\n\n";

      // The invalidate routine marks a line as invalid.  It will call the
      // invalidate_line hook if one exists.
      genFunc("void "+cname+"_invalidate_line_hook",cache.env(),cache.invalidate_line());
      _o << "void " << cname << "_invalidate(int set,int way) {\n"
         << "if (" << entry << ".valid) {\n"
         << entry << ".valid = false;\n";
      if (cache.invalidate_line()) {
        _o << cname << "_invalidate_line_hook(ca,set,way);\n";
      }
      if (dmi) {
        // If we have a DMI cache, then we have to invalidate this entry, since it's no longer in the cache.
        gc_string un = getDmiCacheInvalidateName(GlobalMemoryName);
        _o << un << "(" << entry << ".tag);\n";
      }
      writeCacheLogCode(cache,"CacheNone","CacheInvalidate","set","way",entry+".tag");
      _o << "}\n"
         << "};\n\n";

      // Lock or unlock on a line basis.
      _o << "bool " << cname << "_lock(int set,int way,bool l) {\n"
         << "bool p = " << entry << ".locked;\n"
         << entry << ".locked = l;\n"
         << "if (l) {\n"
         << "if (!p) ++" << array << "[set]._lock_count;\n";
      writeCacheLogCode(cache,"CacheNone","CacheLock","set","way",entry+".tag");
      _o << "} else {\n"
         << "if (p) --" << array << "[set]._lock_count;\n";
      writeCacheLogCode(cache,"CacheNone","CacheUnlock","set","way",entry+".tag");
      _o << "}\n"
         << "return p;\n"
         << "};\n\n";
    }

    gc_string getLatencyFunc(const Cache &cache)
    {
      return cache.name() + "_cache_miss_latency";
    }

    void genMissLatencyFunc(const Cache &cache)
    {
      auto l = cache_miss_latency(cache);

      if (l.is_func()) {
        genFunc("unsigned "+getLatencyFunc(cache),cache.env(),l._func);
      }
    }

    // Generate the cache replacement algorithm if relevant.
    void genCacheReplace(const Cache &cache,const gc_string &cname,const gc_string &array,
                         const gc_string &action_parm,const gc_string &action_arg,
                         const gc_string &linemask,int ways)
    {
      genMissLatencyFunc(cache);

      gc_string entry = array + "[set][lru_index]";

      // Generate user-replacement algorithm, if one exists.
      genFunc("unsigned "+cname+"_replace_hook",cache.env(),cache.replace());

      // Interface function to the replacement algorithm.
      _o << "unsigned " << cname << "_replace(" << action_parm << "CacheAccess ca,addr_t addr) {\n"
         << "unsigned set = " << cname << "_get_set(addr);\n";

      if (cache.replace()) {
        // We have a user-replacement algorithm- call it for the way to replace.
        _o << "unsigned lru_index = " << cname << "_replace_hook(ca,set);\n";
      } else if (ways == 1) {
        // Just a single way (direct-mapped), so we have no choice.
        _o << "unsigned lru_index = 0;\n";
      } else {
        // Default replacement is true LRU.
        // If we have more than one way, we take from the head and place the new item at the tail.
        _o << "unsigned lru_index;\n"
           << "do {\n"
           << "lru_index = " << array << "[set]._h;\n"
           << array << "[set]._h = " << array << "[set][lru_index]._n;\n"
           << array << "[set][" << array << "[set]._h]._p = -1;\n"
           << array << "[set][lru_index]._n = -1;\n"
           << array << "[set][lru_index]._p = " << array << "[set]._t;\n"
           << "unsigned tmp = " << array << "[set]._t;\n"
           << array << "[set]._t = lru_index;\n"
           << array << "[set][tmp]._n = lru_index;\n"
           << "} while (!" << array << "[set].fully_locked() && " << array << "[set][lru_index].locked);\n";
      }
      writeCacheLogCode(cache,"CacheMiss","ca","set","lru_index","addr");
      // The selected index's age is cleared.  Then we flush its data if the line is
      // dirty and valid.  Then we load the new data.
      _o << cname << "_flush_line(" << action_arg << "ca,set,lru_index);\n";
      if (!trans_cache()) {
        // For transactional ISSs, this is done in the _end method, whereas for non-transactional ISSs,
        // we do it immediately.
        _o << entry << ".tag = addr & ~" << hex << linemask << dec << ";\n"
           << entry << ".valid = true;\n"
           << entry << ".dirty = false;\n";
      }
      _o << "return lru_index;\n"
         << "};\n\n"
         << "unsigned " << cname << "_replace_noload(" << action_parm << "CacheAccess ca,addr_t addr)"
         << nonJitOnly()
         << "{\n"
         << "unsigned way = " << cname << "_replace(" << action_arg << "ca,addr);\n"
         << "return way;\n"
         << "}\n"
         << endNonJitOnly()
         << "\n\n"
         << "unsigned " << cname << "_replace_and_load(" << action_parm << "CacheAccess ca,unsigned set,addr_t addr)"
         << nonJitOnly()
         << "{\n"
         << "unsigned way = " << cname << "_replace(" << action_arg << "ca,addr);\n";
      if (!trans_cache()) {
        // No loading for transactional ISSs, since data movement is done by
        // the performance model.
        _o << cname << "_load(ca,set,way,addr);\n";
      }
      if (auto l = cache_miss_latency(cache)) {
        if (time_tagged()) {
          // If we have a time-tagged ISS and a miss penalty for this cache, add
          // it to the overall instruction latency here.  This can be either a function or an integer.        
          _o << "add_cache_latency(";
          if (l.is_int()) {
            _o << l._val << ");\n";
          } else {
            _o << getLatencyFunc(cache) << "(ca,addr));\n";
          }
        } else if (log_td_commit() && !graphite_support()) {
          // If we're simply tracking latency via the td counter, add on the
          // latency here.
          _o << "_td_counter += ";
          if (l.is_int()) {
            _o << l._val << ";\n";
          } else {
            _o << getLatencyFunc(cache) << "(ca,addr);\n";
          }
        } else if (graphite_support()) {
          // We have support for a Graphite performance model, so inform the
          // model of the latency.
          _o << "if (" << getGraphitePerfModel() << ") " << getGraphitePerfModel() << "->handleMem(ca,addr,";
          if (l.is_int()) {
            _o << l._val << "";
          } else {
            _o << getLatencyFunc(cache) << "()";
          }
          _o << ");\n";
        }
      }
      _o << "return way;\n"
         << "}\n"
         << endNonJitOnly()
         << "\n\n";
    }

    // Generate a cache.
    void genCacheFuncs(const Cache &cache,bool perf,bool has_data)
    {

      gc_string cname = getCacheName(cache);
      gc_string array = getCacheArrayName(cache);

      int      ways     = cache.ways();
      bool has_rpl      = cache.replace();

      gc_string linesize = getCacheGetLineSize(cache) + "()";
      gc_string linemask = getCacheGetLineMask(cache) + "()";

      _walker.set_cur_cache(cache.name());

      bool has_next_cache = cache.next();

      gc_string mname = getMemAccessor(true,extern_mem(),extern_dmi(),getGlobalMemRef()) + ".";

      bool n_is_mem = false;
      gc_string nname;
      gc_string log_nname;
      gc_string narg0    = (has_next_cache || extern_mem()) ? "ca," : "";
      gc_string marg0    = (extern_mem()) ? "ca," : "";
      gc_string lr_narg0 = (has_next_cache) ? "force_enable,ca," : ((extern_mem()) ? "ca," : "");
      gc_string lw_narg0 = (has_next_cache) ? "force_enable," : "";
      gc_string wp_narg0 = (has_next_cache) ? "ca," : "";
      gc_string rp_narg0 = (extern_mem()) ? "ca," : "";
      gc_string load_arg;
      if (has_next_cache) {
        if (perf) {
          nname = getFullPerfCacheName(*(cache.next())) + "_";
        } else {
          nname = getCacheName(*(cache.next())) + "_";
        }
        log_nname = nname + "log_";
        if (cache.next()->linesize() == cache.linesize()) {
          load_arg = "<false>";
        } else {
          load_arg = "<true>";
        }
      } else {
        log_nname = nname = mname;
        n_is_mem = true;
      }

      bool sm_cache    = logging_mem();
    
      // The transactional ISS propagates an action flag, whereas the
      // non-transactional ISS does not.
      gc_string action_parm = (trans_cache()) ? "CacheAction &action," : "";
      gc_string action_arg  = (trans_cache()) ? "action," : "";
      gc_string action_var  = (trans_cache()) ? "CacheAction action;\n" : "";

      gc_string status_parm = (trans_cache() && cache.miss_enable()) ? "const CacheStatus &cs," : "";

      {
        gc_string entry = array + "[set][way]";

        // Miscellaneous helper routines.
        genCacheUtilities(cache,cname,array,entry,action_parm,action_arg,status_parm,linemask,has_rpl);

        // For a transactional ISS, we don't need these routines, since all data
        // movement is handled by the performance model.
        if (!trans_cache()) {
          genCachePageReadWrite(cache,cname,nname,mname,entry,narg0,wp_narg0,marg0,action_var,action_arg,
                                linemask,linesize,has_next_cache,perf);
        }

        genCacheNextReadWrite(cache,cname,nname,entry,narg0,linemask,linesize,has_next_cache,perf,has_data);

        genCacheFill(cache,cname,mname,entry,linesize,sm_cache,perf);

        if (has_data || tags_only_mode()) {
          _o << "void " << cname << "_read_from_mem(CacheAccess ca,unsigned set,unsigned way, addr_t addr) {\n";
          if (!perf) {
            if (!tags_only_mode()) {
              _o << mname << "readpage(" << rp_narg0 << entry << "._data," << linesize << ",(addr & ~" << hex << linemask << dec << "),addr);\n";
            }
            writeCacheLogCode(cache,"CacheLoad","ca","set","way","addr");
          }
          _o << "}\n\n";
        }

        genCacheFlushInvalidate(cache,cname,nname,array,entry,action_parm,wp_narg0,load_arg,linesize,perf);
      }
      
      genCacheReplace(cache,cname,array,action_parm,action_arg,linemask,ways);

      // The log read/write functions is a read-only method for accessing the
      // cache.  If will return data if it's in the cache, else defer to the
      // next level.  It's used by the logging mechanism and by test writers
      // that want to see a view of the memory hierarchy from the core's
      // perspective w/o modifying state.
      if (!perf_mode() && (has_data || tags_only_mode())) {
        writeCacheLogReadFunc(cache,log_nname,lr_narg0,64);
        writeCacheLogReadFunc(cache,log_nname,lr_narg0,32);
        writeCacheLogReadFunc(cache,log_nname,lr_narg0,16);
        writeCacheLogReadFunc(cache,log_nname,lr_narg0,8 );
        writeCacheLogReadFunc(cache,log_nname,lr_narg0,0 );
        writeCacheLogReadPage(cache,log_nname,lr_narg0,has_next_cache);
        
        writeCacheLogWriteFunc(cache,log_nname,lw_narg0,64,true);
        writeCacheLogWriteFunc(cache,log_nname,lw_narg0,32,true);
        writeCacheLogWriteFunc(cache,log_nname,lw_narg0,16,true);
        writeCacheLogWriteFunc(cache,log_nname,lw_narg0,8,false);
        writeCacheLogWriteFunc(cache,log_nname,lw_narg0,0,false);
      }
      
      // We only have read/write functionality for non-transactional ISSs.
      // Otherwise, it's up to the performance model to conduct the data transfers.
      if (!trans_cache()) {
        
        // Now generate the access methods.
        writeCacheReadFunc(cache,nname,mname,narg0,marg0,n_is_mem,64);
        writeCacheReadFunc(cache,nname,mname,narg0,marg0,n_is_mem,32);
        writeCacheReadFunc(cache,nname,mname,narg0,marg0,n_is_mem,16);
        writeCacheReadFunc(cache,nname,mname,narg0,marg0,n_is_mem,8 );
        writeCacheReadFunc(cache,nname,mname,narg0,marg0,n_is_mem,0 );
        
        writeCacheWriteFunc(cache,nname,mname,n_is_mem,64,true);
        writeCacheWriteFunc(cache,nname,mname,n_is_mem,32,true);
        writeCacheWriteFunc(cache,nname,mname,n_is_mem,16,true);
        writeCacheWriteFunc(cache,nname,mname,n_is_mem,8, false);
        writeCacheWriteFunc(cache,nname,mname,n_is_mem,0, false);
        
      } else {

        // Otherwise, we write operation begin/end functions.  The performance model
        // must perform the actual data transfers.
        writeCacheTransOps(cache,has_data);
        
      }
      
      // Now generate the closure object which provides the cache access API
      // to the user within an ADL description.
      genCacheAPIObj(cache);
      
    }


    // If we're supporting dynamic caches, then add in the support functions for
    // setting various parameters.
    void genCacheDynSupport(const Cache &cache)
    {
      gc_string pfx = cache.name() + "-";
      gc_string cname = getCacheName(cache);
      
      if (is_dynamic(cache)) {

        gc_string dg  = getCacheDynGuard(cache);

        _cd._member_vars.push_back("bool " + dg);
        _cd._constr_inits.push_back(dg + "(false)");
        
        gc_string set = getSetParmName(cname);
        gc_string get = getGetParmName(cname);
        gc_string lst = getListParmName(cname);

        _o << "bool " << set << "(const std::string &parm,unsigned value)\n"
           << "{\n"
           << "if (set_model_cache_parm(" 
           << getCacheLineSize(cache) << ","
           << getCacheLineMask(cache) << ","
           << getCacheLineShift(cache) << ","
           << getCacheNumSets(cache) << ","
           << getCacheNumWays(cache) << ","
           << "\"" << pfx << "\",parm,value)) {\n"
           << cname << "_reset(true);\n";
        if (have_perf_caches()) {
          _o << getPerfCacheName(cache) << "_cache_reset(true);\n";
        }
        _o << dg << " = true;\n"
           << "return true;\n"
           << "} else {\n"
           << "return false;\n"
           << "}\n"
           << "}\n\n"

           << "bool " << get << "(unsigned &value,const std::string &parm) const\n"
           << "{\n"
           << "return get_model_cache_parm(value,\"" << pfx << "\",parm," 
           << getCacheLineSize(cache) << ","
           << getCacheNumSets(cache) << ","
           << getCacheNumWays(cache) 
           << ");\n"
           << "}\n\n"

           << "void " << lst << "(StrPairs &parms) const\n"
           << "{\n"
           << "return list_model_cache_parm(\"" << pfx << "\",\"" << cache.name() << "\",parms);\n"
           << "}\n\n";

        _cd._dyn_parms.push_back(cname);
      }
    }

    void generateJitICacheTouch()
    {
      // Do this if we have an instruction cache or if we're told to always do it.
      if (!genICacheTouch(_core,_config) || !jit_support()) {
        return;
      }

      _o << "// Function for touching the icache.\n"
         << "void " << getJitICacheTouch() << "(addr_t addr) " << attr_used << "\n"
         << "{\n";

      if (_core.has_caches(InstrCache)) {

        // The usual case- we have an icache.

        const Cache *icache = _core.get_first_cache(InstrCache);

        _o << "if (" << icache->name() << "_cache_enabled(CacheIFetch,addr)) {\n"
           << "int set = -1, way = -1;\n"
           << "bool replace;\n"
           << icache->name() << "_cache_access<true>(CacheIFetch,set,way,addr,replace);\n"
           << "}\n";

      } else {

        // We don't have an icache, but we're told to create this anyway, so we just issue a load to the specified address.
        _o << "instr_read32(addr);\n";

      }

      _o << "}\n";
    }

    // Generate the cache objects, if applicable.
    void generateCaches()
    {
      CacheList caches;

      // Data structures for the caches.  Only include data in the caches if
      // skip-caches it not set.  Otherwise, there's no point in having data,
      // because everything is write-through.
      ForEach(_core.caches(),i) {
        caches.push_back(CacheGen(i->second,_core.is_shared(*(i->second)),false,
                                  is_dynamic(*(i->second)),
                                  // We have data for normal caches unless we're
                                  // in skip mode or tag-only mode.
                                  !(skip_caches_in_exec() || tags_only_caches())
                                  ));
      }

      // Also generate performance caches, if we are using a logging memory
      // interface and we don't have skip-caches set.  If skip-caches are set,
      // then our behavior is basically write-through, with cache management
      // done by an external model.  If skip-caches is not set, then the normal
      // execution flow goes through the caches, and performance caches also
      // exist for an external model to use for proper latency modeling.  Note
      // that performance caches never bother with data.
      if (have_perf_caches()) {
        ForEach(_core.caches(),i) {
          Cache *pcache = new Cache(*i->second);
          pcache->setName(getPerfCacheName(*i->second));
          caches.push_back(CacheGen(pcache,_core.is_shared((*i->second)),true,
                                    is_dynamic(*(i->second)),false));
        }        
      }
      
      generateCacheDS(caches,0);

      generateCacheStatusSupport(caches,InstrCache);
      generateCacheStatusSupport(caches,DataCache);
     
      ForEach(caches,i) {
        _gen_mode = (tags_only_caches()) ? gmTagsOnly : gmNormal;
        if (i->_perf) {
          _gen_mode = gmPerf;
        }
        genCacheFuncs(*i->_cache,i->_perf,i->_data);
        genCacheDynSupport(*i->_cache);
      }

      // Finally, if this is for a JIT, then we want to generate an icache touch
      // function that can be used to update the instruction cache.
      generateJitICacheTouch();
    }


  };

  void generateCacheDS(opfxstream &o,const System &sys,const Resources &shared,
                       ClassData &cd,const ConfigDB &config,StrList *child_args)
  {
    GenCacheBase gm(o,cd,config,0);

    CacheList caches = gm.getCacheGenList(sys,shared);
  
    gm.generateCacheDS(caches,child_args);
  }

  void generateCaches(opfxstream &o,const Core &core,InstrWalker &walker,Watches &watches,
                      const gc_string &filename,ClassData &cd,const ConfigDB &config,const CoreConfig &coreconfig )
  {
    GenCache gm(o,core,walker,watches,filename,cd,config,coreconfig);

    gm.generateCaches();
  }

}

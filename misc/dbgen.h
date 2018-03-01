//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Entry-point for the db generation client.  In general, the database is some
// sort of a textual representation of static data structures- either a list or
// a key/value associative structure, along with scalar elements (integers,
// strings, and booleans).
//

#ifndef db_gen_h
#define db_gen_h

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "opencxx/parser/Ptree.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/AnyOption.h"

#include "fe/AdlParser.h"
#include "fe/Data.h"
#include "fe/Helpers.h"
#include "fe/Types.h"

#include "DocWalker.h"

//#define DEBUG
#ifdef DEBUG
#define DBG(x) _o << "\n<!-- " << x << " -->\n";
#else
#define DBG(x)
#endif

namespace adl {

  class DocCfg;

  // Base interface class for generating a database.
  struct GenerateDBiface : public gc_cleanup  {
    // These are the writer methods which must be overloaded for each type of database that is
    // supported.

    virtual void bool_add (bool value) = 0;
    virtual void data_end () = 0;
    virtual void data_start () = 0;
    virtual void int_add (uint64_t value) = 0;
    virtual void list_start() = 0;
    virtual void list_end() = 0;
    virtual void sublist_start(const gc_string &sep = "sublist") = 0;
    virtual void submap_start(const gc_string &sep = "submap") = 0;
    virtual void map_end() = 0;
    virtual void map_start() = 0;
    virtual void tag_end () = 0;
    virtual void tag_start (const gc_string &key, const gc_string &name = "") = 0;

    // Note that anything that takes a string as an argument should quote the string
    // appropriately for its format type.  A big string is simply something that might contain
    // newlines and lots of text w/punctuation.  Thus, a database format might have an
    // alternative for specifying a large string.
    virtual void string_add (const gc_string &) = 0;
    virtual void bigstring_add (const gc_string &) = 0;
  };

  // The visitor class which populates the database writer class.
  class GenerateCoreDB : public GenerateDBiface {
  protected:
    Opencxx::Ptree * get_func_contents (Opencxx::Ptree * value);
    void genDocString (const gc_string & doc);
    void addfunc (Ptree * p);
    void addexpr (Ptree * p);
    void addfunc (const HelperInfo &hi);
 
    void genBits(const BitPairVect &bits);
    
    // Generate instruction field documentation.
    void genInstrFields ();
    void genInstrFieldDescription (const InstrBase *instr);
    
    // Generates information about fields of a nested instruction field. 
    void genFieldsDescription (const InstrField*);
    
    // True if it has fields that we need to print out. 
    bool hasPrintFields(const Instr *instr) const;
    
    // Generates information about instruction fields of a nested instruction field
    void genInstrFieldsDescription (const InstrField*);
    
    // Generate relocation information
    void genRelocations();

    // For enumerated instrfields.
    void genEnums(const StrIntList&);
    void genAddr(const InstrField*);
    void genType(const InstrField*);
    void genDisplay(const InstrField*);
    void genInstrFieldAsm (const InstrField*);
    void genRanges(const InstrField*);
    void genMasks(const InstrField*);
    void genRef(const InstrField*);
    // For tabled instrfields.
    void genTable(const InstrField*);
    
    // Generates inputs and outputs of instruction ( sources and targets)
    void genUsageInfo (const InstrBase & instr);
    void genUsageInfo (const Instr & instr, bool is_alias);
    void genRegUsage(const gc_string &name,const UsedRegs &ur);
    void genUsageInfo (const UsageInfo &ui);

    void genInstrBlock(const InstrBase *instr);
    
    // Generates attributes
    void genAttrs (const Attrs & attrs);    
     
    void genAsmConfig(); 
   
    // Generates groups.
    void genGroups ();

    // Generates helpers.
    void genHelpers();
    
    // Output an assembler format for an instruction.
    void genInstrAsm (const InstrBase &instr);
    
    // Generate instruction documentation.
    void genInstrs ();

    // Generate subinstruction documentation.
    void genSubInstrs();
  
    bool skip_instr(const Instr *instr);

    template <class R> void genShared (const R & res) {
      tag_start ("shared");
      int_add ((_core.is_shared (res)) ? 1 : 0);
      tag_end ();
    }
    void genRegField (const RegField & rf);
    void genRegFields (const RegBase & reg);
    void genRegReset (const RegBase & reg);
    // Write stuff common to both registers and register files.
    void genRegBaseStuff (const RegBase & reg);
    // Write register read/write hook information.
  protected:
    void genInstrBase(const InstrBase *instr);
    void genInstr(const Instr *instr,const gc_string &parent);

    std::pair<Opencxx::Ptree *,bool> genInstrParentAction(const Instr *instr,const Instr::Alias *alias);
    std::pair<Opencxx::Ptree *,bool> genInstrAction(const Instr *instr,Opencxx::Ptree *action,const gc_string &action_name);
    std::pair<Opencxx::Ptree *,bool> genInstrActions(const Instr *instr);

    bool unknown_field(const Instr &instr,Opencxx::Ptree *x);

    typedef gc_set<const RegBase *> RegSet;

    void getHookInfoTopLevel(UsageInfo &ui,const UsageInfo &src);
    void getHookInfo(UsageInfo &ui,const UsageInfo &src,RegSet &handled_src,RegSet &handled_trg);
    void getHookInfo(UsageInfo &ui,const UsedRegs &src,RegSet &handled_src,RegSet &handled_trg,
                     bool is_src,bool top_level);

    void genInstrAlias(const Instr *instr,bool orig_format);
    void addAliasFieldTarget(const Instr::Field &field);
    void genAliasSrcTgtFields(const Instr::Fields &fields,const UsedRegs &used_regs,gc_string type,StrSet &dumped);
    void genAliasMiscFields(const Instr::Fields &fields,const StrSet &dumped);
    bool genFieldSrcTgt(const char *fname,const UsedRegs &used_regs,gc_string type);
    void genInstrAsmRules(const Instr *instr);
    void genInstrWidth(const Instr *instr);
    void GenNestedInstr(const Instr *outer,const Instr *inner);
    void genRegHook (const gc_string & type, const RegHook & hook);
    // Generate register documentation
    void genRegs ();
    void genEntryHook (const gc_string & type, const RegEntryHook & rh);
    // If we have a sparse register file, then create a table displaying indices vs. register
    // names.
    void genSparseFile (const RegFile & rf);
    // Write register read/write hook information.
    void genRegFileHook (const gc_string & type, const RegFileHook & hook);
    // Generate register-file documentation
    void genRegFiles ();
    // Generate exception information.
    void genExceptions ();
    // Generate context information.
    void genContexts ();
    // Generate information about a generic field.
    template <class Fields> void genFields (const Fields & fields, const gc_string & type = "fields") {
      if (!fields.empty()) {
        tag_start(type);
        map_start();

        ForEach(fields,i) {
          auto field = *(i->second);

          tag_start("field", field.name());
          map_start();

          tag_start("doc");
          bigstring_add(field.doc());
          tag_end ();
          
          tag_start("bits");
          int_add(field.bits());
          tag_end ();


          map_end();
        }

        DBG (__LINE__);
        map_end ();
      }
    }
 
    template <typename C> void genStrList(const gc_string &name, const C &c) {
      if (!c.empty()) {
        tag_start(name);
        list_start();
        ForEach(c,i) {
          gc_ostringstream ss; 
          ss << *i; 
          string_add (ss.str ());
        }
        list_end();
      }
    }
 
    template <typename C> void genKeysList(const gc_string &name, const C &c) {
      if (!c.empty()) {
        tag_start(name);
        list_start();
        ForEach(c,i) {
          string_add (i->first) ;
        }
        list_end();
      }
    }


    void genResources(const Context &ctx);
  protected:
    // Generate cache information.
    void genCaches ();
    // Generate eventbus information.
    void genEventBuses ();
    // Generate external resource information.
    void genExtResources();
    // Generate memory information.
    void genMemories ();
    void genMmuLookups (const MmuLookupHash & lookups, const gc_string & name);
    void genMmu ();
    void genCoreLevelHooks();
    void genParms();
    void genAspects();
    void genMasks();
    void genMask(const AddrMask&);
    void genExtractedOp(const Instr *instr,Ptree *op,bool expand_exprs);
    bool expandExprs(const Instr *instr) const;
    void calcUsedData();

    const Core    &_core;     // Core we're generating documentation for.
    ostream       &_o;
    DocWalker      _walker;
    const DocCfg  &_doccfg;
    int            _no_expand_attr;
    typedef gc_set<const InstrField *> InstrFieldSet;
    typedef gc_set<const Relocation *> RelocSet;

    InstrFieldSet  _used_instrfields;
    RelocSet       _used_relocs;

  public:
    GenerateCoreDB(const Core &c,ostream &o,const DocCfg &doccfg);
    virtual ~GenerateCoreDB ();
    void exec ();
  }; // GenerateCoreDB

  class GenerateSysDB : public GenerateDBiface {
  public:
    GenerateSysDB (const Data & data, ostream & o, const DocCfg &doccfg);
    virtual ~GenerateSysDB ();
    // This must be overloaded to produce a core writer.
    virtual GenerateCoreDB * make_core_db (const Core &c, ostream &o, const DocCfg &doccfg) = 0;
    void exec ();
    void genItem (const SysItem *item, const Resources & pshared);
    void listSharedItems (const gc_string & key, const ResItems & items);
    void genSharedResources (const System & sys, const Resources & pshared);
    void genSystem (const System & sys, const Resources & pshared);
  protected:
    const Data   &_data;
    ostream      &_o;
    const DocCfg &_doccfg;
  };

  // The Perl writer class.
  class GeneratePerlCoreDB : public GenerateCoreDB {
  public:
    GeneratePerlCoreDB (const Core &c,ostream &o, const DocCfg &doccfg);

    // The overall data structure is called $data and is a hash reference.
    virtual void bool_add (bool x);
    virtual void data_end ();
    virtual void data_start ();
    virtual void int_add (uint64_t x);
    virtual void list_end ();
    virtual void list_start ();
    virtual void sublist_start(const gc_string &sep = "sublist");
    virtual void submap_start(const gc_string &sep = "submap");
    virtual void map_end ();
    virtual void map_start ();
    virtual void tag_end () {}
    virtual void tag_start (const gc_string & key, const gc_string & name = "");

    virtual void string_add (const gc_string & x);
    virtual void bigstring_add (const gc_string & x);
  };

  class GeneratePerlSysDB : virtual public GenerateSysDB {
  public:
    GeneratePerlSysDB (const Data & d, ostream & o, const DocCfg &doccfg);
    GeneratePerlCoreDB * make_core_db (const Core & c, ostream & o, const DocCfg &doccfg);

    // The overall data structure is called $data and is a hash reference.
    virtual void bool_add (bool x);
    virtual void data_end ();
    virtual void data_start ();
    virtual void int_add (uint64_t x);
    virtual void list_end ();
    virtual void list_start ();
    virtual void sublist_start (const gc_string & sep = "sublist");
    virtual void submap_start (const gc_string & sep = "submap");
    virtual void map_end ();
    virtual void map_start ();
    virtual void tag_start (const gc_string & key, const gc_string & name = "");
    virtual void tag_end () {}

    virtual void string_add (const gc_string & x);
    virtual void bigstring_add (const gc_string & x);
  };

  class GenerateXmlCoreDB : virtual public GenerateCoreDB {
  public:
    GenerateXmlCoreDB (const Core & c, ostream & o, const DocCfg &doccfg);
    // The database contains a single item named <data>.  We also have the usual XML header
    // stuff.
    virtual void bool_add (bool x);
    virtual void data_end ();
    virtual void data_start ();
    virtual void int_add (uint64_t x);
    virtual void list_end ();
    virtual void list_start ();
    virtual void sublist_start(const gc_string &sep = "sublist");
    virtual void submap_start (const gc_string & sep = "submap");
    virtual void map_end ();
    virtual void map_start ();
    virtual void tag_end ();
    virtual void tag_start (const gc_string & key, const gc_string & name = "");

    virtual void string_add (const gc_string & x);
    virtual void bigstring_add (const gc_string & x);
  private:
    gc_list <gc_string> key_names;
    list <int>          key_count_stack; // close tag when 0
    list <bool>         no_calls_stack;
  };

  class GenerateXmlSysDB : virtual public GenerateSysDB {
  public:
    GenerateXmlSysDB (const Data & d, ostream & o, const DocCfg &docffg);

    GenerateXmlCoreDB * make_core_db (const Core & c, ostream & o, const DocCfg &doccfg);

    virtual void bool_add (bool x);
    virtual void data_end ();
    virtual void data_start ();
    virtual void int_add (uint64_t x);
    virtual void list_end ();
    virtual void list_start ();
    virtual void sublist_start (const gc_string & sep = "sublist");
    virtual void submap_start (const gc_string & sep = "submap");
    virtual void map_end ();
    virtual void map_start (); 
    virtual void tag_end ();
    virtual void tag_start (const gc_string & key, const gc_string & name = "");

    virtual void string_add (const gc_string & x);
    virtual void bigstring_add (const gc_string &x);

  private:
    gc_list <gc_string> key_names;
    list <int>          key_count_stack; // close tag when 0
    list <bool>         no_calls_stack;
  };
}


#endif

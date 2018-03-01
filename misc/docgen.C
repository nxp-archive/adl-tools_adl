//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Entry point for the documentation generation client.
//

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/SourceLocation.h"
#include "opencxx/parser/PtreeArray.h"

#include "helpers/gc_hash_map.h"
#include "helpers/gc_hash_set.h"
#include "helpers/gc_list.h"
#include "helpers/gc_map.h"
#include "helpers/gc_set.h"
#include "helpers/gc_vector.h"
#include "helpers/pfxstream.h"
#include "helpers/AnyOption.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/FilenameUtils.h"
#include "helpers/LogUsage.h"
#include "helpers/StringUtils.h"

#include "fe/Helpers.h"
#include "fe/AdlParser.h"
#include "fe/Data.h"
#include "fe/FieldSupport.h"

#include "DocWalker.h"
#include "DocCfgParser.h"
#include "DocHelpers.h"

using namespace std;
using namespace Opencxx;

namespace adl {

  vector<char>   Headers;

  static gc_hash_set<gc_string>&   HiddenNames() { static gc_hash_set<gc_string> x; return x; }
  static gc_hash_set<const Named*>&   HiddenObjects() { static gc_hash_set<const Named*> x; return x; }

  static const gc_string nbsp_str("&nbsp;");

  bool setupHeaders()
  {
    Headers.push_back('=');
    Headers.push_back('-');
    Headers.push_back('`');
    Headers.push_back(':');
    Headers.push_back('.');
    Headers.push_back('\'');
    Headers.push_back('\"');
    Headers.push_back('~');
    Headers.push_back('^');
    Headers.push_back('_');
    Headers.push_back('*');
    Headers.push_back('+');
    Headers.push_back('#');
    Headers.push_back('!');
    Headers.push_back('$');
    Headers.push_back('%');
    Headers.push_back('&');
    Headers.push_back('(');
    Headers.push_back(')');
    Headers.push_back(',');
    Headers.push_back('/');
    Headers.push_back(';');
    Headers.push_back('<');
    Headers.push_back('>');
    Headers.push_back('?');
    Headers.push_back('@');
    Headers.push_back('[');
    Headers.push_back(']');
    Headers.push_back('\\');
    Headers.push_back('|');
    Headers.push_back('{');
    Headers.push_back('}');
    return true;
  }

  static bool dummy ATTRIBUTE_UNUSED = setupHeaders();

  static void hideName (const gc_string &s)
  {
    HiddenNames().insert(s);
  }

  static void hideObject (const Named &obj)
  {
    hideName(obj.name());
    HiddenObjects().insert(&obj);
  }

  static bool isHiddenName (const gc_string &s)
  {
    return (HiddenNames().end() != HiddenNames().find(s));
  }

  static bool isHiddenObject (const Named &obj)
  {
    return (HiddenObjects().end() != HiddenObjects().find(&obj));
  }

  // Creates a header message.
  // Link anchor names are tracked in unquoted form.
  // You generally shouldn't use this form directly, but instead use
  // Header, CIHeader, and IHeader.
# define CHeader(x,pfx,compact)                     \
  {                                                 \
    gc_ostringstream ss;                            \
    ss << x;                                        \
    printHeader(ss.str(),pfx,compact);              \
  }
# define Header(x) CHeader(x,"",false)

  // Creates an italic header.
  // The "C" variant is for compact headers (no extra blank line).
# define CIHeader(x,compact) CHeader(x,"*",compact)
# define IHeader(x) CIHeader(x,false)

  struct GenerateRstBase {

    opfxstream      &_o;
    const AdlParser &_parser;
    const DocCfg    &_doccfg;

    vector<int>    _indents;
    int            _level;    // Current heading level.

    // Booleans for controlling the emission of headers.
    bool           _rf_header;
    bool           _reg_header;
    bool           _cache_header;
    bool           _memory_header;
    bool           _eventbus_header;
    bool           _extres_header;
    bool           _ctx_header;

    bool           _genregs;
    bool           _genrfs;
    bool           _genifields;
    bool           _geninstrs;
    bool           _geniba;
    bool           _genibb;
    bool           _genexcepts;
    bool           _gencaches;
    bool           _genmems;
    bool           _genevtbuses;
    bool           _genextres;
    bool           _gencontexts;
    bool           _genmmu;
    bool           _genclh;
    bool           _genhelpers;

    bool           _ops_in_affect_tables;
    bool           _instr_prefix_bits;
    bool           _display_syntax_prefix_fields;
    bool           _proportional_subfields;
    bool           _print_location;
    bool           _show_unused_instrs;
    bool           _full_path;
    bool           _expand_exprs;
    unsigned       _hl_level;

    // Stores the names of all registers/register-files and all register-files.
    StrSet        &_allregs;
    StrSet        &_regfiles;

    // Rather than use implicit targets, which might be confused by rst's
    // case-insensitivity and punctuation conversion, we create explicit targets
    // and use them instead.  The boolean in the value indicates whether this
    // item has already been inserted as a header (vs. being inserted as a
    // reference).
    typedef gc_hash_map<gc_string,pair<gc_string,bool > > TargetMap;
    TargetMap       _targets;
    static unsigned _target_id;

    // Tracks what instructions have just a single nesting, and thus in some
    // circumstances we should ignore the child or ignore this one in favor of
    // the child.
    gc_set<const Instr *> _one_nested;
    

    GenerateRstBase(opfxstream &o,const AdlParser &parser,StrSet &allregs,StrSet &regfiles,
                    const AnyOption &options,const DocCfg &doccfg,int level = 0) :
      _o(o),
      _parser(parser),
      _doccfg(doccfg),
      _level(level),
      _rf_header(false),
      _reg_header(false),
      _cache_header(false),
      _memory_header(false),
      _eventbus_header(false),
      _extres_header(false),
      _ctx_header(false),
      _genregs(options.getFlag("reg",true)),
      _genrfs(options.getFlag("rf",true)),
      _genifields(options.getFlag("if",true)),
      _geninstrs(options.getFlag("instr",true)),
      _geniba(options.getFlag("iba",true)),
      _genibb(options.getFlag("ibb",true)),
      _genexcepts(options.getFlag("exc",true)),
      _gencaches(options.getFlag("caches",true)),
      _genmems(options.getFlag("mem",true)),
      _genevtbuses(options.getFlag("eb",true)),
      _genextres(options.getFlag("er",true)),
      _gencontexts(options.getFlag("contexts",true)),
      _genmmu(options.getFlag("mmu",true)),
      _genclh(options.getFlag("clh",true)),
      _genhelpers(options.getFlag("helper-functions",true)),
      _ops_in_affect_tables(options.getFlag("ops-in-affect-tables",true)),
      _instr_prefix_bits(options.getFlag("instr-prefix-bits",true)),
      _display_syntax_prefix_fields(options.getFlag("spf",true)),
      _proportional_subfields(options.getFlag("proportional-subfields",false)),
      _print_location(false),
      _show_unused_instrs(options.getFlag("unused-instrs",true)),
      _full_path(true),
      _expand_exprs(false),
      _hl_level(options.getOption("hl-level",1)),
      _allregs(allregs),
      _regfiles(regfiles)
    {
		};

    GenerateRstBase(const GenerateRstBase &grb) :
      _o(grb._o),
      _parser(grb._parser),
      _doccfg(grb._doccfg),
      _level(grb._level),
      _rf_header(false),
      _reg_header(false),
      _cache_header(false),
      _memory_header(false),
      _eventbus_header(false),
      _extres_header(false),
      _ctx_header(false),
      _genregs(grb._genregs),
      _genrfs(grb._genrfs),
      _genifields(grb._genifields),
      _geninstrs(grb._geninstrs),
      _geniba(grb._geniba),
      _genibb(grb._genibb),
      _genexcepts(grb._genexcepts),
      _gencaches(grb._gencaches),
      _genmems(grb._genmems),
      _genevtbuses(grb._genevtbuses),
      _genextres(grb._genextres),
      _gencontexts(grb._gencontexts),
      _genmmu(grb._genmmu),
      _genclh(grb._genclh),
      _genhelpers(grb._genhelpers),
      _ops_in_affect_tables(grb._ops_in_affect_tables),
      _instr_prefix_bits(grb._instr_prefix_bits),
      _display_syntax_prefix_fields(grb._display_syntax_prefix_fields),
      _proportional_subfields(grb._proportional_subfields),
      _print_location(grb._print_location),
      _show_unused_instrs(grb._show_unused_instrs),
      _full_path(grb._full_path),
      _expand_exprs(grb._expand_exprs),
      _hl_level(grb._hl_level),
      _allregs(grb._allregs),
      _regfiles(grb._regfiles)
    {};

    virtual ~GenerateRstBase() {};

    void printHeader(const gc_string &s,const char *pfx,bool compact)
    {
      const gc_string *anchor = getTargetNoDupl(s);   
      gc_string tmp = quote_string(s);                
      unsigned c = tmp.size() + strlen(pfx)*2;
      _o << '\n';
      if (anchor) {                                   
        _o << ".. _" << *anchor << ":\n";
      }      
      _o << pfx << tmp << pfx << '\n'
         << gc_string(c,Headers.at(_level)) << '\n';  
      if (!(compact)) {      
        _o << '\n';
      }                                         
    }

    void set_print_location(bool p)
    {
      _print_location = p;
    }

    void set_full_path(bool p)
    {
      _full_path = p;
    }

    bool expand_exprs() const
    {
      return _expand_exprs;
    }

    void set_expand_exprs(bool p)
    {
      _expand_exprs = p;
    }

    bool expandExprs(const Instr &instr) const
    {
      static int no_expand = Data::get_attr(DocNoExpandExprs);
      return (_expand_exprs && !instr.has_attr(no_expand));
    }
    

    gc_string quote_string(const gc_string &x)
    {
      gc_string o;
      o.reserve(x.size());

      ForEach(x,i) {
        if (*i == '_') {
          o.push_back('\\');
        }
        o.push_back(*i);
      }

      return o;
    }

    void add_one_nested(const Instr &instr)
    {
      _one_nested.insert(&instr);
    }

    bool is_one_nested(const Instr &instr) const
    {
      return _one_nested.count(&instr);
    }

    // Given a name, returns a target id.  If the name doesn't already have a
    // target id, then one is created for it.
    const gc_string &getTarget(const gc_string &n)
    {
      IfFind(iter,_targets,n) {
        return iter->second.first;
      } else {
        char buf[100];
        sprintf(buf,"adl-internal-target-%d",_target_id++);
        return _targets.insert(make_pair(n,make_pair(gc_string(buf),false))).first->second.first;
      }
    }

    // Same as above, but if the item has already been marked as a header, it
    // returns 0.
    const gc_string *getTargetNoDupl(const gc_string &n)
    {
      IfFind(iter,_targets,n) {
        if (iter->second.second) {
          // Already marked as a header, so return 0.
          return 0;
        } else {
          // Not already used as a header, so mark it and return the target id.
          iter->second.second = true;
          return &(iter->second.first);
        }
      } else {
        char buf[100];
        sprintf(buf,"adl-internal-target-%d",_target_id++);
        return &(_targets.insert(make_pair(n,make_pair(gc_string(buf),true))).first->second.first);
      }
    }

    // Returns a link to a given target identified by s.  Note that we use an
    // anonymous link combiend with an explicit target id in order to avoid
    // conflicts over case and punctuation.
    gc_string makeLink (const gc_string &s)
    {
      return "`" + s + " <#" + getTarget(s) + ">`__";
    }

    // Returns an inline literal version of the string.
    gc_string makeLiteral (const gc_string &s)
    {
      return "``" + s + "``";
    }

    // Returns a link, unless the user has disallowed code creation for this
    // kind of thing.  If use_literal is true, then a non-link is treated as a literal,
    // otherwise the original text is returned.
    gc_string linkIfPossible (const gc_string &s,bool allow,bool use_literal = false)
    {
      if (allow) {
        return makeLink(s);
      } else if (use_literal) {
        return makeLiteral(s);
      } else {
        return s;
      }
    }

    // Print code-block setup information.
    void printCodeSetup()
    {
      gc_string language;
      switch (_hl_level) {
      case 0:
        break;
      case 1:
        language = "cpp";
        break;
      default:
        language = "adl";
        break;
      }
      _o << "\n.. code_block:: " << language << "\n"
         << "   :color:\n\n";
    }

    // Print the source code for a function, with an optional
    // label in front.  If "usageOrNull" is provided, there
    // are also links to any invoked helper functions.  If
    // "body" is true, only the body is printed and not the
    // name or arguments (useful for anonymous "func" items).
    void printFunc(const gc_string &msg,Ptree *value,const UsageInfo *usageOrNull,bool body=true)
    {
      if (value) {
        _o << msg;
        if (_print_location) {
          SourceLocation sl = _parser.getLocation(value);
          string fn = (!_full_path) ? base_name(sl.GetFilename().c_str()) : sl.GetFilename();
          _o << " *(" << fn << ":" << dec << sl.GetLineNo() << ")*";
        }
        if (!msg.empty() || _print_location) {
          _o << ":\n";
        }
        pushindent(2);
        printCodeSetup();
        pushindent(2);
        _o << ((body) ? get_func_body(value) : value) << "\n\n";
        popindent();
        if (NULL != usageOrNull) {
          gc_vector< gc_string > functions;
          ForEach(usageOrNull->_helpers,toHelperName) {
            // Since helper functions are in an appendix,
            // linkIfPossible() wouldn't help here.  This
            // assumes that all function names will become
            // valid headers (link targets).
            gc_string s = linkIfPossible(*toHelperName,_genhelpers,true);
            functions.push_back(s);
          }
          std::sort(functions.begin(),functions.end(),CaseInsensitiveSort());
          if (!functions.empty()) {
            _o << "The code above uses the following routines (directly or indirectly): " << adl::join(functions,", ") << "\n\n";
          }
        }
        popindent();
      }
    }

    void printFunc(const gc_string &msg,const HelperInfo &hi)
    {
      printFunc(msg,hi._func,&hi,false);
    }

    // Print a reStructuredText indented block by first removing
    // the same amount of incidental leading whitespace from each
    // line (preserving any further indentation).  This is useful
    // for avoiding reStructuredText errors just because the user
    // happened to have certain indentation in the ADL source file.
    // The argument "maxline" is passed to unindent_lines().
    void printIndentedBlockWithLabel(const gc_string &label,const gc_string &block,bool blanksep,unsigned maxline=2)
    {
      gc_string unprefixed;
      gc_string quoted;
      unindent_lines(unprefixed,block," \t",maxline);
      indent_lines(quoted,unprefixed,"",true/* first too */);
      if (!quoted.empty()) {
        _o << label << (label.empty() ? "" : ":") << "\n";
        if (blanksep) {
          _o << "\n";
        }
        if (!label.empty()) pushindent(2);
        _o << quoted;
        if (!label.empty()) popindent();
        _o << "\n";
      }
    }

    // Print a literal block by first removing the same
    // amount of incidental leading whitespace from each
    // line (preserving any further indentation).  This
    // is very useful for avoiding reStructuredText errors
    // just because the user happened to have certain
    // indentation in the ADL source file.  The argument
    // "maxline" is passed to unindent_lines().
    // is_code:  If true, treat as a code block, with appropriate syntax highlighting. Otherwise, just
    //           print as a literal.
    void printLiteralBlockWithLabel(const gc_string &label,const gc_string &block,bool is_code,unsigned maxline=2)
    {
      gc_string unprefixed;
      gc_string quoted;
      unindent_lines(unprefixed,block," \t",maxline);
      indent_lines(quoted,unprefixed,"  ",true/* first too */);
      if (!quoted.empty()) {
        if (is_code) {
          _o << label << (label.empty() ? "" : ":") << "\n";
          pushindent(2);
          printCodeSetup();
          pushindent(2);
          _o << quoted << "\n";
          popindent();
          popindent();
        } else {
          _o << label << "::\n\n"
             << quoted
             << "\n";
        }
      }
    }

    void printLiteralBlock(const gc_string &block,bool is_code,unsigned maxline=2)
    {
      printLiteralBlockWithLabel("",block,is_code,maxline);
    }

    // Push and pop header levels.
    void pushlevel()
    {
      ++_level;
      assert(_level < (int)Headers.size());
    }

    void poplevel()
    {
      --_level;
      assert(_level >= 0);
    }

    // Push and pop indent levels on the prefix stream.
    void pushindent(int c)
    {
      _indents.push_back(c);
      _o.addindent(c);
    }

    void popindent()
    {
      if (!_indents.empty()) {
        int c = _indents.back();
        _indents.pop_back();
        _o.subindent(c);
      }
    }
  
    void nameHeader(const gc_string &pfx,const SysItem &item)
    {
      gc_string name = pfx + " " + item.name();
      if (!item.type().empty()) {
        name += " (" + item.type() + ")";
      }
      Header(name);
    }

    void condHeader(bool &flag,const gc_string &header)
    {
      if (!flag) {
        Header(header);
        flag = true;
      }
    }

    struct FieldInfo;

    struct FipSort {
      bool operator()(const FieldInfo *x,const FieldInfo *y) {
        return (x->_lo < y->_lo);
      }
    };
    
    // This creates an m-ary tree of FieldInfo objects.
    struct FieldInfos : public gc_set<FieldInfo *,FipSort> {
      // Adds an item to a sublist if it's contained within a sub-item, otherwise adds
      // to the current level.
      void add(FieldInfo *fi) 
      {
        if (!add_if_contains(fi)) {
          insert(fi);
        }
      }

      // Adds an item to a subitem if it finds a containing sub-item, else returns false.
      bool add_if_contains(FieldInfo *fi) 
      {
        ForEach((*this),i) {
          if ((**i).add_if_contains(fi)) {
            return true;
          }
        }
        return false;
      }
    };
  
    // Used for creating an instruction or register field description.
    struct FieldInfo : public gc {
      int        _lo;  // Lowest bit.  Display order may make it leftmost or rightmost.
      int        _hi;  // Highest bit.
      gc_string     _s;
      bool       _rev; // Whether to reverse the gc_string when displayed.  Use only if "_s" shows bits.
      bool       _r;   // Whether or not the range is reserved.
      FieldInfos _si;

      FieldInfo(unsigned l,unsigned h,bool rev) :
                _lo(std::min(l, h)), _hi(std::max(h, l)), _rev(rev), _r(false) {};
      FieldInfo(unsigned l,unsigned h,const gc_string &s,bool rev,bool r = false) :
               _lo(std::min(l, h)), _hi(std::max(h, l)), _s(s), _rev(rev), _r(r) {};

      bool multibit() const { return _lo != _hi; };
      int width() const { return (_hi - _lo + 1); };
      bool contains(const FieldInfo &x) const { return (x._lo >= _lo && x._hi <= _hi); };
      bool intersects(int l,int h) const { return !(h < _lo || l > _hi); };

      unsigned label_width() const { return (_r) ? 0 : _s.size(); };

      bool add_if_contains(FieldInfo *fi)
      {
        if (!contains(*fi)) {
          return false;
        } else if (_si.add_if_contains(fi)) {
          ; // Was added to sub-list.
        } else {
          // We contain it but it's not contained by sub-items, so add to this level.
          _si.insert(fi);
        }
        return true;
      }

    };

    // Used for managing opcode mappings.
    typedef gc_multimap<gc_string, const Instr*> AliasByInstrName;

    // Information on entire families of instructions (sets of aliases).
    // This contains display-formatted gc_strings, not "raw" data.
    struct InstrFamilyInfo : public gc {
      const GenerateRstBase &_base;
      const Instr& _instr;
      gc_vector< const Instr* > _aliases;
      bool _hasAliases;
      bool _allAliasesNested;
      bool _oneBlock; // every instruction is in the same block
      bool _oneNested; // family consists of a single nested alias
      bool _oneTotal; // family consists of a single alias, period
      gc_string _displayName; // for single-alias families, matches the name of the first alias
      
      typedef gc_hash_map< gc_string, gc_string > NewNameByOldName;
      static NewNameByOldName& RenamedInstrs() { static NewNameByOldName x; return x; }
      
      InstrFamilyInfo(const Instr &instr,GenerateRstBase &base,const AliasByInstrName &aliasesByInstr)
        : _base(base), _instr(instr), _aliases(), _hasAliases(false), _allAliasesNested (false), _oneBlock(false),
        _oneNested(false), _oneTotal(false), _displayName()
      {
        auto aliases = aliasesByInstr.equal_range(instr.name());
        gc_hash_set< gc_string > uniqueSyntaxes;
        bool allAliasesNested = true;
        for (auto toAliasByInstrName = aliases.first; toAliasByInstrName != aliases.second; ++toAliasByInstrName) {
          auto const &aliasInstr = toAliasByInstrName->second;
          _aliases.push_back(aliasInstr);
          _hasAliases = true;
          if (aliasInstr->is_nested()) {
            uniqueSyntaxes.insert(_base.getInstructionTitle(*aliasInstr,false/* include block */));
          } else {
            allAliasesNested = false;
          }
        }
        _allAliasesNested = (_hasAliases && allAliasesNested);
        _oneBlock = (1 == instr.blks().size());
        _displayName = _base.getInstructionTitle(instr);
        bool isCustomParentName = (instr.name() != instr.docName());
        if (_hasAliases) {
          const Instr *first = NULL;
          if (1 == _aliases.size()) {
            _oneTotal = true;
            first = _aliases.front();
            _oneNested = first->is_nested();
            base.add_one_nested(instr);
          }
          if (!isCustomParentName) {
            if (_oneNested) {
              // copy the name of any single nested alias (#498);
              // remember this mapping so that any links are also renamed
              gc_string newName = _base.getInstructionTitle(*first);
              RenamedInstrs().insert(std::make_pair(_displayName, newName));
              _displayName = newName;
            } else if (1 == uniqueSyntaxes.size()) {
              // when all nested instructions have the same syntax (title),
              // ignoring any block, the default family name should match (#498)
              gc_string newName = GenerateRstBase::addBlocksToInstructionTitle(*(uniqueSyntaxes.begin()),instr);
              RenamedInstrs().insert(std::make_pair(_displayName, newName));
              _displayName = newName;
            }
          }
        }
      }
    };

    // Return true if the instruction does not appear to be used
    // and should therefore be omitted from reference lists.
    static inline bool isInstructionUnused(const Instr &instr) {
      // see similar logic in Instr::print_intro() in "fe/Data.C"
      return (!instr.is_used() && !instr.is_pseudo() && instr.get_nested().empty());
    }

    // Determine the name of the section for an instruction.
    // The optional flag indicates whether or not to include
    // the block name at the end.
    gc_string getInstructionTitle(const Instr &instr,bool block=true) const
    {
      gc_ostringstream os;
      if (instr.docName() != instr.name()) {
        os << instr.docName();
      } else if (const Instr::Syntax *syntax = instr.syntax()) {
        auto fiter = syntax->_fmt.begin();
        if (!_display_syntax_prefix_fields) {
          fiter += syntax->_in_begin;
        }
        for( ; fiter != syntax->_fmt.end(); ++fiter) {
          if (syntax->isfield(*fiter)) {
            auto field = syntax->get(*fiter);
            os << getInstrFieldRefString(field,(syntax->isPfield(*fiter) && !syntax->isPfxfield(*fiter)));
          } else {
            os << *fiter;
          }
        }
      } else {
        os << instr.name();
        if (instr.sizeFields()) {
          os << ' ';
        }
        bool first = true;
        ForRange(instr.sizeFields(),i) {
          list_sep(os,first);
          instr.getField(i).dump(os,true);
        }
      }
      if (!instr.docNameAdd().empty()) {
        os << nbsp << instr.docNameAdd();
      }
      gc_string result = os.str();
      if (block) {
        result = addBlocksToInstructionTitle(result,instr);
      }
      if (InstrFamilyInfo::RenamedInstrs().end() != InstrFamilyInfo::RenamedInstrs().find(result)) {
        result = InstrFamilyInfo::RenamedInstrs()[result];
      }
      return result;
    }

    // For UTF-8-encoded streams, writes a non-breaking-space
    // character.  This is very useful for forcing things to
    // stick together, or to take up more space than they would
    // otherwise (because space is normally compressed).
    //
    // Note that a method in iostreams allows any function that
    // takes and returns a stream to act as a manipulator, so
    // you can simply say: "os << nbsp << ..." to call this.
    static std::ostream& nbsp(std::ostream &os)
    {
      char nbspChars[] = {(char)0xC2, (char)0xA0}; // UTF-8 non-breaking space
      os.write(nbspChars, sizeof(nbspChars));
      return os;
    }

    // When generating "raw" HTML instead of reStructuredText,
    // and the desire is to have gc_strings that do not wrap, use
    // this helper routine to convert raw spaces into non-
    // breaking spaces.
    static gc_string nbspify(gc_string const& s)
    {
      gc_string result;
      ForEach(s,c) {
        if (' ' == *c) {
          result += nbsp_str;
        } else {
          result += *c;
        }
      }
      return result;
    }

    // Returns a new gc_string that has the specified instruction's
    // block information appended.  Usually, you just call
    // getInstructionTitle() to do this, but if you have a reason
    // to determine the standard block appendage for a title, you
    // can call this separately.
    static gc_string addBlocksToInstructionTitle(const gc_string &title,const Instr &instr)
    {
      gc_ostringstream os;
      os << title;
      if (false == instr.get_blk().empty()) {
        os << " " << nbsp << nbsp << "(";
        if (!instr.blks().empty()) {
          os << adl::join(instr.blks(),", ");
        } else {
          os << instr.get_blk();
        }
        os << ")";
      }
      gc_string result = os.str();
      return result;
    }

    // Append copies of all the values in a map, to a vector.
    // If "noHidden" is true, the "doc_hidden" attribute is also
    // checked, and any object with this attribute is registered
    // as hidden and not included in the final list.
    template < typename key_t, typename value_t >
    static void extractValues(const gc_map< key_t, value_t > &kvp,gc_vector< value_t > &values,bool noHidden=true)
    {
      static int hidden = Data::get_attr(DocHidden);
      values.reserve(kvp.size());
      ForEach(kvp,toPair) {
        auto &value = toPair->second;
        bool keep = true;
        if ((noHidden) && value->has_attr(hidden)) {
          hideObject(*value);
          keep = false;
        }
        if (keep) {
          values.push_back(value);
        }
      }
    }

    // Given a typical map of gc_strings to some type, constructs a
    // case-insensitively-sorted list of the keys.  Useful when an
    // original map allows case-sensitive keys, but is presented in
    // a case-insensitive order.
    template < typename by_name_hash_t >
    static void sortedNames(const by_name_hash_t &kvp,gc_vector< gc_string > &sorted)
    {
      sorted.reserve(kvp.size());
      ForEach(kvp,toPair) {
        auto &name = toPair->first;
        sorted.push_back(name);
      }
      std::sort(sorted.begin(),sorted.end(),CaseInsensitiveSort());
    }

    typedef gc_multimap<gc_string, const InstrField*> ChildrenByInstrFieldName;

    // Information on instruction fields that combine to
    // implement one field interface.
    struct InstrFieldNestingInfo : public gc {
      const InstrField& _ifield;
      const InstrField* _valuesField; // the field whose table() or reference determines permitted values
      gc_vector< const InstrField* > _children;
      
      InstrFieldNestingInfo(const InstrField &ifield,const ChildrenByInstrFieldName &childrenByInstrField)
      : _ifield(ifield), _valuesField(&ifield), _children()
      {
        auto children = childrenByInstrField.equal_range(ifield.name());
        for (AdlTypeof(children.first) toChildByFieldName = children.first;
             toChildByFieldName != children.second; ++toChildByFieldName) {
          auto const &field = toChildByFieldName->second;
          if (!field->table().empty()) {
            _valuesField = field;
          } else {
            _children.push_back(field);
          }
        }
      }
    };

    static gc_string getInstrFieldSyntax(const InstrField &field)
    {
      gc_ostringstream os;
      gc_string result;
      auto const &syntax = field.syntax();
      if (NULL != syntax) {
        ForEach(syntax->_fmt,i) {
          if (syntax->isfield(*i)) {
            if (syntax->get(*i)._name) {
              os << syntax->get(*i)._name;
            }
          } else {
            os << *i;
          }
        }
        result = os.str();
      }
      return result;
    }

    static gc_string getInstrFieldTitle(const InstrField &field)
    {
      gc_ostringstream os;
      const InstrField *parentField = field.encap_field();
      if (NULL != parentField) {
        os << parentField->name();
        os << ".";
      }
      os << field.name();
      if (field.has_doc_name()) {
        os << " (" << field.docName() << ")";
      }
      return os.str();
    }

    // Determine appropriate gc_string value for a possible reference to an
    // instruction field (e.g. in an instruction's syntax gc_string).
    static gc_string getInstrFieldRefString(const Instr::Field &fieldInfo,bool is_perm)
    {
      gc_ostringstream os;
      gc_string result;
      auto const &field = fieldInfo._field;
      if (field->is_enumerated()) {
        size_t const numenums = field->enums().size();
        // For enumerated fields, list the options here
        // if there are just a few.
        if (numenums > 3/* arbitrary */) {
          if (is_perm) {
            os << '{' << fieldInfo << '}';
          } else {
            os << fieldInfo;
          }
        } else {
          if (numenums != 1) {
            os << "(";
          }
          bool first = true;
          ForEach(field->enums(),j) {
            if (j->first == ReservedEnum) {
              continue;
            }
            pfx_sep(os,"/",first);
            os << j->first;
            first = false;
          }
          if (numenums != 1) {
            os << ")";
          }
        }
        result = os.str();
      } else if (NULL != field->syntax()) {
        result = getInstrFieldSyntax(*field);
      } else {
        fieldInfo.dump(os,true);
        result = os.str();
      }
      return result;
    }

    // Returns the set of attributes that only has
    // meaning to the documentation generator, and
    // therefore should not be included in documentation.
    gc_set< unsigned int > getDocAttributes()
    {
      gc_set< unsigned int > result;
      // IMPORTANT: List all documentation-related attributes here.
      result.insert(Data::get_attr(DocHidden));
      result.insert(Data::get_attr(DocNoCode));
      result.insert(Data::get_attr(DocNoExpandExprs));
      return result;
    }

    // Writes an Attributes section if any exist for
    // the specified object (note that many common
    // objects inherit indirectly from Attrs, so many
    // inputs are valid here).
    void writeAttributes(Attrs &object)
    {
      gc_vector< gc_string > attrs;
      ForEach(object.attrs(),toParamsById) {
        auto skip = getDocAttributes();
        if (skip.end() == skip.find(toParamsById->first)) {
          attrs.push_back(Data::get_attr_name(toParamsById->first));
        }
      }
      if (!attrs.empty()) {
        _o << "\nAttributes:\n";
        pushindent(2);
        _o << adl::join(attrs, ", ") << "\n";
        popindent();
        _o << "\n";
      }
    }

    // Writes an Attributes section if any exist for
    // the specified instruction family.
    void writeAttributes(const InstrFamilyInfo &family)
    {
      gc_set< gc_string > attrs;
      // find unique attributes across all instructions in the family
      ForEach(family._instr.attrs(),toParamsById) {
        auto skip = getDocAttributes();
        if (skip.end() == skip.find(toParamsById->first)) {
          attrs.insert(Data::get_attr_name(toParamsById->first));
        }
      }
      ForEach(family._aliases,toInstr) {
        ForEach((*toInstr)->attrs(),toParamsById) {
          auto skip = getDocAttributes();
          if (skip.end() == skip.find(toParamsById->first)) {
            attrs.insert(Data::get_attr_name(toParamsById->first));
          }
        }
      }
      if (!attrs.empty()) {
        _o << "\nAttributes:\n";
        pushindent(2);
        _o << adl::join(attrs, ", ") << "\n";
        popindent();
        _o << "\n";
      }
    }

    struct CaseInsensitiveSort: public std::binary_function< gc_string,gc_string,bool > {
      bool operator() (const gc_string &a,const gc_string &b) {
        gc_string sa(a);
        gc_string sb(b);
        std::transform(sa.begin(),sa.end(),sa.begin(),::tolower);
        std::transform(sb.begin(),sb.end(),sb.begin(),::tolower);
        return (strcmp(sa.c_str(),sb.c_str()) < 0);
      }
    };

    struct NamedCaseInsensitiveSort: public std::binary_function< Named,Named,bool > {
      NamedCaseInsensitiveSort (bool docName=true): _useDocName(docName) {}
      bool operator() (const Named *a,const Named *b) {
        gc_string sa((_useDocName) ? a->docName() : a->name());
        gc_string sb((_useDocName) ? b->docName() : b->name());
        return CaseInsensitiveSort()(sa.c_str(),sb.c_str());
      }
    private:
      bool _useDocName;
    };

    struct InstrFieldTitleSort: public std::binary_function< const InstrField*,const InstrField*,bool > {
      bool operator() (const InstrField* const &a,const InstrField* const &b) {
        gc_string sa = getInstrFieldTitle(*a);
        gc_string sb = getInstrFieldTitle(*b);
        return CaseInsensitiveSort()(sa.c_str(),sb.c_str());
      }
    };

    struct InstrTitleSort: public std::binary_function< const Instr*,const Instr*,bool > {
      InstrTitleSort(GenerateRstBase &b) : _b(b) {};
      bool operator() (const Instr* const &a,const Instr* const &b) {
        gc_string sa = _b.getInstructionTitle(*a);
        gc_string sb = _b.getInstructionTitle(*b);
        return CaseInsensitiveSort()(sa.c_str(),sb.c_str());
      }
      const GenerateRstBase &_b;
    };

    typedef gc_vector< const Instr* > InstrsWithAttr;

    // Writes a section with all attributes that are
    // used by at least one instruction, and lists
    // the instructions that have each attribute.
    // If no instructions have attributes, this call
    // has no effect.
    void genInstrAttributesAppendix(const InstrHash *instrs)
    {
      auto const &allAttrs = Data::get_attrs();
      auto skippedAttrs = getDocAttributes();
      bool areAny = false;
      ForEach(allAttrs,toAttrByName) {
        InstrsWithAttr instrsWithAttr;
        if (skippedAttrs.end() != skippedAttrs.find(toAttrByName->second._id)) {
          continue;
        }
        ForEach((*instrs),i) {
          auto &instr = *(i->second);
          // Skip if unused or if this top-level item is an alias.
          if (isInstructionUnused(instr) || instr.is_alias()) {
            continue;
          }
          if (instr.has_attr(toAttrByName->second._id)) {
            instrsWithAttr.push_back(&instr);
          } else {
            // Check sub-instructions, regardless of usage or alias, and add this
            // instruction if it's found.
            ForEach(instr.get_nested(),ni) {
              auto &ninstr = *(ni->second);
              if (ninstr.has_attr(toAttrByName->second._id)) {
                instrsWithAttr.push_back(&instr);
                break;
              }            
            }
          }
        }
        std::sort(instrsWithAttr.begin(),instrsWithAttr.end(),InstrTitleSort(*this));
        if (!instrsWithAttr.empty()) {
          if (!areAny) {
            Header("Instructions by Attribute");
            pushlevel();
            areAny = true;
          }
          IHeader(toAttrByName->first);
          pushlevel();
          
          ForEach(instrsWithAttr,i) {
            _o << "* " << linkIfPossible(getInstructionTitle(**i),_geninstrs) << "\n";
          }
          
          poplevel();
        }
      }
      if (areAny) {
        poplevel();
      }
    }

    // Writes a section with all of the units that
    // implement instructions, and lists the instructions
    // in each block.  If no blocks are specified, this
    // call has no effect.
    void genInstrBlocksAppendix(const InstrHash *instrs,const Core &core)
    {
      auto const &blocks = core.blocks();
      if (blocks.size() > 1) {
        bool areAny = false;
        ForEach(blocks,toBlockName) {
          gc_vector< const Instr* > instrsWithBlock;
          ForEach((*instrs),i) {
            auto &instr = *(i->second);
            // Skip if singly nested- we'll use the child instead for the link.
            if (isInstructionUnused(instr) || is_one_nested(instr)) {
              continue;
            }
            ForEach(instr.blks(),toIBlk) {
              if (*toIBlk == *toBlockName) {
                instrsWithBlock.push_back(&instr);
              }
            }
          }
          std::sort(instrsWithBlock.begin(),instrsWithBlock.end(),InstrTitleSort(*this));
          if (!instrsWithBlock.empty()) {
            if (!areAny) {
              Header("Instructions by Block");
              pushlevel();
              areAny = true;
            }
            IHeader("Unit \"" << *toBlockName << "\"");
            pushlevel();
            
            ForEach(instrsWithBlock,i) {
              _o << "* " << linkIfPossible(getInstructionTitle(**i),_geninstrs) << "\n";
            }
            
            poplevel();
          }
        }
        if (areAny) {
          poplevel();
        }
      }
    }

    // Create a padding gc_string so that the text in the column will be
    // proportional in length wrt. the smallest label in the row.
    // minw:  Min width of any column.
    // minlw: Min label width (width of gc_string in the column).
    gc_string createPadding(const FieldInfo &fi,double width,double minw,unsigned minlw)
    {
      if (_proportional_subfields) {
        double mulf = width / minw;
        if (mulf > 1 ) {
          // We need to pad if this column should be larger than the minimum.  We
          // multiply the multiply-factor by the min.  label width to get the
          // padding size.  We subtract out our own label and then divide by two
          // to create the padding for either side of the label.
          unsigned p = (unsigned)(((mulf * minlw) - fi.label_width()) / 2);
          if (p) {
            gc_string x;
            ForRange(p,i) {
              x += nbsp_str;
            }
            return x;
          }
        }
      }
      return gc_string();
    }

    // If we care about proportional padding, then figure out the padding factor.
    void calculatePadding(unsigned &minlw,double &minw,const FieldInfo &fi,const gc_vector< FieldInfo* > &orderedSubInfos)
    {
      minlw = (unsigned)-1;
      minw = 100.0;
      if (_proportional_subfields) {
        ForEach(orderedSubInfos,i) {
          FieldInfo &x = **i;
          // Find minimum label width.  Add in approximation for the
          // size of indices.
          minlw = min(minlw,x.label_width() + (x.multibit() ? 5 : 3));
          // Find minimum column width.
          double w = ((double)(x.width() / (double)fi.width()))*100;
          minw = min(minw,w);
        }       
      }
    }

    // Writes a table of subfields, if any exist.
    // All fields are assumed to be in order of lowest-bit-range,
    // regardless of output order; "lowestFirst" can be set to
    // false to change the output order.
    void writeSubFields(FieldInfo &fi,bool lowestFirst)
    {
      if (!fi._si.empty()) {
        // Insert reserved fields for any gaps in the list.
        int last = fi._lo-1;
        ForEach(fi._si,i) {
          FieldInfo &x = **i;
          if (x._lo > (last+1)) {
            fi._si.add(new FieldInfo(last+1,x._lo-1,uitogs(0),false,true));
          }
          last = x._hi;
        }
        // Handle any trailing reserved bits.
        if (last < (fi._lo+fi.width()-1)) {
          fi._si.add(new FieldInfo(last+1,fi._lo+fi.width()-1,uitogs(0),false,true));
        }
        
        // Figure out which way to traverse.
        gc_vector< FieldInfo* > orderedSubInfos;
        if (lowestFirst) {
          std::copy(fi._si.begin(), fi._si.end(), std::back_inserter(orderedSubInfos));
        } else {
          std::copy(fi._si.rbegin(), fi._si.rend(), std::back_inserter(orderedSubInfos));
        }

        unsigned minlw;
        double minw;
        calculatePadding(minlw,minw,fi,orderedSubInfos);

        _o << "<table class=\"subfield\"><tr>\n";
        ForEach(orderedSubInfos,i) {
          FieldInfo &x = **i;
          if (x._r) {
            _o << "<td class=\"subfieldunused\" title=\"Reserved\"";
          } else {
            _o << "<td class=\"subfield\"";
          }
          double w = ((double)(x.width() / (double)fi.width()))*100;
          if (w > 0) {
              _o << " width=\"" << w << "%\"";
          }
          _o << ">";
          gc_string padding = createPadding(x,w,minw,minlw);
          _o << padding;
          if (!x._r) {
            _o << nbspify(x._s);
          }
          if (x.multibit()) {
            int& lo = (lowestFirst) ? x._lo : x._hi;
            int& hi = (lowestFirst) ? x._hi : x._lo;
            _o << "[" << lo << "," << hi << "]";
          } else {
            _o << "[" << x._lo << "]";
          }
          _o << padding;
          writeSubFields(**i,lowestFirst);
          _o << "</td>\n";
        }
        _o << "</tr></table>\n";
      }
    }

    gc_string writeConstStr(int count,uint8_t c)
    {
      gc_ostringstream os;
      for (int i = 0; i != count; ++i) {
        // for readability, arbitrarily split every 4 bits
        if (i && ((i % 4) == 0)) {
          os << " / ";
        }
        os << (int)c;
      }
      return os.str();
    }

    // Writes a graphical depiction of fields, given a FieldInfos object.  This
    // will sort the list and insert reserved fields in any gaps, so you don't
    // have to do that before calling it.
    //
    // The "lowestFirst" option indicates the display order: the fields should
    // always be provided in order of lowest-bit-range, but this allows the
    // final table to render fields backwards.
    void writeFields(FieldInfos &infos,int width,bool lowestFirst,bool write_rsrv_value,int reserved_value)
    {
      // Insert reserved fields for any gaps in the list.
      int last = -1;
      ForEach(infos,i) {
        FieldInfo &fi = **i;
        if (fi._lo > (last+1)) {
          infos.add(new FieldInfo(last+1,fi._lo-1,writeConstStr((fi._lo-last-1),reserved_value),false,true));
        }
        last = fi._hi;
      }
      // Handle any trailing reserved bits.
      if (last < width-1) {
        infos.add(new FieldInfo(last+1,width-1,writeConstStr((width-last-1),reserved_value),false,true));
      }

      // Figure out which way to traverse.
      gc_vector< FieldInfo* > orderedInfos;
      if (lowestFirst) {
        std::copy(infos.begin(), infos.end(), std::back_inserter(orderedInfos));
      } else {
        std::copy(infos.rbegin(), infos.rend(), std::back_inserter(orderedInfos));
      }

      // Now create the table.
      _o << "\n.. raw:: html\n\n";
      pushindent(3);
      _o << "<table class=\"field\">\n<tr>";
      // Create the first row.
      ForEach(orderedInfos,i) {
        FieldInfo &fi = **i;
        _o << "<td";
        if (fi.multibit()) {
          _o << " colspan=\"2\"";
        }
        double w = ((double)(fi.width() / (double)width))*100;
        if (w > 0) {
          _o << " width=\"" << w << "%\"";
        }
        if (fi._r && !write_rsrv_value) {
          // Do not actually display the "value" (0) of a reserved field;
          // the title attribute creates a tooltip in most browsers
          _o << " class=\"reserved\" title=\"Reserved\">";
        } else {
          gc_string s = fi._s;
          if ((!lowestFirst) && (fi._rev)) {
            // Certain field gc_strings represent values that should
            // be completely reversed when printing fields in the
            // opposite order.  This applies to bit gc_strings.
            std::reverse(s.begin(),s.end());
          }
          // Convert spaces to non-breaking spaces for a more cohesive
          // tabular display.
          s = nbspify(s);
          if (fi._r) {
            _o << " class=\"reserved\" title=\"Reserved\">" << s;
          } else {
            _o << " class=\"field\">" << s;
          }
        }
        // If we have sub-items, then recursively create a table to store them.  We do not, however,
        // print their indices.
        writeSubFields(fi,lowestFirst);
        _o << "</td>\n";
      }
      _o << "</tr>\n"
         << "<tr>\n";
      // List bit ranges.
      bool odd = true;
      ForEach(orderedInfos,i) {
        FieldInfo &fi = **i;
        if (fi.multibit()) {
          int& lo = (lowestFirst) ? fi._lo : fi._hi;
          int& hi = (lowestFirst) ? fi._hi : fi._lo;
          _o << "<td class=\"left" << ((odd) ? "odd" : "even")
             << "\">" << lo << "</td><td class=\"right" << ((odd) ? "odd" : "even")
             << "\">" << hi << "</td>\n";
        } else {
          _o << "<td class=\"single" << ((odd) ? "odd" : "even")
             << "\">" << fi._lo << "</td>\n";
        }
        odd = !odd;
      }
      _o << "</tr>\n"
         << "</table>\n\n";
      popindent();
    }

    // Writes a table of permitted values for the specified instruction field.
    void writePermittedValues(const InstrField &field)
    {
      // Generate a table for allowed values, if one exists.
      // Also check for simple register file references.
      const RegFile *rf = field.regfile();
      if (!field.table().empty()) {
        gc_string rfname = (rf) ? rf->prefix() : "";
        _o << "Permitted values:\n";
        pushindent(2);
        _o << ".. list-table::\n"
           << "   :class: attributes\n"
           << "   :widths: 10 8\n"
           << "   :header-rows: 1\n\n";
        pushindent(3);
        _o << "* - Values\n"
           << "  - Encoding\n";
        unsigned int lookup = 0;
        ForEach(field.table().entries(),entry) {
          auto &indices = *entry;
          _o << "* - ";
          ForEach(indices,ridx) {
            _o << rfname << *ridx << " ";
          }
          _o << "\n";
          _o << "  - ";
          rnumber::RNumber bits(lookup, field.width());
          bits.printToOS(_o, rnumber::RNumber::rbin);
          _o << "\n";
          ++lookup;
        }
        popindent();
        popindent();
      } else if (NULL != rf) {
        gc_string rfname = rf->prefix();
        _o << "Permitted values:\n";
        pushindent(2);
        _o << rfname << "0 - " << rfname << ((1L << field.width()) - 1) << "\n";
        popindent();
        _o << "\n";
      }
    }

    // Generate instruction field documentation.
    void genInstrFields(const InstrFieldHash *ifields,const Core &core)
    {
      if (!ifields || ifields->empty()) {
        return;
      }

      // It is necessary to do a first pass to extract sub-fields,
      // so that they can be mapped to their parent fields and
      // presented hierarchically.
      gc_hash_set< gc_string > seenFields;
      ChildrenByInstrFieldName childrenByInstrField;
      ForEach((*ifields),i) {
        auto &possibleNestedField = i->second;
        gc_string fieldName = getInstrFieldTitle(*possibleNestedField);
        // Do not generate the same field more than once.
        // FIXME: Figure out why it is possible for identical
        // fields to appear in the given hash.
        if (seenFields.end() != seenFields.find(fieldName)) {
          continue;
        }
        auto const &parentField = possibleNestedField->encap_field();
        if (NULL != parentField) {
          childrenByInstrField.insert(std::make_pair(parentField->name(),possibleNestedField));
        }
        seenFields.insert(fieldName);
      }

      Header("Instruction Fields");
      pushlevel();

      gc_vector< InstrField* > initialFields;
      extractValues(*ifields,initialFields,true/* ignore hidden */);
      gc_vector< InstrField* > sortedInstrFields;
      sortedInstrFields.reserve(initialFields.size());
      ForEach(initialFields,i) {
        auto &field = *i;
        // Skip fields that are automatically-generated.
        // The is_generated() flag is set for things that look like
        // "VarInstrOpcode_imp_bits...", and the field pointer will
        // be of type InstrFieldImpl* for all other things that
        // look like "<name>_imp_bits...".
        // FIXME: shouldn't is_generated() be made to return true
        // whenever an InstrFieldImpl* is in use?
        const InstrFieldImpl *fieldImpl = dynamic_cast<const InstrFieldImpl *>(field);
        if (field->is_generated() || (NULL != fieldImpl)) {
          continue;
        }
        // Nested fields are not part of the main field list.
        if (NULL != field->encap_field()) {
          continue;
        }
        sortedInstrFields.push_back(field);
      }
      std::sort(sortedInstrFields.begin(),sortedInstrFields.end(),InstrFieldTitleSort());

      bool generateAlphabet = (ifields->size() > 5/* arbitrary */);
      AlphabetIndex ai(*this,"ifield-section");
      if (generateAlphabet) {
        // find all necessary letters, and generate a set of alphabet links
        ForEach(sortedInstrFields,i) {
          auto &field = *i;
          gc_string fieldName = getInstrFieldTitle(*field);
          int firstLetter = std::tolower(fieldName[0]);
          if (ai._letters.end() == ai._letters.find(firstLetter)) {
            ai._letters.insert(firstLetter);
          }
        }
        ai.writeIndex();
      }
      
      ForEach(sortedInstrFields,toField) {
        auto &field = *(*toField);
        InstrFieldNestingInfo family(field,childrenByInstrField);
        gc_string fieldName = getInstrFieldTitle(field);
        if (generateAlphabet) {
          // generate anchor for each new letter of the alphabet
          int firstLetter = std::tolower(fieldName[0]);
          ai.writeTarget(firstLetter);
        }
        IHeader(fieldName);
        pushlevel();
        // List some characteristics of the field.
        // Lines are first composed as a list so that the
        // heading can be omitted when there are no fields.
        gc_vector< gc_string > lines;
        if (field.size()) {
          lines.push_back("* - Size\n");
          gc_ostringstream os;
          gc_string s;
          os << "  - " << field.size() << "\n";
          s = os.str();
          lines.push_back(s);
        }
        if (field.shift()) {
          lines.push_back("* - Shift\n");
          gc_ostringstream os;
          gc_string s;
          os << "  - " << field.shift() << "\n";
          s = os.str();
          lines.push_back(s);
        }
        if (field.offset()) {
          lines.push_back("* - Offset\n");
          gc_ostringstream os;
          gc_string s;
          os << "  - " << field.offset() << "\n";
          s = os.str();
          lines.push_back(s);
        }        
        if (!field.bits().empty()) {
          lines.push_back("* - Bits\n");
          gc_ostringstream os;
          gc_string s;
          auto &bits = field.bits();
          os << "  - ";
          ForEach(bits,b) {
            os << *b;
            os << ' ';
          }
          os << "\n";
          s = os.str();
          lines.push_back(s);
        }
        if (field.addr() == InstrField::aPcRel) {
          lines.push_back("* - Addressing\n");
          lines.push_back("  - PC-Relative\n");
        } else if (field.addr() == InstrField::aAbs) {
          lines.push_back("* - Addressing\n");
          lines.push_back("  - Absolute\n");
        } else {
          // None
        }
        if (field.is_signed()) {
          lines.push_back("* - Signed\n");
          lines.push_back("  - True\n");
        }
      #if 0
        // requested in #498 - don't show this
        if (field.is_pseudo()) {
          lines.push_back("* - Pseudo\n");
          lines.push_back("  - True\n");
        }
      #endif
        if (field.is_alias()) {
          InstrField::Alias *alias = field.get_alias();
          if (NULL == alias) {
            lines.push_back("* - Alias\n");
            lines.push_back("  - True\n");
          } else {
            lines.push_back("* - Alias\n");
            gc_ostringstream os;
            gc_string s;
            os << "  - " << alias->_target_name << "\n";
            s = os.str();
            lines.push_back(s);
          }
        }
        if (field.is_prefix()) {
          lines.push_back("* - Prefix\n");
          lines.push_back("  - True\n");
        }
        if (field.is_unsigned_upper_bound()) {        //
          lines.push_back("* - Unsigned Upper Bound\n");
          lines.push_back("  - True\n");
        }              
        if (field.index()) {
          lines.push_back("* - Index\n");
          gc_ostringstream os;
          gc_string s;
          os << "  - " << field.index() << "\n";
          s = os.str();
          lines.push_back(s);
        }
        gc_string relocName = !field.relocs().empty() ? field.relocs().front()->name() : gc_string();
        if (false == relocName.empty()) {
          gc_string s;
          lines.push_back("* - Relocation Name\n");
          {
            gc_ostringstream os;
            os << "  - " << relocName << "\n";
            s = os.str();
            lines.push_back(s);
          }
          const Relocation *reloc = core.get_reloc(relocName);
          if (NULL != reloc) {
            
            if (reloc->width() !=  -1) {
              lines.push_back("* - Relocation Width\n");
              {
                gc_ostringstream os;
                os << "  - " << reloc->width() << "\n";
                s = os.str();
                lines.push_back(s);
              }
            }
            lines.push_back("* - Relocation Value\n");
            {
              gc_ostringstream os;
              os << "  - " << reloc->value() << "\n";
              s = os.str();
              lines.push_back(s);
            }
            if (reloc->right_shift() != -1) {
              lines.push_back("* - Relocation Value Right Shift\n"); // Specifies the number of bits the relocation value is right-shifted before it is encoded
              {
                gc_ostringstream os;
                os << "  - " << reloc->right_shift() << "\n";  
                s = os.str();
                lines.push_back(s);
              }         
            }
          }
        }
        
        if (field.instrtable()) {   //
          lines.push_back("* - Instruction Table\n");
          gc_ostringstream os;
          gc_string s;
          // os << "  - " << linkIfPossible(field.instrtablename(), _geninstrs && _geniba) << "\n";
          os << "  - " << field.instrtablename() << "\n";
          s = os.str();
          lines.push_back(s);
        }
        
        if (!lines.empty()) {
          _o << "\n";
          _o << ".. list-table::\n"
             << "   :class: attributes\n"
             << "   :widths: 10 8\n"
             << "   :header-rows: 0\n\n";
          pushindent(3);
          ForEach(lines,l) {
            _o << l->c_str();
          }
          popindent();
          _o << "\n";
        }
        gc_string doc(field.doc());
        if (!doc.empty()) {
          printIndentedBlockWithLabel("Description",doc,true/* blank line after label */);
        }
        // Generate a table for enumerations, if we're enumerated.
        // Warning: ".. raw:: html" line must not be indented at all.
        if (field.is_enumerated()) {
          _o << ".. raw:: html\n\n";
          pushindent(3);
          _o << "<table class=\"enumeratedfield\">\n"
             << "<tr>\n"
             << "  <th>Enumeration</th>\n"
             << "  <th>Value</th>\n"
             << "</tr>\n";
          ForEach(field.enums(),j) {
            _o << "<tr><td>" << j->first << "</td><td>" << j->second
               << "</td></tr>\n";
          }
          _o << "</table>\n\n";
          popindent();
        }
        // Show each nested field's name, and then the syntax and a
        // table/range of permitted values at the parent level.
        // Technically, the table might come from one of the children.
        ForEach(family._children,toChild) {
          auto &child = *toChild;
          CIHeader(getInstrFieldTitle(*child),true/* compact */);
        }
        const InstrField::Syntax *syntax = field.syntax();
        if (NULL != syntax) {
          _o << "\nSyntax: **";
          _o << getInstrFieldSyntax(field);
          _o << "**\n\n";
        }
        // Describe any semantics of the field.
        gc_string value = field.name();
        if (field.is_inverted()) {
          value = "~" + value;
        }
        if (field.shift()) {
          value = value + " << " + uitogs(field.shift());
        }
        if (field.is_signed()) {
          value = "SignExtend(" + value + ")";
        }
        if (field.addr() == InstrField::aPcRel) {
          value = core.cia()->name() + " + (" + value + ")";
        }
        // If the field has semantics, print a semantics description line.
        if (value != field.name()) {
          _o << "\nSemantics:  **" << value << "**\n\n";
        }
        // Print any reference.
        {
          gc_ostringstream os;
          gc_string s;
          if (const RegFile *rf = field.regfile()) {
            os << linkIfPossible(rf->name(),_genregs && _genrfs);
          } else if (const Memory *mem = field.memory()) {
            os << mem->name() << " Memory";
          } else if (field.instrtable()) { //
            os << linkIfPossible(field.instrtablename(), _geninstrs && _geniba);
          }
          s = os.str();
          if (!s.empty()) {
            _o << "\nReference: **" << s << "**\n\n";
          }
        }
        
        writePermittedValues(*(family._valuesField));
        
        poplevel();
      }

      poplevel();
    }

    gc_string getFieldDocName(const Core &core,const gc_string &n)
    {
      if (auto f = core.get_instrfield(n)) {
        return getInstrFieldTitle(*f);
      } else {
        return n;
      }
    }

    // Dumps out information on the field mappings between the aliases
    // of a family and their target instruction.  For brevity, all
    // fields are shown as one set in a table, even though technically
    // each alias could use a slightly different subset of them.
    // Also, any constant mappings are ignored.  All of the mappings
    // in the given range are assumed to target the same original
    // instruction (family).
    void writeOperandDescription(const Core &core,const InstrFamilyInfo &family)
    {
      gc_multimap<gc_string, gc_string> valuesByFieldName;
      gc_multimap<gc_string, gc_string> aliasNamesByOperandValue;
      ForEach(family._aliases,toAliasInstr) {
        auto &aliasInstr = *toAliasInstr;
        ForEach(aliasInstr->get_alias()->_fields,field) {
          // skip constant mappings, as requested in #498
          // (FIXME: should this be available as an option?)
          if (field->isOpcode() || field->isOpcodeImp()) {
            continue;
          }
          // skip references to hidden registers
          if (isHiddenName(field->_name)) {
            continue;
          }
          gc_ostringstream val;
          if ((Instr::Field::Ref == field->_type) &&
              (NULL != field->_sname)) {
            val << linkIfPossible(getFieldDocName(core,field->_sname),_genifields);
          } else {
            field->dump(val,true);
          }
          gc_string value = val.str();
          valuesByFieldName.insert(std::make_pair(getFieldDocName(core,field->_name),value));
          aliasNamesByOperandValue.insert(std::make_pair(value,getInstructionTitle(*aliasInstr,!family._oneBlock)));
        }
      }
      // since this is a unified table, all unique field names
      // and unique key values must be displayed
      gc_string previousKey;
      gc_set<gc_string> seenValues;
      gc_ostringstream os;
      ForEach(valuesByFieldName,vbfn) {
        auto &key = vbfn->first;
        auto value = vbfn->second;
        if (previousKey != key) {
          seenValues.clear();
        }
        previousKey = key;
        if (seenValues.end() == seenValues.find(value)) {
          seenValues.insert(value);
          os << "   * - " << linkIfPossible(key,_genifields) << "\n"
             << "     - ";
          os << value << "\n";
          gc_vector<gc_string> usageList;
          ForEach(aliasNamesByOperandValue,anbov) {
            auto opv = anbov->first;
            if (opv == value) {
              usageList.push_back(anbov->second);
            }
          }
          if (!family._oneTotal) {
            os << "     - ";
            if (!usageList.empty()) {
              // FIXME: can be simplified to vertical bar prefix when "prest" supports that new syntax
              os << "::\n        \n          " << adl::join(usageList, "\n          ");
            }
          }
          os << "\n";
        }
      }
      // finally, write the result to the stream
      gc_string s = os.str();
      if (!s.empty()) {
        _o << "Operands";
        if (!family._oneNested) {
          _o << " for Instruction **" << family._displayName << "**";
        }
        _o << ":\n";
        pushindent(2);
        //aliasInstr.print(_o, instr); _o << "\n\n"; // DEBUG ONLY
        _o << ".. list-table::\n"
           << "   :class: attributes\n";
        if (!family._oneTotal) {
          _o << "   :widths: 10 20 30\n";
        } else {
          _o << "   :widths: 10 20\n";
        }
        _o << "   :header-rows: 1\n\n"
           << "   * - Operand\n"
           << "     - Mapping\n";
        if (!family._oneTotal) {
          _o << "     - Applies To\n";
        }
        _o << s;
        popindent();
        _o << "\n";
      }
    }

    // Returns true if we should consider this bit-range when displaying an
    // instruction's encoding.  This is only relevevant for prefix bits: Some
    // users want them displayed in the encoding, some don't (since they're not
    // part of the *real* encoding for that instruction).
    bool validBits(const BitPair &b,const BitPair &ifw)
    {
      if (_instr_prefix_bits) {
        return true;
      } else {
        // If we're not printing prefix bits, then figure out if this lies
        // outside of the fetch-width (the true width of the instruction).  If
        // so, then discount it.
        return ifw.subset(b);
      }
    }

    BitPair getFetchWidthRange(const Instr &instr,const Core &core)
    {
      if (core.is_little_endian()) {
        return BitPair(instr.fetch_width()-1,0);
      } else {
        return BitPair(0,instr.fetch_width()-1);
      }
    }

    gc_string writeBitStr(int l,int r,int bitDelta,bool invert,const rnumber::RNumber &tmpValue)
    {
      gc_ostringstream os;
      for (int fieldBit = l; fieldBit != r; fieldBit += bitDelta) {
        unsigned int oneOrZero = (invert) ? tmpValue.getBitLSB(fieldBit) : tmpValue.getBit(fieldBit);
        // for readability, arbitrarily split every 4 bits
        if ((0 == (fieldBit % 4)) && (fieldBit != l)) {
          os << " / ";
        }
        os << ((oneOrZero) ? "1" : "0");
      }
      return os.str();
    }

    void genInstrFieldItem(FieldInfos &infos,const Core &core,const Instr &instr,const BitPair &ifw,const Instr::Field &f)
    {
      auto const &instrfield = *(f._field);
      auto const &bitRanges = instrfield.bits();
      // WARNING: The _value field is meaningless unless f.isOpcode() or f.isOpcodeImp().
      // For reserved values, we set it to 0 or 1, depending upon the reserved-bit value.
      rnumber::RNumber tmpValue(((instrfield.is_reserved()) ? 0 : f._value), 64/* bits */);
      if (instrfield.is_reserved() && instr.reserved_bits_value()) {
        tmpValue.setAll();
      }
      // Calculate total number of bits.
      unsigned totalFieldBits = 0;
      ForEach(bitRanges,b) {
        if (validBits(*b,ifw)) {
          totalFieldBits += b->width();
        }
      }
      // Generate bit info.
      const bool invert = core.is_little_endian();
      int base = totalFieldBits-1;
      if (invert) {
        base = 0;
      }
      // Determine the base name for the field; this may be
      // expanded to include bit sub-ranges, etc.
      gc_string baseDisplayName;
      if (instrfield.is_generated()) {
        // Avoid printing names like *_imp_bits_*, and find the
        // field that is actually being implemented.
        const InstrFieldImpl *fieldImpl = dynamic_cast<const InstrFieldImpl *>(f._field);
        const InstrField *implIface = (NULL != fieldImpl) ? fieldImpl->interface() : NULL;
        if (NULL != implIface) {
          baseDisplayName = gc_string(implIface->docName());
        } else {
          baseDisplayName = "(gen)"; // may be reset below for split fields
        }
      } else {
        baseDisplayName = (f._sname) ? f._sname : instrfield.docName();
      }
      RevForEach(bitRanges,b) {
        if (!validBits(*b,ifw)) {
          continue;
        }
        gc_string displayName = baseDisplayName; // initialize each time through
        bool isBits = false;
        int baseDelta = -b->width();
        int bitDelta = -1;
        if (invert) {
          baseDelta = -baseDelta;
          bitDelta = -bitDelta;
        }
        // Add information on this bit range.
        if (f.isOpcode()) {
          displayName = uitogs(f._value) + " " + displayName;
        } else if (f.isOpcodeImp() || instrfield.is_reserved()) {
          int l = base;
          int r = base+baseDelta;
          displayName = writeBitStr(l,r,bitDelta,invert,tmpValue);
          isBits = true;
        }
        // For split fields, indicate which sub-range this is.
#if 0
        // As requested in #498, this should not be included by default.
        // But it does seem like it could be a useful optional behavior.
        // FIXME
        if (instrfield.is_split()) {
          if ((1 == baseDelta) || (-1 == baseDelta)) {
            displayName += (" [" + uitogs(base) + "]");
          } else {
            displayName += (" [" + uitogs(base) + "," + uitogs(base+baseDelta-bitDelta) + "]");
          }
        }
#endif
        FieldInfo *fi = new FieldInfo(b->l(),b->r(),displayName,isBits,instrfield.is_reserved());
        infos.add(fi);
        base += baseDelta;
      }
    }

    void writeInstrInfo(const Instr &instr,const Core &core,FieldInfos &infos,const gc_string &msg)
    {
      if (!infos.empty()) {
        _o << "Encoding" << msg << ":\n";
        pushindent(2);
        // If we're not writing out instruction prefix bits, then the
        // instruction's fetch-width is what we care about.
        writeFields(infos,((_instr_prefix_bits) ? instr.width() : instr.fetch_width()),
                    core.is_big_endian(),true,instr.reserved_bits_value());
        popindent();
      }
    }

    void getAliasInfo(FieldInfos &infos,const Instr::Alias &alias,const Instr &instr,const BitPair &ifw,const Core &core)
    {
      ForEach(alias._fields,i) {
        genInstrFieldItem(infos,core,instr,ifw,*i);
      }       
      const Instr *trg = alias._target;
      ForRange(trg->sizeOpcs(),i) {
        genInstrFieldItem(infos,core,instr,ifw,trg->getOpc(i));
      }
    }

    // Dumps out a table graphically portraying the fields of an instruction.
    // Currently uses raw HTML output, though this may be changed in the future.
    void genInstrFieldDescription(const Instr &instr,const Core &core)
    {
      // Generate the list of items to create.  Include the name or value,
      // and bit indices.  The field might be split across multiple ranges.
      if (!instr.shorthand()) {
        FieldInfos infos;
        BitPair ifw(getFetchWidthRange(instr,core));

        ForRange(instr.sizeAll(),i) {
          Instr::Field f = instr.getAll(i);
          genInstrFieldItem(infos,core,instr,ifw,f);
        }
        writeInstrInfo(instr,core,infos,gc_string());
      } else {
        // For short-hand instructions, we iterate over the alias fields and
        // then the target's opcodes.  When there's just a single target, we
        // just write out the data in a simple format, otherwise we write it out
        // for each instruction and identify the target.
        if (instr.is_complex_alias()) {
          printFunc("Alias Action",instr.alias_action(),0);
        }
        else if (instr.get_aliases().size() > 1) {
          ForEach(instr.get_aliases(),a_iter) {
            const Instr::Alias &alias = **a_iter;
            const Instr &trg = *alias._target;

            FieldInfos infos;
            BitPair ifw(getFetchWidthRange(trg,core));

            getAliasInfo(infos,alias,instr,ifw,core);
            writeInstrInfo(trg,core,infos," for "+trg.name());
          }
        }
        else {
          FieldInfos infos;
          BitPair ifw(getFetchWidthRange(instr,core));

          const Instr::Alias &alias = *instr.get_alias();
          getAliasInfo(infos,alias,instr,ifw,core);
          writeInstrInfo(instr,core,infos,gc_string());
        }
      }
    }

    bool explicit_operand(const Instr &instr,const UsedReg &reg)
    {
      if (reg._begin && (instr.getField(reg._begin->ToString()))) {
        return true;
      }
      return false;
    }

    // Write a table showing the registers that influence or are modified by
    // an instruction.
    void writeInstrRegUsageInfo(const gc_string &title,const Instr &instr,const UsedRegs &regs,const Core &core)
    {
      //_testBitRangesMatchMask(); // DEBUG ONLY
      //_o << regs << "\n\n"; // DEBUG ONLY
      gc_vector< gc_string > usageLines;
      ForEach(regs,toUsedReg) {
        auto reg = toUsedReg->regb();
        if (isHiddenObject(*reg)) {
          continue;
        }
        // Should we omit this?  If this flag is false and it's an explicit
        // operand, then the answer is yes.
        if (!_ops_in_affect_tables && explicit_operand(instr,*toUsedReg)) {
          continue;
        }
        gc_ostringstream os;
        gc_string name((reg) ? reg->name() : "(unspecified)");
        if (reg) {
          os << "   * - " << linkIfPossible(name,_genregs && _genrfs);
          if (toUsedReg->_begin) {
            os << " (" << toUsedReg->_begin;
            if (toUsedReg->_end) {
              os << ", " << toUsedReg->_end;
            }
            if (toUsedReg->is_parm()) {
              os << "[parm #" << toUsedReg->_parm << "]";
            }
            os << ")";
          }
          os << "\n";
        } else {
          os << "   * - (unspecified)\n";
        }
        // determine the name of the field that has this mask
        if (toUsedReg->_mask.iszero()) {
          os << "     - Entire Register\n";
        } else {
          StrVect fieldNames;
          if (getUsedFields(fieldNames,*toUsedReg,core.is_little_endian(),&(HiddenObjects()))) {
            os << "     - " << adl::join(fieldNames, ", ") << "\n";
          } else {
            os << "     - Mask:  0x" << hex << setfill('0') << setw((reg) ? reg->width()/4 : 0) << toUsedReg->_mask << "\n";
          }
        }
        // add any other relevant information
        gc_vector< gc_string > info;
        if (toUsedReg->_partial) info.push_back("Partial");
        if (toUsedReg->_direct) info.push_back("Direct");
        if (toUsedReg->_cond) info.push_back("Conditional");
        if (toUsedReg->delayed()) {
          info.push_back(gc_string("Delayed ") + toUsedReg->delay()->ToString() + " cycles");
        }
        os << "     - " << adl::join(info, ", ") << "\n";
        gc_string s = os.str();
        if (!s.empty()) {
          usageLines.push_back(s);
        }
      }
      if (!usageLines.empty()) {
        _o << title << ":\n";
        pushindent(2);
        _o << ".. list-table::\n"
           << "   :class: attributes\n"
           << "   :widths: 10 15 10\n"
           << "   :header-rows: 1\n\n"
           << "   * - Register\n"
           << "     - Fields\n"
           << "     - Details\n";
        ForEach(usageLines,l) {
          _o << *l;
        }
        _o << "\n";
        popindent();
      }
    }

    struct AlphabetIndex {
      GenerateRstBase& _gen;
      gc_string _prefix;
      gc_set< int > _letters;
      
      // Helper for generating an alphabetical index.
      // Prefix finds the targets, e.g. for "#instr-section-a",
      // the prefix is "instr-section".
      AlphabetIndex(GenerateRstBase &gen,const gc_string &prefix)
      : _gen(gen), _prefix(prefix), _letters() {}
      
      // Generates the index with links for any letters in the set.
      // These links won't work unless writeTarget() is called for
      // each of the letters in the set.
      void writeIndex()
      {
        // It is not really possible to create a "small" link to a letter,
        // that wouldn't be seen to match some other likely name (such as
        // "d" matching the section for a register named D).  Given the
        // difficulty, raw HTML is used for the heading links.
        _gen._o << ".. raw:: html\n\n";
        _gen.pushindent(3);
        ForEach(_letters,toChar) {
          _gen._o << "<a href=\"#" << _prefix << "-" << (char)std::tolower(*toChar) << "\">"
                  << nbsp_str << nbsp_str << (char)std::toupper(*toChar) << nbsp_str << nbsp_str << "</a>" << " ";
        }
        _gen.popindent();
        _gen._o << "\n\n";
      }
      
      // Generates the target anchor for a letter, and removes it
      // from the list of letters so that future calls for the
      // same letter have no effect.
      void writeTarget(int letter)
      {
        if (_letters.end() != _letters.find(letter)) {
          _gen._o << "\n" << ".. _" << _prefix << "-" << (char)std::tolower(letter) << ":\n\n";
          _letters.erase(letter);
        }
      }
    };

    void writeInstrAction(const Instr &instr,Ptree *action)
    {
      printFunc("Action",action,&(instr.usageinfo()));
    }

    void writeExtractedOp(const Instr &instr,const gc_string &name,Ptree *op,bool expand_exprs)
    {
      if (op) {
        gc_string boldName = (name.empty()) ? "" : ("**" + name + "**"); // make bold
        Ptree *op2 = extractBlockBody(doOpTransforms(op,expand_exprs && expandExprs(instr),get_walker(),_doccfg));
        printFunc(boldName,op2);
      }
    }

    DocWalker *get_walker()
    {
      return 0;
    }

    // Implemented by core-level writer.
    virtual Ptree *getDocAction(const Instr &instr)
    {
      return instr.action();
    }

    // Returns the extracted doc operation and whether expression expansion is
    // allowed.
    virtual pair<Ptree *,bool> getDocOperation(const Instr &instr,Ptree *action)
    {
      return make_pair((Ptree*)0,false);
    }

    virtual Ptree *getUpdatedOp(Ptree *extop,const Instr &instr)
    {
      return 0;
    }

    // Generate instruction documentation.
    void genInstrs(const InstrHash *instrs,const Core &core)
    {
      if (!instrs || instrs->empty()) {
        return;
      }

      // It is necessary to do a first pass to extract aliases,
      // so that they can be mapped to their original instructions
      // and presented hierarchically.
      AliasByInstrName aliasesByInstr;
      ForEach((*instrs),i) {
        if (!i->second->is_used() && !_show_unused_instrs) continue;
        auto possibleAliasInstr = i->second;
        if (possibleAliasInstr->is_simple_alias()) {
          auto aliasInfo = possibleAliasInstr->get_alias();
          auto originalInstr = aliasInfo->_target;
          aliasesByInstr.insert(std::make_pair(originalInstr->name(), possibleAliasInstr));
        }
      }

      // create a properly-sorted list
      gc_vector< const Instr* > instrsByName;
      instrsByName.reserve(instrs->size());
      ForEach((*instrs),i) {
        auto instr = i->second;
        if (!instr->is_used() && !_show_unused_instrs) continue;
        // Simple aliases are printed in sub-headings, not as top-level instructions.
        if (false == instr->is_simple_alias()) {
          // a side effect of creating a family is to track renames so that
          // getInstructionTitle() behaves as expected; it is therefore
          // vital that a family be constructed here, even if it's not used,
          // so that the sort has the right names (not a good design, FIXME)
          InstrFamilyInfo family(*instr,*this,aliasesByInstr);
          instrsByName.push_back(instr);
        }
      }
      std::sort(instrsByName.begin(),instrsByName.end(),InstrTitleSort(*this));

      Header("Instructions");
      pushlevel();

      bool generateAlphabet = (instrs->size() > 5/* arbitrary */);
      AlphabetIndex ai(*this,"instr-section");
      if (generateAlphabet) {
        // find all necessary letters, and generate a set of alphabet links
        // FIXME: doing a first pass is annoying
        ForEach((*instrs),i) {
          auto &instr = *(i->second);
        if (!instr.is_used() && !_show_unused_instrs) continue;
          if (false == instr.is_alias()) {
            InstrFamilyInfo family(instr,*this,aliasesByInstr);
            int firstLetter = std::tolower(family._displayName[0]);
            if (ai._letters.end() == ai._letters.find(firstLetter)) {
              ai._letters.insert(firstLetter);
            }
          }
        }
        ai.writeIndex();
      }
      
      ForEach(instrsByName,i) {
        auto &instr = *(*i);
        
        // Simple aliases are printed in sub-headings, not as top-level instructions.
        if (!instr.is_simple_alias()) {
          InstrFamilyInfo family(instr,*this,aliasesByInstr);
          
          if (generateAlphabet) {
            // generate anchor for each new letter of the alphabet
            int firstLetter = std::tolower(family._displayName[0]);
            ai.writeTarget(firstLetter);
          }
          
          IHeader(family._displayName);
          pushlevel();
          
          // Build the description gc_string.
          gc_map< gc_string, gc_string > docParagraphsByInstrName;
          gc_map< gc_string, gc_string > docOperationsByInstrName;
          gc_map< gc_string, pair<Ptree *,bool> >   docExtrOpsByInstrName;
          gc_set< gc_string > uniqueDocs, uniqueOps;
          gc_set< Ptree *, PtreeCmp > uniqueExtrOps;

          // Get the action code and extracted operation (if found).
          Ptree *action = getDocAction(instr);
          auto extop = getDocOperation(instr,action);

          if (family._hasAliases) {
            ForEach(family._aliases,toAliasInstr) {
              auto &aliasInstr = *toAliasInstr;
              const gc_string &aliasInstrDoc = aliasInstr->doc();
              if (!aliasInstrDoc.empty()) {
                docParagraphsByInstrName.insert(std::make_pair(getInstructionTitle(*aliasInstr), aliasInstrDoc));
                uniqueDocs.insert(aliasInstrDoc);
              }

              const gc_string &aliasInstrOp = aliasInstr->doc_op();
              if (!aliasInstrOp.empty()) {
                docOperationsByInstrName.insert(std::make_pair(getInstructionTitle(*aliasInstr), aliasInstrOp));
                uniqueOps.insert(aliasInstrOp);
              }

              if (Ptree *extop2 = getUpdatedOp(extop.first,*aliasInstr)) {
                docExtrOpsByInstrName.insert(std::make_pair(getInstructionTitle(*aliasInstr), make_pair(extop2,extop.second)));
                uniqueExtrOps.insert(extop2);
              }
            }
          }
          // Base documentation is overridden when there is only one
          // nested instruction.  Otherwise, it comes first.
          if ((!family._oneNested) || docParagraphsByInstrName.empty()) {
            const gc_string &instrDoc = instr.doc();
            if (!instrDoc.empty()) {
              docParagraphsByInstrName.insert(std::make_pair(""/* force first */,instrDoc));
              uniqueDocs.insert(instrDoc);
            }
          }
          if ((!family._oneNested) || docOperationsByInstrName.empty()) {
            const gc_string &instrOp = instr.doc_op();
            if (!instrOp.empty()) {
              docOperationsByInstrName.insert(std::make_pair(""/* force first */,instrOp));
              uniqueOps.insert(instrOp);
            }
          }
          if (docExtrOpsByInstrName.empty() && extop.first) {
            docExtrOpsByInstrName.insert(std::make_pair(""/* force first */,extop));
            uniqueExtrOps.insert(extop.first);
          }
          if (1 == uniqueDocs.size()) {
            docParagraphsByInstrName.clear();
            docParagraphsByInstrName.insert(std::make_pair("",*(uniqueDocs.begin())));
          }
          if (1 == uniqueOps.size()) {
            docOperationsByInstrName.clear();
            docOperationsByInstrName.insert(std::make_pair("",*(uniqueOps.begin())));
          }
          if (1 == uniqueExtrOps.size()) {
            docExtrOpsByInstrName.clear();
            docExtrOpsByInstrName.insert(std::make_pair("",make_pair(*uniqueExtrOps.begin(),true)));
          }

          if (!docParagraphsByInstrName.empty()) {
            _o << "Description:\n";
            pushindent(2);
            ForEach(docParagraphsByInstrName,toDocByInstrName) {
              auto &name = toDocByInstrName->first;
              auto &doc = toDocByInstrName->second;
              gc_string boldName = (name.empty()) ? "" : ("**" + name + "**"); // make bold
              printIndentedBlockWithLabel(boldName,doc,false/* blank line after label */);
            }
            popindent();
          }
          if (!docOperationsByInstrName.empty()) {
            _o << "Operation:\n\n";
            pushindent(2);
            ForEach(docOperationsByInstrName,toOpByInstrName) {
              auto &name = toOpByInstrName->first;
              auto &op = toOpByInstrName->second;
              gc_string boldName = (name.empty()) ? "" : ("**" + name + "**"); // make bold
              printLiteralBlockWithLabel(boldName,op,true);
            }
            popindent();
          }
          if (!docExtrOpsByInstrName.empty()) {
            _o << "Extracted Operation:\n\n";
            pushindent(2);
            ForEach(docExtrOpsByInstrName,toOpByInstrName) {
              auto &name = toOpByInstrName->first;
              auto &op = toOpByInstrName->second;
              writeExtractedOp(instr,name,op.first,op.second);
            }
            popindent();
          }
					// Print original or expanded action, depending upon user options.
          writeInstrAction(instr,action);

          if (!instr.usageinfo()._src_regs.empty()) {
            writeInstrRegUsageInfo("Affect instruction",instr,instr.usageinfo()._src_regs,core);
          }
          if (!instr.usageinfo()._trg_regs.empty()) {
            writeInstrRegUsageInfo("Affected by instruction",instr,instr.usageinfo()._trg_regs,core);
          }
          
          if (!family._allAliasesNested) {
            // The table for a family of instructions isn't meaningful, so don't
            // print it if we have only nested aliases.
            genInstrFieldDescription(instr,core);
          }
          
          // Create sub-headings with information on aliases.
          if (family._hasAliases) {
            if (!family._oneNested) _o << "Aliases:\n";
            ForEach(family._aliases,toAliasInstr) {
              auto &aliasInstr = *toAliasInstr;
              
              if (!family._oneNested) {
                IHeader(getInstructionTitle(*aliasInstr));
                pushlevel();
              }
              
              genInstrFieldDescription(*aliasInstr,core);
              _o << "\n";
              
              if (!family._oneNested) {
                poplevel();
              }
            }
            _o << "\n";
          }
          
          writeOperandDescription(core,family);
          writeAttributes(family);
          
          poplevel();
        }
      }

      poplevel();
    }

    struct RFSizeSort {
      bool operator()(const RegField *x,const RegField *y) {
        return (x->width() > y->width());
      }
    };

    // Writes a table describing a register and its fields.
    // Returns information about the fields, in the order
    // they were written.
    void genRegDescription(const RegBase &reg,bool big_endian)
    {
      static int hidden = Data::get_attr(DocHidden);
      FieldInfos infos;
      if (reg.has_fields()) {
        // We have fields, so sort the fields by size so that we handle contained fields
        // properly, then insert into the field list object.
        gc_vector<const RegField *> regfields;
        ForEach(reg.fields(),i) {
          regfields.push_back(i->second);
        }
        sort(regfields.begin(),regfields.end(),RFSizeSort());
        ForEach(regfields,i) {
          // If the field is indexed, insert all instances of the field.
          const RegField &rf = (**i);
          if (rf.has_attr(hidden)) {
            hideObject(rf);
            continue;
          }
          if (rf.indexed()) {
            int base = 0;
            ForRange(rf.index_count(),j) {
              infos.add(new FieldInfo(base,base+rf.width()-1,rf.name() + uitogs(j),
                                      false,rf.reserved()));
              base += rf.width();
            }
          } else {            
            ForEach(rf.bits(),b) {
              infos.add(new FieldInfo(b->l(),b->r(),rf.name(),false,rf.reserved()));
            }
          }
        }
      } else {
        // No fields- just insert a single field with the same name as the register.
        infos.add(new FieldInfo(0,reg.width()-1,reg.name(),false));
      }
    
      writeFields(infos,reg.width(),big_endian,false,0);
    }

    void writeRegFields(const RegBase &reg)
    {
      if (reg.has_fields()) {
        gc_vector< RegField* > fields;
        ForEach(reg.fields(),i) {
          auto &field = i->second;
          if (isHiddenObject(*field)) {
            continue;
          }
          fields.push_back(field);
        }
        std::sort(fields.begin(),fields.end(),BitsSort<RegField>());
        if (!fields.empty()) {
          _o << "Fields:\n";
          pushindent(2);
          _o << ".. list-table::\n"
             << "   :class: attributes\n"
             << "   :widths: 15 10 40\n"
             << "   :header-rows: 1\n\n"
             << "   * - Field\n"
             << "     - Range\n"
             << "     - Description\n"
             ;
          pushindent(3);
          ForEach(fields,i) {
            auto &field = *(*i);
            _o << "* - " << quote_string(field.name());
            gc_vector< gc_string > properties;
            if (field.reserved()) {
              properties.push_back("reserved");
            }
            if (field.readonly()) {
              properties.push_back("read-only");
            }
            if (!properties.empty()) {
              _o << nbsp << "(" << adl::join(properties,nbspify(", ")) << ")";
            }
            _o << "\n";
            _o << "  - ";
            if (field.indexed()) {
              _o << nbsp << "[" << field.width() << "bits" << nbsp << "x" << nbsp << field.index_count() << "]" << nbsp;
            } else {
              ForEach(field.bits(),i) {
                if (i->l() != i->r()) {
                  _o << nbsp << "[" << i->l() << "," << i->r() << "]";
                } else {
                  _o << nbsp << "[" << i->l() << "]";
                } 
              }
              _o << nbsp;
            }
            _o << "\n";
            _o << "  - ";
            if (!field.doc().empty()) {
              _o << "\n";
              pushindent(8);
              _o << field.doc();
              popindent();
            }
            _o << "\n";
          }
          popindent();
          popindent();
        }
        _o << "\n";
      }
    }
  
    void writeRegReset(const RegBase &reg)
    {
      switch (reg.resettype()) {
      case RegBase::None:
        break;
      case RegBase::Value:
        _o << "Reset value:  " << reg.resetvalue() << "\n\n";
        break;
      case RegBase::Enum: 
        break;
      case RegBase::Action:
        printFunc("Reset semantics",reg.resetaction());
      }
    }

    // Write stuff common to both registers and register files.
    void writeRegBaseStuff(const RegBase &reg)
    {
      // Print reset information.
      writeRegReset(reg);

      gc_string doc = reg.doc();
      if (!doc.empty()) {
        printIndentedBlockWithLabel("Description",doc,true/* blank line after label */);
      }

      if (reg.pseudo()) {
        _o << "\nThis is a pseudo register.  It does not represent an actual storage element.\n\n";
      }

      // List all fields here.
      writeRegFields(reg);
    }

    // Write register read/write hook information.
    void writeRegHook(const char *msg,const RegHook &hook)
    {
      _o << "On a " << msg;
      if (hook.ignore()) {
        _o << ": Action is ignored.\n\n";
      } else if (const Reg *ra = hook.alias().reg()) {
        _o << ": Action is aliased to register **" << ra->name() << "_**.\n\n";
      } else if (const RegFile *ra = hook.alias().regfile()) {
        _o << ": Action is aliased to register-file element **" << ra->name() << "[" << hook.alias().index() << "]**.\n\n";
      } else {
        printFunc("",hook.action());
      }
      popindent();
    }
 
    // Write register entry read/write hook information.
    void writeEntryHook(const char *msg,const RegEntryHook &hook)
    {
      _o << "On a " << msg;
      if (hook.ignore()) {
        _o << ": Action is ignored.\n\n";
      } else {
        _o << ":\n";
        gc_ostringstream os;
        os << get_func_body(hook.action());
        printLiteralBlock(os.str(),true);
      }
    }

    // Generate register documentation
    void genReg(const Reg &reg,const Resources &share,bool big_endian)
    {
      if (share.contains(reg)) {
        return;
      }

      static int hidden = Data::get_attr(DocHidden);
      if (reg.has_attr(hidden)) {
        hideObject(reg);
        return;
      }

      condHeader(_reg_header,"Registers");

      pushlevel();

      IHeader(reg.name());
      pushlevel();

      // Generate the graphical depiction of the register.
      genRegDescription(reg,big_endian);

      // Write common stuff.
      writeRegBaseStuff(reg);

      static int no_impl = Data::get_attr(DocNoCode);
      if (!reg.has_attr(no_impl)) {
        // Write read/write action information.
        if (const RegHook *hook = reg.readhook()) {
          writeRegHook("read",*hook);
        }
        if (const RegHook *hook = reg.writehook()) {
          writeRegHook("write",*hook);
        }
      }

      poplevel();

      poplevel();
    }

    // If we have a sparse register file, then create a table displaying indices
    // vs. register names.
    void writeSparseFile(const RegFile &rf)
    {
      static int no_impl = Data::get_attr(DocNoCode);

      if (rf.sparse()) {
        _o << ".. list-table:: Constituent Registers\n"
           << "   :class: sparsecontents\n"
           << "   :widths: 10 40\n"
           << "   :header-rows: 1\n\n"
           << "   * - Index\n"
           << "     - Register Usage\n"
           ;
        pushindent(3);
        ForEach(rf.entries(),i) {
          auto &entry = *(i->second);
          gc_ostringstream os;
          genEntryName(os,entry,rf);
          gc_string regName = os.str();
          if (isHiddenName(regName)) {
            continue;
          }
          // Register entry names can be a lot of things, and not
          // all generated names will be valid links.  So carefully
          // check which ones can actually be linked.
          gc_string anchor = getTarget(regName);
          bool makeLink = _genregs && _genrfs;
          _o << "* - " << entry.id() << "\n" 
             << "  - " << linkIfPossible(regName,makeLink) << "\n";
          if (!rf.has_attr(no_impl)) {
            if (const RegEntryHook *hook = entry.read()) {
              pushindent(4);
              writeEntryHook("read",*hook);
              popindent();
            }
            if (const RegEntryHook *hook = entry.write()) {
              pushindent(4);
              writeEntryHook("write",*hook);
              popindent();
            }
          }
        }
        _o << "\n";
        popindent();
      }
    }

    // Write register read/write hook information.
    void writeRegFileHook(const char *msg,const RegFileHook &hook)
    {
      _o << "On a " << msg;
      if (hook.ignore()) {
        _o << ": Action is ignored.\n\n";
      } else if (const RegFile *rfa = hook.alias().regfile()) {
        _o << ": Action is aliased to register file **" << rfa->name() << "_**.\n\n";
        // Display a mapping table if necessary.
        if (!hook.aliasindices().empty()) {
          _o << ".. raw:: html\n\n";
          pushindent(3);
          _o << "<table class=\"indexmapping\">\n"
             << "<tr>\n"
             << "  <th>Source Index</th>\n"
             << "  <th>Target Index</th>\n"
             << "</tr>\n";
          ForRange(hook.aliasindices().size(),i) {
            _o << "<tr><td>" << i << "</td><td>" << hook.aliasindices()[i] << "</td></tr>\n";
          }
          _o << "</table>\n\n";
          popindent();        
        }
      } else {
        printFunc("",hook.action());
      }
    }

    // Generate register-file documentation
    void genRegFile(const RegFile &rf,const Resources &share,bool big_endian)
    {
      if (share.contains(rf)) {
        return;
      }

      static int hidden = Data::get_attr(DocHidden);
      if (rf.has_attr(hidden)) {
        hideObject(rf);
        return;
      }

      condHeader(_rf_header,"Register Files");

      pushlevel();

      IHeader(rf.name());
      pushlevel();

      // Generate the graphical depiction of the register.
      genRegDescription(rf,big_endian);

      _o << "Number of registers in file:  " << rf.size() << "\n\n";

      // Write common stuff.
      writeRegBaseStuff(rf);

      // If sparse, then list containing registers.
      writeSparseFile(rf);

      static int no_impl = Data::get_attr(DocNoCode);
      if (!rf.has_attr(no_impl)) {
        // Write read/write action information.
        if (const RegFileHook *hook = rf.readhook()) {
          writeRegFileHook("read",*hook);
        }
        if (const RegFileHook *hook = rf.writehook()) {
          writeRegFileHook("write",*hook);
        }
      }

      poplevel();

      poplevel();
    }

    // Generate information about eventbus fields.
    template <class Fields>
    void genFields(const Fields &fields,const gc_string &type)
    {
      if (!fields.empty()) {
        _o << ".. list-table:: **" << type << " Fields**\n"
           << "   :widths: 10 10 40\n"
           << "   :header-rows: 1\n\n"
           << "   * - Name\n"
           << "     - Bits\n"
           << "     - Description\n";

        pushindent(3);
        ForEach(fields,f) {
          auto &field = *(f->second);

          _o << "* - " << field.name() << "\n" 
             << "  - " << field.bits() << "\n";
          _o << "  - ";
          pushindent(4);
          _o << field.doc() << "\n";
          popindent();
        }
        _o << "\n";
        popindent();
      }
    }

    // Generate cache information
    void genCache(const Cache &cache,const Resources &share)
    {
      if (share.contains(cache)) {
        return;
      }

      condHeader(_cache_header,"Caches");

      pushlevel();

      IHeader(cache.name());
      pushlevel();

      switch (cache.type()) {
      case InstrCache:
        _o << "Instruction Cache\n\n";
        break;
      case DataCache:
        _o << "Data Cache\n\n";
        break;
      case UnifiedCache:
        _o << "Unified Cache\n\n";
        break;
      }

      _o << "Size:  " << cache.size() << " bytes.  Line size:  " 
         << cache.linesize() << " bytes.  ";
      if (cache.fully_assoc()) {
        _o << "Fully associative.\n\n";
      } else {
        _o << "Associativity:  " << cache.ways() << " ways.\n\n";
      }

      if (!cache.doc().empty()) {
        _o << cache.doc() << "\n\n";
      }

      genFields(cache.set_fields(),"Set");
      genFields(cache.way_fields(),"Way");

      printFunc("Cache-enable predicate",cache.enable());
      printFunc("Write-through predicate",cache.write_through());
      printFunc("Action on a cache hit",cache.hit());
      printFunc("Action on a cache miss",cache.miss());
      printFunc("Action on a line invalidation",cache.invalidate_line());
      printFunc("Action on a line read",cache.read_line());
      printFunc("Action on a line replacement",cache.replace());
      printFunc("Action on a line access",cache.line_access());
      
      poplevel();

      poplevel();
    }

    // Generate local memory information
    void genMemory(const Memory &mem,const Resources &share)
    {
      if (share.contains(mem)) {
        return;
      }

      condHeader(_memory_header,"Memories");

      pushlevel();

      IHeader(mem.name());
      pushlevel();

      if (mem.instr_mem()) {
        _o << "Instruction Memory\n\n";
      }

      _o << "Size:  " << mem.size() << " bytes.  Address-unit size:  " 
         << mem.addr_unit() << " bytes.\n\n";

      if (mem.parent()) {
        _o << "Parent memory:  " << mem.parent()->name() 
           << " (located at offset 0x" << mem.offset() << dec << "\n\n"; 
      }

      if (!mem.doc().empty()) {
        _o << mem.doc() << "\n\n";
      }

      if (const MemoryHook *mh = mem.read_hook()) {
        printFunc("Action on a read",mh->action());
      }
      if (const MemoryHook *mh = mem.write_hook()) {
        printFunc("Action on a write",mh->action());
      }
      
      poplevel();

      poplevel();
    }

    // Generate information about an event bus.
    void genEventBus(const EventBus &eventbus,const Resources &share)
    {
      if (share.contains(eventbus)) {
        return;
      }

      condHeader(_eventbus_header,"Event Buses");

      pushlevel();

      IHeader(eventbus.name());
      pushlevel();

      genFields(eventbus.fields(),"Event Bus");

      printFunc("Event action",eventbus.action());
      
      if (!eventbus.doc().empty()) {
        _o << eventbus.doc() << "\n\n";
      }

      poplevel();

      poplevel();
    }

    // Generate information about an external resource.
    void genExtResource(const ExtResource &extres,const Resources &share)
    {
      if (share.contains(extres)) {
        return;
      }

      condHeader(_memory_header,"External Resources");

      pushlevel();

      IHeader(extres.name());
      pushlevel();

      _o << "Object type:  " << extres.objtype() << "\n\n";

      if (extres.external()) {
        _o << "Externally defined.\n\n";
      } else {
        _o << "Internally defined.\n\n";
      }

      if (!extres.constr_args().empty()) {
        _o << "Constructed With:  ";
        ForEach(extres.constr_args(),i) {
          _o << *i << " ";
        }
        _o << "\n\n";
      }
     
      if (!extres.doc().empty()) {
        _o << extres.doc() << "\n\n";
      }

      poplevel();

      poplevel();
    }

    // Generate information about a context.
    void genContext(const Context &ctx)
    {
      condHeader(_ctx_header,"Context");

      pushlevel();

      IHeader(ctx.name());
      pushlevel();

      if (!ctx.doc().empty()) {
        _o << ctx.doc() << "\n\n";
      }

      _o << "Number of contexts:  " << ctx.num_contexts() << "\n\n";

      _o << "**Registers:**  ";
      ForEach(ctx._regs,i) {
        _o << i->first << " ";
      }
      _o << "\n\n"
         << "**Register Files:**  ";
      ForEach(ctx._regfiles,i) {
        _o << i->first << " ";
      }
      _o << "\n\n";

      _o << "\n\n"
         << "**MMU Lookups:**  ";
      ForEach(ctx._mmu,i) {
        _o << i->first << " ";
      }
      _o << "\n\n";

      printFunc("Enable",ctx.active());
   
      poplevel();

      poplevel();
    }

    // Generate information about an MMU lookup
    void genMmuLookups(const MmuLookupHash &lookups)
    {
      ForEach (lookups,l) {
        const MmuLookup &lookup = *(l->second);

        if (lookup.is_interface()) {
          continue;
        }

        IHeader("Lookup:  " << lookup.name());

        switch (lookup.type()) {
        case MmuLookup::Instr:
          _o << "Instruction-only\n\n";
          break;
        case MmuLookup::Data:
          _o << "Data-only\n\n";
          break;
        default:
          ;
        }

        if (!lookup.doc().empty()) {
          _o << lookup.doc() << "\n\n";
        }

        _o << "Priority:  " << lookup.priority() << "\n\n";

        switch (lookup.sizetype()) {
        case MmuLookup::None:
          break;
        case MmuLookup::Const:
          _o << "Page Size:  " << lookup.pagesize() << " bytes.\n\n";
          break;
        case MmuLookup::BitSize:
          _o << "Page Size:  2^(" << lookup.sizescale() << " * " << lookup.sizefield()->name() << " + " << lookup.sizeoffset() << ") << " << lookup.sizeshift() << "\n\n";
          break;
        case MmuLookup::LeftMask:
          _o << "Page Size:  " << lookup.sizefield()->name() << " field (left-justified mask) << " << lookup.sizeshift() << "\n\n";
          break;
        }

        if (lookup.has_array()) {
          _o << lookup.entries() << " entries, ";
          if (lookup.fully_assoc()) {
            _o << "fully associative.\n\n";
          } else {
            _o << lookup.assoc() << "-way associative.\n\n";
          }
        }

        genFields(lookup.set_fields(),"Set");
        genFields(lookup.way_fields(),"Way");

        if (!lookup.tests().empty()) {
          if (lookup.tests().size() == 1) {
            _o << "Tests::\n\n";
            pushindent(3);
            ForEach(lookup.tests().begin()->second->tests(),t) {
              _o << *t << "\n\n";
            }
            popindent();
          } else {
            _o << "Test Sets:\n\n";
            pushindent(3);
            ForEach(lookup.tests(),t) {
              const MmuTestSet &testset = *t->second;
              _o << testset.name() << "\n";
              pushindent(3);
              if (testset.enable()) {
                _o << "Enable::\n\n";
                pushindent(3);
                _o << testset.enable()._func << "\n\n";
                popindent();
              }
              if (1) {
                _o << "Tests::\n\n";
                pushindent(3);
                ForEach(testset.tests(),iter) {
                  _o << *iter << "\n\n";
                }
                popindent();
              }
              popindent();
            }
            popindent();
          }
        }

        printFunc("Execute permission",lookup.exec_perm());
        printFunc("Load permission",lookup.load_perm());
        printFunc("Store permission",lookup.store_perm());

        printFunc("Valid predicate",lookup.valid_func());
        printFunc("Translation-hit function",lookup.hit_func());
        printFunc("Translation-miss function",lookup.miss_func());
        printFunc("Reset function",lookup.reset());

        if (!lookup.lookups().empty()) {
          IHeader("Child Lookups");
          pushlevel();
          genMmuLookups(lookup.lookups());
          poplevel();
        }
      }
    }

    // Generate MMU information
    void genMmu(const MMU *mmu,const Resources &share)
    {
      if (mmu) {
     
        // If all lookups are shared, then we skip printing this one.
        bool s = true;
        ForEach(mmu->top_lookups(),i) {
          const MmuLookup &ml = *(i->second);
          if (ml.is_interface()) {
            continue;
          }
          if (!share.contains(ml)) {
            s = false;
          }
        }

        if (s) {
          return;
        }
      
        Header("MMU");
        pushlevel();

        if (!mmu->doc().empty()) {
          _o << mmu->doc() << "\n\n";
        }

        _o << "\nInterleaved fetch :  " << boolalpha << mmu->interleaved_fetch()
           << "\nInterleaved reads :  " << boolalpha << mmu->interleaved_reads()
           << "\nInterleaved writes:  " << boolalpha << mmu->interleaved_writes()
           << "\n\n";

        genMmuLookups(mmu->top_lookups());

        printFunc("Instruction-enable predicate",mmu->instr_enable());
        printFunc("Data-enable predicate",mmu->data_enable());

        printFunc("Action on an instruction miss",mmu->instr_miss());
        printFunc("Action on a data miss",mmu->data_miss());
        printFunc("Action on a load miss",mmu->load_miss());
        printFunc("Action on a store miss",mmu->store_miss());
        printFunc("Action on a multi-hit",mmu->multi_hit());

        printFunc("Misaligned-write hook",mmu->misaligned_write());
        printFunc("Misaligned-read hook",mmu->misaligned_read());

        printFunc("Post-write hook",mmu->post_write());
        printFunc("Post-read hook",mmu->post_read());

        poplevel();
      }
    }

    // Generate exception information.
    void genExceptions(const ExceptionHash *excepts)
    {
      if (!excepts || excepts->empty()) {
        return;
      }

      Header("Exceptions");
      pushlevel();
    
      ForEach((*excepts),i) {
        const Exception &ex = *(i->second);
        // This name must be unique in case it conflicts with an earlier
        // definition, such as a register.  Since this is an appendix, a
        // conflict cannot be discovered in advance when doing one pass.
        IHeader(ex.name() << " Exception");
        pushlevel();

        if (!ex.doc().empty()) {
          _o << ex.doc() << "\n\n";
        }

        _o << "Priority:  " << ex.priority() << "\n\n";

        printFunc("Action taken when exception occurs",ex.action());

        poplevel();
      }

      poplevel();
    }

    // Generate information about core-level hook functions.
    void genCoreHookFuncs(const Core &core)
    {
      if (core.decode_miss_handler() ||
          core.pre_cycle_handler() ||
          core.post_cycle_handler() ||
          core.pre_fetch_handler() ||
          core.post_fetch_handler() ||
          core.post_exec_handler() ||
          core.active_watch() ||
          core.itable_watch()) {

        Header("Core-level Hook Functions");

        printFunc("Action taken on a decode miss",core.decode_miss_handler());

        printFunc("Action taken before a clock cycle",core.pre_cycle_handler());

        printFunc("Action taken after a clock cycle",core.post_cycle_handler());

        printFunc("Action taken before an instruction fetch",core.pre_fetch_handler());

        printFunc("Action taken after an instruction fetch",core.post_fetch_handler());

        printFunc("Action taken after instruction execution",core.post_exec_handler());

        printFunc("Predicate for determining if the core isactive",core.active_watch());

        printFunc("Predicate for determing the currently-active instruction table",core.itable_watch());
      
      }
    }

    // Generate an appendix with helper function information.
    void genHelperFunctionsAppendix(const Core &core)
    {
      Header("Helper Functions");
      pushlevel();
      
      _o << "Built-in helper routines are documented in `the ADL user manual`_.\n\n";
      _o << ".. _`the ADL user manual`: ";
      _o << "http://cad.freescale.net/trac/adl/chrome/site/adl-language-reference.html#global-functions\n\n";
      
      HelperMap allHelpers = core.helpers();
      if (!allHelpers.empty()) {
        gc_vector< gc_string > sortedHelperNames;
        sortedNames(allHelpers,sortedHelperNames);
        
        _o << "In addition, the following helper routines are defined for " << core.name() << ":\n\n";
        
        bool generateAlphabet = (sortedHelperNames.size() > 5/* arbitrary */);
        AlphabetIndex ai(*this,"helpers-section");
        if (generateAlphabet) {
          // find all necessary letters, and generate a set of alphabet links
          ForEach(sortedHelperNames,i) {
            auto &name = *i;
            int firstLetter = std::tolower(name[0]);
            if (ai._letters.end() == ai._letters.find(firstLetter)) {
              ai._letters.insert(firstLetter);
            }
          }
          ai.writeIndex();
        }
        
        ForEach(sortedHelperNames,toName) {
          auto &helperName = *toName;
          auto toHelperInfoByName = allHelpers.find(helperName);
          
          if (generateAlphabet) {
            // generate anchor for each new letter of the alphabet
            int firstLetter = std::tolower(helperName[0]);
            ai.writeTarget(firstLetter);
          }
          
          IHeader(helperName);
          pushlevel();
          
          //gc_ostringstream os;
          //os << toHelperInfoByName->second._func;
          //gc_string s = os.str();
          //printLiteralBlock(s,1);
          printFunc("",toHelperInfoByName->second);
          
          poplevel();
        }
      }
      
      poplevel();
    }

  };

  struct GenerateRstCore : public GenerateRstBase {

    const Core    &_core;     // Core we're generating documentation for.
    DocWalker      _walker;

    GenerateRstCore(const Core &c,const GenerateRstBase &grb) :
      GenerateRstBase(grb),
      _core(c),
      _walker(c,c.env())
    {
    }

    virtual DocWalker *get_walker()
    {
      return &_walker;
    }

    virtual Ptree *getDocAction(const Instr &instr)
    {
      _walker.reset();
      return _walker.Translate(instr.action());
    }

    // For this to work, getDocAction must be called before getDocOperation,
    // which then just returns what the walker saved.
    virtual pair<Ptree *,bool> getDocOperation(const Instr &instr,Ptree *action)
    {
      Ptree *op = _walker.doc_op();
      if (!op && expandExprs(instr)) {
        op = action;
      }
      return make_pair(op,!_walker.has_ctrl_stmts());
    }

    // Do any transformations based upon alias parameters.
    virtual Ptree *getUpdatedOp(Ptree *extop,const Instr &instr)
    {
      return replaceAliasParms(extop,_walker,instr.get_alias());
    }

    void populateRegSets()
    {
      ForEach(_core.regs(),i) {
        _allregs.insert(i->first);
      }
      ForEach(_core.regfiles(),i) {
        _allregs.insert(i->first);
        _regfiles.insert(i->first);
      }
    }

    void exec()
    {
      nameHeader("Core:",_core);

      pushlevel();

      populateRegSets();

      if (!_core.doc().empty()) {
        Header("Overview");
        _o << _core.doc() << "\n\n";
      }

      if (_genregs) {
        ForEach(_core.regs(),i) {
          genReg(*(i->second),_core.shared(),_core.is_big_endian());
        }
      }

      if (_genrfs) {
        ForEach(_core.regfiles(),i) {
          genRegFile(*(i->second),_core.shared(),_core.is_big_endian());
        }
      }
   
      if (_genifields) {
        genInstrFields(&(_core.instrfields()),_core);
      }

      if (_geninstrs) {
        auto instrs = &(_core.instrs());
        genInstrs(instrs,_core);
        if (_geniba) {
          genInstrAttributesAppendix(instrs);
        }
        if (_genibb) {
          genInstrBlocksAppendix(instrs,_core);
        }
      }

      if (_genexcepts) {
        genExceptions(&(_core.exceptions()));
      }

      if (_gencaches) {
        ForEach(_core.caches(),i) {
          genCache(*(i->second),_core.shared());
        }
      }

      if (_genmems) {
        ForEach(_core.memories(),i) {
          genMemory(*(i->second),_core.shared());
        }
      }

      if (_genevtbuses) {
        ForEach(_core.eventbuses(),i) {
          genEventBus(*(i->second),_core.shared());
        }
      }

      if (_genextres) {
        ForEach(_core.extresources(),i) {
          genExtResource(*(i->second),_core.shared());
        }
      }

      if (_gencontexts) {
        ForEach(_core.contexts(),i) {
          genContext(*(i->second));
        }
      }

      if (_genmmu) {
        genMmu(_core.mmu(),_core.shared());
      }

      if (_genclh) {
        genCoreHookFuncs(_core);
      }

      if (_genhelpers) {
        genHelperFunctionsAppendix(_core);
      }

      poplevel();
    };

  };

  class GenerateRstSys : public GenerateRstBase {

  public:

    GenerateRstSys(opfxstream &o,const AdlParser &parser,StrSet &allregs,StrSet &regfiles,
                   const AnyOption &options,const DocCfg &doccfg,int level = 0) :
      GenerateRstBase(o,parser,allregs,regfiles,options,doccfg,level)
    {};

    void exec(const Data &data)
    {
      ForEach(data.cores(),i) {
        genItem(i->second,Resources());
      }
      ForEach(data.systems(),i) {
        genItem(i->second,Resources());
      }
    }

    void genItem(const SysItem *item,const Resources &pshared)
    {
      if (const Core *core = dynamic_cast<const Core *>(item)) {
        GenerateRstCore gencore(*core,*this);
        gencore.exec();      
      } else if (const System *sys = dynamic_cast<const System *>(item)) {
        GenerateRstSys gensys(*this);
        gensys.genSystem(*sys,pshared);
      }
    }

    // This just grabs the first MMU it can find.
    const MMU *get_mmu(const SysItem *si)
    {
      if (const Core *c = dynamic_cast<const Core *>(si)) {
        return c->mmu();
      } else if (const System *s = dynamic_cast<const System *>(si)) {
        ForEach(s->local_systems(),i) {
          if (const MMU *m = get_mmu(i->second)) {
            return m;
          }
        }
        ForEach(s->local_cores(),i) {
          if (const MMU *m = get_mmu(i->second)) {
            return m;
          }
        }
      }
      return 0;
    }

    struct GetInstrFields {
      const InstrFieldHash  *operator()(const Core &c)
      {
        return &(c.instrfields());
      }
    };

    struct GetInstrs {
      const InstrHash  *operator()(const Core &c)
      {
        return &(c.instrs());
      }
    };

    struct GetExceptions {
      const ExceptionHash  *operator()(const Core &c)
      {
        return &(c.exceptions());
      }
    };

    // If all instruction fields are the same at this level, then display
    // them here.
    template <class T,class F>
    const T *getCommonItem(const System &sys)
    {
      vector<const T *> stuff;
      ForEach(sys.items(),i) {
        stuff.push_back(getCommonItem<T,F>(i->second));
        if (!stuff.empty()) {
          auto iter = stuff.begin();
          const T *x = *(iter++);
          for ( ; iter != stuff.end(); ++iter) {
            if ( *x != *(*iter) ) {
              return 0;
            }
          }
          return x;
        }
      }
      return 0;
    }

    template <class T,class F>
    const T *getCommonItem(const SysItem *si)
    {
      if (const Core *c = dynamic_cast<const Core *>(si)) {
        F f;
        return f(*c);
      } else if (const System *s = dynamic_cast<const System *>(si)) {
        return getCommonItem<T,F>(*s);
      }
      return 0;
    }

    const Core *getFirstCore(const SysItem *si)
    {
      if (const Core *c = dynamic_cast<const Core *>(si)) {
        return c;
      } else if (const System *s = dynamic_cast<const System *>(si)) {
        ForEach(s->items(),i) {
          if (const Core *c = getFirstCore(i->second)) {
            return c;
          }
        }
      }
      return 0;    
    }

    void genSharedResources(const System &sys,const Resources &pshared)
    {
      if (!sys.doc().empty()) {
        Header("Overview");
        _o << sys.doc() << "\n\n";
      }

      Header("Shared Resources");

      pushlevel();

      const Core *core = getFirstCore(&sys);
      if (!core) {
        RError("Could not find a child that is a core.");
      }

      const Resources &shared = sys.shared();

      if (_genregs) {
        ForEach(shared._regs,i) {
          genReg(*(sys.get_reg(i->first)),pshared,core->is_big_endian());
        }
      }

      if (_genrfs) {
        ForEach(shared._regfiles,i) {
          genRegFile(*(sys.get_regfile(i->first)),pshared,core->is_big_endian());
        }
      }

      if (_genifields) {
        if (const InstrFieldHash *i = getCommonItem<InstrFieldHash,GetInstrFields>(sys)) {
          genInstrFields(i,*core);
          _genifields = false;
        }
      }

      if (_geninstrs) {
        if (const InstrHash *i = getCommonItem<InstrHash,GetInstrs>(sys)) {
          genInstrs(i,*core);
          _geninstrs = false;
        }
      }

      if (_genexcepts) {
        if (const ExceptionHash *i = getCommonItem<ExceptionHash,GetExceptions>(sys)) {
          genExceptions(i);
          _genexcepts = false;
        }
      }
   
      if (_gencaches) {
        ForEach(shared._caches,i) {
          genCache(*(sys.get_cache(i->first)),pshared);
        }
      }

      if (_genmems) {
        ForEach(shared._memories,i) {
          genMemory(*(sys.get_memory(i->first)),pshared);
        }
      }

      if (_genevtbuses) {
        ForEach(shared._eventbuses,i) {
          genEventBus(*(sys.get_eventbus(i->first)),pshared);
        }
      }

      if (_genextres) {
        ForEach(shared._extresources,i) {
          genExtResource(*(sys.get_extresource(i->first)),pshared);
        }
      }

      if (_genmmu) {
        genMmu(get_mmu(&sys),pshared);
      }

      poplevel();

    }

    void genSystem(const System &sys,const Resources &pshared)
    {
      nameHeader("System:",sys);

      pushlevel();

      genSharedResources(sys,pshared);

      ForEach(sys.items(),i) {
        genItem(i->second,sys.shared());
      }    

      poplevel();
    }

  };

  unsigned GenerateRstBase::_target_id = 0;  

  int calcContentDepth(const SysHash &sys)
  {
    // We start with a level of 2, which implies a core and its primary underlying
    // elements.
    // If we have systems, then we calculate the extra depth needed.
    int depth = 2;
    ForEach(sys,i) {
      depth = max(depth,1 + calcContentDepth(i->second->local_systems()));
    }
    return depth;
  }

  // If the user specified it, then create a file which contains perl data
  // structures with various info.
  void createPerlFile(const StrSet &allregs,const StrSet &regfiles,const AnyOption &options)
  {
    if (options.haveOption("pl-file")) {
      string fn = options.getOption("pl-file");
      ofstream pf(fn.c_str());
      if (!pf) {
        RError("Unable to open Perl data file '" << fn << "'.");
      }

      ostream_iterator<gc_string> oiter(pf," ");
      
      pf << "@regfiles = qw ( ";    
      copy (regfiles.begin(),regfiles.end(),oiter);
      pf << " );\n";

      pf << "@allregs = qw ( ";
      copy (allregs.begin(),allregs.end(),oiter);
      pf << " );\n";
    }
  }

  // Opens the output file and initiates the model creation process.
  void process(const AdlParser &parser,const gc_string &ofn,const AnyOption &options,const DocCfg &doccfg)
  {
    ofstream out(ofn.c_str());
    if (!out) {
      RError("Unable to open output file '" << ofn << "'.");
    }

    opfxstream outln(out.rdbuf());

    bool print_location = options.getFlag("psl",false);
    bool full_path      = options.getFlag("fp",true);

    // Look to see if we have a title.  Otherwise, use a default.
    string t = options.getOption("title","ADL Documentation");
    string h(t.size(),'=');
    outln << h << '\n' << t << '\n' << h << "\n\n";

    // If we have subtitles, then add them.
    if (options.haveOption("subtitle")) {
      auto subtitles = options.getValues("subtitle");
      bool first = true;
      ForEach(subtitles,i) {
        const string &st = *i;
        // For the first subtitle, we can use a standard (lone) section
        // identifier.  Subsequent subtitles are not directly supported, so we
        // use the class identifier to assign them a class, then let the
        // stylesheet style them appropriately.
        if (first) {
          string h(st.size(),'+');
          outln << h << '\n' << *i << '\n' << h << "\n\n";
        } else {
          outln << ".. class:: subtitle\n\n" << *i << "\n\n";
        }
        first = false;
      }
    }

    // Basic header stuff to include a table of contents with section numbering
    // and some subsitutions for mathematical symbols.  These are only used when
    // the highlighting level is set to >= 2.
    outln << ".. contents:: Table of Contents\n"
          << "   :backlinks: entry\n"
          << "   :depth: " << calcContentDepth(parser.data().systems()) << "\n\n"
          << ".. sectnum::\n\n"
          << ".. |ne|:  mathml:: ne\n"
          << ".. |le|:  mathml:: leq\n"
          << ".. |ge|:  mathml:: geq\n"
          << ".. |la|:  mathml:: wedge\n"
          << ".. |lo|:  mathml:: vee\n"
          << ".. |xor|: mathml:: oplus\n\n";

    StrSet allregs, regfiles;

    GenerateRstSys genrst(outln,parser,allregs,regfiles,options,doccfg);

    genrst.set_print_location(print_location);
    genrst.set_full_path(full_path);
    genrst.set_expand_exprs(options.getFlag("expand-exprs",genrst.expand_exprs()));

    genrst.exec(parser.data());

    createPerlFile(allregs,regfiles,options);
  }

  DocCfg *create_configdb(const AnyOption &options,bool fullpatherrors)
  {
    string conf_name = options.getOption("config");
    
    DocCfg *configdb = new DocCfg;
    configdb->parse(conf_name,fullpatherrors);
        
    return configdb;
  }

}

using namespace adl;

int main(int argc, const char** argv)
{
  log_usage_at_exit(argc,argv,"tool");

  AnyOption options(argc,argv);
  standardParserOptionSetup(options);

  // WARNING: If you add flags here, also check "scripts/adl2doc.in" (Perl),
  // and the documentation in "doc/adl-language-reference.rst".
  options.addUsage(":  [options] <source>");
  options.setFlag("help","h",                    "Display help.");
  options.setFlag("print-source-location","psl", "Display source locations of action-code.");
  options.setFlag("full-path","fp",              "Display the full path of source location filenames.\nDefault is true.");
  options.setOption("title","t",                 "Specify a document title.");
  options.setOption("subtitle","st",             "Specify a document subtitle.   Multiple options are allowed.");
  options.setOption("output","o",                "Specify an output file.  Default is the basename of the input with an .rst extension.");
  options.setOption("hl-level",                  "Specify a high-light level.  Default is 1, which means color syntax highlighting.");
  options.setOption("pl-file",                   "Specify a filename to which to write extra side-band information.");
  options.setOption("config","cf",               "Specify an documentation-generation configuration file.");
  options.setFlag("registers","reg",             "Generate section for registers.\nDefault is true if any are defined.");
  options.setFlag("register-files","rf",         "Generate section for register files.\nDefault is true if any are defined.");
  options.setFlag("instruction-fields","if",     "Generate section for instruction fields.\nDefault is true if any are defined.");
  options.setFlag("instructions","instr",        "Generate section for instructions.\nDefault is true if any are defined.");
  options.setFlag("instr-by-attr","iba",         "Generate cross-section for instructions by attribute.\nDefault is true if any are defined.");
  options.setFlag("instr-by-block","ibb",        "Generate cross-section for instructions by block.\nDefault is true if any are defined.");
  options.setFlag("exceptions","exc",            "Generate section for exceptions.\nDefault is true if any are defined.");
  options.setFlag("caches",                      "Generate section for caches.\nDefault is true if any are defined.");
  options.setFlag("memories","mem",              "Generate section for memories.\nDefault is true if any are defined.");
  options.setFlag("event-buses","eb",            "Generate section for event buses.\nDefault is true if any are defined.");
  options.setFlag("ext-resources","er",          "Generate section for external resources.\nDefault is true if any are defined.");
  options.setFlag("contexts",                    "Generate section for contexts.\nDefault is true if any are defined.");
  options.setFlag("mmu",                         "Generate section for the MMU.\nDefault is true if any lookups are defined.");
  options.setFlag("core-level-hooks","clh",      "Generate section for core-level hook functions.\nDefault is true if any are defined.");
  options.setFlag("helper-functions",            "Generate section for helper functions.\nDefault is true if any are defined.");
  options.setFlag("instr-prefix-bits","ipb",     "Display fields in instructions which come from prefix fields.  Default is true.");
  options.setFlag("ops-in-affect-tables","oat",  "Display explicit operand information in affected/affected-by tables.  Default is true.");
  options.setFlag("syntax-prefix-fields","spf",  "Display fields in syntax strings which precede instruction mnemonics.  Defaultis true.");
  options.setFlag("proportional-subfields","psf","Force sub-fields to be displayed with widths proportional to their bit sizes.  Default is false.");
  options.setFlag("unused-instrs",               "Display all instructions, even instructiosn which do not occur in any instantiated instruction tables.");
  options.setFlag("expand-exprs",                "Expand variables in-place in instruction action code.  Default is false.");

  if (!options.processCommandLine()) {
    return 1;
  }

  if (options.getFlag("h")) {
    options.printUsage();
    return 0;    
  };

  if (!options.getArgc()) {
    cerr << "No source file specified.\n";
    options.printUsage();
    return 1;
  }

  AdlParser parser;

  string input = options.getArgv(0);

  // If the user hasn't specified an output name, then use the
  // core's name.
  gc_string ofn = options.getOption("o").c_str();
  if (ofn.empty()) {
    ofn = gen_default_name(input,".rst").c_str();
  }

  bool fullpatherrors = standardParserSetup(parser,options,ofn.c_str());

  // This parses the ADL file and creates the data model.
  if (!parser.parse(input)) {
    return 1;
  }

  DocCfg *doccfg = create_configdb(options,fullpatherrors);

  try {
    process(parser,ofn,options,*doccfg);
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n";
    return 1;
  }

  return 0;
}

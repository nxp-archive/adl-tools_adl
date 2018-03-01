
%defines
%define api.pure full
%define api.push-pull push
%define parse.error verbose
%name-prefix "uvp"

%{

#include <string>
#include <iostream>
#include <stdexcept>

  namespace adl {
    class  UvpParser;
    class  UvpFlexLexer;
    struct UvpToken;
    struct UvpValue;
    struct UvpList;
    struct UvpMap;
    struct UvpCmd;
  }
%}

%parse-param { adl::UvpParser& driver }
%locations
%debug

%union
{
  adl::UvpToken     *tok;
  adl::UvpValue     *val;
  adl::UvpList      *list;
  adl::UvpMap       *map;
  adl::UvpCmd       *cmd;

  struct {
    adl::UvpToken *first;
    adl::UvpValue *second;
  } mi;
};

%{

#include <sstream>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"

#include "iss/UvpParser.h"

#include "iss/UvpParseTypes.h"

  using namespace std;
  using namespace rnumber;
  using namespace adl;

%}

%token        END      0 "end of file"
%token        ARROW      "=>"
%token <tok>  IDENT      "identifier"
%token <tok>  NUMBER     "number"
%token <tok>  STRING     "str"
%token        UVP_EOF    "eof"

%type  <tok>  test_port test_cmd test_instance block_type block_name
%type  <tok>  string     "string"
%type  <val>  value      "value"
%type  <list> vlist value_list
%type  <map>  vmap value_map
%type  <mi>   map_item   "map item"
%type  <mi>   parm parm_item
%type  <cmd>  parm_list parm_list_opt_eos cblock cmd

%%

%start start;

/* The parser accepts single items at a time so that a UVP file with a series of tests
   can be parsed incrementally:  The parse routine will return after each outer item
   has been parsed. */
start: item {
  driver.set_test_finished();
  YYACCEPT;
}
| UVP_EOF {
  driver.set_finished();
  YYACCEPT;
}

item: block 
| parm {
  // Handle a parameter.
  driver.handle_parm(@$,make_uniq_p($1.first),make_uniq_p($1.second));
}
| cmd {
  // Handle a command.
  driver.handle_cmd(@$,make_uniq_p($1));
}
| eos

item_list: item
| item_list item

/* A block notes a level of hierarchy.  This is reported to the user's derived class before we parse
   the block body.  We then signal the end of the block to let the user know that a level of hierarchy has
   been completed. */
block: block_type ARROW block_name  {
  driver.handle_block_begin(@$,make_uniq_p($1),make_uniq_p($3));
} '{' item_list '}' {
  driver.handle_block_end(@7);
}

/* A command is a port and command name followed by a block of key-value pairs. */
cmd: test_port ':' test_cmd test_instance cblock {
  $$ = $5;
  $$->_port.reset($1);
  $$->_cmd.reset($3);
  $$->_instance.reset($4);
{
  assert($3->data());
}

}

/* Command block. */
cblock: '{' parm_list_opt_eos '}'
{
  $$ = $2;
}

parm_list_opt_eos: {
  $$ = new UvpCmd;
}
| parm_list opt_eos {
  $$ = $1;
  $1->_parms.finalize();
}

/* A parm-list is a list of key-value pairs. */
parm_list: parm_item {
  $$ = new UvpCmd;
  if ( $1.first ) {
    $$->_parms.push_back(make_pair(*($1.first),unique_ptr<UvpValue>($1.second)));
    delete $1.first;
  }
}
| parm_list eos parm_item {
  $$ = $1;
  // Ignore empty parameters.
  if ($3.first) {
    $$->_parms.push_back(make_pair(*($3.first),unique_ptr<UvpValue>($3.second)));    
    delete $3.first;
  }
}

/* This just allows for extra separators. */
parm_item: parm | eos {
  $$.first = 0;
  $$.second = 0;
}

/* A parameter is a key-value pair. */
parm: IDENT '=' value {
  $$.first = $1;
  $$.second = $3;
}
| IDENT {
  $$.first = $1;
  $$.second = 0;
}

opt_eos:
| eos;

eos: ';'

block_type: IDENT

block_name: IDENT

test_port: IDENT

test_cmd: IDENT

test_instance: {
  $$ = 0;
}
| IDENT

value: NUMBER {
  $$ = $1;
}
| string {
  $$ = $1;
}
| IDENT {
  $$ = $1;
}
| vlist {
  $$ = $1;
}
| vmap {
  $$ = $1;
};

/* If we come across a sequence of strings, then we implicitly concatenate them together. */
string: STRING
| string STRING
{
  $$ = new StrToken($1->append(*$2));
  delete $1;
  delete $2;
};


// We build up a list here.
vlist: '{' value_list '}' {
  $$ = $2;
}
| '{' value_list ',' '}' {
  $$ = $2;
}
| '{' '}' {
  $$ = new UvpList;
}

value_list: value {
  $$ = new UvpList;
  $$->push_back(make_uniq_p($1));
}
| value_list ',' value {
  $$ = $1;
  $$->push_back(make_uniq_p($3));
};

vmap: '[' value_map ']' {
  $$ = $2;
  $$->finalize();
}
| '[' value_map ',' ']' {
  $$ = $2;
  $$->finalize();
}
| '[' ']' {
  $$ = new UvpMap;
}

// We build up a map here.
value_map: map_item {
  $$ = new UvpMap;
  $$->push_back(make_pair(*($1.first),unique_ptr<UvpValue>($1.second)));
  delete $1.first;
}
| value_map ',' map_item {
  $$ = $1;
  $$->push_back(make_pair(*($3.first),unique_ptr<UvpValue>($3.second)));
  delete $3.first;
};

map_item: IDENT ARROW value {
  // A normal map item of the form "key => value".
  $$.first = $1;
  $$.second = $3;
}
| IDENT {
  // Single elements in a map are allowed.  They're taken to be keys with
  // null values.
  $$.first = $1;
  $$.second = 0;
};

%%

#undef yyFlexLexer
#define yyFlexLexer uvpFlexLexer
#include "FlexLexer.h"
#include "iss/UvpFlexLexer.h"

namespace adl {

  UvpParser::UvpParser(const std::string &fn,bool trace_parsing,bool trace_scanning) :
    _fn(fn),
    _trace_parsing(trace_parsing),
    _lexer(new UvpFlexLexer(fn,trace_scanning)),
    _parser(uvppstate_new()),
    _busy(false),
    _finished(false),
    _test_finished(false),
    _restarted(false)
  {
    memset(&_value,0,sizeof(YYSTYPE));
  }

  UvpParser::~UvpParser()
  {
    delete _lexer;
    uvppstate_delete(_parser);
  }

  void UvpParser::reset()
  {
    _busy = false;
    _finished = false;
    _test_finished = false;
    _lexer->reset();
  }

  void UvpParser::parse()
  {
    if (_finished) {
      return;
    }

    uvpdebug = _trace_parsing;
    _busy = true;
    do {
      uvppush_parse(_parser,_lexer->yylex(_value,_loc),&_value,&_loc,*this);
    } while (_busy);
  }

  void UvpParser::setFullPathErrors(bool f)
  {
    _fn = (f) ? _lexer->filename() : base_name(_lexer->filename());
  }

  string UvpParser::printLocation(const YYLTYPE &l) 
  {
    ostringstream ss;

    int last_column = l.last_column-1;

    ss << _fn << ':' << l.first_line << '.' << l.first_column;
    if (l.first_line != l.last_line) {
      ss << '-' << l.last_line << '.' << last_column;
    } else if (l.first_column != last_column) {
      ss << '-' << last_column;
    }

    return ss.str();
  }

  void UvpParser::error (const YYLTYPE& l, const char *m)
  {
    RError( printLocation(l) << ": " << m << '\n');
  }

  void UvpList::print(ostream &o) const
  {
    o << "{ ";
    const UvpList &me = *this;
    ForEach( me, i ) {
      o << *i << ", ";
    }
    o << " }";
  }

  void UvpMap::print(ostream &o) const
  {
    o << "[ ";
    const UvpMap &me = *this;
    ForEach( me, i ) {
      o << (i->first);
      if ( i->second ) {
        o << " => " << i->second;
      }
      o << ", ";
    }
    o << " ]";
  }

  void UvpToken::print(ostream &o) const
  {
    o << static_cast<const Token &>(*this);
  }

  void StrToken::print(ostream &o) const
  {
    o << '"' << static_cast<const Token &>(*this) << '"';
  }

  bool NumToken::operator==(const UvpValue *uv) const
  {
    if (const NumToken *t = dynamic_cast<const NumToken*>(uv)) {
      RNumber x(str());
      RNumber y(t->str());
      return x == y;
    } else {
      return false;
    }
  }

  ostream &operator<< (ostream &o,const UvpCmd &c)
  {
    o << c._port << " : " << c._cmd << ' ';
    if (c._instance) {
      o << c._instance << ' ';
    }
    o << "{ ";
    ForEach( c._parms, i ) {
      o << (i->first);
      if ( i->second ) {
        o << " => " << i->second;
      }
      o << "; ";
    }
    o << " }";
    return o;
  }

  const UvpValue &find_key(const UvpMap &parms,const char *n,const char *msg)
  {
    UvpMap::const_iterator iter = parms.find_key(n);
    if (parms.not_found(iter)) {
      RError("Could not find key '" << n << "' for " << msg);
    } else {
      return *iter->second;
    }
  }

  const UvpValue &find_key(const UvpCmd &cmd,const char *n,const char *msg)
  {
    return find_key(cmd._parms,n,msg);
  }

  string uvtostr(const UvpValue &v,const char *msg)
  {
    return get_uvptype<StrToken>(v,"string",msg).str();
  }

  RNumber uvtorn(const UvpValue &v,const char *msg)
  {
    return RNumber(get_uvptype<NumToken>(v,"number",msg).str());
  }

  uint64_t uvtoi64(const UvpValue &v,const char *msg)
  {
    return atoi64(get_uvptype<NumToken>(v,"number",msg).data(),msg);
  }

  uint32_t uvtoi32(const UvpValue &v,const char *msg)
  {
    return atoi32(get_uvptype<NumToken>(v,"number",msg).data(),msg);
  }

  // Looks up a key and returns the token value.
  string get_str(const UvpCmd &cmd,const char *k,const char *msg)
  {
    return uvtostr(find_key(cmd,k,msg),msg);
  }

  // Looks up a key and tries to convert it to an RNumber.  Throws an error
  // if there's a problem.
  RNumber get_rn(const UvpCmd &cmd,const char *k,const char *msg)
  {
    return uvtorn(find_key(cmd,k,msg),msg);
  }

  // Looks up a key and tries to convert it to a uint64_t.  If the key isn't found or
  // the data is not an integer, throws an error w/the specified message.
  uint64_t get_uint64(const UvpCmd &cmd,const char *k,const char *msg)
  {
    return uvtoi64(find_key(cmd,k,msg),msg);
  }

  uint32_t get_uint32(const UvpCmd &cmd,const char *k,const char *msg)
  {
    return uvtoi32(find_key(cmd,k,msg),msg);
  }

  int32_t get_int32(const UvpCmd &cmd,const char *k,const char *msg)
  {
    return uvtoi32(find_key(cmd,k,msg),msg);
  }

  void uvptype_error(const char *tmsg,const char *msg)
  {
    RError("Expected a " << tmsg << " for " << msg);
  }
}

void uvperror(const YYLTYPE* l,adl::UvpParser &driver,const char *m)
{
  driver.error(*l,m);
}

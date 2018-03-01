%{

  //
  // Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
  //
  // You may distribute under the terms of the Artistic License, as specified in
  // the COPYING file.
  //
  // uvptokenizer.ll: Scanner for the uvp parser.  Comments are extracted and the
  // rest is passed to the yaccer.
  //

#include <string>
#include <stdexcept>
#include <stdio.h>

#include "iss/UvpParseTypes.h"

#include "iss/UvpParser.h"
#include "iss/uvpparser.hh"
#include "iss/UvpFlexLexer.h"

  using namespace std; 
  using namespace adl;

// By default yylex returns int but we use token_type.
// Unfortunately yyterminate by default returns 0, which is
// not of token_type.
#define yyterminate() return token::END

%}

%option batch
%option c++
%option noyywrap
%option nounput
%option never-interactive
%option debug
%option prefix="uvp"
%option yyclass="UvpFlexLexer"

%x comment


WS                      [ \t]
B                       [01_]
H                       [a-fA-F0-9_]
D                       [0-9]
L                       [a-zA-Z_]
bin                     0[bB]{B}+
dec                     {D}+
hex                     0[xX]{H}+
ident                   [a-zA-Z_\.][a-zA-Z_0-9\.]*
blank                   [ \t]

%{
# define YY_USER_ACTION  yyloc.columns (yyleng);
%}
%%
%{
  yyloc.step ();
%}
{blank}+   yyloc.step ();
[\n]+      yyloc.lines (yyleng); yyloc.step ();

%{
  typedef int token;
%}

  /* Comment handling. */

"//".*\n                 yyloc.lines (1); yyloc.step(); // C++ style comment

"/*"                     BEGIN(comment);
<comment>[^*\n]*         
<comment>[^*\n]*\n       yyloc.lines(1); yyloc.step();
<comment>"*"+[^*/\n]*    
<comment>"*"+[^*/\n]*\n  yyloc.lines(1); yyloc.step();
<comment>"*/"            BEGIN(INITIAL);

"=>"                    { return ARROW; };

{ident}                 { yylval.tok = new  IdentToken(yystr(),YYLeng()); return IDENT; }

{hex}                   { yylval.tok = new  NumToken(yystr(),YYLeng()); return NUMBER; }
{dec}                   { yylval.tok = new  NumToken(yystr(),YYLeng()); return NUMBER; }
{bin}                   { yylval.tok = new  NumToken(yystr(),YYLeng()); return NUMBER; }

  /* Double-quote delimited strings.  We allow for escaped characters but we do not do any character substitutions. */
\"(\\.|[^\\"])*\"       { yylval.tok = new   StrToken(yystr()+1,YYLeng()-2); return STRING; }

<<EOF>>                 { return UVP_EOF; }

.                       { return YYText()[0]; }

%%

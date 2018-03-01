/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_UVP_ISS_UVPPARSER_HH_INCLUDED
# define YY_UVP_ISS_UVPPARSER_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int uvpdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    ARROW = 258,
    IDENT = 259,
    NUMBER = 260,
    STRING = 261,
    UVP_EOF = 262
  };
#endif
/* Tokens.  */
#define END 0
#define ARROW 258
#define IDENT 259
#define NUMBER 260
#define STRING 261
#define UVP_EOF 262

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 30 "iss/uvpparser.yy" /* yacc.c:1909  */

  adl::UvpToken     *tok;
  adl::UvpValue     *val;
  adl::UvpList      *list;
  adl::UvpMap       *map;
  adl::UvpCmd       *cmd;

  struct {
    adl::UvpToken *first;
    adl::UvpValue *second;
  } mi;

#line 83 "iss/uvpparser.hh" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



#ifndef YYPUSH_MORE_DEFINED
# define YYPUSH_MORE_DEFINED
enum { YYPUSH_MORE = 4 };
#endif

typedef struct uvppstate uvppstate;

int uvppush_parse (uvppstate *ps, int pushed_char, YYSTYPE const *pushed_val, YYLTYPE *pushed_loc, adl::UvpParser& driver);

uvppstate * uvppstate_new (void);
void uvppstate_delete (uvppstate *ps);

#endif /* !YY_UVP_ISS_UVPPARSER_HH_INCLUDED  */

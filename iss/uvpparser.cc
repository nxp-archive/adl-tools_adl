/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 1

/* Pull parsers.  */
#define YYPULL 0


/* Substitute the variable and function names.  */
#define yypush_parse    uvppush_parse
#define yypstate_new    uvppstate_new
#define yypstate_delete uvppstate_delete
#define yypstate        uvppstate
#define yylex           uvplex
#define yyerror         uvperror
#define yydebug         uvpdebug
#define yynerrs         uvpnerrs


/* Copy the first part of user declarations.  */
#line 8 "iss/uvpparser.yy" /* yacc.c:339  */


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

#line 92 "iss/uvpparser.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "uvpparser.hh".  */
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
#line 30 "iss/uvpparser.yy" /* yacc.c:355  */

  adl::UvpToken     *tok;
  adl::UvpValue     *val;
  adl::UvpList      *list;
  adl::UvpMap       *map;
  adl::UvpCmd       *cmd;

  struct {
    adl::UvpToken *first;
    adl::UvpValue *second;
  } mi;

#line 161 "iss/uvpparser.cc" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */
#line 43 "iss/uvpparser.yy" /* yacc.c:358  */


#include <sstream>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"

#include "iss/UvpParser.h"

#include "iss/UvpParseTypes.h"

  using namespace std;
  using namespace rnumber;
  using namespace adl;


#line 217 "iss/uvpparser.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   65

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  19
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  49
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  67

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   265

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    16,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    13,    15,
       2,    14,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    17,     2,    18,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    11,     2,    12,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    85,    85,    89,    94,    95,    99,   103,   105,   106,
     111,   111,   118,   130,   135,   138,   144,   151,   161,   161,
     167,   171,   176,   177,   179,   181,   183,   185,   187,   189,
     192,   194,   197,   200,   203,   206,   211,   212,   221,   224,
     227,   231,   235,   240,   244,   248,   253,   258,   264,   269
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "\"=>\"", "\"identifier\"",
  "\"number\"", "\"str\"", "\"eof\"", "\"string\"", "\"value\"",
  "\"map item\"", "'{'", "'}'", "':'", "'='", "';'", "','", "'['", "']'",
  "$accept", "start", "item", "item_list", "block", "$@1", "cmd", "cblock",
  "parm_list_opt_eos", "parm_list", "parm_item", "parm", "opt_eos", "eos",
  "block_type", "block_name", "test_port", "test_cmd", "test_instance",
  "value", "string", "vlist", "value_list", "vmap", "value_map",
  "map_item", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   123,   125,    58,    61,    59,    44,    91,    93
};
# endif

#define YYPACT_NINF -46

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-46)))

#define YYTABLE_NINF -28

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      26,    32,   -46,   -46,    10,   -46,   -46,   -46,   -46,   -46,
      12,    30,    23,   -46,    40,    49,   -46,   -46,   -46,     2,
      -1,   -46,    48,   -46,   -46,   -46,   -46,   -46,    51,   -46,
     -46,    36,    33,   -46,    -7,   -46,   -46,    45,   -46,    46,
     -46,    20,    23,     0,   -46,     1,    34,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,    35,    44,    47,    50,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,    34,   -46
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    21,     3,    24,     0,     2,     4,     6,     5,     7,
       0,     0,     0,     1,     0,     0,    33,    31,    36,     0,
       0,    20,    32,    34,    35,    26,    10,    28,    29,    40,
      41,     0,    49,    45,     0,    46,    37,     0,    30,     0,
      38,     0,     0,     0,    43,     0,    14,    12,    39,    42,
      48,    44,    47,     8,     0,    21,     0,    22,    16,    18,
      19,    11,     9,    13,    15,    23,    17
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -46,   -46,    -3,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
      -5,   -44,   -46,   -45,   -46,   -46,   -46,   -46,   -46,   -19,
     -46,   -46,   -46,   -46,   -46,    18
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,    54,     6,    37,     7,    47,    56,    57,
      58,     8,    64,     9,    10,    26,    11,    28,    39,    21,
      22,    23,    31,    24,    34,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      30,    60,    59,    32,    32,     1,    16,    17,    18,    43,
      13,    44,    65,    19,    29,    14,     3,    33,    51,    20,
      60,    59,    49,    50,    16,    17,    18,    16,    17,    18,
       1,    19,    48,     2,    19,   -25,    42,    20,    55,     1,
      20,     3,    53,    15,    25,   -27,    12,    61,    40,     3,
       3,    62,    41,    27,    36,    38,    45,    46,    12,    63,
      66,    52,     0,     0,     0,     3
};

static const yytype_int8 yycheck[] =
{
      19,    46,    46,     4,     4,     4,     4,     5,     6,    16,
       0,    18,    57,    11,    12,     3,    15,    18,    18,    17,
      65,    65,    41,    42,     4,     5,     6,     4,     5,     6,
       4,    11,    12,     7,    11,     3,     3,    17,     4,     4,
      17,    15,    45,    13,     4,    13,    14,    12,    12,    15,
      15,    54,    16,     4,     6,     4,    11,    11,    14,    12,
      65,    43,    -1,    -1,    -1,    15
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     7,    15,    20,    21,    23,    25,    30,    32,
      33,    35,    14,     0,     3,    13,     4,     5,     6,    11,
      17,    38,    39,    40,    42,     4,    34,     4,    36,    12,
      38,    41,     4,    18,    43,    44,     6,    24,     4,    37,
      12,    16,     3,    16,    18,    11,    11,    26,    12,    38,
      38,    18,    44,    21,    22,     4,    27,    28,    29,    30,
      32,    12,    21,    12,    31,    32,    29
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    19,    20,    20,    21,    21,    21,    21,    22,    22,
      24,    23,    25,    26,    27,    27,    28,    28,    29,    29,
      30,    30,    31,    31,    32,    33,    34,    35,    36,    37,
      37,    38,    38,    38,    38,    38,    39,    39,    40,    40,
      40,    41,    41,    42,    42,    42,    43,    43,    44,    44
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       0,     7,     5,     3,     0,     2,     1,     3,     1,     1,
       3,     1,     0,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     4,
       2,     1,     3,     3,     4,     2,     1,     3,     3,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, driver, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, driver); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, adl::UvpParser& driver)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (driver);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, adl::UvpParser& driver)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, driver);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, adl::UvpParser& driver)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , driver);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, driver); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, adl::UvpParser& driver)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (driver);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}



struct yypstate
  {
    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;
    /* Used to determine if this is the first time this instance has
       been used.  */
    int yynew;
  };

/* Initialize the parser data structure.  */
yypstate *
yypstate_new (void)
{
  yypstate *yyps;
  yyps = (yypstate *) malloc (sizeof *yyps);
  if (!yyps)
    return YY_NULLPTR;
  yyps->yynew = 1;
  return yyps;
}

void
yypstate_delete (yypstate *yyps)
{
#ifndef yyoverflow
  /* If the stack was reallocated but the parse did not complete, then the
     stack still needs to be freed.  */
  if (!yyps->yynew && yyps->yyss != yyps->yyssa)
    YYSTACK_FREE (yyps->yyss);
#endif
  free (yyps);
}

#define uvpnerrs yyps->uvpnerrs
#define yystate yyps->yystate
#define yyerrstatus yyps->yyerrstatus
#define yyssa yyps->yyssa
#define yyss yyps->yyss
#define yyssp yyps->yyssp
#define yyvsa yyps->yyvsa
#define yyvs yyps->yyvs
#define yyvsp yyps->yyvsp
#define yylsa yyps->yylsa
#define yyls yyps->yyls
#define yylsp yyps->yylsp
#define yyerror_range yyps->yyerror_range
#define yystacksize yyps->yystacksize


/*---------------.
| yypush_parse.  |
`---------------*/

int
yypush_parse (yypstate *yyps, int yypushed_char, YYSTYPE const *yypushed_val, YYLTYPE *yypushed_loc, adl::UvpParser& driver)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  if (!yyps->yynew)
    {
      yyn = yypact[yystate];
      goto yyread_pushed_token;
    }

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = *yypushed_loc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      if (!yyps->yynew)
        {
          YYDPRINTF ((stderr, "Return for a new token:\n"));
          yyresult = YYPUSH_MORE;
          goto yypushreturn;
        }
      yyps->yynew = 0;
yyread_pushed_token:
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yypushed_char;
      if (yypushed_val)
        yylval = *yypushed_val;
      if (yypushed_loc)
        yylloc = *yypushed_loc;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 85 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  driver.set_test_finished();
  YYACCEPT;
}
#line 1481 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 89 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  driver.set_finished();
  YYACCEPT;
}
#line 1490 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 95 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  // Handle a parameter.
  driver.handle_parm((yyloc),make_uniq_p((yyvsp[0].mi).first),make_uniq_p((yyvsp[0].mi).second));
}
#line 1499 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 99 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  // Handle a command.
  driver.handle_cmd((yyloc),make_uniq_p((yyvsp[0].cmd)));
}
#line 1508 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 111 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  driver.handle_block_begin((yyloc),make_uniq_p((yyvsp[-2].tok)),make_uniq_p((yyvsp[0].tok)));
}
#line 1516 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 113 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  driver.handle_block_end((yylsp[0]));
}
#line 1524 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 118 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.cmd) = (yyvsp[0].cmd);
  (yyval.cmd)->_port.reset((yyvsp[-4].tok));
  (yyval.cmd)->_cmd.reset((yyvsp[-2].tok));
  (yyval.cmd)->_instance.reset((yyvsp[-1].tok));
{
  assert((yyvsp[-2].tok)->data());
}

}
#line 1539 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 131 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.cmd) = (yyvsp[-1].cmd);
}
#line 1547 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 135 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.cmd) = new UvpCmd;
}
#line 1555 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 138 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.cmd) = (yyvsp[-1].cmd);
  (yyvsp[-1].cmd)->_parms.finalize();
}
#line 1564 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 144 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.cmd) = new UvpCmd;
  if ( (yyvsp[0].mi).first ) {
    (yyval.cmd)->_parms.push_back(make_pair(*((yyvsp[0].mi).first),unique_ptr<UvpValue>((yyvsp[0].mi).second)));
    delete (yyvsp[0].mi).first;
  }
}
#line 1576 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 151 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.cmd) = (yyvsp[-2].cmd);
  // Ignore empty parameters.
  if ((yyvsp[0].mi).first) {
    (yyval.cmd)->_parms.push_back(make_pair(*((yyvsp[0].mi).first),unique_ptr<UvpValue>((yyvsp[0].mi).second)));    
    delete (yyvsp[0].mi).first;
  }
}
#line 1589 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 161 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.mi).first = 0;
  (yyval.mi).second = 0;
}
#line 1598 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 167 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.mi).first = (yyvsp[-2].tok);
  (yyval.mi).second = (yyvsp[0].val);
}
#line 1607 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 171 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.mi).first = (yyvsp[0].tok);
  (yyval.mi).second = 0;
}
#line 1616 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 189 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.tok) = 0;
}
#line 1624 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 194 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.val) = (yyvsp[0].tok);
}
#line 1632 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 197 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.val) = (yyvsp[0].tok);
}
#line 1640 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 200 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.val) = (yyvsp[0].tok);
}
#line 1648 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 203 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.val) = (yyvsp[0].list);
}
#line 1656 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 206 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.val) = (yyvsp[0].map);
}
#line 1664 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 213 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.tok) = new StrToken((yyvsp[-1].tok)->append(*(yyvsp[0].tok)));
  delete (yyvsp[-1].tok);
  delete (yyvsp[0].tok);
}
#line 1674 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 221 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.list) = (yyvsp[-1].list);
}
#line 1682 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 224 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.list) = (yyvsp[-2].list);
}
#line 1690 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 227 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.list) = new UvpList;
}
#line 1698 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 231 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.list) = new UvpList;
  (yyval.list)->push_back(make_uniq_p((yyvsp[0].val)));
}
#line 1707 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 235 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.list) = (yyvsp[-2].list);
  (yyval.list)->push_back(make_uniq_p((yyvsp[0].val)));
}
#line 1716 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 240 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.map) = (yyvsp[-1].map);
  (yyval.map)->finalize();
}
#line 1725 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 244 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.map) = (yyvsp[-2].map);
  (yyval.map)->finalize();
}
#line 1734 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 248 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.map) = new UvpMap;
}
#line 1742 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 253 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.map) = new UvpMap;
  (yyval.map)->push_back(make_pair(*((yyvsp[0].mi).first),unique_ptr<UvpValue>((yyvsp[0].mi).second)));
  delete (yyvsp[0].mi).first;
}
#line 1752 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 258 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  (yyval.map) = (yyvsp[-2].map);
  (yyval.map)->push_back(make_pair(*((yyvsp[0].mi).first),unique_ptr<UvpValue>((yyvsp[0].mi).second)));
  delete (yyvsp[0].mi).first;
}
#line 1762 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 264 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  // A normal map item of the form "key => value".
  (yyval.mi).first = (yyvsp[-2].tok);
  (yyval.mi).second = (yyvsp[0].val);
}
#line 1772 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 269 "iss/uvpparser.yy" /* yacc.c:1646  */
    {
  // Single elements in a map are allowed.  They're taken to be keys with
  // null values.
  (yyval.mi).first = (yyvsp[0].tok);
  (yyval.mi).second = 0;
}
#line 1783 "iss/uvpparser.cc" /* yacc.c:1646  */
    break;


#line 1787 "iss/uvpparser.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, driver, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, driver, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, driver);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, driver);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, driver, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, driver);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, driver);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  yyps->yynew = 1;

yypushreturn:
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 276 "iss/uvpparser.yy" /* yacc.c:1906  */


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

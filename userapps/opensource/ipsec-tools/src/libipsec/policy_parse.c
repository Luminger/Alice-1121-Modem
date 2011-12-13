/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse __libipsecparse
#define yylex   __libipseclex
#define yyerror __libipsecerror
#define yylval  __libipseclval
#define yychar  __libipsecchar
#define yydebug __libipsecdebug
#define yynerrs __libipsecnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DIR = 258,
     PRIORITY = 259,
     PLUS = 260,
     PRIO_BASE = 261,
     PRIO_OFFSET = 262,
     ACTION = 263,
     PROTOCOL = 264,
     MODE = 265,
     LEVEL = 266,
     LEVEL_SPECIFY = 267,
     IPADDRESS = 268,
     ME = 269,
     ANY = 270,
     SLASH = 271,
     HYPHEN = 272
   };
#endif
#define DIR 258
#define PRIORITY 259
#define PLUS 260
#define PRIO_BASE 261
#define PRIO_OFFSET 262
#define ACTION 263
#define PROTOCOL 264
#define MODE 265
#define LEVEL 266
#define LEVEL_SPECIFY 267
#define IPADDRESS 268
#define ME 269
#define ANY 270
#define SLASH 271
#define HYPHEN 272




/* Copy the first part of user declarations.  */
#line 63 "policy_parse.y"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <netinet/in.h>
#ifdef HAVE_NETINET6_IPSEC
#  include <netinet6/ipsec.h>
#else
#  include <netinet/ipsec.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#include <errno.h>

#include "config.h"

#include "ipsec_strerror.h"
#include "libpfkey.h"

#ifndef INT32_MAX
#define INT32_MAX	(0xffffffff)
#endif

#ifndef INT32_MIN
#define INT32_MIN	(-INT32_MAX-1)
#endif

#define ATOX(c) \
  (isdigit(c) ? (c - '0') : (isupper(c) ? (c - 'A' + 10) : (c - 'a' + 10) ))

static u_int8_t *pbuf = NULL;		/* sadb_x_policy buffer */
static int tlen = 0;			/* total length of pbuf */
static int offset = 0;			/* offset of pbuf */
static int p_dir, p_type, p_protocol, p_mode, p_level, p_reqid;
static u_int32_t p_priority = 0;
static long p_priority_offset = 0;
static struct sockaddr *p_src = NULL;
static struct sockaddr *p_dst = NULL;

struct _val;
extern void yyerror __P((char *msg));
static struct sockaddr *parse_sockaddr __P((struct _val *buf));
static int rule_check __P((void));
static int init_x_policy __P((void));
static int set_x_request __P((struct sockaddr *src, struct sockaddr *dst));
static int set_sockaddr __P((struct sockaddr *addr));
static void policy_parse_request_init __P((void));
static caddr_t policy_parse __P((char *msg, int msglen));

extern void __policy__strbuffer__init__ __P((char *msg));
extern void __policy__strbuffer__free__ __P((void));
extern int yyparse __P((void));
extern int yylex __P((void));

extern char *__libipsectext;	/*XXX*/



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 130 "policy_parse.y"
typedef union YYSTYPE {
	u_int num;
	u_int32_t num32;
	struct _val {
		int len;
		char *buf;
	} val;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 194 "policy_parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 206 "policy_parse.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   44

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  18
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  15
/* YYNRULES -- Number of rules. */
#define YYNRULES  32
/* YYNRULES -- Number of states. */
#define YYNSTATES  55

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   272

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     9,    10,    17,    18,    26,    27,
      34,    35,    44,    45,    54,    56,    57,    60,    68,    75,
      81,    86,    93,    97,   100,   102,   104,   106,   108,   110,
     111,   116,   120
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      19,     0,    -1,    -1,     3,     8,    20,    26,    -1,    -1,
       3,     4,     7,     8,    21,    26,    -1,    -1,     3,     4,
      17,     7,     8,    22,    26,    -1,    -1,     3,     4,     6,
       8,    23,    26,    -1,    -1,     3,     4,     6,     5,     7,
       8,    24,    26,    -1,    -1,     3,     4,     6,    17,     7,
       8,    25,    26,    -1,     3,    -1,    -1,    26,    27,    -1,
      28,    16,    29,    16,    31,    16,    30,    -1,    28,    16,
      29,    16,    31,    16,    -1,    28,    16,    29,    16,    31,
      -1,    28,    16,    29,    16,    -1,    28,    16,    29,    16,
      16,    30,    -1,    28,    16,    29,    -1,    28,    16,    -1,
      28,    -1,     9,    -1,    10,    -1,    11,    -1,    12,    -1,
      -1,    13,    32,    17,    13,    -1,    14,    17,    15,    -1,
      15,    17,    14,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   152,   152,   151,   167,   166,   203,   202,   225,   224,
     236,   235,   258,   257,   279,   291,   293,   305,   306,   307,
     308,   309,   310,   311,   315,   322,   326,   330,   334,   341,
     341,   352,   358
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DIR", "PRIORITY", "PLUS", "PRIO_BASE", 
  "PRIO_OFFSET", "ACTION", "PROTOCOL", "MODE", "LEVEL", "LEVEL_SPECIFY", 
  "IPADDRESS", "ME", "ANY", "SLASH", "HYPHEN", "$accept", "policy_spec", 
  "@1", "@2", "@3", "@4", "@5", "@6", "rules", "rule", "protocol", "mode", 
  "level", "addresses", "@7", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    18,    20,    19,    21,    19,    22,    19,    23,    19,
      24,    19,    25,    19,    19,    26,    26,    27,    27,    27,
      27,    27,    27,    27,    27,    28,    29,    30,    30,    32,
      31,    31,    31
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     4,     0,     6,     0,     7,     0,     6,
       0,     8,     0,     8,     1,     0,     2,     7,     6,     5,
       4,     6,     3,     2,     1,     1,     1,     1,     1,     0,
       4,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    14,     0,     0,     2,     1,     0,     0,     0,    15,
       0,     8,     0,     4,     0,     3,     0,    15,     0,    15,
       6,    25,    16,    24,    10,     9,    12,     5,    15,    23,
      15,    15,     7,    26,    22,    11,    13,    20,    29,     0,
       0,     0,    19,     0,     0,     0,    27,    28,    21,    18,
       0,    31,    32,    17,    30
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,     9,    19,    28,    17,    30,    31,    15,    22,
      23,    34,    48,    42,    43
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -19
static const yysigned_char yypact[] =
{
       7,    -3,     4,     1,   -19,   -19,    -2,    15,     2,   -19,
      17,   -19,    18,   -19,    19,     3,    20,   -19,    21,   -19,
     -19,   -19,   -19,    10,   -19,     3,   -19,     3,   -19,    22,
     -19,   -19,     3,   -19,    14,     3,     3,     6,   -19,    16,
      23,     5,    25,    26,    24,    28,   -19,   -19,   -19,     5,
      31,   -19,   -19,   -19,   -19
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -19,   -19,   -19,   -19,   -19,   -19,   -19,   -19,   -17,   -19,
     -19,   -19,   -18,   -19,   -19
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      25,     3,    27,    10,     5,     4,    11,     6,     7,    14,
       1,    32,    21,    35,    36,    12,    46,    47,     8,    38,
      39,    40,    41,    13,    16,    18,    29,    20,    24,    26,
      37,    53,    33,    44,     0,     0,     0,     0,     0,    51,
      45,    49,    52,    50,    54
};

static const yysigned_char yycheck[] =
{
      17,     4,    19,     5,     0,     8,     8,     6,     7,     7,
       3,    28,     9,    30,    31,    17,    11,    12,    17,    13,
      14,    15,    16,     8,     7,     7,    16,     8,     8,     8,
      16,    49,    10,    17,    -1,    -1,    -1,    -1,    -1,    15,
      17,    16,    14,    17,    13
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,    19,     4,     8,     0,     6,     7,    17,    20,
       5,     8,    17,     8,     7,    26,     7,    23,     7,    21,
       8,     9,    27,    28,     8,    26,     8,    26,    22,    16,
      24,    25,    26,    10,    29,    26,    26,    16,    13,    14,
      15,    16,    31,    32,    17,    17,    11,    12,    30,    16,
      17,    15,    14,    30,    13
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 152 "policy_parse.y"
    {
			p_dir = yyvsp[-1].num;
			p_type = yyvsp[0].num;

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			p_priority = PRIORITY_DEFAULT;
#else
			p_priority = 0;
#endif

			if (init_x_policy())
				return -1;
		}
    break;

  case 4:
#line 167 "policy_parse.y"
    {
			char *offset_buf;

			p_dir = yyvsp[-3].num;
			p_type = yyvsp[0].num;

			/* buffer big enough to hold a prepended negative sign */
			offset_buf = malloc(yyvsp[-1].val.len + 2);
			if (offset_buf == NULL) 
			{
				__ipsec_errcode = EIPSEC_NO_BUFS;
				return -1;
			}

			/* positive input value means higher priority, therefore lower
			   actual value so that is closer to the beginning of the list */
			sprintf (offset_buf, "-%s", yyvsp[-1].val.buf);

			errno = 0;
			p_priority_offset = atol(offset_buf);

			free(offset_buf);

			if (errno != 0 || p_priority_offset < INT32_MIN)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_OFFSET;
				return -1;
			}

			p_priority = PRIORITY_DEFAULT + (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
    break;

  case 6:
#line 203 "policy_parse.y"
    {
			p_dir = yyvsp[-4].num;
			p_type = yyvsp[0].num;

			errno = 0;
			p_priority_offset = atol(yyvsp[-1].val.buf);

			if (errno != 0 || p_priority_offset > INT32_MAX)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_OFFSET;
				return -1;
			}

			/* negative input value means lower priority, therefore higher
			   actual value so that is closer to the end of the list */
			p_priority = PRIORITY_DEFAULT + (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
    break;

  case 8:
#line 225 "policy_parse.y"
    {
			p_dir = yyvsp[-3].num;
			p_type = yyvsp[0].num;

			p_priority = yyvsp[-1].num32;

			if (init_x_policy())
				return -1;
		}
    break;

  case 10:
#line 236 "policy_parse.y"
    {
			p_dir = yyvsp[-5].num;
			p_type = yyvsp[0].num;

			errno = 0;
			p_priority_offset = atol(yyvsp[-1].val.buf);

			if (errno != 0 || p_priority_offset > PRIORITY_OFFSET_NEGATIVE_MAX)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_BASE_OFFSET;
				return -1;
			}

			/* adding value means higher priority, therefore lower
			   actual value so that is closer to the beginning of the list */
			p_priority = yyvsp[-3].num32 - (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
    break;

  case 12:
#line 258 "policy_parse.y"
    {
			p_dir = yyvsp[-5].num;
			p_type = yyvsp[0].num;

			errno = 0;
			p_priority_offset = atol(yyvsp[-1].val.buf);

			if (errno != 0 || p_priority_offset > PRIORITY_OFFSET_POSITIVE_MAX)
			{
				__ipsec_errcode = EIPSEC_INVAL_PRIORITY_BASE_OFFSET;
				return -1;
			}

			/* subtracting value means lower priority, therefore higher
			   actual value so that is closer to the end of the list */
			p_priority = yyvsp[-3].num32 + (u_int32_t) p_priority_offset;

			if (init_x_policy())
				return -1;
		}
    break;

  case 14:
#line 280 "policy_parse.y"
    {
			p_dir = yyvsp[0].num;
			p_type = 0;	/* ignored it by kernel */

			p_priority = 0;

			if (init_x_policy())
				return -1;
		}
    break;

  case 16:
#line 293 "policy_parse.y"
    {
			if (rule_check() < 0)
				return -1;

			if (set_x_request(p_src, p_dst) < 0)
				return -1;

			policy_parse_request_init();
		}
    break;

  case 23:
#line 311 "policy_parse.y"
    {
			__ipsec_errcode = EIPSEC_FEW_ARGUMENTS;
			return -1;
		}
    break;

  case 24:
#line 315 "policy_parse.y"
    {
			__ipsec_errcode = EIPSEC_FEW_ARGUMENTS;
			return -1;
		}
    break;

  case 25:
#line 322 "policy_parse.y"
    { p_protocol = yyvsp[0].num; }
    break;

  case 26:
#line 326 "policy_parse.y"
    { p_mode = yyvsp[0].num; }
    break;

  case 27:
#line 330 "policy_parse.y"
    {
			p_level = yyvsp[0].num;
			p_reqid = 0;
		}
    break;

  case 28:
#line 334 "policy_parse.y"
    {
			p_level = IPSEC_LEVEL_UNIQUE;
			p_reqid = atol(yyvsp[0].val.buf);	/* atol() is good. */
		}
    break;

  case 29:
#line 341 "policy_parse.y"
    {
			p_src = parse_sockaddr(&yyvsp[0].val);
			if (p_src == NULL)
				return -1;
		}
    break;

  case 30:
#line 347 "policy_parse.y"
    {
			p_dst = parse_sockaddr(&yyvsp[0].val);
			if (p_dst == NULL)
				return -1;
		}
    break;

  case 31:
#line 352 "policy_parse.y"
    {
			if (p_dir != IPSEC_DIR_OUTBOUND) {
				__ipsec_errcode = EIPSEC_INVAL_DIR;
				return -1;
			}
		}
    break;

  case 32:
#line 358 "policy_parse.y"
    {
			if (p_dir != IPSEC_DIR_INBOUND) {
				__ipsec_errcode = EIPSEC_INVAL_DIR;
				return -1;
			}
		}
    break;


    }

/* Line 991 of yacc.c.  */
#line 1367 "policy_parse.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__) \
    && !defined __cplusplus
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 369 "policy_parse.y"


void
yyerror(msg)
	char *msg;
{
	fprintf(stderr, "libipsec: %s while parsing \"%s\"\n",
		msg, __libipsectext);

	return;
}

static struct sockaddr *
parse_sockaddr(buf)
	struct _val *buf;
{
	struct addrinfo hints, *res;
	char *serv = NULL;
	int error;
	struct sockaddr *newaddr = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_flags = AI_NUMERICHOST;
	error = getaddrinfo(buf->buf, serv, &hints, &res);
	if (error != 0) {
		yyerror("invalid IP address");
		__ipsec_set_strerror(gai_strerror(error));
		return NULL;
	}

	if (res->ai_addr == NULL) {
		yyerror("invalid IP address");
		__ipsec_set_strerror(gai_strerror(error));
		return NULL;
	}

	newaddr = malloc(res->ai_addrlen);
	if (newaddr == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		freeaddrinfo(res);
		return NULL;
	}
	memcpy(newaddr, res->ai_addr, res->ai_addrlen);

	freeaddrinfo(res);

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return newaddr;
}

static int
rule_check()
{
	if (p_type == IPSEC_POLICY_IPSEC) {
		if (p_protocol == IPPROTO_IP) {
			__ipsec_errcode = EIPSEC_NO_PROTO;
			return -1;
		}

		if (p_mode != IPSEC_MODE_TRANSPORT
		 && p_mode != IPSEC_MODE_TUNNEL) {
			__ipsec_errcode = EIPSEC_INVAL_MODE;
			return -1;
		}

		if (p_src == NULL && p_dst == NULL) {
			 if (p_mode != IPSEC_MODE_TRANSPORT) {
				__ipsec_errcode = EIPSEC_INVAL_ADDRESS;
				return -1;
			}
		}
		else if (p_src->sa_family != p_dst->sa_family) {
			__ipsec_errcode = EIPSEC_FAMILY_MISMATCH;
			return -1;
		}
	}

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static int
init_x_policy()
{
	struct sadb_x_policy *p;

	if (pbuf) {
		free(pbuf);
		tlen = 0;
	}
	pbuf = malloc(sizeof(struct sadb_x_policy));
	if (pbuf == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		return -1;
	}
	tlen = sizeof(struct sadb_x_policy);

	memset(pbuf, 0, tlen);
	p = (struct sadb_x_policy *)pbuf;
	p->sadb_x_policy_len = 0;	/* must update later */
	p->sadb_x_policy_exttype = SADB_X_EXT_POLICY;
	p->sadb_x_policy_type = p_type;
	p->sadb_x_policy_dir = p_dir;
	p->sadb_x_policy_id = 0;
#ifdef HAVE_PFKEY_POLICY_PRIORITY
	p->sadb_x_policy_priority = p_priority;
#else
    /* fail if given a priority and libipsec was not compiled with 
	   priority support */
	if (p_priority != 0)
	{
		__ipsec_errcode = EIPSEC_PRIORITY_NOT_COMPILED;
		return -1;
	}
#endif

	offset = tlen;

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static int
set_x_request(src, dst)
	struct sockaddr *src, *dst;
{
	struct sadb_x_ipsecrequest *p;
	int reqlen;
	caddr_t n;

	reqlen = sizeof(*p)
		+ (src ? sysdep_sa_len(src) : 0)
		+ (dst ? sysdep_sa_len(dst) : 0);
	tlen += reqlen;		/* increment to total length */

	n = realloc(pbuf, tlen);
	if (n == NULL) {
		__ipsec_errcode = EIPSEC_NO_BUFS;
		return -1;
	}
	pbuf = n;
	p = (struct sadb_x_ipsecrequest *)&pbuf[offset];
	p->sadb_x_ipsecrequest_len = reqlen;
	p->sadb_x_ipsecrequest_proto = p_protocol;
	p->sadb_x_ipsecrequest_mode = p_mode;
	p->sadb_x_ipsecrequest_level = p_level;
	p->sadb_x_ipsecrequest_reqid = p_reqid;
	offset += sizeof(*p);

	if (set_sockaddr(src) || set_sockaddr(dst))
		return -1;

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static int
set_sockaddr(addr)
	struct sockaddr *addr;
{
	if (addr == NULL) {
		__ipsec_errcode = EIPSEC_NO_ERROR;
		return 0;
	}

	/* tlen has already incremented */

	memcpy(&pbuf[offset], addr, sysdep_sa_len(addr));

	offset += sysdep_sa_len(addr);

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return 0;
}

static void
policy_parse_request_init()
{
	p_protocol = IPPROTO_IP;
	p_mode = IPSEC_MODE_ANY;
	p_level = IPSEC_LEVEL_DEFAULT;
	p_reqid = 0;
	if (p_src != NULL) {
		free(p_src);
		p_src = NULL;
	}
	if (p_dst != NULL) {
		free(p_dst);
		p_dst = NULL;
	}

	return;
}

static caddr_t
policy_parse(msg, msglen)
	char *msg;
	int msglen;
{
	int error;

	pbuf = NULL;
	tlen = 0;

	/* initialize */
	p_dir = IPSEC_DIR_INVALID;
	p_type = IPSEC_POLICY_DISCARD;
	policy_parse_request_init();
	__policy__strbuffer__init__(msg);

	error = yyparse();	/* it must be set errcode. */
	__policy__strbuffer__free__();

	if (error) {
		if (pbuf != NULL)
			free(pbuf);
		return NULL;
	}

	/* update total length */
	((struct sadb_x_policy *)pbuf)->sadb_x_policy_len = PFKEY_UNIT64(tlen);

	__ipsec_errcode = EIPSEC_NO_ERROR;

	return pbuf;
}

caddr_t
ipsec_set_policy(msg, msglen)
	char *msg;
	int msglen;
{
	caddr_t policy;

	policy = policy_parse(msg, msglen);
	if (policy == NULL) {
		if (__ipsec_errcode == EIPSEC_NO_ERROR)
			__ipsec_errcode = EIPSEC_INVAL_ARGUMENT;
		return NULL;
	}

	__ipsec_errcode = EIPSEC_NO_ERROR;
	return policy;
}



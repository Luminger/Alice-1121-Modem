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



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EOT = 258,
     SLASH = 259,
     BLCL = 260,
     ELCL = 261,
     ADD = 262,
     GET = 263,
     DELETE = 264,
     DELETEALL = 265,
     FLUSH = 266,
     DUMP = 267,
     EXIT = 268,
     PR_ESP = 269,
     PR_AH = 270,
     PR_IPCOMP = 271,
     PR_ESPUDP = 272,
     F_PROTOCOL = 273,
     F_AUTH = 274,
     F_ENC = 275,
     F_REPLAY = 276,
     F_COMP = 277,
     F_RAWCPI = 278,
     F_MODE = 279,
     MODE = 280,
     F_REQID = 281,
     F_EXT = 282,
     EXTENSION = 283,
     NOCYCLICSEQ = 284,
     ALG_AUTH = 285,
     ALG_AUTH_NOKEY = 286,
     ALG_ENC = 287,
     ALG_ENC_NOKEY = 288,
     ALG_ENC_DESDERIV = 289,
     ALG_ENC_DES32IV = 290,
     ALG_ENC_OLD = 291,
     ALG_COMP = 292,
     F_LIFETIME_HARD = 293,
     F_LIFETIME_SOFT = 294,
     F_LIFEBYTE_HARD = 295,
     F_LIFEBYTE_SOFT = 296,
     DECSTRING = 297,
     QUOTEDSTRING = 298,
     HEXSTRING = 299,
     STRING = 300,
     ANY = 301,
     SPDADD = 302,
     SPDDELETE = 303,
     SPDDUMP = 304,
     SPDFLUSH = 305,
     F_POLICY = 306,
     PL_REQUESTS = 307,
     F_AIFLAGS = 308,
     TAGGED = 309
   };
#endif
#define EOT 258
#define SLASH 259
#define BLCL 260
#define ELCL 261
#define ADD 262
#define GET 263
#define DELETE 264
#define DELETEALL 265
#define FLUSH 266
#define DUMP 267
#define EXIT 268
#define PR_ESP 269
#define PR_AH 270
#define PR_IPCOMP 271
#define PR_ESPUDP 272
#define F_PROTOCOL 273
#define F_AUTH 274
#define F_ENC 275
#define F_REPLAY 276
#define F_COMP 277
#define F_RAWCPI 278
#define F_MODE 279
#define MODE 280
#define F_REQID 281
#define F_EXT 282
#define EXTENSION 283
#define NOCYCLICSEQ 284
#define ALG_AUTH 285
#define ALG_AUTH_NOKEY 286
#define ALG_ENC 287
#define ALG_ENC_NOKEY 288
#define ALG_ENC_DESDERIV 289
#define ALG_ENC_DES32IV 290
#define ALG_ENC_OLD 291
#define ALG_COMP 292
#define F_LIFETIME_HARD 293
#define F_LIFETIME_SOFT 294
#define F_LIFEBYTE_HARD 295
#define F_LIFEBYTE_SOFT 296
#define DECSTRING 297
#define QUOTEDSTRING 298
#define HEXSTRING 299
#define STRING 300
#define ANY 301
#define SPDADD 302
#define SPDDELETE 303
#define SPDDUMP 304
#define SPDFLUSH 305
#define F_POLICY 306
#define PL_REQUESTS 307
#define F_AIFLAGS 308
#define TAGGED 309




/* Copy the first part of user declarations.  */
#line 32 "parse.y"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <net/pfkeyv2.h>
#ifdef HAVE_NETINET6_IPSEC
#  include <netinet6/ipsec.h>
#else 
#  include <netinet/ipsec.h>
#endif
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "libpfkey.h"
#include "vchar.h"

#define DEFAULT_NATT_PORT	4500

#ifndef UDP_ENCAP_ESPINUDP
#define UDP_ENCAP_ESPINUDP	2
#endif

#define ATOX(c) \
  (isdigit((int)c) ? (c - '0') : \
    (isupper((int)c) ? (c - 'A' + 10) : (c - 'a' + 10)))

u_int32_t p_spi;
u_int p_ext, p_alg_enc, p_alg_auth, p_replay, p_mode;
u_int32_t p_reqid;
u_int p_key_enc_len, p_key_auth_len;
caddr_t p_key_enc, p_key_auth;
time_t p_lt_hard, p_lt_soft;
size_t p_lb_hard, p_lb_soft;

#ifdef HAVE_PFKEY_POLICY_PRIORITY
extern int last_msg_type;
extern u_int32_t last_priority;
#endif

extern int exit_now;

static u_int p_natt_type;
static struct addrinfo * p_natt_oa = NULL;

static int p_aiflags = 0, p_aifamily = PF_UNSPEC;

static struct addrinfo *parse_addr __P((char *, char *));
static int fix_portstr __P((vchar_t *, vchar_t *, vchar_t *));
static int setvarbuf __P((char *, int *, struct sadb_ext *, int, caddr_t, int));
void parse_init __P((void));
void free_buffer __P((void));

int setkeymsg0 __P((struct sadb_msg *, unsigned int, unsigned int, size_t));
static int setkeymsg_spdaddr __P((unsigned int, unsigned int, vchar_t *,
	struct addrinfo *, int, struct addrinfo *, int));
static int setkeymsg_spdaddr_tag __P((unsigned int, char *, vchar_t *));
static int setkeymsg_addr __P((unsigned int, unsigned int,
	struct addrinfo *, struct addrinfo *, int));
static int setkeymsg_add __P((unsigned int, unsigned int,
	struct addrinfo *, struct addrinfo *));
extern int setkeymsg __P((char *, size_t *));
extern int sendkeymsg __P((char *, size_t));

extern int yylex __P((void));
extern void yyfatal __P((const char *));
extern void yyerror __P((const char *));
extern int f_rfcmode;


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
#line 114 "parse.y"
typedef union YYSTYPE {
	int num;
	unsigned long ulnum;
	vchar_t val;
	struct addrinfo *res;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 272 "parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 284 "parse.c"

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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   144

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  35
/* YYNRULES -- Number of rules. */
#define YYNRULES  82
/* YYNRULES -- Number of states. */
#define YYNSTATES  158

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   309

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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    39,    48,    55,    64,
      68,    72,    73,    75,    77,    79,    81,    84,    86,    88,
      90,    92,    94,    99,   102,   105,   108,   112,   114,   117,
     119,   122,   125,   128,   130,   132,   134,   135,   138,   141,
     144,   147,   150,   153,   156,   159,   162,   165,   168,   181,
     187,   200,   203,   206,   207,   210,   212,   214,   216,   219,
     220,   223,   224,   228,   232,   236,   238,   240,   242,   243,
     245,   248,   250
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      56,     0,    -1,    -1,    56,    57,    -1,    58,    -1,    61,
      -1,    59,    -1,    60,    -1,    62,    -1,    63,    -1,    89,
      -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,     7,
      79,    82,    82,    64,    65,    73,    66,     3,    -1,     9,
      79,    81,    81,    64,    65,    73,     3,    -1,    10,    79,
      81,    81,    64,     3,    -1,     8,    79,    81,    81,    64,
      65,    73,     3,    -1,    11,    64,     3,    -1,    12,    64,
       3,    -1,    -1,    14,    -1,    15,    -1,    16,    -1,    17,
      -1,    17,    81,    -1,    42,    -1,    44,    -1,    67,    -1,
      68,    -1,    69,    -1,    20,    70,    19,    71,    -1,    20,
      70,    -1,    19,    71,    -1,    22,    37,    -1,    22,    37,
      23,    -1,    33,    -1,    32,    72,    -1,    36,    -1,    34,
      72,    -1,    35,    72,    -1,    30,    72,    -1,    31,    -1,
      43,    -1,    44,    -1,    -1,    73,    74,    -1,    27,    28,
      -1,    27,    29,    -1,    24,    25,    -1,    24,    46,    -1,
      26,    42,    -1,    21,    42,    -1,    38,    42,    -1,    39,
      42,    -1,    40,    42,    -1,    41,    42,    -1,    47,    79,
      45,    83,    84,    45,    83,    84,    85,    86,    87,     3,
      -1,    47,    54,    43,    87,     3,    -1,    48,    79,    45,
      83,    84,    45,    83,    84,    85,    86,    87,     3,    -1,
      49,     3,    -1,    50,     3,    -1,    -1,    79,    80,    -1,
      53,    -1,    45,    -1,    45,    -1,    45,    84,    -1,    -1,
       4,    42,    -1,    -1,     5,    46,     6,    -1,     5,    42,
       6,    -1,     5,    45,     6,    -1,    42,    -1,    46,    -1,
      45,    -1,    -1,    45,    -1,    51,    88,    -1,    52,    -1,
      13,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   154,   154,   156,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   180,   192,   211,   223,   238,
     248,   258,   261,   269,   277,   281,   288,   298,   299,   320,
     321,   322,   326,   327,   331,   335,   343,   355,   370,   385,
     401,   422,   446,   461,   474,   478,   507,   509,   513,   514,
     515,   516,   517,   518,   527,   528,   529,   530,   536,   575,
     587,   625,   636,   645,   647,   651,   676,   687,   695,   707,
     708,   713,   721,   730,   741,   748,   749,   750,   773,   777,
     789,   813,   818
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EOT", "SLASH", "BLCL", "ELCL", "ADD", 
  "GET", "DELETE", "DELETEALL", "FLUSH", "DUMP", "EXIT", "PR_ESP", 
  "PR_AH", "PR_IPCOMP", "PR_ESPUDP", "F_PROTOCOL", "F_AUTH", "F_ENC", 
  "F_REPLAY", "F_COMP", "F_RAWCPI", "F_MODE", "MODE", "F_REQID", "F_EXT", 
  "EXTENSION", "NOCYCLICSEQ", "ALG_AUTH", "ALG_AUTH_NOKEY", "ALG_ENC", 
  "ALG_ENC_NOKEY", "ALG_ENC_DESDERIV", "ALG_ENC_DES32IV", "ALG_ENC_OLD", 
  "ALG_COMP", "F_LIFETIME_HARD", "F_LIFETIME_SOFT", "F_LIFEBYTE_HARD", 
  "F_LIFEBYTE_SOFT", "DECSTRING", "QUOTEDSTRING", "HEXSTRING", "STRING", 
  "ANY", "SPDADD", "SPDDELETE", "SPDDUMP", "SPDFLUSH", "F_POLICY", 
  "PL_REQUESTS", "F_AIFLAGS", "TAGGED", "$accept", "commands", "command", 
  "add_command", "delete_command", "deleteall_command", "get_command", 
  "flush_command", "dump_command", "protocol_spec", "spi", 
  "algorithm_spec", "esp_spec", "ah_spec", "ipcomp_spec", "enc_alg", 
  "auth_alg", "key_string", "extension_spec", "extension", 
  "spdadd_command", "spddelete_command", "spddump_command", 
  "spdflush_command", "ipaddropts", "ipaddropt", "ipaddr", "ipandport", 
  "prefix", "portstr", "upper_spec", "upper_misc_spec", "policy_spec", 
  "policy_requests", "exit_command", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    55,    56,    56,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    58,    59,    60,    61,    62,
      63,    64,    64,    64,    64,    64,    64,    65,    65,    66,
      66,    66,    67,    67,    68,    69,    69,    70,    70,    70,
      70,    70,    71,    71,    72,    72,    73,    73,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    75,    75,
      76,    77,    78,    79,    79,    80,    81,    82,    82,    83,
      83,    84,    84,    84,    84,    85,    85,    85,    86,    86,
      87,    88,    89
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     9,     8,     6,     8,     3,
       3,     0,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     4,     2,     2,     2,     3,     1,     2,     1,
       2,     2,     2,     1,     1,     1,     0,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    12,     5,
      12,     2,     2,     0,     2,     1,     1,     1,     2,     0,
       2,     0,     3,     3,     3,     1,     1,     1,     0,     1,
       2,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,    63,    63,    63,    63,    21,    21,     0,
      63,    63,     0,     0,     3,     4,     6,     7,     5,     8,
       9,    11,    12,    13,    14,    10,     0,     0,     0,     0,
      22,    23,    24,    25,     0,     0,    82,     0,     0,     0,
      61,    62,    67,    65,    64,     0,    66,     0,     0,     0,
      26,    19,    20,     0,    69,    69,     0,    68,    21,    21,
      21,    21,     0,     0,     0,    71,    71,     0,     0,     0,
       0,     0,     0,     0,    81,    80,    59,    70,     0,     0,
      73,    74,    72,    27,    28,    46,    46,    46,    17,    69,
      69,     0,     0,     0,    71,    71,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
      31,    47,    18,    16,     0,     0,     0,    43,    34,     0,
      37,     0,     0,    39,    33,    53,    35,    50,    51,    52,
      48,    49,    54,    55,    56,    57,    15,    75,    77,    76,
      78,    78,    44,    45,    42,    38,    40,    41,     0,    36,
      79,     0,     0,    32,     0,     0,    58,    60
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,    14,    15,    16,    17,    18,    19,    20,    34,
      85,   107,   108,   109,   110,   124,   118,   144,    91,   111,
      21,    22,    23,    24,    26,    44,    47,    45,    65,    57,
     140,   151,    63,    75,    25
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -54
static const short yypact[] =
{
     -54,     7,   -54,   -54,   -54,   -54,   -54,    28,    28,     5,
     -31,   -54,    25,    55,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,   -54,   -41,   -40,   -40,   -40,
     -54,   -54,   -54,    22,    67,    72,   -54,    36,   -20,    20,
     -54,   -54,    81,   -54,   -54,    56,   -54,    22,    22,    22,
     -54,   -54,   -54,    64,   101,   101,   -36,   -54,    28,    28,
      28,    28,    68,   113,    77,    81,    81,   115,   116,   117,
      62,    62,    62,   121,   -54,   -54,   -54,   -54,    80,    82,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   101,
     101,    42,     0,     8,    81,    81,    54,    63,    84,    91,
     -24,    87,    79,    88,    89,    90,    92,   130,   -54,   -54,
     -54,   -54,   -54,   -54,    32,    32,    66,   -54,   -54,    66,
     -54,    66,    66,   -54,   118,   -54,   112,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
      93,    93,   -54,   -54,   -54,   -54,   -54,   -54,    54,   -54,
     -54,    64,    64,   -54,   133,   136,   -54,   -54
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,    -8,
      40,   -54,   -54,   -54,   -54,   -54,    -7,   -19,    27,   -54,
     -54,   -54,   -54,   -54,    83,   -54,    43,    95,   -53,   -35,
      29,     1,   -34,   -54,   -54
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      35,   127,    66,   112,    42,    46,    67,     2,    36,    68,
      69,   113,    43,    43,     3,     4,     5,     6,     7,     8,
       9,    98,   128,    37,   100,    54,   101,   102,    40,    98,
      78,    79,   100,    43,   101,   102,    94,    95,   103,   104,
     105,   106,    30,    31,    32,    33,   103,   104,   105,   106,
      70,    71,    72,    73,    10,    11,    12,    13,    41,   114,
     115,    96,    97,    98,    99,    55,   100,    46,   101,   102,
      51,    48,    49,    43,   137,    52,    50,   138,   139,    53,
     103,   104,   105,   106,   116,   117,    56,    27,    28,    29,
      59,    60,    61,    38,    39,   119,   120,   121,   122,   123,
     145,    42,   146,   147,    83,    64,    84,   130,   131,   142,
     143,    86,    87,    92,    93,    62,    76,   154,   155,    77,
      74,    80,    81,    82,    88,    89,   125,    90,   126,   129,
     132,   133,   134,   136,   135,   149,   156,   148,   150,   157,
      58,   153,   152,     0,   141
};

static const short yycheck[] =
{
       8,    25,    55,     3,    45,    45,    42,     0,     3,    45,
      46,     3,    53,    53,     7,     8,     9,    10,    11,    12,
      13,    21,    46,    54,    24,    45,    26,    27,     3,    21,
      65,    66,    24,    53,    26,    27,    89,    90,    38,    39,
      40,    41,    14,    15,    16,    17,    38,    39,    40,    41,
      58,    59,    60,    61,    47,    48,    49,    50,     3,    94,
      95,    19,    20,    21,    22,    45,    24,    45,    26,    27,
       3,    28,    29,    53,    42,     3,    33,    45,    46,    43,
      38,    39,    40,    41,    30,    31,     5,     4,     5,     6,
      47,    48,    49,    10,    11,    32,    33,    34,    35,    36,
     119,    45,   121,   122,    42,     4,    44,    28,    29,    43,
      44,    71,    72,    86,    87,    51,     3,   151,   152,    42,
      52,     6,     6,     6,     3,    45,    42,    45,    37,    42,
      42,    42,    42,     3,    42,    23,     3,    19,    45,     3,
      45,   148,   141,    -1,   115
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    56,     0,     7,     8,     9,    10,    11,    12,    13,
      47,    48,    49,    50,    57,    58,    59,    60,    61,    62,
      63,    75,    76,    77,    78,    89,    79,    79,    79,    79,
      14,    15,    16,    17,    64,    64,     3,    54,    79,    79,
       3,     3,    45,    53,    80,    82,    45,    81,    81,    81,
      81,     3,     3,    43,    45,    45,     5,    84,    82,    81,
      81,    81,    51,    87,     4,    83,    83,    42,    45,    46,
      64,    64,    64,    64,    52,    88,     3,    42,    84,    84,
       6,     6,     6,    42,    44,    65,    65,    65,     3,    45,
      45,    73,    73,    73,    83,    83,    19,    20,    21,    22,
      24,    26,    27,    38,    39,    40,    41,    66,    67,    68,
      69,    74,     3,     3,    84,    84,    30,    31,    71,    32,
      33,    34,    35,    36,    70,    42,    37,    25,    46,    42,
      28,    29,    42,    42,    42,    42,     3,    42,    45,    46,
      85,    85,    43,    44,    72,    72,    72,    72,    19,    23,
      45,    86,    86,    71,    87,    87,     3,     3
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
        case 3:
#line 157 "parse.y"
    {
			free_buffer();
			parse_init();
		}
    break;

  case 15:
#line 181 "parse.y"
    {
			int status;

			status = setkeymsg_add(SADB_ADD, yyvsp[-4].num, yyvsp[-6].res, yyvsp[-5].res);
			if (status < 0)
				return -1;
		}
    break;

  case 16:
#line 193 "parse.y"
    {
			int status;

			if (yyvsp[-5].res->ai_next || yyvsp[-4].res->ai_next) {
				yyerror("multiple address specified");
				return -1;
			}
			if (p_mode != IPSEC_MODE_ANY)
				yyerror("WARNING: mode is obsolete");

			status = setkeymsg_addr(SADB_DELETE, yyvsp[-3].num, yyvsp[-5].res, yyvsp[-4].res, 0);
			if (status < 0)
				return -1;
		}
    break;

  case 17:
#line 212 "parse.y"
    {
			int status;

			status = setkeymsg_addr(SADB_DELETE, yyvsp[-1].num, yyvsp[-3].res, yyvsp[-2].res, 1);
			if (status < 0)
				return -1;
		}
    break;

  case 18:
#line 224 "parse.y"
    {
			int status;

			if (p_mode != IPSEC_MODE_ANY)
				yyerror("WARNING: mode is obsolete");

			status = setkeymsg_addr(SADB_GET, yyvsp[-3].num, yyvsp[-5].res, yyvsp[-4].res, 0);
			if (status < 0)
				return -1;
		}
    break;

  case 19:
#line 239 "parse.y"
    {
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_FLUSH, yyvsp[-1].num, sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
    break;

  case 20:
#line 249 "parse.y"
    {
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_DUMP, yyvsp[-1].num, sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
    break;

  case 21:
#line 258 "parse.y"
    {
			yyval.num = SADB_SATYPE_UNSPEC;
		}
    break;

  case 22:
#line 262 "parse.y"
    {
			yyval.num = SADB_SATYPE_ESP;
			if (yyvsp[0].num == 1)
				p_ext |= SADB_X_EXT_OLD;
			else
				p_ext &= ~SADB_X_EXT_OLD;
		}
    break;

  case 23:
#line 270 "parse.y"
    {
			yyval.num = SADB_SATYPE_AH;
			if (yyvsp[0].num == 1)
				p_ext |= SADB_X_EXT_OLD;
			else
				p_ext &= ~SADB_X_EXT_OLD;
		}
    break;

  case 24:
#line 278 "parse.y"
    {
			yyval.num = SADB_X_SATYPE_IPCOMP;
		}
    break;

  case 25:
#line 282 "parse.y"
    {
			yyval.num = SADB_SATYPE_ESP;
			p_ext &= ~SADB_X_EXT_OLD;
			p_natt_oa = 0;
			p_natt_type = UDP_ENCAP_ESPINUDP;
		}
    break;

  case 26:
#line 289 "parse.y"
    {
			yyval.num = SADB_SATYPE_ESP;
			p_ext &= ~SADB_X_EXT_OLD;
			p_natt_oa = yyvsp[0].res;
			p_natt_type = UDP_ENCAP_ESPINUDP;
		}
    break;

  case 27:
#line 298 "parse.y"
    { p_spi = yyvsp[0].ulnum; }
    break;

  case 28:
#line 300 "parse.y"
    {
			char *ep;
			unsigned long v;

			ep = NULL;
			v = strtoul(yyvsp[0].val.buf, &ep, 16);
			if (!ep || *ep) {
				yyerror("invalid SPI");
				return -1;
			}
			if (v & ~0xffffffff) {
				yyerror("SPI too big.");
				return -1;
			}

			p_spi = v;
		}
    break;

  case 35:
#line 336 "parse.y"
    {
			if (yyvsp[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yyvsp[0].num;
		}
    break;

  case 36:
#line 344 "parse.y"
    {
			if (yyvsp[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yyvsp[-1].num;
			p_ext |= SADB_X_EXT_RAWCPI;
		}
    break;

  case 37:
#line 355 "parse.y"
    {
			if (yyvsp[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yyvsp[0].num;

			p_key_enc_len = 0;
			p_key_enc = "";
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
    break;

  case 38:
#line 370 "parse.y"
    {
			if (yyvsp[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yyvsp[-1].num;

			p_key_enc_len = yyvsp[0].val.len;
			p_key_enc = yyvsp[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
    break;

  case 39:
#line 385 "parse.y"
    {
			if (yyvsp[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			yyerror("WARNING: obsolete algorithm");
			p_alg_enc = yyvsp[0].num;

			p_key_enc_len = 0;
			p_key_enc = "";
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
    break;

  case 40:
#line 402 "parse.y"
    {
			if (yyvsp[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yyvsp[-1].num;
			if (p_ext & SADB_X_EXT_OLD) {
				yyerror("algorithm mismatched");
				return -1;
			}
			p_ext |= SADB_X_EXT_DERIV;

			p_key_enc_len = yyvsp[0].val.len;
			p_key_enc = yyvsp[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
    break;

  case 41:
#line 423 "parse.y"
    {
			if (yyvsp[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_enc = yyvsp[-1].num;
			if (!(p_ext & SADB_X_EXT_OLD)) {
				yyerror("algorithm mismatched");
				return -1;
			}
			p_ext |= SADB_X_EXT_IV4B;

			p_key_enc_len = yyvsp[0].val.len;
			p_key_enc = yyvsp[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_ENCRYPT,
			    p_alg_enc, PFKEY_UNUNIT64(p_key_enc_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
    break;

  case 42:
#line 446 "parse.y"
    {
			if (yyvsp[-1].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_auth = yyvsp[-1].num;

			p_key_auth_len = yyvsp[0].val.len;
			p_key_auth = yyvsp[0].val.buf;
			if (ipsec_check_keylen(SADB_EXT_SUPPORTED_AUTH,
			    p_alg_auth, PFKEY_UNUNIT64(p_key_auth_len)) < 0) {
				yyerror(ipsec_strerror());
				return -1;
			}
		}
    break;

  case 43:
#line 461 "parse.y"
    {
			if (yyvsp[0].num < 0) {
				yyerror("unsupported algorithm");
				return -1;
			}
			p_alg_auth = yyvsp[0].num;

			p_key_auth_len = 0;
			p_key_auth = NULL;
		}
    break;

  case 44:
#line 475 "parse.y"
    {
			yyval.val = yyvsp[0].val;
		}
    break;

  case 45:
#line 479 "parse.y"
    {
			caddr_t pp_key;
			caddr_t bp;
			caddr_t yp = yyvsp[0].val.buf;
			int l;

			l = strlen(yp) % 2 + strlen(yp) / 2;
			if ((pp_key = malloc(l)) == 0) {
				yyerror("not enough core");
				return -1;
			}
			memset(pp_key, 0, l);

			bp = pp_key;
			if (strlen(yp) % 2) {
				*bp = ATOX(yp[0]);
				yp++, bp++;
			}
			while (*yp) {
				*bp = (ATOX(yp[0]) << 4) | ATOX(yp[1]);
				yp += 2, bp++;
			}

			yyval.val.len = l;
			yyval.val.buf = pp_key;
		}
    break;

  case 48:
#line 513 "parse.y"
    { p_ext |= yyvsp[0].num; }
    break;

  case 49:
#line 514 "parse.y"
    { p_ext &= ~SADB_X_EXT_CYCSEQ; }
    break;

  case 50:
#line 515 "parse.y"
    { p_mode = yyvsp[0].num; }
    break;

  case 51:
#line 516 "parse.y"
    { p_mode = IPSEC_MODE_ANY; }
    break;

  case 52:
#line 517 "parse.y"
    { p_reqid = yyvsp[0].ulnum; }
    break;

  case 53:
#line 519 "parse.y"
    {
			if ((p_ext & SADB_X_EXT_OLD) != 0) {
				yyerror("replay prevention cannot be used with "
				    "ah/esp-old");
				return -1;
			}
			p_replay = yyvsp[0].ulnum;
		}
    break;

  case 54:
#line 527 "parse.y"
    { p_lt_hard = yyvsp[0].ulnum; }
    break;

  case 55:
#line 528 "parse.y"
    { p_lt_soft = yyvsp[0].ulnum; }
    break;

  case 56:
#line 529 "parse.y"
    { p_lb_hard = yyvsp[0].ulnum; }
    break;

  case 57:
#line 530 "parse.y"
    { p_lb_soft = yyvsp[0].ulnum; }
    break;

  case 58:
#line 537 "parse.y"
    {
			int status;
			struct addrinfo *src, *dst;

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			last_msg_type = SADB_X_SPDADD;
#endif

			/* fixed port fields if ulp is icmpv6 */
			if (yyvsp[-2].val.buf != NULL) {
				if (yyvsp[-3].num != IPPROTO_ICMPV6)
					return -1;
				free(yyvsp[-7].val.buf);
				free(yyvsp[-4].val.buf);
				if (fix_portstr(&yyvsp[-2].val, &yyvsp[-7].val, &yyvsp[-4].val))
					return -1;
			}

			src = parse_addr(yyvsp[-9].val.buf, yyvsp[-7].val.buf);
			dst = parse_addr(yyvsp[-6].val.buf, yyvsp[-4].val.buf);
			if (!src || !dst) {
				/* yyerror is already called */
				return -1;
			}
			if (src->ai_next || dst->ai_next) {
				yyerror("multiple address specified");
				freeaddrinfo(src);
				freeaddrinfo(dst);
				return -1;
			}

			status = setkeymsg_spdaddr(SADB_X_SPDADD, yyvsp[-3].num, &yyvsp[-1].val,
			    src, yyvsp[-8].num, dst, yyvsp[-5].num);
			freeaddrinfo(src);
			freeaddrinfo(dst);
			if (status < 0)
				return -1;
		}
    break;

  case 59:
#line 576 "parse.y"
    {
			int status;

			status = setkeymsg_spdaddr_tag(SADB_X_SPDADD,
			    yyvsp[-2].val.buf, &yyvsp[-1].val);
			if (status < 0)
				return -1;
		}
    break;

  case 60:
#line 588 "parse.y"
    {
			int status;
			struct addrinfo *src, *dst;

			/* fixed port fields if ulp is icmpv6 */
			if (yyvsp[-2].val.buf != NULL) {
				if (yyvsp[-3].num != IPPROTO_ICMPV6)
					return -1;
				free(yyvsp[-7].val.buf);
				free(yyvsp[-4].val.buf);
				if (fix_portstr(&yyvsp[-2].val, &yyvsp[-7].val, &yyvsp[-4].val))
					return -1;
			}

			src = parse_addr(yyvsp[-9].val.buf, yyvsp[-7].val.buf);
			dst = parse_addr(yyvsp[-6].val.buf, yyvsp[-4].val.buf);
			if (!src || !dst) {
				/* yyerror is already called */
				return -1;
			}
			if (src->ai_next || dst->ai_next) {
				yyerror("multiple address specified");
				freeaddrinfo(src);
				freeaddrinfo(dst);
				return -1;
			}

			status = setkeymsg_spdaddr(SADB_X_SPDDELETE, yyvsp[-3].num, &yyvsp[-1].val,
			    src, yyvsp[-8].num, dst, yyvsp[-5].num);
			freeaddrinfo(src);
			freeaddrinfo(dst);
			if (status < 0)
				return -1;
		}
    break;

  case 61:
#line 626 "parse.y"
    {
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_X_SPDDUMP, SADB_SATYPE_UNSPEC,
			    sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
    break;

  case 62:
#line 637 "parse.y"
    {
			struct sadb_msg msg;
			setkeymsg0(&msg, SADB_X_SPDFLUSH, SADB_SATYPE_UNSPEC,
			    sizeof(msg));
			sendkeymsg((char *)&msg, sizeof(msg));
		}
    break;

  case 65:
#line 652 "parse.y"
    {
			char *p;

			for (p = yyvsp[0].val.buf + 1; *p; p++)
				switch (*p) {
				case '4':
					p_aifamily = AF_INET;
					break;
#ifdef INET6
				case '6':
					p_aifamily = AF_INET6;
					break;
#endif
				case 'n':
					p_aiflags = AI_NUMERICHOST;
					break;
				default:
					yyerror("invalid flag");
					return -1;
				}
		}
    break;

  case 66:
#line 677 "parse.y"
    {
			yyval.res = parse_addr(yyvsp[0].val.buf, NULL);
			if (yyval.res == NULL) {
				/* yyerror already called by parse_addr */
				return -1;
			}
		}
    break;

  case 67:
#line 688 "parse.y"
    {
			yyval.res = parse_addr(yyvsp[0].val.buf, NULL);
			if (yyval.res == NULL) {
				/* yyerror already called by parse_addr */
				return -1;
			}
		}
    break;

  case 68:
#line 696 "parse.y"
    {
			yyval.res = parse_addr(yyvsp[-1].val.buf, yyvsp[0].val.buf);
			if (yyval.res == NULL) {
				/* yyerror already called by parse_addr */
				return -1;
			}
		}
    break;

  case 69:
#line 707 "parse.y"
    { yyval.num = -1; }
    break;

  case 70:
#line 708 "parse.y"
    { yyval.num = yyvsp[0].ulnum; }
    break;

  case 71:
#line 713 "parse.y"
    {
			yyval.val.buf = strdup("0");
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
    break;

  case 72:
#line 722 "parse.y"
    {
			yyval.val.buf = strdup("0");
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
    break;

  case 73:
#line 731 "parse.y"
    {
			char buf[20];
			snprintf(buf, sizeof(buf), "%lu", yyvsp[-1].ulnum);
			yyval.val.buf = strdup(buf);
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
    break;

  case 74:
#line 742 "parse.y"
    {
			yyval.val = yyvsp[-1].val;
		}
    break;

  case 75:
#line 748 "parse.y"
    { yyval.num = yyvsp[0].ulnum; }
    break;

  case 76:
#line 749 "parse.y"
    { yyval.num = IPSEC_ULPROTO_ANY; }
    break;

  case 77:
#line 751 "parse.y"
    {
			struct protoent *ent;

			ent = getprotobyname(yyvsp[0].val.buf);
			if (ent)
				yyval.num = ent->p_proto;
			else {
				if (strcmp("icmp6", yyvsp[0].val.buf) == 0) {
					yyval.num = IPPROTO_ICMPV6;
				} else if(strcmp("ip4", yyvsp[0].val.buf) == 0) {
					yyval.num = IPPROTO_IPV4;
				} else {
					yyerror("invalid upper layer protocol");
					return -1;
				}
			}
			endprotoent();
		}
    break;

  case 78:
#line 773 "parse.y"
    {
			yyval.val.buf = NULL;
			yyval.val.len = 0;
		}
    break;

  case 79:
#line 778 "parse.y"
    {
			yyval.val.buf = strdup(yyvsp[0].val.buf);
			if (!yyval.val.buf) {
				yyerror("insufficient memory");
				return -1;
			}
			yyval.val.len = strlen(yyval.val.buf);
		}
    break;

  case 80:
#line 790 "parse.y"
    {
			char *policy;
#ifdef HAVE_PFKEY_POLICY_PRIORITY
			struct sadb_x_policy *xpl;
#endif

			policy = ipsec_set_policy(yyvsp[0].val.buf, yyvsp[0].val.len);
			if (policy == NULL) {
				yyerror(ipsec_strerror());
				return -1;
			}

			yyval.val.buf = policy;
			yyval.val.len = ipsec_get_policylen(policy);

#ifdef HAVE_PFKEY_POLICY_PRIORITY
			xpl = (struct sadb_x_policy *) yyval.val.buf;
			last_priority = xpl->sadb_x_policy_priority;
#endif
		}
    break;

  case 81:
#line 813 "parse.y"
    { yyval.val = yyvsp[0].val; }
    break;

  case 82:
#line 819 "parse.y"
    {
			exit_now = 1;
			YYACCEPT;
		}
    break;


    }

/* Line 991 of yacc.c.  */
#line 2038 "parse.c"

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


#line 824 "parse.y"


int
setkeymsg0(msg, type, satype, l)
	struct sadb_msg *msg;
	unsigned int type;
	unsigned int satype;
	size_t l;
{

	msg->sadb_msg_version = PF_KEY_V2;
	msg->sadb_msg_type = type;
	msg->sadb_msg_errno = 0;
	msg->sadb_msg_satype = satype;
	msg->sadb_msg_reserved = 0;
	msg->sadb_msg_seq = 0;
	msg->sadb_msg_pid = getpid();
	msg->sadb_msg_len = PFKEY_UNIT64(l);
	return 0;
}

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_spdaddr(type, upper, policy, srcs, splen, dsts, dplen)
	unsigned int type;
	unsigned int upper;
	vchar_t *policy;
	struct addrinfo *srcs;
	int splen;
	struct addrinfo *dsts;
	int dplen;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0;
	struct sadb_address m_addr;
	struct addrinfo *s, *d;
	int n;
	int plen;
	struct sockaddr *sa;
	int salen;
	struct sadb_x_policy *sp;
#ifdef HAVE_POLICY_FWD
	struct sadb_x_ipsecrequest *ps = NULL;
	int saved_level, saved_id = 0;
#endif

	msg = (struct sadb_msg *)buf;

	if (!srcs || !dsts)
		return -1;

	/* fix up length afterwards */
	setkeymsg0(msg, type, SADB_SATYPE_UNSPEC, 0);
	l = sizeof(struct sadb_msg);

	sp = (struct sadb_x_policy*) (buf + l);
	memcpy(buf + l, policy->buf, policy->len);
	l += policy->len;

	l0 = l;
	n = 0;

	/* do it for all src/dst pairs */
	for (s = srcs; s; s = s->ai_next) {
		for (d = dsts; d; d = d->ai_next) {
			/* rewind pointer */
			l = l0;

			if (s->ai_addr->sa_family != d->ai_addr->sa_family)
				continue;
			switch (s->ai_addr->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				continue;
			}

			/* set src */
			sa = s->ai_addr;
			salen = sysdep_sa_len(s->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
			m_addr.sadb_address_proto = upper;
			m_addr.sadb_address_prefixlen =
			    (splen >= 0 ? splen : plen);
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

			/* set dst */
			sa = d->ai_addr;
			salen = sysdep_sa_len(d->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
			m_addr.sadb_address_proto = upper;
			m_addr.sadb_address_prefixlen =
			    (dplen >= 0 ? dplen : plen);
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

			msg->sadb_msg_len = PFKEY_UNIT64(l);

			sendkeymsg(buf, l);

#ifdef HAVE_POLICY_FWD
			/* create extra call for FWD policy */
			if (f_rfcmode && sp->sadb_x_policy_dir == IPSEC_DIR_INBOUND) {
				sp->sadb_x_policy_dir = IPSEC_DIR_FWD;
				ps = (struct sadb_x_ipsecrequest*) (sp+1);

				/* if request level is unique, change it to
				 * require for fwd policy */
				/* XXX: currently, only first policy is updated
				 * only. Update following too... */
				saved_level = ps->sadb_x_ipsecrequest_level;
				if (saved_level == IPSEC_LEVEL_UNIQUE) {
					saved_id = ps->sadb_x_ipsecrequest_reqid;
					ps->sadb_x_ipsecrequest_reqid=0;
					ps->sadb_x_ipsecrequest_level=IPSEC_LEVEL_REQUIRE;
				}

				sendkeymsg(buf, l);
				/* restoring for next message */
				sp->sadb_x_policy_dir = IPSEC_DIR_INBOUND;
				if (saved_level == IPSEC_LEVEL_UNIQUE) {
					ps->sadb_x_ipsecrequest_reqid = saved_id;
					ps->sadb_x_ipsecrequest_level = saved_level;
				}
			}
#endif

			n++;
		}
	}

	if (n == 0)
		return -1;
	else
		return 0;
}

static int
setkeymsg_spdaddr_tag(type, tag, policy)
	unsigned int type;
	char *tag;
	vchar_t *policy;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0;
#ifdef SADB_X_EXT_TAG
	struct sadb_x_tag m_tag;
#endif
	int n;

	msg = (struct sadb_msg *)buf;

	/* fix up length afterwards */
	setkeymsg0(msg, type, SADB_SATYPE_UNSPEC, 0);
	l = sizeof(struct sadb_msg);

	memcpy(buf + l, policy->buf, policy->len);
	l += policy->len;

	l0 = l;
	n = 0;

#ifdef SADB_X_EXT_TAG
	memset(&m_tag, 0, sizeof(m_tag));
	m_tag.sadb_x_tag_len = PFKEY_UNIT64(sizeof(m_tag));
	m_tag.sadb_x_tag_exttype = SADB_X_EXT_TAG;
	if (strlcpy(m_tag.sadb_x_tag_name, tag,
	    sizeof(m_tag.sadb_x_tag_name)) >= sizeof(m_tag.sadb_x_tag_name))
		return -1;
	memcpy(buf + l, &m_tag, sizeof(m_tag));
	l += sizeof(m_tag);
#endif

	msg->sadb_msg_len = PFKEY_UNIT64(l);

	sendkeymsg(buf, l);

	return 0;
}

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_addr(type, satype, srcs, dsts, no_spi)
	unsigned int type;
	unsigned int satype;
	struct addrinfo *srcs;
	struct addrinfo *dsts;
	int no_spi;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0, len;
	struct sadb_sa m_sa;
	struct sadb_x_sa2 m_sa2;
	struct sadb_address m_addr;
	struct addrinfo *s, *d;
	int n;
	int plen;
	struct sockaddr *sa;
	int salen;

	msg = (struct sadb_msg *)buf;

	if (!srcs || !dsts)
		return -1;

	/* fix up length afterwards */
	setkeymsg0(msg, type, satype, 0);
	l = sizeof(struct sadb_msg);

	if (!no_spi) {
		len = sizeof(struct sadb_sa);
		m_sa.sadb_sa_len = PFKEY_UNIT64(len);
		m_sa.sadb_sa_exttype = SADB_EXT_SA;
		m_sa.sadb_sa_spi = htonl(p_spi);
		m_sa.sadb_sa_replay = p_replay;
		m_sa.sadb_sa_state = 0;
		m_sa.sadb_sa_auth = p_alg_auth;
		m_sa.sadb_sa_encrypt = p_alg_enc;
		m_sa.sadb_sa_flags = p_ext;

		memcpy(buf + l, &m_sa, len);
		l += len;

		len = sizeof(struct sadb_x_sa2);
		m_sa2.sadb_x_sa2_len = PFKEY_UNIT64(len);
		m_sa2.sadb_x_sa2_exttype = SADB_X_EXT_SA2;
		m_sa2.sadb_x_sa2_mode = p_mode;
		m_sa2.sadb_x_sa2_reqid = p_reqid;

		memcpy(buf + l, &m_sa2, len);
		l += len;
	}

	l0 = l;
	n = 0;

	/* do it for all src/dst pairs */
	for (s = srcs; s; s = s->ai_next) {
		for (d = dsts; d; d = d->ai_next) {
			/* rewind pointer */
			l = l0;

			if (s->ai_addr->sa_family != d->ai_addr->sa_family)
				continue;
			switch (s->ai_addr->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				continue;
			}

			/* set src */
			sa = s->ai_addr;
			salen = sysdep_sa_len(s->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

			/* set dst */
			sa = d->ai_addr;
			salen = sysdep_sa_len(d->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

			msg->sadb_msg_len = PFKEY_UNIT64(l);

			sendkeymsg(buf, l);

			n++;
		}
	}

	if (n == 0)
		return -1;
	else
		return 0;
}

#ifdef SADB_X_EXT_NAT_T_TYPE
static u_int16_t get_port (struct addrinfo *addr)
{
	struct sockaddr *s = addr->ai_addr;
	u_int16_t port = 0;

	switch (s->sa_family) {
	case AF_INET:
	  {
		struct sockaddr_in *sin = (struct sockaddr_in *)s;
		port = ntohs(sin->sin_port);
		break;
	  }
	case AF_INET6:
	  {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)s;
		port = ntohs(sin6->sin6_port);
		break;
	  }
	}

	if (port == 0)
		port = DEFAULT_NATT_PORT;

	return port;
}
#endif

/* XXX NO BUFFER OVERRUN CHECK! BAD BAD! */
static int
setkeymsg_add(type, satype, srcs, dsts)
	unsigned int type;
	unsigned int satype;
	struct addrinfo *srcs;
	struct addrinfo *dsts;
{
	struct sadb_msg *msg;
	char buf[BUFSIZ];
	int l, l0, len;
	struct sadb_sa m_sa;
	struct sadb_x_sa2 m_sa2;
	struct sadb_address m_addr;
	struct addrinfo *s, *d;
	int n;
	int plen;
	struct sockaddr *sa;
	int salen;

	msg = (struct sadb_msg *)buf;

	if (!srcs || !dsts)
		return -1;

	/* fix up length afterwards */
	setkeymsg0(msg, type, satype, 0);
	l = sizeof(struct sadb_msg);

	/* set encryption algorithm, if present. */
	if (satype != SADB_X_SATYPE_IPCOMP && p_key_enc) {
		union {
			struct sadb_key key;
			struct sadb_ext ext;
		} m;

		m.key.sadb_key_len =
			PFKEY_UNIT64(sizeof(m.key)
				   + PFKEY_ALIGN8(p_key_enc_len));
		m.key.sadb_key_exttype = SADB_EXT_KEY_ENCRYPT;
		m.key.sadb_key_bits = p_key_enc_len * 8;
		m.key.sadb_key_reserved = 0;

		setvarbuf(buf, &l, &m.ext, sizeof(m.key),
			(caddr_t)p_key_enc, p_key_enc_len);
	}

	/* set authentication algorithm, if present. */
	if (p_key_auth) {
		union {
			struct sadb_key key;
			struct sadb_ext ext;
		} m;

		m.key.sadb_key_len =
			PFKEY_UNIT64(sizeof(m.key)
				   + PFKEY_ALIGN8(p_key_auth_len));
		m.key.sadb_key_exttype = SADB_EXT_KEY_AUTH;
		m.key.sadb_key_bits = p_key_auth_len * 8;
		m.key.sadb_key_reserved = 0;

		setvarbuf(buf, &l, &m.ext, sizeof(m.key),
			(caddr_t)p_key_auth, p_key_auth_len);
	}

	/* set lifetime for HARD */
	if (p_lt_hard != 0 || p_lb_hard != 0) {
		struct sadb_lifetime m_lt;
		u_int slen = sizeof(struct sadb_lifetime);

		m_lt.sadb_lifetime_len = PFKEY_UNIT64(slen);
		m_lt.sadb_lifetime_exttype = SADB_EXT_LIFETIME_HARD;
		m_lt.sadb_lifetime_allocations = 0;
		m_lt.sadb_lifetime_bytes = p_lb_hard;
		m_lt.sadb_lifetime_addtime = p_lt_hard;
		m_lt.sadb_lifetime_usetime = 0;

		memcpy(buf + l, &m_lt, slen);
		l += slen;
	}

	/* set lifetime for SOFT */
	if (p_lt_soft != 0 || p_lb_soft != 0) {
		struct sadb_lifetime m_lt;
		u_int slen = sizeof(struct sadb_lifetime);

		m_lt.sadb_lifetime_len = PFKEY_UNIT64(slen);
		m_lt.sadb_lifetime_exttype = SADB_EXT_LIFETIME_SOFT;
		m_lt.sadb_lifetime_allocations = 0;
		m_lt.sadb_lifetime_bytes = p_lb_soft;
		m_lt.sadb_lifetime_addtime = p_lt_soft;
		m_lt.sadb_lifetime_usetime = 0;

		memcpy(buf + l, &m_lt, slen);
		l += slen;
	}

	len = sizeof(struct sadb_sa);
	m_sa.sadb_sa_len = PFKEY_UNIT64(len);
	m_sa.sadb_sa_exttype = SADB_EXT_SA;
	m_sa.sadb_sa_spi = htonl(p_spi);
	m_sa.sadb_sa_replay = p_replay;
	m_sa.sadb_sa_state = 0;
	m_sa.sadb_sa_auth = p_alg_auth;
	m_sa.sadb_sa_encrypt = p_alg_enc;
	m_sa.sadb_sa_flags = p_ext;

	memcpy(buf + l, &m_sa, len);
	l += len;

	len = sizeof(struct sadb_x_sa2);
	m_sa2.sadb_x_sa2_len = PFKEY_UNIT64(len);
	m_sa2.sadb_x_sa2_exttype = SADB_X_EXT_SA2;
	m_sa2.sadb_x_sa2_mode = p_mode;
	m_sa2.sadb_x_sa2_reqid = p_reqid;

	memcpy(buf + l, &m_sa2, len);
	l += len;

#ifdef SADB_X_EXT_NAT_T_TYPE
	if (p_natt_type) {
		struct sadb_x_nat_t_type natt_type;

		len = sizeof(struct sadb_x_nat_t_type);
		memset(&natt_type, 0, len);
		natt_type.sadb_x_nat_t_type_len = PFKEY_UNIT64(len);
		natt_type.sadb_x_nat_t_type_exttype = SADB_X_EXT_NAT_T_TYPE;
		natt_type.sadb_x_nat_t_type_type = p_natt_type;

		memcpy(buf + l, &natt_type, len);
		l += len;

		if (p_natt_oa) {
			sa = p_natt_oa->ai_addr;
			switch (sa->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				return -1;
			}
			salen = sysdep_sa_len(sa);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_X_EXT_NAT_T_OA;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);
		}
	}
#endif

	l0 = l;
	n = 0;

	/* do it for all src/dst pairs */
	for (s = srcs; s; s = s->ai_next) {
		for (d = dsts; d; d = d->ai_next) {
			/* rewind pointer */
			l = l0;

			if (s->ai_addr->sa_family != d->ai_addr->sa_family)
				continue;
			switch (s->ai_addr->sa_family) {
			case AF_INET:
				plen = sizeof(struct in_addr) << 3;
				break;
#ifdef INET6
			case AF_INET6:
				plen = sizeof(struct in6_addr) << 3;
				break;
#endif
			default:
				continue;
			}

			/* set src */
			sa = s->ai_addr;
			salen = sysdep_sa_len(s->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

			/* set dst */
			sa = d->ai_addr;
			salen = sysdep_sa_len(d->ai_addr);
			m_addr.sadb_address_len = PFKEY_UNIT64(sizeof(m_addr) +
			    PFKEY_ALIGN8(salen));
			m_addr.sadb_address_exttype = SADB_EXT_ADDRESS_DST;
			m_addr.sadb_address_proto = IPSEC_ULPROTO_ANY;
			m_addr.sadb_address_prefixlen = plen;
			m_addr.sadb_address_reserved = 0;

			setvarbuf(buf, &l, (struct sadb_ext *)&m_addr,
			    sizeof(m_addr), (caddr_t)sa, salen);

#ifdef SADB_X_EXT_NAT_T_TYPE
			if (p_natt_type) {
				struct sadb_x_nat_t_port natt_port;

				/* NATT_SPORT */
				len = sizeof(struct sadb_x_nat_t_port);
				memset(&natt_port, 0, len);
				natt_port.sadb_x_nat_t_port_len = PFKEY_UNIT64(len);
				natt_port.sadb_x_nat_t_port_exttype =
					SADB_X_EXT_NAT_T_SPORT;
				natt_port.sadb_x_nat_t_port_port = htons(get_port(s));
				
				memcpy(buf + l, &natt_port, len);
				l += len;

				/* NATT_DPORT */
				natt_port.sadb_x_nat_t_port_exttype =
					SADB_X_EXT_NAT_T_DPORT;
				natt_port.sadb_x_nat_t_port_port = htons(get_port(d));
				
				memcpy(buf + l, &natt_port, len);
				l += len;
			}
#endif
			msg->sadb_msg_len = PFKEY_UNIT64(l);

			sendkeymsg(buf, l);

			n++;
		}
	}

	if (n == 0)
		return -1;
	else
		return 0;
}

static struct addrinfo *
parse_addr(host, port)
	char *host;
	char *port;
{
	struct addrinfo hints, *res = NULL;
	int error;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = p_aifamily;
	hints.ai_socktype = SOCK_DGRAM;		/*dummy*/
	hints.ai_protocol = IPPROTO_UDP;	/*dummy*/
	hints.ai_flags = p_aiflags;
	error = getaddrinfo(host, port, &hints, &res);
	if (error != 0) {
		yyerror(gai_strerror(error));
		return NULL;
	}
	return res;
}

static int
fix_portstr(spec, sport, dport)
	vchar_t *spec, *sport, *dport;
{
	char *p, *p2 = "0";
	u_int l;

	l = 0;
	for (p = spec->buf; *p != ',' && *p != '\0' && l < spec->len; p++, l++)
		;
	if (*p != '\0') {
		if (*p == ',') {
			*p = '\0';
			p2 = ++p;
		}
		for (p = p2; *p != '\0' && l < spec->len; p++, l++)
			;
		if (*p != '\0' || *p2 == '\0') {
			yyerror("invalid an upper layer protocol spec");
			return -1;
		}
	}

	sport->buf = strdup(spec->buf);
	if (!sport->buf) {
		yyerror("insufficient memory");
		return -1;
	}
	sport->len = strlen(sport->buf);
	dport->buf = strdup(p2);
	if (!dport->buf) {
		yyerror("insufficient memory");
		return -1;
	}
	dport->len = strlen(dport->buf);

	return 0;
}

static int
setvarbuf(buf, off, ebuf, elen, vbuf, vlen)
	char *buf;
	int *off;
	struct sadb_ext *ebuf;
	int elen;
	caddr_t vbuf;
	int vlen;
{
	memset(buf + *off, 0, PFKEY_UNUNIT64(ebuf->sadb_ext_len));
	memcpy(buf + *off, (caddr_t)ebuf, elen);
	memcpy(buf + *off + elen, vbuf, vlen);
	(*off) += PFKEY_ALIGN8(elen + vlen);

	return 0;
}

void
parse_init()
{
	p_spi = 0;

	p_ext = SADB_X_EXT_CYCSEQ;
	p_alg_enc = SADB_EALG_NONE;
	p_alg_auth = SADB_AALG_NONE;
	p_mode = IPSEC_MODE_ANY;
	p_reqid = 0;
	p_replay = 0;
	p_key_enc_len = p_key_auth_len = 0;
	p_key_enc = p_key_auth = 0;
	p_lt_hard = p_lt_soft = 0;
	p_lb_hard = p_lb_soft = 0;

	p_aiflags = 0;
	p_aifamily = PF_UNSPEC;

	/* Clear out any natt OA information */
	if (p_natt_oa)
		freeaddrinfo (p_natt_oa);
	p_natt_oa = NULL;
	p_natt_type = 0;

	return;
}

void
free_buffer()
{
	/* we got tons of memory leaks in the parser anyways, leave them */

	return;
}


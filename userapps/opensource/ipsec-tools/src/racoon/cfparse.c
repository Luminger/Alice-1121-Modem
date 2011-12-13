/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

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
     PATH = 258,
     PATHTYPE = 259,
     INCLUDE = 260,
     IDENTIFIER = 261,
     VENDORID = 262,
     LOGGING = 263,
     LOGLEV = 264,
     PADDING = 265,
     PAD_RANDOMIZE = 266,
     PAD_RANDOMIZELEN = 267,
     PAD_MAXLEN = 268,
     PAD_STRICT = 269,
     PAD_EXCLTAIL = 270,
     LISTEN = 271,
     X_ISAKMP = 272,
     X_ISAKMP_NATT = 273,
     X_ADMIN = 274,
     STRICT_ADDRESS = 275,
     ADMINSOCK = 276,
     DISABLED = 277,
     MODECFG = 278,
     CFG_NET4 = 279,
     CFG_MASK4 = 280,
     CFG_DNS4 = 281,
     CFG_NBNS4 = 282,
     CFG_AUTH_SOURCE = 283,
     CFG_SYSTEM = 284,
     CFG_RADIUS = 285,
     CFG_LOCAL = 286,
     CFG_NONE = 287,
     CFG_ACCOUNTING = 288,
     CFG_CONF_SOURCE = 289,
     CFG_MOTD = 290,
     CFG_POOL_SIZE = 291,
     CFG_AUTH_THROTTLE = 292,
     RETRY = 293,
     RETRY_COUNTER = 294,
     RETRY_INTERVAL = 295,
     RETRY_PERSEND = 296,
     RETRY_PHASE1 = 297,
     RETRY_PHASE2 = 298,
     NATT_KA = 299,
     ALGORITHM_CLASS = 300,
     ALGORITHMTYPE = 301,
     STRENGTHTYPE = 302,
     SAINFO = 303,
     FROM = 304,
     REMOTE = 305,
     ANONYMOUS = 306,
     INHERIT = 307,
     EXCHANGE_MODE = 308,
     EXCHANGETYPE = 309,
     DOI = 310,
     DOITYPE = 311,
     SITUATION = 312,
     SITUATIONTYPE = 313,
     CERTIFICATE_TYPE = 314,
     CERTTYPE = 315,
     PEERS_CERTFILE = 316,
     CA_TYPE = 317,
     VERIFY_CERT = 318,
     SEND_CERT = 319,
     SEND_CR = 320,
     IDENTIFIERTYPE = 321,
     MY_IDENTIFIER = 322,
     PEERS_IDENTIFIER = 323,
     VERIFY_IDENTIFIER = 324,
     DNSSEC = 325,
     CERT_X509 = 326,
     CERT_PLAINRSA = 327,
     NONCE_SIZE = 328,
     DH_GROUP = 329,
     KEEPALIVE = 330,
     PASSIVE = 331,
     INITIAL_CONTACT = 332,
     NAT_TRAVERSAL = 333,
     NAT_TRAVERSAL_LEVEL = 334,
     PROPOSAL_CHECK = 335,
     PROPOSAL_CHECK_LEVEL = 336,
     GENERATE_POLICY = 337,
     SUPPORT_PROXY = 338,
     PROPOSAL = 339,
     EXEC_PATH = 340,
     EXEC_COMMAND = 341,
     EXEC_SUCCESS = 342,
     EXEC_FAILURE = 343,
     GSSAPI_ID = 344,
     COMPLEX_BUNDLE = 345,
     DPD = 346,
     DPD_DELAY = 347,
     DPD_RETRY = 348,
     DPD_MAXFAIL = 349,
     XAUTH_LOGIN = 350,
     PREFIX = 351,
     PORT = 352,
     PORTANY = 353,
     UL_PROTO = 354,
     ANY = 355,
     IKE_FRAG = 356,
     MODE_CFG = 357,
     PFS_GROUP = 358,
     LIFETIME = 359,
     LIFETYPE_TIME = 360,
     LIFETYPE_BYTE = 361,
     STRENGTH = 362,
     SCRIPT = 363,
     PHASE1_UP = 364,
     PHASE1_DOWN = 365,
     NUMBER = 366,
     SWITCH = 367,
     BOOLEAN = 368,
     HEXSTRING = 369,
     QUOTEDSTRING = 370,
     ADDRSTRING = 371,
     UNITTYPE_BYTE = 372,
     UNITTYPE_KBYTES = 373,
     UNITTYPE_MBYTES = 374,
     UNITTYPE_TBYTES = 375,
     UNITTYPE_SEC = 376,
     UNITTYPE_MIN = 377,
     UNITTYPE_HOUR = 378,
     EOS = 379,
     BOC = 380,
     EOC = 381,
     COMMA = 382
   };
#endif
/* Tokens.  */
#define PATH 258
#define PATHTYPE 259
#define INCLUDE 260
#define IDENTIFIER 261
#define VENDORID 262
#define LOGGING 263
#define LOGLEV 264
#define PADDING 265
#define PAD_RANDOMIZE 266
#define PAD_RANDOMIZELEN 267
#define PAD_MAXLEN 268
#define PAD_STRICT 269
#define PAD_EXCLTAIL 270
#define LISTEN 271
#define X_ISAKMP 272
#define X_ISAKMP_NATT 273
#define X_ADMIN 274
#define STRICT_ADDRESS 275
#define ADMINSOCK 276
#define DISABLED 277
#define MODECFG 278
#define CFG_NET4 279
#define CFG_MASK4 280
#define CFG_DNS4 281
#define CFG_NBNS4 282
#define CFG_AUTH_SOURCE 283
#define CFG_SYSTEM 284
#define CFG_RADIUS 285
#define CFG_LOCAL 286
#define CFG_NONE 287
#define CFG_ACCOUNTING 288
#define CFG_CONF_SOURCE 289
#define CFG_MOTD 290
#define CFG_POOL_SIZE 291
#define CFG_AUTH_THROTTLE 292
#define RETRY 293
#define RETRY_COUNTER 294
#define RETRY_INTERVAL 295
#define RETRY_PERSEND 296
#define RETRY_PHASE1 297
#define RETRY_PHASE2 298
#define NATT_KA 299
#define ALGORITHM_CLASS 300
#define ALGORITHMTYPE 301
#define STRENGTHTYPE 302
#define SAINFO 303
#define FROM 304
#define REMOTE 305
#define ANONYMOUS 306
#define INHERIT 307
#define EXCHANGE_MODE 308
#define EXCHANGETYPE 309
#define DOI 310
#define DOITYPE 311
#define SITUATION 312
#define SITUATIONTYPE 313
#define CERTIFICATE_TYPE 314
#define CERTTYPE 315
#define PEERS_CERTFILE 316
#define CA_TYPE 317
#define VERIFY_CERT 318
#define SEND_CERT 319
#define SEND_CR 320
#define IDENTIFIERTYPE 321
#define MY_IDENTIFIER 322
#define PEERS_IDENTIFIER 323
#define VERIFY_IDENTIFIER 324
#define DNSSEC 325
#define CERT_X509 326
#define CERT_PLAINRSA 327
#define NONCE_SIZE 328
#define DH_GROUP 329
#define KEEPALIVE 330
#define PASSIVE 331
#define INITIAL_CONTACT 332
#define NAT_TRAVERSAL 333
#define NAT_TRAVERSAL_LEVEL 334
#define PROPOSAL_CHECK 335
#define PROPOSAL_CHECK_LEVEL 336
#define GENERATE_POLICY 337
#define SUPPORT_PROXY 338
#define PROPOSAL 339
#define EXEC_PATH 340
#define EXEC_COMMAND 341
#define EXEC_SUCCESS 342
#define EXEC_FAILURE 343
#define GSSAPI_ID 344
#define COMPLEX_BUNDLE 345
#define DPD 346
#define DPD_DELAY 347
#define DPD_RETRY 348
#define DPD_MAXFAIL 349
#define XAUTH_LOGIN 350
#define PREFIX 351
#define PORT 352
#define PORTANY 353
#define UL_PROTO 354
#define ANY 355
#define IKE_FRAG 356
#define MODE_CFG 357
#define PFS_GROUP 358
#define LIFETIME 359
#define LIFETYPE_TIME 360
#define LIFETYPE_BYTE 361
#define STRENGTH 362
#define SCRIPT 363
#define PHASE1_UP 364
#define PHASE1_DOWN 365
#define NUMBER 366
#define SWITCH 367
#define BOOLEAN 368
#define HEXSTRING 369
#define QUOTEDSTRING 370
#define ADDRSTRING 371
#define UNITTYPE_BYTE 372
#define UNITTYPE_KBYTES 373
#define UNITTYPE_MBYTES 374
#define UNITTYPE_TBYTES 375
#define UNITTYPE_SEC 376
#define UNITTYPE_MIN 377
#define UNITTYPE_HOUR 378
#define EOS 379
#define BOC 380
#define EOC 381
#define COMMA 382




/* Copy the first part of user declarations.  */
#line 3 "./cfparse.y"

/*
 * Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002 and 2003 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include <netinet/in.h>
#ifdef HAVE_NETINET6_IPSEC
#  include <netinet6/ipsec.h>
#else
#  include <netinet/ipsec.h>
#endif

#ifdef ENABLE_HYBRID
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <pwd.h>
#include <grp.h>

#include "var.h"
#include "misc.h"
#include "vmbuf.h"
#include "plog.h"
#include "sockmisc.h"
#include "str2val.h"
#include "genlist.h"
#include "debug.h"
#ifdef ENABLE_ADMINPORT
#include "admin.h"
#endif

#include "cfparse_proto.h"
#include "cftoken_proto.h"
#include "algorithm.h"
#include "localconf.h"
#include "policy.h"
#include "sainfo.h"
#include "oakley.h"
#include "pfkey.h"
#include "remoteconf.h"
#include "grabmyaddr.h"
#include "isakmp_var.h"
#include "handler.h"
#include "isakmp.h"
#ifdef ENABLE_HYBRID
#include "isakmp_xauth.h"
#include "isakmp_cfg.h"
#endif
#include "ipsec_doi.h"
#include "strnames.h"
#include "gcmalloc.h"
#ifdef HAVE_GSSAPI
#include "gssapi.h"
#endif
#include "vendorid.h"
#include "rsalist.h"

struct proposalspec {
	time_t lifetime;		/* for isakmp/ipsec */
	int lifebyte;			/* for isakmp/ipsec */
	struct secprotospec *spspec;	/* the head is always current spec. */
	struct proposalspec *next;	/* the tail is the most prefered. */
	struct proposalspec *prev;
};

struct secprotospec {
	int prop_no;
	int trns_no;
	int strength;		/* for isakmp/ipsec */
	int encklen;		/* for isakmp/ipsec */
	time_t lifetime;	/* for isakmp */
	int lifebyte;		/* for isakmp */
	int proto_id;		/* for ipsec (isakmp?) */
	int ipsec_level;	/* for ipsec */
	int encmode;		/* for ipsec */
	int vendorid;		/* for isakmp */
	char *gssid;
	struct sockaddr *remote;
	int algclass[MAXALGCLASS];

	struct secprotospec *next;	/* the tail is the most prefiered. */
	struct secprotospec *prev;
	struct proposalspec *back;
};

static int num2dhgroup[] = {
	0,
	OAKLEY_ATTR_GRP_DESC_MODP768,
	OAKLEY_ATTR_GRP_DESC_MODP1024,
	OAKLEY_ATTR_GRP_DESC_EC2N155,
	OAKLEY_ATTR_GRP_DESC_EC2N185,
	OAKLEY_ATTR_GRP_DESC_MODP1536,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	OAKLEY_ATTR_GRP_DESC_MODP2048,
	OAKLEY_ATTR_GRP_DESC_MODP3072,
	OAKLEY_ATTR_GRP_DESC_MODP4096,
	OAKLEY_ATTR_GRP_DESC_MODP6144,
	OAKLEY_ATTR_GRP_DESC_MODP8192
};

static struct remoteconf *cur_rmconf;
static int tmpalgtype[MAXALGCLASS];
static struct sainfo *cur_sainfo;
static int cur_algclass;

static struct proposalspec *newprspec __P((void));
static void insprspec __P((struct proposalspec *, struct proposalspec **));
static struct secprotospec *newspspec __P((void));
static void insspspec __P((struct secprotospec *, struct proposalspec **));
static void adminsock_conf __P((vchar_t *, vchar_t *, vchar_t *, int));

static int set_isakmp_proposal
	__P((struct remoteconf *, struct proposalspec *));
static void clean_tmpalgtype __P((void));
static int expand_isakmpspec __P((int, int, int *,
	int, int, time_t, int, int, int, char *, struct remoteconf *));
static int listen_addr __P((struct sockaddr *addr, int udp_encap));

void freeetypes (struct etypes **etypes);

#if 0
static int fix_lifebyte __P((u_long));
#endif


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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 172 "./cfparse.y"
{
	unsigned long num;
	vchar_t *val;
	struct remoteconf *rmconf;
	struct sockaddr *saddr;
	struct sainfoalg *alg;
}
/* Line 187 of yacc.c.  */
#line 527 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 540 "y.tab.c"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   339

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  128
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  138
/* YYNRULES -- Number of rules.  */
#define YYNRULES  253
/* YYNRULES -- Number of states.  */
#define YYNSTATES  463

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   382

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    30,    36,    37,    42,
      46,    49,    50,    55,    56,    61,    65,    67,    69,    74,
      75,    78,    79,    84,    85,    90,    91,    96,    97,   102,
     103,   108,   113,   114,   117,   118,   123,   124,   129,   130,
     135,   136,   144,   145,   150,   151,   156,   157,   161,   164,
     165,   167,   172,   173,   176,   177,   182,   183,   188,   189,
     194,   195,   200,   201,   206,   207,   212,   213,   218,   219,
     224,   225,   230,   231,   236,   237,   242,   243,   248,   249,
     254,   259,   260,   263,   264,   269,   270,   276,   277,   282,
     283,   289,   290,   296,   297,   303,   304,   305,   314,   316,
     319,   325,   328,   329,   333,   334,   337,   338,   343,   344,
     351,   352,   359,   360,   365,   366,   371,   372,   378,   380,
     381,   386,   389,   390,   392,   393,   395,   397,   399,   401,
     403,   404,   406,   407,   414,   415,   420,   424,   427,   429,
     430,   433,   434,   439,   440,   445,   446,   451,   454,   455,
     460,   461,   467,   468,   474,   475,   481,   482,   487,   488,
     493,   494,   499,   500,   505,   506,   512,   513,   518,   519,
     525,   526,   531,   532,   537,   538,   543,   544,   549,   550,
     555,   556,   562,   563,   569,   570,   575,   576,   581,   582,
     587,   588,   593,   594,   599,   600,   605,   606,   611,   612,
     617,   618,   623,   624,   629,   630,   637,   638,   643,   644,
     651,   652,   658,   659,   662,   663,   669,   670,   675,   677,
     679,   680,   682,   684,   685,   688,   689,   694,   695,   702,
     703,   710,   711,   716,   717,   722,   723,   729,   731,   733,
     735,   737,   739,   741
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     129,     0,    -1,    -1,   129,   130,    -1,   131,    -1,   135,
      -1,   136,    -1,   140,    -1,   142,    -1,   150,    -1,   162,
      -1,   178,    -1,   187,    -1,   208,    -1,   133,    -1,    -1,
       3,     4,   115,   132,   124,    -1,    -1,    90,   112,   134,
     124,    -1,     5,   115,   124,    -1,     6,   137,    -1,    -1,
       7,   138,   115,   124,    -1,    -1,    66,   115,   139,   124,
      -1,     8,   141,   124,    -1,   114,    -1,     9,    -1,    10,
     125,   143,   126,    -1,    -1,   143,   144,    -1,    -1,    11,
     112,   145,   124,    -1,    -1,    12,   112,   146,   124,    -1,
      -1,    13,   111,   147,   124,    -1,    -1,    14,   112,   148,
     124,    -1,    -1,    15,   112,   149,   124,    -1,    16,   125,
     151,   126,    -1,    -1,   151,   152,    -1,    -1,    17,   160,
     153,   124,    -1,    -1,    18,   160,   154,   124,    -1,    -1,
      19,   155,    97,   124,    -1,    -1,    21,   115,   115,   115,
     111,   156,   124,    -1,    -1,    21,   115,   157,   124,    -1,
      -1,    21,    22,   158,   124,    -1,    -1,    20,   159,   124,
      -1,   116,   161,    -1,    -1,    97,    -1,    23,   125,   163,
     126,    -1,    -1,   163,   164,    -1,    -1,    24,   116,   165,
     124,    -1,    -1,    25,   116,   166,   124,    -1,    -1,    26,
     116,   167,   124,    -1,    -1,    27,   116,   168,   124,    -1,
      -1,    28,    29,   169,   124,    -1,    -1,    28,    30,   170,
     124,    -1,    -1,    33,    32,   171,   124,    -1,    -1,    33,
      30,   172,   124,    -1,    -1,    36,   111,   173,   124,    -1,
      -1,    37,   111,   174,   124,    -1,    -1,    34,    31,   175,
     124,    -1,    -1,    34,    30,   176,   124,    -1,    -1,    35,
     115,   177,   124,    -1,    38,   125,   179,   126,    -1,    -1,
     179,   180,    -1,    -1,    39,   111,   181,   124,    -1,    -1,
      40,   111,   264,   182,   124,    -1,    -1,    41,   111,   183,
     124,    -1,    -1,    42,   111,   264,   184,   124,    -1,    -1,
      43,   111,   264,   185,   124,    -1,    -1,    44,   111,   264,
     186,   124,    -1,    -1,    -1,    48,   188,   190,   192,   125,
     193,   189,   126,    -1,    51,    -1,   191,   191,    -1,    66,
     116,   204,   205,   206,    -1,    66,   115,    -1,    -1,    49,
      66,   255,    -1,    -1,   193,   194,    -1,    -1,   103,   254,
     195,   124,    -1,    -1,   104,   105,   111,   264,   196,   124,
      -1,    -1,   104,   106,   111,   265,   197,   124,    -1,    -1,
      45,   198,   201,   124,    -1,    -1,     6,    66,   199,   124,
      -1,    -1,    67,    66,   115,   200,   124,    -1,   203,    -1,
      -1,   203,   202,   127,   201,    -1,    46,   207,    -1,    -1,
      96,    -1,    -1,    97,    -1,    98,    -1,   111,    -1,    99,
      -1,   100,    -1,    -1,   111,    -1,    -1,    50,   212,    52,
     212,   209,   211,    -1,    -1,    50,   212,   210,   211,    -1,
     125,   213,   126,    -1,    51,   161,    -1,   160,    -1,    -1,
     213,   214,    -1,    -1,    53,   215,   250,   124,    -1,    -1,
      55,    56,   216,   124,    -1,    -1,    57,    58,   217,   124,
      -1,    59,   251,    -1,    -1,    61,   115,   218,   124,    -1,
      -1,    62,    71,   115,   219,   124,    -1,    -1,    61,    71,
     115,   220,   124,    -1,    -1,    61,    72,   115,   221,   124,
      -1,    -1,    61,    70,   222,   124,    -1,    -1,    63,   112,
     223,   124,    -1,    -1,    64,   112,   224,   124,    -1,    -1,
      65,   112,   225,   124,    -1,    -1,    67,    66,   255,   226,
     124,    -1,    -1,    95,   255,   227,   124,    -1,    -1,    68,
      66,   255,   228,   124,    -1,    -1,    69,   112,   229,   124,
      -1,    -1,    73,   111,   230,   124,    -1,    -1,    74,   231,
     254,   124,    -1,    -1,    76,   112,   232,   124,    -1,    -1,
     101,   112,   233,   124,    -1,    -1,   108,   115,   109,   234,
     124,    -1,    -1,   108,   115,   110,   235,   124,    -1,    -1,
     102,   112,   236,   124,    -1,    -1,    82,   112,   237,   124,
      -1,    -1,    83,   112,   238,   124,    -1,    -1,    77,   112,
     239,   124,    -1,    -1,    78,   112,   240,   124,    -1,    -1,
      78,    79,   241,   124,    -1,    -1,    91,   112,   242,   124,
      -1,    -1,    92,   111,   243,   124,    -1,    -1,    93,   111,
     244,   124,    -1,    -1,    94,   111,   245,   124,    -1,    -1,
     104,   105,   111,   264,   246,   124,    -1,    -1,    80,    81,
     247,   124,    -1,    -1,   104,   106,   111,   265,   248,   124,
      -1,    -1,    84,   249,   125,   256,   126,    -1,    -1,   250,
      54,    -1,    -1,    71,   115,   115,   252,   124,    -1,    -1,
      72,   115,   253,   124,    -1,    46,    -1,   111,    -1,    -1,
     116,    -1,   115,    -1,    -1,   256,   257,    -1,    -1,   107,
     258,    47,   124,    -1,    -1,   104,   105,   111,   264,   259,
     124,    -1,    -1,   104,   106,   111,   265,   260,   124,    -1,
      -1,    74,   254,   261,   124,    -1,    -1,    89,   115,   262,
     124,    -1,    -1,    45,    46,   207,   263,   124,    -1,   121,
      -1,   122,    -1,   123,    -1,   117,    -1,   118,    -1,   119,
      -1,   120,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   248,   248,   250,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   269,   268,   288,   288,   293,
     307,   311,   310,   316,   315,   331,   334,   343,   356,   358,
     360,   363,   363,   364,   364,   365,   365,   366,   366,   367,
     367,   372,   374,   376,   380,   379,   385,   384,   394,   393,
     399,   398,   408,   407,   417,   416,   425,   425,   428,   440,
     441,   445,   447,   449,   453,   452,   464,   463,   475,   474,
     486,   485,   497,   496,   506,   505,   519,   518,   528,   527,
     541,   540,   550,   549,   559,   558,   568,   567,   581,   580,
     595,   597,   599,   603,   602,   608,   607,   613,   612,   618,
     617,   623,   622,   628,   627,   641,   649,   640,   683,   688,
     695,   744,   773,   777,   803,   805,   809,   808,   814,   813,
     819,   818,   830,   830,   835,   834,   840,   839,   847,   852,
     851,   858,   914,   915,   918,   919,   920,   923,   924,   925,
     928,   929,   935,   934,   961,   960,   984,  1055,  1061,  1070,
    1072,  1076,  1075,  1080,  1080,  1081,  1081,  1082,  1084,  1083,
    1093,  1092,  1101,  1100,  1108,  1107,  1131,  1130,  1140,  1140,
    1141,  1141,  1142,  1142,  1144,  1143,  1153,  1152,  1167,  1166,
    1183,  1183,  1184,  1184,  1186,  1185,  1191,  1191,  1192,  1192,
    1193,  1193,  1196,  1196,  1199,  1199,  1200,  1200,  1201,  1201,
    1202,  1202,  1204,  1203,  1212,  1211,  1220,  1219,  1228,  1227,
    1237,  1236,  1246,  1245,  1255,  1254,  1259,  1259,  1261,  1260,
    1275,  1274,  1285,  1287,  1311,  1310,  1320,  1319,  1341,  1349,
    1361,  1362,  1363,  1365,  1367,  1371,  1370,  1375,  1374,  1380,
    1379,  1392,  1391,  1397,  1396,  1406,  1405,  1503,  1504,  1505,
    1508,  1509,  1510,  1511
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PATH", "PATHTYPE", "INCLUDE",
  "IDENTIFIER", "VENDORID", "LOGGING", "LOGLEV", "PADDING",
  "PAD_RANDOMIZE", "PAD_RANDOMIZELEN", "PAD_MAXLEN", "PAD_STRICT",
  "PAD_EXCLTAIL", "LISTEN", "X_ISAKMP", "X_ISAKMP_NATT", "X_ADMIN",
  "STRICT_ADDRESS", "ADMINSOCK", "DISABLED", "MODECFG", "CFG_NET4",
  "CFG_MASK4", "CFG_DNS4", "CFG_NBNS4", "CFG_AUTH_SOURCE", "CFG_SYSTEM",
  "CFG_RADIUS", "CFG_LOCAL", "CFG_NONE", "CFG_ACCOUNTING",
  "CFG_CONF_SOURCE", "CFG_MOTD", "CFG_POOL_SIZE", "CFG_AUTH_THROTTLE",
  "RETRY", "RETRY_COUNTER", "RETRY_INTERVAL", "RETRY_PERSEND",
  "RETRY_PHASE1", "RETRY_PHASE2", "NATT_KA", "ALGORITHM_CLASS",
  "ALGORITHMTYPE", "STRENGTHTYPE", "SAINFO", "FROM", "REMOTE", "ANONYMOUS",
  "INHERIT", "EXCHANGE_MODE", "EXCHANGETYPE", "DOI", "DOITYPE",
  "SITUATION", "SITUATIONTYPE", "CERTIFICATE_TYPE", "CERTTYPE",
  "PEERS_CERTFILE", "CA_TYPE", "VERIFY_CERT", "SEND_CERT", "SEND_CR",
  "IDENTIFIERTYPE", "MY_IDENTIFIER", "PEERS_IDENTIFIER",
  "VERIFY_IDENTIFIER", "DNSSEC", "CERT_X509", "CERT_PLAINRSA",
  "NONCE_SIZE", "DH_GROUP", "KEEPALIVE", "PASSIVE", "INITIAL_CONTACT",
  "NAT_TRAVERSAL", "NAT_TRAVERSAL_LEVEL", "PROPOSAL_CHECK",
  "PROPOSAL_CHECK_LEVEL", "GENERATE_POLICY", "SUPPORT_PROXY", "PROPOSAL",
  "EXEC_PATH", "EXEC_COMMAND", "EXEC_SUCCESS", "EXEC_FAILURE", "GSSAPI_ID",
  "COMPLEX_BUNDLE", "DPD", "DPD_DELAY", "DPD_RETRY", "DPD_MAXFAIL",
  "XAUTH_LOGIN", "PREFIX", "PORT", "PORTANY", "UL_PROTO", "ANY",
  "IKE_FRAG", "MODE_CFG", "PFS_GROUP", "LIFETIME", "LIFETYPE_TIME",
  "LIFETYPE_BYTE", "STRENGTH", "SCRIPT", "PHASE1_UP", "PHASE1_DOWN",
  "NUMBER", "SWITCH", "BOOLEAN", "HEXSTRING", "QUOTEDSTRING", "ADDRSTRING",
  "UNITTYPE_BYTE", "UNITTYPE_KBYTES", "UNITTYPE_MBYTES", "UNITTYPE_TBYTES",
  "UNITTYPE_SEC", "UNITTYPE_MIN", "UNITTYPE_HOUR", "EOS", "BOC", "EOC",
  "COMMA", "$accept", "statements", "statement", "path_statement", "@1",
  "special_statement", "@2", "include_statement", "identifier_statement",
  "identifier_stmt", "@3", "@4", "logging_statement", "log_level",
  "padding_statement", "padding_stmts", "padding_stmt", "@5", "@6", "@7",
  "@8", "@9", "listen_statement", "listen_stmts", "listen_stmt", "@10",
  "@11", "@12", "@13", "@14", "@15", "@16", "ike_addrinfo_port",
  "ike_port", "modecfg_statement", "modecfg_stmts", "modecfg_stmt", "@17",
  "@18", "@19", "@20", "@21", "@22", "@23", "@24", "@25", "@26", "@27",
  "@28", "@29", "timer_statement", "timer_stmts", "timer_stmt", "@30",
  "@31", "@32", "@33", "@34", "@35", "sainfo_statement", "@36", "@37",
  "sainfo_name", "sainfo_id", "sainfo_peer", "sainfo_specs", "sainfo_spec",
  "@38", "@39", "@40", "@41", "@42", "@43", "algorithms", "@44",
  "algorithm", "prefix", "port", "ul_proto", "keylength",
  "remote_statement", "@45", "@46", "remote_specs_block", "remote_index",
  "remote_specs", "remote_spec", "@47", "@48", "@49", "@50", "@51", "@52",
  "@53", "@54", "@55", "@56", "@57", "@58", "@59", "@60", "@61", "@62",
  "@63", "@64", "@65", "@66", "@67", "@68", "@69", "@70", "@71", "@72",
  "@73", "@74", "@75", "@76", "@77", "@78", "@79", "@80", "@81",
  "exchange_types", "cert_spec", "@82", "@83", "dh_group_num",
  "identifierstring", "isakmpproposal_specs", "isakmpproposal_spec", "@84",
  "@85", "@86", "@87", "@88", "@89", "unittype_time", "unittype_byte", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   128,   129,   129,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   132,   131,   134,   133,   135,
     136,   138,   137,   139,   137,   140,   141,   141,   142,   143,
     143,   145,   144,   146,   144,   147,   144,   148,   144,   149,
     144,   150,   151,   151,   153,   152,   154,   152,   155,   152,
     156,   152,   157,   152,   158,   152,   159,   152,   160,   161,
     161,   162,   163,   163,   165,   164,   166,   164,   167,   164,
     168,   164,   169,   164,   170,   164,   171,   164,   172,   164,
     173,   164,   174,   164,   175,   164,   176,   164,   177,   164,
     178,   179,   179,   181,   180,   182,   180,   183,   180,   184,
     180,   185,   180,   186,   180,   188,   189,   187,   190,   190,
     191,   191,   192,   192,   193,   193,   195,   194,   196,   194,
     197,   194,   198,   194,   199,   194,   200,   194,   201,   202,
     201,   203,   204,   204,   205,   205,   205,   206,   206,   206,
     207,   207,   209,   208,   210,   208,   211,   212,   212,   213,
     213,   215,   214,   216,   214,   217,   214,   214,   218,   214,
     219,   214,   220,   214,   221,   214,   222,   214,   223,   214,
     224,   214,   225,   214,   226,   214,   227,   214,   228,   214,
     229,   214,   230,   214,   231,   214,   232,   214,   233,   214,
     234,   214,   235,   214,   236,   214,   237,   214,   238,   214,
     239,   214,   240,   214,   241,   214,   242,   214,   243,   214,
     244,   214,   245,   214,   246,   214,   247,   214,   248,   214,
     249,   214,   250,   250,   252,   251,   253,   251,   254,   254,
     255,   255,   255,   256,   256,   258,   257,   259,   257,   260,
     257,   261,   257,   262,   257,   263,   257,   264,   264,   264,
     265,   265,   265,   265
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     0,     4,     3,
       2,     0,     4,     0,     4,     3,     1,     1,     4,     0,
       2,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     4,     0,     2,     0,     4,     0,     4,     0,     4,
       0,     7,     0,     4,     0,     4,     0,     3,     2,     0,
       1,     4,     0,     2,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       4,     0,     2,     0,     4,     0,     5,     0,     4,     0,
       5,     0,     5,     0,     5,     0,     0,     8,     1,     2,
       5,     2,     0,     3,     0,     2,     0,     4,     0,     6,
       0,     6,     0,     4,     0,     4,     0,     5,     1,     0,
       4,     2,     0,     1,     0,     1,     1,     1,     1,     1,
       0,     1,     0,     6,     0,     4,     3,     2,     1,     0,
       2,     0,     4,     0,     4,     0,     4,     2,     0,     4,
       0,     5,     0,     5,     0,     5,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     5,     0,     4,     0,     5,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     5,     0,     5,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     6,     0,     4,     0,     6,
       0,     5,     0,     2,     0,     5,     0,     4,     1,     1,
       0,     1,     1,     0,     2,     0,     4,     0,     6,     0,
       6,     0,     4,     0,     4,     0,     5,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,   105,     0,     0,     3,     4,    14,     5,     6,     7,
       8,     9,    10,    11,    12,    13,     0,     0,    21,     0,
      20,    27,    26,     0,    29,    42,    62,    91,     0,    59,
      59,   148,   144,    17,    15,    19,     0,    23,    25,     0,
       0,     0,     0,   108,     0,   112,     0,    60,   147,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    30,     0,     0,    48,    56,     0,    41,    43,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    63,     0,     0,     0,     0,     0,     0,    90,    92,
     111,   132,     0,     0,   109,   142,   149,   145,    18,    16,
      22,    24,    31,    33,    35,    37,    39,    44,    46,     0,
       0,    54,    52,    64,    66,    68,    70,    72,    74,    78,
      76,    86,    84,    88,    80,    82,    93,     0,    97,     0,
       0,     0,   133,   134,   230,   114,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    57,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   247,   248,   249,    95,     0,    99,
     101,   103,   135,   136,     0,   232,   231,   113,   106,   143,
     151,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   184,     0,     0,     0,     0,     0,     0,
     220,     0,     0,     0,     0,   230,     0,     0,     0,     0,
     146,   150,    32,    34,    36,    38,    40,    45,    47,    49,
      55,     0,    53,    65,    67,    69,    71,    73,    75,    79,
      77,    87,    85,    89,    81,    83,    94,     0,    98,     0,
       0,     0,   138,   139,   137,   110,     0,   122,     0,     0,
       0,     0,   115,   222,   153,   155,     0,     0,   157,   166,
       0,     0,   158,     0,   168,   170,   172,   230,   230,   180,
     182,     0,   186,   200,   204,   202,   216,   196,   198,     0,
     206,   208,   210,   212,   176,   188,   194,     0,     0,     0,
      50,    96,   100,   102,   104,   124,     0,     0,   228,   229,
     116,     0,     0,   107,     0,     0,     0,     0,   226,     0,
     162,   164,     0,   160,     0,     0,     0,   174,   178,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   233,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   190,
     192,     0,     0,   140,     0,   128,   126,     0,     0,     0,
     223,   152,   154,   156,   224,     0,   167,     0,     0,   159,
       0,   169,   171,   173,     0,     0,   181,   183,   185,   187,
     201,   205,   203,   217,   197,   199,     0,   207,   209,   211,
     213,   177,   189,   195,   214,   250,   251,   252,   253,   218,
       0,     0,    51,   125,   141,   131,   123,     0,     0,   117,
     118,   120,     0,   227,   163,   165,   161,   175,   179,     0,
       0,     0,     0,   235,   221,   234,     0,     0,   191,   193,
       0,   127,     0,     0,   225,   140,   241,   243,     0,     0,
       0,   215,   219,   130,   119,   121,   245,     0,     0,     0,
       0,     0,     0,   242,   244,   237,   239,   236,   246,     0,
       0,   238,   240
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    14,    15,    63,    16,    62,    17,    18,    30,
      46,    65,    19,    33,    20,    49,    72,   148,   149,   150,
     151,   152,    21,    50,    79,   153,   154,   119,   351,   159,
     157,   120,    41,    58,    22,    51,    91,   160,   161,   162,
     163,   164,   165,   167,   166,   171,   172,   169,   168,   170,
      23,    52,    99,   173,   247,   178,   249,   250,   251,    24,
      38,   261,    55,    56,   103,   188,   262,   357,   432,   433,
     306,   352,   408,   354,   407,   355,   143,   184,   255,   405,
      25,   146,    61,   107,    42,   147,   221,   263,   315,   316,
     322,   370,   367,   368,   319,   324,   325,   326,   374,   344,
     375,   329,   330,   281,   332,   345,   400,   401,   346,   337,
     338,   333,   335,   334,   340,   341,   342,   343,   426,   336,
     427,   289,   314,   268,   412,   365,   310,   187,   386,   425,
     440,   459,   460,   447,   448,   452,   177,   399
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -343
static const yytype_int16 yypact[] =
{
    -343,    33,  -343,    40,   -67,    19,     6,   -70,   -62,   -55,
     -43,  -343,   -27,   -26,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,   -24,   -29,  -343,    -2,
    -343,  -343,  -343,   -17,  -343,  -343,  -343,  -343,   -14,    13,
      13,  -343,    70,  -343,  -343,  -343,    11,  -343,  -343,    -7,
      -8,    -5,    18,  -343,   -65,    75,    62,  -343,  -343,  -343,
     -27,     5,    14,    23,    27,    29,    20,    49,    32,    50,
      56,  -343,  -343,    53,    53,  -343,  -343,    -6,  -343,  -343,
      54,    55,    59,    61,    37,    10,    63,    57,    67,    68,
    -343,  -343,    71,    73,    74,    76,    77,    78,  -343,  -343,
    -343,    85,   117,    65,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,    89,
      69,  -343,    79,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,   -49,  -343,   -49,
     -49,   -49,  -343,     1,   -15,  -343,     5,    72,    80,    81,
      82,    83,    87,    88,    90,    91,  -343,    92,    84,    93,
      94,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,  -343,  -343,  -343,  -343,   109,  -343,
    -343,  -343,  -343,  -343,   -35,  -343,  -343,  -343,     8,  -343,
    -343,   135,   134,    31,   -25,   124,   122,   123,   125,   130,
     131,   126,   128,  -343,   129,   132,   -44,   119,   133,   136,
    -343,   137,   139,   140,   141,   -15,   142,   143,    -1,    95,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,   145,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,   112,  -343,   116,
     118,   138,  -343,  -343,  -343,  -343,   147,  -343,   177,   -19,
       9,   120,  -343,  -343,  -343,  -343,   144,   146,  -343,  -343,
     148,   149,  -343,   150,  -343,  -343,  -343,   -15,   -15,  -343,
    -343,   -19,  -343,  -343,  -343,  -343,  -343,  -343,  -343,   151,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,   155,   156,     7,
    -343,  -343,  -343,  -343,  -343,  -343,   157,   153,  -343,  -343,
    -343,   158,   159,  -343,   -36,   154,   160,   162,  -343,   161,
    -343,  -343,   163,  -343,   164,   165,   166,  -343,  -343,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,  -343,
     178,   179,   180,   181,   182,   183,   184,   -49,   -40,  -343,
    -343,   185,   187,   190,   188,   152,  -343,   189,   -49,   -40,
    -343,  -343,  -343,  -343,  -343,   191,  -343,   192,   193,  -343,
     194,  -343,  -343,  -343,   195,   196,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,   -20,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
     197,   198,  -343,  -343,  -343,  -343,  -343,   199,   200,  -343,
    -343,  -343,   201,  -343,  -343,  -343,  -343,  -343,  -343,   207,
     -19,   208,    52,  -343,  -343,  -343,   203,   204,  -343,  -343,
     157,  -343,   205,   206,  -343,   190,  -343,  -343,   220,   221,
     210,  -343,  -343,  -343,  -343,  -343,  -343,   209,   211,   -49,
     -40,   212,   213,  -343,  -343,  -343,  -343,  -343,  -343,   214,
     215,  -343,  -343
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,    86,   218,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,   216,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -229,  -343,  -343,  -343,  -343,  -343,  -233,
    -343,  -343,  -343,   114,   222,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -278,  -181,  -343,  -343,
    -343,  -343,  -343,  -343,  -343,  -343,  -139,  -342
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -130
static const yytype_int16 yytable[] =
{
     179,   180,   181,   331,    66,    67,    68,    69,    70,    73,
      74,    75,    76,    77,   256,    31,   121,   411,   360,    80,
      81,    82,    83,    84,    39,   419,    28,   308,    85,    86,
      87,    88,    89,     2,   294,   284,     3,    53,     4,     5,
     129,     6,   130,     7,    26,   269,   270,   271,    27,     8,
     100,   101,    54,   257,   420,    34,     9,    92,    93,    94,
      95,    96,    97,    35,   252,   253,   127,   128,   285,   421,
      36,    10,   174,   175,   176,   258,   254,   395,   396,   397,
     398,    11,    37,    12,   422,    29,    43,   423,   361,    40,
     272,    44,   309,   131,   132,    45,   327,   328,   182,   183,
     185,   186,   266,   267,   297,   298,   424,    48,   456,   122,
      57,   259,   260,    47,   311,   312,   349,   350,    78,    71,
      32,    90,    60,    13,   102,   190,    64,   191,    54,   192,
     106,   193,   112,   194,   195,   196,   197,   198,   108,   199,
     200,   201,   436,   114,    98,   202,   203,   109,   204,   205,
     206,   110,   207,   111,   208,   209,   210,   438,   439,   117,
     118,   113,   115,   211,   212,   213,   214,   215,   116,    40,
     123,   124,   133,   216,   217,   125,   218,   126,   134,   135,
     219,   142,   136,   144,   137,   138,   155,   139,   140,   141,
     145,   264,   265,   156,   158,   273,   277,   278,   220,   231,
     286,   443,   446,   353,   222,   223,   224,   225,   394,     0,
     299,   226,   227,   305,   228,   229,   230,   232,   233,   410,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   248,   274,   275,   301,   276,   279,   280,
     302,   282,   303,   307,   283,   287,   313,     0,   288,   290,
     291,   292,   293,   435,   295,   296,   300,   451,    59,   317,
     189,   318,   304,   320,   321,   323,   347,   348,   356,   358,
     359,     0,   104,     0,     0,     0,   339,   364,   362,  -129,
       0,     0,   105,     0,   363,   366,     0,   369,   371,   372,
     373,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   404,   387,   388,   389,   390,   391,   392,   393,   402,
     455,   403,   406,   409,     0,   413,   414,   415,   416,   417,
     418,   428,   429,   437,   431,   434,   430,   441,   442,   444,
     445,   449,   450,   453,     0,   454,   457,   458,   461,   462
};

static const yytype_int16 yycheck[] =
{
     139,   140,   141,   281,    11,    12,    13,    14,    15,    17,
      18,    19,    20,    21,     6,     9,    22,   359,    54,    24,
      25,    26,    27,    28,    51,    45,     7,    46,    33,    34,
      35,    36,    37,     0,   215,    79,     3,    51,     5,     6,
      30,     8,    32,    10,     4,    70,    71,    72,   115,    16,
     115,   116,    66,    45,    74,   125,    23,    39,    40,    41,
      42,    43,    44,   125,    99,   100,    29,    30,   112,    89,
     125,    38,   121,   122,   123,    67,   111,   117,   118,   119,
     120,    48,   125,    50,   104,    66,   112,   107,   124,   116,
     115,   115,   111,    30,    31,   124,   277,   278,    97,    98,
     115,   116,    71,    72,   105,   106,   126,   124,   450,   115,
      97,   103,   104,   115,   105,   106,   109,   110,   126,   126,
     114,   126,    52,    90,    49,    53,   115,    55,    66,    57,
     125,    59,   112,    61,    62,    63,    64,    65,   124,    67,
      68,    69,   420,   111,   126,    73,    74,   124,    76,    77,
      78,   124,    80,   124,    82,    83,    84,   105,   106,    73,
      74,   112,   112,    91,    92,    93,    94,    95,   112,   116,
     116,   116,   115,   101,   102,   116,   104,   116,   111,   111,
     108,    96,   111,    66,   111,   111,    97,   111,   111,   111,
     125,    56,    58,   124,   115,    71,    66,    66,   126,   115,
      81,   430,   435,    46,   124,   124,   124,   124,   347,    -1,
     115,   124,   124,    66,   124,   124,   124,   124,   124,   358,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   112,   112,   124,   112,   112,   111,
     124,   112,   124,    66,   112,   112,   126,    -1,   112,   112,
     111,   111,   111,    46,   112,   112,   111,    47,    40,   115,
     146,   115,   124,   115,   115,   115,   111,   111,   115,   111,
     111,    -1,    56,    -1,    -1,    -1,   125,   115,   124,   127,
      -1,    -1,    60,    -1,   124,   124,    -1,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   111,   124,   124,   124,   124,   124,   124,   124,   124,
     449,   124,   124,   124,    -1,   124,   124,   124,   124,   124,
     124,   124,   124,   115,   124,   124,   127,   124,   124,   124,
     124,   111,   111,   124,    -1,   124,   124,   124,   124,   124
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   129,     0,     3,     5,     6,     8,    10,    16,    23,
      38,    48,    50,    90,   130,   131,   133,   135,   136,   140,
     142,   150,   162,   178,   187,   208,     4,   115,     7,    66,
     137,     9,   114,   141,   125,   125,   125,   125,   188,    51,
     116,   160,   212,   112,   115,   124,   138,   115,   124,   143,
     151,   163,   179,    51,    66,   190,   191,    97,   161,   161,
      52,   210,   134,   132,   115,   139,    11,    12,    13,    14,
      15,   126,   144,    17,    18,    19,    20,    21,   126,   152,
      24,    25,    26,    27,    28,    33,    34,    35,    36,    37,
     126,   164,    39,    40,    41,    42,    43,    44,   126,   180,
     115,   116,    49,   192,   191,   212,   125,   211,   124,   124,
     124,   124,   112,   112,   111,   112,   112,   160,   160,   155,
     159,    22,   115,   116,   116,   116,   116,    29,    30,    30,
      32,    30,    31,   115,   111,   111,   111,   111,   111,   111,
     111,   111,    96,   204,    66,   125,   209,   213,   145,   146,
     147,   148,   149,   153,   154,    97,   124,   158,   115,   157,
     165,   166,   167,   168,   169,   170,   172,   171,   176,   175,
     177,   173,   174,   181,   121,   122,   123,   264,   183,   264,
     264,   264,    97,    98,   205,   115,   116,   255,   193,   211,
      53,    55,    57,    59,    61,    62,    63,    64,    65,    67,
      68,    69,    73,    74,    76,    77,    78,    80,    82,    83,
      84,    91,    92,    93,    94,    95,   101,   102,   104,   108,
     126,   214,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   115,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   124,   182,   124,   184,
     185,   186,    99,   100,   111,   206,     6,    45,    67,   103,
     104,   189,   194,   215,    56,    58,    71,    72,   251,    70,
      71,    72,   115,    71,   112,   112,   112,    66,    66,   112,
     111,   231,   112,   112,    79,   112,    81,   112,   112,   249,
     112,   111,   111,   111,   255,   112,   112,   105,   106,   115,
     111,   124,   124,   124,   124,    66,   198,    66,    46,   111,
     254,   105,   106,   126,   250,   216,   217,   115,   115,   222,
     115,   115,   218,   115,   223,   224,   225,   255,   255,   229,
     230,   254,   232,   239,   241,   240,   247,   237,   238,   125,
     242,   243,   244,   245,   227,   233,   236,   111,   111,   109,
     110,   156,   199,    46,   201,   203,   115,   195,   111,   111,
      54,   124,   124,   124,   115,   253,   124,   220,   221,   124,
     219,   124,   124,   124,   226,   228,   124,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   256,   124,   124,   124,
     124,   124,   124,   124,   264,   117,   118,   119,   120,   265,
     234,   235,   124,   124,   111,   207,   124,   202,   200,   124,
     264,   265,   252,   124,   124,   124,   124,   124,   124,    45,
      74,    89,   104,   107,   126,   257,   246,   248,   124,   124,
     127,   124,   196,   197,   124,    46,   254,   115,   105,   106,
     258,   124,   124,   201,   124,   124,   207,   261,   262,   111,
     111,    47,   263,   124,   124,   264,   265,   124,   124,   259,
     260,   124,   124
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
        case 15:
#line 269 "./cfparse.y"
    {
			if ((yyvsp[(2) - (3)].num) >= LC_PATHTYPE_MAX) {
				yyerror("invalid path type %d", (yyvsp[(2) - (3)].num));
				return -1;
			}

			/* free old pathinfo */
			if (lcconf->pathinfo[(yyvsp[(2) - (3)].num)])
				racoon_free(lcconf->pathinfo[(yyvsp[(2) - (3)].num)]);

			/* set new pathinfo */
			lcconf->pathinfo[(yyvsp[(2) - (3)].num)] = strdup((yyvsp[(3) - (3)].val)->v);
			vfree((yyvsp[(3) - (3)].val));
		}
    break;

  case 17:
#line 288 "./cfparse.y"
    { lcconf->complex_bundle = (yyvsp[(2) - (2)].num); }
    break;

  case 19:
#line 294 "./cfparse.y"
    {
			char path[MAXPATHLEN];

			getpathname(path, sizeof(path),
				LC_PATHTYPE_INCLUDE, (yyvsp[(2) - (3)].val)->v);
			vfree((yyvsp[(2) - (3)].val));
			if (yycf_switch_buffer(path) != 0)
				return -1;
		}
    break;

  case 21:
#line 311 "./cfparse.y"
    {
			/*XXX to be deleted */
		}
    break;

  case 23:
#line 316 "./cfparse.y"
    {
			/*XXX to be deleted */
			(yyvsp[(2) - (2)].val)->l--;	/* nuke '\0' */
			lcconf->ident[(yyvsp[(1) - (2)].num)] = (yyvsp[(2) - (2)].val);
			if (lcconf->ident[(yyvsp[(1) - (2)].num)] == NULL) {
				yyerror("failed to set my ident: %s",
					strerror(errno));
				return -1;
			}
		}
    break;

  case 26:
#line 335 "./cfparse.y"
    {
			/*
			 * XXX ignore it because this specification
			 * will be obsoleted.
			 */
			yywarn("see racoon.conf(5), such a log specification will be obsoleted.");
			vfree((yyvsp[(1) - (1)].val));
		}
    break;

  case 27:
#line 344 "./cfparse.y"
    {
			/*
			 * set the loglevel by configuration file only when
			 * the command line did not specify any loglevel.
			 */
			if (loglevel <= LLV_BASE)
				loglevel += (yyvsp[(1) - (1)].num);
		}
    break;

  case 31:
#line 363 "./cfparse.y"
    { lcconf->pad_random = (yyvsp[(2) - (2)].num); }
    break;

  case 33:
#line 364 "./cfparse.y"
    { lcconf->pad_randomlen = (yyvsp[(2) - (2)].num); }
    break;

  case 35:
#line 365 "./cfparse.y"
    { lcconf->pad_maxsize = (yyvsp[(2) - (2)].num); }
    break;

  case 37:
#line 366 "./cfparse.y"
    { lcconf->pad_strict = (yyvsp[(2) - (2)].num); }
    break;

  case 39:
#line 367 "./cfparse.y"
    { lcconf->pad_excltail = (yyvsp[(2) - (2)].num); }
    break;

  case 44:
#line 380 "./cfparse.y"
    {
			listen_addr ((yyvsp[(2) - (2)].saddr), 0);
		}
    break;

  case 46:
#line 385 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			listen_addr ((yyvsp[(2) - (2)].saddr), 1);
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 48:
#line 394 "./cfparse.y"
    {
			yyerror("admin directive is obsoleted.");
		}
    break;

  case 50:
#line 399 "./cfparse.y"
    {
#ifdef ENABLE_ADMINPORT
			adminsock_conf((yyvsp[(2) - (5)].val), (yyvsp[(3) - (5)].val), (yyvsp[(4) - (5)].val), (yyvsp[(5) - (5)].num));
#else
			yywarn("admin port support not compiled in");
#endif
		}
    break;

  case 52:
#line 408 "./cfparse.y"
    {
#ifdef ENABLE_ADMINPORT
			adminsock_conf((yyvsp[(2) - (2)].val), NULL, NULL, -1);
#else
			yywarn("admin port support not compiled in");
#endif
		}
    break;

  case 54:
#line 417 "./cfparse.y"
    {
#ifdef ENABLE_ADMINPORT
			adminsock_path = NULL;
#else
			yywarn("admin port support not compiled in");
#endif
		}
    break;

  case 56:
#line 425 "./cfparse.y"
    { lcconf->strict_address = TRUE; }
    break;

  case 58:
#line 429 "./cfparse.y"
    {
			char portbuf[10];

			snprintf(portbuf, sizeof(portbuf), "%ld", (yyvsp[(2) - (2)].num));
			(yyval.saddr) = str2saddr((yyvsp[(1) - (2)].val)->v, portbuf);
			vfree((yyvsp[(1) - (2)].val));
			if (!(yyval.saddr))
				return -1;
		}
    break;

  case 59:
#line 440 "./cfparse.y"
    { (yyval.num) = PORT_ISAKMP; }
    break;

  case 60:
#line 441 "./cfparse.y"
    { (yyval.num) = (yyvsp[(1) - (1)].num); }
    break;

  case 64:
#line 453 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
		 if (inet_pton(AF_INET, (yyvsp[(2) - (2)].val)->v,
		     &isakmp_cfg_config.network4) != 1)
			yyerror("bad IPv4 network address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 66:
#line 464 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, (yyvsp[(2) - (2)].val)->v,
			    &isakmp_cfg_config.netmask4) != 1)
				yyerror("bad IPv4 netmask address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 68:
#line 475 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, (yyvsp[(2) - (2)].val)->v,
			    &isakmp_cfg_config.dns4) != 1)
				yyerror("bad IPv4 DNS address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 70:
#line 486 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, (yyvsp[(2) - (2)].val)->v,
			    &isakmp_cfg_config.nbns4) != 1)
				yyerror("bad IPv4 WINS address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 72:
#line 497 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_SYSTEM;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 74:
#line 506 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 76:
#line 519 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_NONE;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 78:
#line 528 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 80:
#line 541 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.pool_size = (yyvsp[(2) - (2)].num);
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 82:
#line 550 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.auth_throttle = (yyvsp[(2) - (2)].num);
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 84:
#line 559 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_LOCAL;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 86:
#line 568 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 88:
#line 581 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			strncpy(&isakmp_cfg_config.motd[0], (yyvsp[(2) - (2)].val)->v, MAXPATHLEN);
			isakmp_cfg_config.motd[MAXPATHLEN] = '\0';
			vfree((yyvsp[(2) - (2)].val));
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 93:
#line 603 "./cfparse.y"
    {
			lcconf->retry_counter = (yyvsp[(2) - (2)].num);
		}
    break;

  case 95:
#line 608 "./cfparse.y"
    {
			lcconf->retry_interval = (yyvsp[(2) - (3)].num) * (yyvsp[(3) - (3)].num);
		}
    break;

  case 97:
#line 613 "./cfparse.y"
    {
			lcconf->count_persend = (yyvsp[(2) - (2)].num);
		}
    break;

  case 99:
#line 618 "./cfparse.y"
    {
			lcconf->retry_checkph1 = (yyvsp[(2) - (3)].num) * (yyvsp[(3) - (3)].num);
		}
    break;

  case 101:
#line 623 "./cfparse.y"
    {
			lcconf->wait_ph2complete = (yyvsp[(2) - (3)].num) * (yyvsp[(3) - (3)].num);
		}
    break;

  case 103:
#line 628 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			lcconf->natt_ka_interval = (yyvsp[(2) - (3)].num) * (yyvsp[(3) - (3)].num);
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 105:
#line 641 "./cfparse.y"
    {
			cur_sainfo = newsainfo();
			if (cur_sainfo == NULL) {
				yyerror("failed to allocate sainfo");
				return -1;
			}
		}
    break;

  case 106:
#line 649 "./cfparse.y"
    {
			struct sainfo *check;

			/* default */
			if (cur_sainfo->algs[algclass_ipsec_enc] == 0) {
				yyerror("no encryption algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			if (cur_sainfo->algs[algclass_ipsec_auth] == 0) {
				yyerror("no authentication algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			if (cur_sainfo->algs[algclass_ipsec_comp] == 0) {
				yyerror("no compression algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}

			/* duplicate check */
			check = getsainfo(cur_sainfo->idsrc,
					  cur_sainfo->iddst,
					  cur_sainfo->id_i);
			if (check && (!check->idsrc && !cur_sainfo->idsrc)) {
				yyerror("duplicated sainfo: %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			inssainfo(cur_sainfo);
		}
    break;

  case 108:
#line 684 "./cfparse.y"
    {
			cur_sainfo->idsrc = NULL;
			cur_sainfo->iddst = NULL;
		}
    break;

  case 109:
#line 689 "./cfparse.y"
    {
			cur_sainfo->idsrc = (yyvsp[(1) - (2)].val);
			cur_sainfo->iddst = (yyvsp[(2) - (2)].val);
		}
    break;

  case 110:
#line 696 "./cfparse.y"
    {
			char portbuf[10];
			struct sockaddr *saddr;

			if (((yyvsp[(5) - (5)].num) == IPPROTO_ICMP || (yyvsp[(5) - (5)].num) == IPPROTO_ICMPV6)
			 && ((yyvsp[(4) - (5)].num) != IPSEC_PORT_ANY || (yyvsp[(4) - (5)].num) != IPSEC_PORT_ANY)) {
				yyerror("port number must be \"any\".");
				return -1;
			}

			snprintf(portbuf, sizeof(portbuf), "%lu", (yyvsp[(4) - (5)].num));
			saddr = str2saddr((yyvsp[(2) - (5)].val)->v, portbuf);
			vfree((yyvsp[(2) - (5)].val));
			if (saddr == NULL)
				return -1;

			switch (saddr->sa_family) {
			case AF_INET:
				if ((yyvsp[(5) - (5)].num) == IPPROTO_ICMPV6) {
					yyerror("upper layer protocol mismatched.\n");
					racoon_free(saddr);
					return -1;
				}
				(yyval.val) = ipsecdoi_sockaddr2id(saddr,
					(yyvsp[(3) - (5)].num) == ~0 ? (sizeof(struct in_addr) << 3): (yyvsp[(3) - (5)].num),
					(yyvsp[(5) - (5)].num));
				break;
#ifdef INET6
			case AF_INET6:
				if ((yyvsp[(5) - (5)].num) == IPPROTO_ICMP) {
					yyerror("upper layer protocol mismatched.\n");
					racoon_free(saddr);
					return -1;
				}
				(yyval.val) = ipsecdoi_sockaddr2id(saddr,
					(yyvsp[(3) - (5)].num) == ~0 ? (sizeof(struct in6_addr) << 3) : (yyvsp[(3) - (5)].num),
					(yyvsp[(5) - (5)].num));
				break;
#endif
			default:
				yyerror("invalid family: %d", saddr->sa_family);
				(yyval.val) = NULL;
				break;
			}
			racoon_free(saddr);
			if ((yyval.val) == NULL)
				return -1;
		}
    break;

  case 111:
#line 745 "./cfparse.y"
    {
			struct ipsecdoi_id_b *id_b;

			if ((yyvsp[(1) - (2)].num) == IDTYPE_ASN1DN) {
				yyerror("id type forbidden: %d", (yyvsp[(1) - (2)].num));
				(yyval.val) = NULL;
				return -1;
			}

			(yyvsp[(2) - (2)].val)->l--;

			(yyval.val) = vmalloc(sizeof(*id_b) + (yyvsp[(2) - (2)].val)->l);
			if ((yyval.val) == NULL) {
				yyerror("failed to allocate identifier");
				return -1;
			}

			id_b = (struct ipsecdoi_id_b *)(yyval.val)->v;
			id_b->type = idtype2doi((yyvsp[(1) - (2)].num));

			id_b->proto_id = 0;
			id_b->port = 0;

			memcpy((yyval.val)->v + sizeof(*id_b), (yyvsp[(2) - (2)].val)->v, (yyvsp[(2) - (2)].val)->l);
		}
    break;

  case 112:
#line 773 "./cfparse.y"
    {
			cur_sainfo->id_i = NULL;
		}
    break;

  case 113:
#line 778 "./cfparse.y"
    {
			struct ipsecdoi_id_b *id_b;
			vchar_t *idv;

			if (set_identifier(&idv, (yyvsp[(2) - (3)].num), (yyvsp[(3) - (3)].val)) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_sainfo->id_i = vmalloc(sizeof(*id_b) + idv->l);
			if (cur_sainfo->id_i == NULL) {
				yyerror("failed to allocate identifier");
				return -1;
			}

			id_b = (struct ipsecdoi_id_b *)cur_sainfo->id_i->v;
			id_b->type = idtype2doi((yyvsp[(2) - (3)].num));

			id_b->proto_id = 0;
			id_b->port = 0;

			memcpy(cur_sainfo->id_i->v + sizeof(*id_b),
			       idv->v, idv->l);
			vfree(idv);
		}
    break;

  case 116:
#line 809 "./cfparse.y"
    {
			cur_sainfo->pfs_group = (yyvsp[(2) - (2)].num);
		}
    break;

  case 118:
#line 814 "./cfparse.y"
    {
			cur_sainfo->lifetime = (yyvsp[(3) - (4)].num) * (yyvsp[(4) - (4)].num);
		}
    break;

  case 120:
#line 819 "./cfparse.y"
    {
#if 1
			yyerror("byte lifetime support is deprecated");
			return -1;
#else
			cur_sainfo->lifebyte = fix_lifebyte((yyvsp[(3) - (4)].num) * (yyvsp[(4) - (4)].num));
			if (cur_sainfo->lifebyte == 0)
				return -1;
#endif
		}
    break;

  case 122:
#line 830 "./cfparse.y"
    {
			cur_algclass = (yyvsp[(1) - (1)].num);
		}
    break;

  case 124:
#line 835 "./cfparse.y"
    {
			yyerror("it's deprecated to specify a identifier in phase 2");
		}
    break;

  case 126:
#line 840 "./cfparse.y"
    {
			yyerror("it's deprecated to specify a identifier in phase 2");
		}
    break;

  case 128:
#line 848 "./cfparse.y"
    {
			inssainfoalg(&cur_sainfo->algs[cur_algclass], (yyvsp[(1) - (1)].alg));
		}
    break;

  case 129:
#line 852 "./cfparse.y"
    {
			inssainfoalg(&cur_sainfo->algs[cur_algclass], (yyvsp[(1) - (1)].alg));
		}
    break;

  case 131:
#line 859 "./cfparse.y"
    {
			int defklen;

			(yyval.alg) = newsainfoalg();
			if ((yyval.alg) == NULL) {
				yyerror("failed to get algorithm allocation");
				return -1;
			}

			(yyval.alg)->alg = algtype2doi(cur_algclass, (yyvsp[(1) - (2)].num));
			if ((yyval.alg)->alg == -1) {
				yyerror("algorithm mismatched");
				racoon_free((yyval.alg));
				(yyval.alg) = NULL;
				return -1;
			}

			defklen = default_keylen(cur_algclass, (yyvsp[(1) - (2)].num));
			if (defklen == 0) {
				if ((yyvsp[(2) - (2)].num)) {
					yyerror("keylen not allowed");
					racoon_free((yyval.alg));
					(yyval.alg) = NULL;
					return -1;
				}
			} else {
				if ((yyvsp[(2) - (2)].num) && check_keylen(cur_algclass, (yyvsp[(1) - (2)].num), (yyvsp[(2) - (2)].num)) < 0) {
					yyerror("invalid keylen %d", (yyvsp[(2) - (2)].num));
					racoon_free((yyval.alg));
					(yyval.alg) = NULL;
					return -1;
				}
			}

			if ((yyvsp[(2) - (2)].num))
				(yyval.alg)->encklen = (yyvsp[(2) - (2)].num);
			else
				(yyval.alg)->encklen = defklen;

			/* check if it's supported algorithm by kernel */
			if (!(cur_algclass == algclass_ipsec_auth && (yyvsp[(1) - (2)].num) == algtype_non_auth)
			 && pk_checkalg(cur_algclass, (yyvsp[(1) - (2)].num), (yyval.alg)->encklen)) {
				int a = algclass2doi(cur_algclass);
				int b = algtype2doi(cur_algclass, (yyvsp[(1) - (2)].num));
				if (a == IPSECDOI_ATTR_AUTH)
					a = IPSECDOI_PROTO_IPSEC_AH;
				yyerror("algorithm %s not supported by the kernel (missing module?)",
					s_ipsecdoi_trns(a, b));
				racoon_free((yyval.alg));
				(yyval.alg) = NULL;
				return -1;
			}
		}
    break;

  case 132:
#line 914 "./cfparse.y"
    { (yyval.num) = ~0; }
    break;

  case 133:
#line 915 "./cfparse.y"
    { (yyval.num) = (yyvsp[(1) - (1)].num); }
    break;

  case 134:
#line 918 "./cfparse.y"
    { (yyval.num) = IPSEC_PORT_ANY; }
    break;

  case 135:
#line 919 "./cfparse.y"
    { (yyval.num) = (yyvsp[(1) - (1)].num); }
    break;

  case 136:
#line 920 "./cfparse.y"
    { (yyval.num) = IPSEC_PORT_ANY; }
    break;

  case 137:
#line 923 "./cfparse.y"
    { (yyval.num) = (yyvsp[(1) - (1)].num); }
    break;

  case 138:
#line 924 "./cfparse.y"
    { (yyval.num) = (yyvsp[(1) - (1)].num); }
    break;

  case 139:
#line 925 "./cfparse.y"
    { (yyval.num) = IPSEC_ULPROTO_ANY; }
    break;

  case 140:
#line 928 "./cfparse.y"
    { (yyval.num) = 0; }
    break;

  case 141:
#line 929 "./cfparse.y"
    { (yyval.num) = (yyvsp[(1) - (1)].num); }
    break;

  case 142:
#line 935 "./cfparse.y"
    {
			struct remoteconf *new;
			struct proposalspec *prspec;

			new = copyrmconf((yyvsp[(4) - (4)].saddr));
			if (new == NULL) {
				yyerror("failed to get remoteconf for %s.", saddr2str ((yyvsp[(4) - (4)].saddr)));
				return -1;
			}

			new->remote = (yyvsp[(2) - (4)].saddr);
			new->inherited_from = getrmconf_strict((yyvsp[(4) - (4)].saddr), 1);
			new->proposal = NULL;
			new->prhead = NULL;
			cur_rmconf = new;

			prspec = newprspec();
			if (prspec == NULL || !cur_rmconf->inherited_from 
				|| !cur_rmconf->inherited_from->proposal)
				return -1;
			prspec->lifetime = cur_rmconf->inherited_from->proposal->lifetime;
			prspec->lifebyte = cur_rmconf->inherited_from->proposal->lifebyte;
			insprspec(prspec, &cur_rmconf->prhead);
		}
    break;

  case 144:
#line 961 "./cfparse.y"
    {
			struct remoteconf *new;
			struct proposalspec *prspec;

			new = newrmconf();
			if (new == NULL) {
				yyerror("failed to get new remoteconf.");
				return -1;
			}

			new->remote = (yyvsp[(2) - (2)].saddr);
			cur_rmconf = new;

			prspec = newprspec();
			if (prspec == NULL)
				return -1;
			prspec->lifetime = oakley_get_defaultlifetime();
			insprspec(prspec, &cur_rmconf->prhead);
		}
    break;

  case 146:
#line 985 "./cfparse.y"
    {
			/* check a exchange mode */
			if (cur_rmconf->etypes == NULL) {
				yyerror("no exchange mode specified.\n");
				return -1;
			}

			if (cur_rmconf->idvtype == IDTYPE_UNDEFINED)
				cur_rmconf->idvtype = IDTYPE_ADDRESS;


			if (cur_rmconf->idvtype == IDTYPE_ASN1DN) {
				if (cur_rmconf->mycertfile) {
					if (cur_rmconf->idv)
						yywarn("Both CERT and ASN1 ID "
						       "are set. Hope this is OK.\n");
					/* TODO: Preparse the DN here */
				} else if (cur_rmconf->idv) {
					/* OK, using asn1dn without X.509. */
				} else {
					yyerror("ASN1 ID not specified "
						"and no CERT defined!\n");
					return -1;
				}
			}
			
			if (cur_rmconf->prhead->spspec == NULL
				&& cur_rmconf->inherited_from
				&& cur_rmconf->inherited_from->prhead) {
				cur_rmconf->prhead->spspec = cur_rmconf->inherited_from->prhead->spspec;
			}
			if (set_isakmp_proposal(cur_rmconf, cur_rmconf->prhead) != 0)
				return -1;

			/* DH group settting if aggressive mode is there. */
			if (check_etypeok(cur_rmconf, ISAKMP_ETYPE_AGG) != NULL) {
				struct isakmpsa *p;
				int b = 0;

				/* DH group */
				for (p = cur_rmconf->proposal; p; p = p->next) {
					if (b == 0 || (b && b == p->dh_group)) {
						b = p->dh_group;
						continue;
					}
					yyerror("DH group must be equal "
						"in all proposals "
						"when aggressive mode is "
						"used.\n");
					return -1;
				}
				cur_rmconf->dh_group = b;

				if (cur_rmconf->dh_group == 0) {
					yyerror("DH group must be set in the proposal.\n");
					return -1;
				}

				/* DH group settting if PFS is required. */
				if (oakley_setdhgroup(cur_rmconf->dh_group,
						&cur_rmconf->dhgrp) < 0) {
					yyerror("failed to set DH value.\n");
					return -1;
				}
			}

			insrmconf(cur_rmconf);
		}
    break;

  case 147:
#line 1056 "./cfparse.y"
    {
			(yyval.saddr) = newsaddr(sizeof(struct sockaddr));
			(yyval.saddr)->sa_family = AF_UNSPEC;
			((struct sockaddr_in *)(yyval.saddr))->sin_port = htons((yyvsp[(2) - (2)].num));
		}
    break;

  case 148:
#line 1062 "./cfparse.y"
    {
			(yyval.saddr) = (yyvsp[(1) - (1)].saddr);
			if ((yyval.saddr) == NULL) {
				yyerror("failed to allocate sockaddr");
				return -1;
			}
		}
    break;

  case 151:
#line 1076 "./cfparse.y"
    {
			cur_rmconf->etypes = NULL;
		}
    break;

  case 153:
#line 1080 "./cfparse.y"
    { cur_rmconf->doitype = (yyvsp[(2) - (2)].num); }
    break;

  case 155:
#line 1081 "./cfparse.y"
    { cur_rmconf->sittype = (yyvsp[(2) - (2)].num); }
    break;

  case 158:
#line 1084 "./cfparse.y"
    {
			yywarn("This directive without certtype will be removed!\n");
			yywarn("Please use 'peers_certfile x509 \"%s\";' instead\n", (yyvsp[(2) - (2)].val)->v);
			cur_rmconf->getcert_method = ISAKMP_GETCERT_LOCALFILE;
			cur_rmconf->peerscertfile = strdup((yyvsp[(2) - (2)].val)->v);
			vfree((yyvsp[(2) - (2)].val));
		}
    break;

  case 160:
#line 1093 "./cfparse.y"
    {
			cur_rmconf->cacerttype = (yyvsp[(2) - (3)].num);
			cur_rmconf->getcacert_method = ISAKMP_GETCERT_LOCALFILE;
			cur_rmconf->cacertfile = strdup((yyvsp[(3) - (3)].val)->v);
			vfree((yyvsp[(3) - (3)].val));
		}
    break;

  case 162:
#line 1101 "./cfparse.y"
    {
			cur_rmconf->getcert_method = ISAKMP_GETCERT_LOCALFILE;
			cur_rmconf->peerscertfile = strdup((yyvsp[(3) - (3)].val)->v);
			vfree((yyvsp[(3) - (3)].val));
		}
    break;

  case 164:
#line 1108 "./cfparse.y"
    {
			char path[MAXPATHLEN];
			int ret = 0;

			getpathname(path, sizeof(path),
				LC_PATHTYPE_CERT, (yyvsp[(3) - (3)].val)->v);
			vfree((yyvsp[(3) - (3)].val));

			if (cur_rmconf->getcert_method == ISAKMP_GETCERT_DNS) {
				yyerror("Different peers_certfile method "
					"already defined: %d!\n",
					cur_rmconf->getcert_method);
				return -1;
			}
			cur_rmconf->getcert_method = ISAKMP_GETCERT_LOCALFILE;
			if (rsa_parse_file(cur_rmconf->rsa_public, path, RSA_TYPE_PUBLIC)) {
				yyerror("Couldn't parse keyfile.\n", path);
				return -1;
			}
			plog(LLV_DEBUG, LOCATION, NULL, "Public PlainRSA keyfile parsed: %s\n", path);
		}
    break;

  case 166:
#line 1131 "./cfparse.y"
    {
			if (cur_rmconf->getcert_method) {
				yyerror("Different peers_certfile method already defined!\n");
				return -1;
			}
			cur_rmconf->getcert_method = ISAKMP_GETCERT_DNS;
			cur_rmconf->peerscertfile = NULL;
		}
    break;

  case 168:
#line 1140 "./cfparse.y"
    { cur_rmconf->verify_cert = (yyvsp[(2) - (2)].num); }
    break;

  case 170:
#line 1141 "./cfparse.y"
    { cur_rmconf->send_cert = (yyvsp[(2) - (2)].num); }
    break;

  case 172:
#line 1142 "./cfparse.y"
    { cur_rmconf->send_cr = (yyvsp[(2) - (2)].num); }
    break;

  case 174:
#line 1144 "./cfparse.y"
    {
			if (set_identifier(&cur_rmconf->idv, (yyvsp[(2) - (3)].num), (yyvsp[(3) - (3)].val)) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_rmconf->idvtype = (yyvsp[(2) - (3)].num);
		}
    break;

  case 176:
#line 1153 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			/* formerly identifier type login */
			if (set_identifier(&cur_rmconf->idv, IDTYPE_LOGIN, (yyvsp[(2) - (2)].val)) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			/* cur_rmconf->use_xauth = 1; */
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 178:
#line 1167 "./cfparse.y"
    {
			struct idspec  *id;
			id = newidspec();
			if (id == NULL) {
				yyerror("failed to allocate idspec");
				return -1;
			}
			if (set_identifier(&id->id, (yyvsp[(2) - (3)].num), (yyvsp[(3) - (3)].val)) != 0) {
				yyerror("failed to set identifer.\n");
				racoon_free(id);
				return -1;
			}
			id->idtype = (yyvsp[(2) - (3)].num);
			genlist_append (cur_rmconf->idvl_p, id);
		}
    break;

  case 180:
#line 1183 "./cfparse.y"
    { cur_rmconf->verify_identifier = (yyvsp[(2) - (2)].num); }
    break;

  case 182:
#line 1184 "./cfparse.y"
    { cur_rmconf->nonce_size = (yyvsp[(2) - (2)].num); }
    break;

  case 184:
#line 1186 "./cfparse.y"
    {
			yyerror("dh_group cannot be defined here.");
			return -1;
		}
    break;

  case 186:
#line 1191 "./cfparse.y"
    { cur_rmconf->passive = (yyvsp[(2) - (2)].num); }
    break;

  case 188:
#line 1192 "./cfparse.y"
    { cur_rmconf->ike_frag = (yyvsp[(2) - (2)].num); }
    break;

  case 190:
#line 1193 "./cfparse.y"
    { 
			cur_rmconf->script[SCRIPT_PHASE1_UP] = vdup((yyvsp[(2) - (3)].val));
		}
    break;

  case 192:
#line 1196 "./cfparse.y"
    { 
			cur_rmconf->script[SCRIPT_PHASE1_DOWN] = vdup((yyvsp[(2) - (3)].val));
		}
    break;

  case 194:
#line 1199 "./cfparse.y"
    { cur_rmconf->mode_cfg = (yyvsp[(2) - (2)].num); }
    break;

  case 196:
#line 1200 "./cfparse.y"
    { cur_rmconf->gen_policy = (yyvsp[(2) - (2)].num); }
    break;

  case 198:
#line 1201 "./cfparse.y"
    { cur_rmconf->support_proxy = (yyvsp[(2) - (2)].num); }
    break;

  case 200:
#line 1202 "./cfparse.y"
    { cur_rmconf->ini_contact = (yyvsp[(2) - (2)].num); }
    break;

  case 202:
#line 1204 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			cur_rmconf->nat_traversal = (yyvsp[(2) - (2)].num);
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 204:
#line 1212 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			cur_rmconf->nat_traversal = (yyvsp[(2) - (2)].num);
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 206:
#line 1220 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd = (yyvsp[(2) - (2)].num);
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 208:
#line 1228 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd_interval = (yyvsp[(2) - (2)].num);
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 210:
#line 1237 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd_retry = (yyvsp[(2) - (2)].num);
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 212:
#line 1246 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd_maxfails = (yyvsp[(2) - (2)].num);
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 214:
#line 1255 "./cfparse.y"
    {
			cur_rmconf->prhead->lifetime = (yyvsp[(3) - (4)].num) * (yyvsp[(4) - (4)].num);
		}
    break;

  case 216:
#line 1259 "./cfparse.y"
    { cur_rmconf->pcheck_level = (yyvsp[(2) - (2)].num); }
    break;

  case 218:
#line 1261 "./cfparse.y"
    {
#if 1
			yyerror("byte lifetime support is deprecated in Phase1");
			return -1;
#else
			yywarn("the lifetime of bytes in phase 1 "
				"will be ignored at the moment.");
			cur_rmconf->prhead->lifebyte = fix_lifebyte((yyvsp[(3) - (4)].num) * (yyvsp[(4) - (4)].num));
			if (cur_rmconf->prhead->lifebyte == 0)
				return -1;
#endif
		}
    break;

  case 220:
#line 1275 "./cfparse.y"
    {
			struct secprotospec *spspec;

			spspec = newspspec();
			if (spspec == NULL)
				return -1;
			insspspec(spspec, &cur_rmconf->prhead);
		}
    break;

  case 223:
#line 1288 "./cfparse.y"
    {
			struct etypes *new;
			new = racoon_malloc(sizeof(struct etypes));
			if (new == NULL) {
				yyerror("filed to allocate etypes");
				return -1;
			}
			new->type = (yyvsp[(2) - (2)].num);
			new->next = NULL;
			if (cur_rmconf->etypes == NULL)
				cur_rmconf->etypes = new;
			else {
				struct etypes *p;
				for (p = cur_rmconf->etypes;
				     p->next != NULL;
				     p = p->next)
					;
				p->next = new;
			}
		}
    break;

  case 224:
#line 1311 "./cfparse.y"
    {
			cur_rmconf->certtype = (yyvsp[(1) - (3)].num);
			cur_rmconf->mycertfile = strdup((yyvsp[(2) - (3)].val)->v);
			vfree((yyvsp[(2) - (3)].val));
			cur_rmconf->myprivfile = strdup((yyvsp[(3) - (3)].val)->v);
			vfree((yyvsp[(3) - (3)].val));
		}
    break;

  case 226:
#line 1320 "./cfparse.y"
    {
			char path[MAXPATHLEN];
			int ret = 0;

			getpathname(path, sizeof(path),
				LC_PATHTYPE_CERT, (yyvsp[(2) - (2)].val)->v);
			vfree((yyvsp[(2) - (2)].val));

			cur_rmconf->certtype = (yyvsp[(1) - (2)].num);
			cur_rmconf->send_cr = FALSE;
			cur_rmconf->send_cert = FALSE;
			cur_rmconf->verify_cert = FALSE;
			if (rsa_parse_file(cur_rmconf->rsa_private, path, RSA_TYPE_PRIVATE)) {
				yyerror("Couldn't parse keyfile.\n", path);
				return -1;
			}
			plog(LLV_DEBUG, LOCATION, NULL, "Private PlainRSA keyfile parsed: %s\n", path);
		}
    break;

  case 228:
#line 1342 "./cfparse.y"
    {
			(yyval.num) = algtype2doi(algclass_isakmp_dh, (yyvsp[(1) - (1)].num));
			if ((yyval.num) == -1) {
				yyerror("must be DH group");
				return -1;
			}
		}
    break;

  case 229:
#line 1350 "./cfparse.y"
    {
			if (ARRAYLEN(num2dhgroup) > (yyvsp[(1) - (1)].num) && num2dhgroup[(yyvsp[(1) - (1)].num)] != 0) {
				(yyval.num) = num2dhgroup[(yyvsp[(1) - (1)].num)];
			} else {
				yyerror("must be DH group");
				(yyval.num) = 0;
				return -1;
			}
		}
    break;

  case 230:
#line 1361 "./cfparse.y"
    { (yyval.val) = NULL; }
    break;

  case 231:
#line 1362 "./cfparse.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); }
    break;

  case 232:
#line 1363 "./cfparse.y"
    { (yyval.val) = (yyvsp[(1) - (1)].val); }
    break;

  case 235:
#line 1371 "./cfparse.y"
    {
			yyerror("strength directive is obsoleted.");
		}
    break;

  case 237:
#line 1375 "./cfparse.y"
    {
			cur_rmconf->prhead->spspec->lifetime = (yyvsp[(3) - (4)].num) * (yyvsp[(4) - (4)].num);
		}
    break;

  case 239:
#line 1380 "./cfparse.y"
    {
#if 1
			yyerror("byte lifetime support is deprecated");
			return -1;
#else
			cur_rmconf->prhead->spspec->lifebyte = fix_lifebyte((yyvsp[(3) - (4)].num) * (yyvsp[(4) - (4)].num));
			if (cur_rmconf->prhead->spspec->lifebyte == 0)
				return -1;
#endif
		}
    break;

  case 241:
#line 1392 "./cfparse.y"
    {
			cur_rmconf->prhead->spspec->algclass[algclass_isakmp_dh] = (yyvsp[(2) - (2)].num);
		}
    break;

  case 243:
#line 1397 "./cfparse.y"
    {
			if (cur_rmconf->prhead->spspec->vendorid != VENDORID_GSSAPI) {
				yyerror("wrong Vendor ID for gssapi_id");
				return -1;
			}
			cur_rmconf->prhead->spspec->gssid = strdup((yyvsp[(2) - (2)].val)->v);
		}
    break;

  case 245:
#line 1406 "./cfparse.y"
    {
			int doi;
			int defklen;

			doi = algtype2doi((yyvsp[(1) - (3)].num), (yyvsp[(2) - (3)].num));
			if (doi == -1) {
				yyerror("algorithm mismatched 1");
				return -1;
			}

			switch ((yyvsp[(1) - (3)].num)) {
			case algclass_isakmp_enc:
			/* reject suppressed algorithms */
#ifndef HAVE_OPENSSL_RC5_H
				if ((yyvsp[(2) - (3)].num) == algtype_rc5) {
					yyerror("algorithm %s not supported",
					    s_attr_isakmp_enc(doi));
					return -1;
				}
#endif
#ifndef HAVE_OPENSSL_IDEA_H
				if ((yyvsp[(2) - (3)].num) == algtype_idea) {
					yyerror("algorithm %s not supported",
					    s_attr_isakmp_enc(doi));
					return -1;
				}
#endif

				cur_rmconf->prhead->spspec->algclass[algclass_isakmp_enc] = doi;
				defklen = default_keylen((yyvsp[(1) - (3)].num), (yyvsp[(2) - (3)].num));
				if (defklen == 0) {
					if ((yyvsp[(3) - (3)].num)) {
						yyerror("keylen not allowed");
						return -1;
					}
				} else {
					if ((yyvsp[(3) - (3)].num) && check_keylen((yyvsp[(1) - (3)].num), (yyvsp[(2) - (3)].num), (yyvsp[(3) - (3)].num)) < 0) {
						yyerror("invalid keylen %d", (yyvsp[(3) - (3)].num));
						return -1;
					}
				}
				if ((yyvsp[(3) - (3)].num))
					cur_rmconf->prhead->spspec->encklen = (yyvsp[(3) - (3)].num);
				else
					cur_rmconf->prhead->spspec->encklen = defklen;
				break;
			case algclass_isakmp_hash:
				cur_rmconf->prhead->spspec->algclass[algclass_isakmp_hash] = doi;
				break;
			case algclass_isakmp_ameth:
				cur_rmconf->prhead->spspec->algclass[algclass_isakmp_ameth] = doi;
				/*
				 * We may have to set the Vendor ID for the
				 * authentication method we're using.
				 */
				switch ((yyvsp[(2) - (3)].num)) {
				case algtype_gssapikrb:
					if (cur_rmconf->prhead->spspec->vendorid !=
					    VENDORID_UNKNOWN) {
						yyerror("Vendor ID mismatch "
						    "for auth method");
						return -1;
					}
					/*
					 * For interoperability with Win2k,
					 * we set the Vendor ID to "GSSAPI".
					 */
					cur_rmconf->prhead->spspec->vendorid =
					    VENDORID_GSSAPI;
					break;
				case algtype_rsasig:
					if (cur_rmconf->certtype == ISAKMP_CERT_PLAINRSA) {
						if (rsa_list_count(cur_rmconf->rsa_private) == 0) {
							yyerror ("Private PlainRSA key not set. "
								"Use directive 'certificate_type plainrsa ...'\n");
							return -1;
						}
						if (rsa_list_count(cur_rmconf->rsa_public) == 0) {
							yyerror ("Public PlainRSA keys not set. "
								"Use directive 'peers_certfile plainrsa ...'\n");
							return -1;
						}
					}
					break;
				default:
					break;
				}
				break;
			default:
				yyerror("algorithm mismatched 2");
				return -1;
			}
		}
    break;

  case 247:
#line 1503 "./cfparse.y"
    { (yyval.num) = 1; }
    break;

  case 248:
#line 1504 "./cfparse.y"
    { (yyval.num) = 60; }
    break;

  case 249:
#line 1505 "./cfparse.y"
    { (yyval.num) = (60 * 60); }
    break;

  case 250:
#line 1508 "./cfparse.y"
    { (yyval.num) = 1; }
    break;

  case 251:
#line 1509 "./cfparse.y"
    { (yyval.num) = 1024; }
    break;

  case 252:
#line 1510 "./cfparse.y"
    { (yyval.num) = (1024 * 1024); }
    break;

  case 253:
#line 1511 "./cfparse.y"
    { (yyval.num) = (1024 * 1024 * 1024); }
    break;


/* Line 1267 of yacc.c.  */
#line 3675 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule which action triggered
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1513 "./cfparse.y"


static struct proposalspec *
newprspec()
{
	struct proposalspec *new;

	new = racoon_calloc(1, sizeof(*new));
	if (new == NULL)
		yyerror("failed to allocate proposal");

	return new;
}

/*
 * insert into head of list.
 */
static void
insprspec(prspec, head)
	struct proposalspec *prspec;
	struct proposalspec **head;
{
	if (*head != NULL)
		(*head)->prev = prspec;
	prspec->next = *head;
	*head = prspec;
}

static struct secprotospec *
newspspec()
{
	struct secprotospec *new;

	new = racoon_calloc(1, sizeof(*new));
	if (new == NULL) {
		yyerror("failed to allocate spproto");
		return NULL;
	}

	new->encklen = 0;	/*XXX*/

	/*
	 * Default to "uknown" vendor -- we will override this
	 * as necessary.  When we send a Vendor ID payload, an
	 * "unknown" will be translated to a KAME/racoon ID.
	 */
	new->vendorid = VENDORID_UNKNOWN;

	return new;
}

/*
 * insert into head of list.
 */
static void
insspspec(spspec, head)
	struct secprotospec *spspec;
	struct proposalspec **head;
{
	spspec->back = *head;

	if ((*head)->spspec != NULL)
		(*head)->spspec->prev = spspec;
	spspec->next = (*head)->spspec;
	(*head)->spspec = spspec;
}

/* set final acceptable proposal */
static int
set_isakmp_proposal(rmconf, prspec)
	struct remoteconf *rmconf;
	struct proposalspec *prspec;
{
	struct proposalspec *p;
	struct secprotospec *s;
	int prop_no = 1; 
	int trns_no = 1;
	u_int32_t types[MAXALGCLASS];

	p = prspec;
	if (p->next != 0) {
		plog(LLV_ERROR, LOCATION, NULL,
			"multiple proposal definition.\n");
		return -1;
	}

	/* mandatory check */
	if (p->spspec == NULL) {
		yyerror("no remote specification found: %s.\n",
			saddr2str(rmconf->remote));
		return -1;
	}
	for (s = p->spspec; s != NULL; s = s->next) {
		/* XXX need more to check */
		if (s->algclass[algclass_isakmp_enc] == 0) {
			yyerror("encryption algorithm required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_hash] == 0) {
			yyerror("hash algorithm required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_dh] == 0) {
			yyerror("DH group required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_ameth] == 0) {
			yyerror("authentication method required.");
			return -1;
		}
	}

	/* skip to last part */
	for (s = p->spspec; s->next != NULL; s = s->next)
		;

	while (s != NULL) {
		plog(LLV_DEBUG2, LOCATION, NULL,
			"lifetime = %ld\n", (long)
			(s->lifetime ? s->lifetime : p->lifetime));
		plog(LLV_DEBUG2, LOCATION, NULL,
			"lifebyte = %d\n",
			s->lifebyte ? s->lifebyte : p->lifebyte);
		plog(LLV_DEBUG2, LOCATION, NULL,
			"encklen=%d\n", s->encklen);

		memset(types, 0, ARRAYLEN(types));
		types[algclass_isakmp_enc] = s->algclass[algclass_isakmp_enc];
		types[algclass_isakmp_hash] = s->algclass[algclass_isakmp_hash];
		types[algclass_isakmp_dh] = s->algclass[algclass_isakmp_dh];
		types[algclass_isakmp_ameth] =
		    s->algclass[algclass_isakmp_ameth];

		/* expanding spspec */
		clean_tmpalgtype();
		trns_no = expand_isakmpspec(prop_no, trns_no, types,
				algclass_isakmp_enc, algclass_isakmp_ameth + 1,
				s->lifetime ? s->lifetime : p->lifetime,
				s->lifebyte ? s->lifebyte : p->lifebyte,
				s->encklen, s->vendorid, s->gssid,
				rmconf);
		if (trns_no == -1) {
			plog(LLV_ERROR, LOCATION, NULL,
				"failed to expand isakmp proposal.\n");
			return -1;
		}

		s = s->prev;
	}

	if (rmconf->proposal == NULL) {
		plog(LLV_ERROR, LOCATION, NULL,
			"no proposal found.\n");
		return -1;
	}

	return 0;
}

static void
clean_tmpalgtype()
{
	int i;
	for (i = 0; i < MAXALGCLASS; i++)
		tmpalgtype[i] = 0;	/* means algorithm undefined. */
}

static int
expand_isakmpspec(prop_no, trns_no, types,
		class, last, lifetime, lifebyte, encklen, vendorid, gssid,
		rmconf)
	int prop_no, trns_no;
	int *types, class, last;
	time_t lifetime;
	int lifebyte;
	int encklen;
	int vendorid;
	char *gssid;
	struct remoteconf *rmconf;
{
	struct isakmpsa *new;

	/* debugging */
    {
	int j;
	char tb[10];
	plog(LLV_DEBUG2, LOCATION, NULL,
		"p:%d t:%d\n", prop_no, trns_no);
	for (j = class; j < MAXALGCLASS; j++) {
		snprintf(tb, sizeof(tb), "%d", types[j]);
		plog(LLV_DEBUG2, LOCATION, NULL,
			"%s%s%s%s\n",
			s_algtype(j, types[j]),
			types[j] ? "(" : "",
			tb[0] == '0' ? "" : tb,
			types[j] ? ")" : "");
	}
	plog(LLV_DEBUG2, LOCATION, NULL, "\n");
    }

#define TMPALGTYPE2STR(n) \
	s_algtype(algclass_isakmp_##n, types[algclass_isakmp_##n])
		/* check mandatory values */
		if (types[algclass_isakmp_enc] == 0
		 || types[algclass_isakmp_ameth] == 0
		 || types[algclass_isakmp_hash] == 0
		 || types[algclass_isakmp_dh] == 0) {
			yyerror("few definition of algorithm "
				"enc=%s ameth=%s hash=%s dhgroup=%s.\n",
				TMPALGTYPE2STR(enc),
				TMPALGTYPE2STR(ameth),
				TMPALGTYPE2STR(hash),
				TMPALGTYPE2STR(dh));
			return -1;
		}
#undef TMPALGTYPE2STR

	/* set new sa */
	new = newisakmpsa();
	if (new == NULL) {
		yyerror("failed to allocate isakmp sa");
		return -1;
	}
	new->prop_no = prop_no;
	new->trns_no = trns_no++;
	new->lifetime = lifetime;
	new->lifebyte = lifebyte;
	new->enctype = types[algclass_isakmp_enc];
	new->encklen = encklen;
	new->authmethod = types[algclass_isakmp_ameth];
	new->hashtype = types[algclass_isakmp_hash];
	new->dh_group = types[algclass_isakmp_dh];
	new->vendorid = vendorid;
#ifdef HAVE_GSSAPI
	if (gssid != NULL) {
		new->gssid = vmalloc(strlen(gssid) + 1);
		memcpy(new->gssid->v, gssid, new->gssid->l);
		racoon_free(gssid);
	} else
		new->gssid = NULL;
#endif
	insisakmpsa(new, rmconf);

	return trns_no;
}

static int
listen_addr (struct sockaddr *addr, int udp_encap)
{
	struct myaddrs *p;

	p = newmyaddr();
	if (p == NULL) {
		yyerror("failed to allocate myaddrs");
		return -1;
	}
	p->addr = addr;
	if (p->addr == NULL) {
		yyerror("failed to copy sockaddr ");
		delmyaddr(p);
		return -1;
	}
	p->udp_encap = udp_encap;

	insmyaddr(p, &lcconf->myaddrs);

	lcconf->autograbaddr = 0;
	return 0;
}

#if 0
/*
 * fix lifebyte.
 * Must be more than 1024B because its unit is kilobytes.
 * That is defined RFC2407.
 */
static int
fix_lifebyte(t)
	unsigned long t;
{
	if (t < 1024) {
		yyerror("byte size should be more than 1024B.");
		return 0;
	}

	return(t / 1024);
}
#endif

int
cfparse()
{
	int error;

	yycf_init_buffer();

	if (yycf_switch_buffer(lcconf->racoon_conf) != 0)
		return -1;

	error = yyparse();
	if (error != 0) {
		if (yyerrorcount) {
			plog(LLV_ERROR, LOCATION, NULL,
				"fatal parse failure (%d errors)\n",
				yyerrorcount);
		} else {
			plog(LLV_ERROR, LOCATION, NULL,
				"fatal parse failure.\n");
		}
		return -1;
	}

	if (error == 0 && yyerrorcount) {
		plog(LLV_ERROR, LOCATION, NULL,
			"parse error is nothing, but yyerrorcount is %d.\n",
				yyerrorcount);
		exit(1);
	}

	yycf_clean_buffer();

	plog(LLV_DEBUG2, LOCATION, NULL, "parse successed.\n");

	return 0;
}

int
cfreparse()
{
	flushph2();
	flushph1();
	flushrmconf();
	flushsainfo();
	clean_tmpalgtype();
	yycf_init_buffer();

	if (yycf_switch_buffer(lcconf->racoon_conf) != 0)
		return -1;

	return(cfparse());
}

#ifdef ENABLE_ADMINPORT
static void
adminsock_conf(path, owner, group, mode_dec)
	vchar_t *path;
	vchar_t *owner;
	vchar_t *group;
	int mode_dec;
{
	struct passwd *pw = NULL;
	struct group *gr = NULL;
	mode_t mode = 0;
	uid_t uid;
	gid_t gid;
	int isnum;

	adminsock_path = path->v;

	if (owner == NULL)
		return;

	errno = 0;
	uid = atoi(owner->v);
	isnum = !errno;
	if (((pw = getpwnam(owner->v)) == NULL) && !isnum)
		yyerror("User \"%s\" does not exist", owner->v);

	if (pw)
		adminsock_owner = pw->pw_uid;
	else
		adminsock_owner = uid;

	if (group == NULL)
		return;

	errno = 0;
	gid = atoi(group->v);
	isnum = !errno;
	if (((gr = getgrnam(group->v)) == NULL) && !isnum)
		yyerror("Group \"%s\" does not exist", group->v);

	if (gr)
		adminsock_group = gr->gr_gid;
	else
		adminsock_group = gid;

	if (mode_dec == -1)
		return;

	if (mode_dec > 777)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 400) { mode += 0400; mode_dec -= 400; }
	if (mode_dec >= 200) { mode += 0200; mode_dec -= 200; }
	if (mode_dec >= 100) { mode += 0200; mode_dec -= 100; }

	if (mode_dec > 77)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 40) { mode += 040; mode_dec -= 40; }
	if (mode_dec >= 20) { mode += 020; mode_dec -= 20; }
	if (mode_dec >= 10) { mode += 020; mode_dec -= 10; }

	if (mode_dec > 7)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 4) { mode += 04; mode_dec -= 4; }
	if (mode_dec >= 2) { mode += 02; mode_dec -= 2; }
	if (mode_dec >= 1) { mode += 02; mode_dec -= 1; }
	
	adminsock_mode = mode;

	return;
}
#endif


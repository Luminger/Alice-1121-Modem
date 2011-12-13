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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 114 "parse.y"
typedef union YYSTYPE {
	int num;
	unsigned long ulnum;
	vchar_t val;
	struct addrinfo *res;
} YYSTYPE;
/* Line 1249 of yacc.c.  */
#line 151 "parse.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




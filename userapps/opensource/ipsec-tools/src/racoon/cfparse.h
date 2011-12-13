/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1489 of yacc.c.  */
#line 311 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;


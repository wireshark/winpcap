
/*  A Bison parser, made from ../libpcap/grammar.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse pcap_parse
#define yylex pcap_lex
#define yyerror pcap_error
#define yylval pcap_lval
#define yychar pcap_char
#define yydebug pcap_debug
#define yynerrs pcap_nerrs
#define	DST	257
#define	SRC	258
#define	HOST	259
#define	GATEWAY	260
#define	NET	261
#define	MASK	262
#define	PORT	263
#define	LESS	264
#define	GREATER	265
#define	PROTO	266
#define	PROTOCHAIN	267
#define	CBYTE	268
#define	ARP	269
#define	RARP	270
#define	IP	271
#define	SCTP	272
#define	TCP	273
#define	UDP	274
#define	ICMP	275
#define	IGMP	276
#define	IGRP	277
#define	PIM	278
#define	VRRP	279
#define	ATALK	280
#define	AARP	281
#define	DECNET	282
#define	LAT	283
#define	SCA	284
#define	MOPRC	285
#define	MOPDL	286
#define	TK_BROADCAST	287
#define	TK_MULTICAST	288
#define	NUM	289
#define	INBOUND	290
#define	OUTBOUND	291
#define	LINK	292
#define	GEQ	293
#define	LEQ	294
#define	NEQ	295
#define	ID	296
#define	EID	297
#define	HID	298
#define	HID6	299
#define	AID	300
#define	LSH	301
#define	RSH	302
#define	LEN	303
#define	IPV6	304
#define	ICMPV6	305
#define	AH	306
#define	ESP	307
#define	VLAN	308
#define	ISO	309
#define	ESIS	310
#define	ISIS	311
#define	CLNP	312
#define	STP	313
#define	IPX	314
#define	NETBEUI	315
#define	OR	316
#define	AND	317
#define	UMINUS	318

#line 1 "../libpcap/grammar.y"

/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /tcpdump/master/libpcap/grammar.y,v 1.71 2001/07/03 19:15:48 guy Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <stdio.h>

#include "pcap-int.h"

#include "gencode.h"
#include <pcap-namedb.h>

#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#define QSET(q, p, d, a) (q).proto = (p),\
			 (q).dir = (d),\
			 (q).addr = (a)

int n_errors = 0;

static struct qual qerr = { Q_UNDEF, Q_UNDEF, Q_UNDEF, Q_UNDEF };

static void
yyerror(char *msg)
{
	++n_errors;
	bpf_error("%s", msg);
	/* NOTREACHED */
}

#ifndef YYBISON
int yyparse(void);

int
pcap_parse()
{
	return (yyparse());
}
#endif


#line 78 "../libpcap/grammar.y"
typedef union {
	int i;
	bpf_u_int32 h;
	u_char *e;
	char *s;
	struct stmt *stmt;
	struct arth *a;
	struct {
		struct qual q;
		struct block *b;
	} blk;
	struct block *rblk;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		173
#define	YYFLAG		-32768
#define	YYNTBASE	80

#define YYTRANSLATE(x) ((unsigned)(x) <= 318 ? yytranslate[x] : 106)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    64,     2,     2,     2,     2,    66,     2,    73,
    72,    69,    67,     2,    68,     2,    70,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    79,     2,    76,
    75,    74,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    77,     2,    78,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    65,     2,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    71
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     5,     6,     8,    12,    16,    20,    24,    26,
    28,    30,    32,    36,    38,    42,    46,    48,    52,    54,
    56,    58,    61,    63,    65,    67,    71,    75,    77,    79,
    81,    84,    88,    91,    94,    97,   100,   103,   106,   110,
   112,   116,   120,   122,   124,   125,   127,   129,   133,   137,
   141,   145,   147,   149,   151,   153,   155,   157,   159,   161,
   163,   165,   167,   169,   171,   173,   175,   177,   179,   181,
   183,   185,   187,   189,   191,   193,   195,   197,   199,   201,
   203,   205,   207,   209,   211,   213,   216,   219,   222,   225,
   230,   232,   234,   237,   239,   241,   243,   245,   247,   249,
   251,   253,   255,   260,   267,   271,   275,   279,   283,   287,
   291,   295,   299,   302,   306,   308,   310,   312,   314,   316,
   318,   320
};

static const short yyrhs[] = {    81,
    82,     0,    81,     0,     0,    91,     0,    82,    83,    91,
     0,    82,    83,    85,     0,    82,    84,    91,     0,    82,
    84,    85,     0,    63,     0,    62,     0,    86,     0,   105,
     0,    88,    89,    72,     0,    42,     0,    44,    70,    35,
     0,    44,     8,    44,     0,    44,     0,    45,    70,    35,
     0,    45,     0,    43,     0,    46,     0,    87,    85,     0,
    64,     0,    73,     0,    86,     0,    90,    83,    85,     0,
    90,    84,    85,     0,   105,     0,    89,     0,    93,     0,
    87,    91,     0,    94,    95,    96,     0,    94,    95,     0,
    94,    96,     0,    94,    12,     0,    94,    13,     0,    94,
    97,     0,    92,    85,     0,    88,    82,    72,     0,    98,
     0,   102,   100,   102,     0,   102,   101,   102,     0,    99,
     0,    98,     0,     0,     4,     0,     3,     0,     4,    62,
     3,     0,     3,    62,     4,     0,     4,    63,     3,     0,
     3,    63,     4,     0,     5,     0,     7,     0,     9,     0,
     6,     0,    38,     0,    17,     0,    15,     0,    16,     0,
    18,     0,    19,     0,    20,     0,    21,     0,    22,     0,
    23,     0,    24,     0,    25,     0,    26,     0,    27,     0,
    28,     0,    29,     0,    30,     0,    32,     0,    31,     0,
    50,     0,    51,     0,    52,     0,    53,     0,    55,     0,
    56,     0,    57,     0,    58,     0,    59,     0,    60,     0,
    61,     0,    94,    33,     0,    94,    34,     0,    10,    35,
     0,    11,    35,     0,    14,    35,   104,    35,     0,    36,
     0,    37,     0,    54,   105,     0,    54,     0,    74,     0,
    39,     0,    75,     0,    40,     0,    76,     0,    41,     0,
   105,     0,   103,     0,    98,    77,   102,    78,     0,    98,
    77,   102,    79,    35,    78,     0,   102,    67,   102,     0,
   102,    68,   102,     0,   102,    69,   102,     0,   102,    70,
   102,     0,   102,    66,   102,     0,   102,    65,   102,     0,
   102,    47,   102,     0,   102,    48,   102,     0,    68,   102,
     0,    88,   103,    72,     0,    49,     0,    66,     0,    65,
     0,    76,     0,    74,     0,    75,     0,    35,     0,    88,
   105,    72,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   133,   137,   139,   141,   142,   143,   144,   145,   147,   149,
   151,   152,   154,   156,   157,   159,   161,   166,   175,   184,
   193,   202,   204,   206,   208,   209,   210,   212,   214,   216,
   217,   219,   220,   221,   222,   223,   224,   226,   227,   228,
   229,   231,   233,   236,   237,   240,   241,   242,   243,   244,
   245,   248,   249,   250,   253,   255,   256,   257,   258,   259,
   260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
   270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
   280,   281,   282,   283,   284,   286,   287,   288,   289,   290,
   291,   292,   293,   294,   296,   297,   298,   300,   301,   302,
   304,   305,   307,   308,   309,   310,   311,   312,   313,   314,
   315,   316,   317,   318,   319,   321,   322,   323,   324,   325,
   327,   328
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DST","SRC",
"HOST","GATEWAY","NET","MASK","PORT","LESS","GREATER","PROTO","PROTOCHAIN","CBYTE",
"ARP","RARP","IP","SCTP","TCP","UDP","ICMP","IGMP","IGRP","PIM","VRRP","ATALK",
"AARP","DECNET","LAT","SCA","MOPRC","MOPDL","TK_BROADCAST","TK_MULTICAST","NUM",
"INBOUND","OUTBOUND","LINK","GEQ","LEQ","NEQ","ID","EID","HID","HID6","AID",
"LSH","RSH","LEN","IPV6","ICMPV6","AH","ESP","VLAN","ISO","ESIS","ISIS","CLNP",
"STP","IPX","NETBEUI","OR","AND","'!'","'|'","'&'","'+'","'-'","'*'","'/'","UMINUS",
"')'","'('","'>'","'='","'<'","'['","']'","':'","prog","null","expr","and","or",
"id","nid","not","paren","pid","qid","term","head","rterm","pqual","dqual","aqual",
"ndaqual","pname","other","relop","irelop","arth","narth","byteop","pnum", NULL
};
#endif

static const short yyr1[] = {     0,
    80,    80,    81,    82,    82,    82,    82,    82,    83,    84,
    85,    85,    85,    86,    86,    86,    86,    86,    86,    86,
    86,    86,    87,    88,    89,    89,    89,    90,    90,    91,
    91,    92,    92,    92,    92,    92,    92,    93,    93,    93,
    93,    93,    93,    94,    94,    95,    95,    95,    95,    95,
    95,    96,    96,    96,    97,    98,    98,    98,    98,    98,
    98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
    98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
    98,    98,    98,    98,    98,    99,    99,    99,    99,    99,
    99,    99,    99,    99,   100,   100,   100,   101,   101,   101,
   102,   102,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   104,   104,   104,   104,   104,
   105,   105
};

static const short yyr2[] = {     0,
     2,     1,     0,     1,     3,     3,     3,     3,     1,     1,
     1,     1,     3,     1,     3,     3,     1,     3,     1,     1,
     1,     2,     1,     1,     1,     3,     3,     1,     1,     1,
     2,     3,     2,     2,     2,     2,     2,     2,     3,     1,
     3,     3,     1,     1,     0,     1,     1,     3,     3,     3,
     3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     2,     2,     2,     4,
     1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     4,     6,     3,     3,     3,     3,     3,     3,
     3,     3,     2,     3,     1,     1,     1,     1,     1,     1,
     1,     3
};

static const short yydefact[] = {     3,
    45,     0,     0,     0,    58,    59,    57,    60,    61,    62,
    63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
    74,    73,   121,    91,    92,    56,   115,    75,    76,    77,
    78,    94,    79,    80,    81,    82,    83,    84,    85,    23,
     0,    24,     1,    45,    45,     4,     0,    30,     0,    44,
    43,     0,   102,   101,    88,    89,     0,     0,    93,     0,
     0,   113,    10,     9,    45,    45,    31,     0,   102,   101,
    14,    20,    17,    19,    21,    38,    11,     0,     0,    12,
    47,    46,    52,    55,    53,    54,    35,    36,    86,    87,
    33,    34,    37,     0,    96,    98,   100,     0,     0,     0,
     0,     0,     0,     0,     0,    95,    97,    99,     0,     0,
   117,   116,   119,   120,   118,     0,     0,     0,     6,    45,
    45,     5,   101,     8,     7,    39,   114,   122,     0,     0,
     0,    22,    25,    29,     0,    28,     0,     0,     0,     0,
    32,     0,   111,   112,   110,   109,   105,   106,   107,   108,
    41,    42,    90,   101,    16,    15,    18,    13,     0,     0,
    49,    51,    48,    50,   103,     0,    26,    27,     0,   104,
     0,     0,     0
};

static const short yydefgoto[] = {   171,
     1,    68,    65,    66,   132,    77,    78,    60,   134,   135,
    46,    47,    48,    49,    91,    92,    93,    61,    51,   109,
   110,    52,    53,   116,    54
};

static const short yypact[] = {-32768,
   146,   -22,    -9,    -5,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -20,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   325,-32768,    11,   266,   266,-32768,   145,-32768,   396,    19,
-32768,   348,-32768,-32768,-32768,-32768,    -7,   -20,-32768,   325,
   -72,-32768,-32768,-32768,   206,   206,-32768,    25,   -44,   -27,
-32768,-32768,    -4,   -28,-32768,-32768,-32768,   145,   145,-32768,
    30,    44,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     3,-32768,-32768,   325,-32768,-32768,-32768,   325,   325,   325,
   325,   325,   325,   325,   325,-32768,-32768,-32768,   325,   325,
-32768,-32768,-32768,-32768,-32768,    17,   -27,    75,-32768,   206,
   206,-32768,    23,-32768,-32768,-32768,-32768,-32768,    12,    26,
    27,-32768,-32768,     8,    11,   -27,    59,    68,    81,   105,
-32768,    69,   238,   238,   -30,    85,   -37,   -37,-32768,-32768,
    75,    75,-32768,    49,-32768,-32768,-32768,-32768,   145,   145,
-32768,-32768,-32768,-32768,-32768,    78,-32768,-32768,    40,-32768,
   124,   127,-32768
};

static const short yypgoto[] = {-32768,
-32768,   148,    15,    16,   -45,   -70,     5,    -1,-32768,-32768,
   -41,-32768,-32768,-32768,-32768,    88,-32768,    10,-32768,-32768,
-32768,     0,   -38,-32768,   -31
};


#define	YYLAST		430


static const short yytable[] = {    45,
    59,    76,    67,   129,    94,    44,    69,    83,   133,    85,
    50,    86,    55,    70,    23,    80,    98,    99,   -40,   119,
   124,    69,   -12,   122,   125,    56,   117,   127,    70,    57,
    58,   104,   105,   123,   123,   101,   102,   103,   104,   105,
    62,   131,    45,    45,   128,    79,    80,   136,    44,    44,
   133,   153,    42,    50,    50,   155,    58,   111,   112,   118,
   156,   157,   161,   121,   121,   130,   113,   114,   115,   120,
   120,   162,    63,    64,    50,    50,    79,    58,    67,   158,
   -40,   -40,    69,   163,   -12,   -12,    63,    64,   123,   154,
   -40,   137,   138,   142,   -12,    94,   126,   143,   144,   145,
   146,   147,   148,   149,   150,   139,   140,   164,   151,   152,
   -28,   -28,   169,   167,   168,    98,    99,   170,   121,    45,
   128,    98,    99,   172,   120,   120,   173,    80,    80,    50,
    50,    98,    99,   100,   101,   102,   103,   104,   105,   100,
   101,   102,   103,   104,   105,    -2,   165,   166,    43,   159,
   160,   102,   103,   104,   105,     2,     3,    79,    79,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,   141,    23,
    23,    24,    25,    26,     0,     0,    71,    72,    73,    74,
    75,     0,     0,     0,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,     0,    40,    40,
     0,     0,     0,    41,     0,     2,     3,    42,    42,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,     0,     0,
    23,    24,    25,    26,     0,     0,     0,    71,    72,    73,
    74,    75,     0,     0,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,     0,     0,    40,
     0,     0,     0,    41,     0,     2,     3,     0,    42,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,     0,     0,
    23,    24,    25,    26,   102,   103,   104,   105,     0,     0,
     0,     0,     0,     0,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,     0,     0,    40,
     0,     0,     0,    41,     0,     0,     0,     0,    42,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,     0,     0,    23,
     0,     0,    26,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,    28,    29,    30,    31,     0,    33,
    34,    35,    36,    37,    38,    39,    95,    96,    97,     0,
     0,     0,    41,     0,    98,    99,     0,    42,    81,    82,
    83,    84,    85,     0,    86,     0,     0,    87,    88,     0,
     0,     0,   100,   101,   102,   103,   104,   105,     0,     0,
     0,   106,   107,   108,     0,     0,     0,     0,    89,    90
};

static const short yycheck[] = {     1,
    32,    47,    44,     8,    77,     1,    45,     5,    79,     7,
     1,     9,    35,    45,    35,    47,    47,    48,     0,    65,
    66,    60,     0,    65,    66,    35,    58,    72,    60,    35,
    32,    69,    70,    65,    66,    66,    67,    68,    69,    70,
    41,    70,    44,    45,    72,    47,    78,    79,    44,    45,
   121,    35,    73,    44,    45,    44,    58,    65,    66,    60,
    35,    35,     4,    65,    66,    70,    74,    75,    76,    65,
    66,     4,    62,    63,    65,    66,    78,    79,   120,    72,
    62,    63,   121,     3,    62,    63,    62,    63,   120,   121,
    72,    62,    63,    94,    72,    77,    72,    98,    99,   100,
   101,   102,   103,   104,   105,    62,    63,     3,   109,   110,
    62,    63,    35,   159,   160,    47,    48,    78,   120,   121,
    72,    47,    48,     0,   120,   121,     0,   159,   160,   120,
   121,    47,    48,    65,    66,    67,    68,    69,    70,    65,
    66,    67,    68,    69,    70,     0,    78,    79,     1,   135,
   135,    67,    68,    69,    70,    10,    11,   159,   160,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    91,    35,
    35,    36,    37,    38,    -1,    -1,    42,    43,    44,    45,
    46,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    -1,    64,    64,
    -1,    -1,    -1,    68,    -1,    10,    11,    73,    73,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    36,    37,    38,    -1,    -1,    -1,    42,    43,    44,
    45,    46,    -1,    -1,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    -1,    -1,    64,
    -1,    -1,    -1,    68,    -1,    10,    11,    -1,    73,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    36,    37,    38,    67,    68,    69,    70,    -1,    -1,
    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    -1,    -1,    64,
    -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,    73,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    -1,    -1,    35,
    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    49,    50,    51,    52,    53,    -1,    55,
    56,    57,    58,    59,    60,    61,    39,    40,    41,    -1,
    -1,    -1,    68,    -1,    47,    48,    -1,    73,     3,     4,
     5,     6,     7,    -1,     9,    -1,    -1,    12,    13,    -1,
    -1,    -1,    65,    66,    67,    68,    69,    70,    -1,    -1,
    -1,    74,    75,    76,    -1,    -1,    -1,    -1,    33,    34
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 134 "../libpcap/grammar.y"
{
	finish_parse(yyvsp[0].blk.b);
;
    break;}
case 3:
#line 139 "../libpcap/grammar.y"
{ yyval.blk.q = qerr; ;
    break;}
case 5:
#line 142 "../libpcap/grammar.y"
{ gen_and(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 6:
#line 143 "../libpcap/grammar.y"
{ gen_and(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 7:
#line 144 "../libpcap/grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 8:
#line 145 "../libpcap/grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 9:
#line 147 "../libpcap/grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 10:
#line 149 "../libpcap/grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 12:
#line 152 "../libpcap/grammar.y"
{ yyval.blk.b = gen_ncode(NULL, (bpf_u_int32)yyvsp[0].i,
						   yyval.blk.q = yyvsp[-1].blk.q); ;
    break;}
case 13:
#line 154 "../libpcap/grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 14:
#line 156 "../libpcap/grammar.y"
{ yyval.blk.b = gen_scode(yyvsp[0].s, yyval.blk.q = yyvsp[-1].blk.q); ;
    break;}
case 15:
#line 157 "../libpcap/grammar.y"
{ yyval.blk.b = gen_mcode(yyvsp[-2].s, NULL, yyvsp[0].i,
				    yyval.blk.q = yyvsp[-3].blk.q); ;
    break;}
case 16:
#line 159 "../libpcap/grammar.y"
{ yyval.blk.b = gen_mcode(yyvsp[-2].s, yyvsp[0].s, 0,
				    yyval.blk.q = yyvsp[-3].blk.q); ;
    break;}
case 17:
#line 161 "../libpcap/grammar.y"
{
				  /* Decide how to parse HID based on proto */
				  yyval.blk.q = yyvsp[-1].blk.q;
				  yyval.blk.b = gen_ncode(yyvsp[0].s, 0, yyval.blk.q);
				;
    break;}
case 18:
#line 166 "../libpcap/grammar.y"
{
#ifdef INET6
				  yyval.blk.b = gen_mcode6(yyvsp[-2].s, NULL, yyvsp[0].i,
				    yyval.blk.q = yyvsp[-3].blk.q);
#else
				  bpf_error("'ip6addr/prefixlen' not supported "
					"in this configuration");
#endif /*INET6*/
				;
    break;}
case 19:
#line 175 "../libpcap/grammar.y"
{
#ifdef INET6
				  yyval.blk.b = gen_mcode6(yyvsp[0].s, 0, 128,
				    yyval.blk.q = yyvsp[-1].blk.q);
#else
				  bpf_error("'ip6addr' not supported "
					"in this configuration");
#endif /*INET6*/
				;
    break;}
case 20:
#line 184 "../libpcap/grammar.y"
{ 
				  yyval.blk.b = gen_ecode(yyvsp[0].e, yyval.blk.q = yyvsp[-1].blk.q);
				  /*
				   * $1 was allocated by "pcap_ether_aton()",
				   * so we must free it now that we're done
				   * with it.
				   */
				  free(yyvsp[0].e);
				;
    break;}
case 21:
#line 193 "../libpcap/grammar.y"
{
				  yyval.blk.b = gen_acode(yyvsp[0].e, yyval.blk.q = yyvsp[-1].blk.q);
				  /*
				   * $1 was allocated by "pcap_ether_aton()",
				   * so we must free it now that we're done
				   * with it.
				   */
				  free(yyvsp[0].e);
				;
    break;}
case 22:
#line 202 "../libpcap/grammar.y"
{ gen_not(yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 23:
#line 204 "../libpcap/grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 24:
#line 206 "../libpcap/grammar.y"
{ yyval.blk = yyvsp[-1].blk; ;
    break;}
case 26:
#line 209 "../libpcap/grammar.y"
{ gen_and(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 27:
#line 210 "../libpcap/grammar.y"
{ gen_or(yyvsp[-2].blk.b, yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 28:
#line 212 "../libpcap/grammar.y"
{ yyval.blk.b = gen_ncode(NULL, (bpf_u_int32)yyvsp[0].i,
						   yyval.blk.q = yyvsp[-1].blk.q); ;
    break;}
case 31:
#line 217 "../libpcap/grammar.y"
{ gen_not(yyvsp[0].blk.b); yyval.blk = yyvsp[0].blk; ;
    break;}
case 32:
#line 219 "../libpcap/grammar.y"
{ QSET(yyval.blk.q, yyvsp[-2].i, yyvsp[-1].i, yyvsp[0].i); ;
    break;}
case 33:
#line 220 "../libpcap/grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, yyvsp[0].i, Q_DEFAULT); ;
    break;}
case 34:
#line 221 "../libpcap/grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, yyvsp[0].i); ;
    break;}
case 35:
#line 222 "../libpcap/grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, Q_PROTO); ;
    break;}
case 36:
#line 223 "../libpcap/grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, Q_PROTOCHAIN); ;
    break;}
case 37:
#line 224 "../libpcap/grammar.y"
{ QSET(yyval.blk.q, yyvsp[-1].i, Q_DEFAULT, yyvsp[0].i); ;
    break;}
case 38:
#line 226 "../libpcap/grammar.y"
{ yyval.blk = yyvsp[0].blk; ;
    break;}
case 39:
#line 227 "../libpcap/grammar.y"
{ yyval.blk.b = yyvsp[-1].blk.b; yyval.blk.q = yyvsp[-2].blk.q; ;
    break;}
case 40:
#line 228 "../libpcap/grammar.y"
{ yyval.blk.b = gen_proto_abbrev(yyvsp[0].i); yyval.blk.q = qerr; ;
    break;}
case 41:
#line 229 "../libpcap/grammar.y"
{ yyval.blk.b = gen_relation(yyvsp[-1].i, yyvsp[-2].a, yyvsp[0].a, 0);
				  yyval.blk.q = qerr; ;
    break;}
case 42:
#line 231 "../libpcap/grammar.y"
{ yyval.blk.b = gen_relation(yyvsp[-1].i, yyvsp[-2].a, yyvsp[0].a, 1);
				  yyval.blk.q = qerr; ;
    break;}
case 43:
#line 233 "../libpcap/grammar.y"
{ yyval.blk.b = yyvsp[0].rblk; yyval.blk.q = qerr; ;
    break;}
case 45:
#line 237 "../libpcap/grammar.y"
{ yyval.i = Q_DEFAULT; ;
    break;}
case 46:
#line 240 "../libpcap/grammar.y"
{ yyval.i = Q_SRC; ;
    break;}
case 47:
#line 241 "../libpcap/grammar.y"
{ yyval.i = Q_DST; ;
    break;}
case 48:
#line 242 "../libpcap/grammar.y"
{ yyval.i = Q_OR; ;
    break;}
case 49:
#line 243 "../libpcap/grammar.y"
{ yyval.i = Q_OR; ;
    break;}
case 50:
#line 244 "../libpcap/grammar.y"
{ yyval.i = Q_AND; ;
    break;}
case 51:
#line 245 "../libpcap/grammar.y"
{ yyval.i = Q_AND; ;
    break;}
case 52:
#line 248 "../libpcap/grammar.y"
{ yyval.i = Q_HOST; ;
    break;}
case 53:
#line 249 "../libpcap/grammar.y"
{ yyval.i = Q_NET; ;
    break;}
case 54:
#line 250 "../libpcap/grammar.y"
{ yyval.i = Q_PORT; ;
    break;}
case 55:
#line 253 "../libpcap/grammar.y"
{ yyval.i = Q_GATEWAY; ;
    break;}
case 56:
#line 255 "../libpcap/grammar.y"
{ yyval.i = Q_LINK; ;
    break;}
case 57:
#line 256 "../libpcap/grammar.y"
{ yyval.i = Q_IP; ;
    break;}
case 58:
#line 257 "../libpcap/grammar.y"
{ yyval.i = Q_ARP; ;
    break;}
case 59:
#line 258 "../libpcap/grammar.y"
{ yyval.i = Q_RARP; ;
    break;}
case 60:
#line 259 "../libpcap/grammar.y"
{ yyval.i = Q_SCTP; ;
    break;}
case 61:
#line 260 "../libpcap/grammar.y"
{ yyval.i = Q_TCP; ;
    break;}
case 62:
#line 261 "../libpcap/grammar.y"
{ yyval.i = Q_UDP; ;
    break;}
case 63:
#line 262 "../libpcap/grammar.y"
{ yyval.i = Q_ICMP; ;
    break;}
case 64:
#line 263 "../libpcap/grammar.y"
{ yyval.i = Q_IGMP; ;
    break;}
case 65:
#line 264 "../libpcap/grammar.y"
{ yyval.i = Q_IGRP; ;
    break;}
case 66:
#line 265 "../libpcap/grammar.y"
{ yyval.i = Q_PIM; ;
    break;}
case 67:
#line 266 "../libpcap/grammar.y"
{ yyval.i = Q_VRRP; ;
    break;}
case 68:
#line 267 "../libpcap/grammar.y"
{ yyval.i = Q_ATALK; ;
    break;}
case 69:
#line 268 "../libpcap/grammar.y"
{ yyval.i = Q_AARP; ;
    break;}
case 70:
#line 269 "../libpcap/grammar.y"
{ yyval.i = Q_DECNET; ;
    break;}
case 71:
#line 270 "../libpcap/grammar.y"
{ yyval.i = Q_LAT; ;
    break;}
case 72:
#line 271 "../libpcap/grammar.y"
{ yyval.i = Q_SCA; ;
    break;}
case 73:
#line 272 "../libpcap/grammar.y"
{ yyval.i = Q_MOPDL; ;
    break;}
case 74:
#line 273 "../libpcap/grammar.y"
{ yyval.i = Q_MOPRC; ;
    break;}
case 75:
#line 274 "../libpcap/grammar.y"
{ yyval.i = Q_IPV6; ;
    break;}
case 76:
#line 275 "../libpcap/grammar.y"
{ yyval.i = Q_ICMPV6; ;
    break;}
case 77:
#line 276 "../libpcap/grammar.y"
{ yyval.i = Q_AH; ;
    break;}
case 78:
#line 277 "../libpcap/grammar.y"
{ yyval.i = Q_ESP; ;
    break;}
case 79:
#line 278 "../libpcap/grammar.y"
{ yyval.i = Q_ISO; ;
    break;}
case 80:
#line 279 "../libpcap/grammar.y"
{ yyval.i = Q_ESIS; ;
    break;}
case 81:
#line 280 "../libpcap/grammar.y"
{ yyval.i = Q_ISIS; ;
    break;}
case 82:
#line 281 "../libpcap/grammar.y"
{ yyval.i = Q_CLNP; ;
    break;}
case 83:
#line 282 "../libpcap/grammar.y"
{ yyval.i = Q_STP; ;
    break;}
case 84:
#line 283 "../libpcap/grammar.y"
{ yyval.i = Q_IPX; ;
    break;}
case 85:
#line 284 "../libpcap/grammar.y"
{ yyval.i = Q_NETBEUI; ;
    break;}
case 86:
#line 286 "../libpcap/grammar.y"
{ yyval.rblk = gen_broadcast(yyvsp[-1].i); ;
    break;}
case 87:
#line 287 "../libpcap/grammar.y"
{ yyval.rblk = gen_multicast(yyvsp[-1].i); ;
    break;}
case 88:
#line 288 "../libpcap/grammar.y"
{ yyval.rblk = gen_less(yyvsp[0].i); ;
    break;}
case 89:
#line 289 "../libpcap/grammar.y"
{ yyval.rblk = gen_greater(yyvsp[0].i); ;
    break;}
case 90:
#line 290 "../libpcap/grammar.y"
{ yyval.rblk = gen_byteop(yyvsp[-1].i, yyvsp[-2].i, yyvsp[0].i); ;
    break;}
case 91:
#line 291 "../libpcap/grammar.y"
{ yyval.rblk = gen_inbound(0); ;
    break;}
case 92:
#line 292 "../libpcap/grammar.y"
{ yyval.rblk = gen_inbound(1); ;
    break;}
case 93:
#line 293 "../libpcap/grammar.y"
{ yyval.rblk = gen_vlan(yyvsp[0].i); ;
    break;}
case 94:
#line 294 "../libpcap/grammar.y"
{ yyval.rblk = gen_vlan(-1); ;
    break;}
case 95:
#line 296 "../libpcap/grammar.y"
{ yyval.i = BPF_JGT; ;
    break;}
case 96:
#line 297 "../libpcap/grammar.y"
{ yyval.i = BPF_JGE; ;
    break;}
case 97:
#line 298 "../libpcap/grammar.y"
{ yyval.i = BPF_JEQ; ;
    break;}
case 98:
#line 300 "../libpcap/grammar.y"
{ yyval.i = BPF_JGT; ;
    break;}
case 99:
#line 301 "../libpcap/grammar.y"
{ yyval.i = BPF_JGE; ;
    break;}
case 100:
#line 302 "../libpcap/grammar.y"
{ yyval.i = BPF_JEQ; ;
    break;}
case 101:
#line 304 "../libpcap/grammar.y"
{ yyval.a = gen_loadi(yyvsp[0].i); ;
    break;}
case 103:
#line 307 "../libpcap/grammar.y"
{ yyval.a = gen_load(yyvsp[-3].i, yyvsp[-1].a, 1); ;
    break;}
case 104:
#line 308 "../libpcap/grammar.y"
{ yyval.a = gen_load(yyvsp[-5].i, yyvsp[-3].a, yyvsp[-1].i); ;
    break;}
case 105:
#line 309 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_ADD, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 106:
#line 310 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_SUB, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 107:
#line 311 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_MUL, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 108:
#line 312 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_DIV, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 109:
#line 313 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_AND, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 110:
#line 314 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_OR, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 111:
#line 315 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_LSH, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 112:
#line 316 "../libpcap/grammar.y"
{ yyval.a = gen_arth(BPF_RSH, yyvsp[-2].a, yyvsp[0].a); ;
    break;}
case 113:
#line 317 "../libpcap/grammar.y"
{ yyval.a = gen_neg(yyvsp[0].a); ;
    break;}
case 114:
#line 318 "../libpcap/grammar.y"
{ yyval.a = yyvsp[-1].a; ;
    break;}
case 115:
#line 319 "../libpcap/grammar.y"
{ yyval.a = gen_loadlen(); ;
    break;}
case 116:
#line 321 "../libpcap/grammar.y"
{ yyval.i = '&'; ;
    break;}
case 117:
#line 322 "../libpcap/grammar.y"
{ yyval.i = '|'; ;
    break;}
case 118:
#line 323 "../libpcap/grammar.y"
{ yyval.i = '<'; ;
    break;}
case 119:
#line 324 "../libpcap/grammar.y"
{ yyval.i = '>'; ;
    break;}
case 120:
#line 325 "../libpcap/grammar.y"
{ yyval.i = '='; ;
    break;}
case 122:
#line 328 "../libpcap/grammar.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 330 "../libpcap/grammar.y"


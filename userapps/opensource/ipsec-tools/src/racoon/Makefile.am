# $Id: Makefile.am,v 1.17.2.1 2005/02/18 04:56:49 ludvigm Exp $

sbin_PROGRAMS = racoon racoonctl plainrsa-gen
noinst_PROGRAMS = eaytest

adminsockdir=${localstatedir}/racoon

INCLUDES = -I${srcdir}/../libipsec 
AM_CFLAGS = @GLIBC_BUGS@ -DSYSCONFDIR=\"${sysconfdir}\" \
	-DADMINPORTDIR=\"${adminsockdir}\"
AM_LDFLAGS = -lcrypto

MISSING_ALGOS = \
	missing/crypto/sha2/sha2.c \
	missing/crypto/rijndael/rijndael-api-fst.c \
	missing/crypto/rijndael/rijndael-alg-fst.c

racoon_SOURCES = \
	main.c session.c isakmp.c handler.c \
	isakmp_ident.c isakmp_agg.c isakmp_base.c \
	isakmp_quick.c isakmp_inf.c isakmp_newg.c \
	gssapi.c dnssec.c getcertsbyname.c \
	pfkey.c admin.c evt.c ipsec_doi.c oakley.c grabmyaddr.c vendorid.c \
	policy.c localconf.c remoteconf.c crypto_openssl.c algorithm.c \
	proposal.c sainfo.c strnames.c \
	vmbuf.c plog.c logger.c schedule.c str2val.c misc.c sockmisc.c \
	safefile.c backupsa.c genlist.c rsalist.c \
	cftoken.l cfparse.y prsa_tok.l prsa_par.y 
EXTRA_racoon_SOURCES = isakmp_xauth.c isakmp_cfg.c isakmp_unity.c throttle.c \
	isakmp_frag.c nattraversal.c $(MISSING_ALGOS)
racoon_LDFLAGS = ../libipsec/libipsec.la
racoon_LDADD = $(CRYPTOBJS) $(HYBRID_OBJS) $(NATT_OBJS) $(FRAG_OBJS) $(LEXLIB)
racoon_DEPENDENCIES = ../libipsec/libipsec.la \
	$(CRYPTOBJS) $(HYBRID_OBJS) $(NATT_OBJS) $(FRAG_OBJS)

racoonctl_SOURCES = kmpstat.c vmbuf.c str2val.c sockmisc.c
racoonctl_LDFLAGS = ../libipsec/libipsec.la
racoonctl_LDADD = misc_noplog.o
racoonctl_DEPENDENCIES = ../libipsec/libipsec.la

plainrsa_gen_SOURCES = plainrsa-gen.c plog.c vmbuf.c \
	crypto_openssl.c logger.c misc.c
EXTRA_plainrsa_gen_SOURCES = $(MISSING_ALGOS)
plainrsa_gen_LDADD = $(CRYPTOBJS)
plainrsa_gen_DEPENDENCIES = $(CRYPTOBJS)

eaytest_SOURCES = eaytest.c vmbuf.c str2val.c plog.c logger.c
EXTRA_eaytest_SOURCES = missing/crypto/sha2/sha2.c
eaytest_LDADD =	crypto_openssl_test.o misc_noplog.o $(CRYPTOBJS)
eaytest_DEPENDENCIES = crypto_openssl_test.o misc_noplog.o $(CRYPTOBJS)

noinst_HEADERS = \
	admin.h           dnssec.h      isakmp_base.h   oakley.h      session.h \
	admin_var.h       dump.h        isakmp_ident.h  pfkey.h       sockmisc.h \
	algorithm.h       gcmalloc.h    isakmp_inf.h    plog.h        str2val.h \
	backupsa.h        gnuc.h        isakmp_newg.h   policy.h      strnames.h \
	grabmyaddr.h      isakmp_quick.h  proposal.h    var.h         evt.h \
	gssapi.h          isakmp_var.h	vendorid.h      nattraversal.h\
	crypto_openssl.h  handler.h     localconf.h     remoteconf.h  vmbuf.h \
	debug.h           ipsec_doi.h   logger.h        safefile.h \
	debugrm.h         isakmp.h      misc.h          sainfo.h \
	dhgroup.h         isakmp_agg.h  netdb_dnssec.h  schedule.h \
	isakmp_cfg.h      isakmp_xauth.h isakmp_unity.h isakmp_frag.h \
	throttle.h \
	cfparse_proto.h	  cftoken_proto.h genlist.h     rsalist.h \
	missing/crypto/sha2/sha2.h missing/crypto/rijndael/rijndael_local.h \
	missing/crypto/rijndael/rijndael-api-fst.h \
	missing/crypto/rijndael/rijndael-alg-fst.h \
	missing/crypto/rijndael/rijndael.h

man5_MANS = racoon.conf.5
man8_MANS = racoon.8 racoonctl.8 plainrsa-gen.8

EXTRA_DIST = \
   ${man5_MANS} ${man8_MANS} \
   missing/crypto/rijndael/boxes-fst.dat \
   doc/FAQ doc/README.certificate doc/README.gssapi \
   contrib/sp.pl stats.pl \
   samples/psk.txt.sample  samples/racoon.conf.sample \
   samples/psk.txt.in samples/racoon.conf.in \
   samples/racoon.conf.sample-gssapi samples/racoon.conf.sample-natt \
   samples/racoon.conf.sample-inherit samples/racoon.conf.sample-plainrsa \
   samples/roadwarrior/README \
   samples/roadwarrior/client/phase1-down.sh \
   samples/roadwarrior/client/phase1-up.sh \
   samples/roadwarrior/client/racoon.conf \
   samples/roadwarrior/server/phase1-down.sh \
   samples/roadwarrior/server/racoon.conf \
   samples/roadwarrior/server/racoon.conf-radius

DISTCLEANFILES = cfparse.c cftoken.c prsa_tok.c prsa_par.c cfparse.h prsa_par.h

TESTS = eaytest

install-exec-local:
	${mkinstalldirs} $(DESTDIR)${adminsockdir}

# Config file parser
cftoken.o: cftoken.c cfparse.h
	$(COMPILE) -c -o $@ $<

cfparse.o: cfparse.c
	$(COMPILE) -c -o $@ $<

cftoken.c: cftoken.l
	$(LEX) -ocftoken.c $(srcdir)/cftoken.l

cfparse.h cfparse.c: $(srcdir)/cfparse.y
	$(YACC) -d $(srcdir)/cfparse.y && \
	mv y.tab.c cfparse.c && \
	mv y.tab.h cfparse.h

# Plain-RSA parser
prsa_tok.o: prsa_tok.c prsa_par.h
	$(COMPILE) -c -o $@ $<

prsa_par.o: prsa_par.c
	$(COMPILE) -c -o $@ $<

prsa_tok.c: $(srcdir)/prsa_tok.l
	$(LEX) -Pprsa -oprsa_tok.c $(srcdir)/prsa_tok.l

prsa_par.h prsa_par.c: $(srcdir)/prsa_par.y
	$(YACC) -pprsa -d $(srcdir)/prsa_par.y && \
	mv y.tab.c prsa_par.c && \
	mv y.tab.h prsa_par.h

# special object rules
crypto_openssl_test.o: crypto_openssl.c
	$(COMPILE) -DEAYDEBUG -o crypto_openssl_test.o -c $(srcdir)/crypto_openssl.c

misc_noplog.o: misc.c
	$(COMPILE) -DNOUSE_PLOG -o misc_noplog.o -c $(srcdir)/misc.c

# missing/*.c
strdup.o:	$(srcdir)/missing/strdup.c
	$(COMPILE) -c $(srcdir)/missing/$*.c
getaddrinfo.o:	$(srcdir)/missing/getaddrinfo.c
	$(COMPILE) -c $(srcdir)/missing/$*.c
getnameinfo.o:	$(srcdir)/missing/getnameinfo.c
	$(COMPILE) -c $(srcdir)/missing/$*.c
rijndael-api-fst.o: $(srcdir)/missing/crypto/rijndael/rijndael-api-fst.c
	$(COMPILE) -c $(srcdir)/missing/crypto/rijndael/$*.c
rijndael-alg-fst.o: $(srcdir)/missing/crypto/rijndael/rijndael-alg-fst.c
	$(COMPILE) -c $(srcdir)/missing/crypto/rijndael/$*.c
sha2.o: $(srcdir)/missing/crypto/sha2/sha2.c
	$(COMPILE) -c $(srcdir)/missing/crypto/sha2/$*.c

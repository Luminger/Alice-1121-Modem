O_TARGET := sshd.o 

###-DDEBUG_TRACE -DDEBUG_RSA -DDEBUG_DSS
CFLAGS= -s -Os -fomit-frame-pointer -W -I.  -DDROPBEAR_SERVER -Ilibtomcrypt \
	-I$(INC_APP_PATH) -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
        -I. -Ilibtomcrypt/src/headers

COMMONOBJS=dbutil.o buffer.o \
		dss.o bignum.o \
		signkey.o rsa.o random.o \
		queue.o \
		atomicio.o compat.o  fake-rfc2553.o

SVROBJS=svr-kex.o svr-algo.o svr-auth.o sshpty.o \
		svr-authpasswd.o svr-authpubkey.o svr-session.o svr-service.o \
		svr-chansession.o svr-runopts.o svr-agentfwd.o svr-main.o svr-x11fwd.o\
		svr-tcpfwd.o svr-authpam.o \

CLIOBJS=cli-algo.o cli-main.o cli-auth.o cli-authpasswd.o cli-kex.o \
		cli-session.o cli-service.o cli-runopts.o cli-chansession.o \
		cli-authpubkey.o cli-tcpfwd.o cli-channel.o

CLISVROBJS=common-session.o packet.o common-algo.o common-kex.o \
			common-channel.o common-chansession.o termcodes.o loginrec.o \
			tcp-accept.o listener.o process-packet.o \
			common-runopts.o circbuffer.o
	
obj-y := $(COMMONOBJS) $(SVROBJS) $(CLISVROBJS) \
		$(join $(subdir-y), $(patsubst %,/%.o,$(notdir $(subdir-y))))

include $(TOPDIR)/Rules.make



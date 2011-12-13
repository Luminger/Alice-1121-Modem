#
# build file 
#
#
CFLAGS = -s -Os -fomit-frame-pointer
LDFLAGS = -Wl,-allow-shlib-undefined
ifeq ($(strip $(BUILD_TOD)), static)
CFLAGS += -DBUILD_STATIC 
endif

LIBS=-L$(OPENSOURCE_DIR)/iptables/libipq/ -lipq
CFLAGS+=-Wall

OBJS = filter.o

all: UrlFilter

install::
	install -m 755 UrlFilter $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/UrlFilter

dynamic: all install

static: UrlFilter.a

.c.o:
	$(CC) $(CFLAGS) -c $<

UrlFilter: filter.o
	$(CC) -o UrlFilter filter.o $(LDFLAGS) $(LIBS)

UrlFilter.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	-rm -f UrlFilter *.o  UrlFilter.a

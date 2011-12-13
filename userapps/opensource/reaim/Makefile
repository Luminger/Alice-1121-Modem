#
# Simple build file for now
#
# License:  GNU General Public License, Version 2.
#
CFLAGS= -Os -fomit-frame-pointer
LDFLAGS= -Wall -Wl,-allow-shlib-undefined
ifeq ($(strip $(BUILD_REAIM)), static)
CFLAGS += -DBUILD_STATIC
endif

ifeq ($(strip $(BUILD_REAIM)), debug)
CFLAGS += -g -static
else
CFLAGS += -s
endif

all: reaim

ifdef NetBSD
FW_FLAGS="-DUSE_IPF"
endif

install:
	install -m 755 reaim $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/reaim

dynamic: reaim install

debug: reaim install

static: reaim.a

reaim: reaim.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o reaim reaim.c $(FW_FLAGS)

reaim.a: reaim.c
	$(CC) $(CFLAGS) -c -o reaim.o reaim.c $(FW_FLAGS)
	$(AR) rcs reaim.a  reaim.o

clean:
	-rm -f reaim reaim.o reaim.a

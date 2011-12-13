# Makefile file for bftpd
#
# License:  GNU General Public License, Version 2.
#
CFLAGS= -Wall -s -Os -fomit-frame-pointer \
        -I. -I$(INC_BRCMCFM_PATH) \
        -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
        -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PRIV_PATH)/$(BRCM_BOARD) \
        -I$(BROADCOM_CFM_DIR)/inc -I$(BROADCOM_CFM_DIR)/util/psi
        
LDFLAGS=-Wl,-allow-shlib-undefined
ifeq ($(strip $(BUILD_FTPD)), static)
CFLAGS += -DBUILD_STATIC
endif

CFLAGS += ${BRCM_DRIVER_ATM_BONDING_DEFINES}

all: bftpd

LIBS= -lcrypt

ifeq ($(strip $(BUILD_FTPD_STORAGE)), y)
    HEADERS=list.h storage/commands.h storage/login.h storage/main.h storage/mystring.h storage/fwsyscall.h
    OBJS=list.o cwd.o options.o storage/commands.o storage/login.o storage/main.o storage/mystring.o storage/fwsyscall.o storage/dirlist.o
    SRCS=list.c cwd.c options.c storage/commands.c storage/login.c storage/main.c storage/mystring.c storage/fwsyscall.c storage/dirlist.c
else
    HEADERS=commands.h list.h login.h main.h mystring.h fwsyscall.h
    OBJS=commands.o list.o login.o main.o mystring.o fwsyscall.o
    SRCS=commands.c list.c login.c main.c mystring.c fwsyscall.c
endif

BOARD_API_SRC=$(BROADCOM_CFM_DIR)/util/psi/board_api.c
BOARD_API_OBJ=board_api.o
SYSCALL_SRC=$(BROADCOM_CFM_DIR)/util/system/syscall.c
SYSCALL_OBJ=syscall.o
SYSKILL_SRC=$(BROADCOM_CFM_DIR)/util/system/syskill.c
SYSKILL_OBJ=syskill.o

install:
	install -m 755 bftpd $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/bftpd

dynamic: bftpd install

static: bftpd.a

$(BOARD_API_OBJ):	$(INC_BRCMCFM_PATH)/board_api.h
	$(CC) $(CFLAGS) -c $(BOARD_API_SRC)

$(SYSCALL_OBJ):	$(INC_BRCMCFM_PATH)/syscall.h 
	$(CC) $(CFLAGS) -I$(INC_KERNEL_PATH) -I$(INC_KERNEL_PATH2) -c $(SYSCALL_SRC)
$(SYSKILL_OBJ):	$(INC_BRCMCFM_PATH)/syscall.h 
	$(CC) $(CFLAGS) -I$(INC_KERNEL_PATH) -I$(INC_KERNEL_PATH2) -c $(SYSKILL_SRC)
		
bftpd: $(HEADERS) $(OBJS) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(SYSKILL_OBJ)
	$(CC) $(LDFLAGS) $(OBJS) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(SYSKILL_OBJ) $(LIBS) -o bftpd

bftpd.a: $(HEADERS) $(OBJS) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(SYSKILL_OBJ)
	$(CC) $(CFLAGS) -c -o $(OBJS)
	$(AR) rcs bftpd.a $(OBJS) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(SYSKILL_OBJ) $(LIBS)

clean:
	-rm -f bftpd $(OBJS) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(SYSKILL_OBJ) bftpd.a



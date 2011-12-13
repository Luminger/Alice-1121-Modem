ifeq ($(strip $(BRCM_CHIP)), 6368)
PROGRAM_NAME:=xtmctl
PROGRAM_NAME_ALT:=xtm
PROGRAM_NAME_ALT2:=xtm
PROGRAM_FILES=xtmctl.o xtmcfguser.o
else
PROGRAM_NAME:=atmctl
PROGRAM_NAME_ALT:=atm
PROGRAM_NAME_ALT2:=xtmctl
PROGRAM_FILES=atmctl.o atmapiuser.o atmbuser.o
endif

all:
	cp $(PROGRAM_NAME)_$(PROFILE)_save $(PROGRAM_NAME)

install:
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	ln -sf $(PROGRAM_NAME) $(INSTALL_DIR)/bin/$(PROGRAM_NAME_ALT)
	ln -sf $(PROGRAM_NAME) $(INSTALL_DIR)/bin/$(PROGRAM_NAME_ALT2)

dynamic: all install

clean:
	rm -f $(PROGRAM_NAME)


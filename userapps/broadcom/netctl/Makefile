all:
	cp netctl_$(PROFILE)_save netctl

PROGRAM_NAME := netctl

install:
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)

dynamic: all install

clean:
	rm -f netctl


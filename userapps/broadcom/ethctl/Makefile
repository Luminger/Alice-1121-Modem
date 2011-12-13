all:
	cp ethctl_$(PROFILE)_save ethctl

PROGRAM_NAME:= ethctl

install::
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	mkdir -p $(INSTALL_DIR)/sbin
	ln -sf ../bin/$(PROGRAM_NAME) $(INSTALL_DIR)/sbin/ethctl

dynamic: all install

clean:
	rm -f ethctl


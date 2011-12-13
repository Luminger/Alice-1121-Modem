all:
	cp adslctl_$(PROFILE)_save adslctl

PROGRAM_NAME:=adslctl

install:
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	ln -sf $(PROGRAM_NAME) $(INSTALL_DIR)/bin/adsl

dynamic: all install

clean:
	rm -f adslctl


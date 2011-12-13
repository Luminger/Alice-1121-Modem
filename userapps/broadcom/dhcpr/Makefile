all:
	cp dhcpr_$(PROFILE)_save dhcpr

install::
	install -m 755 dhcpr $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/dhcpr

dynamic: all install

clean:
	rm -f dhcpr


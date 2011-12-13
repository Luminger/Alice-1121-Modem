all:
	cp dnsprobe_$(PROFILE)_save dnsprobe

install::
	install -m 755 dnsprobe $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/dnsprobe

dynamic: all install

clean:
	rm -f dnsprobe


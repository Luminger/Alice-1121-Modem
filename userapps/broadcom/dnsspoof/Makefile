all:
	cp dnsspoof_$(PROFILE)_save dnsspoof

install::
	install -m 755 dnsspoof $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/dnsspoof

dynamic: all install

clean:
	rm -f dnsspoof


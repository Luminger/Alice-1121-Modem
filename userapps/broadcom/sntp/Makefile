all:
	cp sntp_$(PROFILE)_save sntp

install:
	install -m 755 sntp $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/sntp

dynamic: all install

clean:
	rm -f sntp


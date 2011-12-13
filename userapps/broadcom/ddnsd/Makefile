all:
	cp ddnsd_$(PROFILE)_save ddnsd

install: $(PROGRAMS)
	mkdir -p $(INSTALL_DIR)/usr/bin
	install -m 755 ddnsd $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/ddnsd

dynamic: all install

clean:
	rm -f ddnsd


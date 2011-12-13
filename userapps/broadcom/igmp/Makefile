all:
	cp igmp_$(PROFILE)_save igmp

install::
	install -m 755 igmp $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/igmp

dynamic: all install

clean:
	rm -f igmp


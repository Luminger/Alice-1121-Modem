all:
	cp wsccmd_$(PROFILE)_save wsccmd

PROGRAM_NAME:= wsccmd

install:
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	install -m 444 wsc_config_1a_ap.txt  $(INSTALL_DIR)/etc

dynamic: all install

clean:
	rm -f wsccmd


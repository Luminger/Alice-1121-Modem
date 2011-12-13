all:
	cp wlctl_$(PROFILE)_save wlctl

	if [ -f libwlctl.so_${PROFILE}_save ]; then \
		cp libwlctl.so_${PROFILE}_save libwlctl.so ;\
	fi;

PROGRAM_NAME:= wlctl

install:
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	cp -av lib$(PROGRAM_NAME).so $(INSTALL_DIR)/lib
	mkdir -p $(INSTALL_DIR)/sbin
	ln -sf $(PROGRAM_NAME) $(INSTALL_DIR)/bin/wl

dynamic: all install

clean:
	rm -f wlctl


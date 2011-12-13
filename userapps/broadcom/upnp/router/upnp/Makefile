UPNP	:= $(BROADCOM_DIR)/upnp/igd/linux

all:
	cp $(UPNP)/upnp_$(PROFILE)_save $(UPNP)/upnp

install: all
	install -d $(INSTALL_DIR)/bin
	install -m 755 $(UPNP)/upnp $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/upnp

dynamic: all install

clean:
	rm -f $(UPNP)/upnp


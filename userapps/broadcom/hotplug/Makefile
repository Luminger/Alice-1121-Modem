all:
	cp hotplug_$(PROFILE)_save hotplug

PROGRAM_NAME:= hotplug

install::
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	mkdir -p $(INSTALL_DIR)/sbin	
	ln -sf ../bin/$(PROGRAM_NAME) $(INSTALL_DIR)/sbin/hotplug
	
dynamic: all install

dynamic: all install

clean:
	rm -f hotplug


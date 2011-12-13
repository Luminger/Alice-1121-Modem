all:
	cp nvram_$(PROFILE)_save nvram

	if [ -f libnvram.so_${PROFILE}_save ]; then \
		cp libnvram.so_${PROFILE}_save libnvram.so ;\
	fi;

PROGRAM_NAME:= nvram

install: $(PROGRAM_NAME)
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin	
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	cp -av lib$(PROGRAM_NAME).so $(INSTALL_DIR)/lib 

dynamic: all install

clean:
	rm -f nvram


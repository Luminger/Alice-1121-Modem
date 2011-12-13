all:
	cp lld2d_$(PROFILE)_save lld2d

PROGRAM_NAME:= lld2d

install:
	install -m 755 $(PROGRAM_NAME) $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/$(PROGRAM_NAME)
	install -m 444 ./wrt54g.large.ico $(INSTALL_DIR)/etc
	install -m 444 ./wrt54g.small.ico $(INSTALL_DIR)/etc

dynamic: all install

clean:
	rm -f lld2d


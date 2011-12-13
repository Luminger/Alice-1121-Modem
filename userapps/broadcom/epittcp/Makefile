all:
	cp epi_ttcp_$(PROFILE)_save epi_ttcp

install:
	install -m 755 epi_ttcp $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/epi_ttcp

dynamic: all install

clean:
	rm -f epi_ttcp


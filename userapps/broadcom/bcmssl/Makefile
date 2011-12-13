
all: lib install

lib:
	cp src/libbcmssl_$(PROFILE).so src/libbcmssl.so
	cp ctaocrypt/libbcmcrypto_$(PROFILE).so ctaocrypt/libbcmcrypto.so

install:
	install -m 755 src/libbcmssl.so $(INSTALL_DIR)/lib
	install -m 755 ctaocrypt/libbcmcrypto.so $(INSTALL_DIR)/lib
	$(STRIP) $(INSTALL_DIR)/lib/libbcmssl.so
	$(STRIP) $(INSTALL_DIR)/lib/libbcmcrypto.so

clean:
	-rm src/libbcmssl.so ctaocrypt/libbcmcrypto.so $(INSTALL_DIR)/lib/libbcmssl.so $(INSTALL_DIR)/lib/libbcmcrypto.so



obj-$(CONFIG_BCM_WLAN) += wl.o
wl-objs += wl_dep.o
$(obj)/wl_dep.o: $(obj)/shared/bcm4306_map.o $(obj)/shared/bcm4318_map.o $(obj)/shared/bcm4321_map.o $(obj)/shared/bcm4312_map.o
	cp $(obj)/wl$(PROFILE).o_save $(obj)/wl_dep.o

include $(BRCMDRIVERS_DIR)/broadcom/./net/wl/impl3/Makefile.srom



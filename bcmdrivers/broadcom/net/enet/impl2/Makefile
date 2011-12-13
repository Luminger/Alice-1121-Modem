obj-$(CONFIG_BCM_ENET) = bcm_enet.o
obj-$(CONFIG_BCM_ENET) += vnet.o

bcm_enet-objs += bcm_enet_dep.o

$(obj)/bcm_enet_dep.o:
	cp $(obj)/bcm_enet$(PROFILE).o_save $(obj)/bcm_enet_dep.o



vnet-objs += vnet_dep.o

$(obj)/vnet_dep.o:
	cp $(obj)/vnet$(PROFILE).o_save $(obj)/vnet_dep.o



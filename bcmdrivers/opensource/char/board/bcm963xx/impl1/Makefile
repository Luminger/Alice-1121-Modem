# Makefile for the bcm963xx board drivers
#


obj-y := board.o bcm63xx_flash.o bcm63xx_led.o

EXTRA_CFLAGS += -I. -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)

ifeq ($(strip $(WIRELESS)),1)
	EXTRA_CFLAGS += -DWIRELESS
endif

#dare sunmin
ifneq ($(strip $(BUILD_VCONFIG)),)
	EXTRA_CFLAGS += -DSUPPORT_PORT_MAP
endif

ifneq ($(strip $(BRCM_DRIVER_USB)),)
	EXTRA_CFLAGS += -DBRCM_DRIVER_USB
endif

ifneq ($(strip $(DARE_IMAGE_SIGNATURE)),)
	EXTRA_CFLAGS += -DDARE_IMAGE_SIGNATURE
endif

-include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s


#
# Makefile for the linux squashfs routines.
#

obj-$(CONFIG_SQUASHFS) += squashfs.o

ifeq ($(strip $(BUILD_SQUASH_HIGH)),y)
	EXTRA_CFLAGS += -DBUILD_SQUASH_HIGH
endif
ifeq ($(strip $(BUILD_SQUASH_NORMAL)),y)
	EXTRA_CFLAGS += -DBUILD_SQUASH_NORMAL
endif
ifeq ($(strip $(BUILD_SQUASH_LOW)),y)
	EXTRA_CFLAGS += -DBUILD_SQUASH_LOW
endif

#squashfs-objs := inode.o

# CONFIG_MIPS_BRCM Begin Broadcom changed code.

ifdef CONFIG_ZLIB_FS_INFLATE
squashfs-objs := inode.o
else
ifdef CONFIG_LZMA_FS_INFLATE
squashfs-objs := inode.o ../inflate_lzma/7zlzma.o ../inflate_lzma/LZMADecoder.o ../inflate_lzma/IInOutStreams.o
endif
endif

# CONFIG_MIPS_BRCM End Broadcom changed code.


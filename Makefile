#****************************************************************************
#
#  Copyright (c) 2001, 2002, 2003, 2004  Broadcom Corporation
#  All Rights Reserved
#  No portions of this material may be reproduced in any form without the
#  written permission of:
#          Broadcom Corporation
#          16251 Laguna Canyon Road
#          Irvine, California 92618
#  All information contained in this document is Broadcom Corporation
#  company private, proprietary, and trade secret.
#
#****************************************************************************

# Top-level Makefile for all commengine xDSL platforms

include version.make
ifneq ($(wildcard svnversion.make),)
include svnversion.make
endif

BRCM_RELEASETAG=$(BRCM_VERSION).$(BRCM_RELEASE)L.$(BRCM_EXTRAVERSION)

###########################################
#
# Define Basic Variables
#
###########################################
BUILD_DIR = $(shell pwd)
BL_BUILD_DIR=$(BUILD_DIR)/cfe/build/broadcom/bcm63xx_rom
KERNEL_VER = 2.6
ifeq ($(strip $(KERNEL_VER)),2.6)
INC_KERNEL_BASE = $(BUILD_DIR)/kernel
ORIGINAL_KERNEL = linuxmips.tar.bz2
endif
KERNEL_DIR = $(INC_KERNEL_BASE)/linux
BRCMDRIVERS_DIR = $(BUILD_DIR)/bcmdrivers
USERAPPS_DIR = $(BUILD_DIR)/userapps
LINUXDIR = $(INC_KERNEL_BASE)/linux
HOSTTOOLS_DIR = $(BUILD_DIR)/hostTools
IMAGES_DIR = $(BUILD_DIR)/images
TARGETS_DIR = $(BUILD_DIR)/targets
DEFAULTCFG_DIR = $(TARGETS_DIR)/defaultcfg
XCHANGE_DIR = $(BUILD_DIR)/xChange
FSSRC_DIR = $(TARGETS_DIR)/fs.src
CFE_FILE = $(TARGETS_DIR)/cfe/cfe$(BRCM_CHIP).bin
SHARED_DIR = $(BUILD_DIR)/shared
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)
GENDEFCONFIG_CMD = $(HOSTTOOLS_DIR)/scripts/gendefconfig
RUN_NOISE=0
6510_REF_CODE=$(BUILD_DIR)/userapps/broadcom/6510refCode

###########################################
#
# Import Build Profiles
#
###########################################
BRCM_BOARD=bcm963xx
LAST_PROFILE=$(shell find targets -name vmlinux | sed -e "s?targets/??" -e "s?/.*??" -e "q")
ifeq ($(strip $(PROFILE)),)
PROFILE=$(LAST_PROFILE)
export PROFILE
endif

ifneq ($(strip $(PROFILE)),)
include $(TARGETS_DIR)/$(PROFILE)/$(PROFILE)
export BRCM_CHIP
export BRCM_FLASHPSI_SIZE
export BRCM_DRIVER_WIRELESS_PCMCIA_DATASWAP BRCM_DRIVER_WIRELESS_EBI_DMA
export BRCM_DRIVER_USB BRCM_DRIVER_ETHERNET_CONFIG
export BRCM_DEFAULTCFG
export BRCM_CERT_FILE
export BRCM_KERNEL_NF_FIREWALL BRCM_KERNEL_NF_MANGLE BRCM_KERNEL_NF_NAT
export BRCM_KERNEL_NF_NAT_ALG_FTP BRCM_KERNEL_NF_NAT_ALG_SIP BRCM_KERNEL_NF_NAT_ALG_TFTP BRCM_KERNEL_NF_NAT_ALG_H323 BRCM_KERNEL_NF_NAT_ALG_H323_SIGNAL_PROXY
export BRCM_KERNEL_NF_NAT_ALG_IRC BRCM_KERNEL_NF_NAT_ALG_WM BRCM_KERNEL_NF_NAT_ALG_PT BRCM_KERNEL_NF_NAT_ALG_PPTP BRCM_KERNEL_NF_NAT_ALG_IPSEC
export BRCM_KERNEL_NF_NAT_ALG_RTSP BRCM_KERNEL_NF_NAT_ALG_SNMP BRCM_KERNEL_NF_NAT_ALG_TALK
endif

###########################################
#
# Define Toolchain
#
###########################################
ifeq ($(strip $(BRCM_UCLIBC)),y)
NTC=1
ifeq ($(strip $(NTC)),1)
ifndef TOOLCHAIN
TOOLCHAIN=/opt/toolchains/uclibc-crosstools_gcc-3.4.2_uclibc-20050502
endif
CROSS_COMPILE = $(TOOLCHAIN)/bin/mips-linux-uclibc-
else
TOOLCHAIN=/opt/toolchains/uclibc
CROSS_COMPILE = $(TOOLCHAIN)/bin/mips-uclibc-
endif
else
TOOLCHAIN=/usr/crossdev/mips
CROSS_COMPILE = $(TOOLCHAIN)/bin/mips-linux-
endif

AR              = $(CROSS_COMPILE)ar
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CXX             = $(CROSS_COMPILE)g++
CPP             = $(CROSS_COMPILE)cpp
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
SSTRIP          = $(CROSS_COMPILE)sstrip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
RANLIB          = $(CROSS_COMPILE)ranlib

LIB_PATH        = $(TOOLCHAIN)/mips-linux/lib
LIBDIR          = $(TOOLCHAIN)/mips-linux/lib
LIBCDIR         = $(TOOLCHAIN)/mips-linux

###########################################
#
# Application-specific settings
#
###########################################
TARGET_FS = $(TARGETS_DIR)/$(PROFILE)/fs
PROFILE_DIR = $(TARGETS_DIR)/$(PROFILE)
INSTALL_DIR=$(PROFILE_DIR)/fs.install
PROFILE_PATH = $(TARGETS_DIR)/$(PROFILE)/$(PROFILE)
VENDOR_NAME = bcm
FS_KERNEL_IMAGE_NAME = $(VENDOR_NAME)$(PROFILE)_fs_kernel
CFE_FS_KERNEL_IMAGE_NAME = $(VENDOR_NAME)$(PROFILE)_cfe_fs_kernel
FLASH_IMAGE_NAME = $(VENDOR_NAME)$(PROFILE)_flash_image_$(BRCM_BOARD_ID)
INC_BRCMDRIVER_PUB_PATH=$(BRCMDRIVERS_DIR)/opensource/include
INC_BRCMDRIVER_PRIV_PATH=$(BRCMDRIVERS_DIR)/broadcom/include
INC_ENDPOINT_PATH=$(BRCMDRIVERS_DIR)/broadcom/char/endpoint/bcm9$(BRCM_CHIP)/inc
INC_ADSLDRV_PATH=$(BRCMDRIVERS_DIR)/broadcom/char/adsl/impl1
INC_ATMAPI_DRV_PATH=$(BRCMDRIVERS_DIR)/broadcom/char/atmapi/impl1
INC_ATMBONDING_DRV_PATH=$(BRCMDRIVERS_DIR)/broadcom/char/atmbonding/impl1/common
INC_ATMBONDING_ETH_DRV_PATH=$(BRCMDRIVERS_DIR)/broadcom/atmbondingeth/impl1
BROADCOM_CFM_DIR=$(BROADCOM_DIR)/cfm
INC_BRCMCFM_PATH=$(BROADCOM_CFM_DIR)/inc
INC_BRCMSHARED_PUB_PATH=$(SHARED_DIR)/opensource/include
INC_BRCMSHARED_PRIV_PATH=$(SHARED_DIR)/broadcom/include
INC_BRCMBOARDPARMS_PATH=$(SHARED_DIR)/opensource/boardparms
INC_FLASH_PATH=$(SHARED_DIR)/opensource/flash

KERNEL_DEBUG=0

ifeq ($(strip $(BRCM_KERNEL_DEBUG)),y) 
KERNEL_DEBUG=1
endif

ifeq ($(strip $(BRCM_KERNEL_OPROFILE)),y) 
	export CONFIG_DEBUG_INFO=1
	export CONFIG_FRAME_POINTER=1
endif

ifeq ($(strip $(BRCM_APP_PHONE)),sip)
export VOXXXLOAD=1
export VOIPLOAD=1
export SIPLOAD=1

ifeq ($(strip $(BRCM_VODSL_CONFIG_MANAGER)),y)
	export BRCM_VODSL_CFGMGR=1
endif

BRCM_RELEASETAG := $(BRCM_RELEASETAG).sip
endif

ifeq ($(strip $(BRCM_APP_PHONE)),mgcp)
export VOXXXLOAD=1
export VOIPLOAD=1
export MGCPLOAD=1
export BRCM_VODSL_CFGMGR=0
BRCM_RELEASETAG := $(BRCM_RELEASETAG).mgcp
endif

ifeq ($(strip $(BRCM_PROFILER_ENABLED)),y)
export BRCM_PROFILER_TOOL=1
else
export BRCM_PROFILER_TOOL=0
endif

ifneq ($(strip $(BUILD_VODSL)),)
export VOXXXLOAD=1
endif

ifeq ($(strip $(BRCM_VODSL_STUNC)),y)
	export BRCM_VODSL_STUN_CLIENT=1
endif

ifeq ($(strip $(BRCM_VODSL_RANDOMP)),y)
	export BRCM_VODSL_RANDOM_PORT=1
endif


ifeq ($(strip $(BRCM_VODSL_VOICE_ANNOUNCEMENT)),y)
	export BRCM_VODSL_VOICE_ANNOUNCEMENT=1
endif

ifeq ($(strip $(BRCM_VODSL_PSTN_LIFE_LINE_SUPPORT)),y)
   export BRCM_VODSL_PSTN_LIFE_LINE_SUPPORT=1
   BRCM_VOICE_GLOBAL_CFLAGS = PSTN_LIFE_LINE_SUPPORT=1
endif

ifeq ($(strip $(BRCM_VODSL_MULTIPLE_SIP_ACCOUNTS)),y)
	export BRCM_VODSL_MULTI_SIP_ACCNT=1
endif

ifeq ($(strip $(BRCM_VODSL_CALL_FEATURE_PERSIST_STORE_SUPPORT)),y)	
             export BRCM_VODSL_CALL_FEATURE_PERSIST_STORE_SUPPORT=1
endif

BRCM_DSP_HAL := gw

BRCM_DSP_HAL_EXTENSION :=
XCHANGE_DSP_APP_EXTENSION :=
export XCHANGE_DSP_APP=$(BRCM_DSP_CODEC)

ifeq ($(strip $(BRCM_DSP_PCM)),y)
XCHANGE_DSP_APP := dspApp3341_tdm
BRCM_DSP_HAL_EXTENSION := _pcm
endif

ifeq ($(strip $(BRCM_DSP_PCM_G726)),y)
XCHANGE_DSP_APP := dspApp3341_tdm_g726
BRCM_DSP_HAL_EXTENSION := _pcm
endif

ifeq ($(strip $(BRCM_DSP_PCM_T38_EXT)),y)
XCHANGE_DSP_APP := dspApp3341_tdm_t38
BRCM_DSP_HAL_EXTENSION := _pcm
endif

ifeq ($(strip $(BRCM_DSP_APM_FXO_EXT)),y)
XCHANGE_DSP_APP := dspApp3341
BRCM_DSP_HAL_EXTENSION := _hybrid
XCHANGE_DSP_APP_EXTENSION := _fxo_ext
endif

BCM_VDSL_TYPE_DEFINES := -DDVDSL_CO_TYPE

ifeq ($(strip $(BUILD_VDSL)),y)
export BUILD_VDSL=y
VBOOT_BIN="boot.bin"
ifneq ($(strip $(CPE_ANNEX_B)),)
  CPE_MODEM="cpe_annex_b.bin"
else
  CPE_MODEM="cpe_annex_a.bin"
endif
ifeq ($(strip $(VCOPE_TYPE)),CO)
  VMODEM_BIN="co_modem.bin"
  BCM_VDSL_TYPE_DEFINES := -DVDSL_CO_TYPE=1
endif
ifeq ($(strip $(VCOPE_TYPE)),CPE)
  VMODEM_BIN=$(CPE_MODEM)
endif

endif

export ${BCM_VDSL_TYPE_DEFINES}

#
#  Warning here, we do re-assign some of the variables defined earlier:
#  BRCM_DSP_HAL and BRCM_DSP_HAL_EXTENSION for example, in order to pickup
#  the correct board HAL application.
#
ifeq ($(strip $(BRCM_DSP_FXO)),y)
ifeq ($(strip $(BRCM_SLIC_LE9502)),y)
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)_Le9502FXO
BRCM_RELEASETAG := $(BRCM_RELEASETAG)._LE9502
BRCM_DSP_HAL := _Le9502FXO
BRCM_DSP_HAL_EXTENSION :=
export BRCM_SLIC_LE9502
else
ifeq ($(strip $(BRCM_SLIC_LE9500)),y)
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)_Le9500FXO
BRCM_RELEASETAG := $(BRCM_RELEASETAG)._LE9500
BRCM_DSP_HAL := _Le9500FXO
BRCM_DSP_HAL_EXTENSION :=
export BRCM_SLIC_LE9500
endif
endif
endif

BRCM_DRIVER_ATM_BONDING_DEFINES := -DDBCM_ATM_BONDING -DDBCM_ATM_BONDING_ETH -DDATM_BONDING_TEST

ifeq ($(strip $(BRCM_CHIP)),6358)
ifeq ($(strip $(BRCM_DRIVER_ATM_BONDING)),m)
BRCM_DRIVER_ATM_BONDING_DEFINES := -DBCM_ATM_BONDING=1
#expandAtmbSrc:
#	@cd $(INC_ATMBONDING_DRV_PATH); \
#	if [ ! -e boot6358Vector.obj ]; then \
#	   echo Untarring AtmBonding xdrv source...; \
#		tar xvfz atmb_xdrivers.tgz 2> /dev/null; \
#	fi
else
#expandAtmbSrc:
endif
else
#expandAtmbSrc:
endif

ifeq ($(strip $(BRCM_CHIP)),6358)
ifeq ($(strip $(BRCM_DRIVER_ATM_BONDING_ETH)),m)
BRCM_DRIVER_ATM_BONDING_DEFINES := -DBCM_ATM_BONDING_ETH=1
endif
endif

export ${BRCM_DRIVER_ATM_BONDING_DEFINES}

#
# DSP codec flags definition.  To be used throughout the application (for configuration and vodsl)
#

BRCM_DSP_CODEC_DEFINES := -DXCFG_G711_SUPPORT=1

ifeq ($(strip $(BRCM_DSP_CODEC_G723)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_CODEC_G726)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_CODEC_G729)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_CODEC_G7xx)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_PCM)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_PCM_G726)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_PCM_T38_EXT)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_CODEC_T38_EXT)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_APM_FXO_EXT)),y)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
endif

ifeq ($(strip $(BRCM_DSP_CODEC)),all)
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV16_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_ILBC_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_GSMAMR_SUPPORT=1
endif


#
#  Definition of the number of voice channels supported based on the specific
#  application being created.
#

ifeq ($(strip $(BRCM_DSP_APM_FXO_EXT)), y)
BRCM_DSP_CHAN_DEFINES = -DNUM_APM_VOICE_CHANNELS=2 -DNUM_TDM_VOICE_CHANNELS=0 -DNUM_FXO_CHANNELS=1
else
BRCM_DSP_CHAN_DEFINES = -DNUM_APM_VOICE_CHANNELS=0 -DNUM_TDM_VOICE_CHANNELS=0 -DNUM_FXO_CHANNELS=0
endif

export BRCM_DSP_CODEC_DEFINES
export BRCM_DSP_CHAN_DEFINES
export BRCM_DSP_FXO
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)$(BRCM_DSP_HAL)$(BRCM_DSP_HAL_EXTENSION)
export XCHANGE_DSP_APP_EXTENSION
export BRCM_DSP_HAL_EXTENSION

# If no codec is selected, build G.711 load.
# Any XCHANGE_BUILD_APP directory would be OK
# because G.711 is included with all the voice DSP images.
ifneq ($(strip $(BUILD_VODSL)),)
ifeq ($(strip $(XCHANGE_DSP_APP)),)
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)gw
export XCHANGE_DSP_APP=g711
endif
endif
BRCM_RELEASETAG := $(BRCM_RELEASETAG).$(XCHANGE_DSP_APP)

ifeq ($(strip $(BRCM_CHIP)),6368)
ifeq ($(strip $(BRCM_6368_G729_FXO)),y)
BRCM_DSP_HAL := vw_fxo
BRCM_DSP_CHAN_DEFINES = -DNUM_FXO_CHANNELS=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_GSMAMR_SUPPORT=1
export BRCM_6368_G729_FXO
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_fxo
export BRCM_DSP_CODEC_DEFINES
endif
endif

ifeq ($(strip $(BRCM_CHIP)),6358)
ifeq ($(strip $(BRCM_6358_G729_FXO)),y)
BRCM_DSP_HAL := vw_fxo
BRCM_DSP_CHAN_DEFINES = -DNUM_FXO_CHANNELS=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_GSMAMR_SUPPORT=1
export BRCM_6358_G729_FXO
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_fxo
export BRCM_DSP_CODEC_DEFINES
else
ifeq ($(strip $(BRCM_6358_G729_4FXS)),y)
BRCM_DSP_HAL := vw_4fxs
export BRCM_6358_G729_4FXS
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_4fxs
else
ifeq ($(strip $(BRCM_6358_G729_2FXS_LEGERITY)),y)
BRCM_DSP_HAL := vw_legerity
export BRCM_6358_G729_2FXS_LEGERITY
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_legerity
else
ifeq ($(strip $(BRCM_6358_G729_FXO_LEGERITY)),y)
BRCM_DSP_HAL := vw_fxo_legerity
BRCM_DSP_CHAN_DEFINES = -DNUM_FXO_CHANNELS=1
export BRCM_6358_G729_FXO_LEGERITY
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_fxo_legerity
else
ifeq ($(strip $(BRCM_6358_G729_ISDN_FXO)),y)
BRCM_DSP_HAL := vw_isdn_fxo
BRCM_DSP_CHAN_DEFINES   = -DNUM_FXO_CHANNELS=1
BRCM_DSP_CODEC_DEFINES := -DXCFG_G711_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
export BRCM_6358_G729_ISDN_FXO
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_isdn_fxo
export BRCM_DSP_CODEC_DEFINES
else
ifeq ($(strip $(BRCM_6358_G729_ISDN_FXO_LE88311)),y)
BRCM_DSP_HAL := vw_isdn_1fxs_fxo
BRCM_DSP_CHAN_DEFINES   = -DNUM_FXO_CHANNELS=1
BRCM_DSP_CODEC_DEFINES := -DXCFG_G711_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
export BRCM_6358_G729_ISDN_FXO_LE88311
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_isdn_1fxs_fxo
export BRCM_DSP_CODEC_DEFINES
else
ifeq ($(strip $(BRCM_6358_G729_DECT)),y)
BRCM_DSP_HAL := vw_dect
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G711_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV16_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_ILBC_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
export BRCM_6358_G729_DECT
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_dect
export BRCM_DSP_CODEC_DEFINES
else
ifeq ($(strip $(BRCM_6358_G729_1FXS)),y)
BRCM_DSP_HAL := vw_1fxs
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G711_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV16_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_ILBC_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
export BRCM_6358_G729_1FXS
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_1fxs
else
ifeq ($(strip $(BRCM_6358_G729_WB_DECT)),y)
BRCM_DSP_HAL := vw_wb_dect
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV16_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV32_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_LPCMWB_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G722_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_ILBC_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_AMRWB_SUPPORT=1
export BRCM_6358_G729_WB_DECT
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_wb_dect
export BRCM_DSP_CODEC_DEFINES
else
ifeq ($(strip $(BRCM_6358_WIDEBAND)),y)
BRCM_DSP_HAL := vw_wb
BRCM_DSP_CODEC_DEFINES += -DXCFG_G7231_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G726_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G729_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV16_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_BV32_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_LPCMWB_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_G722_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_ILBC_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_FAX_SUPPORT=1
BRCM_DSP_CODEC_DEFINES += -DXCFG_AMRWB_SUPPORT=1
export BRCM_6358_WIDEBAND
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_wb
export BRCM_DSP_CODEC_DEFINES
else
ifeq ($(strip $(BRCM_6358_2FXS_DECT_PONG)),y)
BRCM_DSP_HAL := vw_pong
export BRCM_6358_2FXS_DECT_PONG
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw_pong
else
BRCM_DSP_HAL := vw
export BRCM_6358_G729
export XCHANGE_BUILD_APP=Bcm$(BRCM_CHIP)vw
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

ifeq ($(strip $(BRCM_VODSL_DUAL_3341)),y)
BRCM_RELEASETAG := $(BRCM_RELEASETAG).dual3341
endif

#Set up ADSL standard
export ADSL=$(BRCM_ADSL_STANDARD)

#Set up ADSL_PHY_MODE  {file | obj}
export ADSL_PHY_MODE=file

#Set up ADSL_SELF_TEST
export ADSL_SELF_TEST=$(BRCM_ADSL_SELF_TEST)

#Set up ADSL_PLN_TEST
export ADSL_PLN_TEST=$(BUILD_TR69_XBRCM)

#WL command
ifneq ($(strip $(WL)),)
build_nop:=$(shell cd $(USERAPPS_DIR)/broadcom/cfm; make clean; cd $(BUILD_DIR))
export WL
endif
export BUILD_WL_MIMO
export BUILD_WL_DEFAULT
export BUILD_WL_IMPL3

ifeq ($(strip $(BRCM_IKOS)),y)
BUSYBOX_CONFIG=ikos.config
FS_COMPRESSION=-noD
else
BUSYBOX_CONFIG=brcm.config
FS_COMPRESSION=-lzma
endif

#MGMT command
ifneq ($(strip $(MGMT)),) 
export BUILD_SNMP=dynamic
export BUILD_TR69c=
export BUILD_TR69C_SSL=
export BUILD_TR69C_BCM_SSL=
BUILD_SNMP_SET=y

ifeq ("$(MGMT)", "NETSNMP")
export NET_SNMP=y
export BRCM_SNMP=
export BUILD_TR69c=
BUILD_SNMP_ADSL_MIB=y
BUILD_SNMP_AT_MIB=y
BUILD_SNMP_ATM_MIB=y
BUILD_SNMP_ICMP_MIB=y
BUILD_SNMP_IP_MIB=y
BUILD_SNMP_SNMP_MIB=y
BUILD_SNMP_SYSOR_MIB=y
BUILD_SNMP_TCP_MIB=y
BUILD_SNMP_UDP_MIB=y
else

ifeq ("$(MGMT)", "BRCMSNMP")
export NET_SNMP=
export BRCM_SNMP=y
BUILD_SNMP_AUTO=y
BUILD_SNMP_CHINA_TELECOM_CPE_MIB=y
BUILD_SNMP_BRCM_CPE_MIB=y
BUILD_SNMP_UDP=y
BUILD_SNMP_EOC=y
BUILD_SNMP_AAL5=y
BUILD_SNMP_AUTO=y
BUILD_SNMP_DEBUG=
endif 


#endif of ifneq ($(strip $(MGMT)),)
endif

#end of MGMT
endif

#############################################
#
# Locale Selection
#
#############################################

ifeq ($(strip $(BRCM_VRG_COUNTRY_STATIC_PROFILE)),n)

# User-selected Configuration

LOCALEFLAGS = -DBRCM_VRG_COUNTRY_CFG_NON_STATIC_PROFILE=1
LOCALEFLAGS_TYPE2 = $(addprefix -D , BRCM_VRG_COUNTRY_CFG_NON_STATIC_PROFILE=1)

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_NORTH_AMERICA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_NORTH_AMERICA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_NORTH_AMERICA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_AUSTRALIA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_AUSTRALIA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_AUSTRALIA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_AUSTRIA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_AUSTRIA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_AUSTRIA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_BELGIUM)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_BELGIUM=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_BELGIUM=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_BRAZIL)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_BRAZIL=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_BRAZIL=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_CHILE)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_CHILE=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_CHILE=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_CHINA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_CHINA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_CHINA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_CZECH)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_CZECH=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_CZECH=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_DENMARK)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_DENMARK=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_DENMARK=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_ETSI)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_ETSI=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_ETSI=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_FINLAND)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_FINLAND=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_FINLAND=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_FRANCE)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_FRANCE=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_FRANCE=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_GERMANY)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_GERMANY=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_GERMANY=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_HUNGARY)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_HUNGARY=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_HUNGARY=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_IRELAND)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_IRELAND=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_IRELAND=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_ITALY)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_ITALY=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_ITALY=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_JAPAN)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_JAPAN=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_JAPAN=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_NETHERLANDS)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_NETHERLANDS=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_NETHERLANDS=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_NEW_ZEALAND)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_NEW_ZEALAND=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_NEW_ZEALAND=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_POLAND)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_POLAND=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_POLAND=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_ROMANIA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_ROMANIA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_ROMANIA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_SLOVAKIA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_SLOVAKIA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_SLOVAKIA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_SLOVENIA)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_SLOVENIA=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_SLOVENIA=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_SPAIN)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_SPAIN=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_SPAIN=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_SWEDEN)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_SWEDEN=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_SWEDEN=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_NORWAY)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_NORWAY=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_NORWAY=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_SWITZERLAND)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_SWITZERLAND=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_SWITZERLAND=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_TR57)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_TR57=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_TR57=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_UK)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_UK=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_UK=1)
endif

ifeq ($(strip $(BRCM_VRG_COUNTRY_CFG_TAIWAN)),y)
LOCALEFLAGS += -DBRCM_VRG_COUNTRY_CFG_TAIWAN=1
LOCALEFLAGS_TYPE2 += $(addprefix -D , BRCM_VRG_COUNTRY_CFG_TAIWAN=1)
endif

else

# Using Static Configuration

LOCALEFLAGS = -DBRCM_VRG_COUNTRY_CFG_STATIC_PROFILE=1
LOCALEFLAGS_TYPE2 = $(addprefix -D , BRCM_VRG_COUNTRY_CFG_STATIC_PROFILE=1)

endif

###########################################
#
# Complete list of applications
#
###########################################
export OPENSOURCE_DIR=$(USERAPPS_DIR)/opensource
SUBDIRS_OPENSOURCE = $(OPENSOURCE_DIR)/atm2684/pvc2684ctl \
        $(OPENSOURCE_DIR)/openssl \
        $(OPENSOURCE_DIR)/ipsec-tools \
        $(OPENSOURCE_DIR)/bridge-utils \
        $(OPENSOURCE_DIR)/ppp/pppoe \
        $(OPENSOURCE_DIR)/udhcp \
        $(OPENSOURCE_DIR)/iptables \
        $(OPENSOURCE_DIR)/ebtables \
        $(OPENSOURCE_DIR)/reaim  \
        $(OPENSOURCE_DIR)/iproute2  \
        $(OPENSOURCE_DIR)/libosip2 \
        $(OPENSOURCE_DIR)/libusb \
        $(OPENSOURCE_DIR)/siproxd \
        $(OPENSOURCE_DIR)/net-snmp  \
        $(OPENSOURCE_DIR)/ftpd \
        $(OPENSOURCE_DIR)/libcreduction \
        $(OPENSOURCE_DIR)/radvd \
        $(OPENSOURCE_DIR)/dhcpv6 \
        $(OPENSOURCE_DIR)/dproxy-nexgen \
        $(OPENSOURCE_DIR)/pptp \
        $(OPENSOURCE_DIR)/busybox \
        $(OPENSOURCE_DIR)/oprofile
        

#In future, we need to add soap when it
#is decoupled from cli

export BROADCOM_DIR=$(USERAPPS_DIR)/broadcom
SUBDIRS_BROADCOM = \
	$(BROADCOM_DIR)/nvram \
	$(BROADCOM_DIR)/cfm \
        $(BROADCOM_DIR)/upnp/router/upnp \
        $(BROADCOM_DIR)/nas \
        $(BROADCOM_DIR)/wlctl \
        $(BROADCOM_DIR)/vodsl \
        $(BROADCOM_DIR)/atmctl \
        $(BROADCOM_DIR)/adslctl \
        $(BROADCOM_DIR)/netctl \
        $(BROADCOM_DIR)/dnsprobe \
        $(BROADCOM_DIR)/dynahelper \
        $(BROADCOM_DIR)/dnsspoof \
        $(BROADCOM_DIR)/igmp \
        $(BROADCOM_DIR)/dhcpr \
	$(BROADCOM_DIR)/sntp \
	$(BROADCOM_DIR)/ippd \
	$(BROADCOM_DIR)/ddnsd \
	$(BROADCOM_DIR)/ethctl \
        $(BROADCOM_DIR)/hotplug \
        $(BROADCOM_DIR)/epittcp \
        $(BROADCOM_DIR)/snmp \
        $(BROADCOM_DIR)/tr69c \
        $(BROADCOM_DIR)/tr64 \
        $(BROADCOM_DIR)/ses \
        $(BROADCOM_DIR)/relayCtl \
        $(BROADCOM_DIR)/vdslctl \
        $(BROADCOM_DIR)/hostCode \
        $(BROADCOM_DIR)/bcmssl 

SUBDIRS_APP = $(SUBDIRS_BROADCOM) $(SUBDIRS_OPENSOURCE)
SUBDIRS = $(foreach dir, $(SUBDIRS_APP), $(shell if [ -d "$(dir)" ]; then echo $(dir); fi))

OPENSOURCE_APPS = ipsec-tools pvc2684ctl pvc2684d brctl pppd udhcp iptables ebtables ip \
                  reaim tc libosip2 libusb siproxd snmp bftpd radvd dhcpv6 busybox oprofile dproxy UrlFilter pptp

BROADCOM_APPS = nvram bcmcrypto bcmshared bcmssl nas wlctl cfm upnp vodsl atmctl adslctl netctl dnsprobe dynahelper dnsspoof \
                igmp dhcpr diagapp sntp ddnsd ilmi ippd hotplug ethctl epittcp snmp ses \
                hmi2proxy relayctl vdslctl lld2d wsccmd
LIBC_OPTIMIZATION = libcreduction

ifneq ($(strip $(BUILD_GDBSERVER)),)
TOOLCHAIN_UTIL_APPS = gdbserver
endif

BUSYBOX_DIR = $(OPENSOURCE_DIR)/busybox

BRCMAPPS = openssl $(BROADCOM_APPS) $(OPENSOURCE_APPS) $(TOOLCHAIN_UTIL_APPS) $(LIBC_OPTIMIZATION)

all: dare_info signature sanity_check profile_check create_install kernelbuild modbuild kernelbuildlite app hosttools buildimage

menuconfig:
	@cd $(INC_KERNEL_BASE); \
	if [ ! -e linux/CREDITS ]; then \
	  echo Untarring original Linux kernel source...; \
	  (tar xkfj $(ORIGINAL_KERNEL) 2> /dev/null || true); \
	fi
	$(MAKE) -C $(KERNEL_DIR)/scripts/lxdialog HOSTCC=gcc
	$(CONFIG_SHELL) $(HOSTTOOLS_DIR)/scripts/Menuconfig $(TARGETS_DIR)/config.in

#dare modified
DARE_BUILD_TIME := $(shell date)
ifeq ($(strip $(BRCM_FLASHBLK_SIZE)),128)
CFE_FILE = $(TARGETS_DIR)/cfe/cfe$(BRCM_CHIP)_128K_block.bin
endif
dare_info:
	rm -f $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h;
	@echo '#define DARE_BASIC_VER' \"$(DARE_BASIC_VER)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
	@echo '#define DARE_SW_VER' \"$(DARE_SW_VER)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
	@echo '#define DARE_SVN_URL' \"$(DARE_SVN_URL)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
	@echo '#define DARE_SVN_VER' \"$(DARE_SVN_VER)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
	@echo '#define DARE_BUILD_TIME' \"$(DARE_BUILD_TIME)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
ifneq ($(strip $(DARE_SOFTWARE_VERSION)),)
	@echo '#define DARE_SOFTWARE_VERSION' \"$(DARE_SOFTWARE_VERSION)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
ifneq ($(strip $(DARE_HARDWARE_VERSION)),)
	@echo '#define DARE_HARDWARE_VERSION' \"$(DARE_HARDWARE_VERSION)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
ifneq ($(strip $(DARE_MANUFACTURER)),)
	@echo '#define DARE_MANUFACTURER' \"$(DARE_MANUFACTURER)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
ifneq ($(strip $(DARE_MODELNAME)),)
	@echo '#define DARE_MODELNAME' \"$(DARE_MODELNAME)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
ifneq ($(strip $(DARE_DESCRIPTION)),)
	@echo '#define DARE_DESCRIPTION' \"$(DARE_DESCRIPTION)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
ifneq ($(strip $(DARE_PRODUCT_CLASS)),)
	@echo '#define DARE_PRODUCT_CLASS' \"$(DARE_PRODUCT_CLASS)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
ifneq ($(strip $(DARE_OUI)),)
	@echo '#define DARE_OUI' \"$(DARE_OUI)\" >> $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/dareinfo.h
endif
#dare modified end

sanity_check:
	@if [ "$(PROFILE)" = "" ]; then \
          echo You need to specify build profile name from $(TARGETS_DIR) using 'make PROFILE=<profile name>...'; exit 1; \
	fi

profile_check:
profile_check_xxx:
	@if [ "$(LAST_PROFILE)" != "" ] && [ "$(LAST_PROFILE)" != "$(PROFILE)" ]; then \
		echo "The specified profile, $(PROFILE), differs from the last one built, $(LAST_PROFILE)."; \
		echo "The entire image must be rebuilt."; \
		read -p "Press ENTER to rebuild the entire image or CTRL-C to abort. " ; \
		$(MAKE) PROFILE=$(LAST_PROFILE) clean; \
		$(MAKE) PROFILE=$(PROFILE); \
		echo "Ignore the make exit error, Error 1"; \
		exit 1; \
	fi

create_install:
		mkdir -p $(PROFILE_DIR)/fs.install
		mkdir -p $(PROFILE_DIR)/fs.install/etc

$(KERNEL_DIR)/vmlinux:
	$(GENDEFCONFIG_CMD) $(PROFILE_PATH)
	cd $(KERNEL_DIR); \
	cp -f $(KERNEL_DIR)/arch/mips/defconfig $(KERNEL_DIR)/.config; \
	$(MAKE) oldconfig; $(MAKE); $(MAKE) modules_install

kernelbuild:
ifeq ($(wildcard $(KERNEL_DIR)/vmlinux),)
	@cd $(INC_KERNEL_BASE); \
	if [ ! -e linux/CREDITS ]; then \
	  echo Untarring original Linux kernel source...; \
	  (tar xkfj $(ORIGINAL_KERNEL) 2> /dev/null || true); \
	fi
	$(GENDEFCONFIG_CMD) $(PROFILE_PATH)
	cd $(KERNEL_DIR); \
	cp -f $(KERNEL_DIR)/arch/mips/defconfig $(KERNEL_DIR)/.config; \
	$(MAKE) oldconfig; $(MAKE)
else
	cd $(KERNEL_DIR); $(MAKE)
endif

kernel: profile_check create_install kernelbuild hosttools buildimage

ifneq ($(strip $(BRCM_CHIP)),6348)
ifneq ($(strip $(BRCM_CHIP)),6338)
ifeq ($(strip $(BRCM_DRIVER_BCMDSP)),m)
kernelbuildlite:
	@echo "******************** Kernel Build Lite ********************"; \
	$(BRCMDRIVERS_DIR)/broadcom/char/dspapp/impl1/getDspModSizes.sh  $(BRCMDRIVERS_DIR)/broadcom/char/dspapp/impl1/dspdd.ko $(SHARED_DIR) $(CROSS_COMPILE) $(KERNEL_DEBUG);
	cd $(KERNEL_DIR); $(MAKE)
else
kernelbuildlite:
endif
else
kernelbuildlite:
endif
else
kernelbuildlite:
endif

ifeq ($(strip $(VOXXXLOAD)),1)
modbuild: touch_voice_files
	cd $(KERNEL_DIR); $(MAKE) modules && $(MAKE) modules_install
else
modbuild:
	cd $(KERNEL_DIR); $(MAKE) modules && $(MAKE) modules_install
endif

adslmodbuild:
	cd $(KERNEL_DIR); $(MAKE) M=$(INC_ADSLDRV_PATH) modules 
adslmodbuildclean:
	cd $(KERNEL_DIR); $(MAKE) M=$(INC_ADSLDRV_PATH) clean

modules: profile_check create_install modbuild kernelbuildlite hosttools buildimage

adslmodule: adslmodbuild
adslmoduleclean: adslmodbuildclean

app: profile_check create_install prebuild $(BRCMAPPS) hosttools buildimage

prebuild:
	mkdir -p $(INSTALL_DIR)/bin $(INSTALL_DIR)/lib

# touch_voice_files doesn't clean up voice, just enables incremental build of voice code
touch_voice_files:
	find bcmdrivers/broadcom/char/endpoint/ \( -name '*.o' -o -name '*.a' -o -name '*.lib' -o -name '*.ko' -o -name '*.cmd' -o -name '.*.cmd' -o -name '*.c' -o -name '*.mod' \) -print -exec rm -f "{}" ";"
	rm -rf kernel/linux/.tmp_versions/endpointdd.mod
	rm -rf kernel/linux/arch/mips/defconfig
	rm -rf kernel/linux/include/config/bcm/endpoint/
	rm -rf kernel/linux/include/asm-mips/offset.h
	rm -rf kernel/linux/include/asm-mips/reg.h
	find kernel/linux/lib/ -name '*.o' -print -exec rm -f "{}" ";"
	find kernel/linux/lib/ -name '*.lib' -print -exec rm -f "{}" ";"

# Build user applications depending on if they are
# specified in the profile
ifneq ($(strip $(BUILD_PVC2684CTL)),)
export BUILD_PVC2684D=$(BUILD_PVC2684CTL)
pvc2684d:
pvc2684ctl:
	$(MAKE) -C $(OPENSOURCE_DIR)/atm2684/pvc2684ctl $(BUILD_PVC2684CTL)
else
pvc2684d:
pvc2684ctl:
endif

ifneq ($(strip $(BUILD_BRCTL)),)
brctl:
	cd $(OPENSOURCE_DIR);   (tar xkfj bridge-utils.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/bridge-utils $(BUILD_BRCTL)
else
brctl:
endif

ifneq ($(strip $(BUILD_VCONFIG)),)
export BUILD_VCONFIG=y
endif

ifneq ($(strip $(BUILD_SUPERDMZ)),)
export BUILD_SUPERDMZ=y
endif

ifneq ($(strip $(BUILD_CFM)),)

ifneq ($(strip $(BUILD_CFM_MENU)),)
export BUILD_CFM_MENU=y
endif

cfm:
	$(MAKE) -C $(BROADCOM_DIR)/cfm $(BUILD_CFM)
else
cfm:
endif

# iptables is dependent on kernel netfilter modules
ifneq ($(strip $(BRCM_KERNEL_NETFILTER)),)
ifneq ($(strip $(BUILD_IPTABLES)),)
iptables:
	cd $(OPENSOURCE_DIR);   (tar xkfj iptables.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/iptables $(BUILD_IPTABLES)
iptables-build:
	cd $(OPENSOURCE_DIR);   (tar xkfj iptables.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/iptables static
else
iptables:
endif
else
iptables:
	@echo Warning: You need to enable netfilter in the kernel !!!!!
endif

ifneq ($(strip $(BUILD_EBTABLES)),)
ebtables:
	cd $(OPENSOURCE_DIR);   (tar xkfj ebtables.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/ebtables $(BUILD_EBTABLES)
else
ebtables:
endif

ifneq ($(strip $(BUILD_PPPD)),)
pppd:
	$(MAKE) -C $(OPENSOURCE_DIR)/ppp/pppoe $(BUILD_PPPD)
else
pppd:
endif

ifneq ($(strip $(BUILD_REAIM)),)
reaim:
	cd $(OPENSOURCE_DIR);   (tar xkfj reaim.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/reaim $(BUILD_REAIM)
else
reaim:
endif

ifneq ($(strip $(BRCM_KERNEL_NETQOS)),)
tc:
	cd $(OPENSOURCE_DIR);   (tar xkfj iproute2.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/iproute2 dynamic
else
tc:
endif

ifneq ($(strip $(BUILD_IPROUTE2)),)
ip:
	cd $(OPENSOURCE_DIR);   (tar xkfj iproute2.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/iproute2 dynamic
else
ip:
endif

ifneq ($(strip $(BUILD_GDBSERVER)),)
gdbserver:
	install -m 755 $(TOOLCHAIN)/mips-linux-uclibc/target-apps/usr/bin/gdbserver $(INSTALL_DIR)/bin
	$(STRIP) $(INSTALL_DIR)/bin/gdbserver
else
gdbserver:
endif

ifneq ($(strip $(BUILD_ETHWAN)),)
export BUILD_ETHWAN=y
endif

ifneq ($(strip $(BUILD_UDHCP)),)
udhcp:
	$(MAKE) -C $(OPENSOURCE_DIR)/udhcp $(BUILD_UDHCP)
else
udhcp:
endif

# UPNP is dependent on iptables
ifneq ($(strip $(BUILD_IPTABLES)),)
ifneq ($(strip $(BUILD_UPNP)),)
upnp: iptables-build
	$(MAKE) -C $(BROADCOM_DIR)/upnp/router/upnp  $(BUILD_UPNP)
else
upnp:
endif
else
upnp:
	@echo Warning: You need to build iptables first !!!!!
endif

ifneq ($(strip $(BUILD_IPSEC_TOOLS)),)
ipsec-tools:
	cd $(OPENSOURCE_DIR);   (tar xkfj ipsec-tools.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/ipsec-tools $(BUILD_IPSEC_TOOLS)
else
ipsec-tools:
endif

ifneq ($(strip $(BUILD_CERT)),)
openssl:
	cd $(OPENSOURCE_DIR);   (tar xkfj openssl.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/openssl dynamic
else
openssl:
endif


ifneq ($(strip $(BUILD_SIPROXD)),)
siproxd:
	cd $(OPENSOURCE_DIR);   (tar xkfj siproxd.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/siproxd $(BUILD_SIPROXD)
libosip2:
	cd $(OPENSOURCE_DIR);   (tar xkfj libosip2.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/libosip2
else
siproxd:

libosip2:

endif

ifneq ($(strip $(BUILD_SNMP)),)

ifneq ($(strip $(BUILD_SNMP_SET)),)
export SNMP_SET=1
else
export SNMP_SET=0
endif

ifneq ($(strip $(BUILD_SNMP_ADSL_MIB)),)
export SNMP_ADSL_MIB=1
else
export SNMP_ADSL_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_ATM_MIB)),)
export SNMP_ATM_MIB=1
else
export SNMP_ATM_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_AT_MIB)),)
export SNMP_AT_MIB=1
else
export SNMP_AT_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_SYSOR_MIB)),)
export SNMP_SYSOR_MIB=1
else
export SNMP_SYSOR_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_TCP_MIB)),)
export SNMP_TCP_MIB=1
else
export SNMP_TCP_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_UDP_MIB)),)
export SNMP_UDP_MIB=1
else
export SNMP_UDP_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_IP_MIB)),)
export SNMP_IP_MIB=1
else
export SNMP_IP_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_ICMP_MIB)),)
export SNMP_ICMP_MIB=1
else
export SNMP_ICMP_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_SNMP_MIB)),)
export SNMP_SNMP_MIB=1
else
export SNMP_SNMP_MIB=0
endif

ifneq ($(strip $(BUILD_SNMP_ATMFORUM_MIB)),)
export SNMP_ATMFORUM_MIB=1
else
export SNMP_ATMFORUM_MIB=0
endif

ifneq ($(strip $(BRCM_SNMP)),)

ifneq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB)),)
export BUILD_SNMP_CHINA_TELECOM_CPE_MIB=y
endif

ifneq ($(strip $(BUILD_CT_1_39_OPEN)),)
export BUILD_CT_1_39_OPEN=y
endif

ifneq ($(strip $(BUILD_SNMP_CHINA_TELECOM_CPE_MIB_V2)),)
export BUILD_SNMP_CHINA_TELECOM_CPE_MIB_V2=y
endif

ifneq ($(strip $(BUILD_SNMP_BRCM_CPE_MIB)),)
export BUILD_SNMP_BRCM_CPE_MIB=y
endif

ifneq ($(strip $(BUILD_SNMP_UDP)),)
export BUILD_SNMP_UDP=y
endif

ifneq ($(strip $(BUILD_SNMP_EOC)),)
export BUILD_SNMP_EOC=y
endif

ifneq ($(strip $(BUILD_SNMP_AAL5)),)
export BUILD_SNMP_AAL5=y
endif

ifneq ($(strip $(BUILD_SNMP_AUTO)),)
export BUILD_SNMP_AUTO=y
endif

ifneq ($(strip $(BUILD_SNMP_DEBUG)),)
export BUILD_SNMP_DEBUG=y
endif

ifneq ($(strip $(BUILD_SNMP_TRANSPORT_DEBUG)),)
export BUILD_SNMP_TRANSPORT_DEBUG=y
endif

ifneq ($(strip $(BUILD_SNMP_LAYER_DEBUG)),)
export BUILD_SNMP_LAYER_DEBUG=y
endif
endif

snmp:
ifneq ($(strip $(BRCM_SNMP)),)
##	$(MAKE) -C $(BROADCOM_DIR)/snmp $(BUILD_SNMP)
else
	cd $(OPENSOURCE_DIR);   (tar xkfj net-snmp.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/net-snmp $(BUILD_SNMP)
endif
else
snmp:
endif


ifeq ($(strip $(BUILD_SQUASH_HIGH)),y)
export BUILD_SQUASH_HIGH=y
endif
ifneq ($(strip $(BUILD_SQUASH_NORMAL)),)
export BUILD_SQUASH_NORMAL=y
endif
ifneq ($(strip $(BUILD_SQUASH_LOW)),)
export BUILD_SQUASH_LOW=y
endif
#dare image signature check
ifneq ($(strip $(DARE_IMAGE_SIGNATURE)),)
export DARE_IMAGE_SIGNATURE=y
signature:
	cd $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/; cp -f bcmTag.h.bak bcmTag.h
	cd $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/; sed 's/#define BCM_SIG_1   \"BroadcomCorporation\"/#define BCM_SIG_1   \"$(DARE_IMAGE_SIGNATURE_NAME)\"/' bcmTag.h > bcmTag.h.tmp
	cd $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/; cp -f bcmTag.h.tmp bcmTag.h
else
signature:
	cd $(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)/; cp -f bcmTag.h.bak bcmTag.h
endif

ifneq ($(strip $(BUILD_4_LEVEL_QOS)),)
export BUILD_4_LEVEL_QOS=y
endif

ifneq ($(strip $(BUILD_ILMI)),)
ilmi:
	cd $(OPENSOURCE_DIR);   (tar xkfj net-snmp.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(BROADCOM_DIR)/ilmi $(BUILD_ILMI)
else
ilmi:
endif

ifneq ($(strip $(BUILD_VODSL)),)
vodsl:
	$(MAKE) -C $(BROADCOM_DIR)/vodsl $(BUILD_VODSL)
else
vodsl:
endif

# Leave it for the future when soap server is decoupled from cfm
ifneq ($(strip $(BUILD_SOAP)),)
ifeq ($(strip $(BUILD_SOAP_VER)),2)
soapserver:
	$(MAKE) -C $(BROADCOM_DIR)/SoapToolkit/SoapServer $(BUILD_SOAP)
else
soap:
	$(MAKE) -C $(BROADCOM_DIR)/soap $(BUILD_SOAP)
endif
else
soap:
endif

ifneq ($(strip $(BUILD_NAS)),)
export WIRELESS=1
nas:
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/nas ]; then \
	  rm -rf $(BROADCOM_DIR)/nas/*; \
	  cp -pRdf $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/nas/* $(BROADCOM_DIR)/nas; \
	fi
	$(MAKE) -C $(BROADCOM_DIR)/nas $(BUILD_NAS)

#add hotplug here, for nas use only
hotplug:
	$(MAKE) -C $(BROADCOM_DIR)/hotplug $(BUILD_NAS)

bcmcrypto:
	if [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/bcmcrypto/Makefile ]; then \
		$(MAKE) -C $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/bcmcrypto dynamic; \
	elif [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmcrypto.a ]; then \
		install -m 755 $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmcrypto.a $(INSTALL_DIR)/lib; \
	elif [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmcrypto.so ]; then \
		install -m 755 $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmcrypto.so $(INSTALL_DIR)/lib; \
	elif [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmcrypto.so_$(PROFILE)_save ]; then \
		install -m 755 $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmcrypto.so_$(PROFILE)_save $(INSTALL_DIR)/lib/libwlbcmcrypto.so; \
	fi

bcmshared:
	if [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/shared/Makefile ] && [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/shared/UdpLib.c ]; then \
		$(MAKE) -C $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/shared dynamic; \
	elif [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmshared.a ]; then \
		install -m 755 $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmshared.a $(INSTALL_DIR)/lib; \
	elif [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmshared.so ]; then \
		install -m 755 $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmshared.so $(INSTALL_DIR)/lib; \
	elif [ -f $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmshared.so_$(PROFILE)_save ]; then \
		install -m 755 $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/libwlbcmshared.so_$(PROFILE)_save $(INSTALL_DIR)/lib/libwlbcmshared.so; \
	fi
else
export WIRELESS=0
nas:
#add hotplug here, for nas use only
hotplug:
bcmcrypto:
bcmshared:
endif

ifneq ($(strip $(BUILD_WLCTL)),)
export WIRELESS=1
export BUILD_WLCTL_SHLIB=1
wlctl:
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/wlctl ]; then \
	  rm -rf $(BROADCOM_DIR)/wlctl/*; \
	  cp -pRdf $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/wlctl/* $(BROADCOM_DIR)/wlctl; \
	fi
	$(MAKE) -C $(BROADCOM_DIR)/wlctl $(BUILD_WLCTL)
else
export WIRELESS=0
wlctl:
endif

export BUILD_WL_SHLIB=1
#Set 1 to BUILD_WL_SHLIB to enable wl_shared library.
#Set 0 to BUILD_WL_SHLIB to use wl static library.

ifneq ($(strip $(BUILD_EPITTCP)),)
epittcp:
	$(MAKE) -C $(BROADCOM_DIR)/epittcp $(BUILD_EPITTCP)
else
epittcp:
endif

ifneq ($(strip $(BUILD_SES)),)
ses:
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/ses ]; then \
	  rm -rf $(BROADCOM_DIR)/ses/*; \
	  cp -pRdf $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/ses/* $(BROADCOM_DIR)/ses; \
	fi
	$(MAKE) -C $(BROADCOM_DIR)/ses $(BUILD_SES)
else
ses:
endif

ifneq ($(strip $(BUILD_NVRAM)),)
nvram:
	$(MAKE) -C $(BROADCOM_DIR)/nvram $(BUILD_NVRAM)
else
nvram:
endif

#Always compile Ethernet control utility
ethctl:
	$(MAKE) -C $(BROADCOM_DIR)/ethctl dynamic

ifneq ($(strip $(BUILD_DNSPROBE)),)
dnsprobe:
	$(MAKE) -C $(BROADCOM_DIR)/dnsprobe $(BUILD_DNSPROBE)
else
dnsprobe:
endif

ifneq ($(strip $(BUILD_DYNAHELPER)),)
dynahelper:
	$(MAKE) -C $(BROADCOM_DIR)/dynahelper $(BUILD_DYNAHELPER)
else
dynahelper:
endif

ifneq ($(strip $(BUILD_DNSSPOOF)),)
dnsspoof:
	$(MAKE) -C $(BROADCOM_DIR)/dnsspoof $(BUILD_DNSSPOOF)
else
dnsspoof:
endif

ifneq ($(strip $(BUILD_DPROXY)),)
dproxy:
	cd $(OPENSOURCE_DIR);   (tar xkfj dproxy-nexgen.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/dproxy-nexgen $(BUILD_DPROXY)
else
dproxy:
endif

ifneq ($(strip $(BUILD_IGMP)),)
igmp:
	$(MAKE) -C $(BROADCOM_DIR)/igmp $(BUILD_IGMP)
else
igmp:
endif

ifneq ($(strip $(BUILD_DHCPR)),)
dhcpr:
	$(MAKE) -C $(BROADCOM_DIR)/dhcpr $(BUILD_DHCPR)
else
dhcpr:
endif

ifeq ($(strip $(BUILD_LIBUSB)),y)
libusb:
	cd $(OPENSOURCE_DIR);   (tar xkfj libusb.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/libusb
	install -m 755 $(OPENSOURCE_DIR)/libusb/.libs/libusb-0.1.so.4 $(INSTALL_DIR)/lib
else
libusb:
endif
ifneq ($(strip $(BUILD_ATMCTL)),)
atmctl:
	$(MAKE) -C $(BROADCOM_DIR)/atmctl $(BUILD_ATMCTL)
else
atmctl:
endif

ifneq ($(strip $(BUILD_ADSLCTL)),)
adslctl:
	$(MAKE) -C $(BROADCOM_DIR)/adslctl $(BUILD_ADSLCTL)
else
adslctl:
endif

ifeq ($(strip $(BUILD_CFM_CLI)),y)
ifneq ($(strip $(BUILD_NETCTL)),)
netctl:
	$(MAKE) -C $(BROADCOM_DIR)/netctl $(BUILD_NETCTL)
else
netctl:
endif
else
netctl:
endif

ifneq ($(strip $(BUILD_BUSYBOX)),)
busybox:
	cd $(OPENSOURCE_DIR); (tar xkfj busybox.tar.bz2 2> /dev/null || true)
#	$(MAKE) -C $(OPENSOURCE_DIR)/busybox $(BUILD_BUSYBOX)
	cd $(OPENSOURCE_DIR)/busybox; cp -f $(BUSYBOX_CONFIG) .config
#dare add trace and ping
ifneq ($(strip $(DARE_TRACE_PING)),)
	cd $(OPENSOURCE_DIR)/busybox; sed 's/# CONFIG_TRACEROUTE is not set/CONFIG_TRACEROUTE=y\n# CONFIG_FEATURE_TRACEROUTE_VERBOSE is not set/' .config > brcmtmp.config
	cd $(OPENSOURCE_DIR)/busybox; cp -f brcmtmp.config .config
endif
	$(MAKE) -C $(OPENSOURCE_DIR)/busybox install
else
busybox:
endif

#zcy add
pptp:
	$(MAKE) -C $(OPENSOURCE_DIR)/pptp
ifneq ($(strip $(BUILD_LIBCREDUCTION)),)
libcreduction:
	$(MAKE) -C $(OPENSOURCE_DIR)/libcreduction install
else
libcreduction:
endif

ifneq ($(strip $(BUILD_DIAGAPP)),)
diagapp:
	$(MAKE) -C $(BROADCOM_DIR)/diagapp $(BUILD_DIAGAPP)
else
diagapp:
endif

ifneq ($(strip $(BUILD_FTPD)),)
bftpd:
	cd $(OPENSOURCE_DIR);   (tar xkfj ftpd.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/ftpd $(BUILD_FTPD)
else
bftpd:
endif

ifneq ($(strip $(BUILD_DDNSD)),)
ddnsd:
	$(MAKE) -C $(BROADCOM_DIR)/ddnsd $(BUILD_DDNSD)
else
ddnsd:
endif

ifneq ($(strip $(BUILD_SNTP)),)
sntp:
	$(MAKE) -C $(BROADCOM_DIR)/sntp $(BUILD_SNTP)
else
sntp:
endif

ifneq ($(strip $(BUILD_IPPD)),)
ippd:
	$(MAKE) -C $(BROADCOM_DIR)/ippd $(BUILD_IPPD)
else
ippd:
endif

ifneq ($(strip $(BUILD_LLTD)),)
lld2d:
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/lltd ]; then \
	  rm -rf $(BROADCOM_DIR)/lltd/*; \
	  cp  -rf $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/lltd/* $(BROADCOM_DIR)/lltd; \
	fi
	$(MAKE) -C $(BROADCOM_DIR)/lltd $(BUILD_LLTD)
else
lld2d:
endif

ifneq ($(strip $(BUILD_WSC)),)
export BUILD_WSC_CONFIG_UPNP_DISABLE=0
# BUILD_WSC_CONFIG_UPNP_DISABLE=1 to disable UPNP w/ AP config mode
# BUILD_WSC_CONFIG_UPNP_DISABLE=0 should be 0 for WiFi test
wsccmd:
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/wsc ]; then \
	  rm -rf $(BROADCOM_DIR)/wsc/*; \
	  cp  -rf $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/wsc/* $(BROADCOM_DIR)/wsc; \
	fi
	$(MAKE) -C $(BROADCOM_DIR)/wsc $(BUILD_WSC)
else
wsccmd:
endif

ifneq ($(strip $(BUILD_PORT_MIRRORING)),)
export BUILD_PORT_MIRRORING=1
else
export BUILD_PORT_MIRRORING=0
endif

ifeq ($(BRCM_USE_SUDO_IFNOT_ROOT),y)
BRCM_BUILD_USR=$(shell whoami)
BRCM_BUILD_USR1=$(shell sudo touch foo;ls -l foo | awk '{print $$3}';sudo rm -rf foo)
else
BRCM_BUILD_USR=root
endif

ifneq ($(strip $(BUILD_HOSTMGR)),)
hmi2proxy: vdsl_processing
	$(MAKE) -C $(BROADCOM_DIR)/hostCode/hostMgr $(BUILD_HOSTMGR) PHY=memap6348 TARGET='"HOSTMGR"'
else
hmi2proxy:
endif

ifneq ($(strip $(BUILD_RELAYCONTROL)),)
relayctl: add_6510_support
	$(MAKE) -C $(BROADCOM_DIR)/relayCtl/relay6348 $(BUILD_RELAYCONTROL) PHY=proxy TARGET='"RELAY"'
else
relayctl:
endif

ifneq ($(strip $(BUILD_VDSLCTL)),)
vdslctl: add_6510_support
	$(MAKE) -C $(BROADCOM_DIR)/vdslctl/vdslctl $(BUILD_VDSLCTL) PHY=proxy TARGET='"VDSLCTL"'
else
vdslctl:
endif

ifneq ($(strip $(BUILD_TR69C_BCM_SSL)),)
bcmssl: 
	cd $(BROADCOM_DIR);	(tar xkjf bcmssl.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(BROADCOM_DIR)/bcmssl
else
bcmssl:
endif

ifneq ($(strip $(BUILD_IPV6)),)
radvd: 
	cd $(OPENSOURCE_DIR);   (tar xkjf radvd.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/radvd
else
radvd:
endif

ifneq ($(strip $(BUILD_IPV6)),)
dhcpv6: 
	cd $(OPENSOURCE_DIR);   (tar xkjf dhcpv6.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/dhcpv6
else
dhcpv6:
endif

ifneq ($(strip $(BUILD_IPV6)),)
ip:
	cd $(OPENSOURCE_DIR);   (tar xkfj iproute2.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/iproute2 dynamic
else
ip:
endif

ifneq ($(strip $(BUILD_OPROFILE)),)
oprofile:
	cd $(OPENSOURCE_DIR);   (tar xkfj oprofile.tar.bz2 2> /dev/null || true)
	$(MAKE) -C $(OPENSOURCE_DIR)/oprofile
else
oprofile:
endif

hosttools:
	$(MAKE) -C $(HOSTTOOLS_DIR)

vdsl_processing: prebuild add_6510_support vdsl_modem vdsl_modem_file

vdsl_modem_file:
	@echo "************* VDSL Housekeeping **************************************"
	@echo	"VCOPE_BOARD=$(VCOPE_BOARD) HMI_VERSION=$(HMI_VERSION)"
	@echo "VCOPE_TYPE=$(VCOPE_TYPE) VCOPE_LINE_NUMBER=$(VCOPE_LINE_NUMBER)"
	@echo "**********************************************************************"
  
vdsl_modem:
	- cmp -s $(INSTALL_DIR)/images/modem.bin $(6510_REF_CODE)/images/$(VMODEM_BIN); \
	if [ ! $$? -eq 0 ]; then \
		echo "VCOPE: different type - need clean up"; \
		rm -f `find userapps/broadcom/hostCode/ -name *.[oa] -o -name *.depend`; \
		rm -f `find userapps/broadcom/relayCtl/ -name *.[oa] -o -name *.depend`; \
		rm -f `find userapps/broadcom/vdslctl/  -name *.[oa] -o -name *.depend`; \
		mkdir -p $(INSTALL_DIR)/images; \
	fi
	@echo "$(INSTALL_DIR)/images/: Creating \"modem.bin\" out of $(VMODEM_BIN)" 
	cp -f $(6510_REF_CODE)/images/$(VMODEM_BIN) $(INSTALL_DIR)/images/modem.bin;
	cp -f $(6510_REF_CODE)/images/$(VBOOT_BIN) $(INSTALL_DIR)/images/boot.bin;

export BRCM_IKOS

# IKOS Emulator build that does not include the CFE boot loader.
# Edit targets/ikos/ikos and change the chip and board id to desired values.
# Then build: make PROFILE=ikos ikos
ikos:
	@echo -e '#define SOFTWARE_VERSION ""\n#define RELEASE_VERSION ""\n#define PSI_VERSION ""\n' > $(BROADCOM_CFM_DIR)/inc/version.h
	@-mv -f $(FSSRC_DIR)/etc/profile $(FSSRC_DIR)/etc/profile.dontuse >& /dev/null
	@if [ ! -a $(CFE_FILE) ] ; then echo "no cfe" > $(CFE_FILE); echo "no cfe" > $(CFE_FILE).del; fi
	$(MAKE) PROFILE=$(PROFILE)
	@mv -f $(FSSRC_DIR)/etc/profile.dontuse $(FSSRC_DIR)/etc/profile
	@cd $(PROFILE_DIR); \
	$(OBJCOPY) --output-target=srec vmlinux vmlinux.srec; \
	xxd $(FS_KERNEL_IMAGE_NAME) | grep "^00000..:" | xxd -r > bcmtag.bin; \
	$(OBJCOPY) --output-target=srec --input-target=binary --change-addresses=0xb8010000 bcmtag.bin bcmtag.srec; \
	$(OBJCOPY) --output-target=srec --input-target=binary --change-addresses=0xb8010100 rootfs.img rootfs.srec; \
	@rm bcmtag.bin; \
	grep -v "^S7" vmlinux.srec > bcm9$(BRCM_CHIP)_$(PROFILE).srec; \
	grep "^S3" bcmtag.srec >> bcm9$(BRCM_CHIP)_$(PROFILE).srec; \
	grep -v "^S0" rootfs.srec >> bcm9$(BRCM_CHIP)_$(PROFILE).srec
	@if [ ! -a $(CFE_FILE).del ] ; then rm -f $(CFE_FILE) $(CFE_FILE).del; fi
	@echo -e "\nAn image without CFE for the IKOS emulator has been built.  It is named"
	@echo -e "targets/$(PROFILE)/bcm9$(BRCM_CHIP)_$(PROFILE).srec\n"

# IKOS Emulator build that includes the CFE boot loader.
# Both Linux and CFE boot loader toolchains need to be installed.
# Edit targets/ikos/ikos and change the chip and board id to desired values.
# Then build: make PROFILE=ikos ikoscfe
ikoscfe:
	@echo -e '#define SOFTWARE_VERSION ""\n#define RELEASE_VERSION ""\n#define PSI_VERSION ""\n' > $(BROADCOM_CFM_DIR)/inc/version.h
	@-mv -f $(FSSRC_DIR)/etc/profile $(FSSRC_DIR)/etc/profile.dontuse >& /dev/null
	$(MAKE) PROFILE=$(PROFILE)
	@mv -f $(FSSRC_DIR)/etc/profile.dontuse $(FSSRC_DIR)/etc/profile
	$(MAKE) -C $(BL_BUILD_DIR) clean
	$(MAKE) -C $(BL_BUILD_DIR)
	$(MAKE) -C $(BL_BUILD_DIR) ikos_finish
	cd $(PROFILE_DIR); \
	echo -n "** no kernel  **" > kernelfile; \
	$(HOSTTOOLS_DIR)/bcmImageBuilder --output $(CFE_FS_KERNEL_IMAGE_NAME) --chip $(BRCM_CHIP) --board $(BRCM_BOARD_ID) --blocksize $(BRCM_FLASHBLK_SIZE) --cfefile $(BL_BUILD_DIR)/cfe$(BRCM_CHIP).bin --rootfsfile rootfs.img --kernelfile kernelfile --include-cfe; \
	$(HOSTTOOLS_DIR)/createimg --boardid=$(BRCM_BOARD_ID) --numbermac=$(BRCM_NUM_MAC_ADDRESSES) --macaddr=$(BRCM_BASE_MAC_ADDRESS) --tp=$(BRCM_MAIN_TP_NUM) --psisize=$(BRCM_PSI_SIZE) --inputfile=$(CFE_FS_KERNEL_IMAGE_NAME) --outputfile=$(FLASH_IMAGE_NAME); \
	$(HOSTTOOLS_DIR)/addvtoken $(FLASH_IMAGE_NAME) $(FLASH_IMAGE_NAME).w; \
	$(OBJCOPY) --output-target=srec --input-target=binary --change-addresses=0xb8000000 $(FLASH_IMAGE_NAME).w $(FLASH_IMAGE_NAME).srec; \
	$(OBJCOPY) --output-target=srec vmlinux vmlinux.srec; \
	@rm kernelfile; \
	grep -v "^S7" vmlinux.srec > bcm9$(BRCM_CHIP)_$(PROFILE).srec; \
	grep "^S3" $(BL_BUILD_DIR)/cferam$(BRCM_CHIP).srec >> bcm9$(BRCM_CHIP)_$(PROFILE).srec; \
	grep -v "^S0" $(FLASH_IMAGE_NAME).srec >> bcm9$(BRCM_CHIP)_$(PROFILE).srec; \
	grep -v "^S7" vmlinux.srec > bcm9$(BRCM_CHIP)_$(PROFILE).utram.srec; \
	grep -v "^S0" $(BL_BUILD_DIR)/cferam$(BRCM_CHIP).srec >> bcm9$(BRCM_CHIP)_$(PROFILE).utram.srec;
	@echo -e "\nAn image with CFE for the IKOS emulator has been built.  It is named"
	@echo -e "targets/$(PROFILE)/bcm9$(BRCM_CHIP)_$(PROFILE).srec"
	@echo -e "\nBefore testing with the IKOS emulator, this build can be unit tested"
	@echo -e "with an existing chip and board as follows."
	@echo -e "1. Flash targets/$(PROFILE)/$(FLASH_IMAGE_NAME).w onto an existing board."
	@echo -e "2. Start the EPI EDB debugger.  At the edbice prompt, enter:"
	@echo -e "   edbice> fr m targets/$(PROFILE)/bcm9$(BRCM_CHIP)_$(PROFILE).utram.srec"
	@echo -e "   edbice> r"
	@echo -e "3. Program execution will start at 0xb8000000 (or 0xbfc00000) and,"
	@echo -e "   if successful, will enter the Linux shell.\n"


buildimage: $(KERNEL_DIR)/vmlinux
ifeq ($(strip $(BRCM_KERNEL_ROOTFS)),squashfs)
	cd $(TARGETS_DIR); ./buildFS
else
ifeq ($(BRCM_BUILD_USR),root)
	su --command="cd $(TARGETS_DIR); ./buildFS"
else
ifeq ($(BRCM_BUILD_USR1),root)
	cd $(TARGETS_DIR); sudo ./buildFS
else
	cd $(TARGETS_DIR); rm -rf $(PROFILE)/fs; ln -s /work/$(BRCM_BUILD_USR)/fs $(PROFILE)/fs; sudo ./buildFS
endif
endif
endif
ifeq ($(strip $(BRCM_KERNEL_ROOTFS)),squashfs)
	#$(HOSTTOOLS_DIR)/mksquashfs $(TARGET_FS) $(PROFILE_DIR)/rootfs.img -noappend -be -always-use-fragments -gzip
	$(HOSTTOOLS_DIR)/mksquashfs $(TARGET_FS) $(PROFILE_DIR)/rootfs.img -noappend -be $(FS_COMPRESSION) -no-fragments -noI -all-root -D $(HOSTTOOLS_DIR)/device_table.txt
else
 ifeq ($(strip $(BRCM_KERNEL_ROOTFS)),cramfs)
#	$(HOSTTOOLS_DIR)/mkcramfs -r -g $(TARGET_FS) $(PROFILE_DIR)/rootfs.img
	$(HOSTTOOLS_DIR)/mkcramfs -r $(TARGET_FS) $(PROFILE_DIR)/rootfs.img
 else
  ifeq ($(strip $(BRCM_KERNEL_ROOTFS)),jffs2)
	$(HOSTTOOLS_DIR)/mkfs.jffs2 -b -p -e $(BRCM_FLASHBLK_SIZE) -r $(TARGET_FS) -o $(PROFILE_DIR)/rootfs.img
  endif
 endif
endif

ifneq ($(strip $(BRCM_KERNEL_ROOTFS)),nfs)
	cd $(PROFILE_DIR); \
	cp $(KERNEL_DIR)/vmlinux . ; \
	$(STRIP) --remove-section=.note --remove-section=.comment vmlinux; \
	$(OBJCOPY) -O binary vmlinux vmlinux.bin; \
	$(HOSTTOOLS_DIR)/cmplzma -k -2 vmlinux vmlinux.bin vmlinux.lz;\
	$(HOSTTOOLS_DIR)/bcmImageBuilder --output $(FS_KERNEL_IMAGE_NAME) --chip $(BRCM_CHIP) --board $(BRCM_BOARD_ID) --blocksize $(BRCM_FLASHBLK_SIZE) --cfefile $(CFE_FILE) --rootfsfile rootfs.img --kernelfile vmlinux.lz; \
	$(HOSTTOOLS_DIR)/bcmImageBuilder --output $(CFE_FS_KERNEL_IMAGE_NAME) --chip $(BRCM_CHIP) --board $(BRCM_BOARD_ID) --blocksize $(BRCM_FLASHBLK_SIZE) --cfefile $(CFE_FILE) --rootfsfile rootfs.img --kernelfile vmlinux.lz --include-cfe; \
	$(HOSTTOOLS_DIR)/createimg --boardid=$(BRCM_BOARD_ID) --numbermac=$(BRCM_NUM_MAC_ADDRESSES) --macaddr=$(BRCM_BASE_MAC_ADDRESS) --tp=$(BRCM_MAIN_TP_NUM) --psisize=$(BRCM_PSI_SIZE) --inputfile=$(CFE_FS_KERNEL_IMAGE_NAME) --outputfile=$(FLASH_IMAGE_NAME); \
	$(HOSTTOOLS_DIR)/addvtoken $(FLASH_IMAGE_NAME) $(FLASH_IMAGE_NAME).w
	@mkdir -p $(IMAGES_DIR)
	@cp $(PROFILE_DIR)/$(FS_KERNEL_IMAGE_NAME) $(IMAGES_DIR)/$(FS_KERNEL_IMAGE_NAME)_$(BRCM_RELEASETAG)-$(shell date '+%y%m%d_%H%M')
	@echo
	@echo -e "Done! Image $(PROFILE) has been built in $(IMAGES_DIR)."
else
	cd $(PROFILE_DIR); \
	cp $(KERNEL_DIR)/vmlinux . ; \
	$(STRIP) --remove-section=.note --remove-section=.comment vmlinux
	@echo
	@echo -e "\t=== Following the below steps to start your NFS root file system on host ==="
	@echo -e "Step 1: Copy $(PROFILE_DIR)/vmlinux to your TFTP server boot directory, such as /tftpboot"
	@echo -e "Step 2: Add \"$(PROFILE_DIR)/fs *(rw,no_root_squash)\" to /etc/exports"
	@echo -e "Step 3: Become root and restart your nfs server, such as \"service nfs restart\""
	@echo -e "Step 4: Reboot your board and break into CFE bootloader, choose h on \"Run from flash/host\" and vmlinux on \"Default host run file name\""
endif


###########################################
#
# System code clean-up
#
###########################################

subdirs: $(patsubst %, _dir_%, $(SUBDIRS))

$(patsubst %, _dir_%, $(SUBDIRS)) :
	$(MAKE) -C $(patsubst _dir_%, %, $@) $(TGT)

clean: target_clean app_clean kernel_clean hosttools_clean  remove_6510_support
	rm -f .tmpconfig*

fssrc_clean:
	rm -fr $(FSSRC_DIR)/bin
	rm -fr $(FSSRC_DIR)/sbin
	rm -fr $(FSSRC_DIR)/lib
	rm -fr $(FSSRC_DIR)/upnp
	rm -fr $(FSSRC_DIR)/docs
	rm -fr $(FSSRC_DIR)/webs
	rm -fr $(FSSRC_DIR)/usr
	rm -fr $(FSSRC_DIR)/linuxrc
	rm -fr $(FSSRC_DIR)/images
	rm -fr $(FSSRC_DIR)/etc/wlan
	rm -fr $(FSSRC_DIR)/etc/certs

kernel_clean: sanity_check
	$(MAKE) -C $(KERNEL_DIR) mrproper
	rm -f $(KERNEL_DIR)/arch/mips/defconfig
	rm -f $(HOSTTOOLS_DIR)/lzma/decompress/*.o
	rm -rf $(XCHANGE_DIR)/dslx/lib/LinuxKernel
	rm -rf $(XCHANGE_DIR)/dslx/obj/LinuxKernel

app_clean: sanity_check app_expand_before_clean fssrc_clean wsc_clean
	$(MAKE) subdirs TGT=clean
	rm -rf $(XCHANGE_DIR)/dslx/lib/LinuxUser
	rm -rf $(XCHANGE_DIR)/dslx/obj/LinuxUser

wsc_clean:
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/bcmcrypto/Makefile ]; then \
		$(MAKE) -C $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/bcmcrypto clean; \
	fi
	if [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/shared/Makefile ] && [ -e $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/shared/UdpLib.c ]; then \
		$(MAKE) -C $(BRCMDRIVERS_DIR)/broadcom/net/wl/bcm9$(BRCM_CHIP)/shared clean; \
	fi
	if [ -e $(BROADCOM_DIR)/lltd/src/common.mk ]; then \
		$(MAKE) -C $(BROADCOM_DIR)/lltd clean; \
	fi
	if [ -e $(BROADCOM_DIR)/wsc/Wsccmd/src/linux/common.mk ]; then \
		$(MAKE) -C $(BROADCOM_DIR)/wsc/Wsccmd/src/linux clean; \
	fi

app_expand_before_clean:
	# these archives need to be expended before cleaning - put makefile structures there there for cleaning
	# OProfile
	(tar xkfj $(OPENSOURCE_DIR)/oprofile.tar.bz2 -C $(OPENSOURCE_DIR) 2> /dev/null || true)

target_clean: sanity_check
	rm -f $(PROFILE_DIR)/rootfs.img
	rm -f $(PROFILE_DIR)/vmlinux
	rm -f $(PROFILE_DIR)/vmlinux.bin
	rm -f $(PROFILE_DIR)/vmlinux.lz
	rm -f $(PROFILE_DIR)/$(FS_KERNEL_IMAGE_NAME)
	rm -f $(PROFILE_DIR)/$(CFE_FS_KERNEL_IMAGE_NAME)
	rm -f $(PROFILE_DIR)/$(FLASH_IMAGE_NAME)
	rm -f $(PROFILE_DIR)/$(FLASH_IMAGE_NAME).w
	rm -f $(PROFILE_DIR)/*.srec
	rm -fr $(PROFILE_DIR)/modules
	rm -fr $(PROFILE_DIR)/op
	rm -fr $(INSTALL_DIR)
	find targets -name vmlinux -print -exec rm -f "{}" ";"
ifeq ($(strip $(BRCM_KERNEL_ROOTFS)),squashfs)
	rm -fr $(TARGET_FS)
else
ifeq ($(BRCM_BUILD_USR),root)
	su --command="rm -fr $(TARGET_FS)"
else 
	sudo rm -fr $(TARGET_FS)/*; rm -rf $(TARGET_FS)
endif
endif

hosttools_clean:
	$(MAKE) -C $(HOSTTOOLS_DIR) clean

add_6510_support: 
	$(6510_REF_CODE)/add6510support DYMMY_HMI_VERSON $(6510_REF_CODE)
	
remove_6510_support:
	rm -fr $(BROADCOM_DIR)/hostCode
	rm -fr $(BROADCOM_DIR)/relayCtl
	rm -fr $(BROADCOM_DIR)/vdslctl

clean_6510:
	rm -f `find userapps/broadcom/hostCode/ -name *.[oa]`
	rm -f `find userapps/broadcom/hostCode/ -name *.depend`
	rm -f `find userapps/broadcom/relayCtl/ -name *.[oa]`
	rm -f `find userapps/broadcom/relayCtl/ -name *.depend`
	rm -f `find userapps/broadcom/vdslctl/  -name *.[oa]`
	rm -f `find userapps/broadcom/vdslctl/  -name *.depend`
	rm -f `find userapps/broadcom/cfm/      -name *.[oa]`
	rm -f `find userapps/broadcom/cfm/      -name *.depend`

voice_clean:
	find bcmdrivers/broadcom/char/endpoint -name '*.o' -exec rm -f "{}" ";"
	find bcmdrivers/broadcom/char/endpoint -name '*.ko' -exec rm -f "{}" ";"
	find bcmdrivers/broadcom/char/dspapp -name '*.o' -exec rm -f "{}" ";"
	find bcmdrivers/broadcom/char/dspapp -name '*.ko' -exec rm -f "{}" ";"
	find userapps/broadcom/cfm/main/vodslcfgmngr -name '*.o' -exec rm -f "{}" ";"
	find userapps/broadcom/cfm/web -name '*.o' -exec rm -f "{}" ";"
	find userapps/broadcom/vodsl -name '*.o' -exec rm -f "{}" ";"
	rm -rf $(XCHANGE_DIR)/dslx/lib/LinuxKernel 
	rm -rf $(XCHANGE_DIR)/dslx/obj/LinuxKernel
	rm -rf $(XCHANGE_DIR)/dslx/lib/LinuxUser
	rm -rf $(XCHANGE_DIR)/dslx/obj/LinuxUser

###########################################
#
# System-wide exported variables
# (in alphabetical order)
#
###########################################

export \
AR                         \
AS                         \
BRCM_APP_PHONE             \
BRCMAPPS                   \
BRCM_BOARD                 \
BRCM_DRIVER_PCI            \
BRCMDRIVERS_DIR            \
BRCM_DSP_APM_FXO           \
BRCM_DSP_APM_FXO_EXT       \
BRCM_DSP_CODEC_G711        \
BRCM_DSP_CODEC_G723        \
BRCM_DSP_CODEC_G726        \
BRCM_DSP_CODEC_G729        \
BRCM_DSP_CODEC_G7xx        \
BRCM_DSP_CODEC_T38_EXT     \
BRCM_DSP_CODEC_T38_INT     \
BRCM_DSP_HAL               \
BRCM_DSP_HAL_EXTENSION     \
BRCM_DSP_PCM               \
BRCM_DSP_PCM_G726          \
BRCM_DSP_PCM_T38_EXT       \
BRCM_EXTRAVERSION          \
BRCM_KERNEL_NETQOS         \
BRCM_KERNEL_ROOTFS         \
BRCM_KERNEL_OPROFILE       \
BRCM_LDX_APP               \
BRCM_MIPS_ONLY_BUILD       \
BRCM_MIPS_ONLY_BUILD       \
BRCM_PSI_VERSION           \
BRCM_PTHREADS              \
BRCM_RELEASE               \
BRCM_RELEASETAG            \
BRCM_SNMP                  \
BRCM_UCLIBC                \
BRCM_VERSION               \
BRCM_VODSL_DUAL_3341       \
BRCM_VODSL_SRTP            \
BRCM_VOICE_COUNTRY_JAPAN   \
BRCM_VOICE_GLOBAL_CFLAGS   \
BRCM_VOIP_TO_FXO           \
BROADCOM_CFM_DIR           \
BUILD_ADSLCTL              \
BUILD_ATMCTL               \
BUILD_BR2684CTL            \
BUILD_BRCM_VLAN            \
BUILD_BRCTL                \
BUILD_BUSYBOX              \
BUILD_CERT                 \
BUILD_CFM                  \
BUILD_CFM_CLI              \
BUILD_CFM_SSHD             \
BUILD_CFM_TELNETD          \
BUILD_DDNSD                \
BUILD_DHCPR                \
BUILD_DIAGAPP              \
BUILD_DIR                  \
BUILD_DNSPROBE             \
BUILD_DPROXY               \
BUILD_DYNAHELPER           \
BUILD_DNSSPOOF             \
BUILD_EBTABLES             \
BUILD_EPITTCP              \
BUILD_ETHWAN               \
BUILD_FTPD                 \
BUILD_FTPD_STORAGE         \
BUILD_GDBSERVER            \
BUILD_IGMP                 \
BUILD_IPPD                 \
BUILD_IPROUTE2             \
BUILD_IPSEC_TOOLS          \
BUILD_IPTABLES             \
BUILD_WPS_BTN              \
BUILD_LLTD                 \
BUILD_WSC                   \
BUILD_BCMCRYPTO \
BUILD_BCMSHARED \
BUILD_NAS                  \
BUILD_NETCTL               \
BUILD_NVRAM                \
BUILD_OPROFILE             \
BUILD_PORT_MIRRORING			 \
BUILD_PPPD                 \
BUILD_PVC2684CTL           \
BUILD_REAIM                \
BUILD_RT2684D              \
BUILD_SES                  \
BUILD_SIPROXD              \
BUILD_SLACTEST             \
BUILD_SNMP                 \
BUILD_SNTP                 \
BUILD_SOAP                 \
BUILD_SOAP_VER             \
BUILD_SSHD_MIPS_GENKEY     \
BUILD_TOD                  \
BUILD_TR64                 \
BUILD_TR64_DEVICECONFIG    \
BUILD_TR64_DEVICEINFO      \
BUILD_TR64_LANCONFIGSECURITY \
BUILD_TR64_LANETHINTERFACECONFIG \
BUILD_TR64_LANHOSTS        \
BUILD_TR64_LANHOSTCONFIGMGMT \
BUILD_TR64_LANUSBINTERFACECONFIG \
BUILD_TR64_LAYER3          \
BUILD_TR64_MANAGEMENTSERVER  \
BUILD_TR64_TIME            \
BUILD_TR64_USERINTERFACE   \
BUILD_TR64_QUEUEMANAGEMENT \
BUILD_TR64_LAYER2BRIDGE   \
BUILD_TR64_WANCABLELINKCONFIG \
BUILD_TR64_WANCOMMONINTERFACE \
BUILD_TR64_WANDSLINTERFACE \
BUILD_TR64_WANDSLLINKCONFIG \
BUILD_TR64_WANDSLCONNECTIONMGMT \
BUILD_TR64_WANDSLDIAGNOSTICS \
BUILD_TR64_WANETHERNETCONFIG \
BUILD_TR64_WANETHERNETLINKCONFIG \
BUILD_TR64_WANIPCONNECTION \
BUILD_TR64_WANPOTSLINKCONFIG \
BUILD_TR64_WANPPPCONNECTION \
BUILD_TR64_WLANCONFIG      \
BUILD_TR69C                \
BUILD_TR69_QUEUED_TRANSFERS \
BUILD_TR69C_SSL            \
BUILD_TR69_XBRCM           \
BUILD_TR69_UPLOAD          \
BUILD_UDHCP                \
BUILD_UPNP                 \
BUILD_VCONFIG              \
BUILD_SUPERDMZ             \
BUILD_VODSL                \
BUILD_SIP_TLS              \
BUILD_WLCTL                \
BUILD_ZEBRA                \
BUILD_LIBUSB               \
BUILD_WANVLANMUX           \
BUSYBOX_DIR                \
CC                         \
CROSS_COMPILE              \
CXX                        \
DEFAULTCFG_DIR             \
FSSRC_DIR                  \
HOSTTOOLS_DIR              \
INC_ATMBONDING_ETH_DRV_PATH    \
INC_ATMBONDING_DRV_PATH    \
INC_ATMAPI_DRV_PATH        \
INC_ADSLDRV_PATH           \
INC_BRCMBOARDPARMS_PATH    \
INC_BRCMCFM_PATH           \
INC_BRCMDRIVER_PRIV_PATH   \
INC_BRCMDRIVER_PUB_PATH    \
INC_BRCMSHARED_PRIV_PATH   \
INC_BRCMSHARED_PUB_PATH    \
INC_FLASH_PATH             \
INC_ENDPOINT_PATH          \
INC_KERNEL_BASE            \
INSTALL_DIR                \
JTAG_KERNEL_DEBUG          \
KERNEL_DIR                 \
LD                         \
LIBCDIR                    \
LIBDIR                     \
LIB_PATH                   \
LINUXDIR                   \
LOCALEFLAGS                \
LOCALEFLAGS_TYPE2          \
NM                         \
OBJCOPY                    \
OBJDUMP                    \
PROFILE_DIR                \
RANLIB                     \
RUN_NOISE                  \
SSTRIP                     \
STRIP                      \
TARGETS_DIR                \
TOOLCHAIN                  \
USERAPPS_DIR               \
WEB_POPUP                  \
HTTPS_SUPPORT              \
XCHANGE_DIR                \
XCHANGE_DSP_APP_EXTENSION  \
VCOPE_TYPE                 \
VCOPE_BOARD                \
VCOPE_LINE_NUMBER          \
BRCM_DRIVER_ATM_BONDING_DEFINES \
BCM_VDSL_TYPE_DEFINES \
BUILD_VIRT_SRVR			   \
BUILD_PORT_TRIG			   \
BUILD_TR69C_BCM_SSL		   \
BUILD_IPV6                \
BUILD_MULTISUBNET         \
LINUX_KERNEL_USBMASS      \
PPP_AUTODISCONN           \
DARE_SHOW_CUSTOMERVER      \
DARE_SHOW_CUSTOMERMODEL    \
DARE_SHOW_CUSTOMERCFEVER   \
DARE_DHCP_IN_BRIDGE       \
DARE_CUSTOMER_MODEL        \
DARE_CUSTOMER_VER          \
DARE_CUSTOMER_CFE_VER      \
DARE_MANUAL_PPP            \
DARE_PPP_MTU               \
DARE_PPP_MTU_DFTVAL        \
DARE_TRACE_PING            \
DARE_REDUCE_HTML            \
BUILD_PPTP                 \

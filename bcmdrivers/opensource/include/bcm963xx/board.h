/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/
/***********************************************************************/
/*                                                                     */
/*   MODULE:  board.h                                                  */
/*   PURPOSE: Board specific information.  This module should include  */
/*            all base device addresses and board specific macros.     */
/*                                                                     */
/***********************************************************************/
#ifndef _BOARD_H
#define _BOARD_H

#include "bcm_hwdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*          board ioctl calls for flash, led and some other utilities        */
/*****************************************************************************/
/* Defines. for board driver */
#define BOARD_IOCTL_MAGIC       'B'
#define BOARD_DRV_MAJOR          206

#define BOARD_IOCTL_FLASH_WRITE         _IOWR(BOARD_IOCTL_MAGIC, 0, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_FLASH_READ          _IOWR(BOARD_IOCTL_MAGIC, 1, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_DUMP_ADDR           _IOWR(BOARD_IOCTL_MAGIC, 2, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_MEMORY          _IOWR(BOARD_IOCTL_MAGIC, 3, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_MIPS_SOFT_RESET     _IOWR(BOARD_IOCTL_MAGIC, 4, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_LED_CTRL            _IOWR(BOARD_IOCTL_MAGIC, 5, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_PSI_SIZE        _IOWR(BOARD_IOCTL_MAGIC, 6, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_SDRAM_SIZE      _IOWR(BOARD_IOCTL_MAGIC, 7, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_ID              _IOWR(BOARD_IOCTL_MAGIC, 8, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_CHIP_ID         _IOWR(BOARD_IOCTL_MAGIC, 9, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_CHIP_REV        _IOWR(BOARD_IOCTL_MAGIC, 10, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_CFE_VER         _IOWR(BOARD_IOCTL_MAGIC, 11, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_BASE_MAC_ADDRESS _IOWR(BOARD_IOCTL_MAGIC, 12, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_MAC_ADDRESS     _IOWR(BOARD_IOCTL_MAGIC, 13, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_RELEASE_MAC_ADDRESS _IOWR(BOARD_IOCTL_MAGIC, 14, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_ENET_MACS   _IOWR(BOARD_IOCTL_MAGIC, 15, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_NUM_ENET_PORTS  _IOWR(BOARD_IOCTL_MAGIC, 16, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_MONITOR_FD      _IOWR(BOARD_IOCTL_MAGIC, 17, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_WAKEUP_MONITOR_TASK _IOWR(BOARD_IOCTL_MAGIC, 18, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_TRIGGER_EVENT   _IOWR(BOARD_IOCTL_MAGIC, 19, BOARD_IOCTL_PARMS)        
#define BOARD_IOCTL_GET_TRIGGER_EVENT   _IOWR(BOARD_IOCTL_MAGIC, 20, BOARD_IOCTL_PARMS)        
#define BOARD_IOCTL_UNSET_TRIGGER_EVENT _IOWR(BOARD_IOCTL_MAGIC, 21, BOARD_IOCTL_PARMS) 
#define BOARD_IOCTL_GET_WLAN_ANT_INUSE  _IOWR(BOARD_IOCTL_MAGIC, 22, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_SES_LED         _IOWR(BOARD_IOCTL_MAGIC, 23, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_GET_VCOPE_GPIO      _IOWR(BOARD_IOCTL_MAGIC, 24, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_CS_PAR          _IOWR(BOARD_IOCTL_MAGIC, 25, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_SET_GPIO            _IOWR(BOARD_IOCTL_MAGIC, 26, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_QUERY_RESET_DEFAULT_REQUEST  _IOWR(BOARD_IOCTL_MAGIC, 27, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_WIRELESS_PROTECT    _IOWR(BOARD_IOCTL_MAGIC, 28, BOARD_IOCTL_PARMS)
#define BOARD_IOCTL_BYPASS_SIGNATURE    _IOWR(BOARD_IOCTL_MAGIC, 29, BOARD_IOCTL_PARMS)
    
// for the action in BOARD_IOCTL_PARMS for flash operation
typedef enum 
{
    PERSISTENT,
    NVRAM,
    BCM_IMAGE_CFE,
    BCM_IMAGE_FS,
    BCM_IMAGE_KERNEL,
    BCM_IMAGE_WHOLE,
    SCRATCH_PAD,
    FLASH_SIZE,
    SET_CS_PARAM,
    DAREPERSISTENT,  /* dare sunmin dual config backup */
} BOARD_IOCTL_ACTION;
    
typedef struct boardIoctParms
{
    char *string;
    char *buf;
    int strLen;
    int offset;
    BOARD_IOCTL_ACTION  action;        /* flash read/write: nvram, persistent, bcm image */
    int result;
} BOARD_IOCTL_PARMS;

// LED defines 
typedef enum
{   
    kLedAdsl,
    kLedSecAdsl,
    kLedHpna,
    kLedWanData,
    kLedPPP,
    kLedSes,
    kLedEphyLinkSpeed,
    kLedVoip,
    kLedVoip1,
    kLedVoip2,
    kLedPots,
    kLedDect,
    kLedUsb1,  /* dare sunmin for USB led */
    kLedUsb2,
    kLedEnd,                // NOTE: Insert the new led name before this one.  Alway stay at the end.
} BOARD_LED_NAME;

typedef enum
{
    kLedStateOff,                        /* turn led off */
    kLedStateOn,                         /* turn led on */
    kLedStateFail,                       /* turn led on red */
    kLedStateBlinkOnce,                  /* blink once, ~100ms and ignore the same call during the 100ms period */
    kLedStateSlowBlinkContinues,         /* slow blink continues at ~600ms interval */
    kLedStateFastBlinkContinues,         /* fast blink continues at ~200ms interval */
} BOARD_LED_STATE;

typedef enum
{
  GPIO_LOW = 0,
  GPIO_HIGH,
} GPIO_STATE_t;

// virtual and physical map pair defined in board.c
typedef struct ledmappair
{
    BOARD_LED_NAME ledName;         // virtual led name
    BOARD_LED_STATE ledInitState;   // initial led state when the board boots.
    unsigned long ledMask;         // physical GPIO pin mask
    unsigned short ledActiveLow;    // reset bit to turn on LED
    unsigned short ledSerial;       // indicated that LED is driven via serial output
    unsigned long ledMaskFail;     // physical GPIO pin mask for state failure
    unsigned short ledActiveLowFail;// reset bit to turn on LED
    unsigned short ledSerialFail;   // indicated that LED is driven via serial output
} LED_MAP_PAIR, *PLED_MAP_PAIR;

typedef void (*HANDLE_LED_FUNC)(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState);

/* Flash storage address information that is determined by the flash driver. */
typedef struct flashaddrinfo
{
    int flash_persistent_start_blk;
    int flash_persistent_number_blk;
    int flash_persistent_length;
    unsigned long flash_persistent_blk_offset;
    int flash_scratch_pad_start_blk;         // start before psi (SP_BUF_LEN)
    int flash_scratch_pad_number_blk;
    int flash_scratch_pad_length;
    unsigned long flash_scratch_pad_blk_offset;
    int flash_nvram_start_blk;
    int flash_nvram_number_blk;
    int flash_nvram_length;
    unsigned long flash_nvram_blk_offset;
    unsigned long flash_rootfs_start_offset;
    int flash_dare_persistent_start_blk;  // dare sunmin dual config backup
    int flash_dare_persistent_number_blk;
    int flash_dare_persistent_length;
    unsigned long flash_dare_persistent_blk_offset;
} FLASH_ADDR_INFO, *PFLASH_ADDR_INFO;

// scratch pad defines
/* SP - Persisten Scratch Pad format:
       sp header        : 32 bytes
       tokenId-1        : 8 bytes
       tokenId-1 len    : 4 bytes
       tokenId-1 data    
       ....
       tokenId-n        : 8 bytes
       tokenId-n len    : 4 bytes
       tokenId-n data    
*/

#define MAGIC_NUM_LEN       8
#define MAGIC_NUMBER        "gOGoBrCm"
#define TOKEN_NAME_LEN      16
#define SP_VERSION          1
#define SP_MAX_LEN          8 * 1024            // 8k buf before psi
#define SP_RESERVERD        20

typedef struct _SP_HEADER
{
    char SPMagicNum[MAGIC_NUM_LEN];             // 8 bytes of magic number
    int SPVersion;                              // version number
    char SPReserved[SP_RESERVERD];              // reservied, total 32 bytes
} SP_HEADER, *PSP_HEADER;

typedef struct _TOKEN_DEF
{
    char tokenName[TOKEN_NAME_LEN];
    int tokenLen;
} SP_TOKEN, *PSP_TOKEN;

typedef struct cs_config_pars_tag
{
  int   mode;
  int   base;
  int   size;
  int   wait_state;
  int   setup_time;
  int   hold_time;
} cs_config_pars_t;

#define MAC_ADDRESS_ANY         (unsigned long) -1

/* dare sunmin for USB led */
typedef struct dare_usbinfo
{
    int bup;
    char info_string[64];
} DARE_USBINFO,*PDARE_USBINFO;



/*****************************************************************************/
/*          Function Prototypes                                              */
/*****************************************************************************/
#if !defined(__ASM_ASM_H)
void dumpaddr( unsigned char *pAddr, int nLen );

extern void kerSysEarlyFlashInit( void );
extern void kerSysFlashInit( void );
extern void kerSysFlashAddrInfoGet(PFLASH_ADDR_INFO pflash_addr_info);
extern int kerSysNvRamGet(char *string, int strLen, int offset);
extern int kerSysNvRamSet(char *string, int strLen, int offset);
extern int kerSysPersistentGet(char *string, int strLen, int offset);
extern int kerSysPersistentSet(char *string, int strLen, int offset);
extern int kerSysScratchPadGet(char *tokName, char *tokBuf, int tokLen);
extern int kerSysScratchPadSet(char *tokName, char *tokBuf, int tokLen);
extern int kerSysScratchPadClearAll(void);
extern int kerSysBcmImageSet( int flash_start_addr, char *string, int size);
extern int kerSysGetMacAddress( unsigned char *pucaAddr, unsigned long ulId );
extern int kerSysReleaseMacAddress( unsigned char *pucaAddr );
extern int kerSysGetSdramSize( void );
extern void kerSysGetBootline(char *string, int strLen);
extern void kerSysSetBootline(char *string, int strLen);
extern void kerSysMipsSoftReset(void);
extern void kerSysLedCtrl(BOARD_LED_NAME, BOARD_LED_STATE);
extern void kerSysLedRegisterHwHandler( BOARD_LED_NAME, HANDLE_LED_FUNC, int );
extern int kerSysFlashSizeGet(void);
extern int kerSysMemoryMappedFlashSizeGet(void);
extern unsigned long kerSysReadFromFlash( void *toaddr, unsigned long fromaddr, unsigned long len );
extern void kerSysRegisterDyingGaspHandler(char *devname, void *cbfn, void *context);
extern void kerSysDeregisterDyingGaspHandler(char *devname);    
extern void kerSysWakeupMonitorTask( void );
extern int kerConfigCs(BOARD_IOCTL_PARMS *parms);
extern void kerSetGpio(int gpio, GPIO_STATE_t state);
//dare for customer settings.
int kerSysDarePersistentGet(char *string, int strLen, int offset); 
int kerSysDarePersistentSet(char *string, int strLen, int offset);
/* dare sunmin for usb led */
int kerSetUsbInfo(int index, int bup, char *info);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H */


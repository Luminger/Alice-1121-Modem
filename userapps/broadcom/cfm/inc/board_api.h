/*
<:copyright-gpl
 Copyright 2004 Broadcom Corp. All Rights Reserved.

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

#if !defined(_BOARD_API_H_)
#define _BOARD_API_H_

#include <board.h>

#if defined(__cplusplus)
extern "C" {
#endif

int sysScratchPadSet(char *tokenId, char *tokBuf, int bufLen);
int sysScratchPadGet(char *tokenId, char *tokBuf, int bufLen);
int sysScratchPadClearAll(void);
int sysPersistentGet(char *string,int strLen,int offset);
int sysPersistentSet(char *string,int strLen,int offset);
int sysFlashImageSet(void *image, int size, int addr,
    BOARD_IOCTL_ACTION imageType);
int sysDumpAddr(char *addr, int len);
int sysSetMemory(char *addr, int size, unsigned long value );
void sysMipsSoftReset(void);
int sysGetBoardIdName(char *name, int length);
int sysGetBaseMacAddress(unsigned char *pucaAddr);
int sysGetMacAddress( unsigned char *pucaAddr, unsigned long ulId );
int sysReleaseMacAddress( unsigned char *pucaAddr );
int sysGetSdramSize( void );
int sysGetPsiSize( void );
void sysLedCtrl(BOARD_LED_NAME, BOARD_LED_STATE);
int sysFlashSizeGet(void);
int sysGetChipId(void);
int sysGetChipRev(void);
int sysGetNumEnetMacs(void);
int sysGetNumEnetPorts(void);
int sysGetCFEVersion(char *string, int strLen);
int sysSetMonitorFd(int fd);
void sysWakeupMonitorTask(void);
int sysGetVcopeInfo(int info_type);
int sysConfigCs(int cs_number, void *info);
int sysSetGpioBit(int gpio_bit, GPIO_STATE_t flag);
//sunmin dare wireless protect
int sysGetWirelessProtectState(void);
/* dare sunmin add NVRAM operation functaion */
int sysNvRamSet(char *string,int strLen,int offset);
int sysNvRamGet(char *string,int strLen,int offset);
/* dare sunmin dual config backup */
int sysDarePersistentGet(char *string,int strLen,int offset);
int sysDarePersistentSet(char *string,int strLen,int offset);
 /* dare sunmin fix switch will hange after reboot */
void dare_remove_vnet();

#if defined(__cplusplus)
}
#endif

#endif /* _BOARD_API_H_ */


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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <memory.h>

#include <bcmtypes.h>
#include <board_api.h>
#include <syscall.h>  /* dare sunmin fix switch will hange after reboot */
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

int boardIoctl(int board_ioctl, BOARD_IOCTL_ACTION action, char *string, int strLen, int offset, char *buf)
{
    BOARD_IOCTL_PARMS IoctlParms;
    int boardFd = 0;

    boardFd = open("/dev/brcmboard", O_RDWR);
    if ( boardFd != -1 ) {
        IoctlParms.string = string;
        IoctlParms.strLen = strLen;
        IoctlParms.offset = offset;
        IoctlParms.action = action;
        IoctlParms.buf    = buf;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
        boardFd = IoctlParms.result;
    } else
        printf("Unable to open device /dev/brcmboard.\n");

    return boardFd;
}
#ifdef USE_ALL

/*****************************************************************************
* sysScratchPadGet - get the contents of persistent stratch pad flash memory
* INPUT:   tokenId -- token id, ASCIZ tokBuf (up to 15 char)
*          tokBuf  -- buf 
*          bufLen  -- buf len
* RETURNS: 0 - ok, -1 fail.
*/
int sysScratchPadGet(char *tokenId, char *tokBuf, int bufLen)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_READ, SCRATCH_PAD, tokenId, bufLen, bufLen, tokBuf));
}

/*****************************************************************************
* sysScratchPadSet - write the contents of persistent scratch pad flash memory
* INPUT:   tokenId -- token id, ASCIZ tokBuf (up to 15 char)
*          tokBuf  -- buf 
*          bufLen  -- buf len
* RETURNS: 0 - ok, -1 fail.
*/
int sysScratchPadSet(char *tokenId, char *tokBuf, int bufLen)
{
    char *buf = malloc(bufLen);

    //If the data associated with the tokenId already exists in the scratch pad,
    //we don't want to set it again.
	//Note that writing to the scratch pad is a non-preemptive time consuming
	//operation that should be avoided.
    if (buf != NULL &&
        boardIoctl(BOARD_IOCTL_FLASH_READ, SCRATCH_PAD, tokenId, bufLen, bufLen, buf) >= 0) {
        if (bcmp(buf, tokBuf, bufLen) == 0) {
            free(buf);
            return 0;
        }
    }
    if (buf != NULL) free(buf);

    return (boardIoctl(BOARD_IOCTL_FLASH_WRITE, SCRATCH_PAD, tokenId, bufLen, bufLen, tokBuf));
}

/*****************************************************************************
* sysScratchPadClearAll  - wipeout the scratch pad
* RETURNS: 0 - ok, -1 fail.
*/
int sysScratchPadClearAll(void)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_WRITE, SCRATCH_PAD, "", -1, -1, ""));
}

/*****************************************************************************
* sysDumpAddr - Dump kernel memory.
* RETURNS: OK, always.
*/
int sysDumpAddr(char *addr, int len)
{
    return (boardIoctl(BOARD_IOCTL_DUMP_ADDR, 0, addr, len, 0, ""));
}

/*****************************************************************************
* sysDumpAddr - Set kernel memory.
* RETURNS: OK, always.
*/
int sysSetMemory(char *addr, int size, unsigned long value )
{
    return (boardIoctl(BOARD_IOCTL_SET_MEMORY, 0, addr, size, (int) value, ""));
}

/*****************************************************************************
 * image points to image to be programmed to flash; size is the size (in bytes)
 * of the image.
 * if error, return -1; otherwise return 0
 */

#endif // USE_ALL

/*****************************************************************************
* sysPersistentGet - get the contents of persistent flash memory
* RETURNS: OK, always.
*/
int sysPersistentGet(char *string, int strLen, int offset)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_READ, PERSISTENT, string, strLen, offset, ""));
}

/*****************************************************************************
* sysPersistenSet - write the contents of persistent Scrach Pad flash memory
* RETURNS: OK, always.
*/
int sysPersistentSet(char *string, int strLen, int offset)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_WRITE, PERSISTENT, string, strLen, offset, ""));
}

//********************************************************************************
// Get PSI size
//********************************************************************************
int sysGetPsiSize( void )
{
    return( boardIoctl(BOARD_IOCTL_GET_PSI_SIZE, 0, "", 0, 0, "") );
}

int sysFlashImageSet(void *image, int size, int addr,
    BOARD_IOCTL_ACTION imageType)
{
    int result;

    result = boardIoctl(BOARD_IOCTL_FLASH_WRITE, imageType, image, size, addr, "");

    return(result);
}

/*****************************************************************************
 * Get flash size 
 * return int flash size
 */
int sysFlashSizeGet(void)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_READ, FLASH_SIZE, "", 0, 0, ""));
}

/*****************************************************************************
* kerSysMipsSoftReset - soft reset the mips. (reboot, go to 0xbfc00000)
* RETURNS: NEVER
*/
void sysMipsSoftReset(void)
{  

#ifdef SUPPORT_PORT_MAP
    /* dare sunmin fix switch will hange after reboot */
	  dare_remove_vnet();
#endif

    boardIoctl(BOARD_IOCTL_MIPS_SOFT_RESET, 0, "", 0, 0, "");
}

//********************************************************************************
// Get Chip Id
//********************************************************************************
int sysGetChipId( void )
{
    return( boardIoctl(BOARD_IOCTL_GET_CHIP_ID, 0, "", 0, 0, "") );
}

//********************************************************************************
// Get Chip REV
//********************************************************************************
int sysGetChipRev( void )
{
    return( boardIoctl(BOARD_IOCTL_GET_CHIP_REV, 0, "", 0, 0, "") );
}

//********************************************************************************
// Wakeup monitor task
//********************************************************************************
void sysWakeupMonitorTask(void)
{
    boardIoctl(BOARD_IOCTL_WAKEUP_MONITOR_TASK, 0, "", 0, 0, "");
}

#ifdef USE_ALL
//********************************************************************************
// LED status display:  ADSL link: DOWN/UP, PPP: DOWN/STARTING/UP
//********************************************************************************
void sysLedCtrl(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState)
{
    boardIoctl(BOARD_IOCTL_LED_CTRL, 0, "", (int)ledName, (int)ledState, "");
}


//********************************************************************************
// Get board id
//********************************************************************************
int sysGetBoardIdName(char *name, int length)
{
    return( boardIoctl(BOARD_IOCTL_GET_ID, 0, name, length, 0, "") );
}

//********************************************************************************
// Get MAC Address
//********************************************************************************
int sysGetBaseMacAddress( unsigned char *pucaAddr )
{
    return(boardIoctl(BOARD_IOCTL_GET_BASE_MAC_ADDRESS, 0, pucaAddr, 6, 0, ""));
}

//********************************************************************************
// Get MAC Address
//********************************************************************************
int sysGetMacAddress( unsigned char *pucaAddr, unsigned long ulId )
{
    return(boardIoctl(BOARD_IOCTL_GET_MAC_ADDRESS, 0, pucaAddr, 6, (int) ulId, ""));
}

//********************************************************************************
// Release MAC Address
//********************************************************************************
int sysReleaseMacAddress( unsigned char *pucaAddr )
{
    return( boardIoctl(BOARD_IOCTL_RELEASE_MAC_ADDRESS, 0, pucaAddr, 6, 0, "") );
}

//********************************************************************************
// Get SDRAM size
//********************************************************************************
int sysGetSdramSize( void )
{
    return( boardIoctl(BOARD_IOCTL_GET_SDRAM_SIZE, 0, "", 0, 0, "") );
}

//********************************************************************************
// Get number of Ethernet MACs
//********************************************************************************
int sysGetNumEnetMacs(void)
{
    return(boardIoctl(BOARD_IOCTL_GET_NUM_ENET_MACS, 0, "", 0, 0, ""));
}

//********************************************************************************
// Get number of Ethernet ports
//********************************************************************************
int sysGetNumEnetPorts(void)
{
    return(boardIoctl(BOARD_IOCTL_GET_NUM_ENET_PORTS, 0, "", 0, 0, ""));
}

//********************************************************************************
// Get CFE vesion info
//********************************************************************************
int sysGetCFEVersion(char *string, int strLen)
{
    return (boardIoctl(BOARD_IOCTL_GET_CFE_VER, 0, string, strLen, 0, ""));
}

//********************************************************************************
// Set monitor loop file descriptor
//********************************************************************************
int sysSetMonitorFd(int fd)
{
    return (boardIoctl(BOARD_IOCTL_SET_MONITOR_FD, 0, "", 0, fd, ""));
}

//******************************************************************************
// Get VCOPE board information: cs, gpio, board revision
//******************************************************************************
int sysGetVcopeInfo(int info_type)
{
    return(boardIoctl(BOARD_IOCTL_GET_VCOPE_GPIO, 0, "", 0, info_type, ""));
}

//******************************************************************************
// Configure Chip Select, by setting given parameter to a passed value
//******************************************************************************
int sysConfigCs (int cs_number, void *cs_info)
{
    return(boardIoctl(BOARD_IOCTL_SET_CS_PAR, 0, "", 0, cs_number, cs_info));
}

//******************************************************************************
// Configure GPIO bit according to the passed values
//******************************************************************************
int  sysSetGpioBit (int gpio_bit, GPIO_STATE_t flag)
{
    return(boardIoctl(BOARD_IOCTL_SET_GPIO, 0, "", gpio_bit, flag, ""));
}

#endif // USE_ALL
/* dare sunmin fix switch will hang after reboot. */
int sysGetResetDefaultRequest( void )
{
    return( boardIoctl(BOARD_IOCTL_QUERY_RESET_DEFAULT_REQUEST, 0, "", 0, 0, "") );
}


//dare sunmin add NVRAM operation functaion
/*****************************************************************************
* sysPersistentGet - get the contents of non-volatile RAM,
* RETURNS: OK, always.
*/
int sysNvRamGet(char *string, int strLen, int offset)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_READ, NVRAM, string, strLen, offset, ""));
}



/*****************************************************************************
* sysPersistentSet - write the contents of non-volatile RAM
* RETURNS: OK, always.
*/
int sysNvRamSet(char *string, int strLen, int offset)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_WRITE, NVRAM, string, strLen, offset, ""));
}


/* dare sunmin dual config backup */
int sysDarePersistentGet(char *string, int strLen, int offset)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_READ, DAREPERSISTENT, string, strLen, offset, ""));
}

int sysDarePersistentSet(char *string, int strLen, int offset)
{
    return (boardIoctl(BOARD_IOCTL_FLASH_WRITE, DAREPERSISTENT, string, strLen, offset, ""));
}

void sysDareSignatureBypass(void)
{
    boardIoctl(BOARD_IOCTL_BYPASS_SIGNATURE, 0, "", 0, 0, "");
}

/* dare sunmin dual config backup end */

#ifdef SUPPORT_PORT_MAP
/* dare sunmin fix switch will hange after reboot */
void sysTem(char* cmd)
{
    extern char **environ;
    int pid = 0, status = 0;

  if (cmd== NULL)
  	return;
  
   pid = fork();
   if ( pid == -1 )
      return ;

   if ( pid == 0 ) {
      char *argv[4];
      argv[0] = "sh";
      argv[1] = "-c";
      argv[2] = cmd;
      argv[3] = 0;
      execve("/bin/sh", argv, environ);
      exit(127);
   }

   /* wait for child process return */
   do {
      if ( waitpid(pid, &status, 0) == -1 ) {
         if ( errno != EINTR )
            return ;
      } else
         return;
   } while ( 1 );
}


void dare_remove_vnet()
{
   /* dare sunmin fix switch will hange after reboot */
   int i;
   char cmd[64];
   static char *portName[] = {"eth0.2","eth0.3","eth0.4","eth0.5","eth1.2","eth1.3","eth1.4","eth1.5"};
   
   for(i = 0;i<8;i++)
   {
      sprintf(cmd,"ifconfig %s down 2>/dev/null",portName[i]);
      sysTem(cmd);
      //printf("#####cmd:%s\n",cmd);
   }
   
   sprintf(cmd, "rmmod vnet 2>/dev/null");
   sysTem(cmd);
}

//sunmin dare wireless protect
int sysGetWirelessProtectState()
{
    return(boardIoctl(BOARD_IOCTL_WIRELESS_PROTECT, 0, "", 0, 0, ""));
}

#endif

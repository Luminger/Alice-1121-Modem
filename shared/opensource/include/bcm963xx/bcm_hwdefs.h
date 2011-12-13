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
/*   MODULE:  bcm_hwdefs.h                                             */
/*   PURPOSE: Define all base device addresses and HW specific macros. */
/*                                                                     */
/***********************************************************************/
#ifndef _BCM_HWDEFS_H
#define _BCM_HWDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#define	DYING_GASP_API

/*****************************************************************************/
/*                    Physical Memory Map                                    */
/*****************************************************************************/

#define PHYS_DRAM_BASE          0x00000000      /* Dynamic RAM Base */
#define PHYS_FLASH_BASE         0x1FC00000      /* Flash Memory     */

#define HPI_BASE	            0x1D000000      /* VCOPE base address */
#define HPI_SIZE	            0x8000          /* 32K */

/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE           (0x80000000 | PHYS_DRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE   (0xA0000000 | PHYS_DRAM_BASE)   /* uncached DRAM */

/* Binary images are always built for a standard MIPS boot address */
#define IMAGE_BASE          (0xA0000000 | PHYS_FLASH_BASE)

/* Some chips don't support alternative boot vector */
#if defined(_BCM96338_) || defined(CONFIG_BCM96338)
#define FLASH_BASE          (0xA0000000 | PHYS_FLASH_BASE)  /* uncached Flash  */
#define BOOT_OFFSET         0
#else
#define FLASH_BASE          (0xA0000000 | (MPI->cs[0].base & 0xFFFFFF00))
#define BOOT_OFFSET         (FLASH_BASE - IMAGE_BASE)
#endif

/*****************************************************************************/
/*  Select the PLL value to get the desired CPU clock frequency.             */
/*****************************************************************************/
#define FPERIPH            50000000

/*****************************************************************************/
/* Board memory type offset                                                  */
/*****************************************************************************/
#define ONEK                            1024
#define DARE_FLASH_BLOCK_SIZE           (64*ONEK)  /* dare sunmin set flash block size */
#define FLASH_LENGTH_BOOT_ROM           (DARE_FLASH_BLOCK_SIZE) /* dare sunmin set boot length is one block size. */
#define FLASH_RESERVED_AT_END           (DARE_FLASH_BLOCK_SIZE*2) /* dare sunmin reserved 2 sectors for PSI, scratch pad */

#define BOARD_IMAGE_DOWNLOAD_ADDRESS    \
    ((cfe_sdramsize > 0x00800000) ? 0x80800000 : 0x80000000) 
#define BOARD_IMAGE_DOWNLOAD_SIZE       \
    ((cfe_sdramsize > 0x00800000) ? 0x00800000 : 0x00400000) 

/*****************************************************************************/
/*       NVRAM Offset and definition                                         */
/*****************************************************************************/
#define NVRAM_DATA_OFFSET               0x0580
#define NVRAM_DATA_ID                   0x0f1e2d3c  // This is only for backwards compatability

#define NVRAM_LENGTH                    ONEK    // 1k nvram 
#define NVRAM_VERSION_NUMBER            4

#define NVRAM_BOOTLINE_LEN              256
#define NVRAM_BOARD_ID_STRING_LEN       16
#define NVRAM_MAC_ADDRESS_LEN           6

#define THREAD_NUM_ADDRESS_OFFSET       (NVRAM_DATA_OFFSET + 4 + NVRAM_BOOTLINE_LEN + NVRAM_BOARD_ID_STRING_LEN)
#define THREAD_NUM_ADDRESS              (0xA0000000 + PHYS_FLASH_BASE + THREAD_NUM_ADDRESS_OFFSET)

#define DEFAULT_BOOTLINE    "e=192.168.1.1:ffffff00 h=192.168.1.159 g= r=h f=308nfs6358 i=bcm963xx_fs_kernel d=1 p=0 " //ztj for nfs
#define DEFAULT_BOARD_IP    "192.168.1.1"
#define DEFAULT_MAC_NUM     10
#define DEFAULT_BOARD_MAC   "00:10:18:00:00:00"
#define DEFAULT_TP_NUM      0
#define DEFAULT_PSI_SIZE    24

#define NVRAM_MAC_COUNT_MAX 32
#define NVRAM_MAX_PSI_SIZE  64

#ifndef _LANGUAGE_ASSEMBLY
typedef struct
{
    unsigned long ulVersion;
    char szBootline[NVRAM_BOOTLINE_LEN];
    char szBoardId[NVRAM_BOARD_ID_STRING_LEN];
    unsigned long ulMainTpNum;
    unsigned long ulPsiSize;
    unsigned long ulNumMacAddrs;
    unsigned char ucaBaseMacAddr[NVRAM_MAC_ADDRESS_LEN];
    char chReserved[2];
    unsigned long ulCheckSum;
} NVRAM_DATA, *PNVRAM_DATA;
#endif

#define BOOT_LATEST_IMAGE   '0'
#define BOOT_PREVIOUS_IMAGE '1'

/*****************************************************************************/
/*       Misc Offsets                                                        */
/*****************************************************************************/
#define CFE_VERSION_OFFSET           0x0570
#define CFE_VERSION_MARK_SIZE        5
#define CFE_VERSION_SIZE             5

#if defined(_BCM96338_) || defined(CONFIG_BCM96338)
#define WAN_DATA_LED                ((unsigned long *) 0xfffe2030)
#elif defined(_BCM96348_)
#define WAN_DATA_LED                ((unsigned long *) 0xfffe4430)
#elif defined(_BCM96358_)
#define WAN_DATA_LED                ((unsigned long *) 0xfffe243c)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BCM_HWDEFS_H */


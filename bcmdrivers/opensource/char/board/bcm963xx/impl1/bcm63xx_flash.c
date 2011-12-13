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
/*
 ***************************************************************************
 * File Name  : bcm63xx_flash.c
 *
 * Description: This file contains the flash device driver APIs for bcm63xx board. 
 *
 * Created on :  8/10/2002  seanl:  use cfiflash.c, cfliflash.h (AMD specific)
 *
 ***************************************************************************/


/* Includes. */
#include <linux/fs.h>
#include <linux/capability.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/preempt.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>

#include <bcm_map_part.h>
#include <board.h>
#include <bcmTag.h>
#include "flash_api.h"
#include "boardparms.h"

//#define DEBUG_FLASH

int signature_bypass = 0; /* dare sunmin for signature */
int dare_verify_tag(char *string);
extern PFILE_TAG kerSysImageTagGet(void);

static FLASH_ADDR_INFO fInfo;
NVRAM_DATA bootNvramData;

#define ALLOC_TYPE_KMALLOC   0
#define ALLOC_TYPE_VMALLOC   1


void *retriedKmalloc(size_t size)
{
   void *pBuf;
   unsigned char *bufp8 ;

   size += 4 ; /* 4 bytes are used to store the housekeeping information used for freeing */

   // Memory allocation changed from kmalloc() to vmalloc() as the latter is not susceptible to memory fragmentation under low memory conditions
   // We have modified Linux VM to search all pages by default, it is no longer necessary to retry here
   if (!in_interrupt() ) {
      pBuf = vmalloc(size);
      if (pBuf) {
         memset(pBuf, 0, size);
         bufp8 = (unsigned char *) pBuf ;
         *bufp8 = ALLOC_TYPE_VMALLOC ;
         pBuf = bufp8 + 4 ;
      }
   }
   else { // kmalloc is still needed if in interrupt
      pBuf = kmalloc(size, GFP_ATOMIC);
      if (pBuf) {
         memset(pBuf, 0, size);
         bufp8 = (unsigned char *) pBuf ;
         *bufp8 = ALLOC_TYPE_KMALLOC ;
         pBuf = bufp8 + 4 ;
      }
   }

   return pBuf;
}


void retriedKfree(void *pBuf)
{
   unsigned char *bufp8  = (unsigned char *) pBuf ;
   bufp8 -= 4 ;

   if (*bufp8 == ALLOC_TYPE_KMALLOC)
      kfree(bufp8);
   else
      vfree(bufp8);
}

// Initialize the flash and fill out the fInfo structure
void kerSysEarlyFlashInit( void )
{
    flash_init();

    fInfo.flash_nvram_length = NVRAM_LENGTH;
    fInfo.flash_nvram_start_blk = 0;  /* always the first block */
    fInfo.flash_nvram_number_blk = 1; /*always fits in the first block */
    fInfo.flash_nvram_blk_offset = NVRAM_DATA_OFFSET;

    fInfo.flash_rootfs_start_offset = flash_get_sector_size(0);
    if( fInfo.flash_rootfs_start_offset < FLASH_LENGTH_BOOT_ROM )
        fInfo.flash_rootfs_start_offset = FLASH_LENGTH_BOOT_ROM;
 
    /* Read the flash contents into NVRAM buffer */
    flash_read_buf (fInfo.flash_nvram_start_blk, fInfo.flash_nvram_blk_offset,
        (unsigned char *)&bootNvramData, sizeof (NVRAM_DATA)) ;

    if ((BpSetBoardId(bootNvramData.szBoardId) != BP_SUCCESS))
        printk("\n*** Board is not initialized properly ***\n\n");
}

/* dare sunmin dual config backup */
void kerSysDisplayFlashLayout(void)
{
    int i;

    printk("%4s %6s %4s %7s %s\n",
         "Blk#", "BlkOff", "Blks", "MemLen", "Partition Name" ); 
    
    printk("%4d %6lu %4d %7d %s\n",
            fInfo.flash_nvram_start_blk, fInfo.flash_nvram_blk_offset,
            fInfo.flash_nvram_number_blk, fInfo.flash_nvram_length,
            "NVRAM" );

    printk("%4d %6lu %4d %7d %s\n",
            fInfo.flash_dare_persistent_start_blk, fInfo.flash_dare_persistent_blk_offset,
            fInfo.flash_dare_persistent_number_blk, fInfo.flash_dare_persistent_length,
            "Config 2" );
            
    printk("%4d %6lu %4d %7d %s\n",
            fInfo.flash_scratch_pad_start_blk, fInfo.flash_scratch_pad_blk_offset,
            fInfo.flash_scratch_pad_number_blk, fInfo.flash_scratch_pad_length,
            "Scratch PAD" );
            
    printk("%4d %6lu %4d %7d %s\n",
            fInfo.flash_persistent_start_blk, fInfo.flash_persistent_blk_offset,
            fInfo.flash_persistent_number_blk, fInfo.flash_persistent_length,
            "Config 1" );
}

void kerSysFlashInit( void )
{
    int i = 0;
    int totalBlks = 0;
    int totalSize = 0;
    int startAddr = 0;
    int usedBlkSize = 0;
    PFILE_TAG pTag = NULL;
    unsigned long kernelEndAddr = 0;
    unsigned long spAddr = 0;
    
    /* dare sunmin dual config backup */
    unsigned long darePersistentAddr = 0; 
    int tailSize = 0;		/* Length of flash tailend used */
    /* dare sunmin dual config backup end */

    totalBlks = flash_get_numsectors();
    totalSize = flash_get_total_size();

    printk("Total Flash size: %dK with %d sectors\n", totalSize/1024, totalBlks);

    if ((bootNvramData.ulPsiSize != -1) && (bootNvramData.ulPsiSize != 0))
        fInfo.flash_persistent_length = bootNvramData.ulPsiSize * ONEK;
    else
        fInfo.flash_persistent_length = DEFAULT_PSI_SIZE * ONEK;

    startAddr = totalSize - fInfo.flash_persistent_length;
    fInfo.flash_persistent_start_blk = flash_get_blk(startAddr+FLASH_BASE);
    fInfo.flash_persistent_number_blk = totalBlks - fInfo.flash_persistent_start_blk;
    // save abs SP address (Scratch Pad). it is before PSI 
    spAddr = startAddr - SP_MAX_LEN ;
    // find out the offset in the start_blk
    usedBlkSize = 0;
    for (i = fInfo.flash_persistent_start_blk; 
        i < (fInfo.flash_persistent_start_blk + fInfo.flash_persistent_number_blk); i++)
    {
        usedBlkSize += flash_get_sector_size((unsigned short) i);
    }
    fInfo.flash_persistent_blk_offset =  usedBlkSize - fInfo.flash_persistent_length;

    
    tailSize += usedBlkSize; /* dare sunmin dual config backup */


    // get the info for sp
    if (!(pTag = kerSysImageTagGet()))
    {
        printk("Failed to read image tag from flash\n");
        return;
    }
    kernelEndAddr = (unsigned long)simple_strtoul(pTag->kernelAddress,NULL,10)+ \
        (unsigned long) simple_strtoul(pTag->kernelLen, NULL, 10) + BOOT_OFFSET;

    // make sure sp does not share kernel block
    fInfo.flash_scratch_pad_start_blk = flash_get_blk(spAddr+FLASH_BASE);
    if (fInfo.flash_scratch_pad_start_blk != flash_get_blk(kernelEndAddr))
    {
        fInfo.flash_scratch_pad_length = SP_MAX_LEN;
        if (fInfo.flash_persistent_start_blk == fInfo.flash_scratch_pad_start_blk)  // share blk
        {
#if 0 /* do not used scratch pad unless it's in its own sector */
            printk("Scratch pad is not used for this flash part.\n");  
            fInfo.flash_scratch_pad_length = 0;     // no sp
#else /* allow scratch pad to share a sector with another section such as PSI */
            fInfo.flash_scratch_pad_number_blk = 1;
            fInfo.flash_scratch_pad_blk_offset = fInfo.flash_persistent_blk_offset - fInfo.flash_scratch_pad_length;
#endif
        }
        else // on different blk
        {
            fInfo.flash_scratch_pad_number_blk = fInfo.flash_persistent_start_blk\
                - fInfo.flash_scratch_pad_start_blk;
            // find out the offset in the start_blk
            usedBlkSize = 0;
            for (i = fInfo.flash_scratch_pad_start_blk; 
                i < (fInfo.flash_scratch_pad_start_blk + fInfo.flash_scratch_pad_number_blk); i++)
                usedBlkSize += flash_get_sector_size((unsigned short) i);
                fInfo.flash_scratch_pad_blk_offset =  usedBlkSize - fInfo.flash_scratch_pad_length;
                
            tailSize += usedBlkSize; /* dare sunmin dual config backup */    

        }
    }
    else
    {
        printk("No flash for scratch pad!\n");  
        fInfo.flash_scratch_pad_length = 0;     // no sp
    }


    /* dare sunmin dual config backup */
    fInfo.flash_dare_persistent_length = fInfo.flash_persistent_length; /* use same size as config 1 section */
    darePersistentAddr = totalSize - tailSize - fInfo.flash_dare_persistent_length; 
    
    fInfo.flash_dare_persistent_start_blk = flash_get_blk(darePersistentAddr + FLASH_BASE);
    if (fInfo.flash_dare_persistent_start_blk != flash_get_blk(kernelEndAddr))
    {   
      fInfo.flash_dare_persistent_number_blk = fInfo.flash_scratch_pad_start_blk\
          - fInfo.flash_dare_persistent_start_blk;
      // find out the offset in the start_blk
      usedBlkSize = 0;
      for (i = fInfo.flash_dare_persistent_start_blk; 
           i < (fInfo.flash_dare_persistent_start_blk + fInfo.flash_dare_persistent_number_blk); i++)
      {
        usedBlkSize += flash_get_sector_size((unsigned short) i);
      }
      fInfo.flash_dare_persistent_blk_offset =  usedBlkSize - fInfo.flash_dare_persistent_length;
    }
    else
    {
        printk("No flash for second config!\n");  
        fInfo.flash_dare_persistent_length = 0;     // no sp
    }
    
    
    /* dare sunmin dual config backup */  
    kerSysDisplayFlashLayout();
    
#if defined(DEBUG_FLASH)
    printk("fInfo.flash_scratch_pad_start_blk = %d\n", fInfo.flash_scratch_pad_start_blk);
    printk("fInfo.flash_scratch_pad_number_blk = %d\n", fInfo.flash_scratch_pad_number_blk);
    printk("fInfo.flash_scratch_pad_length = 0x%x\n", fInfo.flash_scratch_pad_length);
    printk("fInfo.flash_scratch_pad_blk_offset = 0x%x\n", (unsigned int)fInfo.flash_scratch_pad_blk_offset);

    printk("fInfo.flash_nvram_start_blk = %d\n", fInfo.flash_nvram_start_blk);
    printk("fInfo.flash_nvram_blk_offset = 0x%x\n", (unsigned int)fInfo.flash_nvram_blk_offset);
    printk("fInfo.flash_nvram_number_blk = %d\n", fInfo.flash_nvram_number_blk);

    printk("psi startAddr = %x\n", startAddr+FLASH_BASE);
    printk("fInfo.flash_persistent_start_blk = %d\n", fInfo.flash_persistent_start_blk);
    printk("fInfo.flash_persistent_blk_offset = 0x%x\n", (unsigned int)fInfo.flash_persistent_blk_offset);
    printk("fInfo.flash_persistent_number_blk = %d\n", fInfo.flash_persistent_number_blk);

    printk("fInfo.flash_rootfs_start_offset = %lu\n", fInfo.flash_rootfs_start_offset);
#endif

}



/***********************************************************************
 * Function Name: kerSysFlashAddrInfoGet
 * Description  : Fills in a structure with information about the NVRAM
 *                and persistent storage sections of flash memory.  
 *                Fro physmap.c to mount the fs vol.
 * Returns      : None.
 ***********************************************************************/
void kerSysFlashAddrInfoGet(PFLASH_ADDR_INFO pflash_addr_info)
{
    pflash_addr_info->flash_nvram_blk_offset = fInfo.flash_nvram_blk_offset;
    pflash_addr_info->flash_nvram_length = fInfo.flash_nvram_length;
    pflash_addr_info->flash_nvram_number_blk = fInfo.flash_nvram_number_blk;
    pflash_addr_info->flash_nvram_start_blk = fInfo.flash_nvram_start_blk;
    pflash_addr_info->flash_persistent_blk_offset = fInfo.flash_persistent_blk_offset;
    pflash_addr_info->flash_persistent_length = fInfo.flash_persistent_length;
    pflash_addr_info->flash_persistent_number_blk = fInfo.flash_persistent_number_blk;
    pflash_addr_info->flash_persistent_start_blk = fInfo.flash_persistent_start_blk;
    pflash_addr_info->flash_rootfs_start_offset = fInfo.flash_rootfs_start_offset;
}


// get shared blks into *** pTempBuf *** which has to be released bye the caller!
// return: if pTempBuf != NULL, poits to the data with the dataSize of the buffer
// !NULL -- ok
// NULL  -- fail
static char *getSharedBlks(int start_blk, int end_blk)
{
    int i = 0;
    int usedBlkSize = 0;
    int sect_size = 0;
    char *pTempBuf = NULL;
    char *pBuf = NULL;

    for (i = start_blk; i < end_blk; i++)
        usedBlkSize += flash_get_sector_size((unsigned short) i);

#if defined(DEBUG_FLASH)
    printk("usedBlkSize = %d\n", usedBlkSize);
#endif

    if ((pTempBuf = (char *) retriedKmalloc(usedBlkSize)) == NULL)
    {
        printk("failed to allocate memory with size: %d\n", usedBlkSize);
        return pTempBuf;
    }
    
    pBuf = pTempBuf;
    for (i = start_blk; i < end_blk; i++)
    {
        sect_size = flash_get_sector_size((unsigned short) i);

#if defined(DEBUG_FLASH)
        printk("i = %d, sect_size = %d, end_blk = %d\n", i, sect_size, end_blk);
#endif
        flash_read_buf((unsigned short)i, 0, pBuf, sect_size);
        pBuf += sect_size;
    }
    
    return pTempBuf;
}

// Set the pTempBuf to flash from start_blk to end_blk
// return:
// 0 -- ok
// -1 -- fail
static int setSharedBlks(int start_blk, int end_blk, char *pTempBuf)
{
    int i = 0;
    int sect_size = 0;
    int sts = 0;
    char *pBuf = pTempBuf;

    for (i = start_blk; i < end_blk; i++)
    {
        sect_size = flash_get_sector_size((unsigned short) i);
        flash_sector_erase_int(i);

        if (flash_write_buf(i, 0, pBuf, sect_size) != sect_size)
        {
            printk("Error writing flash sector %d.", i);
            sts = -1;
            break;
        }
        pBuf += sect_size;
    }

    return sts;
}



#if !defined(CONFIG_BRCM_IKOS)
/*******************************************************************************
 * NVRAM functions
 *******************************************************************************/

// get nvram data
// return:
//  0 - ok
//  -1 - fail
int kerSysNvRamGet(char *string, int strLen, int offset)
{
    char *pBuf = NULL;

    if (strLen > NVRAM_LENGTH)
        return -1;

    if ((pBuf = getSharedBlks(fInfo.flash_nvram_start_blk,
        (fInfo.flash_nvram_start_blk + fInfo.flash_nvram_number_blk))) == NULL)
        return -1;

    // get string off the memory buffer
    memcpy(string, (pBuf + fInfo.flash_nvram_blk_offset + offset), strLen);

    retriedKfree(pBuf);

    return 0;
}


// set nvram 
// return:
//  0 - ok
//  -1 - fail
int kerSysNvRamSet(char *string, int strLen, int offset)
{
    int sts = 0;
    char *pBuf = NULL;

    if (strLen > NVRAM_LENGTH)
        return -1;

    if ((pBuf = getSharedBlks(fInfo.flash_nvram_start_blk,
        (fInfo.flash_nvram_start_blk + fInfo.flash_nvram_number_blk))) == NULL)
        return -1;

    // set string to the memory buffer
    memcpy((pBuf + fInfo.flash_nvram_blk_offset + offset), string, strLen);

    if (setSharedBlks(fInfo.flash_nvram_start_blk, 
        (fInfo.flash_nvram_number_blk + fInfo.flash_nvram_start_blk), pBuf) != 0)
        sts = -1;
    
    retriedKfree(pBuf);

    return sts;
}


/***********************************************************************
 * Function Name: kerSysEraseNvRam
 * Description  : Erase the NVRAM storage section of flash memory.
 * Returns      : 1 -- ok, 0 -- fail
 ***********************************************************************/
int kerSysEraseNvRam(void)
{
    int sts = 1;
    char *tempStorage = retriedKmalloc(NVRAM_LENGTH);
    
    // just write the whole buf with '0xff' to the flash
    if (!tempStorage)
        sts = 0;
    else
    {
        memset(tempStorage, 0xff, NVRAM_LENGTH);
        if (kerSysNvRamSet(tempStorage, NVRAM_LENGTH, 0) != 0)
            sts = 0;
        retriedKfree(tempStorage);
    }

    return sts;
}
#endif


/*******************************************************************************
 * PSI functions
 *******************************************************************************/
// get psi data
// return:
//  0 - ok
//  -1 - fail
int kerSysPersistentGet(char *string, int strLen, int offset)
{
    char *pBuf = NULL;

    if (strLen > fInfo.flash_persistent_length)
        return -1;

    if ((pBuf = getSharedBlks(fInfo.flash_persistent_start_blk,
        (fInfo.flash_persistent_start_blk + fInfo.flash_persistent_number_blk))) == NULL)
        return -1;

    // get string off the memory buffer
    memcpy(string, (pBuf + fInfo.flash_persistent_blk_offset + offset), strLen);

    retriedKfree(pBuf);

    return 0;
}


// set psi 
// return:
//  0 - ok
//  -1 - fail
int kerSysPersistentSet(char *string, int strLen, int offset)
{
    int sts = 0;
    char *pBuf = NULL;

    if (strLen > fInfo.flash_persistent_length)
        return -1;

    if ((pBuf = getSharedBlks(fInfo.flash_persistent_start_blk,
        (fInfo.flash_persistent_start_blk + fInfo.flash_persistent_number_blk))) == NULL)
        return -1;

    // set string to the memory buffer
    memcpy((pBuf + fInfo.flash_persistent_blk_offset + offset), string, strLen);

    if (setSharedBlks(fInfo.flash_persistent_start_blk, 
        (fInfo.flash_persistent_number_blk + fInfo.flash_persistent_start_blk), pBuf) != 0)
        sts = -1;
    
    retriedKfree(pBuf);

    return sts;
}


// flash bcm image 
// return: 
// 0 - ok
// !0 - the sector number fail to be flashed (should not be 0)
int kerSysBcmImageSet( int flash_start_addr, char *string, int size)
{
    int sts;
    int sect_size;
    int blk_start;
    int i;
    char *pTempBuf = NULL;
    int whole_image = 0;

    blk_start = flash_get_blk(flash_start_addr);
    if( blk_start < 0 )
        return( -1 );

    if (flash_start_addr == FLASH_BASE && size > fInfo.flash_rootfs_start_offset)
        whole_image = 1;

/*#ifdef DARE_IMAGE_SIGNATURE
    if ( signature_bypass == 0 && dare_verify_tag(string) != 0 )
    	return 1;
#endif*/

   /* write image to flash memory */

    // Disable other tasks from this point on
    local_bh_disable();
    preempt_disable();

    do 
    {
        sect_size = flash_get_sector_size(blk_start);
// NOTE: for memory problem in multiple PVC configuration, temporary get rid of kmalloc this 64K for now.
//        if ((pTempBuf = (char *)retriedKmalloc(sect_size)) == NULL)
//        {
//            printk("Failed to allocate memory with size: %d.  Reset the router...\n", sect_size);
//            kerSysMipsSoftReset();     // reset the board right away.
//        }
        // for whole image, no check on psi
        if (!whole_image && blk_start == fInfo.flash_persistent_start_blk)  // share the blk with psi
        {
            if (size > (sect_size - fInfo.flash_persistent_length))
            {
                printk("Image is too big\n");
                break;          // image is too big. Can not overwrite to nvram
            }
            if ((pTempBuf = (char *)retriedKmalloc(sect_size)) == NULL)
            {
               printk("Failed to allocate memory with size: %d.  Reset the router...\n", sect_size);
               kerSysMipsSoftReset();     // reset the board right away.
            }
            flash_read_buf((unsigned short)blk_start, 0, pTempBuf, sect_size);
            if (copy_from_user((void *)pTempBuf,(void *)string, size) != 0)
                break;  // failed ?
            flash_sector_erase_int(blk_start);     // erase blk before flash
            if (flash_write_buf(blk_start, 0, pTempBuf, sect_size) == sect_size) 
                size = 0;   // break out and say all is ok
            retriedKfree(pTempBuf);
            break;
        }
        
        flash_sector_erase_int(blk_start);     // erase blk before flash

        if (sect_size > size) 
        {
            if (size & 1) 
                size++;
            sect_size = size;
        }
        
        if ((i = flash_write_buf(blk_start, 0, string, sect_size)) != sect_size) {
            break;
        }
        blk_start++;
        string += sect_size;
        size -= sect_size; 

        printk(".");
        
    } while (size > 0);

    if (whole_image)  
    {
        // If flashing a whole image, erase to end of flash.
        int total_blks = flash_get_numsectors();
        while( blk_start < total_blks )
        {
            flash_sector_erase_int(blk_start);
            blk_start++;
        }
    }
    if (pTempBuf)
        retriedKfree(pTempBuf);

    if( size == 0 ) 
        sts = 0;  // ok
    else  
        sts = blk_start;    // failed to flash this sector

    printk("\n");

    preempt_enable();
    local_bh_enable();

    return sts;
}

/*******************************************************************************
 * SP functions
 *******************************************************************************/
// get sp data.  NOTE: memcpy work here -- not using copy_from/to_user
// return:
//  >0 - number of bytes copied to tokBuf
//  -1 - fail
int kerSysScratchPadGet(char *tokenId, char *tokBuf, int bufLen)
{
    PSP_TOKEN pToken = NULL;
    char *pBuf = NULL;
    char *pShareBuf = NULL;
    char *startPtr = NULL;
    int usedLen;
    int sts = -1;

    if (fInfo.flash_scratch_pad_length == 0)
        return sts;

    if( bufLen >= (fInfo.flash_scratch_pad_length - sizeof(SP_HEADER) -
        sizeof(SP_TOKEN)) ) 
    {
        printk("Exceed scratch pad space by %d\n", bufLen -
            fInfo.flash_scratch_pad_length - sizeof(SP_HEADER) -
            sizeof(SP_TOKEN));
        return sts;
    }

    if( (pShareBuf = getSharedBlks(fInfo.flash_scratch_pad_start_blk,
        (fInfo.flash_scratch_pad_start_blk +
        fInfo.flash_scratch_pad_number_blk))) == NULL )
    {
        return sts;
    }

    // pBuf points to SP buf
    pBuf = pShareBuf + fInfo.flash_scratch_pad_blk_offset;  

    if(memcmp(((PSP_HEADER)pBuf)->SPMagicNum, MAGIC_NUMBER, MAGIC_NUM_LEN) != 0) 
    {
        printk("Scratch pad is not initialized.\n");
        retriedKfree(pShareBuf);
        return sts;
    }

    // search for the token
    usedLen = sizeof(SP_HEADER);
    startPtr = pBuf + sizeof(SP_HEADER);
    pToken = (PSP_TOKEN) startPtr;
    while( pToken->tokenName[0] != '\0' && pToken->tokenLen > 0 &&
        pToken->tokenLen < fInfo.flash_scratch_pad_length &&
        usedLen < fInfo.flash_scratch_pad_length )
    {

        if (strncmp(pToken->tokenName, tokenId, TOKEN_NAME_LEN) == 0)
        {
            if ( pToken->tokenLen > bufLen )
            {
                printk("The length %d of token %s is greater than buffer len %d.\n", pToken->tokenLen, pToken->tokenName, bufLen);
                break;
            } else             
                sts = pToken->tokenLen;
            memcpy(tokBuf, startPtr + sizeof(SP_TOKEN), sts);
            break;
        }

        usedLen += ((pToken->tokenLen + 0x03) & ~0x03);
        startPtr += sizeof(SP_TOKEN) + ((pToken->tokenLen + 0x03) & ~0x03);
        pToken = (PSP_TOKEN) startPtr;
    }

    retriedKfree(pShareBuf);

    return sts;
}


// set sp.  NOTE: memcpy work here -- not using copy_from/to_user
// return:
//  0 - ok
//  -1 - fail
int kerSysScratchPadSet(char *tokenId, char *tokBuf, int bufLen)
{
    PSP_TOKEN pToken = NULL;
    char *pShareBuf = NULL;
    char *pBuf = NULL;
    SP_HEADER SPHead;
    SP_TOKEN SPToken;
    char *curPtr;
    int sts = -1;

    if (fInfo.flash_scratch_pad_length == 0)
        return sts;

    if( bufLen >= fInfo.flash_scratch_pad_length - sizeof(SP_HEADER) -
        sizeof(SP_TOKEN) )
    {
        printk("Scratch pad overflow by %d bytes.  Information not saved.\n",
            bufLen  - fInfo.flash_scratch_pad_length - sizeof(SP_HEADER) -
            sizeof(SP_TOKEN));
        return sts;
    }

    if( (pShareBuf = getSharedBlks( fInfo.flash_scratch_pad_start_blk,
        (fInfo.flash_scratch_pad_start_blk +
        fInfo.flash_scratch_pad_number_blk) )) == NULL )
    {
        return sts;
    }

    // pBuf points to SP buf
    pBuf = pShareBuf + fInfo.flash_scratch_pad_blk_offset;  

    // form header info.
    memset((char *)&SPHead, 0, sizeof(SP_HEADER));
    memcpy(SPHead.SPMagicNum, MAGIC_NUMBER, MAGIC_NUM_LEN);
    SPHead.SPVersion = SP_VERSION;

    // form token info.
    memset((char*)&SPToken, 0, sizeof(SP_TOKEN));
    strncpy(SPToken.tokenName, tokenId, TOKEN_NAME_LEN - 1);
    SPToken.tokenLen = bufLen;

    if(memcmp(((PSP_HEADER)pBuf)->SPMagicNum, MAGIC_NUMBER, MAGIC_NUM_LEN) != 0)
    {
        // new sp, so just flash the token
        printk("No scratch pad found.  Initialize scratch pad...\n");
        memcpy(pBuf, (char *)&SPHead, sizeof(SP_HEADER));
        curPtr = pBuf + sizeof(SP_HEADER);
        memcpy(curPtr, (char *)&SPToken, sizeof(SP_TOKEN));
        curPtr += sizeof(SP_TOKEN);
        if( tokBuf )
            memcpy(curPtr, tokBuf, bufLen);
    }
    else  
    {
        int putAtEnd = 1;
        int curLen;
        int usedLen;
        int skipLen;

        /* Calculate the used length. */
        usedLen = sizeof(SP_HEADER);
        curPtr = pBuf + sizeof(SP_HEADER);
        pToken = (PSP_TOKEN) curPtr;
        skipLen = (pToken->tokenLen + 0x03) & ~0x03;
        while( pToken->tokenName[0] >= 'A' && pToken->tokenName[0] <= 'z' &&
            strlen(pToken->tokenName) < TOKEN_NAME_LEN &&
            pToken->tokenLen > 0 &&
            pToken->tokenLen < fInfo.flash_scratch_pad_length &&
            usedLen < fInfo.flash_scratch_pad_length )
        {
            usedLen += sizeof(SP_TOKEN) + skipLen;
            curPtr += sizeof(SP_TOKEN) + skipLen;
            pToken = (PSP_TOKEN) curPtr;
            skipLen = (pToken->tokenLen + 0x03) & ~0x03;
        }

        if( usedLen + SPToken.tokenLen + sizeof(SP_TOKEN) >
            fInfo.flash_scratch_pad_length )
        {
            printk("Scratch pad overflow by %d bytes.  Information not saved.\n",
                (usedLen + SPToken.tokenLen + sizeof(SP_TOKEN)) -
                fInfo.flash_scratch_pad_length);
            return sts;
        }

        curPtr = pBuf + sizeof(SP_HEADER);
        curLen = sizeof(SP_HEADER);
        while( curLen < usedLen )
        {
            pToken = (PSP_TOKEN) curPtr;
            skipLen = (pToken->tokenLen + 0x03) & ~0x03;
            if (strncmp(pToken->tokenName, tokenId, TOKEN_NAME_LEN) == 0)
            {
                // The token id already exists.
                if( tokBuf && pToken->tokenLen == bufLen )
                {
                    // The length of the new data and the existing data is the
                    // same.  Overwrite the existing data.
                    memcpy((curPtr+sizeof(SP_TOKEN)), tokBuf, bufLen);
                    putAtEnd = 0;
                }
                else
                {
                    // The length of the new data and the existing data is
                    // different.  Shift the rest of the scratch pad to this
                    // token's location and put this token's data at the end.
                    char *nextPtr = curPtr + sizeof(SP_TOKEN) + skipLen;
                    int copyLen = usedLen - (curLen+sizeof(SP_TOKEN) + skipLen);
                    memcpy( curPtr, nextPtr, copyLen );
                    memset( curPtr + copyLen, 0x00, 
                        fInfo.flash_scratch_pad_length - (curLen + copyLen) );
                    usedLen -= sizeof(SP_TOKEN) + skipLen;
                }
                break;
            }

            // get next token
            curPtr += sizeof(SP_TOKEN) + skipLen;
            curLen += sizeof(SP_TOKEN) + skipLen;
        } // end while

        if( putAtEnd )
        {
            if( tokBuf )
            {
                memcpy( pBuf + usedLen, &SPToken, sizeof(SP_TOKEN) );
                memcpy( pBuf + usedLen + sizeof(SP_TOKEN), tokBuf, bufLen );
            }
            memcpy( pBuf, &SPHead, sizeof(SP_HEADER) );
        }

    } // else if not new sp

    sts = setSharedBlks(fInfo.flash_scratch_pad_start_blk, 
        (fInfo.flash_scratch_pad_number_blk + fInfo.flash_scratch_pad_start_blk),
        pShareBuf);
    
    retriedKfree(pShareBuf);

    return sts;

    
}

// wipe out the scratchPad
// return:
//  0 - ok
//  -1 - fail
int kerSysScratchPadClearAll(void)
{ 
    int sts = -1;
    char *pShareBuf = NULL;
   int j ;
   int usedBlkSize = 0;

   // printk ("kerSysScratchPadClearAll.... \n") ;

    if (fInfo.flash_scratch_pad_length == 0)
        return sts;

    if( (pShareBuf = getSharedBlks( fInfo.flash_scratch_pad_start_blk,
        (fInfo.flash_scratch_pad_start_blk + fInfo.flash_scratch_pad_number_blk) )) == NULL )
        return sts;

   for (j = fInfo.flash_scratch_pad_start_blk;
         j < (fInfo.flash_scratch_pad_start_blk + fInfo.flash_scratch_pad_number_blk);
         j++) {
      usedBlkSize += flash_get_sector_size((unsigned short) j);
   }

   memset(pShareBuf, 0x00, usedBlkSize) ;

    sts = setSharedBlks(fInfo.flash_scratch_pad_start_blk,    
         (fInfo.flash_scratch_pad_number_blk + fInfo.flash_scratch_pad_start_blk),  pShareBuf);

   retriedKfree(pShareBuf);
	
   //printk ("kerSysScratchPadClearAll Done.... \n") ;
   return sts;
}

int kerSysFlashSizeGet(void)
{
   return flash_get_total_size();
}

#if !defined(CONFIG_BRCM_IKOS)
int kerSysMemoryMappedFlashSizeGet(void)
{
    return( flash_get_total_memory_mapped_size() );
}
#endif

unsigned long kerSysReadFromFlash( void *toaddr, unsigned long fromaddr,
    unsigned long len )
{
    int sect = flash_get_blk((int) fromaddr);
    unsigned char *start = flash_get_memptr(sect);
    flash_read_buf( sect, (int) fromaddr - (int) start, toaddr, len );

    return( len );
}

#if defined(CONFIG_BRCM_IKOS)
static NVRAM_DATA ikos_nvram_data =
    {
    NVRAM_VERSION_NUMBER,
    "",
    "ikos",
    0,
    DEFAULT_PSI_SIZE,
    11,
    {0x02, 0x10, 0x18, 0x01, 0x00, 0x01},
    {0x00, 0x00},
    0x720c9f60
    };

int kerSysNvRamGet(char *string, int strLen, int offset)
{
    memcpy(string, (unsigned char *) &ikos_nvram_data, sizeof(NVRAM_DATA));
    return(0);
}

int kerSysNvRamSet(char *string, int strLen, int offset)
{
    return(0);
}

int kerSysEraseNvRam(void)
{
    return(0);
}

int kerSysMemoryMappedFlashSizeGet(void)
{
    return( 4 * 1024 * 1024 );
}
#endif

/* dare sunmin dual config backup */
int kerSysDarePersistentGet(char *string, int strLen, int offset)
{
    char *pBuf = NULL;

    if (strLen > fInfo.flash_dare_persistent_length)
        return -1;

    if ((pBuf = getSharedBlks(fInfo.flash_dare_persistent_start_blk,
        (fInfo.flash_dare_persistent_start_blk + fInfo.flash_dare_persistent_number_blk))) == NULL)
        return -1;

    // get string off the memory buffer
    memcpy(string, (pBuf + fInfo.flash_dare_persistent_blk_offset + offset), strLen);

    retriedKfree(pBuf);

    return 0;
}

int kerSysDarePersistentSet(char *string, int strLen, int offset)
{
    int sts = 0;
    char *pBuf = NULL;

    if (strLen > fInfo.flash_dare_persistent_length)
        return -1;

    if ((pBuf = getSharedBlks(fInfo.flash_dare_persistent_start_blk,
        (fInfo.flash_dare_persistent_start_blk + fInfo.flash_dare_persistent_number_blk))) == NULL)
        return -1;

    // set string to the memory buffer
    memcpy((pBuf + fInfo.flash_dare_persistent_blk_offset + offset), string, strLen);

    if (setSharedBlks(fInfo.flash_dare_persistent_start_blk, 
        (fInfo.flash_dare_persistent_number_blk + fInfo.flash_dare_persistent_start_blk), pBuf) != 0)
        sts = -1;
    
    retriedKfree(pBuf);

    return sts;
}


#ifdef DARE_IMAGE_SIGNATURE
int dare_verify_tag(char *string)
{
   int ret = 0;
   
   PFILE_TAG ptag1 = (PFILE_TAG) string;
   //  Big notice:  the offset depends on the upgrade file type
   PFILE_TAG ptag2 = (PFILE_TAG) (string + FLASH_LENGTH_BOOT_ROM);
   
   //printk("My tag is %s, Image tage is %s, %s\n", BCM_SIG_1, ptag1->signiture_1, ptag2->signiture_1);
   
   if((strcmp(ptag1->signiture_1,BCM_SIG_1)!=0)&&(strcmp(ptag2->signiture_1,BCM_SIG_1)!=0)) 
     ret = 1;
     
   if ( ret == 1)
     printk("Digital Signature check Fail (kernel).\n"); 
   else
     printk("Digital Signature check ok (kernel).\n");
   
   return ret;
}
#endif

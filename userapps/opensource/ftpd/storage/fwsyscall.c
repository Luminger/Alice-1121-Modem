/*****************************************************************************
//
//  Copyright (c) 2000-2003  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       fwsyscall.c
//  Author:         seanl
//  Creation Date:  10/10/03
//                  The system call for kill process and firmware flashing 
//                  functions (from syscall.c and dltftp.c)
//
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <pwd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <net/if.h>
#include <ctype.h>

#include "fwsyscall.h"
#define  BCMTAG_EXE_USE
#include "bcmTag.h"

extern char **environ;
char glbIfName[IFC_NAME_LEN];

#define READ_BUF_SIZE       128
#define CMD_LEN             256

/***************************************************************************
// Function Name: killAllApps().
// Description  : Uses the single implementation in bcmUploadPrepare
//
//		  This function kills all unneeded processes and unloads
//		  modules in an attempt to recover memory prior to flashing
//		  a new image or a new configuration via either httpd, bftpd,
//		  tftpd, client save/reboot, tftp client upload.
//
//		  The premise is that the target will reboot and hence
//		  applications and modules may be unloaded.
//
//		  Heuristics for identifying necessary processes for an
//		  upload to progress:
//			- do not kill init process
//			- do not kill self
//			- do not kill cfm
//			- do not kill httpd
//			- do not kill kernel threads (VM size = 0)
//			- if upload via WanIf
//			     do not kill pvc2684d
//			     do not kill Wanif process (e.g. pppd)
//
//		  In the case of WanIf, a default route is added.
//
// Parameters   : none.
// Returns      : none.
//
****************************************************************************/
void killAllApps(void) {
    bcmUploadPrepare( glbIfName );		/* cfm/util/system/syskill.c */
}

/***************************************************************************
// Function Name: getCrc32
// Description  : caculate the CRC 32 of the given data.
// Parameters   : pdata - array of data.
//                size - number of input data bytes.
//                crc - either CRC32_INIT_VALUE or previous return value.
// Returns      : crc.
****************************************************************************/
UINT32 getCrc32(byte *pdata, UINT32 size, UINT32 crc)
{
    while (size-- > 0)
        crc = (crc >> 8) ^ Crc32_table[(crc ^ *pdata++) & 0xff];

    return crc;
}


/***************************************************************************
// Function Name: verifyTag
// Description  : check if the image is brcm image or not. If passNumber == 1,
//                just check the tag crc for memory allocation.  If passNumber == 2, 
//                check the image crc and tagVersion.
// Parameters   : pTag and passNumber.
// Returns      : UPLOAD_OK or UPLOAD_FAIL_ILLEGAL_IMAGE
****************************************************************************/
UPLOAD_RESULT verifyTag(PFILE_TAG pTag, int passNumber)
{
    UINT32 crc;
    UPLOAD_RESULT status = UPLOAD_OK;
    int totalImageSize;
    int tagVer, curVer, tagVerLast;

    // check tag validate token first
    crc = CRC32_INIT_VALUE;
    crc = getCrc32((char*) pTag, (UINT32)TAG_LEN-TOKEN_LEN, crc);

    if (crc != (UINT32)(*(UINT32*)(pTag->tagValidationToken)))
    {
        printf("Not regular image file\n");
        return UPLOAD_FAIL_ILLEGAL_IMAGE;
    }
    if (passNumber == 1)
        return status;

    // if second pass, check imageCrc and tagVersion
    totalImageSize = atoi(pTag->totalImageLen);
    crc = CRC32_INIT_VALUE;
    crc = getCrc32(((char*)pTag + TAG_LEN), (UINT32) totalImageSize, crc);      
    if (crc != (UINT32) (*(UINT32*)(pTag->imageValidationToken)))
        return UPLOAD_FAIL_ILLEGAL_IMAGE;

    // only allow same or greater tag versions (up to tagVerLast - 26) to be uploaded
    tagVer = atoi(pTag->tagVersion);
    curVer = atoi(BCM_TAG_VER);
    tagVerLast = atoi(BCM_TAG_VER_LAST);

    if (tagVer < curVer || tagVer > tagVerLast)
    {
       printf("Firmware tag version [%d] is not compatible with the current Tag version [%d]\n", tagVer, curVer);
       return UPLOAD_FAIL_ILLEGAL_IMAGE;
    }

    return status;
}



/***************************************************************************
// Function Name: psiVerify
// Description  : Verify whether it is a kind of valid xml file by just check <psitree> </psitree>
// Parameters   : psiBuf.
// Returns      : PSI_STS_OK -- psi kind of verified -- need xmlparse in cfm.
****************************************************************************/

PSI_STATUS psiVerify(char *psiBuf, int imageSize) {

   if (imageSize > sysGetPsiSize())
       return PSI_STS_ERR_INVALID_LENGTH;
 
   // if <psitree>  </psitree> cannot be found then return
   if ((memcmp(psiBuf, XML_PSI_START_TAG, strlen(XML_PSI_START_TAG)) != 0) || !(strstr(psiBuf, XML_PSI_END_TAG)))
       return PSI_STS_ERR_GENERAL;

   return PSI_STS_OK;
}

// create the xml file and signal httpd to process it
// return: UPLOAD_RESULT
UPLOAD_RESULT createXmlAndSignalUser(char *imagePtr, int imageLen)
{
    char xmlFileName[] = "/var/psi_rcv.xml";
    char httpPidFile[] = "/var/run/httpd_pid";
    FILE *fp = NULL;
    int httpdPid =0;
    char cmd[CMD_LEN];

    if (access(xmlFileName, F_OK) == 0) 
        unlink(xmlFileName);
    if ((fp = fopen(xmlFileName, "w")) == NULL)
    {
        printf("Failed to create %s\n", xmlFileName);
        return UPLOAD_FAIL_FTP;
    }
       
    if (fwrite(imagePtr, 1, imageLen, fp) != imageLen)
    {
        printf("Failed to write %s\n", xmlFileName);
        fclose(fp);
        return UPLOAD_FAIL_FTP;
    }    
    fclose(fp);

    // need to signal httpd to process this xml file
    if ((fp = fopen(httpPidFile, "r")) == NULL)
    {
        printf("Failed to read %s\n", httpPidFile);
        return UPLOAD_FAIL_FTP;
    }
    else 
    {
        fgets(cmd, CMD_LEN, fp);
        httpdPid = atoi(cmd);
        fclose(fp);
    }

    if (httpdPid > 0)
    {
        printf("Signalling httpd[%d] to parse XML file\n",  httpdPid ); 

        kill(httpdPid, SIGUSR1);
        return UPLOAD_OK;
    }
    else
        return UPLOAD_FAIL_FTP;
}


// depending on the image type, do the brcm image or whole flash image and the configuration data
// return: UPLOAD_RESULT
UPLOAD_RESULT flashImage(char *imagePtr, PARSE_RESULT imageType, int imageLen)
{
    PFILE_TAG pTag = (PFILE_TAG) imagePtr;
    int cfeSize, rootfsSize, kernelSize;
    unsigned long cfeAddr, rootfsAddr, kernelAddr;
    UPLOAD_RESULT status = UPLOAD_OK;
    PSI_STATUS psiStatus = PSI_STS_OK;

    switch (imageType) 
    {
    case PSI_IMAGE_FORMAT:
        if (imageLen > sysGetPsiSize() || (psiStatus = psiVerify(imagePtr, imageLen)) != PSI_STS_OK)
        {
            printf("Failed to verify configuration data\n");
            status = UPLOAD_FAIL_FLASH;
        }
        if ((status = createXmlAndSignalUser(imagePtr, imageLen)) == UPLOAD_OK)
           return status;
        else
           printf("\nFailed to create configuration file and signal user - error[%d]\n", (int) status);
        break;

    case FLASH_IMAGE_FORMAT:
        printf("\nFlash whole image...");
        // Pass zero for the base address of whole image flash. It will be filled by kernel code
        if ((sysFlashImageSet(imagePtr, imageLen-TOKEN_LEN, 0, BCM_IMAGE_WHOLE)) == -1)
        {
            printf("Failed to flash whole image\n");
            status = UPLOAD_FAIL_FLASH;
        }
        break;

    case BROADCOM_IMAGE_FORMAT:
        cfeSize = rootfsSize = kernelSize = 0;
        // must be tagged Broadcom image.  Check cfe's existence
        cfeAddr = (unsigned long) strtoul(pTag->cfeAddress, NULL, 10);
        cfeSize = atoi(pTag->cfeLen);
        // check kernel existence
        kernelAddr = (unsigned long) strtoul(pTag->kernelAddress, NULL, 10);
        kernelSize = atoi(pTag->kernelLen);
        // check root filesystem existence
        rootfsAddr = (unsigned long) strtoul(pTag->rootfsAddress, NULL, 10);
        rootfsSize = atoi(pTag->rootfsLen);
        if (cfeAddr) 
        {
           printf("\nFlashing CFE...\n");
           if ((sysFlashImageSet(imagePtr+TAG_LEN, cfeSize, (int) cfeAddr,
                 BCM_IMAGE_CFE)) == -1)
           {
                 printf("Failed to flash CFE\n");
                 status = UPLOAD_FAIL_FLASH;
           }
        }
        if (rootfsAddr && kernelAddr) 
        {
           char *tagFs = imagePtr;
           // tag is alway at the sector start of fs
           if (cfeAddr)
           {
               tagFs = imagePtr + cfeSize;       // will trash cfe memory, but cfe is already flashed
               memcpy(tagFs, imagePtr, TAG_LEN);
           }
           printf("Flashing root file system and kernel...\n");
           if ((sysFlashImageSet(tagFs, TAG_LEN+rootfsSize+kernelSize, 
              (int)(rootfsAddr-TAG_LEN), BCM_IMAGE_FS)) == -1)      
           {
               printf("Failed to flash root file system\n");
               status = UPLOAD_FAIL_FLASH;
           }
        }
        break;

    default:
        printf("Illegal image data\n");
        break;
    }

    if (status != UPLOAD_OK)
        printf("**** IMAGE FLASH FAILED ****\n");
    else 
        printf("Image flash done.\n");
    
    return status;
}


// Check for configuration data
PARSE_RESULT checkConfigData(char *image_start_ptr, int imageSize)
{
    PARSE_RESULT result = NO_IMAGE_FORMAT;
    PSI_STATUS psiResult = PSI_STS_ERR_GENERAL;

    if ((psiResult = psiVerify(image_start_ptr, imageSize)) == PSI_STS_OK) 
    {
        printf("Could be configuration data...\n");
	    result = PSI_IMAGE_FORMAT;
    }
    return result;
}

// Check Broadcom image format
PARSE_RESULT checkBrcmImage(char *image_start_ptr)
{
    PARSE_RESULT result = NO_IMAGE_FORMAT;

    if (verifyTag((FILE_TAG *) image_start_ptr, 2) == UPLOAD_OK)
    {
        printf("Broadcom format verified.\n");
        result = BROADCOM_IMAGE_FORMAT;
    }
    return result;
}


// Check flash image format
PARSE_RESULT checkFlashImage(char *image_start_ptr, int bufSize)
{
    unsigned char *image_ptr = image_start_ptr;
    unsigned long image_len = bufSize - TOKEN_LEN;
    unsigned long crc = CRC32_INIT_VALUE;
    PARSE_RESULT result = NO_IMAGE_FORMAT;
    
    crc = getCrc32(image_ptr, image_len, crc);      
    if (memcmp(&crc, image_ptr + image_len, CRC_LEN) == 0)
    {
        printf("Flash image format verified.\n");
        result = FLASH_IMAGE_FORMAT;
    }
    return result;
}


// parseImageData
// parse the image data to see if it Broadcom flash format or flash image format file or psi data
// fBufType definition: BUF_ALL_TYPES:   check all types (2 images and psi)
//                      BUF_IMAGES:      check images only
//                      BUF_CONFIG_DATA: check psi only
// return: NO_IMAGE_FORMAT  
//         BROADCOM_IMAGE_FORMAT       
//         FLASH_IMAGE_FORMAT          
//         PSI_IMAGE_FORMAT

PARSE_RESULT parseImageData(char *image_start_ptr, int bufSize, BUFFER_TYPE fBufType)
{
    PARSE_RESULT result = NO_IMAGE_FORMAT;

    switch (fBufType) 
    {
    case BUF_ALL_TYPES:
        if ((result = checkBrcmImage(image_start_ptr)) == BROADCOM_IMAGE_FORMAT)
            break;
        if ((result = checkFlashImage(image_start_ptr, bufSize)) == FLASH_IMAGE_FORMAT)
            break;
        if ((result = checkConfigData(image_start_ptr, bufSize)) == PSI_IMAGE_FORMAT)
            break;

    case BUF_IMAGES:
        if ((result = checkBrcmImage(image_start_ptr)) == BROADCOM_IMAGE_FORMAT)
            break;
        if ((result = checkFlashImage(image_start_ptr, bufSize)) == FLASH_IMAGE_FORMAT)
            break;
    case BUF_CONFIG_DATA:
        result = checkConfigData(image_start_ptr, bufSize);
        break;
    
    default:
        printf("Illegal buffer type?\n");
        break;
    }

    return result;
}


/***************************************************************************
// Function Name: BcmScm_isServiceEnabled.
// Description  : check the given service is enabled.
// Parameters   : name -- the given service name.
//                where -- local or remote.
// Returns      : TRUE or FALSE.
****************************************************************************/
int BcmScm_isServiceEnabled(char *name, int where) {
   int enbl = FALSE, len = 0, lan = 0, wan = 0;
   char line[CLI_MAX_BUF_SZ];
   FILE* fs = fopen("/var/services.conf", "r");

   // if list is empty then return default which is
   // return true if access from lan and
   // return false if access from wan
   if ( fs == NULL ) {
      if ( where == CLI_ACCESS_LOCAL )
         return TRUE;
      else
         return FALSE;
   }

   len = strlen(name);

   while ( fscanf(fs, "%s\t%d\t%d", line, &lan, &wan) != EOF )
      if ( strncasecmp(line, name, len) == 0 ) {
         if ( where == CLI_ACCESS_LOCAL )
            enbl = lan;
         else if ( where == CLI_ACCESS_REMOTE )
            enbl = wan;
         break;
      }

   fclose(fs);

   return enbl;
}


/***************************************************************************
// Function Name: BcmScm_isInAccessControlList.
// Description  : check the given IP address in the access control list.
// Parameters   : addr -- the given IP address.
// Returns      : TRUE or FALSE.
****************************************************************************/
int BcmScm_isInAccessControlList(char * addr) {
   int in = TRUE, len = 0;
   char line[SEC_BUFF_MAX_LEN];
   FILE* fs = fopen("/var/acl.conf", "r");

   // if list is empty then return true
   // since allow to access for all by default
   if ( fs == NULL )
      return in;

   // get access control mode
   fgets(line, SEC_BUFF_MAX_LEN, fs);

   // if mode is disabled (zero) then return true
   // since allow to access for all by default
   if ( atoi(line) == 0 ) {
      fclose(fs);
      return in;
   }

   // mode is enabled then access control list is active
   // so in should be initialized to false
   in = FALSE;
   len = strlen(addr);

   while ( fgets(line, SEC_BUFF_MAX_LEN, fs) )
      if ( strncmp(line, addr, len) == 0 ) {
         in = TRUE;
         break;
      }

   fclose(fs);

   return in;
}

//**************************************************************************
// Function Name: bcmcheck_enable
// Description  : check the appName with ip address against the psi 
//                for access mode
// Parameters   : appName - application name in the acl.conf (telnet, ssh, etc.)
//                clntAddr - incoming ip address 
// Returns      : access mode - CLI_ACCESS_LOCAL, CLI_REMOTE_LOCAL, CLI_ACCESS_DISABLED
//**************************************************************************
int bcmCheckEnable(char *appName, struct in_addr clntAddr)
{
   // retrieve PSI from flash to make sure it's up-to-date
   //BcmPsi_profileRetrieve(0);

   // is client address in Access Control List ?
   if ( BcmScm_isInAccessControlList(inet_ntoa(clntAddr)) == FALSE )
      return CLI_ACCESS_DISABLED;

   if ( isAccessFromLan(clntAddr) == TRUE ) {
      // is enabled from lan ?
      if ( BcmScm_isServiceEnabled(appName, CLI_ACCESS_LOCAL) == FALSE )
         return CLI_ACCESS_DISABLED;
      else
         return CLI_ACCESS_LOCAL;
   } else {
      // is enabled from wan ?
      if ( BcmScm_isServiceEnabled(appName, CLI_ACCESS_REMOTE) == FALSE )
         return CLI_ACCESS_DISABLED;
      else
         return CLI_ACCESS_REMOTE;
   }
}



#ifndef FW_SYSCALL_H
#define FW_SYSCALL_H

#include <net/if.h>

#include "bcmtypes.h"
#include "board.h"
#include "board_api.h"
#include "clidefs.h"
#include "syscall.h"
#include "psidefs.h"
#include "version.h"
#include "psixml.h"

#define SEC_BUFF_MAX_LEN     16     // from secdefs.h
#define IFC_NAME_LEN		     16		// from ifcdefs.h

/* HTTP upload image formats. */
typedef enum
{
    NO_IMAGE_FORMAT,
    BROADCOM_IMAGE_FORMAT,
    FLASH_IMAGE_FORMAT,
    PSI_IMAGE_FORMAT,
} PARSE_RESULT;


typedef enum
{
    BUF_ALL_TYPES,
    BUF_IMAGES,
    BUF_CONFIG_DATA,
} BUFFER_TYPE;

typedef enum
{
    UPLOAD_OK,
    UPLOAD_FAIL_NO_MEM,
    UPLOAD_FAIL_ILLEGAL_IMAGE,
    UPLOAD_FAIL_FLASH,
    UPLOAD_FAIL_FTP,
} UPLOAD_RESULT;

#if defined(__cplusplus)
extern "C" {
#endif
PARSE_RESULT parseImageData(char *image_start_ptr, int bufSize, BUFFER_TYPE fBufType);
UPLOAD_RESULT flashImage(char *imagePtr, PARSE_RESULT imageType, int imageLen);
int bcmCheckEnable(char *appName, struct in_addr clntAddr);
void killAllApps(void);
#if defined(__cplusplus)
}
#endif   // defined(__cplusplus)

#endif

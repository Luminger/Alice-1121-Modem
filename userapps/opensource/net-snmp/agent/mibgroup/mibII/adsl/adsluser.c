/***************************************************************************
 * Broadcom Corp. Confidential
 * Copyright 2001 Broadcom Corp. All Rights Reserved.
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED 
 * SOFTWARE LICENSE AGREEMENT BETWEEN THE USER AND BROADCOM. 
 * YOU HAVE NO RIGHT TO USE OR EXPLOIT THIS MATERIAL EXCEPT 
 * SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************
 * File Name  : AdslUser.c
 *
 * Description: This file contains the implementation of the user mode
 *              ADSL library.
 *
 * Updates    : 10/02/2001  lat.  Created.
 ***************************************************************************/

/* Broadcom types. */
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;


/* Includes. */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include "bcmadsl.h"
#include "adsldrv.h"

#include "DiagDef.h"

/* Globals. */
int g_nAdslFd = -1;

/***************************************************************************
 * Function Name: OpenBcmAdsl
 * Description  : Opens the bcmadsl device.
 * Returns      : device handle if successsful or -1 if error
 ***************************************************************************/
static int OpenBcmAdsl( void )
{
    int nFd = open( "/dev/bcmadsl0", O_RDWR );

    if( nFd == -1 )
        printf( "BcmAdsl : open error %d\n", errno );

    return( nFd );
} /* OpenBcmAdsl */

/***************************************************************************
 * Function Name: BcmAdsl_GetObjectValue
 * Description  : Gets MIB object value
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
int BcmAdsl_GetObjectValue(char *objId, int objIdLen, char *dataBuf, long *dataBufLen)
{
    ADSLDRV_GET_OBJ		Arg;

    Arg.bvStatus = BCMADSL_STATUS_ERROR;

    if( g_nAdslFd == -1  )
        g_nAdslFd = OpenBcmAdsl();

    if( g_nAdslFd != -1  )
    {
		Arg.objId		= objId;
		Arg.objIdLen	= objIdLen;
		Arg.dataBuf		= dataBuf;
		Arg.dataBufLen	= *dataBufLen;
		ioctl( g_nAdslFd, ADSLIOCTL_GET_OBJ_VALUE, &Arg );
		*dataBufLen = Arg.dataBufLen;
	}
    else
        Arg.bvStatus = BCMADSL_STATUS_ERROR;

    return( Arg.bvStatus );
}

//**************************************************************************
// Function Name: BcmAdsl_GetVersion
// Description  : Changes ADSL version information
// Returns      : STS_SUCCESS 
//**************************************************************************
BCMADSL_STATUS BcmAdsl_GetVersion(adslVersionInfo *pAdslVer)
{
	ADSLDRV_GET_VERSION		Arg;

    Arg.pAdslVer  = pAdslVer;
    Arg.bvStatus = BCMADSL_STATUS_ERROR;

    if( g_nAdslFd == -1  )
        g_nAdslFd = OpenBcmAdsl();

    if( g_nAdslFd != -1  )
        ioctl( g_nAdslFd, ADSLIOCTL_GET_VERSION, &Arg );
    else
        Arg.bvStatus = BCMADSL_STATUS_ERROR;

    return( Arg.bvStatus );
}

/***************************************************************************
 * Function Name: BcmAdsl_ConnectionStart
 * Description  : Start ADSL connection.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMADSL_STATUS BcmAdsl_ConnectionStart( void )
{
    ADSLDRV_STATUS_ONLY Arg;

    Arg.bvStatus = BCMADSL_STATUS_ERROR;

    if( g_nAdslFd == -1  )
        g_nAdslFd = OpenBcmAdsl();

    if( g_nAdslFd != -1  )
        ioctl( g_nAdslFd, ADSLIOCTL_CONNECTION_START, &Arg );
    else
        Arg.bvStatus = BCMADSL_STATUS_ERROR;

    return( Arg.bvStatus );
} /* BcmAdsl_ConnectionStart */


/***************************************************************************
 * Function Name: BcmAdsl_ConnectionStop
 * Description  : Clean up resources allocated during BcmAdsl_Initialize.
 * Returns      : STS_SUCCESS if successful or error status.
 ***************************************************************************/
BCMADSL_STATUS BcmAdsl_ConnectionStop( void )
{
    ADSLDRV_STATUS_ONLY Arg;

    Arg.bvStatus = BCMADSL_STATUS_ERROR;

    if( g_nAdslFd == -1  )
        g_nAdslFd = OpenBcmAdsl();

    if( g_nAdslFd != -1  )
        ioctl( g_nAdslFd, ADSLIOCTL_CONNECTION_STOP, &Arg );
    else
        Arg.bvStatus = BCMADSL_STATUS_ERROR;

    return( Arg.bvStatus );
} /* BcmAdsl_ConnectionStop */



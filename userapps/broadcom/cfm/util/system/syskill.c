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

/*****************************************************************************
//
//  Filename:       syskill.c
//  Creation Date:  04/20/07
//
******************************************************************************
//  Description:
//      It provides upload related system kill functions for Linux.
//	Used by both cfm , busybox: tftpd/ftpd, tr69c
//
*****************************************************************************/

/********************** Include Files ***************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "bcmtypes.h"
#include "ifcdefs.h"
#include "syscall.h"	/* bcmSystemMute, bcmRemoveModules */

//*********** code shared by cfm  ftpd tftpd tr69c **********************
//****************************************************************

/***************************************************************************
// Function Name: bcmUploadPrepare().
//
// Description  :
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
void bcmUploadPrepare(char * ifcName_p) {

    char ifName[IFC_SMALL_LEN];
    char parseBuf[SYS_CMD_LEN], cmd[SYS_CMD_LEN], procCmd[SYS_CMD_LEN];
    int match, candidate, vmsize, pid, curPid, ifPid;
    char admin[32], state[32];

    FILE * fp;
    const char psFileName[] = "/var/psList";

    FILE * pidFp;
    char pidFile[64];	/* used in sprintf "/proc/%d" */

    curPid = getpid();
    ifPid = 0;

	/* Limit ifcName_p to IFC_SMALL_LEN using local variable ifName */
    ifName[0] = '\0';
    if ( ifcName_p != NULL )
        strncpy(ifName, ifcName_p, IFC_SMALL_LEN - 1);

    if (ifName[0] != '\0')
    {
        sprintf(cmd, "/proc/var/fyi/wan/%s/pid", ifName);
        if ((fp = fopen(cmd, "r")) != NULL)
        {
            fgets(cmd, SYS_CMD_LEN, fp);
            ifPid = atoi(cmd);
            fclose(fp);
        }
    }

    printf("bcmUploadPrepare ... curPid[%d] ifPid[%d] ifName[%s]\n\n",
          curPid, ifPid, ifName);

    bcmSystemMute("sysinfo");

    if ( ifPid )	/* Current process needs WAN Interface */
    {
	/*
	 * If not MER and not ipoa,
	 * add the default route with the ifc the socket is sitting on
	 */
	if ( (strstr(ifName, "nas") == NULL)
          && (strstr(ifName, "eth") == NULL) )
        {
            sprintf(cmd, "route add default dev %s", ifName);
            bcmSystem(cmd);
        }
    }

	/* Lets recover memory by killing unwanted processes */
    bcmSystemMute("ps > /var/psList");
    bcmSystemMute("cat /var/psList");

    if ((fp = fopen(psFileName, "r")) == (FILE*)NULL)
       return;

    while ( fgets(parseBuf, SYS_CMD_LEN, fp) > 0 )
    {
	/* Sample format of busybox ps output
	 *
	 *   PID  Uid     VmSize Stat Command
	 *     1 admin       180 S   init
	 *     2 admin           SWN [ksoftirqd/0]
	 *   479 admin       204 S   upnp -L br0 -W ppp_0_0_35_1 -D
	 */

	/* All kernel threads (i.e. VirtualMemory) are excluded by default */

	/* If successfully scanned a process with a valid VmSize: match > 2 */
        if ( (match = sscanf(parseBuf, "%d %s %d %s %s",
                             &pid, admin, &vmsize, state, procCmd)) > 2)
        {
           candidate = 1;	/* process is a candidate for termination */

		/* Specify the Exclusion List */

           if ( (pid == 1)	/* init process */
             || (pid == curPid)
             || (pid == ifPid)
             || ((ifPid) && (strstr(procCmd, "pvc2684d") != NULL))
             || (strstr(procCmd, "-sh") != NULL)
             || (strstr(procCmd, "pvc2684d") != NULL)
             || (strstr(procCmd, "cfm") != NULL)
             || (strstr(procCmd, "httpd") != NULL)	/* needed for xml */
             || (strstr(parseBuf, "vodsl") != NULL)	/* can't be killed or system will hang */
             || (strstr(parseBuf, "telnetd") != NULL)   /* can't be killed if reboot is issued over telnet session */
              )
           {
               candidate = 0;	/* Exclusion List match */
           }
           else
           {
		/* Assumes procfs is supported by kernel */
               sprintf(pidFile, "/proc/%d", pid);

               if ( (pidFp = fopen(pidFile, "r")) == NULL)
                  candidate = 0; /* May be a shell or process that exited */
               else
                  fclose(pidFp);
           }

           if ( candidate )	/* pid is killable */
           {
               printf("kill process [pid: %3d] [%s]...\n", pid, procCmd);
               sprintf(cmd, "kill -9 %d", pid);
               bcmSystemMute(cmd);
           }
        }
    }

    fclose(fp);
    bcmSystemMute("rm /var/psList");

    bcmRemoveModules(ifPid);

    printf("... done bcmUploadPrepare\n\n");
}


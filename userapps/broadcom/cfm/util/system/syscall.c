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
//  Filename:       syscall.c
//  Author:         Peter T. Tran
//  Creation Date:  12/26/01
//
******************************************************************************
//  Description:
//      It provides system call functions for Linux.
//
*****************************************************************************/

/********************** Include Files ***************************************/

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <crypt.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <ctype.h>
#include <net/if.h>
#include <net/route.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "bcmtypes.h"
#ifdef USE_ALL
#include "bcmadsl.h"
#include "ifcdefs.h"
#include "psiapi.h"
#include "sysdiag.h"
#include "clidefs.h"
#include "cliapi.h"
#include "adslctlapi.h"
#if defined(SUPPORT_VDSL)
#include "vdslctlapi.h"
#endif
#include "secapi.h"
#include "dbapi.h"
#include "dbobject.h"
#include "dbvalid.h"
#include "version.h"
#ifdef PORT_MIRRORING
#include <atm.h>
#include "portMirror.h"
#include "atmapidrv.h"
#endif /* PORT_MIRRORING */
#include "bcmcfm.h"
#include "ifcntwkapi.h"
#endif /* USE ALL */

#include "bcmxdsl.h"
#include "syscall.h"
#include "board_api.h"

#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#define HOSTNAME_SIZE  64
#define DNS_CACHE_SIZE 32 

typedef struct 
{
  time_t timestamp;
  struct in_addr addr;
  char hostname[HOSTNAME_SIZE+1];
} dns_cache_t;


#define PRIORITY_HIGH    3
#define PRIORITY_MEDIUM  2
#define PRIORITY_LOW     1

#ifdef BUILD_STATIC
#include "busybox.h"
#endif

#ifndef USE_ALL
#define IFC_LARGE_LEN        264
#endif

extern char **environ;
extern char *getIfName(void);

extern char glbErrMsg[IFC_LARGE_LEN];
#define READ_BUF_SIZE        128

/* If you change this definition, make sure you change it everywhere */
#define UDHCPD_DECLINE "/var/udhcpd.decline"

#ifdef USE_ALL

/***************************************************************************
// Function Name: bcmSystemNoHang().
// Description  : launch shell command in the child process without hang.
// Parameters   : command - shell command to launch.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmSystemNoHang (char *command) {
   int pid = 0, status = 0, counter = 0, ret = 0;

   if ( command == 0 )
      return 1;

   pid = fork();
   if ( pid == -1 )
      return -1;

   if ( pid == 0 ) {
      char *argv[4];
      argv[0] = "sh";
      argv[1] = "-c";
      argv[2] = command;
      argv[3] = 0;
      execve("/bin/sh", argv, environ);
      exit(127);
   }

   do {
      // check the child is exited or not without hang
      ret = waitpid(pid, &status, WNOHANG | WUNTRACED);
      switch ( ret ) {
         case -1:   // error occurs then return -1
            return -1;
         case 0:    // child does not exited yet
            usleep(20);
            if ( ++counter > 20000 ) {
#ifdef BRCM_DEBUG
               printf("app: child process cannot exits while executing command - %s\n", command);
#endif
               kill(pid, SIGTERM);
               return -1;
            }
            break;
         default:   // child does exit successfully
            return status;
      }
   } while ( 1 );

   return status;
}

/***************************************************************************
// Function Name: bcmCreateDhcpCfg().
// Description  : create DHCP server configuration file.
// Parameters   : ipAddr - IP address of target.
//                mask - subnet mask of target.
//                addrStart - start IP address of DHCP server pool.
//                addrEnd - end IP address of DHCP server pool.
//                dns1 - primary dns.
//                dns2 - secondary dns.
//                leasedTime - leased time.
//                protoInfo - current network protocol.
//                enblNat -- is nat enabled?
//                enblFirewall -- is firewall enabled?
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmCreateDhcpCfg(char *ipAddr, char *mask,
                     char *addrStart, char *addrEnd,
                     char *dns1, char *dns2, int leasedTime,
                     int protocol, int enblNat, int enblFirewall, char* domain) {
   FILE* fs = fopen("/etc/udhcpd.conf", "w");
   void *node = NULL;
   char mac[18], static_ip[16];

   if ( fs != NULL ) {
      // cwu
      char cmd[128]="";
      sprintf(cmd, "echo %s > /var/run/ip", addrStart);
      system(cmd);

      // start IP address
      sprintf(cmd, "start %s\n", addrStart);
      fputs(cmd, fs);

      // end IP address
      sprintf(cmd, "end %s\n", addrEnd);
      fputs(cmd, fs);

      // interface
      switch ( protocol ) {
      case PROTO_PPPOA:
      case PROTO_PPPOE:
      case PROTO_MER:
      case PROTO_IPOA:
#ifdef DARE_DHCP_IN_BRIDGE
      case PROTO_BRIDGE: //DARE enble dhcp in bridge
#endif
      case PROTO_NONE:
#if SUPPORT_ETHWAN
      case PROTO_IPOWAN:
#endif
         fputs("interface br0\n", fs);
         break;
      }

      // If you change the name of this file, make sure you change it
      // everywhere by searching for UDHCPD_DECLINE macro
      sprintf(cmd, "decline_file %s\n", UDHCPD_DECLINE);
      fputs(cmd, fs);

	  // lease
      sprintf(cmd, "option lease %d\n", leasedTime);
      fputs(cmd, fs);
      sprintf(cmd, "option min_lease 30\n");
      fputs(cmd, fs);

      // subnet mask
      sprintf(cmd, "option subnet %s\n", mask);
      fputs(cmd, fs);

      // router
      sprintf(cmd, "option router %s\n", ipAddr);
      fputs(cmd, fs);
      // use DNS relay only when NAT is enabled
      if ( enblNat == TRUE ) {
         // always use DSL router IP address as DNS
         // for DHCP server since we want local PCs
         // use DHCP server relay. The real DNS is
         // stored in /etc/resolv.conf
         sprintf(cmd, "option dns %s\n", ipAddr);
         fputs(cmd, fs);
      } else { // use real DNS when NAT is disabled
         // primary DNS
         if ( strcmp(dns1, "0.0.0.0") != 0 )
            sprintf(cmd, "option dns %s\n", dns1);
         else
            sprintf(cmd, "option dns %s\n", ipAddr);
         fputs(cmd, fs);
         // secondary DNS
         if ( strcmp(dns2, "0.0.0.0") != 0 )
            sprintf(cmd, "option dns %s\n", dns2);
         else
            sprintf(cmd, "option dns %s\n", ipAddr);
         fputs(cmd, fs);
      }
#ifdef SUPPORT_DPROXY
      sprintf(cmd, "option domain %s\n", domain);
      fputs(cmd, fs);
#endif

   node = BcmDhcpd_get( NULL, mac, static_ip );
   while ( node != NULL ) 
   {
      sprintf(cmd, "static_lease %s %s\n", mac, static_ip);
      fputs(cmd, fs);

      node = BcmDhcpd_get( node, mac, static_ip );
   }

      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

#ifdef SUPPORT_SUPER_DMZ // Keven -- super DMZ
int bcmCreateDmzDhcpCfg(char *ipAddr, char *mask,
                        char *addrStart, char *addrEnd, char *domain) {
   FILE* fs = fopen("/var/udhcpd_dmz.conf", "w");

   if ( fs != NULL ) {
      char cmd[128]="";

      // start IP address
      sprintf(cmd, "start %s\n", addrStart);
      fputs(cmd, fs);

      // end IP address
      sprintf(cmd, "end %s\n", addrEnd);
      fputs(cmd, fs);

      // interface
      fputs("interface br1\n", fs);

      // lease
      sprintf(cmd, "option lease 86400\n");
      fputs(cmd, fs);
      sprintf(cmd, "option min_lease 30\n");
      fputs(cmd, fs);

      // subnet mask
      sprintf(cmd, "option subnet %s\n", mask);
      fputs(cmd, fs);

      // router
      sprintf(cmd, "option router %s\n", ipAddr);
      fputs(cmd, fs);

      // always use DSL router IP address as DNS
      // for DHCP server since we want local PCs
      // use DHCP server relay. The real DNS is
      // stored in /etc/resolv.conf
      sprintf(cmd, "option dns %s\n", ipAddr);
      fputs(cmd, fs);

#ifdef SUPPORT_DPROXY
      sprintf(cmd, "option domain %s\n", domain);
      fputs(cmd, fs);
#endif

      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}
#endif

/***************************************************************************
// Function Name: bcmCreateIpExtDhcpCfg().
// Description  : create DHCP server configuration file for PPP IP Extenstion.
// Parameters   : lanAddr - LAN IP address of target.
//                mask - subnet mask of target.
//                wanAddr - WAN IP address.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmCreateIpExtDhcpCfg(char *lanAddr, char *mask, char *wanAddr, char *domain, char *remote) {
   char cmd[SYS_CMD_LEN], dns[SYS_CMD_LEN];
   char dfltGw[IFC_TINY_LEN];
   char ifc[IFC_TINY_LEN];
   FILE* fs = fopen("/etc/udhcpd.conf", "w");

   if ( fs != NULL ) {
       // cwu
      sprintf(cmd, "echo %s > /var/run/ip", wanAddr);
      system(cmd);


      // start IP address
      sprintf(cmd, "start %s\n", wanAddr);
      fputs(cmd, fs);

      // end IP address
      sprintf(cmd, "end %s\n", wanAddr);
      fputs(cmd, fs);

      // interface
      fputs("interface br0\n", fs);

      // lease
      fputs("option lease 30\n", fs);
      fputs("option min_lease 30\n", fs);

      // subnet mask
      sprintf(cmd, "option subnet %s\n", mask);
      fputs(cmd, fs);

      // router
      bcmGetDefaultGateway(dfltGw, ifc);
      if (dfltGw[0] == '\0')
      {
         sprintf(cmd, "option router %s\n", wanAddr);
      }
      else
      {
         sprintf(cmd, "option router %s\n", dfltGw);
      }
      fputs(cmd, fs);

      // don't use DNS relay since there is no ip table for
      // PPP IP extension
      bcmGetDns(dns);
      sprintf(cmd, "option dns %s\n", dns);
      fputs(cmd, fs);

#ifdef SUPPORT_DPROXY
      sprintf(cmd, "option domain %s\n", domain);
      fputs(cmd, fs);
#endif

      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

#ifdef MULTISUBNET
/***************************************************************************
// Function Name: bcmCreateDhcpCfgEx().
// Description  : create DHCP server configuration file for PPP IP Extenstion.
// Parameters   : lanAddr - LAN IP address of target.
//                mask - subnet mask of target.
//                wanAddr - WAN IP address.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmCreateDhcpCfgEx(char * szInterface, char *ipAddr, char *mask,
                     char *addrStart, char *addrEnd,
                     char *dns1, char *dns2, int leasedTime,
                     int protocol, int enblNat, int enblFirewall, char* domain) {

   char szDHCP_file[128];
   char cmdDHCP_leasefile[128];
   sprintf(szDHCP_file, "/var/udhcpd_%s.conf", szInterface);
   FILE* fs = fopen(szDHCP_file, "w");

   if ( fs != NULL ) {
      // cwu
      char cmd[128]="";
      

       // start IP address
      sprintf(cmd, "start %s\n", addrStart);
      fputs(cmd, fs);

      // end IP address
      sprintf(cmd, "end %s\n", addrEnd);
      fputs(cmd, fs);

      // interface
      sprintf(cmd, "interface %s\n", szInterface);
      fputs(cmd, fs);

      sprintf(cmd, "lease_file  /var/udhcpd_%s.lease\n", szInterface);
      fputs(cmd, fs);

      sprintf(cmdDHCP_leasefile, "echo >  /var/udhcpd_%s.lease &", szInterface); 
      bcmSystem(cmdDHCP_leasefile);
      

      sprintf(cmd, "pidfile  /var/udhcpd_%s.pid\n", szInterface);
      fputs(cmd, fs);
	  
	  // lease
      sprintf(cmd, "option lease %d\n", leasedTime);
      fputs(cmd, fs);
      sprintf(cmd, "option min_lease 30\n");
      fputs(cmd, fs);

      // subnet mask
      sprintf(cmd, "option subnet %s\n", mask);
      fputs(cmd, fs);

      // router
      sprintf(cmd, "option router %s\n", ipAddr);
      fputs(cmd, fs);

      
      sprintf(cmd, "option dns %s\n", ipAddr);
      fputs(cmd, fs);

      sprintf(cmd, "option dns %s\n", ipAddr);
      fputs(cmd, fs);
	  

     
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}
#endif

/***************************************************************************
// Function Name: bcmSetIpExtInfo().
// Description  : store wan, gateway, and dns for PPP IP extension.
// Parameters   : wan - WAN IP address.
//                gateway - default gateway.
//                dns - dns.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmSetIpExtInfo(char *wan, char *gateway, char *dns) {
   char str[256];
   FILE* fs = fopen("/var/ipextinfo", "w");

   if ( fs != NULL ) {
      sprintf(str, "%s %s %s\n", wan, gateway, dns);
      fputs(str, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmGetIpExtInfo().
// Description  : get wan, gateway, or dns for PPP IP extension.
// Parameters   : buf - .
//                type - .
// Returns      : none.
****************************************************************************/
void bcmGetIpExtInfo(char *buf, int type) {
   FILE* fs;
   char wan[64], gateway[64], dns[64], str[256];

   if ( buf == NULL ) return;

   buf[0] = '\0';
   if( bcmGetAdslStatus() == MNTR_STS_OK ) {
      fs = fopen("/var/ipextinfo", "r");
      if ( fs != NULL ) {
         fgets(str, 256, fs);
         fclose(fs);
         sscanf(str, "%s %s %s\n", wan, gateway, dns);
         switch ( type ) {
         case 0:
            if( dns[0] >= '0' && dns[0] <= '9' )
               strcpy(buf, wan);
            break;
         case 1:
            if( dns[0] >= '0' && dns[0] <= '9' )
               strcpy(buf, gateway);
            break;
         case 2:
            if( dns[0] >= '0' && dns[0] <= '9' )
               strcpy(buf, dns);
            break;
         }
      }
   }
}

/***************************************************************************
// Function Name: bcmCreateLocalDhcpCfg().
// Description  : create DHCP server configuration file with default local.
// Parameters   : ipAddr -- default local IP address.
//                mask -- default local subnet mask.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmCreateLocalDhcpCfg(char *ipAddr, char *mask, char *domain) {
   FILE* fs = fopen("/etc/udhcpd.conf", "w");
   char cmd[SYS_CMD_LEN], nextAddr[SYS_CMD_LEN];
   struct in_addr addr;

   if ( fs != NULL ) {
      addr.s_addr = inet_addr(ipAddr) + 1;
      strcpy(nextAddr, inet_ntoa(addr));

      sprintf(cmd, "echo %s > /var/run/ip", nextAddr);
      system(cmd);

      // start IP address
      fprintf(fs, "start %s\n", nextAddr);

      // end IP address
      fprintf(fs, "end %s\n", nextAddr);

      // interface
      fputs("interface br0\n", fs);

      // lease
      fputs("option lease 10\n", fs);
      fputs("option min_lease 10\n", fs);

      // subnet mask
      fprintf(fs, "option subnet %s\n", mask);

      // router
      fprintf(fs, "option router %s\n", ipAddr);

      // dns
      fprintf(fs, "option dns %s\n", ipAddr);

#ifdef SUPPORT_DPROXY
      sprintf(cmd, "option domain %s\n", domain);
      fputs(cmd, fs);
#endif

      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmCreateResolvCfg().
// Description  : create resolv configuration file.
// Parameters   : dns1 - primary dns.
//                dns2 - secondary dns.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmCreateResolvCfg(char *dns1, char *dns2) {
   char cmd[SYS_CMD_LEN];
   FILE* fs = NULL;

   bcmSystemMute("mkdir -p /var/fyi/sys");
   bcmSystemMute("echo > /var/fyi/sys/dns");
   fs = fopen("/var/fyi/sys/dns", "w");

   if ( fs != NULL ) {
      sprintf(cmd, "nameserver %s\n", dns1);
      fputs(cmd, fs);
      sprintf(cmd, "nameserver %s\n", dns2);
      fputs(cmd, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

// global ADSL info variable is declared here (in syscall.c)
// and is used in syscall.c, sysdiag.c, cgimain.c, and cgists.c
ADSL_CONNECTION_INFO glbAdslInfo;
#if defined(SUPPORT_VDSL)
XDSL_CONNECTION_INFO glbVdslInfo;
#endif

/***************************************************************************
// Function Name: bcmGetXdslStatus().
// Description  : get ADSL or VDSL status, depending on which link is up
// Parameters   : none
// Returns      : Link satus as in BCMXDSL_STATUS
****************************************************************************/
int bcmGetXdslStatus() 
{
  BCMXDSL_STATUS  ret = BCM_XDSL_LINK_DOWN;
  BCMADSL_STATUS  sts;

#ifndef BCM_ATM_BONDING_ETH
    sts = BcmAdslCtl_GetConnectionInfo(&glbAdslInfo);
    if( sts == BCMADSL_STATUS_SUCCESS )
      ret = glbAdslInfo.LinkState;

#if defined(SUPPORT_VDSL)
    if((sts == BCMADSL_STATUS_ERROR) || ( glbAdslInfo.LinkState == BCM_XDSL_LINK_DOWN))
    {
      sts = BcmVdslCtl_GetConnectionInfo(&glbVdslInfo, glbErrMsg);
      if( sts == BCMADSL_STATUS_SUCCESS )
        ret = glbVdslInfo.LinkState;
    }
#endif
#else
    /* Only Adsl Bonding is supported for now, hence we poll AtmCtl to get the
     * ATM Bonding link information.
     * We need aggregate information for both the lines, but here invoking the
     * adsl ctl to make sure DSL diags gets initialized and ignore the result.
     */
    sts = BcmAdslCtl_GetConnectionInfo(&glbAdslInfo);

    sts = AtmCfgMngr_GetBondingConnectionInfo(&glbAdslInfo);
    if( sts == BCMADSL_STATUS_SUCCESS )
      ret = glbAdslInfo.LinkState;
#endif

  return((int)ret);
}

/***************************************************************************
// Function Name: bcmGetAdslStatus().
// Description  : get ADSL status.
// Parameters   : none
// Returns      : 0 - ADSL link Up (OK)
//                1 - ADSL link Down
//                2 - Other error
****************************************************************************/
int bcmGetAdslStatus() {
   int ret = 0;
   BCMADSL_STATUS  sts;

#ifndef BCM_ATM_BONDING_ETH
	 sts = BcmAdslCtl_GetConnectionInfo(&glbAdslInfo) ;
#else
	 sts = BcmAdslCtl_GetConnectionInfo(&glbAdslInfo) ;
   sts = AtmCfgMngr_GetBondingConnectionInfo(&glbAdslInfo);
#endif
	 if (sts != BCMADSL_STATUS_ERROR) {
      ret = glbAdslInfo.LinkState;
   } else
      ret = BCMADSL_STATUS_ERROR;

   return ret;
}

#if defined(SUPPORT_VDSL)
/***************************************************************************
// Function Name: bcmGetVdslStatus().
// Description  : get VDSL status.
// Parameters   : none
// Returns      : 0 - VDSL link Up (OK)
//                1 - VDSL link Down
//                2 - Other error
****************************************************************************/
int bcmGetVdslStatus() {
   return((BcmVdslCtl_GetConnectionInfo(&glbVdslInfo, glbErrMsg) != BCMADSL_STATUS_ERROR) ?
           glbVdslInfo.LinkState : BCMADSL_STATUS_ERROR);
}
#endif

/***************************************************************************
// Function Name: bcmGetPppStatus().
// Description  : get PPP status.
// Parameters   : str - buffer to retrieve message
//                len - length of buffer
// Returns      : 0 - OK
//                -1 - ERROR
****************************************************************************/
int bcmGetPppStatus(char *str, int len, char *name) {
   FILE* fs;

   char filePath[100];
   sprintf(filePath,"/proc/var/fyi/wan/%s/daemonstatus",name);
   fs = fopen(filePath, "r");
   if ( fs != NULL ) {
      fgets(str, len, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}
//LGD_FOR_TR098
/***************************************************************************
// Function Name: bcmGetPppShowtimeSecs().
// Description  : get PPP the link up start secs.
// Parameters   : str - buffer to retrieve message
//                len - length of buffer
// Returns      : 0 - OK
//                -1 - ERROR
****************************************************************************/
int bcmGetPppShowtimeSecs(char *str, int len, char *name) {
   FILE* fs;

   char filePath[100];
   sprintf(filePath,"/proc/var/fyi/wan/%s/showtimesecs",name);
   fs = fopen(filePath, "r");
   if ( fs != NULL ) {
      fgets(str, len, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmGetPppDisconnectReason().
// Description  : get PPP the link disconnect reason.
// Parameters   : str - buffer to retrieve message
//                len - length of buffer
// Returns      : 0 - OK
//                -1 - ERROR
****************************************************************************/
int bcmGetPppDisconnectReason(char *str, int len, char *name) {
   FILE* fs;

   char filePath[100];
   sprintf(filePath,"/proc/var/fyi/wan/%s/disconnreason",name);
   fs = fopen(filePath, "r");
   if ( fs != NULL ) {
      fgets(str, len, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmGetDhcpcStatus().
// Description  : get DHCPC status.
// Parameters   : str - buffer to retrieve message
//                len - length of buffer
// Returns      : 0 - OK
//                -1 - ERROR
****************************************************************************/
int bcmGetDhcpcStatus(char *str, int len) {
   FILE* fs = fopen("/var/run/dhcpc", "r");

   if ( fs != NULL ) {
      fgets(str, len, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmGetSystemStatus().
// Description  : get system status.
// Parameters   : str - buffer to retrieve message
//                len - length of buffer
// Returns      : 0 - OK
//                -1 - ERROR
****************************************************************************/
int bcmGetSystemStatus(char *str, int len) {
   FILE* fs = fopen("/etc/sysmsg", "r");

   if ( fs != NULL ) {
      fgets(str, len, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmSetSystemStatus().
// Description  : set system status.
// Parameters   : int - system status
// Returns      : 0 - OK
//                -1 - ERROR
****************************************************************************/
int bcmSetSystemStatus(int status) {
   char cmd[SYS_CMD_LEN];
   FILE* fs = fopen("/etc/sysmsg", "w");

   if ( fs != NULL ) {
      sprintf(cmd, "%d\n", status);
      fputs(cmd, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmDisplayLed().
// Description  : display LED corresponding to WAN link status.
// Parameters   : status - WAN link status.
// Returns      : none.
****************************************************************************/
void bcmDisplayLed(int status) {
   switch (status) {
   case MNTR_STS_ADSL_DOWN:
      /* this means ADSL is DOWN */
      sysLedCtrl(kLedPPP, kLedStateOff);
      break;
   case MNTR_STS_ADSL_TRAINING:
      /* this means ADSL is TRAINING */
      sysLedCtrl(kLedPPP, kLedStateOff);
      break;
   case MNTR_STS_PPP_AUTH_ERR:
      sysLedCtrl(kLedPPP, kLedStateFail);
      break;
   case MNTR_STS_PPP_DOWN:
      sysLedCtrl(kLedPPP, kLedStateOff);
      break;
   case MNTR_STS_OK:
      /* this means ADSL and PPP are up */
      sysLedCtrl(kLedPPP, kLedStateOn);
      break;
   }
}

void bcmGetDynamicDnsAddr(char *dns, int primary) {
   char str[SYS_CMD_LEN];
   FILE* fs = NULL;

   fs = fopen("/var/fyi/sys/dns", "r");
   if ( fs != NULL ) {
      if ( fgets(str, SYS_CMD_LEN, fs) > 0 ) {
         if (primary)
            sscanf(str, "nameserver %s\n", dns);
         else {
            if ( fgets(str, SYS_CMD_LEN, fs) > 0 ) 
            sscanf(str, "nameserver %s\n", dns);
         } /* secondary */
      }
      fclose(fs);
   }
   else
      // if cannot find primary dns info then
      // assign default value which is router IP address
      bcmGetIfDestAddr("br0", dns);
}

/***************************************************************************
// Function Name: bcmGetDns().
// Description  : get DSN info.
// Parameters   : dns - buffer to retrieve primary dns.
// Returns      : none.
****************************************************************************/
void bcmGetDns(char *dns) {
   IFC_DNS_INFO dnsInfo;
   int sts = BcmDb_getDnsInfo(&dnsInfo);

   dns[0] = '\0';
   if ( sts == DB_GET_OK ) {
      if ( dnsInfo.dynamic == TRUE )
         bcmGetDynamicDnsAddr(dns,BCM_PRIMARY_DNS);
      else {
         if ( dnsInfo.preferredDns.s_addr != INADDR_NONE )
            strcpy(dns, inet_ntoa(dnsInfo.preferredDns));
      }	  	
   } else
      // in bcmGetDynamicDnsAddr, if cannot find primary dns info then
      // assign default value which is router IP address
      bcmGetDynamicDnsAddr(dns,BCM_PRIMARY_DNS);
}

/***************************************************************************
// Function Name: bcmGetDns2().
// Description  : get DSN info.
// Parameters   : dns - buffer to retrieve primary dns.
// Returns      : none.
****************************************************************************/
void bcmGetDns2(char *dns) {
   IFC_DNS_INFO dnsInfo;
   int sts = BcmDb_getDnsInfo(&dnsInfo);

   dns[0] = '\0';
   if ( sts == DB_GET_OK ) {
      if ( dnsInfo.dynamic == TRUE )
         bcmGetDynamicDnsAddr(dns,BCM_SECONDARY_DNS);
      else {
         if ( dnsInfo.alternateDns.s_addr != INADDR_NONE )
            strcpy(dns, inet_ntoa(dnsInfo.alternateDns));
      }	  	
   } else
      // in bcmGetDynamicDnsAddr, if cannot find primary dns info then
      // assign default value which is router IP address
      bcmGetDynamicDnsAddr(dns,BCM_SECONDARY_DNS);
}

void bcmGetDnsSettings(int *mode, char *primary, char *secondary)
{
   IFC_DNS_INFO dnsInfo;
   
   *mode = 1;
   if (BcmDb_getDnsInfo(&dnsInfo) == DB_GET_OK) {
      *mode = dnsInfo.dynamic;
   }
   bcmGetDns(primary);
   bcmGetDns2(secondary);
}

/***************************************************************************
// Function Name: bcmRestartDnsProbe().
// Description  : start DNS probe.
// Parameters   : none.
// Returns      : none.
****************************************************************************/
#if defined(SUPPORT_DNSPROBE)
void bcmRestartDnsProbe(void) {
   char cmd[CLI_MAX_BUF_SZ];

   // kill the old dnsprobe if it is existed
   int pid = bcmGetPid("/bin/dnsprobe");
   if ( pid > 0 ) {
      sprintf(cmd, "kill -9 %d", pid);
      bcmSystem(cmd);
   }

   // start the new dnsprobe
   bcmSystem("/bin/dnsprobe &");
}
#endif


#if defined(SUPPORT_DPROXY)
/***************************************************************************
// Function Name: bcmSetDPROXY().
// Description  : Configuration
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmSetDProxyConfig()
{
   FILE* pFile;
   char hostDomain[128]="";
   IFC_LAN_INFO lanInfo2;
   IFC_LAN_INFO lanInfo;
   IFC_DNS_INFO dnsInfo;
   char addr[IFC_TINY_LEN];


   BcmDb_getLanInfo(IFC_ENET_ID, &lanInfo);
   BcmDb_getDnsInfo(&dnsInfo);
   pFile = fopen("/var/hosts", "w");
   if (pFile)
   {
      sprintf(hostDomain,"%s.%s",dnsInfo.hostName,dnsInfo.domainName);
      fprintf(pFile,"%s\t%s\n","127.0.0.1","localhost");
      fprintf(pFile,"%s\t%s\n",inet_ntoa(lanInfo.lanAddress),hostDomain);

      if ( BcmDb_getLanInfo(IFC_SUBNET_ID, &lanInfo2) == DB_GET_OK )
      {
         fprintf(pFile,"%s\t%s\n",inet_ntoa(lanInfo2.lanAddress),hostDomain);
      }
      fclose(pFile);
   } 
   else 
   {
      printf("Cannot access file /var/hosts!\n");
   }
}

/***************************************************************************
// Function Name: bcmStartDPROXY().
// Description  : start DProxy
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmStartDProxy(void) {
   char cmd[CLI_MAX_BUF_SZ];
   FILE* pFile;
   IFC_DNS_INFO dnsInfo;

   // stop dproxy
   bcmStopDProxy();

   // Set DProxy Config
   bcmSetDProxyConfig();

   // Check Configuration File
   pFile = fopen("/var/hosts", "r");
   if(pFile == NULL)
   {
      printf("Cannot access file /var/hosts!\n");
      return;
   }
   fclose(pFile);

   BcmDb_getDnsInfo(&dnsInfo);
   if(dnsInfo.domainName[0] != '\0')
   {
       sprintf(cmd, "dproxy -D %s &", dnsInfo.domainName);
       bcmSystem(cmd);
   }
   else
   {
       // start the new dproxy
       bcmSystem("dproxy &");
   }
}

/***************************************************************************
// Function Name: bcmStopDPROXY().
// Description  : stop DProxy
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmStopDProxy(void) {
   char cmd[CLI_MAX_BUF_SZ];

   // kill the old dproxy if it is existed
   int pid = bcmGetPid("dproxy");
   if ( pid > 0 ) {
      sprintf(cmd, "kill -9 %d", pid);
      bcmSystem(cmd);
   }

   sprintf(cmd, "echo > /var/hosts");
   bcmSystem(cmd);
}
#endif

/***************************************************************************
// Function Name: bcmConfigDns().
// Description  : add or remove DNS info to PSI.
// Parameters   : primary and secondary DNS.
// Returns      : none.
****************************************************************************/
void bcmConfigDns(char *dns1, char *dns2, int dynamic) {
   char cmd[CLI_MAX_BUF_SZ], addr[CLI_MAX_BUF_SZ], buf[CLI_MAX_BUF_SZ];
   IFC_DNS_INFO dnsInfo;
    // need to call getDnsInfo before setDnsInfo to retrieve domain name
   int sts = BcmDb_getDnsInfo(&dnsInfo);

   // get local ip address
   bcmGetIfDestAddr("br0", addr);

   // if user changes from static to auto assgined dns
   if (dynamic) {
      if ( sts == DB_GET_OK &&
           bcmIsModuleInserted("iptable_nat") == TRUE ) {
         strcpy(buf, inet_ntoa(dnsInfo.preferredDns));
      #if defined(SUPPORT_DNSPROBE)
         // del the old rule
         sprintf(cmd, "iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null", addr, buf);
         bcmSystemMute(cmd);
      #endif
         // remove old resolve configuration file
         bcmSystemMute("rm /var/fyi/sys/dns");
      }
   } else {
      // create the new resolv.conf with new dsn info
      bcmCreateResolvCfg(dns1, dns2);
      // get old dns1 info
      bcmGetDns(buf);
      if ( buf[0] != '\0' ) {
         if (strcmp(buf, dns1) != 0) {
            if ( bcmIsModuleInserted("iptable_nat") == TRUE ) {
            #if defined(SUPPORT_DNSPROBE)
               // del the old rule
               sprintf(cmd, "iptables -t nat -D PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s 2>/dev/null", addr, buf);
               bcmSystemMute(cmd);
               // add new DNS Forwarding rule
               sprintf(cmd, "iptables -t nat -A PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s", addr, dns1);
               bcmSystemMute(cmd);
               bcmRestartDnsProbe();
            #endif
            }
         }
      } else {
      #if defined(SUPPORT_DNSPROBE)
         printf("No Existing DNS information from DSL router\n");
         sprintf(cmd, "iptables -t nat -A PREROUTING -i br0 -d %s -p udp --dport 53 -j DNAT --to %s", addr, dns1);
         bcmSystemMute(cmd);
      #endif
      }
      // get old dns2 info
      bcmGetDns2(buf);
      // if old dns2 differs with new one, restart dnsprobe
      if (strcmp(buf, dns2) != 0) {
      #if defined(SUPPORT_DNSPROBE)
         if ( bcmIsModuleInserted("iptable_nat") == TRUE )
            bcmRestartDnsProbe();
      #endif
      }
      dnsInfo.preferredDns.s_addr = inet_addr(dns1);
      dnsInfo.alternateDns.s_addr = inet_addr(dns2);
   } /* dynamic to static */
   dnsInfo.dynamic = dynamic;
   BcmDb_setDnsInfo(&dnsInfo);

#if defined(SUPPORT_DPROXY)
   bcmStartDProxy();
#endif
}

/***************************************************************************
// Function Name: bcmRemoveDefaultGatewayByWanIf().
// Description  : remove the defaultGateway configuration if it uses the
//                removed wan interface.
// Parameters   : wanIf - the removed wan interface
// Returns      : none
****************************************************************************/
void bcmRemoveDefaultGatewayByWanIf(char *wanIf) {
   char gtwy[IFC_TINY_LEN], ifName[IFC_TINY_LEN];
   
   bcmGetDefaultGateway(gtwy, ifName);
   
   if ( strcmp(ifName, wanIf) == 0 ) {
      // remove static default gateway in PSI
      BcmDb_removeDefaultGatewayInfo();
      BcmPsi_flush();
   }
}

/***************************************************************************
// Function Name: bcmGetDefaultGateway().
// Description  : get default gateway info.
// Parameters   : gtwy - buffer to retrieve default gateway.
// Returns      : none.
****************************************************************************/
void bcmGetDefaultGateway(char *gtwy, char *wanIf) {
   char str[SYS_CMD_LEN];
   FILE* fs = NULL;
   IFC_DEF_GW_INFO defgw;
   char addr[512], ip[512];
   int ret;
   gtwy[0] = wanIf[0] = '\0';
   addr[0] = ip[0] = '\0';  //dare sunmin fix http will hang when MER in DHCP mode.

   if ( BcmDb_getDefaultGatewayInfo(&defgw) == DB_GET_OK ){
      if (strcmp(inet_ntoa(defgw.defaultGateway), "0.0.0.0") != 0)
         strcpy(gtwy, inet_ntoa(defgw.defaultGateway));
      if (defgw.ifName != NULL)
         strcpy(wanIf, defgw.ifName);      
   }
   else {
      fs = fopen("/var/fyi/sys/gateway", "r");
      if ( fs != NULL ) {
         if (fgets(str, SYS_CMD_LEN, fs) > 0 ) 
	  {
            ret=sscanf(str, "%s\n", addr);
	     if(ret<=0)
	      		memset(addr,0,sizeof(addr));
            if ( BcmDb_validateIpAddress(addr) == DB_OBJ_VALID_OK ) 
               strcpy(gtwy, addr);
            else { // use ifName
               if (bcmGetIfDestAddr(addr, ip) == BCM_DIAG_PASS)
                  strcpy(gtwy, ip); 
            }
         }
         fclose(fs);
      }
   }
}

/***************************************************************************
// Function Name: bcmGetDefaultGatewayWanIf().
// Description  : get default gateway wan interface from route table.
// Parameters   : gtwy - buffer to retrieve default gateway.
// Returns      : none.
****************************************************************************/
void bcmGetDefaultGatewayWanIf(char *gtwy, char *wanIf) {
   char col[11][32];
   char line[512];
   struct in_addr addr;
   int count = 0;
   int flag = 0;

   FILE* fsRoute = fopen("/proc/net/route", "r");
   if ( fsRoute != NULL ) {
      while ( fgets(line, sizeof(line), fsRoute) ) {
          // read pass header line
         if ( count++ < 1 ) continue;
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[0], col[1], col[2], col[3], col[4], col[5],
                col[6], col[7], col[8], col[9], col[10]);
         flag = strtol(col[3], (char**)NULL, 16);
         if ((flag & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY)) {
             addr.s_addr = strtoul(col[2], (char**)NULL, 16);
             if (strcmp(gtwy, inet_ntoa(addr)) == 0) {
                strcpy(wanIf, col[0]);
             }
         }
      }
      fclose(fsRoute);
   }
}

/***************************************************************************
// Function Name: bcmIsDefaultGatewayExisted().
// Description  : Check if the defaultGateway is existed in the route table.
// Parameters   : gw and wanIf
// Returns      : 1 - Exist. 0 - not Exist
****************************************************************************/
int bcmIsDefaultGatewayExisted(char *gw, char *wanIf) {
   char col[11][32];
   char line[512];
   struct in_addr addr;
   int count = 0;
   int flag = 0;

   FILE* fsRoute = fopen("/proc/net/route", "r");
   if ( fsRoute != NULL ) {
      while ( fgets(line, sizeof(line), fsRoute) ) {
         // read pass header line
        if ( count++ < 1 ) continue;
        sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
               col[0], col[1], col[2], col[3], col[4], col[5],
               col[6], col[7], col[8], col[9], col[10]);
        flag = strtol(col[3], (char**)NULL, 16);
        if ((flag & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY)) {
           if ( wanIf[0] == '\0' || strcmp(wanIf, col[0]) == 0) {
             addr.s_addr = strtoul(col[2], (char**)NULL, 16);
             if (strcmp(gw, inet_ntoa(addr)) == 0) {
                fclose(fsRoute);
                return TRUE;
             }
           }
        }
      }
      fclose(fsRoute);
   }

   return FALSE;
}

/***************************************************************************
// Function Name: bcmIsDefaultGatewayIfName().
// Description  : Check if the IfName is the Default Gateway Interface.
// Parameters   : wanIf
// Returns      : 1 - Yes. 0 - No
****************************************************************************/
int bcmIsDefaultGatewayIfName(char *wanIf) {
   char col[11][32];
   char line[512];
   char *gw= "0.0.0.0";
   struct in_addr addr;
   int count = 0;
   int flag = 0;

   FILE* fsRoute = fopen("/proc/net/route", "r");
    if ( fsRoute != NULL ) 
    {
        while ( fgets(line, sizeof(line), fsRoute) ) 
        {
            if ( count++ < 1 ) continue;
            sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[0], col[1], col[2], col[3], col[4], col[5],
                col[6], col[7], col[8], col[9], col[10]);
            flag = strtol(col[3], (char**)NULL, 16);
            if ((flag & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY)) {
                addr.s_addr = strtoul(col[1], (char**)NULL, 16);
                if (strcmp(gw, inet_ntoa(addr)) == 0) 
                {
                    if(strcmp(wanIf, col[0]) == 0)
                    {
                        fclose(fsRoute);
                        return TRUE;
                    }
                }
            }
        }
        fclose(fsRoute);
    }

   return FALSE;
}

#ifdef SUPPORT_IPV6
/***************************************************************************
// Function Name: bcmIsIPv6Enabled().
// Description  : get IPv6 enable flag.
// Returns      : IPv6 enable.
****************************************************************************/
int bcmIsIPv6Enabled(void)
{
   void *pEnblIPv6;
   int  enblIPv6 = 0;
   int  len = sizeof(int);

   if (BcmCfm_scratchPadGet(ENBL_IPV6, &pEnblIPv6, &len) == BcmCfm_Ok)
   {
      enblIPv6 = *(int *)pEnblIPv6;
      BcmCfm_free(pEnblIPv6);
   }

   return enblIPv6;

}

/***************************************************************************
// Function Name: bcmIsDhcp6cEnabled().
// Description  : get DHCP6c enable flag.
// Returns      : DHCP6c enable.
****************************************************************************/
int bcmIsDhcp6cEnabled(void)
{
   IFC_DHCP6_INFO dhcp6Info;

   if (BcmDb_getDhcp6Info(&dhcp6Info) == DB_GET_OK)
      return dhcp6Info.enblDhcp6c;
   else
      return 0;
}

/***************************************************************************
// Function Name: bcmGetLan6Prefix().
// Description  : get LAN IPv6 prefix.
// Parameters   : lan6Prfx - buffer to retrieve LAN IPv6 prefix.
// Returns      : none.
****************************************************************************/
void bcmGetLan6Prefix(char *lan6Prfx)
{
   IFC_DHCP6_INFO dhcp6Info;

   lan6Prfx[0] = '\0';
   if ( BcmDb_getDhcp6Info(&dhcp6Info) == DB_GET_OK ) {
      if (inet_ntop(AF_INET6, &dhcp6Info.lan6Prefix, lan6Prfx, IFC_IPV6_ADDR_LEN) == NULL ||
          strcmp(lan6Prfx, "::") == 0)
         lan6Prfx[0] = '\0';
   }
}

/***************************************************************************
// Function Name: bcmGetDns6().
// Description  : get static IPv6 DNS servers.
// Parameters   : dns6Pr, dns6Sec - buffer to retrieve DNS servers.
// Returns      : none.
****************************************************************************/
void bcmGetDns6(char *dns6Pri, char *dns6Sec)
{
   IFC_DHCP6_INFO dhcp6Info;

   dns6Pri[0] = dns6Sec[0] = '\0';
   if ( BcmDb_getDhcp6Info(&dhcp6Info) == DB_GET_OK ) {
      if (inet_ntop(AF_INET6, &dhcp6Info.dns6Primary, dns6Pri, IFC_IPV6_ADDR_LEN) == NULL ||
          strcmp(dns6Pri, "::") == 0)
         dns6Pri[0] = '\0';
      if (inet_ntop(AF_INET6, &dhcp6Info.dns6Secondary, dns6Sec, IFC_IPV6_ADDR_LEN) == NULL ||
          strcmp(dns6Sec, "::") == 0)
         dns6Sec[0] = '\0';
   }
}

/***************************************************************************
// Function Name: bcmSetDhcp6Info().
// Description  : set DHCP6 info including enblDhcp6c, lan6Prefix, dns6 servers.
// Parameters   : enblDhcp6c, lan6Prfx, dns6Pri, dns6Sec - Parameters to set.
// Returns      : none.
****************************************************************************/
void bcmSetDhcp6Info(int enblDhcp6c, char *lan6Prfx, char *dns6Pri, char *dns6Sec, char *errMsg)
{
   IFC_DHCP6_INFO dhcp6Info;

   memset(&dhcp6Info, 0, sizeof(IFC_DHCP6_INFO));

   dhcp6Info.enblDhcp6c = enblDhcp6c;
   if (lan6Prfx[0] != '\0' &&
       inet_pton(AF_INET6, lan6Prfx, &dhcp6Info.lan6Prefix) <= 0) {
      printf("bcmSetDhcp6Info: bad LAN prefix: %s\n", lan6Prfx);
      return;  // error. do nothing
   }
   if (dns6Pri[0] != '\0' &&
       inet_pton(AF_INET6, dns6Pri, &dhcp6Info.dns6Primary) <= 0) {
      printf("bcmSetDhcp6Info: bad primary DNS: %s\n", dns6Pri);
      return;  // error. do nothing
   }
   if (dns6Sec[0] != '\0' &&
       inet_pton(AF_INET6, dns6Sec, &dhcp6Info.dns6Secondary) <= 0) {
      printf("bcmSetDhcp6Info: bad secondary DNS: %s\n", dns6Sec);
      return;  // error. do nothing
   }

   BcmDb_setDhcp6Info(&dhcp6Info);

}

/***************************************************************************
// Function Name: bcmGetDefaultGateway6().
// Description  : get default gateway info.
// Parameters   : gtwy - buffer to retrieve default gateway.
// Returns      : none.
****************************************************************************/
void bcmGetDefaultGateway6(char *gtwy, char *wanIf)
{
   IFC_DEF_GW6_INFO defgw;

   gtwy[0] = wanIf[0] = '\0';
   if ( BcmDb_getDefaultGateway6Info(&defgw) == DB_GET_OK ){
      if (inet_ntop(AF_INET6, &defgw.defaultGateway6, gtwy, IFC_IPV6_ADDR_LEN) == NULL ||
          strcmp(gtwy, "::") == 0)
         gtwy[0] = '\0';
      if (defgw.ifName != NULL)
         strcpy(wanIf, defgw.ifName);      
   }
}

/***************************************************************************
// Function Name: bcmRemoveDefaultGateway6ByWanIf().
// Description  : remove the defaultGateway configuration if it uses the
//                removed wan interface.
// Parameters   : wanIf - the removed wan interface
// Returns      : none
****************************************************************************/
void bcmRemoveDefaultGateway6ByWanIf(char *wanIf) {
   char gtwy[IFC_IPV6_ADDR_LEN], ifName[IFC_TINY_LEN];
   
   bcmGetDefaultGateway6(gtwy, ifName);
   
   if ( strcmp(ifName, wanIf) == 0 ) {
      // remove static default gateway in PSI
      BcmDb_removeDefaultGateway6Info();
      BcmPsi_flush();
   }
}
#endif

/***************************************************************************
// Function Name: bcmSetAutoDefaultGateway().
// Description  : remove static default gateway in PSI,
//                get the default gateway from var/fyi/sys/gateway, and add
//                it to the route table.
// Parameters   : errMsg -- error message when add default gateway if any.
// Returns      : 1 - Exist. 0 - not Exist
****************************************************************************/
void bcmSetAutoDefaultGateway(char *errMsg) {
   char sysDefaultGateway[IFC_TINY_LEN];
   char cmd[IFC_LARGE_LEN];
   FILE *fs = NULL;
   IFC_DEF_GW_INFO defgw;

   // remove static default gateway
   if ( BcmDb_getDefaultGatewayInfo(&defgw) == DB_GET_OK ) {
      // remove static default gateway in PSI
      BcmDb_removeDefaultGatewayInfo();
      BcmPsi_flush();
      // create command to delete default gateway in route table
      sprintf(cmd, "route del default");
      if (strcmp(inet_ntoa(defgw.defaultGateway), "0.0.0.0") != 0) {
         strcat(cmd, " gw ");
         strcat(cmd, inet_ntoa(defgw.defaultGateway));
      }
      if (strcmp(defgw.ifName, "") != 0) {
         strcat(cmd, " dev ");
         strcat(cmd, defgw.ifName);
      }
      strcat(cmd, " 2> /var/gwerr");
      bcmSystem(cmd);
      fs = fopen("/var/gwerr", "r");
      // read gwerr, if there is err then
      // need to set error message
      if ( fs != NULL ) {
         if ( fgets(cmd, IFC_LARGE_LEN, fs) > 0 ) {
            // remove the last newline character
            cmd[strlen(cmd) - 1] = '\0';
            strcpy(errMsg, cmd);
            bcmSystemMute("cat /var/gwerr");
         } else
            errMsg[0] = '\0';
         fclose(fs);
         bcmSystemMute("rm /var/gwerr");
      }
   }

   //get the default gateway from var/fyi/sys/gateway
   bcmGetDefaultGateway(sysDefaultGateway, cmd);
   if ( sysDefaultGateway[0] != '\0' &&
        bcmIsDefaultGatewayExisted(sysDefaultGateway, "") == FALSE ) {
      sprintf(cmd, "route add default gw %s 2> /var/gwerr", sysDefaultGateway);
      bcmSystem(cmd);
      fs = fopen("/var/gwerr", "r");
      // read gwerr, if there is err then
      // need to set the error message
      if ( fs != NULL ) {
         if ( fgets(cmd, IFC_LARGE_LEN, fs) > 0 ) {
            // remove the last newline character
            cmd[strlen(cmd) - 1] = '\0';
            strcpy(errMsg, cmd);
            bcmSystemMute("cat /var/gwerr");
         } else
            errMsg[0] = '\0';
         fclose(fs);
         bcmSystemMute("rm /var/gwerr");
      }
   }
}

/***************************************************************************
// Function Name: bcmSetStaticDefaultGateway().
// Description  : remove the old, and add the new default gateway.
// Parameters   : gw, wanIf, and error message
// Returns      : none
****************************************************************************/
void bcmSetStaticDefaultGateway(char *gw, char *wanIf, char *errMsg) {
   int runCmd = FALSE;
   char cmd[IFC_LARGE_LEN];
   FILE* fs = NULL;
   IFC_DEF_GW_INFO defgw;

   // intialize gw if it is empty
   if ( gw[0] == '\0' )
      strcpy(gw, "0.0.0.0");

   // if errMsg is NULL then only save configuration
   // but not execute command to avoid error when
   // route destination address cannot be reached since
   // interface device does not created yet
   if ( errMsg != NULL &&
        BcmDb_getDefaultGatewayInfo(&defgw) == DB_GET_OK ) {
      // check if not the same
      if ( strcmp(defgw.ifName, wanIf) != 0 ||
           strcmp(inet_ntoa(defgw.defaultGateway), gw) != 0) {
         // del the previous default route saved in route table
         sprintf(cmd, "route del default");
         if ( strcmp(inet_ntoa(defgw.defaultGateway), "0.0.0.0") != 0 ) {
            strcat(cmd, " gw ");
            strcat(cmd, inet_ntoa(defgw.defaultGateway));
         }
         if ( strcmp(defgw.ifName, "") != 0 ) {
            strcat(cmd, " dev ");
            strcat(cmd, defgw.ifName);
         }
         strcat(cmd, " 2> /var/gwerr");
         bcmSystemMute(cmd);
         fs = fopen("/var/gwerr", "r");
         // read gwerr, if there is err then
         // need to set error message
         if ( fgets(cmd, IFC_LARGE_LEN, fs) > 0 ) {
            // remove the last newline character
            cmd[strlen(cmd) - 1] = '\0';
            strcpy(errMsg, cmd);
            bcmSystemMute("cat /var/gwerr");
         } else
            errMsg[0] = '\0';
         fclose(fs);
         bcmSystemMute("rm /var/gwerr");
      }
      else
         return; // same and do nothing
   }

   // if errMsg is NULL then only save configuration
   // but not execute command to avoid error when
   // route destination address cannot be reached since
   // interface device does not created yet
   // if this gateway and wanIf is already in the route table,
   // do not issue route add default command.
   if ( errMsg != NULL &&
        bcmIsDefaultGatewayExisted(gw, wanIf) == FALSE ) {
      strcpy(cmd, "route add default");
      if ( strcmp(gw, "0.0.0.0") != 0 &&
           BcmDb_validateIpAddress(gw) == DB_OBJ_VALID_OK ) {
         strcat(cmd, " gw ");
         strcat(cmd, gw);
         runCmd = TRUE;
      }
      if (wanIf[0] != '\0') {
         strcat(cmd, " dev ");
         strcat(cmd, wanIf);
         runCmd = TRUE;
      }
      if ( runCmd == TRUE ) {
         strcat(cmd, " 2> /var/gwerr");
         bcmSystemMute(cmd);
         fs = fopen("/var/gwerr", "r");
         // read gwerr, if there is no err then
         // need to configure default gateway in PSI
         if ( fgets(cmd, IFC_LARGE_LEN, fs) <= 0 ) {
            // save the new default gateway info to psi
            errMsg[0] = '\0';
         } else {
            // remove the last newline character
            cmd[strlen(cmd) - 1] = '\0';
            strcpy(errMsg, cmd);
            bcmSystemMute("cat /var/gwerr");
         }
         // close gwerr file
         fclose(fs);
         bcmSystemMute("rm /var/gwerr");
      }
   }

   // save to PSI even when error occurs
   defgw.enblGwAutoAssign = 0;
   strcpy(defgw.ifName, wanIf);
   defgw.defaultGateway.s_addr = inet_addr(gw);
   BcmDb_setDefaultGatewayInfo(&defgw);
   BcmPsi_flush();
}

#ifdef SUPPORT_IPV6
/***************************************************************************
// Function Name: bcmSetStaticDefaultGateway6().
// Description  : remove the old, and add the new default gateway.
// Parameters   : gw, wanIf, and error message
// Returns      : none
****************************************************************************/
void bcmSetStaticDefaultGateway6(char *gw, char *wanIf, char *errMsg) {
   int runCmd = FALSE;
   char cmd[IFC_LARGE_LEN];
   char gtwy[IFC_IPV6_ADDR_LEN];
   struct in6_addr addr, gwAddr;
   FILE* fs = NULL;
   IFC_DEF_GW6_INFO defgw;

   // If wanIf is empty or NULL, do nothing and return */
   if (gw == NULL || wanIf == NULL)
      return;

   memset(&gwAddr, 0, sizeof(struct in6_addr));
   if (gw[0] != '\0' && inet_pton(AF_INET6, gw, &gwAddr) <= 0) {
      printf("bcmSetStaticDefaultGateway6: bad def gw: %s\n", gw);
      return;  // error. do nothing
   }

   // if errMsg is NULL then only save configuration
   // but not execute command to avoid error when
   // route destination address cannot be reached since
   // interface device does not created yet
   if (errMsg != NULL) {
      errMsg[0] = '\0';    /* clear error message */
      if (BcmDb_getDefaultGateway6Info(&defgw) == DB_GET_OK) {
         // check if not the same
         if (strcmp(defgw.ifName, wanIf) == 0 &&
             bcmp((char *)&defgw.defaultGateway6, (char *)&gwAddr, sizeof(struct in6_addr)) == 0) {
            return; // same and do nothing
         }
         // del the previous default route saved in route table
         inet_ntop(AF_INET6, &defgw.defaultGateway6, gtwy, sizeof(gtwy));
         if (strcmp(gtwy, "::") == 0)
            gtwy[0] = '\0';
         cmd[0] = '\0';
         if (gtwy[0] != '\0' && defgw.ifName[0] != '\0')
            sprintf(cmd, "ip -6 ro del default via %s dev %s 2> /var/gwerr", gtwy, defgw.ifName);
         else if (gtwy[0] != '\0' && defgw.ifName[0] == '\0')
            sprintf(cmd, "ip -6 ro del default via %s 2> /var/gwerr", gtwy);
         else if (gtwy[0] == '\0' && defgw.ifName[0] != '\0')
            sprintf(cmd, "ip -6 ro del default dev %s 2> /var/gwerr", defgw.ifName);
         if (cmd[0] != '\0') {
            bcmSystemMute(cmd);
            fs = fopen("/var/gwerr", "r");
            // read gwerr, if there is err then
            // need to set error message
            if ( fgets(cmd, IFC_LARGE_LEN, fs) > 0 ) {
               // remove the last newline character
               cmd[strlen(cmd) - 1] = '\0';
               strcpy(errMsg, cmd);
               bcmSystemMute("cat /var/gwerr");
            } else
               errMsg[0] = '\0';
            fclose(fs);
            bcmSystemMute("rm /var/gwerr");
         }
      }
      // Now add the default route
      cmd[0] = '\0';
      if (gw[0] != '\0' && wanIf[0] != '\0')
         sprintf(cmd, "ip -6 ro add default via %s dev %s 2> /var/gwerr", gw, wanIf);
      else if (gw[0] != '\0' && wanIf[0] == '\0')
         sprintf(cmd, "ip -6 ro add default via %s 2> /var/gwerr", gw);
      else if (gw[0] == '\0' && wanIf[0] != '\0')
         sprintf(cmd, "ip -6 ro add default dev %s 2> /var/gwerr", wanIf);
      if (cmd[0] != '\0') {
         bcmSystemMute(cmd);
         fs = fopen("/var/gwerr", "r");
         // read gwerr, if there is no err then
         // need to configure default gateway in PSI
         if ( fgets(cmd, IFC_LARGE_LEN, fs) <= 0 ) {
            // save the new default gateway info to psi
            errMsg[0] = '\0';
         } else {
            // remove the last newline character
            cmd[strlen(cmd) - 1] = '\0';
            strcpy(errMsg, cmd);
            bcmSystemMute("cat /var/gwerr");
         }
         // close gwerr file
         fclose(fs);
         bcmSystemMute("rm /var/gwerr");
      }
   }

   // save to PSI even when error occurs
   strcpy(defgw.ifName, wanIf);
   defgw.defaultGateway6 = gwAddr;
   BcmDb_setDefaultGateway6Info(&defgw);
   BcmPsi_flush();
}
#endif

//**************************************************************************
// Function Name: getPppoeServiceName
// Description  : get pppoe service name of the specific wan interfaces.
// Parameters   : service -- pppoe service name. (output)
//                ifName -- interface name. (input)
// Returns      : none.
//**************************************************************************
void getPppoeServiceName(char *service, char *ifName) {
   char fileName[IFC_LARGE_LEN];
   char str[SYS_CMD_LEN];
   FILE* fs = NULL;

   if (ifName[0] != 0) {
      sprintf(fileName, "/proc/var/fyi/wan/%s/servicename", ifName);
      fs = fopen(fileName, "r");
      if (fs != NULL) {
         if (fgets(str, SYS_CMD_LEN, fs) > 0)
            sscanf(str, "%s\n", service);
	 fclose(fs);
      }
   }
}

/***************************************************************************
// Function Name: setWanLinkStatus().
// Description  : get PPP status.
// Parameters   : up - wan link status
// Returns      : none
//                -1 - ERROR
****************************************************************************/
void setWanLinkStatus(int up) 
{
   FILE * fd = NULL;

   if ( up == 1 ) {
      if ( (fd = fopen("/var/run/wanup", "a+")) != NULL )
         fclose(fd);
   } else
      unlink("/var/run/wanup");
}

/***************************************************************************
// Function Name: disconnectPPP().
// Description  : if PPPD is alive, disconnet it.
// Returns      : none
****************************************************************************/
void disconnectPPP(void)
{
   int i;
   pid_t *pid = NULL;

   // If PPP is connected disconnect it first
   if ((pid = bcmGetPidList("pppd")) != NULL)
   {
      for (i = 0; i < IFC_PPP_MAX; i++)
      {
         if (pid[i] > 0)
         {
            kill(pid[i], SIGTERM);
         }
         else
         {
            break;
         }
      }
      if (pid[0] > 0)
      {
         sleep(1);
      }
      free(pid);
   }
}

/***************************************************************************
// Function Name: bcmKillAllApps().
// Description	: kill all available applications.
// Parameters	: none.
// Returns	: none.
****************************************************************************/
void bcmKillAllApps(void) {
    bcmUploadPrepare( getIfName() );
}

/***************************************************************************
// Function Name: bcmReboot().
// Description	: reboot system
// Parameters	: none.
// Returns	: none.
****************************************************************************/
void bcmReboot(void)
{
    disconnectPPP();
    BcmNtwk_unInit();
    sysMipsSoftReset();
}

// function to support encryption password for login
static int i64c(int i) {
    if (i <= 0)
        return ('.');
    if (i == 1)
        return ('/');
    if (i >= 2 && i < 12)
        return ('0' - 2 + i);
    if (i >= 12 && i < 38)
        return ('A' - 12 + i);
    if (i >= 38 && i < 63)
        return ('a' - 38 + i);
    return ('z');
}

// function to support encryption password for login
static char *crypt_make_salt(void) {
    time_t now;
    static unsigned long x;
    static char result[3];

    time(&now);
    x += now + getpid() + clock();
    result[0] = i64c(((x >> 18) ^ (x >> 6)) & 077);
    result[1] = i64c(((x >> 12) ^ x) & 077);
    result[2] = '\0';
    return result;
}

// function to support encryption password for login
static char *pw_encrypt(const char *clear, const char *salt) {
    static char cipher[128];
    char *cp;

#ifdef CONFIG_FEATURE_SHA1_PASSWORDS
    if (strncmp(salt, "$2$", 3) == 0) {
        return sha1_crypt(clear);
    }
#endif
    cp = (char *) crypt(clear, salt);
    /* if crypt (a nonstandard crypt) returns a string too large,
       truncate it so we don't overrun buffers and hope there is
       enough security in what's left */
    if (strlen(cp) > sizeof(cipher)-1) {
        cp[sizeof(cipher)-1] = 0;
    }
    strcpy(cipher, cp);
    return cipher;
}

/***************************************************************************
// Function Name: bcmCreateLoginCfg().
// Description  : create password file for login using 'admin' or 'support'.
// Parameters   : cp_admin - clear password of 'admin'.
//                cp_support - clear password of 'support'.
//                cp_user - clear password of 'user'.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmCreateLoginCfg(char *cp_admin, char *cp_support, char *cp_user) {
   struct passwd pw;
   FILE *fsPwd = NULL, *fsGrp = NULL;

   fsPwd = fopen("/etc/passwd", "w");

   if ( fsPwd != NULL ) {
      // In future, we may change uid of 'admin' and 'support'
      // uclibc may have a bug on putpwent in terms of uid,gid setup
      pw.pw_name = "admin";
      pw.pw_passwd = pw_encrypt(cp_admin, crypt_make_salt());
      pw.pw_uid = 0;
      pw.pw_gid = 0;
      pw.pw_gecos = "Administrator";
      pw.pw_dir = "/";
      pw.pw_shell = "/bin/sh";
      putpwent(&pw, fsPwd);

      pw.pw_name = "support";
      pw.pw_passwd = pw_encrypt(cp_support, crypt_make_salt());
      pw.pw_uid = 0;
      pw.pw_gid = 0;
      pw.pw_gecos = "Technical Support";
      pw.pw_dir = "/";
      pw.pw_shell = "/bin/sh";
      putpwent(&pw, fsPwd);

      pw.pw_name = "user";
      pw.pw_passwd = pw_encrypt(cp_user, crypt_make_salt());
      pw.pw_uid = 0;
      pw.pw_gid = 0;
      pw.pw_gecos = "Normal User";
      pw.pw_dir = "/";
      pw.pw_shell = "/bin/sh";
      putpwent(&pw, fsPwd);

      pw.pw_name = "nobody";
      pw.pw_passwd = pw_encrypt(cp_admin, crypt_make_salt());
      pw.pw_uid = 0;
      pw.pw_gid = 0;
      pw.pw_gecos = "nobody for ftp";
      pw.pw_dir = "/";
      pw.pw_shell = "/bin/sh";
      putpwent(&pw, fsPwd);
      fclose(fsPwd);

      fsGrp = fopen("/etc/group", "w");
      if ( fsGrp != NULL ) {
         fprintf(fsGrp, "root::0:root,admin,support,user\n");
         fclose(fsGrp);
         return FILE_OPEN_OK;
      }
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmSetIpExtension().
// Description  : store PPP IP extension info to file.
// Parameters   : ipExt - 1:enable, 0:disable.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmSetIpExtension(int ipExt) {
   FILE* fs = fopen("/var/ipextension", "w");

   if ( fs != NULL ) {
      fprintf(fs, "%d\n", ipExt);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmGetIpExtension().
// Description  : retrieve PPP IP extension info from file.
// Parameters   : str -- buffer.
//                len -- size of buffer.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmGetIpExtension(char *str, int len) {
   FILE* fs = fopen("/var/ipextension", "r");

   if ( fs != NULL ) {
      fgets(str, len, fs);
      fclose(fs);
      return FILE_OPEN_OK;
   }

   return FILE_OPEN_ERR;
}

/***************************************************************************
// Function Name: bcmGetIfcIndexByName().
// Description  : get interface index by its name.
// Parameters   : ifcIdx -- interface index.
//                ifcName -- interface name.
// Returns      : interface index
****************************************************************************/
int bcmGetIfcIndexByName(char *ifcName) {
   struct ifreq ifr;
   int s = 0;

   if ( ifcName == NULL ) return -1;

   if ( (s = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) return -1;

   strcpy(ifr.ifr_name, ifcName);
   if ( ioctl(s, SIOCGIFINDEX, &ifr) < 0 ) {
      close(s);
      return 0;
   }

   close(s);

   return ifr.ifr_ifindex;
}

/***************************************************************************
// Function Name: bcmIsValidIfcName.
// Description  : validate the interface name.
// Parameters   : ifcName -- interface name that need to validate.
// Returns      : TRUE or FALSE
****************************************************************************/
int bcmIsValidIfcName(char *ifcName) {
   int ret = FALSE;

   if (bcmGetIfcIndexByName(ifcName))
      ret = TRUE;
   return ret;
}

/***************************************************************************
// Function Name: BcmGetIfcNumOfSwith.
// Description  : Get the Interface number of this swith.
// Parameters   : ifName -- interface name 
// Returns      : 
****************************************************************************/
int BcmGetIfcNumOfSwith(char * ifName)
{
	char cmd[IFC_LARGE_LEN];
    FILE *errFs = NULL;
    char str[IFC_LARGE_LEN];
	int	numIfc = 0;

	sprintf(cmd, "ethctl %s vport query 2>/var/vcfgerr\n", ifName);
    bcmSystemMute(cmd);
    errFs = fopen("/var/vcfgerr", "r");

    if (errFs != NULL ) {
        fgets(str, IFC_LARGE_LEN, errFs);
        numIfc = atoi(str);
        fclose(errFs);
        bcmSystemMute("rm /var/vcfgerr");
    }
	return numIfc;
}
/***************************************************************************
// Function Name: BcmGetEthIndexToIfName.
// Description  : get interface name by its id (defined in ifcdefs.h).
// Parameters   : index -- Ethernet interface Id.
//                ifName -- interface name.
// Returns      : interface name or NULL
****************************************************************************/
int BcmGetEthIndexToIfName(unsigned long index,char* ifName)
{
	struct lan_ifname_info *selected;
    int  numEnet, numIfc , numGroup;
#ifdef SUPPORT_PORT_MAP	
    PORT_MAP_IFC_STATUS_INFO PMapIfcCfgInfo;
#endif
	struct lan_ifname_info group_0[] = {
        {"ENET", IFC_ENET_ID, "eth0"},
        {"\0", 0, "\0"}
    };

    struct lan_ifname_info group_1[] = {
        {"ENET(1-4)", IFC_ENET_ID, "eth0"},
        {"\0", 0, "\0"}
    };

    struct lan_ifname_info group_2[] = {
        {"ENET1", IFC_ENET0_VNET_ID + 5, "eth0.5"},
        {"ENET2", IFC_ENET0_VNET_ID + 4, "eth0.4"},
        {"ENET3", IFC_ENET0_VNET_ID + 3, "eth0.3"},
        {"ENET4", IFC_ENET0_VNET_ID + 2, "eth0.2"},
        {"\0", 0, "\0"}
    };

    struct lan_ifname_info group_3[] = {
        {"ENET1", IFC_ENET_ID ,"eth0"},
        {"ENET2", IFC_ENET1_ID ,"eth1"},
        {"\0", 0, "\0"}
    };

    struct lan_ifname_info group_4[] = {
        {"ENET4", IFC_ENET_ID, "eth0"},
        {"ENET(1-3)", IFC_ENET1_ID, "eth1"},
        {"\0", 0, "\0"}
    };

    struct lan_ifname_info group_5[] = {
        {"ENET4", IFC_ENET_ID ,"eth0"},
        {"ENET1", IFC_ENET1_VNET_ID + 2 ,"eth1.2"},
        {"ENET2", IFC_ENET1_VNET_ID + 3	,"eth1.3"},
        {"ENET3", IFC_ENET1_VNET_ID + 4 ,"eth1.4"},
        {"\0", 0, "\0"}
    };

	if( index > IFC_LAN_MAX )
		return -1;
	
	numEnet = numIfc = numGroup = 0;
#ifdef SUPPORT_PORT_MAP
	BcmDb_getPMapIfcCfgInfo(&PMapIfcCfgInfo);
	numIfc = BcmGetIfcNumOfSwith(PMapIfcCfgInfo.ifName);
	numEnet = sysGetNumEnetMacs();
	if (numEnet == 1) 
	{
        if (numIfc == 1) 
		{
            selected = group_0;
			numGroup = 0;
        }
        else if (PMapIfcCfgInfo.status == 0) 
		{
            selected = group_1;
			numGroup = 1;
        }
        else 
		{
            selected = group_2;
			numGroup = 2;
        }
    } 
	else 
	{
        if (numIfc == 1) 
		{
            selected = group_3;
			numGroup = 3;
        }
        else if (PMapIfcCfgInfo.status == 0) 
		{
            selected = group_4;
			numGroup = 4;
        }
        else 
		{
            selected = group_5;
			numGroup = 5;
        }
    }
#else
	numIfc = 1;
	numEnet = sysGetNumEnetMacs();
	if (numEnet == 1) 
	{
        if (numIfc == 1) 
		{
            selected = group_0;
			numGroup = 0;
        }
        else 
		{
            selected = group_1;
			numGroup = 1;
        }
    } 
	else 
	{
        if (numIfc == 1) 
		{
            selected = group_3;
			numGroup = 3;
        }
        else 
		{
            selected = group_4;
			numGroup = 4;
        }
    }

#endif
	selected = selected + index -1;
	strcpy(ifName,selected->ifName);
	if(ifName)
	    return numGroup;
	else
		return -1;
 
}

void BcmLan_ExtConfig(char* ethInfo, int objIndex) {

	PETH_CFG_INFO pEthExtInfo;
#ifdef SUPPORT_PORT_MAP
    PORT_MAP_IFC_STATUS_INFO PMapIfcCfgInfo;
#endif	
	char speedType[16];
    int numEnet, numIfc;
    char cmd[128];
    char ifName[IFC_TINY_LEN];

	pEthExtInfo = ethInfo;

	if ( (pEthExtInfo->ethSpeed == ETH_SPEED_AUTO)||pEthExtInfo->ethType == ETH_TYPE_AUTO ) 
            strcpy( speedType, "auto" );
    else {
        if ( pEthExtInfo->ethSpeed == ETH_SPEED_100 ) 
            strcpy( speedType, "100" );
         else 
            strcpy( speedType, "10" );
        
        if ( pEthExtInfo->ethType == ETH_TYPE_FULL_DUPLEX ) 
            strcat( speedType, "FD" );
        else 
            strcat( speedType, "HD" );
    }
#ifdef SUPPORT_PORT_MAP	
    numIfc=0;
	BcmDb_getPMapIfcCfgInfo(&PMapIfcCfgInfo);
	numIfc = BcmGetIfcNumOfSwith(PMapIfcCfgInfo.ifName);
	numEnet = sysGetNumEnetMacs();
	if (numEnet == 1) 
	{
		if (numIfc == 1) 
			sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
		else if (PMapIfcCfgInfo.status == 0) 
			sprintf( cmd, "ethctl eth0 media-type %s port 0 2> /var/ethsts", speedType);
		else 
			sprintf( cmd, "ethctl eth0 media-type %s port %d 2> /var/ethsts", speedType, objIndex -1);
	} 
	else 
	{
		if (numIfc == 1) 
		{
			if(objIndex == 1) 
				sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
			else 
				sprintf( cmd, "ethctl eth1 media-type %s port %d 2> /var/ethsts", speedType, objIndex -2);
		}
		else if (PMapIfcCfgInfo.status == 0) 
		{
			if(objIndex == 1) 
				sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
			else 
				sprintf( cmd, "ethctl eth1 media-type %s port %d 2> /var/ethsts", speedType, objIndex -2);
		}
		else 
		{
			if(objIndex == 1)
				sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
			else 
				sprintf( cmd, "ethctl eth1 media-type %s port %d 2> /var/ethsts", speedType, objIndex -2);
		}
	}
#else
	numIfc = 1;
	numEnet = sysGetNumEnetMacs();

	if (numEnet == 1) 
	{
		if (numIfc == 1) 
			sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
		else 
			sprintf( cmd, "ethctl eth0 media-type %s port 0 2> /var/ethsts", speedType);
	} 
	else 
	{
		if (numIfc == 1) 
		{
			if(objIndex == 1) 
				sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
			else 
				sprintf( cmd, "ethctl eth1 media-type %s port %d 2> /var/ethsts", speedType, objIndex -2);
		}
		else 
		{
			if(objIndex == 1) 
				sprintf( cmd, "ethctl eth0 media-type %s 2> /var/ethsts", speedType);
			else 
				sprintf( cmd, "ethctl eth1 media-type %s port %d 2> /var/ethsts", speedType, objIndex -2);
		}
	}
#endif
	bcmSystemMute(cmd);

	bcmGetIfcNameById(objIndex, ifName);

	sprintf( cmd ,"ifconfig %s %s", ifName, (!pEthExtInfo->ethIfcDisabled)?"up":"down" );
	
    bcmSystemMute(cmd);
}
/***************************************************************************
// Function Name: bcmGetIfcNameById.
// Description  : get interface name by its id (defined in ifcdefs.h).
// Parameters   : ifcId -- interface Id.
//                ifcName -- interface name.
// Returns      : interface name or NULL
****************************************************************************/
char *bcmGetIfcNameById(int ifcId, char *ifcName) {
   if ( ifcName == NULL ) return NULL;

   ifcName[0] = '\0';

   if ( ifcId >= IFC_ENET_ID && ifcId < IFC_USB_ID ) {

#ifdef SUPPORT_PORT_MAP
       if(BcmGetEthIndexToIfName(ifcId,ifcName)==-1)
      sprintf(ifcName, "eth%d", ifcId - IFC_ENET_ID);
#else
      sprintf(ifcName, "eth%d", ifcId - IFC_ENET_ID);
#endif
   }
   else if ( ifcId >= IFC_USB_ID && ifcId < IFC_HPNA_ID )
      sprintf(ifcName, "usb%d", ifcId - IFC_USB_ID);
   else if ( ifcId >= IFC_HPNA_ID && ifcId < IFC_WIRELESS_ID )
      sprintf(ifcName, "il%d", ifcId - IFC_HPNA_ID);
   else if ( ifcId >= IFC_WIRELESS_ID && ifcId < IFC_WIRELESS_ID + IFC_LAN_MAX*IFC_WLAN_MAX ) {
      int num = (ifcId - IFC_WIRELESS_ID)%IFC_WLAN_MAX;
      if (num == 0) { // multiple ssid support
         sprintf(ifcName, "wl%d", (ifcId - IFC_WIRELESS_ID)/IFC_LAN_MAX);
      }
      else {
         sprintf(ifcName, "wl%d.%d",(ifcId - IFC_WIRELESS_ID)/IFC_LAN_MAX, num); 
      }
   }
   else if (ifcId >= IFC_ENET0_VNET_ID && ifcId < IFC_ENET0_VNET_ID + IFC_LAN_MAX + 2)
       sprintf(ifcName, "eth0.%d", ifcId - IFC_ENET0_VNET_ID);
   else if (ifcId >= IFC_ENET1_VNET_ID && ifcId < IFC_ENET1_VNET_ID + IFC_LAN_MAX + 2)
       sprintf(ifcName, "eth1.%d", ifcId - IFC_ENET1_VNET_ID);

   return ifcName;
}

/***************************************************************************
// Function Name: bcmGetIfcNameByIpAddr.
// Description  : get interface name by its IP address.
// Parameters   : ifcIpAddr -- interface IP address.
//                ifcName -- interface name.
// Returns      : interface name or NULL
****************************************************************************/
char *bcmGetIfcNameByIpAddr(unsigned long ipAddr, char *ifcName) {
    char str[SYS_CMD_LEN] = "ifconfig -a > /var/ifcs";
    FILE *fs;
    char *ret = NULL;

    bcmSystemMute(str);
    if( (fs = fopen("/var/ifcs", "r")) != NULL ) {
        while( fgets(str, sizeof(str), fs) != NULL ) {
            if( str[0] >= 'A' && str[0] <= 'z' ) {
                int i;
                char *p;
                char name[16];

                // Copy interface name (br0, eth0, pppoe_0_35, etc.) to local
                // variable.
                for(i = 0, p = str; i<sizeof(name)-1 && *p && *p!=' '; i++, p++)
                    name[i] = *p;
                name[i] = '\0';

                // The next line will have the IP address if one is assigned
                // to the current interface.
                if( fgets(str, sizeof(str), fs) != NULL ) {
                    if( (p = strstr(str, "inet addr:")) != NULL ) {
                        unsigned long addr;

                        p += strlen("inet addr:");
                        addr = inet_addr(p);

                        // if the IP address of the current interfaces matches
                        // the supplied IP address, then the interface is found.
                        if( addr == ipAddr ) {
                            strcpy( ifcName, name );
                            ret = ifcName;
                            break;
                        }
                    }
                }
            }
        }
        fclose(fs);
        unlink("/var/ifcs");
    }

    return( ret );
}

/***************************************************************************
// Function Name: bcmSetConnTrackMax.
// Description  : tune the connection track table size.
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmSetConnTrackMax(void) {
   int conntrack_max = 1000;
   char cmd[SYS_CMD_LEN];
   FILE* fs = fopen("/proc/sys/net/ipv4/ip_conntrack_max", "r");

   // init ip_conntrack_max
   if ( fs == NULL ) // ip_conntrack module is not loaded.
      return;	 
   else {
      fgets(cmd, SYS_CMD_LEN, fs);
      conntrack_max = atoi(cmd);
      fclose(fs);
   }
   // Code in ip_conntrack_init function of ip_conntrack_core.c in kernel
   // has been modified to initialize ip_conntrack_max to be always 0
   // when ip_conntrack module is inserted (before NAT or Firewall is enabled).
   // This function setting real conntrack_max will be called after NAT or Firewall is enabled.
   if ( conntrack_max == 0 )
	conntrack_max = 1000;
   else {   
      if ( conntrack_max < 1000 ) {
         conntrack_max *= 2;
         if ( conntrack_max > 1000 )
            conntrack_max = 1000;
      }
   }
   sprintf(cmd, "echo \"%d\" > /proc/sys/net/ipv4/ip_conntrack_max", conntrack_max);
   bcmSystem(cmd);
}

/***************************************************************************
// Function Name: bcmResetConnTrackTable.
// Description  : reset the connection track table.
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmResetConnTrackTable(void) {
   if ( bcmIsModuleInserted("ip_conntrack") == TRUE )
      bcmSystem("echo > /proc/net/ip_conntrack");
}

/***************************************************************************
// Function Name: bcmHandleConnTrack.
// Description  : handle the connection track table.
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmHandleConnTrack(void) {
   bcmResetConnTrackTable();
   bcmSetConnTrackMax();
}

/***************************************************************************
// Function Name: bcmInsertModules.
// Description  : insert all modules under the given path.
// Parameters   : path -- the given path.
// Returns      : none.
****************************************************************************/
void bcmInsertModules(char *path) {
   struct dirent *entry;
   DIR *dir;
   char *cp = NULL;
   if ( path == NULL ) return;

   dir = opendir(path);
   if ( dir == NULL ) return;

   /* Due to dependencies between conntrack modules and NAT modules,
    * we want to insert all the conntrack modules first.
    */
   while ( (entry = readdir(dir)) != NULL )
   {
      if (strstr(entry->d_name, "conntrack") == NULL) {
         continue;
      }
#if defined(SUPPORT_DYNAHELPER)
      if ( (cp = strstr(entry->d_name, "_ftp")) != NULL ) {
         continue;
      } else if ( (cp = strstr(entry->d_name, "h323")) != NULL ) {
         continue;
      } else if ( (cp = strstr(entry->d_name, "ipsec")) != NULL ) {
         continue;
      } else
#endif
      if ( (cp = strstr(entry->d_name, ".ko")) != NULL ) {
         //dare zy add temp
         if (strstr(entry->d_name, "_pptp") != NULL) {
            continue;
         }
         if (strstr(entry->d_name, "_gre") != NULL) {
            continue;
         }
         //add end
         *cp = '\0';
         bcmInsertModule(entry->d_name);
      }
   }

   /* Now insert all the other modules in the directory. */
   rewinddir(dir);
   while ( (entry = readdir(dir)) != NULL )
   {
      if (strstr(entry->d_name, "conntrack") != NULL) {
         continue;
      }
#if defined(SUPPORT_DYNAHELPER)
      if ( (cp = strstr(entry->d_name, "_ftp")) != NULL ) {
         continue;
      } else if ( (cp = strstr(entry->d_name, "h323")) != NULL ) {
         continue;
      } else if ( (cp = strstr(entry->d_name, "ipsec")) != NULL ) {
         continue;
      } else
#endif
      if ( (cp = strstr(entry->d_name, ".ko")) != NULL ) {
         //dare zy add temp
         if (strstr(entry->d_name, "_pptp") != NULL) {
            continue;
         }
         if (strstr(entry->d_name, "_gre") != NULL) {
            continue;
         }
         //add end
         *cp = '\0';
         bcmInsertModule(entry->d_name);
      }
      }
   closedir(dir);
}

/***************************************************************************
// Function Name: bcmInsertModule.
// Description  : insert module with the given name.
// Parameters   : modName -- the given module name.
// Returns      : none.
****************************************************************************/
void bcmInsertModule(char *modName) {
   char cmd[SYS_CMD_LEN], modulepath[SYS_CMD_LEN];
   struct utsname kernel;

   if (uname(&kernel) == -1)
      return;

#if defined(SUPPORT_DYNAHELPER)
   if ((strstr(modName, "_ftp")  != NULL) ||
       (strstr(modName, "ipsec") != NULL) ||
       (strstr(modName, "h323")  != NULL)) {
      return;
   }
#endif
#if !defined(SUPPORT_DYNAHELPER)
#if !defined(SUPPORT_TOD)
   if ((strstr(modName, "_queue")  != NULL) ||
       (strstr(modName, "_raw")  != NULL)) {
      return;
   }
#endif
#endif
#if defined(STATIC_KERNEL_NF_FIREWALL)
   if ((strcmp(modName, "ip_tables") == 0) ||
       (strcmp(modName, "ip_queue") == 0) ||
       (strcmp(modName, "iptable_raw") == 0) ||
       (strcmp(modName, "ipt_limit") == 0) ||
       (strcmp(modName, "ipt_connlimit") == 0) ||
       (strcmp(modName, "ipt_TCPMSS") == 0) ||
       (strcmp(modName, "iptable_filter") == 0)) {
      return;
   }
#endif
#if defined(STATIC_KERNEL_NF_MANGLE)
   if ((strcmp(modName, "iptable_mangle") == 0) ||
       (strcmp(modName, "ipt_FTOS") == 0) ||
       (strcmp(modName, "ipt_mark") == 0) ||
       (strcmp(modName, "ipt_MARK") == 0)) {
      return;
   }
#endif
#if defined(STATIC_KERNEL_NF_NAT)
   if ((strcmp(modName, "ip_conntrack") == 0) ||
       (strcmp(modName, "iptable_nat") == 0) ||
       (strcmp(modName, "ipt_MASQUERADE") == 0) ||
       (strcmp(modName, "ipt_NOTRACK") == 0) ||
       (strcmp(modName, "ipt_REDIRECT") == 0) ||
       (strcmp(modName, "ipt_state") == 0)) {
      return;
   }
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_FTP)
   if ((strstr(modName, "_ftp")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_SIP)
   if ((strstr(modName, "sip")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_TFTP)
   if ((strstr(modName, "_tftp")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_H323)
   if ((strstr(modName, "h323")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_IRC)
   if ((strstr(modName, "irc")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_PPTP)
   if ((strstr(modName, "gre")  != NULL) || 
       (strstr(modName, "pptp")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_RTSP)
   if ((strstr(modName, "rtsp")  != NULL))
      return;
#endif
#if defined(STATIC_KERNEL_NF_NAT_ALG_SNMP)
   if ((strstr(modName, "snmp")  != NULL))
      return;
#endif
   sprintf(modulepath, "/lib/modules/%s/kernel/net/ipv4/netfilter", kernel.release);

   if ( bcmIsModuleInserted(modName) == FALSE ) {
      sprintf(cmd, "insmod %s/%s.ko", modulepath,modName);
      bcmSystemMute(cmd);
   }
}

/***************************************************************************
// Function Name: bcmRemoveIpTableRule().
// Description  : remove IP table rules.
// Parameters   : device -- interface name.
//                table -- IP table name.
//                chain -- IP table chain.
// Returns      : 1 - Success. 0 - Fail
****************************************************************************/
int bcmRemoveIpTableRule(char *device, char *table, char *chain) {
   char col[IP_TBL_COL_MAX][IFC_SMALL_LEN];
   char comment[IFC_LARGE_LEN], line[IFC_GIANT_LEN];
   int ret = FALSE, count = 0;
   FILE* fs = NULL;

   if ((access("/bin/iptables",F_OK)) != 0)
      return FALSE;

   // execute iptables command to create iptable file
   sprintf(line, "iptables -t %s -L %s -v --line-numbers > /var/iptable",
           table, chain);
   bcmSystemNoHang(line);

   fs = fopen("/var/iptable", "r");
   if ( fs != NULL ) {
      while ( fgets(line, sizeof(line), fs) ) {
         // read pass 2 header lines
         if ( count++ < 2 ) continue;
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[IP_TBL_COL_NUM], col[IP_TBL_COL_PKTS], col[IP_TBL_COL_BYTES],
                col[IP_TBL_COL_TARGET], col[IP_TBL_COL_PROT], col[IP_TBL_COL_OPT],
                col[IP_TBL_COL_IN], col[IP_TBL_COL_OUT], col[IP_TBL_COL_SRC],
                col[IP_TBL_COL_DST], comment);
         // if chain rule for the given device is found
         if ( strcmp(col[IP_TBL_COL_IN], device) == 0 ||
              strcmp(col[IP_TBL_COL_OUT], device) == 0 ) {
            sprintf(line, "iptables -t %s -D %s %s 2>/dev/null",
                    table, chain, col[IP_TBL_COL_NUM]);
            bcmSystemNoHang(line);
            ret = TRUE;
            break;
         }
      }
      fclose(fs);
   }

   // codes to remove iptable file is moved to bcmRemoveAllIpTableRules() function

   return ret;
}

/***************************************************************************
// Function Name: bcmRemoveAllIpTableRules().
// Description  : remove all IP table rules attach with the given device.
// Parameters   : device -- interface name.
// Returns      : none
****************************************************************************/
void bcmRemoveAllIpTableRules(char *device) {
   FILE* fs = NULL;

   if ( bcmIsModuleInserted("iptable_filter") == TRUE ) {
      while ( bcmRemoveIpTableRule(device, "filter", "INPUT") == TRUE )
         ;

      while ( bcmRemoveIpTableRule(device, "filter", "FORWARD") == TRUE )
         ;

      while ( bcmRemoveIpTableRule(device, "filter", "OUTPUT") == TRUE )
         ;
   }

   if ( bcmIsModuleInserted("iptable_nat") == TRUE ) {
      while ( bcmRemoveIpTableRule(device, "nat", "PREROUTING") == TRUE )
         ;
/* We should keep the masquerading rules
      while ( bcmRemoveIpTableRule(device, "nat", "POSTROUTING") == TRUE )
         ;
*/

      while ( bcmRemoveIpTableRule(device, "nat", "OUTPUT") == TRUE )
         ;
   }

   // remove iptable file
   fs = fopen("/var/iptable", "r");
   if ( fs != NULL ) {
      fclose(fs);
      unlink("/var/iptable");
   }
}

/***************************************************************************
// Function Name: bcmRemoveRipIpTableRule().
// Description  : remove RIP IP table rules.
// Parameters   : none.
// Returns      : 1 - Success. 0 - Fail
****************************************************************************/
int bcmRemoveRipIpTableRule(void) {
   char col[IP_TBL_COL_MAX][IFC_SMALL_LEN];
   char comment[IFC_LARGE_LEN], line[IFC_GIANT_LEN];
   int ret = FALSE, count = 0;
   FILE* fs = NULL;

   if ((access("/bin/iptables",F_OK)) != 0)
      return FALSE;

   // execute iptables command to create iptable file
   sprintf(line, "iptables -L INPUT -v --line-numbers > /var/iptable");
   bcmSystemMute(line);

   fs = fopen("/var/iptable", "r");
   if ( fs != NULL ) {
      while ( fgets(line, sizeof(line), fs) ) {
         // read pass 2 header lines
         if ( count++ < 2 ) continue;
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[IP_TBL_COL_NUM], col[IP_TBL_COL_PKTS], col[IP_TBL_COL_BYTES],
                col[IP_TBL_COL_TARGET], col[IP_TBL_COL_PROT], col[IP_TBL_COL_OPT],
                col[IP_TBL_COL_IN], col[IP_TBL_COL_OUT], col[IP_TBL_COL_SRC],
                col[IP_TBL_COL_DST], comment);
         // if protocol column is "udp" and last colum is "udp dpt:route"
         // then delete rule since it is RIP IP tables rule
         if ( strcmp(col[IP_TBL_COL_PROT], "udp") == 0 &&
              strcmp(comment, "udp dpt:route") == 0 ) {
            sprintf(line, "iptables -D INPUT %s 2>/dev/null", col[IP_TBL_COL_NUM]);
            bcmSystemMute(line);
            ret = TRUE;
            break;
         }
      }
      fclose(fs);

      // remove iptable file
      unlink("/var/iptable");
   }

   return ret;
}

/***************************************************************************
// Function Name: bcmRemoveUpnpIpTableRule().
// Description  : remove UPnP IP table rules.
// Parameters   : none.
// Returns      : 1 - Success. 0 - Fail
****************************************************************************/
int bcmRemoveUpnpIpTableRule(void) {
   char col[IP_TBL_COL_MAX][IFC_SMALL_LEN];
   char comment[IFC_LARGE_LEN], line[IFC_GIANT_LEN];
   int ret = FALSE, count = 0;
   FILE* fs = NULL;

   if ((access("/bin/iptables",F_OK)) != 0)
      return FALSE;

   // execute iptables command to create iptable file
   sprintf(line, "iptables -L OUTPUT -v --line-numbers > /var/iptable");
   bcmSystemMute(line);

   fs = fopen("/var/iptable", "r");
   if ( fs != NULL ) {
      while ( fgets(line, sizeof(line), fs) ) {
         // read pass 2 header lines
         if ( count++ < 2 ) continue;
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[IP_TBL_COL_NUM], col[IP_TBL_COL_PKTS], col[IP_TBL_COL_BYTES],
                col[IP_TBL_COL_TARGET], col[IP_TBL_COL_PROT], col[IP_TBL_COL_OPT],
                col[IP_TBL_COL_IN], col[IP_TBL_COL_OUT], col[IP_TBL_COL_SRC],
                col[IP_TBL_COL_DST], comment);
         // if destination column is "239.255.255.250" and target colum is "DROP"
         // then delete rule since it is RIP IP tables rule
         if ( strcmp(col[IP_TBL_COL_TARGET], "DROP") == 0 &&
              strcmp(col[IP_TBL_COL_DST], UPNP_IP_ADDRESS) == 0 ) {
            sprintf(line, "iptables -D OUTPUT %s 2>/dev/null", col[IP_TBL_COL_NUM]);
            bcmSystemMute(line);
            ret = TRUE;
            break;
         }
      }
      fclose(fs);

      // remove iptable file
      unlink("/var/iptable");
   }

   return ret;
}

/***************************************************************************
// Function Name: bcmInsertAllUpnpIpTableRules().
// Description  : insert UPnP IP table rules.
// Parameters   : none.
// Returns      : none.
****************************************************************************/
void bcmInsertAllUpnpIpTableRules(void) {
   char interface[IFC_TINY_LEN], cmd[IFC_LARGE_LEN];
   WAN_CON_ID wanId;
   WAN_CON_INFO wanInfo;

   // init wanId to get WAN info from the begining
   wanId.portId = wanId.vpi = wanId.vci = wanId.conId = 0;
   
   while ( BcmDb_getWanInfoNext(&wanId, &wanInfo) == DB_WAN_GET_OK ) {
      if ( wanInfo.flag.service == FALSE ) continue;
      if ( wanInfo.flag.nat == TRUE ) {
         // get interface name
         BcmDb_getWanInterfaceName(&wanId, wanInfo.protocol, interface);
         // If the br0 interface goes down and then comes back up, we do not need to
         // restart UPnP. All UPnP object values are obtained from the actual WAN
         // interface /dev/bcmadsl0 and /dev/bcmatm0.
         if ( bcmGetPid("upnp") <= 0 ) {
            sprintf(cmd, "upnp -L %s -W %s -D", "br0", interface);
            bcmSystem(cmd);
         }
         // Stop multicast reports for UPnP on WAN.
         sprintf(cmd, "iptables -t filter -I OUTPUT -o %s -d %s -j DROP 2>/dev/null",
                 interface, UPNP_IP_ADDRESS);
         bcmSystemMute(cmd);
      }
   }
}

/***************************************************************************
// Function Name: bcmRemoveEbTableRule().
// Description  : remove ebtables rules.
// Parameters   : device -- interface name.
//                table -- ebtables table name.
//                chain -- ebtables table chain.
// Returns      : 1 - Success. 0 - Fail
****************************************************************************/
int bcmRemoveEbTableRule(char *device, char *table, char *chain) {
   char line[IFC_GIANT_LEN];
   int ret = FALSE, count = 0, index = 0;
   FILE* fs = NULL;

   if ((access("/bin/ebtables",F_OK)) != 0)
      return FALSE;

   // execute iptables command to create iptable file
   sprintf(line, "ebtables -t %s -L %s --Ln > /var/ebtable", table, chain);
   bcmSystemMute(line);

   fs = fopen("/var/ebtable", "r");
   if ( fs != NULL ) {
      while ( fgets(line, sizeof(line), fs) ) {
         // read pass 3 header lines
         if ( count++ < 3 ) continue;
         // if chain rule for the given device is found
         if ( strstr(line, device) != NULL ) {
	    // get the rule index number
	    sscanf(line, "%d.", &index);
            sprintf(line, "ebtables -t %s -D %s %d 2>/dev/null", table, chain, index);
            bcmSystemMute(line);
            ret = TRUE;
            break;
         }
      }
      fclose(fs);
   }

   // code to remove ebtable file is moved to bcmRemoveAllEbTableRules() function

   return ret;
}

/***************************************************************************
// Function Name: bcmRemoveAllEbTableRules().
// Description  : remove all ebtables rules attach with the given device.
//                Each time, only one rule is removed, hence the rule index changes
//                That's why we need to use while loop to remove all
// Parameters   : device -- interface name.
// Returns      : none
****************************************************************************/
void bcmRemoveAllEbTableRules(char *device) {
// while ( bcmRemoveEbTableRule(device, "filter", "INPUT") == TRUE )
//         ;

   while ( bcmRemoveEbTableRule(device, "filter", "FORWARD") == TRUE )
         ;

// while ( bcmRemoveEbTableRule(device, "filter", "OUTPUT") == TRUE )
//         ;

   // remove ebtable file
   unlink("/var/ebtable");
}


/***************************************************************************
// Function Name: bcmGetDefaultRouteInterfaceName().
// Description  : get interface name that is used for first default route.
// Parameters   : ifcName -- the return interface name, '\0' if not found.
// Returns      : none
****************************************************************************/
void bcmGetDefaultRouteInterfaceName(char *ifcName) {
   char col[11][IFC_SMALL_LEN];
   char line[IFC_GIANT_LEN];
   int count = 0;

   if ( ifcName == NULL ) return;

   ifcName[0] = '\0';

   FILE* fsRoute = fopen("/proc/net/route", "r");
   if ( fsRoute != NULL ) {
      while ( fgets(line, sizeof(line), fsRoute) ) {
         // read pass header line
         if ( count++ < 1 ) continue;
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
                col[0], col[1], col[2], col[3], col[4], col[5],
                col[6], col[7], col[8], col[9], col[10]);
         // if destination && mask are 0 then it's default route
         if ( strcmp(col[1], "00000000") == 0 &&
              strcmp(col[7], "00000000") == 0 ) {
            strcpy(ifcName, col[0]);
            break;
         }
      }
      fclose(fsRoute);
   }
}

//***************************************************************************
// Function Name: parseStrInfo
// Description  : parse info to get value of the given variable.
// Parameters   : info -- information string.
//                var -- variable string to be searched in info string.
//                val -- value string after variable string to be returned.
//                len -- size of value string.
// Returns      : none.
//***************************************************************************/
void parseStrInfo(char *info, char *var, char *val, int len) {
   char *pChar = NULL;
   int i = 0;

   if ( info == NULL || var == NULL || val == NULL ) return;

   pChar = strstr(info, var);
   if ( pChar == NULL ) return;

   // move pass the variable string in line
   pChar += strlen(var);

   // Remove spaces from beginning of value string
   while ( *pChar != '\0' && isspace((int)*pChar) != 0 )
      pChar++;

   // get data until end of line, or space char
   for ( i = 0;
         i < len && *pChar != '\0' &&
         isspace((int)*pChar) == 0;
         i++, pChar++ )
      val[i] = *pChar;

   val[i] = '\0';
}

//**************************************************************************
// Function Name: bcmConvertStrToShellStr
// Description  : convert the given string so that each its character is
//                surround by '. If character is ' then it is surround by ".
//                (a#b'c"d => 'a''#''b'"'"'c''"''d')
// Parameters   : str - the given string.
//                buf - the converted string.
// Returns      : none.
//**************************************************************************
void bcmConvertStrToShellStr(char *str, char *buf) {
   if ( buf == NULL ) return;

   int len = strlen(str);
   int i = 0, j = 0;

   for ( i = 0; i < len; i++ ) {
      if ( str[i] != '\'' ) {
         buf[j++] = '\'';
         buf[j++] = str[i];
         buf[j++] = '\'';
      } else {
         buf[j++] = '"';
         buf[j++] = str[i];
         buf[j++] = '"';
      }
   }

   buf[j]  = '\0';
}

//**************************************************************************
// Function Name: bcmProcessMarkStrChars
// Description  : use backslash in front one of the escape codes to process
//                marked string characters.
//                (a'b"c => a\'b\"c)
// Parameters   : str - the string that needs to process its special chars.
// Returns      : none.
//**************************************************************************
void bcmProcessMarkStrChars(char *str) {
   if ( str == NULL ) return;
   if ( str[0] == '\0' ) return;

   char buf[SYS_CMD_LEN];
   int len = strlen(str);
   int i = 0, j = 0;

   for ( i = 0; i < len; i++ ) {
      if ( bcmIsMarkStrChar(str[i]) == TRUE )
         buf[j++] = '\\';
      buf[j++] = str[i];
   }

   buf[j] = '\0';
   strcpy(str, buf);
}

//**************************************************************************
// Function Name: bcmIsMarkStrChar
// Description  : verify the given character is used to mark the begining or
//                ending of string or not.
// Parameters   : c -- the given character.
// Returns      : TRUE or FALSE.
//**************************************************************************
int bcmIsMarkStrChar(char c) {
   // need to add '\0' as termination character to speChars[]
   char specChars[] = { '\'', '"', '\\', '\0' };
   int len = strlen(specChars);
   int i = 0;
   int ret = FALSE;

   for ( i = 0; i < len; i++ )
      if ( c == specChars[i] )
         break;

   if ( i < len )
      ret = TRUE;

   return ret;
}

#if defined(SUPPORT_VDSL)
static char glbVdslSwVer[SYS_CMD_LEN];
//**************************************************************************
//**************************************************************************
// Function Name: bcmSetSwVer
// Description  : store software version to global variable
// Parameters   : swVer - the software version that needs to be set.
// Returns      : none.
//**************************************************************************
void bcmSetVdslSwVer(char *swVer) 
{
   if ( swVer == NULL ) return;

   if ( strlen(swVer) < SYS_CMD_LEN - 1 )
      strcpy(glbVdslSwVer, swVer);
   else {
      strncpy(glbVdslSwVer, swVer, SYS_CMD_LEN - 2);
      glbVdslSwVer[SYS_CMD_LEN - 1] = '\0';
   }
}
#endif

//**************************************************************************
// Function Name: bcmGetSwVer
// Description  : retrieve software version from global variable
// Parameters   : swVer - buffer to get the software version.
//                size - size of buffer.
// Returns      : none.
//**************************************************************************
void bcmGetSwVer(char *swVer, int size) {
   char version[SYS_CMD_LEN], adslPhyVersion[SYS_CMD_LEN];

   if ( swVer == NULL ) return;

   // get adsl physical version
   BcmAdslCtl_GetPhyVersion(adslPhyVersion, SYS_CMD_LEN - 1);
   // create software version
   sprintf(version, "%s%s", SOFTWARE_VERSION, adslPhyVersion);
   strncpy(swVer, version, size);
}

#if defined(SUPPORT_VDSL)
//**************************************************************************
// Function Name: bcmGetVdslSwVer
// Description  : retrieve software version from global variable
// Parameters   : swVer - buffer to get the software version.
//                size - size of buffer.
// Returns      : none.
//**************************************************************************
void bcmGetVdslSwVer(char *swVer, int size) {
   if ( swVer == NULL ) return;

   if ( strlen(glbVdslSwVer) < size - 1 )
      strcpy(swVer, glbVdslSwVer);
   else {
      strncpy(swVer, glbVdslSwVer, size - 2);
      swVer[size - 1] = '\0';
   }
}
#endif

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

int bcmWanEnetQuerySwitch(char *ifName) {
   FILE *errFs = NULL;
   char cmd[IFC_LARGE_LEN];
   char str[IFC_LARGE_LEN];
   int  numIfc = 0;

   sprintf(cmd, "vconfig query %s 2 2>/var/vcfgerr\n", ifName);
   bcmSystem(cmd);
   // Check the status of the previous command
   errFs = fopen("/var/vcfgerr", "r");
   if (errFs != NULL ) {
      fgets(str, IFC_LARGE_LEN, errFs);
      numIfc = atoi(str);
      fclose(errFs);
      bcmSystem("rm /var/vcfgerr");
   }
   return numIfc;
}

#endif // USE_ALL, code below this code can be linked with other apps


//*********** code shared by ftpd and tftpd **********************
//****************************************************************

/***************************************************************************
// Function Name: bcmIsModuleInserted.
// Description  : verify the given module name is already inserted or not.
// Parameters   : modName -- the given module name.
// Returns      : TRUE or FALSE.
****************************************************************************/
#if STATIC_KERNEL_NF_FIREWALL
int bcmIsModuleInserted(char *modName) {
   int ret = TRUE;
   if ((strcmp(modName, "ip_tables") == 0) ||
#if STATIC_KERNEL_NF_NAT
       (strcmp(modName, "iptable_nat")   == 0) ||
#endif
       (strcmp(modName, "iptable_raw") == 0)) {
      ret = TRUE;
   }

   return ret;
}
#else
int bcmIsModuleInserted(char *modName) {
   int ret = FALSE;
   char buf[SYS_CMD_LEN];
   FILE* fs = fopen("/proc/modules", "r");

   if ( fs != NULL ) {
      while ( fgets(buf, SYS_CMD_LEN, fs) > 0 )
         if ( strstr(buf, modName) != NULL ) {
            ret = TRUE;
            break;
         }
      fclose(fs);
   }

   return ret;
}
#endif


/***************************************************************************
// Function Name: bcmCheckInterfaceUp().
// Description  : check status of interface.
// Parameters   : devname - name of device.
// Returns      : 1 - UP.
//                0 - DOWN.
****************************************************************************/
int bcmCheckInterfaceUp(char *devname) {
   int  skfd;
   int  ret;
   struct ifreq intf;

   if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      return 0;
   }

   strcpy(intf.ifr_name, devname);

   // if interface is br0:0 and
   // there is no binding IP address then return down
   if ( strchr(devname, ':') != NULL ) {
      if (ioctl(skfd, SIOCGIFADDR, &intf) < 0) {
         close(skfd);
         return 0;
      }
   }

   // if interface flag is down then return down
   if (ioctl(skfd, SIOCGIFFLAGS, &intf) == -1) {
      ret = 0;
   } else {
      if ( (intf.ifr_flags & IFF_UP) != 0)
         ret = 1;
      else
         ret = 0;
   }

   close(skfd);

   return ret;
}

//If we don't link with busybox, we need this function
#ifndef BUILD_STATIC

static void remove_delimitor( char *s)
{
    char *p1, *p2;

    p1 = p2 = s;
    while ( *p1 != '\0' || *(p1+1) != '\0') {
        if (*p1 != '\0') {
           *p2 = *p1;
           p2++;
         }
         p1++;
    }
    *p2='\0';

}
/* find_pid_by_name()
 *
 *  This finds the pid of the specified process.
 *  Currently, it's implemented by rummaging through
 *  the proc filesystem.
 *
 *  Returns a list of all matching PIDs
 */
static pid_t* find_pid_by_name( char* pidName)
{
        DIR *dir;
        struct dirent *next;
        pid_t* pidList=NULL;
        int i=0;

        /*FILE *status */
        FILE *cmdline;
        char filename[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        /* char name[READ_BUF_SIZE]; */
                
        dir = opendir("/proc");
        if (!dir) {
                printf("cfm:Cannot open /proc");
                return NULL;
        }

        while ((next = readdir(dir)) != NULL) {
                /* re-initialize buffers */
                memset(filename, 0, sizeof(filename));
                memset(buffer, 0, sizeof(buffer));  

                /* Must skip ".." since that is outside /proc */
                if (strcmp(next->d_name, "..") == 0)
                        continue;

                /* If it isn't a number, we don't want it */
                if (!isdigit(*next->d_name))
                        continue;

                /* sprintf(filename, "/proc/%s/status", next->d_name); */
                /* read /porc/<pid>/cmdline instead to get full cmd line */
                sprintf(filename, "/proc/%s/cmdline", next->d_name);
                if (! (cmdline = fopen(filename, "r")) ) {
                        continue;
                }
                if (fgets(buffer, READ_BUF_SIZE-1, cmdline) == NULL) {
                        fclose(cmdline);
                        continue;
                }
                fclose(cmdline);

                /* Buffer should contain a string like "Name:   binary_name" */
                /*sscanf(buffer, "%*s %s", name);*/
                /* buffer contains full commandline params separted by '\0' */
                remove_delimitor(buffer);
                if (strstr(buffer, pidName) != NULL) {
                        pidList=realloc( pidList, sizeof(pid_t) * (i+2));
                        if (!pidList) {
                                printf("cfm: Out of memeory!\n");
				closedir(dir);
                                return NULL;
                        }
                        pidList[i++]=strtol(next->d_name, NULL, 0);
                }
        }
        closedir(dir);

        if (pidList)
                pidList[i]=0;
        else if ( strcmp(pidName, "init")==0) {
                /* If we found nothing and they were trying to kill "init",
                 * guess PID 1 and call it good...  Perhaps we should simply
                 * exit if /proc isn't mounted, but this will do for now. */
                pidList=realloc( pidList, sizeof(pid_t));
                if (!pidList) {
                        printf("cfm: Out of memeory!\n");
                        return NULL;
                }
                pidList[0]=1;
        } else {
                pidList=realloc( pidList, sizeof(pid_t));
                if (!pidList) {
                        printf("cfm: Out of memeory!\n");
                        return NULL;
                }
                pidList[0]=-1;
        }
        return pidList;
}

#endif

void bcmHidePassword(char *command) {
   char *ptr = NULL;
   char * begin, *end;
   int len = 0;

   /* pppd -i .....  -p password */
   if ((ptr = strstr(command,"pppd")) != NULL) {
     if (!strstr(ptr, "-p")) 
        return;
     begin = strstr(ptr,"-p") + 3;
     end = strchr(begin,' ');
     if (end == NULL) 
       len = strlen(begin);
     else 
       len = end - begin;
   }

   while (len > 0) {
      *begin = '*';
      begin++; len--;
   }
}

/***************************************************************************
// Function Name: bcmSystem().
// Description  : launch shell command in the child process.
// Parameters   : command - shell command to launch.
// Returns      : status 0 - OK, -1 - ERROR.
****************************************************************************/
int bcmSystemEx (char *command, int printFlag) {
   int pid = 0, status = 0;
   char *newCommand = NULL;

   if ( command == 0 )
      return 1;

   pid = fork();
   if ( pid == -1 )
      return -1;

   if ( pid == 0 ) {
      char *argv[4];
      argv[0] = "sh";
      argv[1] = "-c";
      argv[2] = command;
      argv[3] = 0;
#ifdef BRCM_DEBUG
      if (printFlag)
         printf("app: %s\r\n", command);
#endif
      if (printFlag) {
        if ((newCommand = strdup(command)) != NULL) {
           bcmHidePassword(newCommand);
           syslog(LOG_DEBUG, newCommand);
           free(newCommand);
        }
      }
      execve("/bin/sh", argv, environ);
      exit(127);
   }

   /* wait for child process return */
   do {
      if ( waitpid(pid, &status, 0) == -1 ) {
         if ( errno != EINTR )
            return -1;
      } else
         return status;
   } while ( 1 );

   return status;
}


/***************************************************************************
// Function Name: bcmGetPid().
// Description  : get process PID by using process name.
// Parameters   : command - command that launch the process.
// Returns      : process ID number.
****************************************************************************/
int bcmGetPid(char * command)
{
    char cmdline[128], *p1, *p2;
    pid_t *pid = NULL;
    int ret = 0;

    p1 = command;
    p2 = cmdline;
    while ( *p1 != '\0') {
        if (*p1 != ' ') {
           *p2 = *p1;
           p2++;
         }
         p1++;
    }
    *p2='\0';

    pid = find_pid_by_name(cmdline);
    if ( pid != NULL ) {
       ret = (int)(*pid);
       free(pid);
    }

    return ret;
}


/***************************************************************************
// Function Name: bcmGetPidList().
// Description  : get process PID by using process name.
// Parameters   : command - command that launch the process.
// Returns      : process ID list
****************************************************************************/
int *bcmGetPidList(char * command)
{
    char cmdline[128], *p1, *p2;
    pid_t *pid = NULL;

    p1 = command;
    p2 = cmdline;
    while ( *p1 != '\0') {
        if (*p1 != ' ') {
           *p2 = *p1;
           p2++;
         }
         p1++;
    }
    *p2='\0';

    pid = find_pid_by_name(cmdline);

    return (int*)pid;
}

/***************************************************************************
// Function Name: bcmGetIntfNameSocket.
// Description  : Return the interface name a socket is bound to
// Parameters   : socketfd: Socket descriptor, intfname: Network interface name
// Returns      : Failed: -1. Succeeded: 0
****************************************************************************/
int bcmGetIntfNameSocket(int socketfd, char *intfname)
{
   int i = 0, fd = 0;
   int numifs = 0, bufsize = 0;
   struct ifreq *all_ifr = NULL;
   struct ifconf ifc;
   struct sockaddr local_addr;
   socklen_t local_len = sizeof(struct sockaddr_in);

   memset(&ifc, 0, sizeof(struct ifconf));
   memset(&local_addr, 0, sizeof(struct sockaddr));

   if (getsockname(socketfd, &local_addr,&local_len) < 0) {
     printf("bcmGetIntfNameSocket: Error in getsockname!\n");
     return -1;
   }

   //printf("bcmGetIntfNameSocket: Session comes from: %s\n",inet_ntoa(((struct sockaddr_in *)&local_addr)->sin_addr));
   
   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
     printf("bcmGetIntfNameSocket: Error openning socket when getting socket intface info\n");
     return -1;
   }

   numifs = 16;

   bufsize = numifs*sizeof(struct ifreq);
   all_ifr = (struct ifreq *)malloc(bufsize);
   if (all_ifr == NULL) {
      printf("bcmGetIntfNameSocket: out of memory!\n");
      close(fd);
      return -1;
   }

   ifc.ifc_len = bufsize;
   ifc.ifc_buf = (char *)all_ifr;
   if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
      printf("bcmGetIntfNameSocket: Error getting interfaces\n");
      close(fd);
      free(all_ifr);
      return -1;
   }

   numifs = ifc.ifc_len/sizeof(struct ifreq);
   //printf("bcmGetIntfNameSocket: numifs=%d\n",numifs);
   for (i = 0; i < numifs; i ++) {
	   //printf("bcmGetIntfNameSocket: intface name=%s\n",all_ifr[i].ifr_name); 
	   struct in_addr addr1,addr2;
	   addr1 = ((struct sockaddr_in *)&(local_addr))->sin_addr;
	   addr2 = ((struct sockaddr_in *)&(all_ifr[i].ifr_addr))->sin_addr;
	   if (addr1.s_addr == addr2.s_addr) {
		strcpy(intfname, all_ifr[i].ifr_name);
	   	break;
	   }
   }

   close(fd);
   free(all_ifr);
   return 0;
}


int getLanInfo(char *lan_ifname, struct in_addr *lan_ip, struct in_addr *lan_subnetmask)
{
   int socketfd;
   struct ifreq lan;

   if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
     printf("app: Error openning socket when getting LAN info\n");
     return -1;
   }

   strcpy(lan.ifr_name,lan_ifname);
   if (ioctl(socketfd,SIOCGIFADDR,&lan) < 0) {
     printf("app: Error getting LAN IP address\n");
     close(socketfd);
     return -1;
   }
   *lan_ip = ((struct sockaddr_in *)&(lan.ifr_addr))->sin_addr;

   if (ioctl(socketfd,SIOCGIFNETMASK,&lan) < 0) {
     printf("app: Error getting LAN subnet address\n");
     close(socketfd);
     return -1;
   }

   *lan_subnetmask = ((struct sockaddr_in *)&(lan.ifr_netmask))->sin_addr;

   close(socketfd);
   return 0;
}

static int isIpExtension(void)
{
   FILE *fp;
   int ipextension = 0;

   if ((fp=fopen("/var/ipextension","r")) != NULL) {
      fscanf(fp,"%d",&ipextension);
      fclose(fp);
   }

   return ipextension;
}

static void getIpExtIp(char *buf)
{
   FILE* fs;
   char wan[64], gateway[64], dns[64], str[256];

   if ( buf == NULL ) return;

   buf[0] = '\0';
   fs = fopen("/var/ipextinfo", "r");
   if ( fs != NULL ) {
      fgets(str, 256, fs);
      fclose(fs);
      sscanf(str, "%s %s %s\n", wan, gateway, dns);
      strcpy(buf, wan);
   }
}

int isAccessFromLan(struct in_addr clntAddr)
{
   int ret = 0;
   struct in_addr inAddr, inMask;
   char wan[64];

   getLanInfo("br0", &inAddr, &inMask);
   /* check ip address of support user to see it is in LAN or not */
   if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
      ret = 1;
   else {
      /* check ip address of support user to see if it is from secondary LAN */
      if (bcmCheckInterfaceUp("br0:0")) {
         getLanInfo("br0:0", &inAddr, &inMask);
         if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
            ret = 1;
      }

#ifdef SUPPORT_SUPER_DMZ // Keven -- super DMZ
      /* Keven -- check ip address of support user to see if it is from non Super DMZ */
      if (bcmCheckInterfaceUp("br1")) {
         getLanInfo("br1", &inAddr, &inMask);
         if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
            ret = 1;
      }
#endif

#ifdef MULTISUBNET

      int nNumOfValid = 0;
      char inf[8];
      int i = 1;

      nNumOfValid = BcmDb_getNumOfValidBridge();
	 

      for( i = 1; i < nNumOfValid; i++)
     {

	  sprintf(inf, "br%d", i);
       
         if (bcmCheckInterfaceUp(inf)) 
	  {
             getLanInfo(inf, &inAddr, &inMask);
             if ( (clntAddr.s_addr & inMask.s_addr) == (inAddr.s_addr & inMask.s_addr) )
             {
                 ret = 1;
		   break;
             }
          }
         
           
     }


#endif

      /* Last option it must be from WAN side */
      if (isIpExtension()) {
         getIpExtIp(wan);
      if ( clntAddr.s_addr == inet_addr(wan) )
         ret = 1;
      }
   }

   return ret;
}

// return 0, ok. return -1 = wrong chip
// used by upload.c and ftpd, tftpd, tftp utilities.
int checkChipId(char *strTagChipId, char *sig2)
{
    int tagChipId = 0;
    unsigned int chipId = (int) sysGetChipId();
    int result;

    tagChipId = strtol(strTagChipId, NULL, 16);

    if (tagChipId == chipId)
        result = 0;
    else {
        printf("Chip Id error.  Image Chip Id = %04x, Board Chip Id = %04x.\n", tagChipId, chipId);
        result = -1;
    }

    return result;
}

/***************************************************************************
// Function Name: bcmCheckForRedirect(void)
// Description  : check for nat redirect for .
// Parameters   : none
// Returns      : 0 --> tcp port 21, 22, 23, 80 is redirected. -1 --> not redirected
****************************************************************************/
int bcmCheckForRedirect(void)
{
   char col[11][32];
   char line[512];
   FILE* fs;  
   int count = 0;

   if (bcmIsModuleInserted("iptable_nat") == FALSE)
      return FALSE;

   bcmSystem("iptables -t nat -L > /var/nat_redirect");

   fs = fopen("/var/nat_redirect", "r");
   if ( fs != NULL ) {
      while ( fgets(line, sizeof(line), fs) ) {
         // read pass 3 header lines
         if ( count++ < 3 )
            continue;
         sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s",
               col[0], col[1], col[2], col[3], col[4], col[5],
               col[6], col[7], col[8], col[9], col[10]);
        if ((strcmp(col[0], "REDIRECT") == 0) && (strcmp(col[1], "tcp") == 0) && (strcmp(col[8], "ports") == 0))
          if (strcmp(col[9], "80") == 0 || strcmp(col[9], "23") == 0 || strcmp(col[9], "21") == 0 ||
              strcmp(col[9], "22") == 0 || strcmp(col[9], "69") == 0) {
              return TRUE;
          }
      }
      fclose(fs);
   }
   unlink("/var/nat_redirect");
   return FALSE;
}

/***************************************************************************
// Function Name: bcmRemoveModules(int lanIf)
// Description  : remove not used modules to free memory.
// Parameters   : none
// Returns      : none.
****************************************************************************/
void bcmRemoveModules(int lanIf)
{
   char *modList[]=
   {
      "bcm_enet",
      "bcm_usb",
      "ipt_state",
      "ipt_mark",
      "ipt_limit",
      "ipt_TCPMSS",
      "ipt_REDIRECT",
      "ipt_MASQUERADE",
      "ipt_MARK",
      "ipt_LOG",
      "ipt_FTOS",
      "ipt_connlimit",
      "ip_nat_tftp",
      "ip_nat_irc",
      "ip_nat_ftp",
      "ip_nat_h323",
      "ip_nat_pptp",
      "ip_nat_gre",
      "ip_nat_rtsp",
      "ip_nat_ipsec",
      "ip_nat_pt",
      "ip_conntrack_tftp",
      "ip_conntrack_irc",
      "ip_conntrack_ftp",
      "ip_conntrack_h323",
      "ip_conntrack_pptp",
      "ip_conntrack_gre",
      "ip_conntrack_rtsp",
      "ip_conntrack_ipsec",
      "ip_conntrack_pt",
      "iptable_mangle",
#if defined(SUPPORT_DYNAHELPER)
      "iptable_raw",
#endif
      "iptable_nat",
      "ip_conntrack",
      "iptable_filter",
      "ip_tables",
#if defined(SUPPORT_DYNAHELPER)
      "ip_queue",
#endif
      NULL,
   };

   char cmd[SYS_CMD_LEN];
   int i = 0;
   int saveNat = FALSE;

   if (lanIf == 0)         // if lan, do not kill bcm_usb and bcm_enet
      i = 2;
   else // if in ipow mode, leave bcm_enet out   
   {
#if defined(BCM_ATM_BONDING_ETH)
      /* Skip the ethernet if we are running atm bonding, as it uses
       * Ethernet interface internally for communications.
       */
      i = 1;
#else
      FILE *fs = fopen("/proc/var/fyi/wan/eth0/pid", "r");
      if (fs != NULL) 
      {
         i = 1;        
         fclose(fs);
      }
#endif
   }

   saveNat = bcmCheckForRedirect();
   bcmSystem("iptables -F");   
   if (bcmIsModuleInserted("iptable_filter") == TRUE)
   {  
       strncpy(cmd, "iptables -t filter -F", SYS_CMD_LEN-1);
       bcmSystem(cmd);
       bcmSystem("iptables -X urlfilter");
   }
   if (bcmIsModuleInserted("iptable_nat") == TRUE)
   {  
       strncpy(cmd, "iptables -t nat -F", SYS_CMD_LEN-1);
       bcmSystem(cmd);
   }
   if (bcmIsModuleInserted("iptable_mangle") == TRUE)
   {  
       strncpy(cmd, "iptables -t mangle -F", SYS_CMD_LEN-1);
       bcmSystem(cmd);
   }
#if defined(SUPPORT_DYNAHELPER)
   if (bcmIsModuleInserted("iptable_raw") == TRUE)
   {
       strncpy(cmd, "iptables -t raw -F", SYS_CMD_LEN-1);
       bcmSystem(cmd);
   }
#endif

   while (modList[i] != NULL)
   {
      if (bcmIsModuleInserted(modList[i]) == TRUE) 
      {
         if (!(saveNat && strcmp(modList[i], "iptable_nat") == 0))
         {
            sprintf(cmd, "rmmod %s", modList[i]);
            bcmSystem(cmd);
         }
      }
      i++;
   }
   printf("\nRemaining modules:\n");
   bcmSystemMute("cat /proc/modules");
   printf("\nMemory info:\n");
   bcmSystemMute("sysinfo");
   sleep(1);
}

/***************************************************************************
// Function Name: bcmGetIfcIndexByName().
// Description  : get interface index by its name.
// Parameters   : ifcIdx -- interface index.
//                ifcName -- interface name.
// Returns      : interface index
****************************************************************************/
int bcmGetIntf(char *ifcName) {
   struct ifreq ifr;
   int s = 0;

   if ( ifcName == NULL ) return -1;

   if ( (s = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) return -1;
   strcpy(ifr.ifr_name, ifcName);
   if ( ioctl(s, SIOCGIFINDEX, &ifr) < 0 ) {
      close(s);
      return 0;
   }
   close(s);
   return ifr.ifr_ifindex;
}

int bcmWaitIntfExists(char *ifName) {
   int retry = 0;
   int ret;

   while (retry < 100) {
      if ((ret =bcmGetIntf(ifName)) <= 0) {
         usleep(5000);
         //printf("not exist,retry %d, ret %d\n",retry,ret);
         retry++;
      }
      else {
         return 1;
      }
   } /* while */
   return 0;
}

//**************************************************************************
// Function Name: bcmMacStrToNum
// Description  : convert MAC address from string to array of 6 bytes.
//                Ex: 0a:0b:0c:0d:0e:0f -> 0a0b0c0d0e0f
// Returns      : status.
//**************************************************************************
int bcmMacStrToNum(char *macAddr, char *str) {
   char *pToken = NULL, *pLast = NULL;
   char *buf;
   UINT16 i = 1;
   int len;
   
   if ( macAddr == NULL ) return FALSE;
   if ( str == NULL ) return FALSE;

   len = strlen(str) + 1;
   if (len > 20)
     len = 20;
   buf = (char*)malloc(len);
   memset(buf,0,len);

   if ( buf == NULL ) return FALSE;

   /* need to copy since strtok_r updates string */
   strncpy(buf, str,len-1);

   /* Mac address has the following format
       xx:xx:xx:xx:xx:xx where x is hex number */
   pToken = strtok_r(buf, ":", &pLast);
   if (pToken != NULL) {
      macAddr[0] = (char) strtol(pToken, (char **)NULL, 16);
      for ( i = 1; i < 6; i++ ) {
         pToken = strtok_r(NULL, ":", &pLast);
         if (pToken == NULL)
            break;
         macAddr[i] = (char) strtol(pToken, (char **)NULL, 16);
      }
   }

   free(buf);

   return TRUE;
}

//**************************************************************************
// Function Name: bcmMacNumToStr
// Description  : convert MAC address from array of 6 bytes to string.
//                Ex: 0a0b0c0d0e0f -> 0a:0b:0c:0d:0e:0f
// Returns      : status.
//**************************************************************************
int bcmMacNumToStr(char *macAddr, char *str) {
   if ( macAddr == NULL ) return FALSE;
   if ( str == NULL ) return FALSE;

   sprintf(str, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
           (UINT8) macAddr[0], (UINT8) macAddr[1], (UINT8) macAddr[2],
           (UINT8) macAddr[3], (UINT8) macAddr[4], (UINT8) macAddr[5]);

   return TRUE;
}

void writeIp_b(int ip, char *buf)
{
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
}

const char *itoa(int i)
{
	static char buf[256];
	sprintf(buf, "%d", i);
	return buf;
}

int readIp(const char* ip)
{
	int n = 0;
	int res = 0;

	while (n < 4 && *ip) {
		if (isdigit(*ip)) {
			res = (res << 8) | atoi(ip);
			n++;
			while (isdigit(*ip)) {
				ip++;
			}
		} else {
			ip++;
		}
	}
	return res;
}

char* writeIp(int ip)
{
	static char buf[24];
	writeIp_b(ip, buf);
	return buf;
}

char* writeMac(const unsigned char* mac)
{
	static char buf[24];

	sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2] ,mac[3], mac[4], mac[5]);
	return buf;
}

#ifdef USE_ALL

#ifdef PORT_MIRRORING
/***************************************************************************
 * Function Name: OpenBlaaDD
 * Description  : Opens the bcmatm device.
 * Returns      : device handle if successsful or -1 if error
 ***************************************************************************/
static int OpenBlaaDD( void )
{
    int nFd ;
   	
	 if( (nFd = open("/dev/bcmatm0", O_RDWR)) < 0 )
		 printf( "OpenBlaaDD : open error %d\n", errno );

    return( nFd );
} /* OpenBcmAtm */
#endif
#endif

/***************************************************************************
// Function Name: bcmConfigPortMirroring.
// Description  : Configure the Port Mirroring feature dynamically.
// Parameters   : MirrorCfg structure pointer.
// Returns      : Failed: -1. Succeeded: 0
****************************************************************************/
int bcmConfigPortMirroring (void *pCfg)
{
#ifdef USE_ALL
	 MirrorCfg *pMirrorCfg = (MirrorCfg *) pCfg ;
	 int fd ;

#ifdef PORTMIRROR_DEBUG
	 printf ("ENGDBG:- In BcmConfigPortMirroring \n") ;
#endif

	 if ((fd = OpenBlaaDD ()) < 0) {
			printf ("Config Port Mirroring Failed \n") ;
			return -1 ;
	 }

   if (ioctl(fd, ATMIOCTL_PORT_MIRRORING, pMirrorCfg) < 0) {

      printf("IOCTL to BLAA Drive for Port Mirror CFG failed . Fatal \n") ;
      close(fd) ;
      return -1 ;
   }

   close(fd);
#endif
   return 0;
}

#ifdef USE_ALL
/***************************************************************************
// Function Name: bcmRemoveTrafficControlRules.
// Description  : remove tc rules for this interface if QoS is enabled.
// Returns      : none.
****************************************************************************/
void bcmRemoveTrafficControlRules(UINT32 portId, UINT16 vpi, UINT16 vci, UINT16 conId, UINT8 protocol) {
    char ifc[16];
    char cmd[SYS_CMD_LEN];

    if (protocol != PROTO_PPPOA) {
        return;
    }

    ifc[0] = '\0';
    snprintf(ifc, 16, "ppp_%d_%d_%d_%d", (UINT16)portId, vpi, vci, conId);
    sprintf(cmd, "tc qdisc del dev %s root", ifc);
    bcmSystem(cmd);
}

/***************************************************************************
// Function Name: bcmAddTrafficControlRules.
// Description  : add tc rules for this interface if QoS is enabled.
// Returns      : none.
****************************************************************************/
void bcmAddTrafficControlRules(UINT32 portId, UINT16 vpi, UINT16 vci, UINT16 conId, UINT8 protocol) {
    char ifc[16];
    char cmd[SYS_CMD_LEN];
    ADSL_CONNECTION_INFO adslInfo;
    int lineRate = 0;

    if (protocol != PROTO_PPPOA) {
        return;
    }

    ifc[0] = '\0';
    sprintf(ifc, "ppp_%d_%d_%d_%d", (UINT16)portId, vpi, vci, conId);
    // Get the actual upstream line rate from the ADSL driver.
    BcmAdslCtl_GetConnectionInfo(&adslInfo);
    if (adslInfo.LinkState != BCM_ADSL_LINK_UP) {
        return;
    }
    if ( adslInfo.ulInterleavedUpStreamRate != 0 )
         lineRate = adslInfo.ulInterleavedUpStreamRate;
    else
         lineRate = adslInfo.ulFastUpStreamRate;

    // Before we do anythng, lets add an ebtable rule at the bottom, that
    // marks the packets with low priority mark as default 0x0001.
    sprintf(cmd, "ebtables -t broute -A BROUTING -j mark --set-mark 0x0001 -p IPv4");
    bcmSystemNoHang(cmd);

    // Create the root. This also creates the classes 1:1, 1:2 and 1:3
    // automatically
    sprintf(cmd, "tc qdisc add dev %s root handle 1: htb default 1", ifc);
    bcmSystemNoHang(cmd);
    sprintf(cmd, "tc class add dev %s parent 1: classid 1:1 htb rate %lukbit",
            ifc, adslInfo.ulInterleavedUpStreamRate);
    bcmSystemNoHang(cmd);
    sprintf(cmd, "tc qdisc add dev %s parent 1:1 handle 10: prio bands 3 "
            "priomap 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", ifc);
    bcmSystemNoHang(cmd);
    // Create the htb's under each class.
    sprintf(cmd, "tc qdisc add dev %s parent 10:1 handle 100: pfifo limit 10", ifc);
    bcmSystemNoHang(cmd);
    sprintf(cmd, "tc qdisc add dev %s parent 10:2 handle 200: pfifo limit 10", ifc);
    bcmSystemNoHang(cmd);
    sprintf(cmd, "tc qdisc add dev %s parent 10:3 handle 300: pfifo limit 10", ifc);
    bcmSystemNoHang(cmd);
    // Now add the filters for each sfq using the default handles.
    sprintf(cmd, "tc filter add dev %s protocol ip parent 10:0 prio 1 handle %d fw classid 10:1",
            ifc, PRIORITY_HIGH);
    bcmSystemNoHang(cmd);
    sprintf(cmd, "tc filter add dev %s protocol ip parent 10:0 prio 1 handle %d fw classid 10:2",
            ifc, PRIORITY_MEDIUM);
    bcmSystemNoHang(cmd);
    sprintf(cmd, "tc filter add dev %s protocol ip parent 10:0 prio 1 handle %d fw classid 10:3",
            ifc, PRIORITY_LOW);
    bcmSystemNoHang(cmd);
}

/* this function stores device into procfs file for other processes to access */
void bcmStoreDeviceInfoToProcFile(void)
{
   FILE* fs;
   void *info=NULL;
   PBcmCfm_DevInfoCfg_t pDevInfo = NULL;

   bcmSystemMute("mkdir -p /var/fyi/sys");
   bcmSystemMute("echo > /var/fyi/sys/info");
   fs = fopen("/var/fyi/sys/info", "w+");

   /* OUI %s */
   /* SerialNumber %s */
   /* ProductClass %s */
   if ( fs != NULL ) {
     if (BcmCfm_objGet(BCMCFM_OBJ_SYS_DEVICE_INFO, &info, 0) == BcmCfm_Ok) {
	pDevInfo = (PBcmCfm_DevInfoCfg_t)info;
	if (pDevInfo != NULL) {
	  fprintf(fs,"OUI %s\n",pDevInfo->manufacturerOui);
	  fprintf(fs,"SerialNumber %s\n",pDevInfo->serialNumber);
	  fprintf(fs,"ProductClass %s\n",pDevInfo->productClass);
	  BcmCfm_objFree(BCMCFM_OBJ_SYS_DEVICE_INFO, info);
	}
     }
     fclose(fs);
   } /* fs != NULL */
}

#endif

/***************************************************************************
// Function Name: resolve_ip_by_name
// Description  : resolve the ip address through URL for type A DNS queries
// Parameters   : hostname - input the hostname string
//                addr - output ip address. user has to give this pointer 
// Returns      : 0 on success and -1 on failure.
****************************************************************************/
int resolve_ip_by_name(char *hostname, struct in_addr *addr)
{
   char answer[256];
   char *ptr = answer;
   int i, j;
   time_t t;
   static dns_cache_t dns_cache[DNS_CACHE_SIZE];
   static int cache_initialized = 0;
   unsigned int check = 0;

   if ((hostname == NULL) || (addr == NULL)) {
      perror("resolve_ip_by_name: NULL hostname or NULL addr requested");
      return -1;
   }

   /* first time: initialize DNS cache */
   if (cache_initialized == 0) {
      memset(dns_cache, 0, sizeof(dns_cache));
      cache_initialized=1;
   }

   time(&t);
   /* clean expired entries */
   for (i=0; i<DNS_CACHE_SIZE; i++) {
      if (dns_cache[i].hostname[0]=='\0') continue;
      if (dns_cache[i].timestamp < t) {
         memset (&dns_cache[i], 0, sizeof(dns_cache[0]));
      }
   }

   /* search requested entry in cache */
   for (i=0; i<DNS_CACHE_SIZE; i++) {
      if (dns_cache[i].hostname[0]=='\0') continue; /* empty */
      if (strcmp(hostname, dns_cache[i].hostname) == 0) { /* match */
         memcpy(addr, &dns_cache[i].addr, sizeof(struct in_addr));
         return 1; 
      }
   }

   /* Initialize the Name Server info */
   if(-1 == res_init()) {
      perror("can't init");
      return -1;
   }

   /* 
    * query the name server for specific type. Currently only type A  
    * is supported                                                    
    */

   if(-1 == res_query(hostname, ns_c_in , ns_t_a, answer, sizeof(answer))) {
      perror("can't query");
      return -1;
   }

   /* Increment DNS header size  */
   ptr += NS_HFIXEDSZ;

  /* 
   * Increment string query string length and extra 2 bytes for null termination
   * and first string length 
   */
   ptr += (strlen(hostname) + 2);

   /* Increment query size  */
   ptr += NS_QFIXEDSZ;

   /* Increment fixed qry data size  */
   ptr += 6;

   /* find an empty slot in the cache */
   check = abs(dns_cache[0].timestamp - t);
   j=0;
   for (i=0; i<DNS_CACHE_SIZE; i++) {
      if (dns_cache[i].hostname[0]=='\0') break;
      if (check >= (dns_cache[i].timestamp -t)) {
         /* remember oldest entry */
         check = abs(dns_cache[i].timestamp - t);
        j=i;
      }
   }
   /* if no empty slot found, take oldest one */
   if (i >= DNS_CACHE_SIZE) i=j;


   /* store the result in the cache */
   memset(&dns_cache[i], 0, sizeof(dns_cache[0]));
   strncpy(dns_cache[i].hostname, hostname, HOSTNAME_SIZE);
   time(&dns_cache[i].timestamp);
   dns_cache[i].timestamp += (htonl(*(unsigned int *)ptr));

   /* offset to ip addr */
   ptr += (NS_INT32SZ + NS_INT16SZ);
   addr->s_addr = htonl(*(unsigned int *)ptr);
   memcpy(&dns_cache[i].addr, addr, sizeof(struct in_addr));
  
   return 0;
}

#ifdef SUPPORT_SUPER_DMZ
void bcmGetBridgeInterfaceInfo(char *varValue, char *skip) {
   int index=0;

   varValue[0] = '\0';

   while ( index < PORT_MAP_MAX_ENTRY) {
      char *pPfx = NULL;
      char *pSfx = NULL;
      char *pIfSfx = NULL;
      char groupName[IFC_TINY_LEN], name[SYS_CMD_LEN], ifName[IFC_SMALL_LEN];;
      if (BcmDb_getBridgeInfoByIndex(index, groupName, name, NULL) == NULL) {
         index++;
         continue;
      }
      if (strcmp(groupName, "Default")) {
         index++;
         continue;
      }
      pPfx = name;
      while (pPfx <= (name + strlen(name))) {
         if ((pSfx = strstr(pPfx, "|")) != NULL) {
            if ((pIfSfx = strstr(pPfx, ":")) != NULL) {
               memset(ifName, '\0', IFC_SMALL_LEN);
               memcpy(ifName, pPfx, (pIfSfx - pPfx));
               if ( ifName[0] != '\0' && strcmp(ifName, skip)) {
                  strcat(varValue, ifName);
                  strcat(varValue, "|");
               }
            }
            pPfx = pSfx + 1;
         } else {
            if ((pIfSfx = strstr(pPfx, ":")) != NULL) {
               memset(ifName, '\0', IFC_SMALL_LEN);
               strncpy(ifName, pPfx, (pIfSfx - pPfx));
               if ( ifName[0] != '\0' && strcmp(ifName, skip)) {
                  strcat(varValue, ifName);
                  strcat(varValue, "|");
               }
            }
            break;
         }
      }
      break;
   }
}

int bcmPortMapQrySwitch(char *ifName) {
   FILE *errFs = NULL;
   char cmd[IFC_LARGE_LEN];
   char str[IFC_LARGE_LEN];
   int  numIfc = 0;

   sprintf(cmd, "ethctl %s vport query 2>/var/vcfgerr\n", ifName);
   bcmSystem(cmd);
   // Check the status of the previous command
   errFs = fopen("/var/vcfgerr", "r");
   if (errFs != NULL ) {
      fgets(str, IFC_LARGE_LEN, errFs);
      numIfc = atoi(str);
      fclose(errFs);
      bcmSystem("rm /var/vcfgerr");
   }
   return numIfc;
}

void bcmPortMapIfcStsEnable(char *ifName) {
   int vid = 0;
   char ifc[IFC_TINY_LEN];
   char cmd[IFC_LARGE_LEN];
   int  numIfc = 0;
   char str[IFC_LARGE_LEN];
   FILE *errFs = NULL;

   // Query the interface to get the number of switch ports supported.
   numIfc = bcmPortMapQrySwitch(ifName);

   if (numIfc <= 1) {
      return;
   }

   // Check if VLAN can be enabled on the switch using this interface.
   sprintf(cmd, "ethctl %s vport enable 2>/var/vcfgerr", ifName);
   bcmSystem(cmd);

   // Check the status of the previous command
   str[0] = '\0';
   errFs = fopen("/var/vcfgerr", "r");
   if (errFs != NULL ) {
      fgets(str, IFC_LARGE_LEN, errFs);
      fclose(errFs);
      bcmSystem("rm /var/vcfgerr");
   }

   // Disable or enable the configuration.

   BcmDb_setPMapIfcCfgSts_noSave(ifName, PMAP_IFC_STS_ENABLE);
   if (numIfc > 0 && numIfc <= 4) {
      // Delete this interface from the temp store also.
      BcmDb_delIfFromBridge(ifName);
      // Now add the virtual interfaces starting with VID = 2
      for (vid = 0; vid < numIfc; vid++) {
         sprintf(cmd, "vconfig add %s %d", ifName, (vid + START_PMAP_ID));
         bcmSystem(cmd);
         // Save this in our local database so that we can apply this
         // configuration when we are done adding the virtual interfaces.
         sprintf(ifc, "%s.%d", ifName, (vid + START_PMAP_ID));
         BcmDb_addBridgeIf("Default", ifc);
         // Add the newly minted virtual interface to the default bridge br0.
         sprintf(cmd, "brctl addif %s %s", IFC_BRIDGE_NAME, ifc);
         bcmSystem(cmd);
         // IFC_BRIDGE_NAME MAC could have changed, propagate the change to all
         // its attached devices by sending an arp.
         sprintf(cmd, "sendarp -s %s -d %s", IFC_BRIDGE_NAME, IFC_BRIDGE_NAME);
         bcmSystem(cmd);
         // Set this new virtual interface UP and RUNNING
         sprintf(cmd, "ifconfig %s up", ifc);
         bcmSystem(cmd);
      }
   }
}

PORT_MAP_STATUS bcmPortmapAdd(char *grpName, char *grpedIntf)
{
   PORT_MAP_STATUS sts = PORT_MAP_STS_OK;
   void *node = NULL;
   char brIfName[IFC_SMALL_LEN];
   char cfgGrpedIntf[IFC_GIANT_LEN];
   char *pIfName = NULL;
   char *pIfNameEnd = NULL;
   char ifName[IFC_TINY_LEN];
   char cmd[IFC_LARGE_LEN];
   char str[IFC_LARGE_LEN];
   FILE *errFs = NULL;

   cmd[0]          = '\0';
   str[0]          = '\0';

   // if route cfg mngr is not initialized yet then return NULL.
   // it can happen when system is changed from other mode to
   // bridge mode only but user does not allow system to update the
   // tree menu yet so that the tree menu still has the old link
   // to configure static route, rip, or default gateway
   if ( BcmPMcm_isInitialized() == FALSE ) {
      node = NULL;
   } else {
      node = BcmPMcm_getPMapCfg(NULL, brIfName, NULL, cfgGrpedIntf, NULL);
   }

   // Check if this group is already in the configuration. Initially when there is
   // no configuration just accept any name.
   while ( node != NULL ) {
      if (strcmp(brIfName, grpName) == 0)
         return sts = PORT_MAP_STS_ERR_GENERAL;
      node = BcmPMcm_getPMapCfg(node, brIfName, NULL, cfgGrpedIntf, NULL);
   }

   // Add the group only if all the interfaces are enabled
   pIfName = grpedIntf;
   while (pIfName <= (grpedIntf + strlen(grpedIntf))) {
      if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) {
         memset(ifName, '\0', IFC_TINY_LEN);
         memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
         if (*ifName != '\0') {
            sprintf(cmd, "ifconfig %s up >/var/ifcfgerr", ifName);
            bcmSystem(cmd);

            str[0] = '\0';
            errFs = fopen("/var/ifcfgerr", "r");
            if (errFs != NULL ) {
               fgets(str, IFC_LARGE_LEN, errFs);
               fclose(errFs);
               bcmSystem("rm /var/ifcfgerr");
               if (strstr(str, "error fetching interface information") != NULL)
                  return sts = ROUTE_STS_ERR_GENERAL;
            }
         }
      } else {
         break;
      }
      pIfName = pIfNameEnd + 1;
   }

   // Allocate a new bridge entry in configuration and get its interface name.
   if (BcmDb_allocBridge(grpName, brIfName) == -1)
      return sts = PORT_MAP_STS_ERR_GENERAL;

   // Add the bridge and set its configuration.
   sprintf(cmd, "brctl addbr %s 2>/dev/null", brIfName);
   bcmSystem(cmd);
   sprintf(cmd, "brctl stp %s disable 2>/dev/null", brIfName);
   bcmSystem(cmd);
   sprintf(cmd, "brctl setfd %s 0 2>/dev/null", brIfName);
   bcmSystem(cmd);
   // ifconfig up the bridge
   sprintf(cmd, "ifconfig %s up 2>/dev/null", brIfName);
   bcmSystem(cmd);

   pIfName = grpedIntf;
   while (pIfName <= (grpedIntf + strlen(grpedIntf))) {
      if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) {
         memset(ifName, '\0', IFC_TINY_LEN);
         memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
         if (*ifName != '\0') {
            BcmDb_delIfFromBridge(ifName);
            BcmDb_addBridgeIf(grpName, ifName);
            sprintf(cmd, "brctl delif br0 %s 2>/dev/null", ifName);
            bcmSystem(cmd);
         }
      } else {
         break;
      }
      pIfName = pIfNameEnd + 1;
   }

   // Get the selected interfaces for this group and parse them to
   // remove the | from the list.
   pIfName = grpedIntf;
   while (pIfName <= (grpedIntf + strlen(grpedIntf))) {
      if ((pIfNameEnd = strstr(pIfName, "|")) != NULL) {
         memset(ifName, '\0', IFC_TINY_LEN);
         memcpy(ifName, pIfName, (pIfNameEnd - pIfName));
         if (*ifName != '\0') {
            BcmDb_delIfFromBridge(ifName);
            BcmDb_addBridgeIf(grpName, ifName);
            sprintf(cmd, "brctl addif %s %s 2>/dev/null", brIfName, ifName);
            bcmSystem(cmd);
            // If we add an interface to br0 which is hooked to the IP stack, when
            // the MAC address changes (as we add or delete interfaces), we must
            // send an arp to all devices attached to it to propagate the change.
            if (strcmp(brIfName, IFC_BRIDGE_NAME) == 0) {
               sprintf(cmd, "sendarp -s %s -d %s", IFC_BRIDGE_NAME, IFC_BRIDGE_NAME);
               bcmSystem(cmd);
            }
         }
      } else {
         break;
      }
      pIfName = pIfNameEnd + 1;
   }
   return sts;
}
#endif

void bcmsend_message(int qid, struct optionmsgbuf *qbuf, long type, char *text)
{
        /* Send a message to the queue */
        //printf("Sending a message ...\n");
        qbuf->mtype = type;
        strcpy(qbuf->mtext, text);
	//printf("the length of the send message is %d\r\n", strlen(text));	
        if((msgsnd(qid, (struct msgbuf *)qbuf,
                strlen(qbuf->mtext)+1, 0)) ==-1)
        {
                perror("msgsnd");
                exit(1);
        }
}

void bcmread_message(int qid, struct optionmsgbuf *qbuf, long type)
{
        /* Read a message from the queue */
        //printf("Reading a message ...\n");
        qbuf->mtype = type;
        msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);

        //printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
}

void bcmremove_queue(int qid)
{
        /* Remove the queue */
        msgctl(qid, IPC_RMID, 0);
}

void bcmchange_queue_mode(int qid, char *mode)
{
        struct msqid_ds myqueue_ds;

        /* Get current info */
        msgctl(qid, IPC_STAT, &myqueue_ds);

        /* Convert and load the mode */
        sscanf(mode, "%ho", &myqueue_ds.msg_perm.mode);

        /* Update the mode */
        msgctl(qid, IPC_SET, &myqueue_ds);
}

void bcmsend_dhcp(struct optionmsgbuf *qbuf, long type, char *text)
{
   int key, m_msgqueue_id;
   key = ftok("/var", 'm');
   /* Open the queue - create if necessary */
   if((m_msgqueue_id = msgget(key, IPC_CREAT|0660)) == -1) {
        perror("msgget");
        exit(1);
   }
   bcmsend_message(m_msgqueue_id, qbuf, type, text);

}

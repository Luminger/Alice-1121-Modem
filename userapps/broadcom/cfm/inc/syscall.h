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
******************************************************************************
//
//  Filename:       syscall.h
//  Author:         Peter T. Tran
//  Creation Date:  12/26/01
//
******************************************************************************
//  Description:
//      Define the global constants, and functions for sysCall utility.
//
*****************************************************************************/

#ifndef __SYS_CALL_H__
#define __SYS_CALL_H__
#include <netinet/in.h>

/********************** Global Functions ************************************/

#define FILE_OPEN_OK			0
#define FILE_OPEN_ERR			-1

#define MNTR_STS_OK			0
#define MNTR_STS_ADSL_DOWN		1
#define MNTR_STS_PPP_AUTH_ERR		2
#define MNTR_STS_PPP_DOWN		3
#define MNTR_STS_OPEN_ERR		4
#define MNTR_STS_DHCPC_PROGRESS  	5
#define MNTR_STS_DHCPC_FAIL   		6
#define MNTR_STS_ADSL_TRAINING		7
#define MNTR_STS_BRIDGE       		8
#define MNTR_STS_PPP_RETRY_AUTH		9

#define DHCPC_STS_FAIL           	0
#define DHCPC_STS_SUCCESS        	1
#define DHCPC_FAIL_MAX           	15

// Values for network protocol
#define PROTO_PPPOE			0
#define PROTO_PPPOA			1
#define PROTO_MER			   2
#define PROTO_BRIDGE			3
#define PROTO_PPPOE_RELAY	4
#define PROTO_IPOA			5
#define PROTO_IPOWAN       6
#define PROTO_NONE 			10

#define SYSTEM_APPID       "SystemInfo"
#define SYS_USER_NAME_ID   1
#define SYS_PASSWORD_ID    2
#define SPT_USER_NAME_ID   3
#define SPT_PASSWORD_ID    4
#define USR_USER_NAME_ID   5
#define USR_PASSWORD_ID    6
#define SYS_DFT_GTWY_ID    7
#define SYS_DNS_ID         8
#define SYS_LOG_ID         9
#define SYS_SNMP_ID        10
#define SYS_FLAG_ID        11
#define SYS_TR69C_ID       12
#define SYS_MGMT_LOCK_ID   13
#define SYS_8021AG_ID      14
#define SYS_DFT_GTWY6_ID   15
#define SYS_DHCP6_ID       16

// Values that are used in syscall.c
#define SYS_CMD_LEN        256

#define IP_TBL_COL_NUM     0
#define IP_TBL_COL_PKTS    1
#define IP_TBL_COL_BYTES   2
#define IP_TBL_COL_TARGET  3
#define IP_TBL_COL_PROT    4
#define IP_TBL_COL_OPT     5
#define IP_TBL_COL_IN      6
#define IP_TBL_COL_OUT     7
#define IP_TBL_COL_SRC     8
#define IP_TBL_COL_DST     9
#define IP_TBL_COL_MAX     10

#define UPNP_IP_ADDRESS    "239.255.255.250"

#if defined(__cplusplus)
extern "C" {
#endif
#define bcmSystem(cmd)		bcmSystemEx (cmd,1)
#define bcmSystemMute(cmd)	bcmSystemEx (cmd,0)


#define MAX_SEND_SIZE 256

struct optionmsgbuf {
        long mtype;
        char mtext[MAX_SEND_SIZE];
};

int bcmSystemEx (char *command, int printFlag);
int bcmSystemNoHang (char *command);
int bcmGetPid (char *command);
int *bcmGetPidList(char * command);
int bcmCreateDhcpCfg(char *ipAddr, char *mask, char *addrStart, char *addrEnd,
                     char *dns1, char *dns2, int leasedTime, int protocol,
                     int enblNat, int enblFirewall, char *domain);
#ifdef MULTISUBNET
int bcmCreateDhcpCfgEx(char * szInterface, char *ipAddr, char *mask,
                     char *addrStart, char *addrEnd,
                     char *dns1, char *dns2, int leasedTime,
                     int protocol, int enblNat, int enblFirewall, char* domain);
#endif
#ifdef SUPPORT_SUPER_DMZ // Keven -- super DMZ
int bcmCreateDmzDhcpCfg(char *ipAddr, char *mask,
                        char *addrStart, char *addrEnd, char* domain);
#endif
int bcmCreateIpExtDhcpCfg(char *lanAddr, char *mask, char *wanAddr, char *domain, char *remote);
int bcmCreateLocalDhcpCfg(char *addr, char *mask, char *domain);
int bcmCreateResolvCfg(char *dns1, char *dns2);
int bcmCreateLoginCfg(char *cp_admin, char *cp_support, char *cp_user);
int bcmGetAdslStatus(void);
int bcmGetVdslStatus(void);
int bcmGetXdslStatus(void); 
int bcmGetPppStatus(char *str, int len, char *devName);
//LGD_FOR_TR098
int bcmGetPppShowtimeSecs(char *str, int len, char *name);
int bcmGetPppDisconnectReason(char *str, int len, char *name) ;
int bcmGetDhcpcStatus(char *str, int len);
int bcmGetSystemStatus(char *str, int len);
int bcmSetSystemStatus(int status);
int bcmSetIpExtension(int ipExt);
int bcmGetIpExtension(char *str, int len);
void bcmDisplayLed(int status);
void bcmGetDns(char *dns);
void bcmGetDns2(char *dns);
void bcmGetDnsSettings(int *mode, char *primary, char *secondary);
#if defined(SUPPORT_DNSPROBE)
void bcmRestartDnsProbe(void);
#endif
#if defined(SUPPORT_DPROXY)
void bcmStartDProxy(void);
void bcmStopDProxy(void);
void bcmSetDProxyConfig(void);
#endif
void bcmConfigDns(char *dns1, char *dns2, int dynamic);
void bcmRemoveDefaultGatewayByWanIf(char *wanIf);
void bcmGetDefaultGateway(char *gtwy, char *wanIf);
void bcmGetDefaultGatewayWanIf(char *gtwy, char *wanIf);
int  bcmIsDefaultGatewayExisted(char *gw, char *wanIf);
int bcmIsDefaultGatewayIfName(char *wanIf);
void bcmSetAutoDefaultGateway(char *errMsg);
void bcmSetStaticDefaultGateway(char *gw, char *wanIf, char *errMsg);
#ifdef SUPPORT_IPV6
int  bcmIsIPv6Enabled(void);
int  bcmIsDhcp6cEnabled(void);
void bcmGetLan6Prefix(char *lan6Prfx);
void bcmGetDns6(char *dns6Pri, char *dns6Sec);
void bcmSetDhcp6Info(int enblDhcp6c, char *lan6Prfx, char *dns6Pri, char *dns6Sec, char *errMsg);
void bcmGetDefaultGateway6(char *gtwy, char *wanIf);
void bcmSetStaticDefaultGateway6(char *gw, char *wanIf, char *errMsg);
void bcmRemoveDefaultGateway6ByWanIf(char *wanIf);
#endif
void bcmUploadPrepare(char *ifcName);
void bcmKillAllApps(void);
void disconnectPPP(void);
void setWanLinkStatus(int up);
int bcmCheckInterfaceUp(char *devname);
int bcmSetIpExtInfo(char *wan, char *gateway, char *dns);
void bcmGetIpExtInfo(char *buf, int type);
int bcmGetIfcIndexByName(char *ifcName);
int bcmIsValidIfcName(char *ifcName);
char *bcmGetIfcNameById(int ifcId, char *ifcName);
char *bcmGetIfcNameByIpAddr(unsigned long ipAddr, char *ifcName);
void getPppoeServiceName(char *service, char *ifName);
void bcmGetDefaultRouteInterfaceName(char *ifcName);

void bcmSetConnTrackMax(void);
void bcmResetConnTrackTable(void);
void bcmHandleConnTrack(void);
void bcmInsertModules(char *path);
void bcmInsertModule(char *modName);
int bcmIsModuleInserted(char *modName);
int bcmRemoveIpTableRule(char *device, char *table, char *chain);
void bcmRemoveAllIpTableRules(char *device);
#if SUPPORT_TC
void bcmRemoveTrafficControlRules(unsigned int portId, unsigned short vpi, unsigned short vci,
                                  unsigned short conId, unsigned char protocol);
void bcmAddTrafficControlRules(unsigned int portId, unsigned short vpi, unsigned short vci,
                               unsigned short conId, unsigned char protocol);
#endif
int bcmRemoveRipIpTableRule(void);
int bcmRemoveUpnpIpTableRule(void);
void bcmInsertAllUpnpIpTableRules(void);
int bcmGetIntfNameSocket(int socketfd, char *intfname);
int bcmCheckEnable(char *appName, struct in_addr clntAddr);

int bcmRemoveEbTableRule(char *device, char *table, char *chain);
void bcmRemoveAllEbTableRules(char *device);

void parseStrInfo(char *info, char *var, char *val, int len);
void bcmConvertStrToShellStr(char *str, char *buf);
void bcmProcessMarkStrChars(char *str);
int  bcmIsMarkStrChar(char c);
void bcmSetVdslSwVer(char *swVer);
void bcmGetSwVer(char *swVer, int size);
void bcmGetVdslSwVer(char *swVer, int size);
int isAccessFromLan(struct in_addr clntAddr);
int checkChipId(char *strTagChipId, char *sig2);
void bcmRemoveModules(int lanIf);
int bcmWaitIntfExists(char *ifName);

int bcmMacStrToNum(char *macAddr, char *str);
int bcmMacNumToStr(char *macAddr, char *str);
char* writeIp(int ip);
void writeIp_b(int ip, char *buf);
char* writeMac(const unsigned char* mac);
int readIp(const char* ip);
const char *itoa(int i);


int bcmWanEnetQuerySwitch(char *ifName);

int BcmGetIfcNumOfSwith(char * ifName);
int BcmGetEthIndexToIfName(unsigned long index,char* ifName);
void BcmLan_ExtConfig(char* ethInfo, int objIndex);

struct lan_ifname_info {
    char name[12];
    int id;
    char ifName[12];
};

int bcmConfigPortMirroring (void *pMirrorCfg) ;
void bcmStoreDeviceInfoToProcFile(void);
void bcmsend_message(int qid, struct optionmsgbuf *qbuf, long type, char *text);
void bcmread_message(int qid, struct optionmsgbuf *qbuf, long type);
void bcmremove_queue(int qid);
void bcmchange_queue_mode(int qid, char *mode);
void bcmsend_dhcp(struct optionmsgbuf *qbuf, long type, char *text);
int getLanInfo(char *lan_ifname, struct in_addr *lan_ip, struct in_addr *lan_subnetmask);
void bcmReboot(void);

#ifdef SUPPORT_PORT_MAP
#include "pmapapi.h"
#include "pmapdefs.h"
#define START_PMAP_ID           2
int bcmPortMapQrySwitch(char *ifName);
void bcmGetBridgeInterfaceInfo(char *varValue, char *skip);
void bcmPortMapIfcStsEnable(char *ifName);
PORT_MAP_STATUS bcmPortmapAdd(char *grpName, char *grpedIntf);
#endif

#define BCM_PPPOE_CLIENT_STATE_PADO          0   /* waiting for PADO */
#define BCM_PPPOE_CLIENT_STATE_PADS          1   /* got PADO, waiting for PADS */
#define BCM_PPPOE_CLIENT_STATE_CONFIRMED     2   /* got PADS, session ID confirmed */
#define BCM_PPPOE_CLIENT_STATE_DOWN          3   /* totally down */
#define BCM_PPPOE_CLIENT_STATE_UP            4   /* totally up */
#define BCM_PPPOE_SERVICE_AVAILABLE          5   /* ppp service is available on the remote */

#define BCM_PPPOE_AUTH_FAILED                7
#define BCM_PPPOE_RETRY_AUTH                 8

#define BCM_PRIMARY_DNS                      1
#define BCM_SECONDARY_DNS                    0

#define PPP_ERROR_NONE 						0
#define PPP_ERROR_ISP_TIME_OUT					1
#define PPP_ERROR_COMMAND_ABORTED				2
#define PPP_ERROR_NOT_ENABLED_FOR_INTERNET		        3
#define PPP_ERROR_BAD_PHONE_NUMBER				4
#define PPP_ERROR_USER_DISCONNECT				5
#define PPP_ERROR_ISP_DISCONNECT				6
#define PPP_ERROR_IDLE_DISCONNECT				7
#define PPP_ERROR_FORCED_DISCONNECT				8
#define PPP_ERROR_SERVER_OUT_OF_RESOURCES		        9
#define PPP_ERROR_RESTRICTED_LOGON_HOURS		        10
#define PPP_ERROR_ACCOUNT_DISABLED				11
#define PPP_ERROR_ACCOUNT_EXPIRED				12
#define PPP_ERROR_PASSWORD_EXPIRED				13
#define PPP_ERROR_AUTHENTICATION_FAILURE		        14
#define PPP_ERROR_NO_DIALTONE					15
#define PPP_ERROR_NO_CARRIER					16
#define PPP_ERROR_NO_ANSWER					17	
#define PPP_ERROR_LINE_BUSY					18
#define PPP_ERROR_UNSUPPORTED_BITSPERSECOND		        19
#define PPP_ERROR_TOO_MANY_LINE_ERRORS			        20
#define PPP_ERROR_IP_CONFIGURATION				21
#define PPP_ERROR_UNKend					22

#if defined(__cplusplus)
}
#endif

#endif

/*****************************************************************************
//
//  Copyright (c) 2000-2001  Broadcom Corporation
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
//  Filename:       ifcdefs.h
//  Author:         Peter T. Tran
//  Creation Date:  12/26/2001
//
//  Description:
//      Define the Driver Interface class and functions.
//
*****************************************************************************/

#ifndef __IFC_DEFS_H
#define __IFC_DEFS_H

/********************** Include Files ***************************************/

#include "bcmtypes.h"
#include "bcmatmapi.h"
#include "bcmxtmcfg.h"
#include <netinet/in.h>

/********************** Global Constants ************************************/

/**************************************************************************
*    BCM_STATUS and its values. BCM_STATUS is used by all the private BCM
*    HAL and BCM MAC functions to return status.
*
***************************************************************************/

typedef UINT32                               BCM_STATUS, *PBCM_STATUS;

/*
 * BCM_STATUS values
 *
 *   Status values are 32 bit values laid out as follows:
 *
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +---+-+-------------------------+-------------------------------+
 *  |Sev|C|       Facility          |               Code            |
 *  +---+-+-------------------------+-------------------------------+
 *
 *  where
 *
 *    Sev - is the severity code
 *      00 - Success
 *      01 - Informational
 *      10 - Warning
 *      11 - Error
 *
 *    C - is the Customer code flag
 *
 *    Facility - is the facility code
 *
 *    Code - is the facility's status code
 */

#define STATUS_UNSUCCESSFUL                 ((BCM_STATUS)0xC0000001L)
#define STATUS_SUCCESS                      ((BCM_STATUS)0x00000000L)
#define STATUS_PENDING                      ((BCM_STATUS)0x00000103L)
#define STATUS_BUFFER_OVERFLOW              ((BCM_STATUS)0xC0000104L)
#define STATUS_INSUFFICIENT_RESOURCES       ((BCM_STATUS)0x80000105L)
#define STATUS_NOT_SUPPORTED                ((BCM_STATUS)0x40000106L)

#define BCM_STATUS_SUCCESS                  ((BCM_STATUS)STATUS_SUCCESS)
#define BCM_STATUS_PENDING                  ((BCM_STATUS)STATUS_PENDING)

#define BCM_STATUS_FAILURE                  ((BCM_STATUS)STATUS_UNSUCCESSFUL)
#define BCM_STATUS_RESOURCES                ((BCM_STATUS)STATUS_INSUFFICIENT_RESOURCES)
#define BCM_STATUS_NOT_SUPPORTED            ((BCM_STATUS)STATUS_NOT_SUPPORTED)
#define BCM_STATUS_INVALID_LENGTH           ((BCM_STATUS)0xC0010014L)
#define BCM_STATUS_INVALID_OID              ((BCM_STATUS)0xC0010015L)

#define IFC_TINY_LEN            16
#define IFC_SMALL_LEN           32
#define IFC_MEDIUM_LEN          40
#define IFC_LARGE_LEN           264
#define IFC_GIANT_LEN           1024
#define IFC_DOMAIN_LEN          64
#define IFC_HOST_LEN            64
#define IFC_PASSWORD_LEN        24
#define IFC_IPV4_ADDR_LEN       16
#define IFC_IPV6_ADDR_LEN       40
#define IFC_PPP_SESSION_LEN     18
#define IFC_PPP_SESSIONS_NAME   "sessions"
#define WEB_SPACE               "&nbsp"

#define MAX_IPV6_PREFIX_LEN     128

#define IFC_STATUS_UNKNOWN          0
#define IFC_STATUS_NEW              1
#define IFC_STATUS_AUTH_ERR         2
#define IFC_STATUS_DOWN             3
#define IFC_STATUS_DHCPC_PROGRESS   4
#define IFC_STATUS_DHCPC_FAIL       5
#define IFC_STATUS_UP               6

#define IFC_WAN_MAX                 8
#define ATM_QUEUE_MAX               8

#define IFC_WLAN_MAX                 4

#if defined(CHIP_6348) || defined(CHIP_6358)
#define ATM_VCC_MAX                 16
#define IFC_PPP_MAX                 16
#else
#define ATM_VCC_MAX                 8
#define IFC_PPP_MAX                 8
#endif
#ifdef CT_SNMP_MIB_VERSION_2
#define ATM_TD_MAX	8
#endif
#define ATM_VCC_APPID 			  "AtmCfgVcc"
#define ATM_TD_APPID 			  "AtmCfgTd"
#define ATM_CFG_APPID 			  "AtmCfg"
#define PPP_APPID 			     "PppInfo"
#define DHCP_CLT_APPID		     "DhcpClntInfo"

#define LAN_APPID 			     "Lan"
#define IFC_NAME_LEN		        16
#define IFC_IP_ADDR_SIZE   	  16
#define IFC_LAN_MAX        	  4
#define IFC_UNKNOWN_ID     	  0
#define IFC_ENET_ID	          1
#define IFC_ENET1_ID            (IFC_ENET_ID+1)
#define IFC_USB_ID		        (IFC_ENET_ID + IFC_LAN_MAX)
#define IFC_HPNA_ID		        (IFC_USB_ID + IFC_LAN_MAX)
#define IFC_WIRELESS_ID		    (IFC_HPNA_ID + IFC_LAN_MAX)
#define IFC_WIRELESS1_ID	    (IFC_WIRELESS_ID + IFC_LAN_MAX)
#define IFC_WIRELESS2_ID	    (IFC_WIRELESS1_ID + IFC_LAN_MAX)
#define IFC_WIRELESS3_ID	    (IFC_WIRELESS2_ID + IFC_LAN_MAX)
#define IFC_SUBNET_ID           (IFC_WIRELESS_ID + IFC_LAN_MAX*IFC_WLAN_MAX) // for sencond LAN subnet
#define IFC_ENET0_VNET_ID       (IFC_SUBNET_ID + IFC_LAN_MAX)
#define IFC_ENET1_VNET_ID       (IFC_ENET0_VNET_ID + IFC_LAN_MAX)
#ifdef MULTISUBNET
#define IFC_LAN_GROUPBASE_ID   100
#define IFC_LAN_GROUP1_ID          (IFC_LAN_GROUPBASE_ID + 1)
#endif 
#define WAN_APPID 			     "Wan"

#define BR2684_APPID 			  "BR2684"
#define IFC_BR2684_ID		     16
#define IFC_BRIDGE_NAME			  "br0"

#define MER2684_APPID 			  "MER2684"
#define IFC_RFC2684_MAX    	  8
#define IFC_MER2684_ID		     (IFC_BR2684_ID + IFC_RFC2684_MAX)

#define PPPOE_APPID 			     "PPPOE"
#define IFC_PPPOE2684_ID   	  (IFC_MER2684_ID + IFC_RFC2684_MAX)

#define PPPOA_APPID 			     "PPPOA"
#define IFC_PPPOA2684_ID   	  (IFC_PPPOE2684_ID + IFC_RFC2684_MAX)

#define IPOA_APPID 			     "IPOA"
#define IFC_IPOA2684_ID   	     (IFC_PPPOA2684_ID + IFC_RFC2684_MAX)

#define PPP_AUTH_METHOD_AUTO    0
#define PPP_AUTH_METHOD_PAP     1
#define PPP_AUTH_METHOD_CHAP    2
#define PPP_AUTH_METHOD_MSCHAP  3

#ifdef CPE_DSL_MIB
#define PPP_ADMIN_STATUS_UP       1
#define PPP_ADMIN_STATUS_DOWN     2
#define PPP_ADMIN_STATUS_TESTING  3
#define PPP_ADMIN_STATUS_CANCEL   4
#endif

#define ATM_AUX_CHANNEL_PORT    0
//DARE MANUAL PPP
#define PPP_START_MANUAL         1
#define PPP_START_AUTOMATIC      0
//DARE MANUAL PPP

#define ATM_AUX_CHANNEL_VPI     1
#define ATM_AUX_CHANNEL_VCI     39

#define INSTID_OBJID            9999
#define INSTID_VALID_VALUE      0x5a000000
#define INSTID_VALID_MASK       0xff000000

#define VLANMUX_DISABLE -1
#define VLANMUX_MATCH_ANY -2

#define NUM_OF_ALLOWED_SVC_PPPOE 4
#define NUM_OF_ALLOWED_SVC_MER   1
#define NUM_OF_ALLOWED_SVC_BR    1

typedef struct IfcAtmTdInfo {
   UINT32 peakCellRate;
   UINT32 sustainedCellRate;
   UINT32 maxBurstSize;
   char serviceCategory[IFC_TINY_LEN];
} IFC_ATM_TD_INFO, *PIFC_ATM_TD_INFO;

typedef struct IfcAtmVccInfo {
   UINT32 id;
   UINT16 tdId;
   UINT32 aalType;
   UINT32 adminStatus;
   UINT16 encapMode;
   UINT16 enblQos;
   ATM_VCC_ADDR vccAddr;
} IFC_ATM_VCC_INFO, *PIFC_ATM_VCC_INFO;

#define DHCP_SRV_DISABLE   0
#define DHCP_SRV_ENABLE    1
#define DHCP_SRV_RELAY     2

typedef struct IfcDhcpSrvInfo {
   UINT16 enblDhcpSrv;
//LGD_FOR_TR098
   UINT16 enbConfDhcpSrv;
   UINT16 leasedTime;
   struct in_addr startAddress;
   struct in_addr endAddress;
//LGD_FOR_TR098
   struct in_addr subnetMask;
} IFC_DHCP_SRV_INFO, *PIFC_DHCP_SRV_INFO;

typedef struct IfcLanInfo {
   UINT32 id;
   struct in_addr lanAddress;
   struct in_addr lanMask;
   IFC_DHCP_SRV_INFO dhcpSrv;
} IFC_LAN_INFO, *PIFC_LAN_INFO;

typedef struct IfcDnsInfo {
   struct in_addr preferredDns;
   struct in_addr alternateDns;
   UINT16 dynamic;
   char domainName[IFC_DOMAIN_LEN];
   char hostName[IFC_HOST_LEN];
} IFC_DNS_INFO, *PIFC_DNS_INFO;

typedef struct IfcDefGWInfo {
   UINT16 enblGwAutoAssign;     // ==1 use auto assigned def. gw
//dare zy add enblpptpgw for set pptp default gateway
   UINT16 enblpptpgw;
//add end
   char ifName[IFC_TINY_LEN];
   struct in_addr defaultGateway;
} IFC_DEF_GW_INFO, *PIFC_DEF_GW_INFO;

#ifdef SUPPORT_IPV6
typedef struct IfcDhcp6Info {
   UINT32 enblDhcp6c;
   struct in6_addr lan6Prefix;
   struct in6_addr dns6Primary;
   struct in6_addr dns6Secondary;
} IFC_DHCP6_INFO, *PIFC_DHCP6_INFO;

typedef struct IfcDefGW6Info {
   char ifName[IFC_TINY_LEN];
   struct in6_addr defaultGateway6;
} IFC_DEF_GW6_INFO, *PIFC_DEF_GW6_INFO;
#endif

#define ETH_SPEED_AUTO           0
#define ETH_SPEED_100            1
#define ETH_SPEED_10             2
#define ETH_TYPE_AUTO            0
#define ETH_TYPE_FULL_DUPLEX     1
#define ETH_TYPE_HALF_DUPLEX     2
#define ETH_CFG_ID               1024
#define ETH_CFG_MTU_MIN          46
#define ETH_CFG_MTU_MAX          1500
typedef struct EthCfgInfo {
   UINT16 ethSpeed;
   UINT16 ethType;
   UINT16 ethMtu;
   UINT16 ethIfcDisabled;
} ETH_CFG_INFO, *PETH_CFG_INFO;

// multiple protocols changes
// new database structures for multiple protocols
// over single PVC behind this line

// WAN flag define
// 8 bit
// bit 0 : enable firewall
// bit 1 : enable nat
// bit 2 : enable igmp
// bit 3 : enable service
// bit 4 : enable fullcone nat
// bit 5-7: reserve
typedef struct WanFlagInfo {
   UINT8
   firewall:1,
   nat:1,
   igmp:1,
   service:1,
   fullcone:1,
#ifdef CPE_DSL_MIB
   bridgeMode:1,
   macLearnMode:1,
   reserved:1;
#else
   reserved:3;
#endif
} WAN_FLAG_INFO;

typedef struct WanPppInfo {
   UINT16 idleTimeout;  // 0 disable onDemand, > 0 timeout in second
   UINT8 enblIpExtension;
   UINT8 authMethod;
//DARE MANUAL PPP
   UINT8 manual;
   UINT8 keepalive;
//dare zcy keepalivetime
   UINT8 keepalivetime;
   UINT8 alivemaxfail;
//add end
   UINT8 Reser2;
   UINT8 Reser3;
//DARE MANUAL PPP
   char userName[IFC_LARGE_LEN];
   char password[IFC_MEDIUM_LEN];
   char serverName[IFC_MEDIUM_LEN];
   UINT16 useStaticIpAddress;
   struct in_addr pppLocalIpAddress;
#ifdef SUPPORT_SUPER_DMZ // Keven -- super DMZ
   UINT8 enableSuperDmz;
   char nonDmzIpAddress[IFC_LARGE_LEN];
   char nonDmzIpMask[IFC_LARGE_LEN];
#endif
   UINT8 enblDebug;    // 0 disable debugging, 1 enable debugging
   UINT8 authErrorRetry; // 1=PPP login from browser on PPP authentication error
   UINT8 pppToBridge;
#ifdef CPE_DSL_MIB
   int adminStatus;   /*up, down, testing or cancel, default is up*/
#endif
   UINT16 pppMTU;    //dare zcy ppp mtu
} WAN_PPP_INFO, *PWAN_PPP_INFO;

typedef struct WanIpInfo {
   UINT8 enblDhcpClnt;
   struct in_addr wanAddress;
   struct in_addr wanMask;
#ifdef SUPPORT_IPV6
   struct in6_addr wanAddress6;
#endif
#ifdef SUPPORT_SUPER_DMZ // Keven -- super DMZ
   UINT8 enableSuperDmz;
   char nonDmzIpAddress[IFC_LARGE_LEN];
   char nonDmzIpMask[IFC_LARGE_LEN];
#endif
#ifdef CPE_DSL_MIB
   struct in_addr wanDefaultGw;
#endif
} WAN_IP_INFO, *PWAN_IP_INFO;

typedef struct WanConInfo {
   UINT32 id;
   UINT16 vccId;
   UINT16 conId;
   INT16 vlanMuxId;          // using vlan for multiple protocols on a single pvc
   UINT8 protocol;
   UINT8 encapMode;
   INT16 vlanId;           // 0 - 4095, -1 -> vlan is not enabled
   WAN_FLAG_INFO flag;
   char conName[IFC_MEDIUM_LEN];
} WAN_CON_INFO, *PWAN_CON_INFO;

typedef struct WanConId {
   UINT16 portId;
   UINT16 vpi;
   UINT16 vci;
   UINT16 conId;
} WAN_CON_ID, *PWAN_CON_ID;

typedef struct IfcPppSessionInfo {
   WAN_CON_ID wanId;
   char session[IFC_PPP_SESSION_LEN];
} IFC_PPP_SESSION_INFO, *PIFC_PPP_SESSION_INFO;

// System flag define
#define SYS_FLAG_AUTO_SCAN           0
#define SYS_FLAG_UPNP                1
#define SYS_FLAG_MAC_POLICY          2
#define SYS_FLAG_SIPROXD             3
#define SYS_FLAG_ENCODE_PASSWORD     4
#define SYS_FLAG_IGMP_SNP            5
#define SYS_FLAG_IGMP_MODE           6
#define SYS_FLAG_ENET_WAN            7
typedef struct SysFlagInfo {
   UINT8
   autoScan:1,
   upnp:1,
   macPolicy:1,
   siproxd:1,
   encodePassword:1,
   igmpSnp:1,
   igmpMode:1,
#if SUPPORT_ETHWAN
   enetWan:1;
#else
   reserved:1;
#endif
   int fltindft;
   int fltoutdft;
} SYS_FLAG_INFO, *PSYS_FLAG_INFO;

typedef struct SysMgmtLockInfo {
   UINT8 action;
   UINT8 type;
} SYS_MGMT_LOCK_INFO, *PSYS_MGMT_LOCK_INFO;

typedef struct Ifc8021agInfo {
   unsigned char mdlevel;
} IFC_8021AG_INFO, *PIFC_8021AG_INFO;

#ifdef SUPPORT_IPV6
//Scratch pad token ID for IPv6 enable
#define ENBL_IPV6    "enblIPv6"
#endif
//zcy add pptp

#define DARE_GLOBAL_APPID       "Global"

typedef struct DareGlobalInfo {
   int Pptpenbl;
   int PptpDftrt;
   char tunnelname[20];
   char Srvip[20];
   char username[32];
   char password[32];
   char peerip[20];
   char peermask[20];
   int pptpauth;
   int pptpcomp;
} DARE_GLOBAL_INFO, *PDARE_GLOBAL_INFO;


//zcy add pptp end

#endif   //  __IFC_DEFS_H


/*****************************************************************************
//
//  Copyright (c) 2004  Broadcom Corporation
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
//  Filename:       objectdefs.h
//  Author:         Peter Tran
//  Creation Date:  03/14/05
//
******************************************************************************
//  Description:
//      Define the global types, enums. and constants for PSI in object format
//
*****************************************************************************/

#ifndef __OBJECT_DEFS_H__
#define __OBJECT_DEFS_H__

/* important limitation: when adding the appName or objectName in either direction, it is 
important not to have appName and objName having the same prefix.   The table entry search
only compare strings up to the prefix length, case not sensitive, to cover situation
such as APP_ID wan_vpi_vci, only wan is searched in the table.    Therefore, it is not
okay to have Lan as the appId, and objectName as lanSomethingSomething.    The search
entry would think lanSomethingSomething is an appId name. */

#define APPNAME_WAN_PREFIX           "wan"
#define APPNAME_IPSRV_PREFIX         "ipsrv"
#define APPNAME_PPPSRV_PREFIX        "pppsrv"
#define APPNAME_PPP_FORMAT           "pppsrv_%d_%d_%d"
#define APPNAME_PPP_FORMAT_OLD              "pppsrv_%d_%d"
#define APPNAME_IPSRV_ENTRY_FORMAT   "ipsrv_%d_%d_%d"
#define APPNAME_IPSRV_ENTRY_FORMAT_OLD      "ipsrv_%d_%d"
#define OBJNAME_LAN_ENTRY_FORMAT     "entry%d"   /* cannot be lan, need to change */
#define OBJNAME_PPP_ENTRY_FORMAT     "ppp_conId%d"
#define OBJNAME_IPSRV_ENTRY_FORMAT   "dhcpc_conId%d"
#define OBJNAME_WAN_ENTRY_FORMAT     "entry%d"   /* cannot be wan, need to change */
#define OBJNAME_GEN_ENTRY_FORMAT     "entry" 

#define TBLNAME_DHCPD_STATIC_LEASE         "StaticIpLease"
#define ENTRYNAME_DHCPD_STATIC_LEASE       "StaticIpLeaseTbl"

#define OBJNAME_DMZ_HOST             "dmzHost"
#define OBJNAME_SRV_CTRL_LIST        "srvCtrlList"
#define TBLNAME_VRT_SRV              "vrtSrv"
#define ENTRYNAME_VRT_SRV            "vrtSrvEntry"
#define TBLNAME_OUT_FILTER           "outFilter"
#define ENTRYNAME_OUT_FILTER         "outFilterEntry"
#define TBLNAME_IN_FILTER            "inFilter"
#define ENTRYNAME_IN_FILTER          "inFilterEntry"
#define TBLNAME_PRT_TRIGGER          "prtTrigger"
#define ENTRYNAME_PRT_TRIGGER        "prtTriggerEntry"
#define TBLNAME_MAC_FILTER           "macFilter"
#define ENTRYNAME_MAC_FILTER         "macFilterEntry"
#define OBJNAME_QOS_MGMT_CFG         "qosMgmtCfg"
#define TBLNAME_QOS_CLS              "qosCls"
#define ENTRYNAME_QOS_CLS            "qosClsEntry"
#define TBLNAME_QOS_QUEUE            "qosQueue"
#define ENTRYNAME_QOS_QUEUE          "qosQueueEntry"
#define TBLNAME_ACC_CTRL             "accCtrl"
#define ENTRYNAME_ACC_CTRL           "accCtrlEntry"
#define OBJNAME_ACC_CTRL_MODE        "accCtrlMode"
#define TBLNAME_ROUTE                "route"
#define ENTRYNAME_ROUTE              "routeEntry"
#define TBLNAME_ROUTE6               "route6"
#define ENTRYNAME_ROUTE6             "route6Entry"
#define TBLNAME_RIP_IFC              "ripIfc"
#define ENTRYNAME_RIP_IFC            "ripIfcEntry"
#define OBJNAME_RIP_GLOBAL           "ripGlobal"
#if SUPPORT_PORT_MAP
#define TBLNAME_PMAP                 "pmap"
#define ENTRYNAME_PMAP               "pmapEntry"
#define OBJNAME_PMAP_IFC_CFG         "pmapIfcCfg"
#define TBLNAME_PMAPFLT              "pmapFlt"
#define ENTRYNAME_PMAPFLT            "pmapFltEntry"
#endif
#ifdef SUPPORT_DDNSD
#define TBLNAME_DDNS                 "ddns"
#define ENTRYNAME_DDNS               "ddnsEntry"
#endif
#ifdef WIRELESS
#define TBLNAME_WL_MSSID_VARS        "wlMssidVars"
#define TBLNAME_WL_MAC_FILTER        "wlMacFilter"
#define TBLNAME_WDS_FILTER           "wdsFilter"
#define TBLNAME_SCAN_WDS_FILTER      "scanWdsFilter"
#define ENTRYNAME_WL_MSSID_VARS      "wlMssidEntry"
#define ENTRYNAME_WL_MAC_FILTER      "wlMacEntry"
#define ENTRYNAME_WDS_FILTER         "wdsEntry"
#define ENTRYNAME_SCAN_WDS_FILTER    "scanWdsEntry"
#endif 
#ifdef SUPPORT_TOD
#define TBLNAME_PARENT_CNTRL         "parentCntrl"
#define ENTRYNAME_PARENT_CNTRL       "parentCntrlTbl"
/************************
 * UrlFilter modification
 * *********************/
#define TBLNAME_PARENT_URL_CNTRL         "UrlCntrl"
#define ENTRYNAME_PARENT_URL_CNTRL       "UrlCrlTbl"
#define OBJNAME_PARENT_URL_MGMT		"UrlListType"

#endif
#ifdef SUPPORT_PR
#define TBLNAME_POLICY_ROUTING         "PolicyRouting"
#define ENTRYNAME_POLICY_ROUTING       "PolicyRoutingTbl"
#endif
#ifdef SUPPORT_IPSEC
#define TBLNAME_IPSEC                "ipsconn"
#define ENTRYNAME_IPSEC              "ipsconnEntry"
#endif
#ifdef SUPPORT_CERT
#define TBLNAME_CERT_LOCAL           "certLocal"
#define ENTRYNAME_CERT_LOCAL         "certLocalEntry"
#define TBLNAME_CERT_CA              "certCA"
#define ENTRYNAME_CERT_CA            "certCAEntry"
#endif

#ifdef PORT_MIRRORING
#define TBLNAME_PORT_MIRROR_CFG      "portMirrorCfg"
#define ENTRYNAME_PORT_MIRROR_CFG    "portMirrorCfgEntry"
#define OBJNAME_PORT_MIRROR_STATUS   "portMirrorStatus"
#endif

/* system info */
#define PSX_OBJ_SYS_USER_NAME      "sysUserName"
#define PSX_OBJ_SYS_PASSWORD       "sysPassword"
#define PSX_OBJ_SPT_USER_NAME      "sptUserName"
#define PSX_OBJ_SPT_PASSWORD       "sptPassword"
#define PSX_OBJ_USR_USER_NAME      "usrUserName"
#define PSX_OBJ_USR_PASSWORD       "usrPassword"

#ifdef SUPPORT_TR69C
#define PSX_OBJ_TR69C              "tr69c"
#endif
#define PSX_OBJ_MGMT_LOCK          "mgmtLock"
#define PSX_OBJ_SOFTWARE_VERSION   "swVersion"
#define PSX_OBJ_HARDWARE_VERSION   "hwVersion"
#define PSX_OBJ_SYSLOG_DATA        "sysLogData"
#define PSX_OBJ_DEF_GATEWAY        "defaultGateway"
#define PSX_OBJ_DEF_GATEWAY6       "defaultGateway6"
#define PSX_OBJ_DNS                "dns"
#define PSX_OBJ_SYSLOG             "sysLog"
#define PSX_OBJ_SNMP               "snmp"
#define PSX_OBJ_PROTOCOL           "protocol"
#define PSX_OBJ_ETHCFG             "ethCfg"
#define PSX_OBJ_MGCP_CFG           "mgcpCfg"
#define PSX_OBJ_SIP_CFG            "sipCfg"
#define PSX_OBJ_8021AG             "p8021ag"
#define PSX_OBJ_DHCP6_INFO         "dhcp6Info"


#define PSX_ATTR_DNS_PRIMARY       "primary"
#define PSX_ATTR_DNS_SECONDARY     "secondary"
#define PSX_ATTR_DNS_DYNAMIC       "dynamic"
#define PSX_ATTR_DNS_DOMAIN        "domain"
#define PSX_ATTR_DNS_HOST          "host"

#define PSX_ATTR_SYSLOG_DISPLAY_LEVEL "displayLevel"
#define PSX_ATTR_SYSLOG_LOG_LEVEL     "logLevel"
#define PSX_ATTR_SYSLOG_OPTION        "option"
#define PSX_ATTR_SYSLOG_SERVER_IP     "serverIP"
#define PSX_ATTR_SYSLOG_SERVER_PORT   "serverPort"

#define PSX_ATTR_PMAP_IFC_NAME        "pmapIfName"
#define PSX_ATTR_PMAP_IFC_STATUS      "pmapIfcStatus"

#define PSX_ATTR_VOICE_MGCP_TAG            "mgcpCfg"
#define PSX_ATTR_VOICE_MGCP_CA_ADDR        "caAddr"
#define PSX_ATTR_VOICE_MGCP_GW_NAME        "gwName"
#define PSX_ATTR_VOICE_MGCP_IF_NAME        "ifName"
#define PSX_ATTR_VOICE_MGCP_PREF_CODEC     "prefCodec"
#define PSX_ATTR_VOICE_MGCP_AALN           "aaln"
#define PSX_ATTR_VOICE_SIP_TAG             "sipCfg"
#define PSX_ATTR_VOICE_SIP_PROXY_ADDR      "proxyAddr"
#define PSX_ATTR_VOICE_SIP_PROXY_PORT      "proxyPort"
#define PSX_ATTR_VOICE_SIP_MSERVER_ADDR    "musicServerAddr"
#define PSX_ATTR_VOICE_SIP_MSERVER_PORT    "musicServerPort"
#define PSX_ATTR_VOICE_SIP_OBPROXY_ADDR    "obProxyAddr"
#define PSX_ATTR_VOICE_SIP_OBPROXY_PORT    "obProxyPort"
#define PSX_ATTR_VOICE_SIP_REGISTRAR_ADDR  "regAddr"
#define PSX_ATTR_VOICE_SIP_REGISTRAR_PORT  "regPort"
#define PSX_ATTR_VOICE_SIP_TAG_MATCHING    "enblTagMat"
#define PSX_ATTR_VOICE_SIP_LOG_ADDR        "sipLogAddr"
#define PSX_ATTR_VOICE_SIP_LOG_PORT        "sipLogPort"
#define PSX_ATTR_VOICE_SIP_EXT1            "extension1"
#define PSX_ATTR_VOICE_SIP_PASSWORD1       "password1"
#define PSX_ATTR_VOICE_SIP_EXT2            "extension2"
#define PSX_ATTR_VOICE_SIP_PASSWORD2       "password2"
#define PSX_ATTR_VOICE_SIP_EXT3            "extension3"
#define PSX_ATTR_VOICE_SIP_PASSWORD3       "password3"
#define PSX_ATTR_VOICE_SIP_EXT4            "extension4"
#define PSX_ATTR_VOICE_SIP_PASSWORD4       "password4"
#define PSX_ATTR_VOICE_SIP_IF_NAME         "ifName"
#define PSX_ATTR_VOICE_SIP_DOMAIN_NAME     "domainName"
#define PSX_ATTR_VOICE_LOCALE_NAME         "localeName"
#define PSX_ATTR_VOICE_CODEC_NAME          "voiceCodec"
#define PSX_ATTR_VOICE_PTIME               "voicePtime"
#define PSX_ATTR_VOICE_LOGLEVEL            "logLevel"
#define PSX_ATTR_VOICE_PSTN_ROUTE_RULE     "pstnRouteRule"
#define PSX_ATTR_VOICE_PSTN_ROUTE_DATA     "pstnRouteData"
#define PSX_ATTR_VOICE_STORE               "store" // To indicate a write to flash
#ifdef STUN_CLIENT
#define PSX_ATTR_VOICE_STUN_SRV_ADDR       "stunSrvAddr"
#define PSX_ATTR_VOICE_STUN_SRV_PORT       "stunSrvPort"
#endif /* STUN_CLIENT */
#ifdef CALL_FEATURE_PERSIST_STORE_SUPPORT
#define PSX_ATTR_VOICE_CALLFWD_EXT1        "callfwdNumber1"
#define PSX_ATTR_VOICE_CALLFWD_EXT2        "callfwdNumber2"
#define PSX_ATTR_VOICE_CALLFWD_EXT3        "callfwdNumber3"
#define PSX_ATTR_VOICE_CALLFWD_EXT4        "callfwdNumber4"
#define PSX_ATTR_VOICE_CALLFWD_BUSY        "callfwdBusy"
#define PSX_ATTR_VOICE_CALLFWD_ALL         "callfwdAll"
#define PSX_ATTR_VOICE_CALLFWD_NOAN        "callfwdNoAn"
#define PSX_ATTR_VOICE_CALLWAIT            "callwait"
#endif

/* #ifdef ADVANCED_FEATURE_CONFIG_GUI_SUPPORT */
#define PSX_ATTR_VOICE_SIP_DISPLAY_NAME1   "dispName1"
#define PSX_ATTR_VOICE_SIP_AUTH_NAME1      "authName1"
#define PSX_ATTR_VOICE_SIP_DISPLAY_NAME2   "dispName2"
#define PSX_ATTR_VOICE_SIP_AUTH_NAME2      "authName2"
#define PSX_ATTR_VOICE_SIP_DISPLAY_NAME3   "dispName3"
#define PSX_ATTR_VOICE_SIP_AUTH_NAME3      "authName3"
#define PSX_ATTR_VOICE_SIP_DISPLAY_NAME4   "dispName4"
#define PSX_ATTR_VOICE_SIP_AUTH_NAME4      "authName4"
#define PSX_ATTR_VOICE_SIP_REG_EXPIRE_TMR  "regExpTmr"
#define PSX_ATTR_VOICE_SIP_TRANSPORT       "transport"
#define PSX_ATTR_VOICE_SIP_DIGIMAP         "digiMap"
#define PSX_ATTR_VOICE_SIP_LINE_ENABLE     "lineEnable"
#define PSX_ATTR_VOICE_SIP_WMI_ENABLE      "wmiEnable"
#define PSX_ATTR_VOICE_SIP_CODEC_LIST      "codecList"
#define PSX_ATTR_VOICE_SIP_DTMF_RELAY      "dtmfRelay"
#define PSX_ATTR_VOICE_SIP_T38_ENABLE      "t38Enable"
#define PSX_ATTR_VOICE_SIP_VAD_ENABLE      "vadEnable"
#define PSX_ATTR_VOICE_PSTN_ROUTE          "pstnRoute"
#define PSX_ATTR_VOICE_TX_GAIN             "egressGain"
#define PSX_ATTR_VOICE_RX_GAIN             "ingressGain"
#define PSX_ATTR_VOICE_TOSBYTE             "tosByte"
#define PSX_ATTR_VOICE_SIP_TR104_VPLINENUMLIST     "tr104vpLineNumList"
#define PSX_ATTR_VOICE_SIP_TR104_VPPHYSLINENUMLIST "tr104vpPhysLineNumList"

/* #endif */


#define PSX_ATTR_ETH_CFG_SPEED        "speed"
#define PSX_ATTR_ETH_CFG_TYPE         "type"
#define PSX_ATTR_ETH_CFG_MTU          "MTU"
#define PSX_ATTR_IPP_MAKE             "make"
#define PSX_ATTR_IPP_DEV              "dev"
#define PSX_ATTR_IPP_NAME             "name"

// parameter for OBJ_SYSTEM_ACTION
// get operation
#define OBJ_SYSTEM_PSI_CHANGE_NUM     0
#define OBJ_SYSTEM_LAN_DEVICE_NUM     1
#define OBJ_SYSTEM_LAN_STATISTICS     2
#define OBJ_SYSTEM_WAN_DEVICE_NUM     3
#define OBJ_SYSTEM_WAN_STATISTICS     4

// wan statistic fields
#define STS_FROM_LAN                  0
#define STS_FROM_WAN                  1
#define OBJ_STS_IFC_COLS              8
#define OBJ_STS_RX_BYTE               0
#define OBJ_STS_RX_PACKET             1
#define OBJ_STS_RX_ERR                2
#define OBJ_STS_RX_DROP               3
#define OBJ_STS_TX_BYTE               4
#define OBJ_STS_TX_PACKET             5
#define OBJ_STS_TX_ERR                6
#define OBJ_STS_TX_DROP               7
#define OBJ_STS_ALL                   8

// set operation
#define OBJ_SYSTEM_REBOOT             0
#define OBJ_SYSTEM_SAVE_CONFIG        1
#define OBJ_SYSTEM_SAVE_IMAGE         2
#define OBJ_SYSTEM_RESTORE_DEFAULT    3
#define OBJ_SYSTEM_KILL_APPS          4
// get operation
// variable name
#define OBJ_SYSTEM_ACTION             "systemAction"

// parameters for IFC_LAN_INFO
#define OBJ_LAN_INFO               0
#define OBJ_LAN_IP_ADDRESS         1
#define OBJ_LAN_SUBNET_MASK        2
#define OBJ_DHCP_SRV_ENABLE        3
#define OBJ_DHCP_SRV_LEASE_TIME    4
#define OBJ_DHCP_SRV_START_ADDR    5
#define OBJ_DHCP_SRV_END_ADDR      6
//LGD_FOR_TR098
#define OBJ_DHCP_SRV_SUBNET_MASK   7
#define OBJ_DHCP_SRV_CONFIGABLE	   8
// parameter for IFC_ATM_TD_INFO
// get & set operation
#define OBJ_TD_INFO                0
#define OBJ_TD_PEAK_CELL_RATE      1
#define OBJ_TD_SUSTAINED_CELL_RATE 2
#define OBJ_TD_MAX_BURST_SIZE      3
#define OBJ_TD_SERVICE_CATEGORY    4
// get operation
#define OBJ_TD_ID_FROM_INFO        5
#define OBJ_TD_ID_FROM_INDEX       6
#define OBJ_TD_ID_AVAILABLE        7

// parameter for IFC_ATM_VCC_INFO
// get & set operation
#define OBJ_VCC_INFO               0
#define OBJ_VCC_TD_ID              1
#define OBJ_VCC_AAL_TYPE           2
#define OBJ_VCC_ADMIN_STATUS       3
#define OBJ_VCC_ENCAP_MODE         4
#define OBJ_VCC_ENABLE_QOS         5
#define OBJ_VCC_VPI                6
#define OBJ_VCC_VCI                7
#define OBJ_VCC_IFC_ID             8
// get operation
#define OBJ_VCC_ID_FROM_ADDRESS    9
#define OBJ_VCC_ID_AVAILABLE       10
// delete operation
#define OBJ_VCC_DELETE_BY_ID       11
#define OBJ_VCC_DELETE_BY_ADDRESS  12

// parameter for WAN_CON_INFO
// get & set operation
#define OBJ_WAN_CON_INFO           0
#define OBJ_WAN_VCC_ID             1
#define OBJ_WAN_CON_ID             2
#define OBJ_WAN_PROTOCOL           3
#define OBJ_WAN_ENCAP_MODE         4
#define OBJ_WAN_CON_NAME           5
#define OBJ_WAN_FLAG_FIREWALL      6
#define OBJ_WAN_FLAG_NAT           7
#define OBJ_WAN_FLAG_IGMP          8
#define OBJ_WAN_FLAG_SERVICE       9
#define OBJ_WAN_FLAG_FULLCONE      10
// get operation
#define OBJ_WAN_ID_AVAILABLE       20

// parameter for WAN_PPP_INFO
// get & set operation
#define OBJ_PPP_INFO               0
#define OBJ_PPP_IDLE_TIMEOUT       1
#define OBJ_PPP_IP_EXTENSION       2
#define OBJ_PPP_AUTH_METHOD        3
#define OBJ_PPP_USER_NAME          4
#define OBJ_PPP_PASSWORD           5
#define OBJ_PPP_SERVICE_NAME       6
#define OBJ_PPP_USE_STATIC_IP_ADDR 7
#define OBJ_PPP_LOCAL_IP_ADDR      8
#define OBJ_PPP_DEBUG              9
// Keven -- super DMZ
#define OBJ_PPP_USE_SUPER_DMZ      10
#define OBJ_PPP_NON_DMZ_IPADDR     11
#define OBJ_PPP_NON_DMZ_NETMSK     12

// parameter for WAN_IP_INFO
// get & set operation
#define OBJ_IP_INFO                0
#define OBJ_IP_ENABLE_DHCP_CLNT    1
#define OBJ_IP_WAN_IP_ADDR         2
#define OBJ_IP_WAN_SUBNET_MASK     3
// Keven -- super DMZ
#define OBJ_USE_SUPER_DMZ          4
#define OBJ_NON_DMZ_IPADDR         5
#define OBJ_NON_DMZ_NETMSK         6

// parameter for DDNSD
// set operation
#ifdef SUPPORT_DDNSD
#define OBJ_DDNSD_INIT             0
#define OBJ_DDNSD_RESTART          1
#define OBJ_DDNSD_ADD              2
#define OBJ_DDNSD_STORE            3
#define OBJ_DDNSD_CLOSE            4
#endif   // SUPPORT_DDNSD

// parameter for SNTP
// set operation
#ifdef SUPPORT_SNTP
#define OBJ_SNTP_INIT              0
#define OBJ_SNTP_RESTART           1
#define OBJ_STNP_SET_VAR           2
#define OBJ_SNTP_STORE             3
#define OBJ_SNTP_CLOSE             4
// variable name
#define OBJ_SNTP_SERVER_1          "ntpServer1"
#define OBJ_SNTP_SERVER_2          "ntpServer2"
#define OBJ_SNTP_TIMEZONE          "timezone"
#define OBJ_SNTP_ENABLED           "ntp_enabled"
#define OBJ_SNTP_USE_DST           "use_dst"
#define OBJ_SNTP_OFFSET            "timezone_offset"
#define OBJ_SNTP_DST_START         "dst_start"
#define OBJ_SNTP_DST_END           "dst_end"
#define OBJ_SNTP_REFRESH           "sntpRefresh"
#endif   // SUPPORT_SNTP

// parameter for IPP
// set operation
#ifdef SUPPORT_IPP
#define OBJ_IPP_INIT               0
#define OBJ_IPP_RESTART            1
#define OBJ_IPP_SET_VAR            2
#define OBJ_IPP_STORE              3
#define OBJ_IPP_CLOSE              4
// variable name
#define OBJ_IPP_ENABLED            "ipp_enabled"
#define OBJ_IPP_REFRESH            "ippRefresh"
#define OBJ_IPP_MAKE               "ippMake"
#define OBJ_IPP_DEV                "ippDev"
#define OBJ_IPP_NAME               "ippName"
#endif   // SUPPORT_IPP

// parameter for TOD
// set operation
#ifdef SUPPORT_TOD
#define OBJ_TOD_INIT               0
#define OBJ_TOD_ADD                1
#define OBJ_TOD_STORE              2
#endif   // SUPPORT_TOD

// parameter for PR
// set operation
#ifdef SUPPORT_PR
#define OBJ_PR_INIT               0
#define OBJ_PR_ADD                1
#define OBJ_PR_STORE              2
#endif   // SUPPORT_PR

#endif /* __OBJECT_DEFS_H__ */

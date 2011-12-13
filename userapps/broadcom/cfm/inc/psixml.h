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
//  Filename:       psixml.h
//  Author:         yt
//  Creation Date:  05/11/04
//
******************************************************************************
//  Description:
//      Define the global types, enums. and constants for PSI in XML format
//
*****************************************************************************/

#ifndef __PSIXML_H__
#define __PSIXML_H__

#include "bcmtypes.h"
#include "psidefs.h"
#include "objectdefs.h"
#include "wldefs.h"
#include "nanoxml.h"

#define PSX_NAME_LEN                   24
#define PSX_MAGIC_LEN                  16
#define PSX_MAGIC_VALUE                "-$-PSI-$02.12r01"
#define PSX_CRC32_INIT_VALUE 0xffffffff /* Initial CRC32 checksum value */
#define PSX_CRC32_GOOD_VALUE 0xdebb20e3 /* Good final CRC32 checksum value */
#define PSX_STR_LEN                    50 
#define PSX_VERIFY                     1
#define PSX_STORE                      0
#define PSX_TABLE_TYPE                 2
#define PSX_APP_TYPE                   1
#define PSX_OBJ_TYPE                   0

#define MONDAY        0x1
#define TUESDAY       0x2
#define WEDNSDAY      0x4
#define THURSDAY      0x8
#define FRIDAY        0x10
#define SATURDAY      0x20
#define SUNDAY        0x40

#define XML_PSI_START_TAG      "<psitree>"
#define XML_PSI_END_TAG        "</psitree>"

#define XML_PORT_RANGE_MIN     1
#define XML_PORT_RANGE_MAX     65535
#define XML_MAX_SNMP_STR_LEN   64
#define XML_MAX_TOD_ENTRY      16
#define XML_MAX_TOD_URL_ENTRY	100
#define XML_MAX_PR_ENTRY	8

/* For Dhcpd static IP lease */
#define XML_MAX_STATIC_IP_LEASE      32

/* string  */
#define PSX_ATTRNAME_SET       1
#define PSX_ATTRSTR_SET        2

/* object and attribute names definitions */
#define PSX_ATTR_STATE             "state"

typedef enum {
   PSX_STS_OK = 0,
   PSX_STS_ERR_GENERAL,
   PSX_STS_ERR_MEMORY,
   PSX_STS_ERR_FIND_APP,
   PSX_STS_ERR_FIND_OBJ,
   PSX_STS_ERR_FIND_HDL_FNC,
   PSX_STS_ERR_FATAL,
   PSX_STS_ERR_MILD
} PSX_STATUS;

/* internal callback statemachine */
typedef enum {
   state_start,
   state_psi_begin,  
   state_psi_end
} xml_state;


typedef struct xmlParseState {
   xml_state state;
   char appName[32];
   PSI_HANDLE appHandle;
   char objName[32];
   char attrName[32];
   char tableName[32];
   int tableSize;
   int tableIndex;
   PSX_STATUS errorFlag;
   int verify;                    /* verify=1 verifcation only, verfify=0 conversion only */
} XML_PARSE_STATE, *pXML_PARSE_STATE;

typedef PSX_STATUS (*PSX_SET_OBJ_FNC) (FILE *file, const char *appName, const UINT16 id,
                                       const PSI_VALUE value, const UINT16 length);
typedef PSX_STATUS (*PSX_GET_OBJ_FNC) (char *appName, char *objName, char *attrName,
				       char* attrValue);
typedef void (*PSX_SET_ATTR_FNC) (FILE *file, const char *name, const UINT16 id,
				  const PSI_VALUE value, const UINT16 length);

typedef PSX_STATUS (*PSX_GET_ATTR_FNC) (char *attrName, char *attrValue);
typedef PSX_STATUS (*PSX_OBJ_FNC) (char *appName,  char *objName);



typedef struct {
   char  appName[PSX_NAME_LEN];
   PSX_SET_OBJ_FNC setObjFnc;
   PSX_GET_OBJ_FNC getObjFnc;     /* base on appName, find the object's getObjFnc */
   PSX_OBJ_FNC startObjFnc;       /* base on appName, find the object's startObjFnc */
   PSX_OBJ_FNC endObjFnc;         /* base on appName, find the object's endObjFnc */
} PSX_APP_ITEM, *PPSX_APP_ITEM;

typedef struct {
   char  objName[PSX_NAME_LEN];
   PSX_SET_ATTR_FNC setAttrFnc;
   PSX_GET_ATTR_FNC getAttrFnc;  /* getting object from XML file, do verification/store */
   PSX_OBJ_FNC startObjFnc;     /* beginning of object, resource allocated */
   PSX_OBJ_FNC endObjFnc;       /* end of object, do verification/storeToPSI, oresource deallocated */
} PSX_OBJ_ITEM, *PPSX_OBJ_ITEM;

#if defined(__cplusplus)
extern "C" {
#endif
extern PSX_STATUS xmlSetObjNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetSysObjNode(FILE* file, const char *appName, const UINT16 id,
			    const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetRouteObjNode(FILE* file, const char *appName, const UINT16 id,
			    const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetPMapObjNode(FILE* file, const char *appName, const UINT16 id,
			    const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetLanNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetWirelessNode(FILE* file, const char *appName, const UINT16 id,
			      const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetWanNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetAtmTdNode(FILE* file, const char *appName, const UINT16 id,
			   const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetAtmVccNode(FILE* file, const char *appName, const UINT16 id,
			    const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetAtmCfgNode(FILE* file, const char *appName, const UINT16 id,
			    const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetSecObjNode(FILE* file, const char *appName, const UINT16 id,
			    const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetAdslObjNode(FILE* file, const char *appName, const UINT16 id,
			     const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetSntpNode(FILE* file, const char *appName, const UINT16 id,
			  const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetToDNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetPRNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
/************************
 * UrlFilter modification
 * *********************/
extern PSX_STATUS xmlSetToDUrlNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);

extern PSX_STATUS xmlSetDhcpClntNode(FILE* file, const char *appName, const UINT16 id,
			      const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetPppNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlSetDdnsNode(FILE* file, const char *appName, const UINT16 id,
			  const PSI_VALUE value, const UINT16 length);
/* For Dhcpd static IP lease */
extern PSX_STATUS xmlSetDhcpdNode(FILE* file, const char *appName, const UINT16 id,
			 const PSI_VALUE value, const UINT16 length);
extern void xmlSetDhcpdCntrlAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
extern PSX_STATUS xmlGetDhcpdObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDhcpdObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndDhcpdObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetDhcpdAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndDhcpdObj(char *appName, char *objName);
extern PSX_STATUS xmlGetDhcpdEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndDhcpdEntryObj(char *appName, char *objName);


extern void xmlSetTr69cAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
	       	 const UINT16 length);
#if defined(SUPPORT_VDSL)
extern void xmlSet8021agAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		     const UINT16 length);
#endif
extern void xmlSetMgmtLockAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
	       	 const UINT16 length);
extern void xmlSetStrObjAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
                      const UINT16 length);
extern void xmlSetSysLogAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern void xmlSetDftGwAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		     const UINT16 length);
#ifdef SUPPORT_IPV6
extern void xmlSetDftGw6Attr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		     const UINT16 length);
extern void xmlSetDhcp6InfoAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		     const UINT16 length);
#endif
extern void xmlSetDnsAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);
extern void xmlSetOtherSysAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
extern void xmlSetSnmpAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		    const UINT16 length);
extern void xmlSetRouteAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			   const UINT16 length);
extern void xmlSetRipIfcAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern void xmlSetRipAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);
extern void xmlSetPMapAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);
extern void xmlSetPMapFltAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);
extern void xmlSetLanAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);
extern void xmlWlSetWebAttr(FILE *file, PWIRELESS_VAR wlVars);
extern void xmlWlSetEncrytionAttr(FILE *file, PWIRELESS_VAR wlVars);
extern void xmlWlSetEncryptionStrengthAttr(FILE *file, PWIRELESS_VAR wlVars);
extern void xmlWlSetRadiusServerAttr(FILE *file, PWIRELESS_VAR wlVars);
extern void xmlSetWirelessVarsAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			   const UINT16 length);
extern void xmlSetWirelessMacFilterAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
				const UINT16 length);
extern void xmlSetAtmInitAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		       const UINT16 length);
extern void xmlSetAtmPortAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		       const UINT16 length);
extern void xmlSetAtmTdNumAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
extern void xmlSetAtmTdTblAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
extern void xmlSetAtmIfcAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern void xmlSetAtmVccNumAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			 const UINT16 length);
extern void xmlSetAtmVccTblAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
extern void xmlSetDmzHostAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		       const UINT16 length);
extern void xmlSetServiceCtrlAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			   const UINT16 length);
extern void xmlSetVirtualSrvAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern void xmlSetIpFilterAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern void xmlSetPrtTriggerAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			  const UINT16 length);
extern void xmlSetMacFilterAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			 const UINT16 length);
extern void xmlSetAccessControlAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			     const UINT16 length);
extern void xmlSetAccessControlModeAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
				 const UINT16 length);
extern void xmlSetQosMgmtCfgAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
				 const UINT16 length);
extern void xmlSetQosAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);
extern void xmlSetToDCntrlAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
extern void xmlSetPRCntrlAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);
/*********************
 * UrlFilter modification
 * *******************/
extern void xmlSetToDUrlCntrlAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
			const UINT16 length);

extern void xmlSetDdnsAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		    const UINT16 length);
extern void xmlSetEthAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		   const UINT16 length);

extern PSX_STATUS xmlParse(char *xmlBuffer, int xmlSize, int verify);
extern void xmlTagBegin(nxml_t handle, const char *tag_name, unsigned len);
extern void xmlTagEnd(nxml_t handle, const char *tag_name, unsigned len);
extern void xmlAttrBegin(nxml_t handle, const char *tag_name, unsigned len);
extern void xmlAttrValue(nxml_t handle, const char *tag_name, unsigned len);
extern void xmlData(nxml_t handle, const char *tag_name, unsigned len);
extern PSX_STATUS xmlGetObjNode(char *appName, char *objTagName,
	         		char *attrName, char *value);
extern PSX_STATUS xmlVerifyObjNode(char *appName, char *objTagName,
			    char *attrName, char *value);
extern PSX_STATUS xmlStartObjNode(char *appName, char *objTagName);
extern PSX_STATUS xmlEndObjNode(char *appName, char *objTagName);

extern PSX_STATUS xmlGetSysObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSysObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndSysObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetSysUserNameAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlGetSysPasswordAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSysUserNamePasswordObj(char *appName, char *objName);
extern PSX_STATUS xmlEndSysUserNamePasswordObj(char *appName, char *objName);
extern PSX_STATUS xmlGetTr69cAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartTr69cObj(char *appName, char *objName);
extern PSX_STATUS xmlEndTr69cObj(char *appName, char *objName);
#if defined(SUPPORT_VDSL)
extern PSX_STATUS xmlGet8021agAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStart8021agObj(char *appName, char *objName);
extern PSX_STATUS xmlEnd8021agObj(char *appName, char *objName);
#endif
extern PSX_STATUS xmlGetMgmtLockAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartMgmtLockObj(char *appName, char *objName);
extern PSX_STATUS xmlEndMgmtLockObj(char *appName, char *objName);
extern PSX_STATUS xmlGetDftGwAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDftGwObj(char *appName, char *objName);
extern PSX_STATUS xmlEndDftGwObj(char *appName, char *objName);
#ifdef SUPPORT_IPV6
extern PSX_STATUS xmlGetDftGw6Attr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDftGw6Obj(char *appName, char *objName);
extern PSX_STATUS xmlEndDftGw6Obj(char *appName, char *objName);
extern PSX_STATUS xmlGetDhcp6InfoAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDhcp6InfoObj(char *appName, char *objName);
extern PSX_STATUS xmlEndDhcp6InfoObj(char *appName, char *objName);
#endif
extern PSX_STATUS xmlGetDnsAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDnsObj(char *appName, char *objName);
extern PSX_STATUS xmlEndDnsObj(char *appName, char *objName);
extern PSX_STATUS xmlGetSyslogAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSyslogObj(char *appName, char *objName);
extern PSX_STATUS xmlEndSyslogObj(char *appName, char *objName);
extern PSX_STATUS xmlGetSnmpAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSnmpObj(char *appName, char *objName);
extern PSX_STATUS xmlEndSnmpObj(char *appName, char *objName);
extern PSX_STATUS xmlGetOtherSysAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartOtherSysObj(char *appName, char *objName);
extern PSX_STATUS xmlEndOtherSysObj(char *appName, char *objName);
extern PSX_STATUS xmlGetAtmVccObjNode(char *appName, char *objName, char *attrName,
                                      char* attrValue);
extern PSX_STATUS xmlGetAtmVccAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartAtmVccObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndAtmVccObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetWanObjNode(char *appName, char *objName, char *attrName,
                                   char* attrValue);
extern PSX_STATUS xmlGetWanAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartWanObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndWanObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetPppObjNode(char *appName, char *objName, char *attrName,
                                   char* attrValue);
extern PSX_STATUS xmlGetPppAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartPppObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndPppObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetSecObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSecObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndSecObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetVrtSrvAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndVrtSrvObj(char *appName, char *objName);
extern PSX_STATUS xmlGetVrtSrvEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndVrtSrvEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetIpFilterAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndIpFilterObj(char *appName, char *objName);
extern PSX_STATUS xmlGetIpFilterEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndIpFilterEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetPrtTriggerAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndPrtTriggerObj(char *appName, char *objName);
extern PSX_STATUS xmlGetPrtTriggerEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndPrtTriggerEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetMacFilterAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndMacFilterObj(char *appName, char *objName);
extern PSX_STATUS xmlGetMacFilterEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndMacFilterEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetQosClsAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartQosClsEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlEndQosClsObj(char *appName, char *objName);
extern PSX_STATUS xmlGetQosClsEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndQosClsEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetQosQueueAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartQosQueueEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlEndQosQueueObj(char *appName, char *objName);
extern PSX_STATUS xmlGetQosQueueEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndQosQueueEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetQosMgmtCfgAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartQosMgmtCfgObj(char *appName, char *objName);
extern PSX_STATUS xmlEndQosMgmtCfgObj(char *appName, char *objName);
extern PSX_STATUS xmlGetAccCntrlAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndAccCntrlObj(char *appName, char *objName);
extern PSX_STATUS xmlGetAccCntrlEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndAccCntrlEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetAccCntrlModeAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartAccCntrlModeObj(char *appName, char *objName);
extern PSX_STATUS xmlEndAccCntrlModeObj(char *appName, char *objName);
extern PSX_STATUS xmlGetDmzHostAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDmzHostObj(char *appName, char *objName);
extern PSX_STATUS xmlEndDmzHostObj(char *appName, char *objName);
extern PSX_STATUS xmlGetServiceCtrlAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartServiceCtrlObj(char *appName, char *objName);
extern PSX_STATUS xmlEndServiceCtrlObj(char *appName, char *objName);
extern PSX_STATUS xmlGetDhcpClntNode(char *appName, char *objName, char *attrName,
                                   char* attrValue);
extern PSX_STATUS xmlGetDhcpClntAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDhcpClntNode(char *appName, char *objName);
extern PSX_STATUS xmlEndDhcpClntNode(char *appName, char *objName);
extern PSX_STATUS xmlGetAtmTdObjNode(char *appName, char *objName, char *attrName,
                                   char* attrValue);
extern PSX_STATUS xmlGetAtmTdAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartAtmTdObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndAtmTdObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetLanNode(char *appName, char *objName, char *attrName,
                                char* attrValue);
extern PSX_STATUS xmlGetLanAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartLanObj(char *appName, char *objName);
extern PSX_STATUS xmlEndLanObj(char *appName, char *objName);
extern PSX_STATUS xmlGetWirelessObjNode(char *appName, char *objName, char *attrName,
                                        char* attrValue);
extern PSX_STATUS xmlStartWirelessObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndWirelessObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetWirelessVarsAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartWirelessVarsObj(char *appName, char *objName);
extern PSX_STATUS xmlEndWirelessVarsObj(char *appName, char *objName);
extern PSX_STATUS xmlGetWirelessMacFilterEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndWirelessMacFilterEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetWirelessMacFilterAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndWirelessMacFilterObj(char *appName, char *objName);
extern void xmlSetWirelessMssidVarsAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlGetWirelessMssidVarsAttr(char * attrName, char * attrValue);
extern PSX_STATUS xmlEndWirelessMssidVarsObj(char *appName, char *objName);
extern PSX_STATUS xmlGetWirelessMssidVarsEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndWirelessMssidVarsEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetRouteObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartRouteObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndRouteObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetRouteAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndRouteObj(char *appName, char *objName);
extern PSX_STATUS xmlStartRouteEntryObj(char *appName, char *objName);
extern void xmlSetRouteEntryAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern PSX_STATUS xmlGetRouteEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndRouteEntryObj(char *appName, char *objName);
#ifdef SUPPORT_IPV6
extern PSX_STATUS xmlGetRoute6Attr(char *attrName, char* attrValue);
extern void xmlSetRoute6EntryAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
		      const UINT16 length);
extern PSX_STATUS xmlGetRoute6EntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndRoute6EntryObj(char *appName, char *objName);
#endif
extern PSX_STATUS xmlGetRipAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartRipObj(char *appName, char *objName);
extern PSX_STATUS xmlEndRipObj(char *appName, char *objName);
extern PSX_STATUS xmlGetRipIfcAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndRipIfcObj(char *appName, char *objName);
extern PSX_STATUS xmlGetRipIfcEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndRipIfcEntryObj(char *appName, char *objName);

extern PSX_STATUS xmlGetPMapObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartPMapObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndPMapObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetPMapAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlGetPMapFltAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndPMapObj(char *appName, char *objName);
extern PSX_STATUS xmlEndPMapFltObj(char *appName, char *objName);
extern PSX_STATUS xmlGetPMapEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlGetPMapFltEntryAttr(char *attrName, char* attrValue);
extern void xmlSetPMapIfcCfgAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
                   const UINT16 length);
extern PSX_STATUS xmlGetPMapIfcCfgAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartPMapIfcCfgObj(char *appName, char *objName);
extern PSX_STATUS xmlEndPMapIfcCfgObj(char *appName, char *objName);
extern PSX_STATUS xmlEndPMapEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlEndPMapFltEntryObj(char *appName, char *objName);

extern PSX_STATUS xmlGetDdnsObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartDdnsObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndDdnsObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetDdnsAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndDdnsObj(char *appName, char *objName);
extern PSX_STATUS xmlGetDdnsEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndDdnsEntryObj(char *appName, char *objName);
extern PSX_STATUS xmlGetAtmCfgNode(char *appName, char *objName, char *attrName,
                                   char* attrValue);
extern PSX_STATUS xmlStartAtmCfgNode(char *appName, char *objName);
extern PSX_STATUS xmlEndAtmCfgNode(char *appName, char *objName);
extern PSX_STATUS xmlGetAtmInitAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartAtmInitObj(char *appName, char *objName);
extern PSX_STATUS xmlEndAtmInitObj(char *appName, char *objName);

extern PSX_STATUS xmlGetAdslNode(char *appName, char *objName, char *attrName,
                                 char* attrValue);
extern PSX_STATUS xmlGetAdslAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartAdslObj(char *appName, char *objName);
extern PSX_STATUS xmlEndAdslObj(char *appName, char *objName);

extern PSX_STATUS xmlGetSntpNode(char *appName, char *objName, char *attrName,
                                 char* attrValue);
extern PSX_STATUS xmlGetSntpAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSntpObj(char *appName, char *objName);
extern PSX_STATUS xmlEndSntpObj(char *appName, char *objName);
extern PSX_STATUS xmlGetToDObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartToDObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndToDObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetToDAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndToDObj(char *appName, char *objName);
extern PSX_STATUS xmlGetToDEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndToDEntryObj(char *appName, char *objName);

extern PSX_STATUS xmlGetPRObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartPRObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndPRObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetPRAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndPRObj(char *appName, char *objName);
extern PSX_STATUS xmlGetPREntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndPREntryObj(char *appName, char *objName);
/*********************
 * UrlFilter modification
 * *******************/
extern PSX_STATUS xmlGetToDUrlObjNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartToDUrlObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndToDUrlObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetToDUrlAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndToDUrlObj(char *appName, char *objName);
extern PSX_STATUS xmlGetToDUrlEntryAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlEndToDUrlEntryObj(char *appName, char *objName);

extern void xmlSetToDUrlMgtAttr(FILE *file, const char *name, const UINT16 id, 
			const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlGetToDUrlMgtAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartToDUrlMgtObj(char *appName, char *objName);
extern PSX_STATUS xmlEndToDUrlMgtObj(char *appName, char *objName);

extern PSX_STATUS xmlSetVoiceNode(FILE* file, const char *appName, const UINT16 id,
                                  const PSI_VALUE value, const UINT16 length);

extern PSX_STATUS xmlGetVoiceNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartVoiceNode(char *appName, char *objName);
extern PSX_STATUS xmlEndVoiceNode(char *appName, char *objName);
extern void  xmlSetMgcpAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value,
                            const UINT16 length);
extern void  xmlSetSipAttr(FILE* file, const char *appName, const UINT16 id,
                           const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlStartMgcpObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndMgcpObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetMgcpAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartSipObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndSipObjNode(char *appName, char *objName);
extern PSX_STATUS xmlGetSipAttr(char *attrName, char* attrValue);

extern PSX_STATUS xmlGetEthAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlSetIppNode(FILE* file, const char *appName, const UINT16 id,
                                const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlGetIppNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlGetIppAttr(char *attrName, char* attrValue);
extern PSX_STATUS xmlStartIppObj(char *appName, char *objName);
extern PSX_STATUS xmlEndIppObj(char *appName, char *objName);
extern PSX_STATUS xmlSetIPSecNode(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlGetIPSecNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartIPSecNode(char *appName, char *objName);
extern PSX_STATUS xmlEndIPSecNode(char *appName, char *objName);
extern PSX_STATUS xmlSetCertNode(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlGetCertNode(char *appName, char *objName, char *attrName, char* attrValue);
extern PSX_STATUS xmlStartCertNode(char *appName, char *objName);
extern PSX_STATUS xmlEndCertNode(char *appName, char *objName);


extern char *xmlWirelessGetPhyType(void);
extern int xmlWirelessGetCoreRev(void);
extern int xmlWirelessGetBands(void);
extern int xmlWirelessGetValidBand(int band);


extern PSX_STATUS xmlGetPortMirrorCfgAttr(char *attrName, char* attrValue) ;
extern PSX_STATUS xmlEndPortMirrorCfgObj(char *appName, char *objName) ;
extern void xmlSetPortMirrorStatusAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value, const UINT16 length) ;
extern PSX_STATUS xmlGetPortMirrorStatusAttr(char *attrName, char* attrValue) ;
extern PSX_STATUS xmlStartPortMirrorStatusAttr(char *appName, char *objName) ;
extern PSX_STATUS xmlEndPortMirrorStatusAttr(char *appName, char *objName) ;
extern void xmlSetPortMirrorCfgEntryAttr(FILE *file, const char *name, const UINT16 id, const PSI_VALUE value, const UINT16 length) ;
extern PSX_STATUS xmlGetPortMirrorCfgEntryAttr(char *attrName, char* attrValue) ;
extern PSX_STATUS xmlEndPortMirrorCfgEntryAttrObj(char *appName, char *objName) ;
extern PSX_STATUS xmlSetPortMirrorObjNode (FILE* file, const char *appName, const UINT16 id, const PSI_VALUE value, const UINT16 length) ;
extern PSX_STATUS xmlGetPortMirrorObjNode(char *appName, char *objName, char *attrName, char* attrValue) ;
extern PSX_STATUS xmlStartPortMirrorObjNode(char *appName, char *objName) ;
extern PSX_STATUS xmlEndPortMirrorObjNode(char *appName, char *objName) ;

extern int base64Encode(const void *inp, int insize, char **outptr);
extern void base64Decode(unsigned char *dest, char *src, int *rawLength);
//zcy for pptp
extern PSX_STATUS xmlSetGlobalNode(FILE* file, const char *appName, const UINT16 id,
			  const PSI_VALUE value, const UINT16 length);
extern PSX_STATUS xmlGetGlobalNode(char *appName, char *objName, char *attrName,
             char* attrValue);  
extern PSX_STATUS xmlStartGlobalObjNode(char *appName, char *objName);
extern PSX_STATUS xmlEndGlobalObjNode(char *appName, char *objName);
//zcy for pptp end
#if defined(__cplusplus)
}
#endif

#endif /* __PSIXML_H__ */

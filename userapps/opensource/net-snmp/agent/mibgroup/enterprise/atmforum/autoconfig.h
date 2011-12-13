/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile:$
 * $brcm_Revision:$
 * $brcm_Date:$
 *
 * [File Description:] 
 *
 * Revision History:
 *
 * $brcm_Log:$
 *
 ***************************************************************************/

#ifndef ATMF_AUTOCONFIG_H
#define ATMF_AUTOCONFIG_H

/* TESTING ONLY */
#define AUTOCONFIG_TEST_EXPECT_ATTACH_POINT  0
#define AUTOCONFIG_TEST_EXPECT_CONFIG        1
#define AUTOCONFIG_TEST_EXPECT_SERVICE_CONN1 2
#define AUTOCONFIG_TEST_EXPECT_SERVICE_CONN2 3
#define AUTOCONFIG_TEST_EXPECT_PROFILE       4
#define AUTOCONFIG_TEST_EXPECT_SERVICE_TYPE  5
#define AUTOCONFIG_TEST_EXPECT_VCC_TABLE     6
/* TESTING ONLY */

#define AUTOCONFIG_MAX_ILMI_MSG_LEN        484
#define ATMF_DEF_ILMI_VPI                  0
#define ATMF_DEF_ILMI_VCI                  16
#define AUTOCONFIG_MESSAGE_TIMEOUT         5*1000000  /* in terms of uSec = 5 seconds */
#define AUTOCONIFG_DISPLAY_STRING_MAX      32
#define AUTOCONIFG_OCTETS_STRING_MAX       16
#define AUTOCONFIG_SYS_ID_LEN              6
#define ATMF_AAL5_PROFILE                  1
#define AUTOCONFIG_ERROR                   -1
#define AUTOCONFIG_SUCCESS                 0
#define AUTOCONFIG_ENTERPRISE_OID    1, 3, 6, 1, 4, 1, 3, 1, 1
#define AUTOCONFIG_ATMFORUM_ENTERPRISE_OID    1, 3, 6, 1, 4, 1, 353
#define AUTOCONFIG_SYSDESCR_OID      1, 3, 6, 1, 2, 1, 1, 1
#define AUTOCONFIG_SNMPTRAP_OID      1, 3, 6, 1, 6, 3, 1, 1, 4, 1
#define AUTOCONFIG_COLDSTART_OID     1, 3, 6, 1, 6, 3, 1, 1, 5, 1

#define AUTOCONFIG_SYSUPTIME_OID     1,3,6,1,2,1,1,3
#define AUTOCONFIG_ATMF_PORT_ENTRY_OID  1,3,6,1,4,1,353,2,1,1,1
#define AUTOCONFIG_ATMF_PORT_MYID_OID   AUTOCONFIG_ATMF_PORT_ENTRY_OID,8
#define AUTOCONFIG_ATMF_MYSYSID_OID     1,3,6,1,4,1,353,2,1,4

#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_OID  1, 3, 6, 1, 4, 1, 353, 2, 2, 1, 1
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_SIZE 15
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_INDEX            1
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VPCS         2
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VCCS         3
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_CONFIGURED_VPCS  4
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_CONFIGURED_VCCS  5
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VPI_BITS     6
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VCI_BITS     7
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_UNI_TYPE         8
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_UNI_VERSION      9
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_DEVICE_TYPE      10
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_ILMI_VERSION     11
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_NNI_SIG_VERSION  12
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_SVPC_VPI     13
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_SVCC_VPI     14
#define AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_SVCC_VCI     15

#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_INDEX    1
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_VPI            2
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_VCI            3
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_SIGNAL_ID      4
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_SERVICE_INDEX  5
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_SERVICE_NAME   6
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_AAL_TYPE       7
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_AAL_INDEX      8

#define AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_INDEX        1
#define AUTOCONFIG_ATMF_AAL5_PROFILE_SDU_SIZE_FORWARD   2
#define AUTOCONFIG_ATMF_AAL5_PROFILE_SDU_SIZE_BACKWARD  3
#define AUTOCONFIG_ATMF_AAL5_PROFILE_AAL_MODE           4
#define AUTOCONFIG_ATMF_AAL5_PROFILE_SSCS_TYPE          5

#define AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_INDEX        1
#define AUTOCONFIG_ATMF_SERVICE_TYPE_PROVIDER_NAME      2
#define AUTOCONFIG_ATMF_SERVICE_TYPE_SERVICE_NAME       3
#define AUTOCONFIG_ATMF_SERVICE_TYPE_SERVICE_SUBNAME    4
#define AUTOCONFIG_ATMF_SERVICE_TYPE_CLIENT             5
#define AUTOCONFIG_ATMF_SERVICE_TYPE_TM_CATEGORY        6
#define AUTOCONFIG_ATMF_SERVICE_TYPE_TM_CONFORMANCEDEF  7
#define AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_LAYER2_ID      1
#define AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_LAYER3_ID      2

#define AUTOCONFIG_ATMF_VCC_PORT_INDEX                  1
#define AUTOCONFIG_ATMF_VCC_VPI                         2
#define AUTOCONFIG_ATMF_VCC_VCI                         3
#define AUTOCONFIG_ATMF_VCC_OPER_STATUS                 4
#define AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_DESCRIPTOR_TYPE  5
#define AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM1            6
#define AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM2            7
#define AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM3            8
#define AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM4            9
#define AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM5            10
#define AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_DESCRIPTOR_TYPE  11
#define AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM1            12
#define AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM2            13
#define AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM3            14
#define AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM4            15
#define AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM5            16
#define AUTOCONFIG_ATMF_VCC_QOS_CATEGORY                17
#define AUTOCONFIG_ATMF_VCC_TX_QOS_CLASS                18
#define AUTOCONFIG_ATMF_VCC_RX_QOS_CLASS                19
#define AUTOCONFIG_ATMF_VCC_BEST_EFFORT_INDICATOR       20
#define AUTOCONFIG_ATMF_VCC_TX_FRAME_DISCARD            21
#define AUTOCONFIG_ATMF_VCC_RX_FRAME_DISCARD            22
#define AUTOCONFIG_ATMF_VCC_SERVICE_CATEGORY            23

/* LAYER 2 and LAYER 3 */
/* 1. layer 3=llc-snap, layer 2 = llc; first 6 bytes of aal5 pdu has snap header
   2. layer 3= snap, layer 2 = null; null encapsulation.
   3. layer 3= eth, ethertype=0x800, ARP-0x0806, pppoe=0x8864 or 0x8863
   4.  */
   
#define AUTOCONFIG_LAYER2_LLC_ENCAP                  "0C"
#define AUTOCONFIG_LAYER2_NULL_ENCAP                 "00"  /* also known as VC-mux */
#define AUTOCONFIG_LAYER3_PPPOA                      "0BCF"
#define AUTOCONFIG_LAYER3_IPOA_LLC_SNAP              "0B800000000800"
#define AUTOCONFIG_LAYER3_IPOA_NULL                  "0BCC"
#define AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_NO_FCS     "0B800080C20007"
#define AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_FCS        "0B800080C20001"
#define AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_NO_FCS_IP  "0B800080C200070800"
#define AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_FCS_IP     "0B800080C200010800"
#define AUTOCONFIG_LAYER3_PPPOE_LLC_SNAP_NO_FCS      "0B800080C200078864"
#define AUTOCONFIG_LAYER3_PPPOE_LLC_SNAP_FCS         "0B800080C200018864"


#define AUTOCONFIG_ATMF_VCC_ENTRY_OID        1, 3, 6, 1, 4, 1, 353, 2, 5, 1, 1
#define AUTOCONFIG_ATMF_VCC_ENTRY_ENTRY_SIZE 23

#define AUTOCONFIG_ATMF_CONFIG_GROUP_OID     1, 3, 6, 1, 4, 1, 353, 2, 12

#define AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_OID        AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 1, 1
#define AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_OID    AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 10, 1
#define AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_SIZE       7
#define AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_SIZE   2

#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_OID        AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 2, 1
#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_ENTRY_SIZE 8

#define AUTOCONFIG_ATMF_SERVICE_CONNINFO_EXT_ENTRY_OID    AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 9, 1

#define AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_OID        AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 5, 1
#define AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_SIZE       5

#define AUTOCONFIG_ATMF_VCC_CHANGE_TRAP    2
#define AUTOCONFIG_ATMF_PORT_INDEX_OID      AUTOCONFIG_ATMF_PORT_ENTRY_OID, 1
#define AUTOCONFIG_ATMF_PORT_INDEX_OID_LEN  12
#define AUTOCONFIG_ATMF_VCC_VPI_OID         AUTOCONFIG_ATMF_VCC_ENTRY_OID, 2
#define AUTOCONFIG_ATMF_VCC_VPI_OID_LEN     12
#define AUTOCONFIG_ATMF_VCC_VCI_OID         AUTOCONFIG_ATMF_VCC_ENTRY_OID, 3
#define AUTOCONFIG_ATMF_VCC_VCI_OID_LEN     12
#define AUTOCONFIG_ATMF_VCC_OPER_STATUS_OID AUTOCONFIG_ATMF_VCC_ENTRY_OID, 4
#define AUTOCONFIG_ATMF_VCC_OPER_STATUS_OID_LEN     12

/* Configuration Fail Trap reasons */
#define AUTOCONFIG_ATMF_CONFIG_FAIL_TRAP  3
#define AUTOCONFIG_ATMF_CONFIG_FAIL_UNSUPPORTED_CLIENT_PROTOCOL  1
#define AUTOCONFIG_ATMF_CONFIG_FAIL_UNSOPPORTED_SERVICE_CAT      2
#define AUTOCONFIG_ATMF_CONFIG_FAIL_UNSUPPORTED_AAL              3
#define AUTOCONFIG_ATMF_CONFIG_FAIL_INVALID_VPI_VCI              4
#define AUTOCONFIG_ATMF_CONFIG_FAIL_TOO_MANY_PVCS                5
#define AUTOCONFIG_ATMF_CONFIG_FAIL_INCOMPLETE_INFO              6
#define AUTOCONFIG_ATMF_CONFIG_FAIL_INCONSISTENT_INFO            7
#define AUTOCONFIG_ATMF_CONFIG_FAIL_TABLE_VAR                    1
#define AUTOCONFIG_ATMF_CONFIG_FAIL_ENTRY_VAR                    2

#define AUTOCONFIG_ATTACHPOINT       1
#define AUTOCONFIG_CONFIG            2
#define AUTOCONFIG_SERVICE_CONNINFO  3
#define AUTOCONFIG_SERVICE_TYPE      4
#define AUTOCONFIG_AAL5_PROFILE      5
#define AUTOCONFIG_VCC_TABLE         6

/* atomic set flags */
#define AUTOCONFIG_ATTACHPOINT_INFO_MY_SYS_ID_SET   0x1
#define AUTOCONFIG_ATTACHPOINT_INFO_MY_IF_ID_SET    0x2
#define AUTOCONFIG_ATTACHPOINT_INFO_SYS_UPTIME_SET  0x4
#define AUTOCONFIG_ATTACHPOINT_INFO_ALL_SET         0x7

#define AUTOCONFIG_CONFIG_INFO_INDEX_SET            0x1
#define AUTOCONFIG_CONFIG_INFO_MAX_VPCS_SET         0x2
#define AUTOCONFIG_CONFIG_INFO_MAX_VCCS_SET         0x4
#define AUTOCONFIG_CONFIG_INFO_CONFIGURED_VPCS_SET  0x8
#define AUTOCONFIG_CONFIG_INFO_CONFIGURED_VCCS_SET  0x10
#define AUTOCONFIG_CONFIG_INFO_MAX_VPI_BITS_SET     0x20
#define AUTOCONFIG_CONFIG_INFO_MAX_VCI_BITS_SET     0x40
#define AUTOCONFIG_CONFIG_INFO_UNI_TYPE_SET         0x80
#define AUTOCONFIG_CONFIG_INFO_UNI_VERSION_SET      0x100
#define AUTOCONFIG_CONFIG_INFO_DEVICE_TYPE_SET      0x200
#define AUTOCONFIG_CONFIG_INFO_ILMI_VERSION_SET     0x400
#define AUTOCONFIG_CONFIG_INFO_NNI_SIG_VERSION_SET  0x800
#define AUTOCONFIG_CONFIG_INFO_MAX_SVPC_VPI_SET     0x1000
#define AUTOCONFIG_CONFIG_INFO_MAX_SVCC_VPI_SET     0x2000
#define AUTOCONFIG_CONFIG_INFO_MAX_SVCC_VCI_SET     0x4000
#define AUTOCONFIG_CONFIG_INFO_ALL_SET              0x7fff

#define AUTOCONFIG_SERVICE_CONNINFO_PORT_INDEX_SET         0x1
#define AUTOCONFIG_SERVICE_CONNINFO_VPI_SET                0x2
#define AUTOCONFIG_SERVICE_CONNINFO_VCI_SET                0x4
#define AUTOCONFIG_SERVICE_CONNINFO_SIGNAL_ID_SET          0x8
#define AUTOCONFIG_SERVICE_CONNINFO_SERVICE_INDEX_SET      0x10
#define AUTOCONFIG_SERVICE_CONNINFO_SERVICE_CONN_NAME_SET  0x20
#define AUTOCONFIG_SERVICE_CONNINFO_AAL_TYPE_SET           0x40
#define AUTOCONFIG_SERVICE_CONNINFO_AAL_INDEX_SET          0x80
#define AUTOCONFIG_SERVICE_CONNINFO_ALL_SET                0xff

#define AUTOCONFIG_AAL5_PROFILE_INDEX_SET               0x1
#define AUTOCONFIG_AAL5_MAX_SDU_SIZE_FORWARD_SET        0x2
#define AUTOCONFIG_AAL5_MAX_SDU_SIZE_BACKWARD_SET       0x4
#define AUTOCONFIG_AAL5_AAL_MODE_SET                    0x8
#define AUTOCONFIG_AAL5_SSCS_TYPE_SET                   0x10
#define AUTOCONFIG_AAL5_PROFILE_ALL_SET                 0x1f

#define AUTOCONFIG_SERVICE_TYPE_INDEX_SET               0x1
#define AUTOCONFIG_SERVICE_TYPE_PROVIDER_NAME_SET       0x2
#define AUTOCONFIG_SERVICE_TYPE_NAME_SET                0x4
#define AUTOCONFIG_SERVICE_TYPE_SUB_NAME_SET            0x8
#define AUTOCONFIG_SERVICE_TYPE_CLIENT_SET              0x10
#define AUTOCONFIG_SERVICE_TYPE_TM_CATEGORY_SET         0x20
#define AUTOCONFIG_SERVICE_TYPE_TM_CONFORM_DEF_SET      0x40
#define AUTOCONFIG_SERVICE_TYPE_EXT_LAYER2_SET          0x80
#define AUTOCONFIG_SERVICE_TYPE_EXT_LAYER3_SET          0x100
#define AUTOCONFIG_SERVICE_TYPE_ALL_SET                 0x1ff

#define AUTOCONFIG_VCC_PORT_INDEX_SET                     0x1
#define AUTOCONFIG_VCC_VPI_SET                            0x2
#define AUTOCONFIG_VCC_VCI_SET                            0x4
#define AUTOCONFIG_VCC_OPER_STATUS_SET                    0x8
#define AUTOCONFIG_VCC_TX_TRAFFIC_DESCRIPTOR_TYPE_SET     0x10
#define AUTOCONFIG_VCC_TX_TRAFFIC_PARM1_SET               0x20
#define AUTOCONFIG_VCC_TX_TRAFFIC_PARM2_SET               0x40
#define AUTOCONFIG_VCC_TX_TRAFFIC_PARM3_SET               0x80
#define AUTOCONFIG_VCC_TX_TRAFFIC_PARM4_SET               0x100
#define AUTOCONFIG_VCC_TX_TRAFFIC_PARM5_SET               0x200
#define AUTOCONFIG_VCC_RX_TRAFFIC_DESCRIPTOR_TYPE_SET     0x400
#define AUTOCONFIG_VCC_RX_TRAFFIC_PARM1_SET               0x800
#define AUTOCONFIG_VCC_RX_TRAFFIC_PARM2_SET               0x1000
#define AUTOCONFIG_VCC_RX_TRAFFIC_PARM3_SET               0x2000
#define AUTOCONFIG_VCC_RX_TRAFFIC_PARM4_SET               0x4000
#define AUTOCONFIG_VCC_RX_TRAFFIC_PARM5_SET               0x8000
#define AUTOCONFIG_VCC_QOS_CATEGORY_SET                   0x10000
#define AUTOCONFIG_VCC_TX_QOS_CLASS_SET                   0x20000
#define AUTOCONFIG_VCC_RX_QOS_CLASS_SET                   0x40000
#define AUTOCONFIG_VCC_BEST_EFFORT_INDICATOR_SET          0x80000
#define AUTOCONFIG_VCC_TX_FRAME_DISCARD_SET               0x100000
#define AUTOCONFIG_VCC_RX_FRAME_DISCARD_SET               0x200000
#define AUTOCONFIG_VCC_SERVICE_CATEGORY_SET               0x400000
#define AUTOCONFIG_VCC_OBSOLETE_OBJS  (AUTOCONFIG_VCC_TX_TRAFFIC_DESCRIPTOR_TYPE_SET | \
AUTOCONFIG_VCC_RX_TRAFFIC_DESCRIPTOR_TYPE_SET | \
AUTOCONFIG_VCC_QOS_CATEGORY_SET | \
AUTOCONFIG_VCC_TX_QOS_CLASS_SET | \
AUTOCONFIG_VCC_RX_QOS_CLASS_SET | \
AUTOCONFIG_VCC_BEST_EFFORT_INDICATOR_SET | \
AUTOCONFIG_VCC_SERVICE_CATEGORY_SET)

#define AUTOCONFIG_VCC_ALL_SET (0x7fffff & ~AUTOCONFIG_VCC_OBSOLETE_OBJS)

typedef enum autoconfigState { NO_CHANGE, RESTART, VERIFY, NET_PREFIX,
                               ADDRESS, POLL, IDLE } autoconfigState;

int autoconfig_init(char *pvcAddrStr);

typedef struct autoconfigAttachPointInfo {
  int atmfPortMyIfIdentifier;
  char atmfMySystemIdentifier[AUTOCONFIG_SYS_ID_LEN];
  int sysUptime;
} AUTOCONFIG_ATTACHPOINT_INFO, *pAUTOCONFIG_ATTACHPOINT_INFO;

/* config info correspond to atmfAtmLayerEntry */
typedef struct autoconfigConfigEntry {
  int atmfAtmLayerIndex;
  int atmfAtmLayerMaxVpcs;
  int atmfAtmLayerMaxVccs;
  int atmfAtmLayerConfiguredVpcs;
  int atmfAtmLayerConfiguredVccs;
  int atmfAtmLayerMaxVpibits;
  int atmfAtmLayerMaxVcibits;
  int atmfAtmLayerUniType;
  int atmfAtmLayerUniVersion;
  int atmfAtmLayerDeviceType;
  int atmfAtmLayerIlmiVersion;
  int atmfAtmLayerNniSigVersion;
  int atmfAtmLayerMaxSvpcVpi;
  int atmfAtmLayerMaxSvccVpi;
  int atmfAtmLayerMaxSvccVci;
} AUTOCONFIG_CONFIG_INFO,  *pAUTOCONFIG_CONFIG_INFO;

/* config info correspond to atmfVccTableEntry */
typedef struct autoconfigVccEntry {
  int atmfVccPortIndex;
  int atmfVccVpi;
  int atmfVccVci;
  int atmfVccOperStatus;
  int atmfVccTransmitTrafficDescriptorType;
  int atmfVccTransmitTrafficDescriptorParm1;
  int atmfVccTransmitTrafficDescriptorParm2;
  int atmfVccTransmitTrafficDescriptorParm3;
  int atmfVccTransmitTrafficDescriptorParm4;
  int atmfVccTransmitTrafficDescriptorParm5;
  int atmfVccReceiveTrafficDescriptorType;
  int atmfVccReceiveTrafficDescriptorParm1;
  int atmfVccReceiveTrafficDescriptorParm2;
  int atmfVccReceiveTrafficDescriptorParm3;
  int atmfVccReceiveTrafficDescriptorParm4;
  int atmfVccReceiveTrafficDescriptorParm5;
  int atmfVccQosCategory;
  int atmfVccTransmitQosClass;
  int atmfVccReceiveQosClass;
  int atmfVccBestEffortIndicator;
  int atmfVccTransmitFrameDiscard;
  int atmfVccReceiveFrameDiscard;
  int atmfVccServiceCategory;
} AUTOCONFIG_VCC_INFO,  *pAUTOCONFIG_VCC_INFO;

typedef struct autoconfigServiceTypeInfo {
  int atmfServiceTypeIndex;
  char atmfServiceProviderName[AUTOCONIFG_DISPLAY_STRING_MAX];
  char atmfServiceName[AUTOCONIFG_DISPLAY_STRING_MAX];
  char atmfServiceSubName[AUTOCONIFG_DISPLAY_STRING_MAX];
  char atmfServiceClient[AUTOCONIFG_DISPLAY_STRING_MAX];
  int atmfServiceTMCategory;
  int atmfServiceTMConformanceDef;
  char atmfAtmServiceLayer2ProtocolId[AUTOCONIFG_OCTETS_STRING_MAX];
  char atmfAtmServiceLayer3ProtocolId[AUTOCONIFG_OCTETS_STRING_MAX];
  int referenceCount;
} AUTOCONFIG_SERVICE_TYPE_INFO, *pAUTOCONFIG_SERVICE_TYPE_INFO;

typedef struct autoconfigAalProfileInfo {
  union {
    struct {
      int atmfAAL1ProfileIndex;
      int atmfAAL1Subtype;
      int atmfAAL1CBRRate;
      int atmfAAL1ClkRecoveryType;
      int atmfAAL1FEC;
      int atmfAAL1SDT;
      int atmfAAL1PartiallyFilledCells;
      int atmfAAL1CellLossIntegrPeriod;
    } aal1;
    struct {
      int atmfAAL34ProfileIndex;
      int atmfAAL34MaxCpcsSduSizeForward;
      int atmfAAL34MaxCpcsSduSizeBackward;
      int atmfAAL34MIDRangeLow;
      int atmfAAL34MIDRangeHigh;
      int atmfAAL34AALMode;
      int atmfAAL34SscsType;
    } aal34;
    struct {
      int atmfAAL5ProfileIndex;
      int atmfAAL5MaxCpcsSduSizeForward;
      int atmfAAL5MaxCpcsSduSizeBackward;
      int atmfAAL5AALMode;
      int atmfAAL5SscsType;
    } aal5;
  };
  int type;
  int referenceCount;
} AUTOCONFIG_AAL_PROFILE_INFO, *pAUTOCONFIG_AAL_PROFILE_INFO;

typedef struct autoconfigServiceConnInfo {
  int atmfServicePortIndex;
  int atmfServiceVpi;
  int atmfServiceVci;
  int atmfServiceSignalId;
  int atmfServiceConnServiceIndex;  
  char atmfServiceConnServiceName[AUTOCONIFG_DISPLAY_STRING_MAX];
  int atmfServiceConnAALType;
  int atmfServiceConnAALIndex;
} AUTOCONFIG_SERVICE_CONN_INFO, *pAUTOCONFIG_SERVICE_CONN_INFO;

typedef struct autoconfigVcInfo {
  int port;
  int vpi;
  int vci;
  int operStatus;  /* ATMF_VCC_STATUS_UNKNOWN, ATMF_VCC_STATUS_END2END_UP, 
                      ATMF_VCC_STATUS_END2END_DOWN, ATMF_VCC_STATUS_LOCAL_UP_END2END_UNKNOWN,
                      ATMF_VCC_STATUS_LOCAL_DOWN */
} AUTOCONFIG_VC_INFO, *pAUTOCONFIG_VC_INFO;

pAUTOCONFIG_ATTACHPOINT_INFO pAutoconfigAttachPointInfo;
pAUTOCONFIG_CONFIG_INFO pAutoconfigConfigInfo;
/* list of VCC that may or may not have service type info */
netsnmp_data_list *pAutoconfigVccList;
/* contain serviceType, and AAL profile info per VCC */
netsnmp_data_list *pAutoconfigServiceConnList;

/* utils */
int autoconfig_input(int op,netsnmp_session * session,int reqid, netsnmp_pdu *pdu,void *magic);
void autoconfig_initSession(netsnmp_session *session, char *addrStr);
int autoconfig_processResponseMessages(netsnmp_pdu *pdu,int reqid);
extern int netsnmp_handle_request(netsnmp_agent_session *asp, int status);
void autoconfig_addToServiceConnList (pAUTOCONFIG_SERVICE_CONN_INFO entry);
void autoconfig_addToAALProfileList (pAUTOCONFIG_AAL_PROFILE_INFO entry);
void autoconfig_addToServiceTypeList (pAUTOCONFIG_SERVICE_TYPE_INFO entry);
void autoconfig_addToVccList (pAUTOCONFIG_VCC_INFO entry);
void autoconfig_addToRequestList (int requestId);
void autoconfig_removeFromRequestList (int requestId);
int autoconfig_sendTrap(int trap,
                        int specific,
                        oid * enterprise, int enterprise_length,
                        netsnmp_variable_list * vars);
int autoconfig_verifyReceiveConnInfoCfg(int oidSet, pAUTOCONFIG_SERVICE_CONN_INFO receiveInfo,
                                        netsnmp_pdu *pdu);
int autoconconfig_verifyReceiveServiceTypeCfg(int oidSet, pAUTOCONFIG_SERVICE_TYPE_INFO receiveInfo,
                                              netsnmp_pdu *pdu);

/* actions */
void autoconfig_clearAttachPoint(pAUTOCONFIG_ATTACHPOINT_INFO point);
void autoconfig_setAttachPoint(pAUTOCONFIG_ATTACHPOINT_INFO point);
int autoconfig_compareAttachPoint(pAUTOCONFIG_ATTACHPOINT_INFO point1,
                                  pAUTOCONFIG_ATTACHPOINT_INFO point2);
int autoconfig_sendColdStart();
int autoconfig_sendConfigFailTrap();
int autoconfig_sendAttachPointRequest();
int autoconfig_sendConfigRequest();
int autoconfig_sendServiceConnInfoRequest(int port, int vpi, int vci);
int autoconfig_sendVccTableRequest(int port, int vpi, int vci);
int autoconfig_sendServiceTypeRequest(int index);
int autoconfig_sendAal5ProfileRequest(int index);
void autoconfig_setConfigurationInfo(pAUTOCONFIG_CONFIG_INFO info);
void autoconfig_setServiceConnInfo(pAUTOCONFIG_SERVICE_CONN_INFO entry);
void autoconfig_setServiceTypeInfo(pAUTOCONFIG_SERVICE_TYPE_INFO entry);
void autoconfig_setVccInfo(pAUTOCONFIG_VCC_INFO entry);
void autoconfig_clearServiceConnInfo(pAUTOCONFIG_VC_INFO entry);
void autoconfig_clearServiceTypeInfo(int index);
void autoconfig_clearAALProfileInfo(int index);
void autoconfig_clearVccInfo(int port, int vpi, int vci);
void autoconfig_clearConfigurationInfo(pAUTOCONFIG_CONFIG_INFO info);
void autoconfig_clearAllInfo();

/* events */
int autoconfig_receiveVccChangeTrap(netsnmp_pdu *pdu, pAUTOCONFIG_VC_INFO vcInfo);
int autoconfig_receiveAttachPointResponse(netsnmp_pdu *pdu);
int autoconfig_receiveConfigResponse(netsnmp_pdu *pdu);
int autoconfig_receiveServiceConnInfoResponse(netsnmp_pdu *pdu);
int autoconfig_receiveAal5ProfileResponse(netsnmp_pdu *pdu);
int autoconfig_receiveServiceTypeResponse(netsnmp_pdu *pdu);
int autoconfig_receiveVccTableResponse(netsnmp_pdu *pdu, int *oidReceive);

/* debugs */
void autoconfig_displayAttachPointParams(void);
void autoconfig_displayConfigParams(void);
void autoconfig_displayServiceConnList(void);
void autoconfig_displayAALProfileList(void);
void autoconfig_displayServiceTypeList(void);
void autoconfig_displayVccList(void);
void autoconfig_displayRequestList(void);

#endif /* ATMF_AUTOCONFIG_H */

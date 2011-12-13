/*
<:copyright-gpl
 Copyright 2007 Broadcom Corp. All Rights Reserved.

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

/***************************************************************************
 * File Name  : bcmxtmcfg.h
 *
 * Description: This file contains the definitions, structures and function
 *              prototypes for the Broadcom Asynchronous/Packet Transfer Mode
 *              (XTM) Configuration driver.
 ***************************************************************************/

#if !defined(_BCMXTMCFG_H_)
#define _BCMXTMCFG_H_

#if defined(__cplusplus)
extern "C" {
#endif


/***************************************************************************
 * Constant Definitions
 ***************************************************************************/

/* Miscellaneous */
#define MAX_PHY_PORTS                   4
#define MAX_RECEIVE_QUEUES              4
#define MAX_SUB_PRIORITIES              8
#define MAX_TRANSMIT_QUEUES             16
#define NETWORK_DEVICE_NAME_SIZE        16

/* Values for ulPortId and ulPortMask fields. */
#define PORT_PHY0_FAST                  0x01
#define PORT_PHY0_INTERLEAVED           0x02
#define PORT_PHY1_FAST                  0x04
#define PORT_PHY1_INTERLEAVED           0x08

/* Conversions between port and port id. */
#define PORT_TO_PORTID(PORT)            (UINT32) (1 << (PORT))
#define PORTID_TO_PORT(PORTID)          (UINT32)                \
    (((PORTID) == PORT_PHY0_FAST)        ? 0 :                  \
     ((PORTID) == PORT_PHY0_INTERLEAVED) ? 1 :                  \
     ((PORTID) == PORT_PHY1_FAST)        ? 2 :                  \
     ((PORTID) == PORT_PHY1_INTERLEAVED) ? 3 : MAX_PHY_PORTS)

/* Values for XTM_INITIALIZATION_PARMS ulPortConfig. */
#define PC_INTERNAL_EXTERNAL_MASK       0x03
#define PC_ALL_INTERNAL                 0x00
#define PC_ALL_EXTERNAL                 0x01
#define PC_INTERNAL_EXTERNAL            0x02
#define PC_NEG_EDGE                     0x04

/* Values for XTM_INITIALIZATION_PARMS ulBondingConfig. */
#define BC_BONDING_ENABLE               0x01
#define BC_12_BIT_SID_ENABLE            0x02
#define BC_USE_DUAL_LATENCY             0x04

/* Values for XTM_TRAFFIC_DESCR_PARM_ENTRY ulTrafficDescrType. */
#define TDT_ATM_NO_TRAFFIC_DESCRIPTOR   1
#define TDT_ATM_NO_CLP_NO_SCR           2
#define TDT_ATM_NO_CLP_SCR              5
#define TDT_ATM_CLP_NO_TAGGING_MCR      8
#define TDT_ATM_CLP_TRANSPARENT_NO_SCR  9
#define TDT_ATM_CLP_TRANSPARENT_SCR     10
#define TDT_ATM_NO_CLP_TAGGING_NO_SCR   11
#define TDT_ATM_PTM_MAX_BIT_RATE_SCR    16

/* Values for XTM_TRAFFIC_DESCR_PARM_ENTRY ulServiceCategory. */
#define SC_OTHER                        1
#define SC_CBR                          2
#define SC_RT_VBR                       3
#define SC_NRT_VBR                      4
#define SC_UBR                          6
#define SC_MBR                          7

/*Values for XTM_INTERFACE_CFG ulIfAdminStatus and XTM_CONN_CFG ulAdminStatus */
#define ADMSTS_UP                       1
#define ADMSTS_DOWN                     2

/* Values for XTM_INTERFACE_CFG ulIfOperStatus and XTM_CONN_CFG ulOperStatus. */
#define OPRSTS_UP                       1
#define OPRSTS_DOWN                     2

/* Values for XTM_INTERFACE_LINK_INFO ulLinkState. */
#define LINK_UP                         1
#define LINK_DOWN                       2
#define LINK_START_TEQ_DATA             3
#define LINK_STOP_TEQ_DATA              4

/* Values for XTM_ADDR ulTrafficType, XTM_INTERFACE_CFG ulIfTrafficType and
 * XTM_INTERFACE_LINK_INFO ulLinkTrafficType.
 */
#define TRAFFIC_TYPE_NOT_CONNECTED      0
#define TRAFFIC_TYPE_ATM                1
#define TRAFFIC_TYPE_PTM                2

/* Values for PTM_ADDR ulPtmPriority. */
#define PTM_PRI_LOW                     0x01
#define PTM_PRI_HIGH                    0x02

/* Values for XTM_TRANSMIT_QUEUE_PARMS ulWeightAlg. */
#define WA_DISABLED                     0 /* disabled */
#define WA_CWRR                         1 /* cell weighted round robin */
#define WA_PWRR                         2 /* packet weighted round robin */
#define WA_WFQ                          3 /* weighted fair queuing */

/* Values for XTM_CONN_CFG ulAtmAalType. */
#define AAL_TRANSPARENT                 1
#define AAL_0_PACKET                    2
#define AAL_0_CELL                      3
#define AAL_5                           7

/* Values for XTM_CONN_CFG ulHeaderType. */
#define HT_NONE                         0x00
#define HT_LLC_SNAP_ETHERNET            0x01 /* AA AA 03 00 80 C2 00 07 00 00 */
#define HT_LLC_SNAP_ROUTE_IP            0x02 /* AA AA 03 00 00 00 08 00 */
#define HT_LLC_ENCAPS_PPP               0x03 /* FE FE 03 CF */
#define HT_VC_MUX_ETHERNET              0x04 /* 00 00 */

/* Values for BcmXtm_SendOamCell ucCircuitType. */
#define CTYPE_OAM_F5_SEGMENT            0x00
#define CTYPE_OAM_F5_END_TO_END         0x01
#define CTYPE_OAM_F4_SEGMENT            0x02
#define CTYPE_OAM_F4_END_TO_END         0x03
#define CTYPE_ASM_P0                    0x04
#define CTYPE_ASM_P1                    0x05
#define CTYPE_ASM_P2                    0x06
#define CTYPE_ASM_P3                    0x07


/***************************************************************************
 * Type Definitions
 ***************************************************************************/

/* Return status values. */
typedef enum BcmXtmStatus
{
    XTMSTS_SUCCESS = 0,
    XTMSTS_ERROR,
    XTMSTS_STATE_ERROR,
    XTMSTS_PARAMETER_ERROR,
    XTMSTS_ALLOC_ERROR,
    XTMSTS_RESOURCE_ERROR,
    XTMSTS_IN_USE,
    XTMSTS_NOT_FOUND,
    XTMSTS_NOT_SUPPORTED,
    XTMSTS_TIMEOUT
} BCMXTM_STATUS;

typedef struct AtmAddr
{
    UINT32 ulPortMask;
    UINT16 usVpi;
    UINT16 usVci;
} ATM_ADDR, *PATM_ADDR;

typedef struct PtmAddr
{
    UINT32 ulPortMask;
    UINT32 ulPtmPriority;
} PTM_ADDR, *PPTM_ADDR;

typedef struct XtmAddr
{
    UINT32 ulTrafficType;
    union
    {
        ATM_ADDR Vcc;
        PTM_ADDR Flow;
    } u;
} XTM_ADDR, *PXTM_ADDR;

typedef struct XtmInitialization
{
    UINT32 ulReceiveQueueSizes[MAX_RECEIVE_QUEUES];
    UINT32 ulPortConfig;
    UINT32 ulBondingConfig;
} XTM_INITIALIZATION_PARMS, *PXTM_INITIALIZATION_PARMS;

typedef struct XtmInterfaceCfg
{
    UINT32 ulIfAdminStatus;         /* UTOPIA enable/disable */
    UINT32 ulIfOperStatus;          /* read only */
    UINT32 ulIfLastChange;          /* read only */
    UINT32 ulIfTrafficType;         /* read only */
    UINT32 ulAtmInterfaceConfVccs;  /* read only */
} XTM_INTERFACE_CFG, *PXTM_INTERFACE_CFG;

/* XTM_TRAFFIC_DESCR_PARM_ENTRY contains the fields needed to create a Traffic
 * Descriptor Table parameter entry.
 */
typedef struct XtmTrafficDescrParmEntry
{
    UINT32 ulTrafficDescrIndex;
    UINT32 ulTrafficDescrType;
    UINT32 ulPcr;
    UINT32 ulScr;
    UINT32 ulMbs;
    UINT32 ulMcr;
    UINT32 ulServiceCategory;
} XTM_TRAFFIC_DESCR_PARM_ENTRY, *PXTM_TRAFFIC_DESCR_PARM_ENTRY;

typedef struct XtmTransmitQueueParms
{
    UINT32 ulPortId;
    UINT32 ulPtmPriority;
    UINT32 ulWeightAlg;
    UINT32 ulWeightValue;
    UINT32 ulSubPriority;
    UINT32 ulSize;
} XTM_TRANSMIT_QUEUE_PARMS, *PXTM_TRANSMIT_QUEUE_PARMS;

typedef struct XtmConnCfg
{
    UINT32 ulAtmAalType;
    UINT32 ulAdminStatus;
    UINT32 ulOperStatus;            /* read only */
    UINT32 ulLastChange;            /* read only */

    UINT32 ulTransmitTrafficDescrIndex;
    UINT32 ulHeaderType;
    UINT32 ulTransmitQParmsSize;
    XTM_TRANSMIT_QUEUE_PARMS TransmitQParms[MAX_TRANSMIT_QUEUES];
} XTM_CONN_CFG, *PXTM_CONN_CFG;

typedef struct XtmInterfaceStats
{
    UINT32 ulIfInOctets;
    UINT32 ulIfOutOctets;
    UINT32 ulIfInPackets;
    UINT32 ulIfOutPackets;
    UINT32 ulIfInOamRmCells;
    UINT32 ulIfOutOamRmCells;
    UINT32 ulIfInAsmCells;
    UINT32 ulIfOutAsmCells;
    UINT32 ulIfInCellErrors;
    UINT32 ulIfInPacketErrors;
} XTM_INTERFACE_STATS, *PXTM_INTERFACE_STATS;

typedef struct XtmInterfaceLinkInfo
{
    UINT32 ulLinkState;
    UINT32 ulLinkUsRate;
    UINT32 ulLinkDsRate;
    UINT32 ulLinkTrafficType;
} XTM_INTERFACE_LINK_INFO, *PXTM_INTERFACE_LINK_INFO;

typedef struct XtmOamCellInfo
{
    UINT8 ucCircuitType;
    UINT32 ulTimeout;
    UINT32 ulRepetition;
    UINT32 ulSent;
    UINT32 ulReceived;
    UINT32 ulMinRspTime;
    UINT32 ulMaxRspTime;
    UINT32 ulAvgRspTime;
} XTM_OAM_CELL_INFO, *PXTM_OAM_CELL_INFO;


/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

BCMXTM_STATUS BcmXtm_Initialize( PXTM_INITIALIZATION_PARMS pInitParms );
BCMXTM_STATUS BcmXtm_Uninitialize( void );
BCMXTM_STATUS BcmXtm_GetTrafficDescrTable( PXTM_TRAFFIC_DESCR_PARM_ENTRY
    pTrafficDescTable, UINT32 *pulTrafficDescrTableSize );
BCMXTM_STATUS BcmXtm_SetTrafficDescrTable( PXTM_TRAFFIC_DESCR_PARM_ENTRY
    pTrafficDescTable, UINT32  ulTrafficDescrTableSize );
BCMXTM_STATUS BcmXtm_GetInterfaceCfg( UINT32 ulPortId, PXTM_INTERFACE_CFG
    pInterfaceCfg );
BCMXTM_STATUS BcmXtm_SetInterfaceCfg( UINT32 ulPortId, PXTM_INTERFACE_CFG
    pInterfaceCfg );
BCMXTM_STATUS BcmXtm_GetConnCfg( PXTM_ADDR pConnAddr, PXTM_CONN_CFG pConnCfg );
BCMXTM_STATUS BcmXtm_SetConnCfg( PXTM_ADDR pConnAddr, PXTM_CONN_CFG pConnCfg );
BCMXTM_STATUS BcmXtm_GetConnAddrs( PXTM_ADDR pConnAddrs, UINT32 *pulNumConns );
BCMXTM_STATUS BcmXtm_GetInterfaceStatistics( UINT32 ulPortId,
    PXTM_INTERFACE_STATS pStatistics, UINT32 ulReset );
BCMXTM_STATUS BcmXtm_SetInterfaceLinkInfo( UINT32 ulPortId,
    PXTM_INTERFACE_LINK_INFO pLinkInfo );
BCMXTM_STATUS BcmXtm_SendOamCell( PXTM_ADDR pConnAddr,
    PXTM_OAM_CELL_INFO pOamCellInfo);
BCMXTM_STATUS BcmXtm_CreateNetworkDevice( PXTM_ADDR pConnAddr,
    char *pszNetworkDeviceName );
BCMXTM_STATUS BcmXtm_DeleteNetworkDevice( PXTM_ADDR pConnAddr );


#if defined(__cplusplus)
}
#endif

#endif /* _BCMXTMCFG_H_ */


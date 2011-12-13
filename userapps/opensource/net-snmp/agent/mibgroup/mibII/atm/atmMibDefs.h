/****************************************************************************
*
*     Copyright (c) 2001 Broadcom Corporation
*           All Rights Reserved
*
*     No portions of this material may be reproduced in any form without the
*     written permission of:
*
*           Broadcom Corporation
*           16251 Laguna Canyon Road
*           Irvine, California  92618
*
*     All information contained in this document is Broadcom Corporation
*     company private, proprietary, and trade secret.
*
*****************************************************************************/
/**
*
*  @file    atmMibDefs.h
*
*  @brief   ATM MIB constant definitions
*
****************************************************************************/

#if !defined( ATM_MIB_DEFS_H )
#define ATM_MIB_DEFS_H

/* ---- Include Files ---------------------------------------------------- */


/* ---- Constants and Types ---------------------------------------------- */

#define ATMIFCONFTBL
#define ATMIFTCTBL
#define ATMTRAFFDESCTBL
#define ATMVCLTBL
#define AAL5VCCTBL

#define ATM_INTERFACE_COUNT               1  /* Number of ATM interfaces */

#define ATM_INTF_MAX_VPCS						0
#define ATM_INTF_MAX_VCCS						8
#define ATM_INTF_CONF_VPCS						0
#define ATM_INTF_MAX_ACTIVE_VPI_BITS		12
#define ATM_INTF_MAX_ACTIVE_VCI_BITS		16
#define ATM_INTF_ILMI_VPI						0
#define ATM_INTF_ILMI_VCI_UNSUPPORTED					0
#define ATM_INTF_ILMI_VCI        					16
#define ATM_INTF_CURRENT_MAX_VPI_BITS		ATM_INTF_MAX_ACTIVE_VPI_BITS
#define ATM_INTF_CURRENT_MAX_VCI_BITS		ATM_INTF_MAX_ACTIVE_VCI_BITS

#define ATM_INTF_TC_OCD                   0

#define ATM_INTF_ADDRTYPE_PRIVATE         1
#define ATM_INTF_ADDRTYPE_NSAPI_E164      2
#define ATM_INTF_ADDRTYPE_NATIVE_E164     3
#define ATM_INTF_ADDRTYPE_OTHER           4

#define ATM_INTF_TC_ALMST_NOALARM         1
#define ATM_INTF_TC_ALMST_LCDFAIL         2

#define ATM_TRAFDESC_PARAM                0
#define ATM_TRAFDESC_QOSCLASS             0
#define ATM_TRAFDESC_ROWSTATUS_ACTIVE     1
#define ATM_TRAFDESC_SERVICE_CAT_UBR      6

#define ATM_VCL_AALTYPE_AAL1              1
#define ATM_VCL_AALTYPE_AAL34             2
#define ATM_VCL_AALTYPE_AAL5              3
#define ATM_VCL_AALTYPE_OTHER             4
#define ATM_VCL_AALTYPE_UNKNOWN           5
#define ATM_VCL_AALTYPE_AAL2              6
#define ATM_VCL_SDUSIZE                   0xFFFF
#define ATM_VCL_ROWSTATUS_ACTIVE          1
#define ATM_VCL_CAST_TYPE_P2P             1
#define ATM_VCL_CONN_KIND_PVC             1

/* ---- Constants and Types ---------------------------------------------- */
/* ---- Variable Externs ------------------------------------------------- */
/* ---- Function Prototypes ---------------------------------------------- */


#endif /* ATM_MIB_DEFS_H */

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
*  @file    atmMibHelper.h
*
*  @brief   Helper functions and defines for the ATM MIB
*
****************************************************************************/

#if !defined( ATM_MIB_HELPER_H )
#define ATM_MIB_HELPER_H

/* ---- Include Files ---------------------------------------------------- */

#include <stdio.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/auto_nlist.h>

#include "atmMibDefs.h"

/* ---- Constants and Types ---------------------------------------------- */

#define ATM_TRAFDESC_OIDLEN   10

//#define ATMMIBLOG_DEBUG(fmt)        printf fmt;printf("\n");
#define ATMMIBLOG_DEBUG(fmt)

#define ATMMIBLOG_ERROR(fmt)        printf fmt;printf("\n");


typedef struct ATM_INTERFACE_CONF_TABLE
{
   void* pTableBeginning;  /* Points to the beginning of the table (linked list)
                           ** (needed for deallocation of the linked list) */

   int   ifIndex;

   int   atmInterfaceMaxVpcs                 ;
   int   atmInterfaceMaxVccs                 ;
   int   atmInterfaceConfVpcs                ;
   int   atmInterfaceConfVccs                ;
   int   atmInterfaceMaxActiveVpiBits        ;
   int   atmInterfaceMaxActiveVciBits        ;
   int   atmInterfaceIlmiVpi                 ;
   int   atmInterfaceIlmiVci                 ;
   int   atmInterfaceAddressType             ;
   char  atmInterfaceAdminAddress[40]        ;
   char  atmInterfaceMyNeighborIpAddress[4]  ;
   char  atmInterfaceMyNeighborIfName[256]   ;
   int   atmInterfaceCurrentMaxVpiBits       ;
   int   atmInterfaceCurrentMaxVciBits       ;
   char  atmInterfaceSubscrAddress[40]       ;

   struct ATM_INTERFACE_CONF_TABLE* next; /* Pointer to the next item in the table (linked list).
                                          ** For a single ATM port, this points to NULL
                                          ** so we have only one entry in the table. */

} ATM_INTERFACE_CONF_TABLE;


typedef struct ATM_INTERFACE_TC_TABLE
{
   void* pTableBeginning;  /* Points to the beginning of the table (linked list)
                           ** (needed for deallocation of the linked list) */

   int   ifIndex                             ;

   int   atmInterfaceOCDEvents               ;
   int   atmInterfaceTCAlarmState            ;

   struct ATM_INTERFACE_TC_TABLE* next; /* Pointer to the next item in the table (linked list).
                                          ** For a single ATM port, this points to NULL
                                          ** so we have only one entry in the table. */

} ATM_INTERFACE_TC_TABLE;


typedef struct ATM_TRAFDESC_TABLE
{
   void* pTableBeginning;  /* Points to the beginning of the table (linked list)
                           ** (needed for deallocation of the linked list) */

   int      atmTrafficDescrParamIndex;
   oid      atmTrafficDescrType[ATM_TRAFDESC_OIDLEN];
   int      atmTrafficDescrParam1;
   int      atmTrafficDescrParam2;
   int      atmTrafficDescrParam3;
   int      atmTrafficDescrParam4;
   int      atmTrafficDescrParam5;
   int      atmTrafficQoSClass;
   int      atmTrafficDescrRowStatus;
   int      atmServiceCategory;
   int     atmTrafficFrameDiscard;

   struct ATM_TRAFDESC_TABLE* next; /* Pointer to the next item in the table (linked list).
                                    ** For a single ATM port, this points to NULL
                                    ** so we have only one entry in the table. */

} ATM_TRAFDESC_TABLE;


typedef struct ATM_VCL_TABLE
{
   void* pTableBeginning;  /* Points to the beginning of the table (linked list)
                           ** (needed for deallocation of the linked list) */

   int      ifIndex;

   int      atmVclVpi;
   int      atmVclVci;
   int      atmVclAdminStatus;
   int      atmVclOperStatus;
   int      atmVclLastChange;
   int      atmVclReceiveTrafficDescrIndex;
   int      atmVclTransmitTrafficDescrIndex;
   int      atmVccAalType;
   int      atmVccAal5CpcsTransmitSduSize;
   int      atmVccAal5CpcsReceiveSduSize;
   int      atmVccAal5EncapsType;
   int      atmVclCrossConnectIdentifier;
   int      atmVclRowStatus;
   int      atmVclCastType;
   int      atmVclConnKind;

   struct   ATM_VCL_TABLE* next; /* Pointer to the next item in the table (linked list).
                                    ** For a single ATM port, this points to NULL
                                    ** so we have only one entry in the table. */

} ATM_VCL_TABLE;


typedef struct AAL5_VCC_TABLE
{
   void* pTableBeginning;  /* Points to the beginning of the table (linked list)
                           ** (needed for deallocation of the linked list) */

   int      aal5ifIndex;

   int      aal5VccVpi;
   int      aal5VccVci;
   int      aal5VccCrcErrors;
   int      aal5VccSarTimeOuts;
   int      aal5VccOverSizedSDUs;

   struct   AAL5_VCC_TABLE* next; /* Pointer to the next item in the table (linked list). */
} AAL5_VCC_TABLE;

/* ---- Variable Externs ------------------------------------------------- */


/* ---- Function Prototypes ---------------------------------------------- */

/* Fill out the ATM interface configuration table */
void  atmMibHelper_fillAtmIfConfTable( ATM_INTERFACE_CONF_TABLE* pTable );

/* Fill out the ATM interface TC table */
void  atmMibHelper_fillAtmIfTCTable( ATM_INTERFACE_TC_TABLE* pTable );

/* Fill out the ATM traffic descriptor table */
int  atmMibHelper_fillAtmTrafDescTable( ATM_TRAFDESC_TABLE* pTable );

/* Fill out the ATM VCL table */
int atmMibHelper_fillAtmVclTable( ATM_VCL_TABLE* pTable );

/* Fill out the AAL5 VCC table */
int atmMibHelper_fillAal5VccTable( AAL5_VCC_TABLE* pTable );

/* Get the number of traffic indexes */
int atmMibHelper_getTrafDescIndexCount( void );

/* Get the number of ATM interfaces */
int atmMibHelper_getAtmIfCount( void );

/* Get the total number of VCC for all the ATM interfaces */
int atmMibHelper_getTotalVccCount( void );

#endif /* ATM_MIB_HELPER_H */


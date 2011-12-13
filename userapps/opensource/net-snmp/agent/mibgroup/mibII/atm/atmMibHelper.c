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
*  @file    atmMibHelper.c
*
*  @brief   Helper functions for the ATM MIB
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/auto_nlist.h>

#include "../interfaces.h"
#include "atmMibHelper.h"
#include "atmMibDefs.h"

#include <bcmtypes.h>
#include <bcmatmapi.h>
#include <atmapidrv.h>

#ifdef SNMP_ATM_MIB

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

#define ATMMIB_STATUS_OK         0
#define ATMMIB_STATUS_ERR        1

/* ---- Private Variables ------------------------------------------------ */

static oid defAtmTrafficDescriptor[ATM_TRAFDESC_OIDLEN]  = { 1, 3, 6, 1, 2, 1, 37, 1, 1, 2 }   ;

/* ---- Private Function Prototypes -------------------------------------- */
static int     getConfVccCount( void );
static void    trafDescCopyDriverData( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow );
static void    trafDescSetDefValues( ATM_TRAFDESC_TABLE* tableRow );
static void    trafDescMapType( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow );
static void    vclCopyDriverData( ATM_VCL_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_CFG* vccCfg );
static void    aal5vccCopyDriverData( AAL5_VCC_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_STATS* stats );
static void    aal5vccSetDefValues( AAL5_VCC_TABLE* tableRow );
static void    vclSetDefValues( ATM_VCL_TABLE* tableRow );
static int     mapAalType( UINT32 driverAalType );

/* ---- Functions -------------------------------------------------------- */
int     getVccAddrs( int numAddrs, PATM_VCC_ADDR pAddrs );
int     getAal5CpcsIfIndex( void );
int     getAtmCellIfIndex( void );

#ifdef ATMIFCONFTBL
/* Fill out the ATM interface configuration table */
void  atmMibHelper_fillAtmIfConfTable( ATM_INTERFACE_CONF_TABLE* pTable )
{
   /* Assuming only one ATM interface (ATM_INTERFACE_COUNT = 1) */

   ATM_INTERFACE_CONF_TABLE* p = pTable;

   p->pTableBeginning = pTable;
   p->ifIndex = getAtmCellIfIndex();

   p->atmInterfaceMaxVpcs                           = ATM_INTF_MAX_VPCS;
   p->atmInterfaceMaxVccs                           = ATM_INTF_MAX_VCCS;
   p->atmInterfaceConfVpcs                          = ATM_INTF_CONF_VPCS;
   p->atmInterfaceConfVccs                          = getConfVccCount();
   p->atmInterfaceMaxActiveVpiBits                  = ATM_INTF_MAX_ACTIVE_VPI_BITS;
   p->atmInterfaceMaxActiveVciBits                  = ATM_INTF_MAX_ACTIVE_VCI_BITS;
   p->atmInterfaceIlmiVpi                           = ATM_INTF_ILMI_VPI;
#ifdef SNMP_AUTOCONFIG
   p->atmInterfaceIlmiVci                           = ATM_INTF_ILMI_VCI;
#else
   p->atmInterfaceIlmiVci                           = ATM_INTF_ILMI_VCI_UNSUPPORTED;
#endif
   p->atmInterfaceAddressType                       = ATM_INTF_ADDRTYPE_OTHER;
   strncpy( p->atmInterfaceAdminAddress, "", strlen("") );
   strncpy( p->atmInterfaceMyNeighborIpAddress, "", strlen("") );
   strncpy( p->atmInterfaceMyNeighborIfName, "", strlen("") );
   p->atmInterfaceCurrentMaxVpiBits                 = ATM_INTF_CURRENT_MAX_VPI_BITS;
   p->atmInterfaceCurrentMaxVciBits                 = ATM_INTF_CURRENT_MAX_VCI_BITS;
   strncpy( p->atmInterfaceSubscrAddress, "", strlen("") );

   p->next = NULL;
}
#endif /* ATMIFCONFTBL */


#ifdef ATMIFTCTBL
/* Fill out the ATM interface TC table */
void  atmMibHelper_fillAtmIfTCTable( ATM_INTERFACE_TC_TABLE* pTable )
{
   /* Assuming only one ATM interface (ATM_INTERFACE_COUNT = 1) */

   ATM_INTERFACE_TC_TABLE* p = pTable;
   p->pTableBeginning = pTable;
   p->ifIndex = getAtmCellIfIndex();

   p->atmInterfaceOCDEvents      = ATM_INTF_TC_OCD             ;
   p->atmInterfaceTCAlarmState   = ATM_INTF_TC_ALMST_NOALARM   ;

   p->next = NULL;
}
#endif /* ATMIFTCTBL */


#ifdef ATMTRAFFDESCTBL
/* Fill out the ATM interface traffic descriptor table */
int   atmMibHelper_fillAtmTrafDescTable( ATM_TRAFDESC_TABLE* pTable )
{
   ATM_TRAFDESC_TABLE* p = pTable;
   int nRet = 0;
   int i;
   UINT32 ulTdtSize = 0;
   int errStatus = ATMMIB_STATUS_ERR;
   ATM_TRAFFIC_DESCR_PARM_ENTRY* pTdt = NULL;
   ATM_TRAFFIC_DESCR_PARM_ENTRY* pTdte = NULL;

   /* Small portion taken from TdteHandler and TdteHandlerShow in atmctl.c */
   BcmAtm_GetTrafficDescrTableSize( &ulTdtSize );
   pTdt = (PATM_TRAFFIC_DESCR_PARM_ENTRY) malloc( ulTdtSize * sizeof(ATM_TRAFFIC_DESCR_PARM_ENTRY) );
   if( pTdt )
   {
      pTdt->ulStructureId = ID_ATM_TRAFFIC_DESCR_PARM_ENTRY;
      nRet = (int) BcmAtm_GetTrafficDescrTable( pTdt, ulTdtSize );
      if( nRet == 0 )
      {
         pTdte = pTdt;
         for ( i = 1; i <= ulTdtSize; i++, pTdte++ )
         {
            p->pTableBeginning = pTable;
            trafDescCopyDriverData( p, pTdte );
            if ( i == ulTdtSize )
            {
               /* the last row */
               p->next = NULL;
               errStatus = ATMMIB_STATUS_OK;
            }
            else
            {
            p->next  = (p+1);
            p++;
         }
         }
      }
      else
      {
         ATMMIBLOG_ERROR(("atmMibHelper_fillAtmTrafDescTable: Could not get traffic description table"));
      }
   }
   else
   {
      ATMMIBLOG_ERROR(("atmMibHelper_fillAtmTrafDescTable: Could not allocate memory for the traffic description table"));
   }

   free( pTdt );

   return( errStatus );
}
#endif /* ATMTRAFFDESCTBL */

#ifdef ATMVCLTBL
/* Fill out the ATM VCL table */
int atmMibHelper_fillAtmVclTable( ATM_VCL_TABLE* pTable )
{
   /* Assuming only one ATM interface (ATM_INTERFACE_COUNT = 1) */

   int errStatus = ATMMIB_STATUS_ERR;
   int nRet = 0;
   PATM_VCC_ADDR pAddrs = NULL;
   ATM_VCL_TABLE* p = pTable;
   int vccCount = 0;
   ATM_VCC_CFG Cfg;
   int i;
   PATM_VCC_ADDR freePointer;

   vccCount = getConfVccCount();

   if ( vccCount > 0 )
   {
      /* Allocate memory for an ATM_VCC_ADDR stuct for each configured VCC. */
      pAddrs = (PATM_VCC_ADDR)malloc( vccCount * (sizeof(ATM_VCC_ADDR) + sizeof(UINT8)) );
      freePointer = pAddrs;
      if ( pAddrs )
      {
         if ( getVccAddrs( vccCount, pAddrs ) == 0 )
         {
            for ( i = 1; i <= vccCount; i++, pAddrs++ )
            {
               memset( &Cfg, 0x00, sizeof(Cfg) );
               Cfg.ulStructureId = ID_ATM_VCC_CFG;

               nRet = (int) BcmAtm_GetVccCfg( pAddrs, &Cfg );
               if ( nRet == 0 )
               {
                  p->pTableBeginning = pTable;
                  p->ifIndex = getAtmCellIfIndex();
		            vclCopyDriverData( p, pAddrs, &Cfg );
                  if ( i == vccCount )
                  {
                     /* the last row */
                     p->next = NULL;
                     errStatus = ATMMIB_STATUS_OK;
                  }
                  else
                  {
                  p->next = (p+1);
                  p++;
               }
               }
               else
               {
                  ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not get VCC configuration"));
                  break;
               }
            }
      }
      else
      {
            ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not get VCC address"));
   }

      free( freePointer );
   }
   else
   {
         ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not allocate memory"));
   }
}

   return( errStatus );
}
#endif /* ATMVCLTBL */


#ifdef AAL5VCCTBL
/* Fill out the AAL5 VCC table */
int atmMibHelper_fillAal5VccTable( AAL5_VCC_TABLE* pTable )
{
   /* Assuming only one AAL5 CPCS interface */

   int errStatus = ATMMIB_STATUS_ERR;
   int nRet = 0;
   PATM_VCC_ADDR pAddrs = NULL;
   AAL5_VCC_TABLE* p = pTable;
   int vccCount = 0;
   ATM_VCC_STATS Stats;
   int i;
   PATM_VCC_ADDR freePointer;


   vccCount = getConfVccCount();

   if ( vccCount > 0 )
   {
      /* Allocate memory for an ATM_VCC_ADDR stuct for each configured VCC. */
      pAddrs = (PATM_VCC_ADDR)malloc( vccCount * (sizeof(ATM_VCC_ADDR) + sizeof(UINT8)) );
      freePointer = pAddrs;
      if ( pAddrs )
      {
         if ( getVccAddrs( vccCount, pAddrs ) == 0 )
         {
            for ( i = 1; i <= vccCount; i++, pAddrs++ )
            {
               memset( &Stats, 0x00, sizeof(Stats) );
               Stats.ulStructureId = ID_ATM_VCC_STATS;

               nRet = (int) BcmAtm_GetVccStatistics( pAddrs, &Stats, 0 );
               if ( nRet == 0 )
               {
                  p->pTableBeginning = pTable;
                  p->aal5ifIndex = getAal5CpcsIfIndex();
		            aal5vccCopyDriverData( p, pAddrs, &Stats );
                  if ( i == vccCount )
                  {
                     /* The last row */
                     p->next = NULL;
                     errStatus = ATMMIB_STATUS_OK;
                  }
                  else
                  {
                  p->next = (p+1);
                  p++;
               }
               }
               else
               {
                  ATMMIBLOG_ERROR(("atmMibHelper_fillAal5VccTable: Could not get AAL5 VCC statistics"));
                  break;
               }
            }
         }
         else
         {
            ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not get VCC address"));
      }

      free( freePointer );
   }
   else
   {
         ATMMIBLOG_ERROR(("atmMibHelper_fillAtmVclTable: Could not allocate memory"));
      }
   }

   return( errStatus );
}
#endif /* AAL5VCCTBL */

/* Get the number of traffic indexes */
int atmMibHelper_getTrafDescIndexCount( void )
{
   int ulTdtSize = 0;

   BcmAtm_GetTrafficDescrTableSize( (UINT32*)&ulTdtSize );

   return( ulTdtSize );
}


/* Get the number of ATM interfaces */
int atmMibHelper_getAtmIfCount( void )
{
#if ( ATM_INTERFACE_COUNT != 1 )
#error Currently only one ATM interface supported by the ATM MIB
#endif
   return( ATM_INTERFACE_COUNT );
}


int atmMibHelper_getTotalVccCount( void )
{
   return ( getConfVccCount() );
}

/* Get the (first) ATM cell interface index */
int getAtmCellIfIndex( void )
{
   /* This should be the index for the ATM cell layer in the ifTable */
   return( Interface_Index_By_Name(ATM_CELL_LAYER_IFNAME, strlen(ATM_CELL_LAYER_IFNAME)) );
}

/* Get the (first) AAL5 CPCS interface index */
int getAal5CpcsIfIndex( void )
{
   /* This should be the index for the AAL5 CPCS layer in the ifTable */
   return( Interface_Index_By_Name(AAL5_CPCS_LAYER_IFNAME, strlen(AAL5_CPCS_LAYER_IFNAME)) );
}

/* Get the number of configured VCCs */
int getConfVccCount( void )
{
   UINT32 ulNumAddrs = 0;
   UINT32 ulInterfaceId = 0;
   UINT32 ulAddrsReturned = 0;
   UINT32 i = 0;

   /* Small portion taken from GetVccAddrsToUse in atmctl.c */
   for( i = 0; i < PHY_NUM_PORTS; i++ )
   {
      ulInterfaceId = (UINT32) i;
      BcmAtm_GetInterfaceId( (UINT8) i, &ulInterfaceId );
      BcmAtm_GetVccAddrs( ulInterfaceId, NULL, 0, &ulAddrsReturned );
      ulNumAddrs += ulAddrsReturned;
   }

   return( ulNumAddrs );
}


#ifdef ATMTRAFFDESCTBL
/* Map the traffic descriptor type from a value provided by the ATM driver (0-15)
** to a oid value for the traffic descriptor type (e.g. { 1, 3, 6, 1, 2, 1, 37, 1, 1, 2 } ) */
void trafDescMapType( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow )
{
   /* Set the default traffic descriptor */
   memcpy(  tableRow->atmTrafficDescrType, defAtmTrafficDescriptor, sizeof(defAtmTrafficDescriptor) );

   /* The ATM driver returns values from 1 to 15 for the traffic descriptor type.
   ** Modify only the last entry in the oid array. */
   tableRow->atmTrafficDescrType[ATM_TRAFDESC_OIDLEN-1] = driverTableRow->ulTrafficDescrType;
}

/* Copy the data from the driver table to the ATM traffic descriptor table */
void trafDescCopyDriverData( ATM_TRAFDESC_TABLE* tableRow, ATM_TRAFFIC_DESCR_PARM_ENTRY* driverTableRow )
{
   /* First, set the default values */
   trafDescSetDefValues( tableRow );

   /* Change only the values that can be obtained from the ATM driver.
   ** Don't change the other values (i.e. keep the defaults) */
   tableRow->atmTrafficDescrParamIndex = driverTableRow->ulTrafficDescrIndex;
   trafDescMapType( tableRow, driverTableRow );
   tableRow->atmTrafficDescrParam1   = driverTableRow->ulTrafficDescrParm1;
   tableRow->atmTrafficDescrParam2   = driverTableRow->ulTrafficDescrParm2;
   tableRow->atmTrafficDescrParam3   = driverTableRow->ulTrafficDescrParm3;
   tableRow->atmTrafficDescrParam4   = driverTableRow->ulTrafficDescrParm4;
   tableRow->atmTrafficDescrParam5   = driverTableRow->ulTrafficDescrParm5;
   tableRow->atmServiceCategory      = driverTableRow->ulServiceCategory;
   tableRow->atmTrafficFrameDiscard  = driverTableRow->ulTrafficFrameDiscard;
}

/* Set the default values for the ATM traffic descriptor table */
void trafDescSetDefValues( ATM_TRAFDESC_TABLE* tableRow )
{
   memcpy(  tableRow->atmTrafficDescrType, defAtmTrafficDescriptor, sizeof(defAtmTrafficDescriptor) );
   tableRow->atmTrafficDescrParam1   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam2   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam3   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam4   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficDescrParam5   = ATM_TRAFDESC_PARAM;
   tableRow->atmTrafficQoSClass      = 0;
   tableRow->atmTrafficDescrRowStatus= ATM_TRAFDESC_ROWSTATUS_ACTIVE;
   tableRow->atmServiceCategory      = ATM_TRAFDESC_SERVICE_CAT_UBR;
   tableRow->atmTrafficFrameDiscard = 1;
}
#endif /* ATMTRAFFDESCTBL */


int getVccAddrs( int numAddrs, PATM_VCC_ADDR pAddrs )
{
   int nRet = 0;
   UINT8 *pucPorts = NULL;
   UINT32 ulInterfaceId = 0;
   UINT32 ulAddrsReturned = 0;
   UINT32 i = 0;

   /* Get the addresses of all configured VCCs. */
   if( pAddrs )
   {
      UINT32 ulAddrsIndex = 0;

      pucPorts = (UINT8 *) (pAddrs + numAddrs);
      for( i = 0; i < PHY_NUM_PORTS; i++ )
      {
         ulInterfaceId = (UINT32) i;
         BcmAtm_GetInterfaceId( (UINT8) i, &ulInterfaceId );
         if( BcmAtm_GetVccAddrs(ulInterfaceId, pAddrs + ulAddrsIndex,
         numAddrs - ulAddrsIndex, &ulAddrsReturned) == STS_SUCCESS)
         {
            memset( pucPorts + ulAddrsIndex, i, ulAddrsReturned );
            ulAddrsIndex += ulAddrsReturned;
         }
      }
   }
   else
   {
      nRet = 1;
   }

   return( nRet );
}


#ifdef ATMVCLTBL
/* Copy the driver data to the user structure for the ATM VCL table */
void vclCopyDriverData( ATM_VCL_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_CFG* vccCfg )
{
	/* First, set the default values */
	vclSetDefValues( tableRow );

   /* Change only the values that can be obtained from the ATM driver.
   ** Don't change the other values (i.e. keep the defaults) */
	tableRow->atmVclVpi						= pAddrs->usVpi;
	tableRow->atmVclVci						= pAddrs->usVci;
	tableRow->atmVclAdminStatus			      = vccCfg->ulAtmVclAdminStatus;
	tableRow->atmVclOperStatus				      = vccCfg->ulAtmVclOperStatus;
	tableRow->atmVclLastChange				      = vccCfg->ulAtmVclLastChange;
	tableRow->atmVccAalType					      = mapAalType( vccCfg->ulAalType );
	tableRow->atmVclTransmitTrafficDescrIndex = vccCfg->ulAtmVclTransmitTrafficDescrIndex;
	tableRow->atmVclReceiveTrafficDescrIndex  = tableRow->atmVclTransmitTrafficDescrIndex;
   tableRow->atmVccAal5EncapsType            = vccCfg->u.Aal5Cfg.ulAtmVccEncapsType;
}

/* Set the default values for the ATM VCL table */
void vclSetDefValues( ATM_VCL_TABLE* tableRow )
{
   tableRow->atmVccAalType					      = ATM_VCL_AALTYPE_AAL5;
	tableRow->atmVccAal5CpcsTransmitSduSize   = ATM_VCL_SDUSIZE;
   tableRow->atmVccAal5CpcsReceiveSduSize    = ATM_VCL_SDUSIZE;
	tableRow->atmVccAalType					      = ATM_VCL_AALTYPE_AAL5;
   tableRow->atmVclCrossConnectIdentifier    = 0;
   tableRow->atmVclRowStatus                 = ATM_VCL_ROWSTATUS_ACTIVE;
   tableRow->atmVclCastType                  = ATM_VCL_CAST_TYPE_P2P;
   tableRow->atmVclConnKind                  = ATM_VCL_CONN_KIND_PVC;
}

/* Map the AAL type provided by the driver to a value understood by the MIB
** (used for the ATM VCL table) */
int mapAalType( UINT32 driverAalType )
{
   int aalType = ATM_VCL_AALTYPE_AAL5;

   switch ( driverAalType )
   {
      case AAL_2:
         aalType = ATM_VCL_AALTYPE_AAL2;
         break;
      case AAL_5:
         aalType = ATM_VCL_AALTYPE_AAL5;
         break;
      default:
         aalType = ATM_VCL_AALTYPE_UNKNOWN;
         break;
   }

   return( aalType );
}
#endif /* ATMVCLTBL */


#ifdef AAL5VCCTBL
/* Copy the driver data to the user structure for the AAL5 VCC table */
void aal5vccCopyDriverData( AAL5_VCC_TABLE* tableRow, PATM_VCC_ADDR pAddrs, ATM_VCC_STATS* stats )
{
	/* First, set the default values */
	aal5vccSetDefValues( tableRow );

   /* Change only the values that can be obtained from the ATM driver.
   ** Keep the defaults for the other values */
   tableRow->aal5VccVpi                      = pAddrs->usVpi;
   tableRow->aal5VccVci                      = pAddrs->usVci;
   tableRow->aal5VccCrcErrors                = stats->u.AtmVccAal5Stats.ulAal5VccCrcErrors;
   tableRow->aal5VccOverSizedSDUs            = stats->u.AtmVccAal5Stats.ulAal5VccOverSizedSdus;
   tableRow->aal5VccSarTimeOuts              = stats->u.AtmVccAal5Stats.ulAal5VccSarTimeOuts;
}

/* Set the default values for the AAL5 VCC table */
void aal5vccSetDefValues( AAL5_VCC_TABLE* tableRow )
{
   tableRow->aal5VccCrcErrors       = 0;
   tableRow->aal5VccOverSizedSDUs   = 0;
   tableRow->aal5VccSarTimeOuts     = 0;
}

#endif /* AAL5VCCTBL */

#endif /* SNMP_ATM_MIB */

int getAtmIntfOperStatus(void)
{
  ATM_INTERFACE_CFG Cfg;
  UINT32 ulInterfaceId;
  int port = 0;

  BcmAtm_GetInterfaceId( (UINT8) port, &ulInterfaceId );
  Cfg.ulStructureId = ID_ATM_INTERFACE_CFG;

  if (BcmAtm_GetInterfaceCfg( ulInterfaceId, &Cfg ) == STS_SUCCESS) 
    {
      if (Cfg.ulIfOperStatus == OPRSTS_UP)
	return IF_OPER_STATUS_UP;
      else 
	return IF_OPER_STATUS_DOWN;
    }
  return IF_OPER_STATUS_UNKNOWN;
}

int getAtmIntfAdminStatus(void)
{
  ATM_INTERFACE_CFG Cfg;
  UINT32 ulInterfaceId;
  int port = 0;

  BcmAtm_GetInterfaceId( (unsigned char) port, &ulInterfaceId );
  Cfg.ulStructureId = ID_ATM_INTERFACE_CFG;

  if (BcmAtm_GetInterfaceCfg( ulInterfaceId, &Cfg ) == STS_SUCCESS) 
    return (Cfg.ulIfAdminStatus);

  return IF_ADMIN_STATUS_DOWN;
}

int setAtmIntfAdminStatus(int status)
{
  ATM_INTERFACE_CFG Cfg;
  UINT32 ulInterfaceId;
  int port = 0;
  int ret = -1;

  Cfg.ulStructureId = ID_ATM_INTERFACE_CFG;
  ret = BcmAtm_GetInterfaceId( (UINT8) port, &ulInterfaceId );
  if (ret == 0) 
    {
      /* Change the state to the specified value. */
      Cfg.ulIfAdminStatus = status; 
      ret = (int)BcmAtm_SetInterfaceCfg(ulInterfaceId, &Cfg );
      if (ret != 0)
	ret = -1;
    }
  return ret;
}

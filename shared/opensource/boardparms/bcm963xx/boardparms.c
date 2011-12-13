/*
<:copyright-gpl 

 Copyright 2003 Broadcom Corp. All Rights Reserved. 
 
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
/**************************************************************************
 * File Name  : boardparms.c
 *
 * Description: This file contains the implementation for the BCM63xx board
 *              parameter access functions.
 * 
 * Updates    : 07/14/2003  Created.
 ***************************************************************************/

/* Includes. */
#include "boardparms.h"

/* Typedefs */
typedef struct boardparameters
{
    char szBoardId[BP_BOARD_ID_LEN];        /* board id string */
    unsigned short usGPIOOverlay;            /* enabled interfaces */

    unsigned short usGpioRj11InnerPair;     /* GPIO pin or not defined */
    unsigned short usGpioRj11OuterPair;     /* GPIO pin or not defined */
    unsigned short usGpioUartRts;           /* GPIO pin or not defined */
    unsigned short usGpioUartCts;           /* GPIO pin or not defined */
    unsigned short usVcopeResetGpio;        /* GPIO pin or not defined */
    unsigned short usVcopeRelayGpio;        /* GPIO pin or not defined */

    unsigned short usGpioLedAdsl;           /* GPIO pin or not defined */
    unsigned short usGpioLedAdslFail;       /* GPIO pin or not defined */
    unsigned short usGpioSecLedAdsl;        /* GPIO pin or not defined */
    unsigned short usGpioSecLedAdslFail;    /* GPIO pin or not defined */
    unsigned short usGpioLedSesWireless;    /* GPIO pin or not defined */        
    unsigned short usGpioLedHpna;           /* GPIO pin or not defined */
    unsigned short usGpioLedWanData;        /* GPIO pin or not defined */
    unsigned short usGpioLedPpp;            /* GPIO pin or not defined */
    unsigned short usGpioLedPppFail;        /* GPIO pin or not defined */
    unsigned short usGpioLedBlPowerOn;      /* GPIO pin or not defined */
    unsigned short usGpioLedBlStop;         /* GPIO pin or not defined */
    unsigned short usGpioBondingSecondaryReset ;
                                            /* GPIO pin or not defined */

    unsigned short usExtIntrAdslDyingGasp;  /* ext intr or not defined */
    unsigned short usExtIntrResetToDefault; /* ext intr or not defined */
    unsigned short usExtIntrSesBtnWireless; /* ext intr or not defined */        
    unsigned short usExtIntrHpna;           /* ext intr or not defined */

    unsigned short usCsHpna;                /* HPNA chip select or not defined */
    unsigned short usCsHpi;                 /* HPI chip select or not defined */

    unsigned short usAntInUseWireless;	    /* antenna in use or not defined */
    unsigned short usWirelessFlags;         /* WLAN flags */       

    unsigned short usVcopeBoardRev;         /* VCOPE board revision */

    ETHERNET_MAC_INFO EnetMacInfos[BP_MAX_ENET_MACS];
    VOIP_DSP_INFO VoIPDspInfo[BP_MAX_VOIP_DSP];
    SLIC_TYPE slicType;                     /* SLIC manufacturer & model */
	DAA_TYPE  daaType;					    /* DAA manufacturer & model*/
    unsigned short usGpioWirelessPowerDown; /* WLAN_PD control or not defined */
    unsigned short usGpioLedUsbHost1;    /* dare sunmin usb host led */    
    unsigned short usGpioLedUsbHost2;    /* dare sunmin usb host led */    
    unsigned short usGpioWirelessProtect;    /* dare sunmin wireless protect */    
} BOARD_PARAMETERS, *PBOARD_PARAMETERS;

#define SW_INFO_DEFAULT(n) { \
  (0x00000001 << (n)) - 1,      /* port_map */ \
  {0, 0, 0, 0, 0, 0, 0, 0}      /* phy_id */ \
}

/* Variables */
#if defined(_BCM96338_) || defined(CONFIG_BCM96338)
static BOARD_PARAMETERS g_bcm96338sv =
{
    "96338SV",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_NOT_DEFINED,                         /* usGpioLedPppFail */
    BP_NOT_DEFINED,                         /* usGpioLedBlPowerOn */
    BP_NOT_DEFINED,                         /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96338l2m8m =
{
    "96338L-2M-8M",                         /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96338l652m8m =
{
    "96338L-65-2M-8M",                      /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_3_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96338gw =
{
    "96338W",                               /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_5_AL,                           /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_0,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    BP_WLAN_MAC_ADDR_OVERRIDE,              /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	  Slic_Not_Defined,                       /* SLIC type */
	  DAA_Not_Defined,                        /* DAA type */
      BP_NOT_DEFINED,                       /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96338gws2 =
{
    "96338WS2",                               /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_5_AL,                           /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_0,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    BP_WLAN_MAC_ADDR_OVERRIDE,              /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */   
    BP_GPIO_6_AH,                           /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96338gw20p2 =
{
    "96338W-20P2",                          /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_3_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_5_AL,                           /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_0,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    BP_WLAN_MAC_ADDR_OVERRIDE,              /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
    DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96338gw2 =
{
    "96338W2",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_5_AL,                           /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_2_AL,                           /* usGpioLedWanData */
    BP_GPIO_1_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_0,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    BP_WLAN_MAC_ADDR_OVERRIDE,              /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
    DAA_Not_Defined,                        /* DAA type */
    BP_GPIO_6_AH,                           /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96338e =
{
    "96338E",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_2_AL,                           /* usGpioLedWanData */
    BP_GPIO_1_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	  Slic_Not_Defined,                       /* SLIC type */
	  DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96338e4 =
{
    "96338E4",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_4_AH,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_HW_DEFINED_AH,                       /* usGpioLedWanData */
    BP_HW_DEFINED_AH,                       /* usGpioLedPpp */
    BP_GPIO_2_AH,                           /* usGpioLedPppFail */
    BP_GPIO_1_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_0_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	  Slic_Not_Defined,                       /* SLIC type */
	  DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96338e420p2 =
{
    "96338E4-20P2",                         /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_3_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
    DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96332cg =
{
    "96332CG",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_4_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_NOT_DEFINED,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
    DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96338gws =
{
    "96338WS",                               /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_5_AL,                           /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_0,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    BP_WLAN_MAC_ADDR_OVERRIDE,              /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */   
    BP_NOT_DEFINED,                           /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96338e_e7r =
{
    "96338E_E7R",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_2_AL,                           /* usGpioLedWanData */
    BP_GPIO_1_AL,                           /* usGpioLedPpp */
    BP_NOT_DEFINED,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	  Slic_Not_Defined,                       /* SLIC type */
	  DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96338gw2_e7t =
{
    "96338W2_E7T",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_5_AL,                           /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_2_AL,                           /* usGpioLedWanData */
    BP_GPIO_1_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_0,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    BP_WLAN_MAC_ADDR_OVERRIDE,              /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
    DAA_Not_Defined,                        /* DAA type */
    BP_GPIO_6_AH,                           /* usGpioWirelessPowerDown */    	  
};

static BOARD_PARAMETERS g_bcm96332cg_f4w =
{
    "96332CG_F4W",                              /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_4_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_NOT_DEFINED,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_6_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
    DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static PBOARD_PARAMETERS g_BoardParms[] =
    {&g_bcm96338sv, &g_bcm96338l2m8m, &g_bcm96338l652m8m, &g_bcm96338gw, &g_bcm96338gws2,
     &g_bcm96338gw20p2, &g_bcm96338gw2, &g_bcm96338e, &g_bcm96338e4, &g_bcm96338e420p2, &g_bcm96332cg, 
     &g_bcm96338gws, &g_bcm96338e_e7r, &g_bcm96338gw2_e7t,&g_bcm96332cg_f4w, 0};
#endif

#if defined(_BCM96348_) || defined(CONFIG_BCM96348)

static BOARD_PARAMETERS g_bcm96348r =
{
    "96348R",                               /* szBoardId */
    BP_NOT_DEFINED,                         /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */    	
};

static BOARD_PARAMETERS g_bcm96348gw =
{
    "96348GW",                              /* szBoardId */
    (BP_PCI | BP_MII2),                     /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x00,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_SPI_SSB_0,             /* usConfigType */
      BP_ENET_REVERSE_MII,                  /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_DSP,                          /* ucDspType */
      0x00,                                 /* ucDspAddress */
      BP_EXT_INTR_2,                        /* usExtIntrVoip */
      BP_GPIO_6_AH,                         /* usGpioVoipReset */
      BP_GPIO_34_AH,                        /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_CS_2},                             /* usCsVoip */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96348w2 =
{
    "96348W2",                              /* szBoardId */
    (BP_PCI | BP_MII2),                     /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_6_AL,                           /* usGpioLedSesWireless */
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_AUX,                        /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x00,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96348w3_10 =
{
    "96348W3-10",                           /* szBoardId */
    (BP_PCI | BP_MII2),                     /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_2_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_6_AL,                           /* usGpioLedSesWireless */
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_4_AH,                           /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_AUX,                        /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x00,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
    Slic_Not_Defined,                       /* SLIC type */
     DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96348gw_10 =
{
    "96348GW-10",                           /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_MII2),     /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x00,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_SPI_SSB_1,             /* usConfigType */
      BP_ENET_REVERSE_MII,                  /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_DSP,                          /* ucDspType */
      0x00,                                 /* ucDspAddress */
      BP_EXT_INTR_2,                        /* usExtIntrVoip */
      BP_GPIO_6_AH,                         /* usGpioVoipReset */
      BP_GPIO_34_AH,                        /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_CS_2},                             /* usCsVoip */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96348gw_11 =
{
    "96348GW-11",                           /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_MII2),     /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_NO_PHY},                      /* ucPhyType */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x00,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_SPI_SSB_1,             /* usConfigType */
      BP_ENET_REVERSE_MII,                  /* usReverseMii */
      SW_INFO_DEFAULT(4)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96348sv =
{
    "96348SV",                              /* szBoardId */
    (BP_PCI | BP_CB | BP_MII2),             /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_NOT_DEFINED,                         /* usGpioLedPppFail */
    BP_NOT_DEFINED,                         /* usGpioLedBlPowerOn */
    BP_NOT_DEFINED,                         /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_PHY,                 /* ucPhyType */
      0x1f,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcmCustom_01 =
{
     "BCMCUST_01",                          /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_MII2),     /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_2_AL,                           /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_3_AL,                           /* usGpioLedWanData */
    BP_GPIO_3_AL,                           /* usGpioLedPpp */
    BP_GPIO_4_AL,                           /* usGpioLedPppFail */
    BP_GPIO_0_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_NOT_DEFINED,                         /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

     {{BP_ENET_INTERNAL_PHY,                /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY,                       /* ucPhyType */
      0x00,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_SPI_SSB_1,             /* usConfigType */
      BP_ENET_REVERSE_MII,                  /* usReverseMii */
      SW_INFO_DEFAULT(1)}},                 /* sw */
    {{BP_VOIP_DSP,                          /* ucDspType */
      0x00,                                 /* ucDspAddress */
      BP_EXT_INTR_2,                        /* usExtIntrVoip */
      BP_GPIO_36_AH,                        /* usGpioVoipReset */
      BP_GPIO_34_AL,                        /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_CS_2},                             /* usCsVoip */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96348m =
{
    "96348M",                               /* szBoardId */
    (BP_UTOPIA),                            /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_GPIO_22_AL,                          /* usVcopeResetGpio */
    BP_GPIO_21_AH,                          /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_19_AL,                          /* usGpioLedWanData */
    BP_GPIO_19_AL,                          /* usGpioLedPpp */
    BP_GPIO_20_AL,                          /* usGpioLedPppFail */
    BP_GPIO_16_AL,                          /* usGpioLedBlPowerOn */
    BP_GPIO_17_AL,                          /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_1,                          /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_CS_1,                                /* usCsHpi */

    BP_NOT_DEFINED,                         /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    3,                                      /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static PBOARD_PARAMETERS g_BoardParms[] =
    {&g_bcm96348sv, &g_bcm96348r, &g_bcm96348gw, &g_bcm96348gw_10,
     &g_bcm96348gw_11, &g_bcm96348w2, &g_bcm96348w3_10,
     &g_bcmCustom_01, &g_bcm96348m, 0};
#endif

#if defined(_BCM96358_) || defined(CONFIG_BCM96358)

static BOARD_PARAMETERS g_bcm96358vw2 =
{
    "96358VW2",                             /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_LED),      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_22_AL,                          /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_27_AL,                    /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_23_AH,                          /* usGpioLedPppFail */
    BP_GPIO_5_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_4_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_2,                          /* usExtIntrResetToDefault */
    BP_GPIO_37_AL,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_GPIO_0_AL,                        /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_GPIO_7_AL,                  /* usGpioVoip1Led */   /*dare yxy for FXS change*/
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_GPIO_10_AH,                        /* usGpioSlic0Relay */
      BP_GPIO_12_AH,                        /* usGpioSlic0Reset */
      BP_GPIO_24_AL,                  /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_GPIO_11_AH,                        /* usGpioSlic1Relay */
      BP_GPIO_12_AH,                        /* usGpioSlic1Reset */
      BP_GPIO_6_AL,                  /* usGpioPotsLed */   
      BP_GPIO_13_AH,                        /* usGpioPotsReset */      
      BP_NOT_DEFINED},                      /* usGpioDectLed */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                         /* SLIC type */
	Legerity_88010,                         /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
	    BP_GPIO_30_AL,                  /* usGpioLedUsbHost1 */     
	    BP_GPIO_31_AL,                  /* usGpioLedUsbHost2 */     
	    BP_GPIO_29_AH,                        /* usGpioWirelessProtect */      
};

static BOARD_PARAMETERS g_bcm96358vw =
{
    "96358VW",                              /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_LED),      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_15_AL,                          /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_SERIAL_GPIO_7_AL,                    /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_22_AL,                          /* usGpioLedWanData */
    BP_GPIO_22_AL,                          /* usGpioLedPpp */
    BP_GPIO_23_AL,                          /* usGpioLedPppFail */
    BP_GPIO_4_AL,                           /* usGpioLedBlPowerOn */
    BP_GPIO_5_AL,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_0,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_SERIAL_GPIO_0_AL,                  /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */  
      BP_GPIO_10_AH,                        /* usGpioSlic0Relay */
      BP_GPIO_12_AH,                        /* usGpioSlic0Reset */
      BP_SERIAL_GPIO_2_AL,                  /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */  
      BP_GPIO_11_AH,                        /* usGpioSlic1Relay */
      BP_GPIO_24_AH,                        /* usGpioSlic1Reset */
      BP_SERIAL_GPIO_4_AL,                  /* usGpioPotsLed */   
      BP_GPIO_13_AH,                        /* usGpioPotsReset */
      BP_NOT_DEFINED},                      /* usGpioDectLed */ 
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Silicon_Labs_3215,                      /* SLIC type */
	Silicon_Labs_3050,                      /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96358sv =
{
    "96358SV",                              /* szBoardId */
    (BP_PCI | BP_CB | BP_SPI_EXT_CS | BP_UART1 | BP_LED), /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_GPIO_15_AL,                          /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_SERIAL_GPIO_7_AL,                    /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_NOT_DEFINED,                         /* usGpioLedPppFail */
    BP_NOT_DEFINED,                         /* usGpioLedBlPowerOn */
    BP_NOT_DEFINED,                         /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96358gwvg =
{
    "96358GWVG",                            /* szBoardId */
    (BP_PCI | BP_CB | BP_SPI_EXT_CS | BP_LED), /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_11_AL,                          /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_6_AL,                           /* usGpioLedSesWireless */ 
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_8_AH,                           /* usGpioLedPppFail */
    BP_GPIO_4_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_5_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_GPIO_7_AL,                         /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_NOT_DEFINED ,                      /* usGpioSlic0Relay */
      BP_GPIO_12_AL,                        /* usGpioSlic0Reset */
      BP_NOT_DEFINED,                       /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_NOT_DEFINED,                       /* usGpioSlic1Relay */
      BP_NOT_DEFINED,                       /* usGpioSlic1Reset */
      BP_NOT_DEFINED,                       /* usGpioPotsLed */   
      BP_NOT_DEFINED,                       /* usGpioPotsReset */
      BP_NOT_DEFINED},                      /* usGpioDectLed */      
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                         /* SLIC type */
	Legerity_88010,                         /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96358mv2g =
{
    "96358MV2G",                            /* szBoardId */
    (BP_PCI | BP_UTOPIA | BP_SPI_EXT_CS | BP_LED), /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_GPIO_9_AL,                           /* usVcopeResetGpio */
    BP_GPIO_10_AH,                          /* usVcopeRelayGpio */

    BP_SERIAL_GPIO_3_AL,                    /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_SERIAL_GPIO_2_AH,                    /* usGpioLedPppFail */
    BP_SERIAL_GPIO_0_AH,                    /* usGpioLedBlPowerOn */
    BP_SERIAL_GPIO_1_AH,                    /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_2,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_CS_1,                                /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    3,                                      /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_GPIO_8_AL,                         /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_SERIAL_GPIO_5_AL,                  /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_GPIO_4_AL ,                        /* usGpioSlic0Relay */
      BP_GPIO_35_AL,                        /* usGpioSlic0Reset */
      BP_SERIAL_GPIO_6_AL,                  /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_GPIO_5_AL,                         /* usGpioSlic1Relay */
      BP_GPIO_35_AL,                        /* usGpioSlic1Reset */
      BP_NOT_DEFINED,                       /* usGpioPotsLed */   
      BP_NOT_DEFINED,                       /* usGpioPotsReset */
      BP_NOT_DEFINED},                      /* usGpioDectLed */      
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                         /* SLIC type */
	Legerity_88010,                         /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96358m =
{
    "96358M",                               /* szBoardId */
    (BP_PCI | BP_UTOPIA | BP_LED),          /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_GPIO_9_AL,                           /* usVcopeResetGpio */
    BP_GPIO_10_AH,                          /* usVcopeRelayGpio */

    BP_SERIAL_GPIO_3_AL,                    /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_GPIO_11_AL,                          /* usGpioLedSesWireless */ 
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_SERIAL_GPIO_2_AH,                    /* usGpioLedPppFail */
    BP_SERIAL_GPIO_0_AH,                    /* usGpioLedBlPowerOn */
    BP_SERIAL_GPIO_1_AH,                    /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_CS_1,                                /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    3,                                      /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
    BP_SERIAL_GPIO_5_AH,                  /* usGpioLedUsbHost1 */     
    BP_SERIAL_GPIO_6_AH,                  /* usGpioLedUsbHost2 */     
};

static BOARD_PARAMETERS g_bcm96358b =
{
    "96358B",                               /* szBoardId */
    (BP_PCI | BP_LED),                      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_SERIAL_GPIO_2_AL,                    /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_SERIAL_GPIO_3_AL,                    /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_SERIAL_GPIO_4_AL,                    /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                          /* usGpioLedWanData */
    BP_NOT_DEFINED,                          /* usGpioLedPpp */
    BP_SERIAL_GPIO_6_AH,                    /* usGpioLedPppFail */
    BP_SERIAL_GPIO_0_AH,                    /* usGpioLedBlPowerOn */
    BP_SERIAL_GPIO_1_AH,                    /* usGpioLedBlStop */
    BP_GPIO_5_AH,                           /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_5,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_4,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_SERIAL_GPIO_5_AL,                  /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96358bgweOem1 =
{
    "96358BGWE_OEM1",                       /* szBoardId */
    (BP_PCI | BP_LED),                      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_SERIAL_GPIO_2_AL,                    /* usGpioLedAdsl */
    BP_SERIAL_GPIO_1_AL,                    /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_SERIAL_GPIO_7_AL,                    /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_NOT_DEFINED,                          /* usGpioLedPppFail */
    BP_NOT_DEFINED,                           /* usGpioLedBlPowerOn */
    BP_NOT_DEFINED,                         /* usGpioLedBlStop */
    BP_GPIO_5_AH,                           /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_GPIO_33_AL,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(4)}},                 /* sw */
    {{BP_VOIP_NO_DSP},                      /* ucDspType */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Slic_Not_Defined,                       /* SLIC type */
	DAA_Not_Defined,                        /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96359pong =
{
    "96359PONG",                            /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS),               /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    /* TBD. BP_SERIAL_GPIO_2_AH, BP_SERIAL_GPIO_4_AH and BP_SERIAL_GPIO_5_AH
     * LEDs do not belong in the fields that they are currently assigned.  New
     * board parameter functions are needed.  They are assigned to these fields
     * so that they are turned off during initialization. They should be removed
     * from these fields after the final implementation is finished.
     */
    BP_NOT_DEFINED,                         /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */ 
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_GPIO_2_AH,                           /* usGpioLedWanData */
    BP_GPIO_2_AH,                           /* usGpioLedPpp */
    BP_GPIO_8_AH,                           /* usGpioLedPppFail */
    BP_GPIO_4_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_26_AH,                          /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_NOT_DEFINED,                         /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_2,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(5)}},                 /* sw */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_SERIAL_GPIO_1_AH,                  /* usGpioVoip1Led */
      BP_SERIAL_GPIO_2_AH,                  /* usGpioVoip1LedFail */
      BP_NOT_DEFINED ,                      /* usGpioSlic0Relay */
      BP_GPIO_12_AL,                        /* usGpioSlic0Reset */
      BP_SERIAL_GPIO_3_AH,                  /* usGpioVoip2Led */
      BP_SERIAL_GPIO_4_AH,                  /* usGpioVoip2LedFail */
      BP_NOT_DEFINED,                       /* usGpioSlic1Relay */
      BP_NOT_DEFINED,                       /* usGpioSlic1Reset */
      BP_NOT_DEFINED,                       /* usGpioPotsLed */   
      BP_NOT_DEFINED,                       /* usGpioPotsReset */
      BP_SERIAL_GPIO_5_AH},                 /* usGpioDectLed */      
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                         /* SLIC type */
	DAA_Not_Defined,                         /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96358ivwg =
{
    "96358IVWG",                            /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_LED),      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_22_AL,                          /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_SERIAL_GPIO_7_AL,                    /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_23_AH,                          /* usGpioLedPppFail */
    BP_GPIO_4_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_5_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_EXT_INTR_2,                          /* usExtIntrResetToDefault */
    BP_EXT_INTR_3,                          /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    0,                                      /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_INTERNAL_PHY,                 /* ucPhyType */
      0x01,                                 /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_GPIO_28_AL,                        /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(1)},                  /* sw */
     {BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MDIO_PSEUDO_PHY,       /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      SW_INFO_DEFAULT(3)}},                 /* sw */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_SERIAL_GPIO_0_AL,                  /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_GPIO_10_AH,                        /* usGpioSlic0Relay */
      BP_GPIO_12_AH,                        /* usGpioSlic0Reset */
      BP_SERIAL_GPIO_2_AL,                  /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_GPIO_11_AH,                        /* usGpioSlic1Relay */
      BP_GPIO_12_AH,                        /* usGpioSlic1Reset */
      BP_SERIAL_GPIO_4_AL,                  /* usGpioPotsLed */   
      BP_GPIO_13_AH,                        /* usGpioPotsReset */      
      BP_NOT_DEFINED},                      /* usGpioDectLed */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                         /* SLIC type */
	Legerity_88010,                         /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static PBOARD_PARAMETERS g_BoardParms[] =
    {&g_bcm96358vw2, &g_bcm96358vw, &g_bcm96358gwvg, &g_bcm96358mv2g,
     &g_bcm96358m,  &g_bcm96358b, &g_bcm96358bgweOem1, &g_bcm96359pong,
     &g_bcm96358ivwg, &g_bcm96358sv, 0};
#endif

#if defined(_BCM96368_) || defined(CONFIG_BCM96368)

static BOARD_PARAMETERS g_bcm96368vvw =
{
    "96368VVW",                             /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_LED),      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_2_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_33_AH,                          /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MMAP,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      {0x0f, {0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00}}},  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_GPIO_25_AL,                        /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_GPIO_3_AH,                         /* usGpioSlic0Relay */
      BP_GPIO_15_AH,                        /* usGpioSlic0Reset */
      BP_GPIO_26_AL,                        /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_GPIO_4_AH,                         /* usGpioSlic1Relay */
      BP_GPIO_15_AH,                        /* usGpioSlic1Reset */
      BP_GPIO_27_AL,                        /* usGpioPotsLed */   
      BP_GPIO_22_AH,                        /* usGpioPotsReset */      
      BP_NOT_DEFINED},                      /* usGpioDectLed */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                          /* SLIC type */
	Legerity_88010,                          /* DAA type */
    BP_NOT_DEFINED,                         /* usGpioWirelessPowerDown */
};

static BOARD_PARAMETERS g_bcm96368sv2 =
{
    "96368SV2",                             /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_LED),      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_2_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_33_AH,                          /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MMAP,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      {0x3f, {0x01, 0x02, 0x03, 0x04, 0x12, 0x11, 0x00, 0x00}}},  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_GPIO_25_AL,                        /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_GPIO_3_AH,                         /* usGpioSlic0Relay */
      BP_GPIO_15_AH,                        /* usGpioSlic0Reset */
      BP_GPIO_26_AL,                        /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_GPIO_4_AH,                         /* usGpioSlic1Relay */
      BP_GPIO_15_AH,                        /* usGpioSlic1Reset */
      BP_GPIO_27_AL,                        /* usGpioPotsLed */   
      BP_GPIO_22_AH,                        /* usGpioPotsReset */      
      BP_NOT_DEFINED},                      /* usGpioDectLed */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                          /* SLIC type */
	Legerity_88010,                          /* DAA type */
};

static BOARD_PARAMETERS g_bcm96369pvg =
{
    "96369PVG",                             /* szBoardId */
    (BP_PCI | BP_SPI_EXT_CS | BP_LED),      /* usGPIOOverlay */

    BP_NOT_DEFINED,                         /* usGpioRj11InnerPair */
    BP_NOT_DEFINED,                         /* usGpioRj11OuterPair */
    BP_NOT_DEFINED,                         /* usGpioUartRts */
    BP_NOT_DEFINED,                         /* usGpioUartCts */
    BP_NOT_DEFINED,                         /* usVcopeResetGpio */
    BP_NOT_DEFINED,                         /* usVcopeRelayGpio */

    BP_GPIO_2_AL,                           /* usGpioLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdsl */
    BP_NOT_DEFINED,                         /* usGpioSecLedAdslFail */
    BP_NOT_DEFINED,                         /* usGpioLedSesWireless */     
    BP_NOT_DEFINED,                         /* usGpioLedHpna */
    BP_NOT_DEFINED,                         /* usGpioLedWanData */
    BP_NOT_DEFINED,                         /* usGpioLedPpp */
    BP_GPIO_33_AH,                          /* usGpioLedPppFail */
    BP_GPIO_0_AH,                           /* usGpioLedBlPowerOn */
    BP_GPIO_1_AH,                           /* usGpioLedBlStop */
    BP_NOT_DEFINED,                         /* usGpioBondingSecondaryReset */

    BP_HW_DEFINED,                          /* usExtIntrAdslDyingGasp */
    BP_NOT_DEFINED,                         /* usExtIntrResetToDefault */
    BP_NOT_DEFINED,                         /* usExtIntrSesBtnWireless */
    BP_NOT_DEFINED,                         /* usExtIntrHpna */

    BP_NOT_DEFINED,                         /* usCsHpna */
    BP_NOT_DEFINED,                         /* usCsHpi */

    BP_WLAN_ANT_MAIN,                       /* usAntInUseWireless */
    BP_NOT_DEFINED,                         /* WLAN flags */

    BP_NOT_DEFINED,                         /* usVcopeBoardRev */

    {{BP_ENET_EXTERNAL_SWITCH,              /* ucPhyType */
      0x0,                                  /* ucPhyAddress */
      BP_NOT_DEFINED,                       /* usGpioPhyReset */
      BP_NOT_DEFINED,                       /* usGpioPhyLinkSpeed */
      BP_ENET_CONFIG_MMAP,                  /* usConfigType */
      BP_NOT_DEFINED,                       /* usReverseMii */
      {0x17, {0x01, 0x02, 0x03, 0x00, 0x10, 0x00, 0x00, 0x00}}},  /* sw */
     {BP_ENET_NO_PHY}},                     /* ucPhyType */
    {{BP_VOIP_MIPS,                         /* ucDspType */  
      -1,                                   /* ucDspAddress */
      BP_NOT_DEFINED,                       /* usExtIntrVoip */
      BP_NOT_DEFINED,                       /* usGpioVoipReset */
      BP_NOT_DEFINED,                       /* usGpioVoipIntr */
      BP_NOT_DEFINED,                       /* usGpioLedVoip */
      BP_NOT_DEFINED,                       /* usCsVoip */         
      BP_GPIO_25_AL,                        /* usGpioVoip1Led */
      BP_NOT_DEFINED,                       /* usGpioVoip1LedFail */
      BP_GPIO_3_AH,                         /* usGpioSlic0Relay */
      BP_GPIO_15_AH,                        /* usGpioSlic0Reset */
      BP_GPIO_26_AL,                        /* usGpioVoip2Led */
      BP_NOT_DEFINED,                       /* usGpioVoip2LedFail */
      BP_GPIO_4_AH,                         /* usGpioSlic1Relay */
      BP_GPIO_15_AH,                        /* usGpioSlic1Reset */
      BP_GPIO_27_AL,                        /* usGpioPotsLed */   
      BP_GPIO_22_AH,                        /* usGpioPotsReset */      
      BP_NOT_DEFINED},                      /* usGpioDectLed */
     {BP_VOIP_NO_DSP}},                     /* ucDspType */
	Legerity_88221,                          /* SLIC type */
	Legerity_88010,                          /* DAA type */
};

static PBOARD_PARAMETERS g_BoardParms[] =
    {&g_bcm96368vvw, &g_bcm96368sv2, &g_bcm96369pvg, 0};
#endif

static PBOARD_PARAMETERS g_pCurrentBp = 0;

/**************************************************************************
 * Name       : bpstrcmp
 *
 * Description: String compare for this file so it does not depend on an OS.
 *              (Linux kernel and CFE share this source file.)
 *
 * Parameters : [IN] dest - destination string
 *              [IN] src - source string
 *
 * Returns    : -1 - dest < src, 1 - dest > src, 0 dest == src
 ***************************************************************************/
static int bpstrcmp(const char *dest,const char *src);
static int bpstrcmp(const char *dest,const char *src)
{
    while (*src && *dest)
    {
        if (*dest < *src) return -1;
        if (*dest > *src) return 1;
        dest++;
        src++;
    }

    if (*dest && !*src) return 1;
    if (!*dest && *src) return -1;
    return 0;
} /* bpstrcmp */

/**************************************************************************
 * Name       : BpGetVoipDspConfig
 *
 * Description: Gets the DSP configuration from the board parameter
 *              structure for a given DSP index.
 *
 * Parameters : [IN] dspNum - DSP index (number)
 *
 * Returns    : Pointer to DSP configuration block if found/valid, NULL
 *              otherwise.
 ***************************************************************************/
VOIP_DSP_INFO *BpGetVoipDspConfig( unsigned char dspNum );
VOIP_DSP_INFO *BpGetVoipDspConfig( unsigned char dspNum )
{
    VOIP_DSP_INFO *pDspConfig = 0;
    int i;

    if( g_pCurrentBp )
    {
        for( i = 0 ; i < BP_MAX_VOIP_DSP ; i++ )
        {
            if( g_pCurrentBp->VoIPDspInfo[i].ucDspType != BP_VOIP_NO_DSP &&
                g_pCurrentBp->VoIPDspInfo[i].ucDspAddress == dspNum )
            {
                pDspConfig = &g_pCurrentBp->VoIPDspInfo[i];
                break;
            }
        }
    }

    return pDspConfig;
}


/**************************************************************************
 * Name       : BpSetBoardId
 *
 * Description: This function find the BOARD_PARAMETERS structure for the
 *              specified board id string and assigns it to a global, static
 *              variable.
 *
 * Parameters : [IN] pszBoardId - Board id string that is saved into NVRAM.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_FOUND - Error, board id input string does not
 *                  have a board parameters configuration record.
 ***************************************************************************/
int BpSetBoardId( char *pszBoardId )
{
    int nRet = BP_BOARD_ID_NOT_FOUND;
    PBOARD_PARAMETERS *ppBp;

    for( ppBp = g_BoardParms; *ppBp; ppBp++ )
    {
        if( !bpstrcmp((*ppBp)->szBoardId, pszBoardId) )
        {
            g_pCurrentBp = *ppBp;
            nRet = BP_SUCCESS;
            break;
        }
    }

    return( nRet );
} /* BpSetBoardId */

/**************************************************************************
 * Name       : BpGetBoardId
 *
 * Description: This function returns the current board id strings.
 *
 * Parameters : [OUT] pszBoardIds - Address of a buffer that the board id
 *                  string is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 ***************************************************************************/

int BpGetBoardId( char *pszBoardId )
{
    int i;

    if (g_pCurrentBp == 0)
        return -1;
    
    for (i = 0; i < BP_BOARD_ID_LEN; i++)
      pszBoardId[i] = g_pCurrentBp->szBoardId[i];

    return 0;
}

/**************************************************************************
 * Name       : BpGetBoardIds
 *
 * Description: This function returns all of the supported board id strings.
 *
 * Parameters : [OUT] pszBoardIds - Address of a buffer that the board id
 *                  strings are returned in.  Each id starts at BP_BOARD_ID_LEN
 *                  boundary.
 *              [IN] nBoardIdsSize - Number of BP_BOARD_ID_LEN elements that
 *                  were allocated in pszBoardIds.
 *
 * Returns    : Number of board id strings returned.
 ***************************************************************************/
int BpGetBoardIds( char *pszBoardIds, int nBoardIdsSize )
{
    PBOARD_PARAMETERS *ppBp;
    int i;
    char *src;
    char *dest;

    for( i = 0, ppBp = g_BoardParms; *ppBp && nBoardIdsSize;
        i++, ppBp++, nBoardIdsSize--, pszBoardIds += BP_BOARD_ID_LEN )
    {
        dest = pszBoardIds;
        src = (*ppBp)->szBoardId;
        while( *src )
            *dest++ = *src++;
        *dest = '\0';
    }

    return( i );
} /* BpGetBoardIds */

/**************************************************************************
 * Name       : BpGetGPIOverlays
 *
 * Description: This function GPIO overlay configuration
 *
 * Parameters : [OUT] pusValue - Address of short word that interfaces in use.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetGPIOverlays( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGPIOOverlay;

        if( g_pCurrentBp->usGPIOOverlay != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );	
} /* BpGetGPIOverlays */

/**************************************************************************
 * Name       : BpGetEthernetMacInfo
 *
 * Description: This function returns all of the supported board id strings.
 *
 * Parameters : [OUT] pEnetInfos - Address of an array of ETHERNET_MAC_INFO
 *                  buffers.
 *              [IN] nNumEnetInfos - Number of ETHERNET_MAC_INFO elements that
 *                  are pointed to by pEnetInfos.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 ***************************************************************************/
int BpGetEthernetMacInfo( PETHERNET_MAC_INFO pEnetInfos, int nNumEnetInfos )
{
    int i, nRet;

    if( g_pCurrentBp )
    {
        for( i = 0; i < nNumEnetInfos; i++, pEnetInfos++ )
        {
            if( i < BP_MAX_ENET_MACS )
            {
                unsigned char *src = (unsigned char *)
                    &g_pCurrentBp->EnetMacInfos[i];
                unsigned char *dest = (unsigned char *) pEnetInfos;
                int len = sizeof(ETHERNET_MAC_INFO);
                while( len-- )
                    *dest++ = *src++;
            }
            else
                pEnetInfos->ucPhyType = BP_ENET_NO_PHY;
        }

        nRet = BP_SUCCESS;
    }
    else
    {
        for( i = 0; i < nNumEnetInfos; i++, pEnetInfos++ )
            pEnetInfos->ucPhyType = BP_ENET_NO_PHY;

        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetEthernetMacInfo */

/**************************************************************************
 * Name       : BpGetRj11InnerOuterPairGpios
 *
 * Description: This function returns the GPIO pin assignments for changing
 *              between the RJ11 inner pair and RJ11 outer pair.
 *
 * Parameters : [OUT] pusInner - Address of short word that the RJ11 inner pair
 *                  GPIO pin is returned in.
 *              [OUT] pusOuter - Address of short word that the RJ11 outer pair
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, values are returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetRj11InnerOuterPairGpios( unsigned short *pusInner,
    unsigned short *pusOuter )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusInner = g_pCurrentBp->usGpioRj11InnerPair;
        *pusOuter = g_pCurrentBp->usGpioRj11OuterPair;

        if( g_pCurrentBp->usGpioRj11InnerPair != BP_NOT_DEFINED &&
            g_pCurrentBp->usGpioRj11OuterPair != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusInner = *pusOuter = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetRj11InnerOuterPairGpios */

/**************************************************************************
 * Name       : BpGetVoipResetGpio
 *
 * Description: This function returns the GPIO pin assignment for the VOIP
 *              Reset operation.
 *
 * Parameters : [OUT] pusValue - Address of short word that the VOIP reset
 *                  GPIO pin is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipResetGpio( unsigned char dspNum, unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = BpGetVoipDspConfig( dspNum );

        if( pDspInfo )
        {
           *pusValue = pDspInfo->usGpioVoipReset;

           if( *pusValue != BP_NOT_DEFINED ||
               *pusValue == BP_UNEQUIPPED )
           {
              nRet = BP_SUCCESS;
           }
           else
           {
              nRet = BP_VALUE_NOT_DEFINED;
           }
        }
        else
        {
           *pusValue = BP_NOT_DEFINED;
           nRet = BP_BOARD_ID_NOT_FOUND;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoipResetGpio */

/**************************************************************************
 * Name       : BpGetVoipIntrGpio
 *
 * Description: This function returns the GPIO pin assignment for VoIP interrupt.
 *
 * Parameters : [OUT] pusValue - Address of short word that the VOIP interrupt
 *                  GPIO pin is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipIntrGpio( unsigned char dspNum, unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = BpGetVoipDspConfig( dspNum );

        if( pDspInfo )
        {
           *pusValue = pDspInfo->usGpioVoipIntr;

           if( *pusValue != BP_NOT_DEFINED )
           {
              nRet = BP_SUCCESS;
           }
           else
           {
              nRet = BP_VALUE_NOT_DEFINED;
           }
        }
        else
        {
           *pusValue = BP_NOT_DEFINED;
           nRet = BP_BOARD_ID_NOT_FOUND;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoipIntrGpio */

/**************************************************************************
 * Name       : BpGetUartRtsCtsGpios
 *
 * Description: This function returns the GPIO pin assignments for RTS and CTS
 *              UART signals.
 *
 * Parameters : [OUT] pusRts - Address of short word that the UART RTS GPIO
 *                  pin is returned in.
 *              [OUT] pusCts - Address of short word that the UART CTS GPIO
 *                  pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, values are returned.
 *              BP_BOARD_ID_NOT_SET - Error, board id input string does not
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetRtsCtsUartGpios( unsigned short *pusRts, unsigned short *pusCts )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusRts = g_pCurrentBp->usGpioUartRts;
        *pusCts = g_pCurrentBp->usGpioUartCts;

        if( g_pCurrentBp->usGpioUartRts != BP_NOT_DEFINED &&
            g_pCurrentBp->usGpioUartCts != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusRts = *pusCts = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetUartRtsCtsGpios */

/**************************************************************************
 * Name       : BpGetAdslLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the ADSL
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the ADSL LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetAdslLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedAdsl;

        if( g_pCurrentBp->usGpioLedAdsl != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetAdslLedGpio */

/**************************************************************************
 * Name       : BpGetAdslFailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the ADSL
 *              LED that is used when there is a DSL connection failure.
 *
 * Parameters : [OUT] pusValue - Address of short word that the ADSL LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetAdslFailLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedAdslFail;

        if( g_pCurrentBp->usGpioLedAdslFail != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetAdslFailLedGpio */

/**************************************************************************
 * Name       : BpGetSecAdslLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the ADSL
 *              LED of the Secondary line, applicable more for bonding.
 *
 * Parameters : [OUT] pusValue - Address of short word that the ADSL LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetSecAdslLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioSecLedAdsl;

        if( g_pCurrentBp->usGpioSecLedAdsl != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetSecAdslLedGpio */

/**************************************************************************
 * Name       : BpGetSecAdslFailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the ADSL
 *              LED of the Secondary ADSL line, that is used when there is 
 *              a DSL connection failure.
 *
 * Parameters : [OUT] pusValue - Address of short word that the ADSL LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetSecAdslFailLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioSecLedAdslFail;

        if( g_pCurrentBp->usGpioSecLedAdslFail != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetSecAdslFailLedGpio */

/**************************************************************************
 * Name       : BpGetWirelessAntInUse
 *
 * Description: This function returns the antennas in use for wireless
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Antenna
 *                  is in use.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessAntInUse( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usAntInUseWireless;

        if( g_pCurrentBp->usAntInUseWireless != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );	
} /* BpGetWirelessAntInUse */

/**************************************************************************
 * Name       : BpGetWirelessFlags
 *
 * Description: This function returns optional control flags for wireless
 *
 * Parameters : [OUT] pusValue - Address of short word control flags
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessFlags( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usWirelessFlags;

        if( g_pCurrentBp->usWirelessFlags != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );	
} /* BpGetWirelessAntInUse */

/**************************************************************************
 * Name       : BpGetWirelessSesExtIntr
 *
 * Description: This function returns the external interrupt number for the 
 *              Wireless Ses Button.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Ses
 *                  external interrup is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessSesExtIntr( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usExtIntrSesBtnWireless;

        if( g_pCurrentBp->usExtIntrSesBtnWireless != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );	
		
} /* BpGetWirelessSesExtIntr */

/**************************************************************************
 * Name       : BpGetWirelessSesLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the Wireless
 *              Ses Led.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Wireless Ses
 *                  Led GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessSesLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedSesWireless;

        if( g_pCurrentBp->usGpioLedSesWireless != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
	
} /* BpGetWirelessSesLedGpio */

/**************************************************************************
 * Name       : BpUpdateWirelessSromMap
 *
 * Description: This function patch wireless PA values
 *
 * Parameters : [IN] unsigned short chipID
 *              [IN/OUT] unsigned short* pBase - base of srom map
 *              [IN/OUT] int size - size of srom map
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpUpdateWirelessSromMap(unsigned short chipID, unsigned short* pBase, int sizeInWords)
{
    int nRet = BP_BOARD_ID_NOT_FOUND;
    int i = 0;
    int j = 0;
    
    /* this data structure could be moved to boardparams structure in the future */
    /* does not require to rebuild cfe here if more srom entries are needed */
    WLAN_SROM_PATCH_INFO wlanPaInfo[]={{"96348W3-10", 0x4318, 49, {{"pa0b0", 47, 0x14ae},
                                                                   {"pa0b1", 48, 0xfad4},
                                                                   {"pa0b2", 49, 0xfe82},
                                                                   {"",       0,      0}}},
                                                                   
                                       {"96348W3",    0x4318, 49, {{"pa0b0", 47, 0x14ae},
                                                                   {"pa0b1", 48, 0xfad4},
                                                                   {"pa0b2", 49, 0xfe82},
                                                                   {"",       0,      0}}},
                                       {"",                 0, 0, {{"",       0,      0}}}, /* last entry*/
                                       };
                                
    if(chipID == 0 || pBase == 0 || sizeInWords <= 0 )
        return nRet;
        
    i = 0;
    while ( wlanPaInfo[i].szboardId[0] != 0 ) {
    	/* check boardId */
        if ( !bpstrcmp(g_pCurrentBp->szBoardId, wlanPaInfo[i].szboardId) ) {
            /* check chipId */
            if ( (wlanPaInfo[i].usWirelessChipId == chipID) && (wlanPaInfo[i].usNeededSize <= sizeInWords) ){
                /* valid , patch entry */
            	while ( wlanPaInfo[i].entries[j].name[0] ) {
	            pBase[wlanPaInfo[i].entries[j].wordOffset] = wlanPaInfo[i].entries[j].value;
	            j++;
            	}
	        nRet = BP_SUCCESS;            	
                goto srom_update_done;
            }
        }
        i++;
    }

srom_update_done:
    
    return( nRet );
	
} /* BpUpdateWirelessSromMap */

/**************************************************************************
 * Name       : BpGetHpnaLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the HPNA
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the HPNA LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetHpnaLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedHpna;

        if( g_pCurrentBp->usGpioLedHpna != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetHpnaLedGpio */

/**************************************************************************
 * Name       : BpGetWanDataLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the WAN Data
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the WAN Data LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWanDataLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedWanData;

        if( g_pCurrentBp->usGpioLedWanData != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetWanDataLedGpio */

/**************************************************************************
 * Name       : BpGetPppLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the PPP
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the PPP LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPppLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedPpp;

        if( g_pCurrentBp->usGpioLedPpp != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetPppLedGpio */

/**************************************************************************
 * Name       : BpGetPppFailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the PPP
 *              LED that is used when there is a PPP connection failure.
 *
 * Parameters : [OUT] pusValue - Address of short word that the PPP LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPppFailLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedPppFail;

        if( g_pCurrentBp->usGpioLedPppFail != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetPppFailLedGpio */

/**************************************************************************
 * Name       : BpGetBootloaderPowerOnLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the power
 *              on LED that is set by the bootloader.
 *
 * Parameters : [OUT] pusValue - Address of short word that the alarm LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBootloaderPowerOnLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedBlPowerOn;

        if( g_pCurrentBp->usGpioLedBlPowerOn != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetBootloaderPowerOn */

/**************************************************************************
 * Name       : BpGetBootloaderStopLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the break
 *              into bootloader LED that is set by the bootloader.
 *
 * Parameters : [OUT] pusValue - Address of short word that the break into
 *                  bootloader LED GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBootloaderStopLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedBlStop;

        if( g_pCurrentBp->usGpioLedBlStop != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetBootloaderStopLedGpio */

/**************************************************************************
 * Name       : BpGetVoipLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the VOIP
 *              LED.
 *
 * Parameters : [OUT] pusValue - Address of short word that the VOIP LED
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 *
 * Note       : The VoIP structure would allow for having one LED per DSP
 *              however, the board initialization function assumes only one
 *              LED per functionality (ie one LED for VoIP).  Therefore in
 *              order to keep this tidy and simple we do not make usage of the
 *              one-LED-per-DSP function.  Instead, we assume that the LED for
 *              VoIP is unique and associated with DSP 0 (always present on
 *              any VoIP platform).  If changing this to a LED-per-DSP function
 *              then one need to update the board initialization driver in
 *              bcmdrivers\opensource\char\board\bcm963xx\impl1
 ***************************************************************************/
int BpGetVoipLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = BpGetVoipDspConfig( 0 );

        if( pDspInfo )
        {
           *pusValue = pDspInfo->usGpioLedVoip;

           if( *pusValue != BP_NOT_DEFINED )
           {
              nRet = BP_SUCCESS;
           }
           else
           {
              nRet = BP_VALUE_NOT_DEFINED;
           }
        }
        else
        {
           *pusValue = BP_NOT_DEFINED;
           nRet = BP_BOARD_ID_NOT_FOUND;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoipLedGpio */

/**************************************************************************
 * Name       : BpGetVoip1LedGpio
 *
 * Description: This function returns the GPIO pin assignment for the VoIP1.
 *              LED which is used when FXS0 is active
 * Parameters : [OUT] pusValue - Address of short word that the VoIP1
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoip1LedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = &g_pCurrentBp->VoIPDspInfo[0];

        if( pDspInfo->ucDspType != BP_VOIP_NO_DSP)
        {
            *pusValue = pDspInfo->usGpioVoip1Led;

            if( pDspInfo->usGpioVoip1Led != BP_NOT_DEFINED )
            {
                nRet = BP_SUCCESS;
            }
            else
            {
                nRet = BP_VALUE_NOT_DEFINED;
            }
        }
        else
        {
            *pusValue = BP_NOT_DEFINED;
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoip1LedGpio */

/**************************************************************************
 * Name       : BpGetVoip1FailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the VoIP1
 *              Fail LED which is used when there's an error with FXS0 
 * Parameters : [OUT] pusValue - Address of short word that the VoIP1
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoip1FailLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = &g_pCurrentBp->VoIPDspInfo[0];

        if( pDspInfo->ucDspType != BP_VOIP_NO_DSP)
        {
            *pusValue = pDspInfo->usGpioVoip1LedFail;

            if( pDspInfo->usGpioVoip1LedFail != BP_NOT_DEFINED )
            {
                nRet = BP_SUCCESS;
            }
            else
            {
                nRet = BP_VALUE_NOT_DEFINED;
            }
        }
        else
        {
            *pusValue = BP_NOT_DEFINED;
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoip1FailLedGpio */

/**************************************************************************
 * Name       : BpGetVoip2LedGpio
 *
 * Description: This function returns the GPIO pin assignment for the VoIP2.
 *              LED which is used when FXS1 is active
 * Parameters : [OUT] pusValue - Address of short word that the VoIP2
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoip2LedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = &g_pCurrentBp->VoIPDspInfo[0];

        if( pDspInfo->ucDspType != BP_VOIP_NO_DSP)
        {
            *pusValue = pDspInfo->usGpioVoip2Led;

            if( pDspInfo->usGpioVoip2Led != BP_NOT_DEFINED )
            {
                nRet = BP_SUCCESS;
            }
            else
            {
                nRet = BP_VALUE_NOT_DEFINED;
            }
        }
        else
        {
            *pusValue = BP_NOT_DEFINED;
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoip2LedGpio */

/**************************************************************************
 * Name       : BpGetVoip2FailLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the VoIP2
 *              Fail LED which is used when there's an error with FXS1 
 * Parameters : [OUT] pusValue - Address of short word that the VoIP2
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoip2FailLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = &g_pCurrentBp->VoIPDspInfo[0];

        if( pDspInfo->ucDspType != BP_VOIP_NO_DSP)
        {
            *pusValue = pDspInfo->usGpioVoip2LedFail;

            if( pDspInfo->usGpioVoip2LedFail != BP_NOT_DEFINED )
            {
                nRet = BP_SUCCESS;
            }
            else
            {
                nRet = BP_VALUE_NOT_DEFINED;
            }
        }
        else
        {
            *pusValue = BP_NOT_DEFINED;
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoip2FailLedGpio */

/**************************************************************************
 * Name       : BpGetPotsLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the POTS1.
 *              LED which is used when DAA is active
 * Parameters : [OUT] pusValue - Address of short word that the POTS11
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPotsLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = &g_pCurrentBp->VoIPDspInfo[0];

        if( pDspInfo->ucDspType != BP_VOIP_NO_DSP)
        {
            *pusValue = pDspInfo->usGpioPotsLed;

            if( pDspInfo->usGpioPotsLed != BP_NOT_DEFINED )
            {
                nRet = BP_SUCCESS;
            }
            else
            {
                nRet = BP_VALUE_NOT_DEFINED;
            }
        }
        else
        {
            *pusValue = BP_NOT_DEFINED;
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetPotsLedGpio */

/**************************************************************************
 * Name       : BpGetDectLedGpio
 *
 * Description: This function returns the GPIO pin assignment for the DECT.
 *              LED which is used when DECT is active
 * Parameters : [OUT] pusValue - Address of short word that the DECT
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetDectLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = &g_pCurrentBp->VoIPDspInfo[0];

        if( pDspInfo->ucDspType != BP_VOIP_NO_DSP)
        {
            *pusValue = pDspInfo->usGpioDectLed;

            if( pDspInfo->usGpioDectLed != BP_NOT_DEFINED )
            {
                nRet = BP_SUCCESS;
            }
            else
            {
                nRet = BP_VALUE_NOT_DEFINED;
            }
        }
        else
        {
            *pusValue = BP_NOT_DEFINED;
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetDectLedGpio */

/**************************************************************************
 * Name       : BpGetSlicGpio
 *
 * Description: This function returns the GPIO pin assignments for Slics.
 *              
 * Parameters : [IN] num - slic# to retreive GPIO's for 
 *              [IN] gpioType - which GPIO to retrieve  
 *              [OUT] pusValue - Address of short word that the Slic#
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetSlicGpio( unsigned short num, SLIC_GPIO_TYPE gpioType, unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        switch (num)
        {
            case 0:
                if ( gpioType == SLIC_GPIO_RESET )
                   *pusValue = g_pCurrentBp->VoIPDspInfo[0].usGpioSlic0Reset;                                
                else if ( gpioType == SLIC_GPIO_RELAY )
                   *pusValue = g_pCurrentBp->VoIPDspInfo[0].usGpioSlic0Relay;
            break;
            case 1:
                if ( gpioType == SLIC_GPIO_RESET )
                   *pusValue = g_pCurrentBp->VoIPDspInfo[0].usGpioSlic1Reset;                                
                else if ( gpioType == SLIC_GPIO_RELAY )
                   *pusValue = g_pCurrentBp->VoIPDspInfo[0].usGpioSlic1Relay;            
            break;
            default:
                *pusValue = BP_NOT_DEFINED;
            break;
        }
            
        if( *pusValue != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }
    return( nRet );
} /* BpGetSlicGpio */
/**************************************************************************
 * Name       : BpGetPotsResetGpio
 *
 * Description: This function returns the GPIO pin assignment for the POTS interface.
 *              i.e DAA/SLAC
 * Parameters : [OUT] pusValue - Address of short word that the POTS reset
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetPotsResetGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->VoIPDspInfo[0].usGpioPotsReset;

        if( g_pCurrentBp->VoIPDspInfo[0].usGpioPotsReset != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetPotsResetGpio */


/**************************************************************************
 * Name       : BpGetBondingSecondaryResetGpio
 *
 * Description: This function returns the GPIO pin assignment for the Reset
 *              functionality for the secondary CPU.
 *
 * Parameters : [OUT] pusValue - Address of short word that the Bonding
 *              Secondary Reset GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetBondingSecondaryResetGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioBondingSecondaryReset ;

        if( g_pCurrentBp->usGpioBondingSecondaryReset != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetBondingSecondaryResetGpio */

/**************************************************************************
 * Name       : BpGetAdslDyingGaspExtIntr
 *
 * Description: This function returns the ADSL Dying Gasp external interrupt
 *              number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the ADSL Dying Gasp
 *                  external interrupt number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetAdslDyingGaspExtIntr( unsigned long *pulValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pulValue = g_pCurrentBp->usExtIntrAdslDyingGasp;

        if( g_pCurrentBp->usExtIntrAdslDyingGasp != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pulValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetAdslDyingGaspExtIntr */

/**************************************************************************
 * Name       : BpGetVoipExtIntr
 *
 * Description: This function returns the VOIP external interrupt number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the VOIP
 *                  external interrupt number is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipExtIntr( unsigned char dspNum, unsigned long *pulValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = BpGetVoipDspConfig( dspNum );

        if( pDspInfo )
        {
           *pulValue = pDspInfo->usExtIntrVoip;

           if( *pulValue != BP_NOT_DEFINED )
           {
              nRet = BP_SUCCESS;
           }
           else
           {
              nRet = BP_VALUE_NOT_DEFINED;
           }
        }
        else
        {
           *pulValue = BP_NOT_DEFINED;
           nRet = BP_BOARD_ID_NOT_FOUND;
        }
    }
    else
    {
        *pulValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoipExtIntr */

/**************************************************************************
 * Name       : BpGetHpnaExtIntr
 *
 * Description: This function returns the HPNA external interrupt number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the HPNA
 *                  external interrupt number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetHpnaExtIntr( unsigned long *pulValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pulValue = g_pCurrentBp->usExtIntrHpna;

        if( g_pCurrentBp->usExtIntrHpna != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pulValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetHpnaExtIntr */

/**************************************************************************
 * Name       : BpGetHpnaChipSelect
 *
 * Description: This function returns the HPNA chip select number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the HPNA
 *                  chip select number is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetHpnaChipSelect( unsigned long *pulValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pulValue = g_pCurrentBp->usCsHpna;

        if( g_pCurrentBp->usCsHpna != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pulValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetHpnaChipSelect */

/**************************************************************************
 * Name       : BpGetVoipChipSelect
 *
 * Description: This function returns the VOIP chip select number.
 *
 * Parameters : [OUT] pulValue - Address of short word that the VOIP
 *                  chip select number is returned in.
 *              [IN] dspNum - Address of the DSP to query.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetVoipChipSelect( unsigned char dspNum, unsigned long *pulValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        VOIP_DSP_INFO *pDspInfo = BpGetVoipDspConfig( dspNum );

        if( pDspInfo )
        {
           *pulValue = pDspInfo->usCsVoip;

           if( *pulValue != BP_NOT_DEFINED )
           {
              nRet = BP_SUCCESS;
           }
           else
           {
              nRet = BP_VALUE_NOT_DEFINED;
           }
        }
        else
        {
           *pulValue = BP_NOT_DEFINED;
           nRet = BP_BOARD_ID_NOT_FOUND;
        }
    }
    else
    {
        *pulValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* BpGetVoipChipSelect */

/**************************************************************************
 * Name       : BpGetVcopeGpio 
 *
 * Description: This function returns the GPIO number of VCOPE board
 *
 * Parameters : [OUT] - GPIO index (name)
 *
 * Returns    : pin number      - Success
 *              BP_NOT_DEFINED  - any kind of Error
 ***************************************************************************/
int BpGetVcopeGpio (int pio_idx)
{
    int     nRet = BP_SUCCESS;

    if( g_pCurrentBp )
    {
        if( pio_idx == BCM6505_RESET_GPIO )
            nRet = g_pCurrentBp->usVcopeResetGpio;
        else if( pio_idx == VCOPE_RELAY_GPIO)
            nRet = g_pCurrentBp->usVcopeRelayGpio;
        else if( pio_idx == HPI_CS )
            nRet = g_pCurrentBp->usCsHpi;
        else if( pio_idx == VCOPE_BOARD_REV )
            nRet = g_pCurrentBp->usVcopeBoardRev;
        else
            nRet = BP_NOT_DEFINED;
    }        
    else
        nRet = BP_NOT_DEFINED;
    
   return(nRet);
}

/**************************************************************************
 * Name       : BpGetResetToDefaultExtIntr
 *
 * Description: This function returns the external interrupt number for the 
 *              reset to default button.
 *
 * Parameters : [OUT] pusValue - Address of short word that reset to default
 *                  external interrupt is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetResetToDefaultExtIntr( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usExtIntrResetToDefault;

        if( g_pCurrentBp->usExtIntrResetToDefault != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );	
		
} /* BpGetResetToDefaultExtIntr */

/**************************************************************************
 * Name       : BpGetSlicType
 *
 * Description: This function returns the type (manufacturer & model) of 
 *              SLIC being used by the board. 
 *
 * Parameters : [OUT] slicType - The slicType will be copied from the bp
 *              structure to this address. Please note that the value copied
 *              is an enum typecasted to a short. Hence, it should be cast
 *              back to an enum before being used.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetSlicType( unsigned short *slicType )
{
    if ( g_pCurrentBp == 0 )
    {
	    *slicType = (unsigned short)Slic_Not_Defined;
	    return BP_BOARD_ID_NOT_SET;
	}

    *slicType = (unsigned short)g_pCurrentBp->slicType;

    return (g_pCurrentBp->slicType != Slic_Not_Defined) ?BP_SUCCESS :BP_VALUE_NOT_DEFINED;
} /* BpGetSlicType */

/**************************************************************************
 * Name       : BpGetDAAType
 *
 * Description: This function returns the type (manufacturer & model) of 
 *              DAA being used by the board. 
 *
 * Parameters : [OUT] daaType - The slicType will be copied from the bp
 *              structure to this address. Please note that the value copied
 *              is an enum typecasted to a short. Hence, it should be cast
 *              back to an enum before being used.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetDAAType( unsigned short *daaType )
{
    if ( g_pCurrentBp == 0 )
    {
	    *daaType = (unsigned short)DAA_Not_Defined;
	    return BP_BOARD_ID_NOT_SET;
	}

    *daaType = (unsigned short)g_pCurrentBp->daaType;

    return (g_pCurrentBp->daaType != DAA_Not_Defined) ?BP_SUCCESS :BP_VALUE_NOT_DEFINED;
} /* BpGetDAAType */

/**************************************************************************
 * Name       : BpGetWirelessPowerDownGpio
 *
 * Description: This function returns the GPIO pin assignment for WLAN_PD
 *              
 *
 * Parameters : [OUT] pusValue - Address of short word that the WLAN_PD
 *                  GPIO pin is returned in.
 *
 * Returns    : BP_SUCCESS - Success, value is returned.
 *              BP_BOARD_ID_NOT_SET - Error, BpSetBoardId has not been called.
 *              BP_VALUE_NOT_DEFINED - At least one return value is not defined
 *                  for the board.
 ***************************************************************************/
int BpGetWirelessPowerDownGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioWirelessPowerDown;

        if( g_pCurrentBp->usGpioWirelessPowerDown != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
} /* usGpioWirelessPowerDown */

//dare sunmin usb host led
int BpGetUsbHost1LedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedUsbHost1;

        if( g_pCurrentBp->usGpioLedUsbHost1 != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
	
}

int BpGetUsbHost2LedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioLedUsbHost2;

        if( g_pCurrentBp->usGpioLedUsbHost2 != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
	
}

//sunmin dare wireless protect
int BpGetWirelessProtectLedGpio( unsigned short *pusValue )
{
    int nRet;

    if( g_pCurrentBp )
    {
        *pusValue = g_pCurrentBp->usGpioWirelessProtect;

        if( g_pCurrentBp->usGpioWirelessProtect != BP_NOT_DEFINED )
        {
            nRet = BP_SUCCESS;
        }
        else
        {
            nRet = BP_VALUE_NOT_DEFINED;
        }
    }
    else
    {
        *pusValue = BP_NOT_DEFINED;
        nRet = BP_BOARD_ID_NOT_SET;
    }

    return( nRet );
	
}


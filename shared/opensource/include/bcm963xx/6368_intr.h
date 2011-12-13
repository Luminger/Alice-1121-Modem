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

#ifndef __6368_INTR_H
#define __6368_INTR_H

#ifdef __cplusplus
    extern "C" {
#endif

#define INTERRUPT_ID_SOFTWARE_0           0
#define INTERRUPT_ID_SOFTWARE_1           1

/*=====================================================================*/
/* BCM6368 Timer Interrupt Level Assignments                          */
/*=====================================================================*/
#define MIPS_TIMER_INT                  7

/*=====================================================================*/
/* Peripheral ISR Table Offset                                              */
/*=====================================================================*/
#define INTERNAL_ISR_TABLE_OFFSET       8
#define INTERNAL_HIGH_ISR_TABLE_OFFSET  (INTERNAL_ISR_TABLE_OFFSET + 32)

/*=====================================================================*/
/* Logical Peripheral Interrupt IDs                                    */
/*=====================================================================*/

#define INTERRUPT_ID_TIMER               (INTERNAL_ISR_TABLE_OFFSET + 0)
#define INTERRUPT_ID_SPI                 (INTERNAL_ISR_TABLE_OFFSET + 1)
#define INTERRUPT_ID_UART                (INTERNAL_ISR_TABLE_OFFSET + 2)
#define INTERRUPT_ID_UART1               (INTERNAL_ISR_TABLE_OFFSET + 3)
#define INTERRUPT_ID_XDSL                (INTERNAL_ISR_TABLE_OFFSET + 4)
#define INTERRUPT_ID_USBH                (INTERNAL_ISR_TABLE_OFFSET + 5)
#define INTERRUPT_ID_IPSEC               (INTERNAL_ISR_TABLE_OFFSET + 6)
#define INTERRUPT_ID_USBH20              (INTERNAL_ISR_TABLE_OFFSET + 7)
#define INTERRUPT_ID_USBS                (INTERNAL_ISR_TABLE_OFFSET + 8)
#define INTERRUPT_ID_RING_OSC            (INTERNAL_ISR_TABLE_OFFSET + 9)
#define INTERRUPT_ID_NAND_FLASH          (INTERNAL_ISR_TABLE_OFFSET + 10)
#define INTERRUPT_ID_ATM                 (INTERNAL_ISR_TABLE_OFFSET + 11)
#define INTERRUPT_ID_PCM                 (INTERNAL_ISR_TABLE_OFFSET + 12)
#define INTERRUPT_ID_MPI                 (INTERNAL_ISR_TABLE_OFFSET + 13)
#define INTERRUPT_ID_DG                  (INTERNAL_ISR_TABLE_OFFSET + 14)
#define INTERRUPT_ID_EPHY                (INTERNAL_ISR_TABLE_OFFSET + 15)
#define INTERRUPT_ID_EPHY_ENERGY_DET_0   (INTERNAL_ISR_TABLE_OFFSET + 16)
#define INTERRUPT_ID_EPHY_ENERGY_DET_1   (INTERNAL_ISR_TABLE_OFFSET + 17)
#define INTERRUPT_ID_EPHY_ENERGY_DET_2   (INTERNAL_ISR_TABLE_OFFSET + 18)
#define INTERRUPT_ID_EPHY_ENERGY_DET_3   (INTERNAL_ISR_TABLE_OFFSET + 19)
#define INTERRUPT_ID_EXTERNAL_0          (INTERNAL_ISR_TABLE_OFFSET + 20)
#define INTERRUPT_ID_EXTERNAL_1          (INTERNAL_ISR_TABLE_OFFSET + 21)
#define INTERRUPT_ID_EXTERNAL_2          (INTERNAL_ISR_TABLE_OFFSET + 22)
#define INTERRUPT_ID_EXTERNAL_3          (INTERNAL_ISR_TABLE_OFFSET + 23)
#define INTERRUPT_ID_EXTERNAL_4          (INTERNAL_ISR_TABLE_OFFSET + 24)
#define INTERRUPT_ID_EXTERNAL_5          (INTERNAL_ISR_TABLE_OFFSET + 25)
#define INTERRUPT_ID_USB_CNTL_RX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 26)
#define INTERRUPT_ID_USB_CNTL_TX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 27)
#define INTERRUPT_ID_USB_BULK_RX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 28)
#define INTERRUPT_ID_USB_BULK_TX_DMA     (INTERNAL_ISR_TABLE_OFFSET + 29)
#define INTERRUPT_ID_USB_ISO_RX_DMA      (INTERNAL_ISR_TABLE_OFFSET + 30)
#define INTERRUPT_ID_USB_ISO_TX_DMA      (INTERNAL_ISR_TABLE_OFFSET + 31)
#define INTERRUPT_ID_ENETSW_RX_DMA_0     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 0)
#define INTERRUPT_ID_ENETSW_RX_DMA_1     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 1)
#define INTERRUPT_ID_ENETSW_RX_DMA_2     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 2)
#define INTERRUPT_ID_ENETSW_RX_DMA_3     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 3)
#define INTERRUPT_ID_ENETSW_TX_DMA_0     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 4)
#define INTERRUPT_ID_ENETSW_TX_DMA_1     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 5)
#define INTERRUPT_ID_ENETSW_TX_DMA_2     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 6)
#define INTERRUPT_ID_ENETSW_TX_DMA_3     (INTERNAL_HIGH_ISR_TABLE_OFFSET + 7)
#define INTERRUPT_ID_ATM_DMA_0           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 8)
#define INTERRUPT_ID_ATM_DMA_1           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 9)
#define INTERRUPT_ID_ATM_DMA_2           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 10)
#define INTERRUPT_ID_ATM_DMA_3           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 11)
#define INTERRUPT_ID_ATM_DMA_4           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 12)
#define INTERRUPT_ID_ATM_DMA_5           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 13)
#define INTERRUPT_ID_ATM_DMA_6           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 14)
#define INTERRUPT_ID_ATM_DMA_7           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 15)
#define INTERRUPT_ID_ATM_DMA_8           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 16)
#define INTERRUPT_ID_ATM_DMA_9           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 17)
#define INTERRUPT_ID_ATM_DMA_10          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 18)
#define INTERRUPT_ID_ATM_DMA_11          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 19)
#define INTERRUPT_ID_ATM_DMA_12          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 20)
#define INTERRUPT_ID_ATM_DMA_13          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 21)
#define INTERRUPT_ID_ATM_DMA_14          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 22)
#define INTERRUPT_ID_ATM_DMA_15          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 23)
#define INTERRUPT_ID_ATM_DMA_16          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 24)
#define INTERRUPT_ID_ATM_DMA_17          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 25)
#define INTERRUPT_ID_ATM_DMA_18          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 26)
#define INTERRUPT_ID_ATM_DMA_19          (INTERNAL_HIGH_ISR_TABLE_OFFSET + 27)
#define INTERRUPT_ID_IPSEC_DMA_0         (INTERNAL_HIGH_ISR_TABLE_OFFSET + 28)
#define INTERRUPT_ID_IPSEC_DMA_1         (INTERNAL_HIGH_ISR_TABLE_OFFSET + 29)
#define INTERRUPT_ID_PCM_DMA_0           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 30)
#define INTERRUPT_ID_PCM_DMA_1           (INTERNAL_HIGH_ISR_TABLE_OFFSET + 31)

#define INTERRUPT_ID_LAST                INTERRUPT_ID_PCM_DMA_1

#ifdef __cplusplus
    }
#endif                    

#endif  /* __BCM6368_H */



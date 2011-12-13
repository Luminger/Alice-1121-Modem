/*
<:copyright-broadcom 
 
 Copyright (c) 2007 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
*/
/***********************************************************************/
/*                                                                     */
/*   MODULE:  6368_common.h                                             */
/*   DATE:    02/06/07                                                 */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            BCM6368                                                  */
/*                                                                     */
/***********************************************************************/
#ifndef __BCM6368_MAP_COMMON_H
#define __BCM6368_MAP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define PERF_BASE            0xb0000000  /* chip control registers */
#define TIMR_BASE            0xb0000040  /* timer registers */
#define GPIO_BASE            0xb0000080  /* gpio registers */
#define UART_BASE            0xb0000100  /* uart registers */
#define UART1_BASE           0xb0000120  /* uart registers */
#define NAND_REG_BASE        0xb0000200
#define NAND_SEC_BASE        0xb0000300
#define OTP_BASE             0xb0000400
#define UBUS_STAT_BASE       0xb0000500
#define NAND_CACHE_BASE      0xb0000600
#define SPI_BASE             0xb0000800  /* SPI master controller registers */
#define MPI_BASE             0xb0001000  /* MPI control registers */
#define MEMC_BASE            0xb0001200  /* Memory control registers */
#define DDR_BASE             0xb0001280  /* DDR IO Buf Control registers */
#define USB_CTL_BASE         0xb0001400  /* USB 2.0 device control registers */
#define USB_EHCI_BASE        0x10001500  /* USB host registers */
#define USB_OHCI_BASE        0x10001600  /* USB host registers */
#define USBH_CFG_BASE        0xb0001700
#define SAR_BASE             0xb0001800  /* ATM SAR control registers */
#define PCM_BASE             0xb0004000  /* PCM control registers */
#define IPSEC_BASE           0xb0004100
#define USB_DMA_BASE         0xb0004800  /* USB 2.0 device DMA regiseters */
#define SAR_DMA_BASE         0xb0005000  /* ATM SAR DMA control registers */
#define PCM_DMA_BASE         0xb0005800  /* PCM UIDMA register base */
#define IPSEC_DMA_BASE       0xb0006000
#define SWITCH_DMA_BASE      0xb0006800
#define SWITCH_BASE          0xb0f00000

#define ADSL_PHY_BASE        0xb0f40000
#define ADSL_ENUM_BASE       0xb0f56000
#define ADSL_LMEM_BASE       0xb0f80000

/*
#####################################################################
# System PLL Control Register
#####################################################################
*/


/*
#####################################################################
# GPIO Control Registers
#####################################################################
*/


/*
#####################################################################
# Memory Control Registers
#####################################################################
*/
#define MEMC_CONTROL             0x0
#define MEMC_CONFIG              0x4
#define MEMC_REF_PD_CONTROL      0x8
#define MEMC_BIST_STATUS         0xc
#define MEMC_M_EM_BUF           0x10
#define MEMC_BANK_CLS_TIM       0x14
#define MEMC_PRIOR_INV_TIM      0x18
#define MEMC_DRAM_TIM           0x1c
#define MEMC_INT_STATUS         0x20
#define MEMC_INT_MASK           0x24
#define MEMC_INT_INFO           0x28
#define MEMC_BARRIER            0x50
#define MEMC_CORE_ID            0x54

#define DDR_REV_ID               0x0
#define DDR_PAD_SSTL_MODE        0x4
#define DDR_CMD_PAD_CNTL         0x8
#define DDR_DQ_PAD_CNTL          0xc
#define DDR_DQS_PAD_CNTL        0x10    
#define DDR_CLK_PAD_CNTL        0x14
#define DDR_PLL_CNTL0           0x18
#define DDR_PLL_CNTL1           0x1c
#define DDR_MIPSDDR_PLL_CONFIG  0x20
#define DDR_MIPSDDR_PLL_MDIV    0x24
#define DDR_DSL_PHY_PHASE_CNTL  0x28
#define DDR_DSL_CPU_PHASE_CNTL  0x2c
#define DDR_MIPS_PHASE_CNTL     0x30
#define DDR_DDR1_2_PHASE_CNTL   0x34
#define DDR_DDR3_4_PHASE_CNTL   0x38    
#define DDR_VCDL_PHASE_CNTL0    0x3c
#define DDR_VCDL_PHASE_CNTL1    0x40
#define DDR_BSLICE_CNTL         0x44
#define DDR_DESKEW_DLL_CNTL     0x48
#define DDR_DESKEW_DLL_RESET    0x4c
#define DDR_DESKEW_DLL_PHASE    0x50
#define DDR_ANALOG_TEST_CNTL    0x54
#define DDR_RD_DQS_GATE_CNTL    0x58
#define DDR_MISC                0x5c
#define DDR_SPARE0              0x60
#define DDR_SPARE1              0x64
#define DDR_SPARE2              0x68
#define DDR_CLBIST              0x6c    
#define DDR_LBIST_CRC           0x70
#define DDR_UBUS_PHASE_CNTR     0x74
#define DDR_UBUS_PI_DSK0        0x78
#define DDR_UBUS_PI_DSK1        0x7c

// Some bit/field definitions for the MEMC_CONTROL register.
#define MEMC_DQSGATEEN          (1<<16)

#define MEMC_ROW_SHFT           6
#define MEMC_ROW_MASK           (0x3<<MEMC_ROW_SHFT)
#define MEMC_11BIT_ROW          0
#define MEMC_12BIT_ROW          1
#define MEMC_13BIT_ROW          2
#define MEMC_14BIT_ROW          3

#define MEMC_COL_SHFT           3
#define MEMC_COL_MASK           (0x7<<MEMC_COL_SHFT)
#define MEMC_8BIT_COL           0
#define MEMC_9BIT_COL           1
#define MEMC_10BIT_COL          2
#define MEMC_11BIT_COL          3

#define MEMC_SEL_PRIORITY       (1<<2)

#define MEMC_WIDTH_SHFT         1
#define MEMC_WIDTH_MASK         (0x1<<MEMC_WIDTH_SHFT)
#define MEMC_32BIT_BUS          0
#define MEMC_16BIT_BUS          1

#define MEMC_MEMTYPE_SDR        (0<<0)
#define MEMC_MEMTYPE_DDR        (1<<0)

/*
#####################################################################
# MPI Control Registers
#####################################################################
*/
#define CS0BASE         0x00
#define CS0CNTL         0x04

/*
# CSxBASE settings
#   Size in low 4 bits
#   Base Address for match in upper 24 bits
*/
#define EBI_SIZE_8K         0
#define EBI_SIZE_16K        1
#define EBI_SIZE_32K        2
#define EBI_SIZE_64K        3
#define EBI_SIZE_128K       4
#define EBI_SIZE_256K       5
#define EBI_SIZE_512K       6
#define EBI_SIZE_1M         7
#define EBI_SIZE_2M         8
#define EBI_SIZE_4M         9
#define EBI_SIZE_8M         10
#define EBI_SIZE_16M        11
#define EBI_SIZE_32M        12
#define EBI_SIZE_64M        13
#define EBI_SIZE_128M       14
#define EBI_SIZE_256M       15

/* CSxCNTL settings */
#define EBI_ENABLE          0x00000001  /* .. enable this range */
#define EBI_WAIT_STATES     0x0000000e  /* .. mask for wait states */
#define ZEROWT              0x00000000  /* ..  0 WS */
#define ONEWT               0x00000002  /* ..  1 WS */
#define TWOWT               0x00000004  /* ..  2 WS */
#define THREEWT             0x00000006  /* ..  3 WS */
#define FOURWT              0x00000008  /* ..  4 WS */
#define FIVEWT              0x0000000a  /* ..  5 WS */
#define SIXWT               0x0000000c  /* ..  6 WS */
#define SEVENWT             0x0000000e  /* ..  7 WS */
#define EBI_WORD_WIDE       0x00000010  /* .. 16-bit peripheral, else 8 */
#define EBI_TS_TA_MODE      0x00000080  /* .. use TS/TA mode */
#define EBI_TS_SEL          0x00000100  /* .. drive tsize, not bs_b */
#define EBI_FIFO            0x00000200  /* .. enable fifo */
#define EBI_RE              0x00000400  /* .. Reverse Endian */

/*
#####################################################################
# UART Control Registers
#####################################################################
*/
#define UART0CONTROL     0x01
#define UART0CONFIG      0x02
#define UART0RXTIMEOUT   0x03
#define UART0BAUD        0x04
#define UART0FIFOCFG     0x0a
#define UART0INTMASK     0x10
#define UART0INTSTAT     0x12
#define UART0DATA        0x17

#define BRGEN            0x80   /* Control register bit defs */
#define TXEN             0x40
#define RXEN             0x20
#define LOOPBK           0x10
#define TXPARITYEN       0x08
#define TXPARITYEVEN     0x04
#define RXPARITYEN       0x02
#define RXPARITYEVEN     0x01

#define XMITBREAK        0x40   /* Config register */
#define BITS5SYM         0x00
#define BITS6SYM         0x10
#define BITS7SYM         0x20
#define BITS8SYM         0x30
#define ONESTOP          0x07
#define TWOSTOP          0x0f

#define DELTAIP         0x0001
#define TXUNDERR        0x0002
#define TXOVFERR        0x0004
#define TXFIFOTHOLD     0x0008
#define TXREADLATCH     0x0010
#define TXFIFOEMT       0x0020
#define RXUNDERR        0x0040
#define RXOVFERR        0x0080
#define RXTIMEOUT       0x0100
#define RXFIFOFULL      0x0200
#define RXFIFOTHOLD     0x0400
#define RXFIFONE        0x0800
#define RXFRAMERR       0x1000
#define RXPARERR        0x2000
#define RXBRK           0x4000

#ifdef __cplusplus
}
#endif

#endif

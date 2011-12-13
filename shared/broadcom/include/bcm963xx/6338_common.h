/*
<:copyright-broadcom 
 
 Copyright (c) 2002 Broadcom Corporation 
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
/*   MODULE: 6338_common.h                                             */
/*   DATE:    05/10/04                                                 */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            BCM6338                                                  */
/*                                                                     */
/***********************************************************************/
#ifndef __BCM6338_MAP_COMMON_H
#define __BCM6338_MAP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define PERF_BASE     0xfffe0000    /* chip control registers */
#define BB_BASE       0xfffe0100    /* bus bridge registers */
#define TIMR_BASE     0xfffe0200    /* timer registers */
#define UART_BASE     0xfffe0300    /* uart registers */
#define GPIO_BASE     0xfffe0400    /* gpio registers */
#define SPI_BASE      0xfffe0c00    /* SPI master controller registers */

#define ADSL_BASE     0xfffe1000	/* ADSL core control registers */
#define ATM_BASE      0xfffe2000	/* ATM SAR control registers */
#define EMAC_DMA_BASE 0xfffe2400    /* EMAC DMA control registers */
#define USB_DMA_BASE  0xfffe2400    /* USB DMA control registers */
#define EMAC1_BASE    0xfffe2800    /* EMAC1 control registers */
#define USB_CTL_BASE  0xfffe3000    /* USB control registers */
#define MEMC_BASE     0xfffe3100    /* Memory control registers */
#define MPI_BASE      0xfffe3160    /* EBI control registers */


/*
#####################################################################
# System PLL Control Register
#####################################################################
*/

#define SOFT_RESET	0x00000001

/*
#####################################################################
# Memory Control Registers
#####################################################################
*/
#define MEMC_INIT_CTL        0x00
    /* Control Bits */
#define MEMC_PFEN1           (1<<16)
#define MEMC_PFEN0           (1<<15)
#define MEMC_EMPRS           (1<<14)
#define MEMC_2_BANKS         (1<<13)
#define MEMC_1_BANK          (0<<13)
#define MEMC_CS1_EN          (1<<12)
#define MEMC_PEND            (1<<11)

#define MEMC_WIDTH_SHFT      10
#define MEMC_WIDTH_MASK      (0x1<<MEMC_WIDTH_SHFT)
#define MEMC_32BIT_BUS       1
#define MEMC_16BIT_BUS       0

#define MEMC_POWER_DOWN      (1<<9)
#define MEMC_SELF_REFRESH    (1<<8)

#define MEMC_ROW_SHFT        4
#define MEMC_ROW_MASK        (0x3<<MEMC_ROW_SHFT)
#define MEMC_11BIT_ROW       0
#define MEMC_12BIT_ROW       1
#define MEMC_13BIT_ROW       2

#define MEMC_COL_SHFT        6
#define MEMC_COL_MASK        (0x3<<MEMC_COL_SHFT)
#define MEMC_8BIT_COL        0
#define MEMC_9BIT_COL        1
#define MEMC_10BIT_COL       2
#define MEMC_11BIT_COL       3

#define MEMC_MASTER_EN       (1<<3)
#define MEMC_MRS_CMD         (1<<2)
#define MEMC_PRE_CMD         (1<<1)
#define MEMC_CBR_CMD         (1<<0)

#define MEMC_CFG_REG         0x04
    /* Control Bits */
#define MEMC_FULL_PG         0
#define MEMC_BURST8          1
#define MEMC_BURST4          2
#define MEMC_BURST2          3
#define MEMC_FAST_MEM        (1<<2)
#define MEMC_SLOW_MEM        (0<<2)

#define MEMC_REF_CTL         0x0C
    /* Control Bits */
#define MEMC_REF_EN          (1<<15)

#define MEMC_PRIOR           0x2C
    /* Control Bits */
#define MEMC_EN_PRIOR        (1<<31)
#define MEMC_ADSL_HPE        (1<<30)
#define MEMC_MIPS_HPE        (1<<29)

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
#define EBI_POLARITY        0x00000040  /* .. set to invert chip select polarity */
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

/*
#####################################################################
# SPI Config Register
#####################################################################
*/
#define SPICLKCFG           6

#define SPICLK_20_0MHZ      0
#define SPICLK_0_391MHZ     1
#define SPICLK_0_781MHZ     2 /* default */
#define SPICLK_1_563MHZ     3
#define SPICLK_3_125MHZ     4
#define SPICLK_6_250MHZ     5
#define SPICLK_12_50MHZ     6
#define SPICLK_MASK         0x07
#define SPISSOFFTIME_MASK   0x38
#define SPISSOFFTIME_SHIFT  3
#define SPIBYTE_SWAP        0x80

#ifdef __cplusplus
}
#endif

#endif

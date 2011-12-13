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
/*   MODULE:  6368_map.h                                               */
/*   DATE:    02/06/07                                                 */
/*   PURPOSE: Define addresses of major hardware components of         */
/*            BCM6368                                                  */
/*                                                                     */
/***********************************************************************/
#ifndef __BCM6368_MAP_H
#define __BCM6368_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bcmtypes.h"
#include "6368_common.h"
#include "6368_intr.h"

/* macro to convert logical data addresses to physical */
/* DMA hardware must see physical address */
#define LtoP( x )       ( (uint32)x & 0x1fffffff )
#define PtoL( x )       ( LtoP(x) | 0xa0000000 )

typedef struct MemoryControl
{
    uint32 Control;             /* (00) */
#define MEMC_SELF_REFRESH    (1<<6) // enable self refresh mode
#define MEMC_MRS             (1<<4) // generate a mode register select cycle
#define MEMC_PRECHARGE       (1<<3) // generate a precharge cycle
#define MEMC_REFRESH         (1<<2) // generate an auto refresh cycle
#define MEMC_SEQUENCE_ENABLE (1<<1) // enable memory controller sequencer
#define MEMC_MASTER_ENABLE   (1<<0) // enable accesses to external sdram

    uint32  Config;             /* (04) */
#define MEMC_SYS_PORT_CMD_MODE  (1<<18)
#define MEMC_PAD_OP_MODE        (1<<17)
#define MEMC_DQS_GATE_EN        (1<<16)
#define MEMC_PRED_RD_STROBE_EN  (1<<15)
#define MEMC_PRED_RD_LATENCY_SEL (1<<14)
#define MEMC_UBUS_CLF_EN        (1<<8)

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

    uint32  RefreshPdControl;   /* (08) */ 
    uint32  BistStatus;         /* (0C) */
    uint32  ExtendedModeBuffer; /* (10) */
    uint32  BankClosingTimer;   /* (14) */
    uint32  PriorityInversionTimer; /* (18) */

    uint32  DramTiming;         /* (1c) */
#define MEMC_WR_NOP_RD        (1<<23)
#define MEMC_WR_NOP_WR        (1<<22)
#define MEMC_RD_NOP_WR        (1<<21)
#define MEMC_RD_NOP_RD        (1<<20)
#define MEMC_CAS_LATENCY_2    (0)
#define MEMC_CAS_LATENCY_2_5  (1)
#define MEMC_CAS_LATENCY_3    (2)

    uint32  IntStatus;          /* (20) */
    uint32  IntMask;            /* (24) */
#define MEMC_INT3             (1<<3)
#define MEMC_INT2             (2<<3)
#define MEMC_INT1             (1<<3)
#define MEMC_INT0             (0<<3)

    uint32  IntInfo;            /* (28) */
    uint8   unused5[0x50-0x2c]; /* (2c) */
    uint32  Barrier;            /* (50) */
    uint32  CoreId;             /* (54) */
} MemoryControl;

#define MEMC ((volatile MemoryControl * const) MEMC_BASE)

typedef struct DDRControl {
    uint32    RevID;            /* 00 */
    uint32    PadSSTLMode;      /* 04 */
    uint32    CmdPadCntl;       /* 08 */
    uint32    DQPadCntl;        /* 0c */
    uint32    DQSPadCntl;       /* 10 */
    uint32    ClkPadCntl0;      /* 14 */
    uint32    MIPSDDRPLLCntl0;  /* 18 */
    uint32    MIPSDDRPLLCntl1;  /* 1c */
    uint32    MIPSDDRPLLConfig; /* 20 */
#define MIPSDDR_NDIV_SHFT       16
#define MIPSDDR_NDIV_MASK       (0x1ff<<MIPSDDR_NDIV_SHFT)
#define REF_MDIV_SHFT           8
#define REF_MDIV_MASK           (0xff<<REF_MDIV_SHFT)
#define MIPSDDR_P2_SHFT         4
#define MIPSDDR_P2_MASK         (0xf<<MIPSDDR_P2_SHFT)
#define MIPSDDR_P1_SHFT         0
#define MIPSDDR_P1_MASK         (0xf<<MIPSDDR_P1_SHFT)
    uint32    MIPSDDRPLLMDiv;   /* 24 */
#define DDR_MDIV_SHFT           8
#define DDR_MDIV_MASK           (0xff<<DDR_MDIV_SHFT)
#define MIPS_MDIV_SHFT          0
#define MIPS_MDIV_MASK          (0xff<<MIPS_MDIV_SHFT)
    uint32    DSLCorePhaseCntl; /* 28 */
#define	DSL_PHY_PI_CNTR_EN	(1 << 20)
#define	DSL_PHY_PI_CNTR_CYCLES_SHIFT	16
#define	DSL_PHY_PI_CNTR_CYCLES_MASK	(0xF << DSL_PHY_PI_CNTR_CYCLES_SHIFT)
    uint32    DSLCpuPhaseCntr;  /* 2c */
#define	DSL_CPU_PI_CNTR_EN	(1 << 20)
#define	DSL_CPU_PI_CNTR_CYCLES_SHIFT	16
#define	DSL_CPU_PI_CNTR_CYCLES_MASK	(0xF << DSL_CPU_PI_CNTR_CYCLES_SHIFT)
    uint32    MIPSPhaseCntl;    /* 30 */
#define	PH_CNTR_EN		(1 << 20)
    uint32    DDR1_2PhaseCntl0;  /* 34 */
    uint32    DDR3_4PhaseCntl0; /* 38 */
    uint32    VCDLPhaseCntl0;   /* 3c */
    uint32    VCDLPhaseCntl1;   /* 40 */
    uint32    WSliceCntl;       /* 44 */
    uint32    DeskewDLLCntl;    /* 48 */
    uint32    DeskewDLLReset;   /* 4c */
    uint32    DeskewDLLPhase;   /* 50 */
    uint32    AnalogTestCntl;   /* 54 */
    uint32    RdDQSGateCntl;    /* 58 */
    uint32    PLLTestReg;       /* 5c */
    uint32    Spare0;           /* 60 */
    uint32    Spare1;           /* 64 */
    uint32    Spare2;           /* 68 */
    uint32    CLBist;           /* 6c */
    uint32    LBistCRC;         /* 70 */
    uint32    UBUSPhaseCntl;    /* 74 */
    uint32    UBUSPIDeskewLLMB0; /* 78 */
    uint32    UBUSPIDeskewLLMB1; /* 7C */

} DDRControl;

#define DDR ((volatile DDRControl * const) DDR_BASE)

/*
** Peripheral Controller
*/
typedef struct PerfControl {
     uint32        RevID;             /* (00) word 0 */
     uint32        blkEnables;        /* (04) word 1 */
#define IPSEC_CLK_EN     (1 << 18)
#define NAND_CLK_EN      (1 << 17)
#define DISABLE_GLESS    (1 << 16)
#define USBH_CLK_EN      (1 << 15)
#define PCM_CLK_EN       (1 << 14)
#define UTOPIA_CLK_EN    (1 << 13)
#define ROBOSW_CLK_EN    (1 << 12)
#define SAR_CLK_EN       (1 << 11)
#define USBD_CLK_EN      (1 << 10)
#define SPI_CLK_EN       (1 << 9)
#define SWPKT_SAR_CLK_EN (1 << 8)
#define SWPKT_USB_CLK_EN (1 << 7)
#define PHYMIPS_CLK_EN   (1 << 6)
#define VDSL_CLK_EN      (1 << 5)
#define VDSL_BONDING_EN  (1 << 4)
#define VDSL_AFE_EN      (1 << 3)
#define VDSL_QPROC_EN    (1 << 2)
   
     uint32        pll_control;       /* (08) word 2 */
#define SOFT_RESET              0x00000001      // 0

     uint32        deviceTimeoutEn;   /* (0c) word 3 */
     uint32        softResetB;        /* (10) word 4 */
#define SOFT_RST_SPI         0x00000001
#define SOFT_RST_MPI         0x00000008
#define SOFT_RST_IPSEC       0x00000010
#define SOFT_RST_EPHY        0x00000040
#define SOFT_RST_SAR         0x00000080
#define SOFT_RST_SWITCH      0x00000400
#define SOFT_RST_USBD        0x00000800
#define SOFT_RST_USBH        0x00001000
#define SOFT_RST_PCM         0x00002000 

    uint32        unused1;            /* (14) word 5 */
    uint32        ExtIrqCfg;          /* (18) word 6*/
    uint32        ExtIrqCfg1;         /* (1c) word 7 */
#define EI_SENSE_SHFT   0
#define EI_STATUS_SHFT  4
#define EI_CLEAR_SHFT   8
#define EI_MASK_SHFT    12
#define EI_INSENS_SHFT  16
#define EI_LEVEL_SHFT   20

     uint32        IrqMask_high;      /* (20) word 8 */
     uint32        IrqMask;           /* (24) word 9 */
     uint32        IrqStatus_high;    /* (28) word 10 */
     uint32        IrqStatus;         /* (2c) word 11 */

     uint32        IrqMask1_high;     /* (30) word 13 */
     uint32        IrqMask1;          /* (34) word 14*/   
     uint32        IrqStatus1_high;   /* (38) word 15 */
     uint32        IrqStatus1;        /* (3c) word 16 */
} PerfControl;

#define PERF ((volatile PerfControl * const) PERF_BASE)

/*
** Timer
*/
typedef struct Timer {
    uint16        unused0;
    byte          TimerMask;
#define TIMER0EN        0x01
#define TIMER1EN        0x02
#define TIMER2EN        0x04
    byte          TimerInts;
#define TIMER0          0x01
#define TIMER1          0x02
#define TIMER2          0x04
#define WATCHDOG        0x08
    uint32        TimerCtl0;
    uint32        TimerCtl1;
    uint32        TimerCtl2;
#define TIMERENABLE     0x80000000
#define RSTCNTCLR       0x40000000      
    uint32        TimerCnt0;
    uint32        TimerCnt1;
    uint32        TimerCnt2;
    uint32        WatchDogDefCount;

    /* Write 0xff00 0x00ff to Start timer
     * Write 0xee00 0x00ee to Stop and re-load default count
     * Read from this register returns current watch dog count
     */
    uint32        WatchDogCtl;

    /* Number of 40-MHz ticks for WD Reset pulse to last */
    uint32        WDResetCount;
} Timer;

#define TIMER ((volatile Timer * const) TIMR_BASE)

/*
** UART
*/
typedef struct UartChannel {
    byte          unused0;
    byte          control;
#define BRGEN           0x80    /* Control register bit defs */
#define TXEN            0x40
#define RXEN            0x20
#define LOOPBK          0x10
#define TXPARITYEN      0x08
#define TXPARITYEVEN    0x04
#define RXPARITYEN      0x02
#define RXPARITYEVEN    0x01

    byte          config;
#define XMITBREAK       0x40
#define BITS5SYM        0x00
#define BITS6SYM        0x10
#define BITS7SYM        0x20
#define BITS8SYM        0x30
#define ONESTOP         0x07
#define TWOSTOP         0x0f
    /* 4-LSBS represent STOP bits/char
     * in 1/8 bit-time intervals.  Zero
     * represents 1/8 stop bit interval.
     * Fifteen represents 2 stop bits.
     */
    byte          fifoctl;
#define RSTTXFIFOS      0x80
#define RSTRXFIFOS      0x40
    /* 5-bit TimeoutCnt is in low bits of this register.
     *  This count represents the number of characters 
     *  idle times before setting receive Irq when below threshold
     */
    uint32        baudword;
    /* When divide SysClk/2/(1+baudword) we should get 32*bit-rate
     */

    byte          txf_levl;       /* Read-only fifo depth */
    byte          rxf_levl;       /* Read-only fifo depth */
    byte          fifocfg;        /* Upper 4-bits are TxThresh, Lower are
                                   *      RxThreshold.  Irq can be asserted
                                   *      when rx fifo> thresh, txfifo<thresh
                                   */
    byte          prog_out;       /* Set value of DTR (Bit0), RTS (Bit1)
                                   *  if these bits are also enabled to GPIO_o
                                   */
#define DTREN   0x01
#define RTSEN   0x02

    byte          unused1;
    byte          DeltaIPEdgeNoSense;     /* Low 4-bits, set corr bit to 1 to 
                                           * detect irq on rising AND falling 
                                           * edges for corresponding GPIO_i
                                           * if enabled (edge insensitive)
                                           */
    byte          DeltaIPConfig_Mask;     /* Upper 4 bits: 1 for posedge sense
                                           *      0 for negedge sense if
                                           *      not configured for edge
                                           *      insensitive (see above)
                                           * Lower 4 bits: Mask to enable change
                                           *  detection IRQ for corresponding
                                           *  GPIO_i
                                           */
    byte          DeltaIP_SyncIP;         /* Upper 4 bits show which bits
                                           *  have changed (may set IRQ).
                                           *  read automatically clears bit
                                           * Lower 4 bits are actual status
                                           */

    uint16        intMask;                /* Same Bit defs for Mask and status */
    uint16        intStatus;
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

    uint16        unused2;
    uint16        Data;                   /* Write to TX, Read from RX */
                                          /* bits 11:8 are BRK,PAR,FRM errors */

    uint32        unused3;
    uint32        unused4;
} Uart;

#define UART ((volatile Uart * const) UART_BASE)

/*
** Gpio Controller
*/

typedef struct GpioControl {
    uint32      GPIODir_high;               /* bits 36:32 */
    uint32      GPIODir;                    /* bits 31:00 */
    uint32      GPIOio_high;                /* bits 36:32 */
    uint32      GPIOio;                     /* bits 31:00 */
    uint32      LEDCtrl;
#define LED_ALL_STROBE          0x0f000000
#define LED3_STROBE             0x08000000
#define LED2_STROBE             0x04000000
#define LED1_STROBE             0x02000000
#define LED0_STROBE             0x01000000
#define LED_TEST                0x00010000
#define DISABLE_LINK_ACT_ALL    0x0000f000
#define DISABLE_LINK_ACT_3      0x00008000
#define DISABLE_LINK_ACT_2      0x00004000
#define DISABLE_LINK_ACT_1      0x00002000
#define DISABLE_LINK_ACT_0      0x00001000
#define LED_INTERVAL_SET_MASK   0x00000f00
#define LED_INTERVAL_SET_1280MS 0x00000700
#define LED_INTERVAL_SET_640MS  0x00000600
#define LED_INTERVAL_SET_320MS  0x00000500
#define LED_INTERVAL_SET_160MS  0x00000400
#define LED_INTERVAL_SET_80MS   0x00000300
#define LED_INTERVAL_SET_40MS   0x00000200
#define LED_INTERVAL_SET_20MS   0x00000100
#define LED_ON_ALL              0x000000f0
#define LED_ON_3                0x00000080
#define LED_ON_2                0x00000040
#define LED_ON_1                0x00000020
#define LED_ON_0                0x00000010
#define LED_ENABLE_ALL          0x0000000f
#define LED_ENABLE_3            0x00000008
#define LED_ENABLE_2            0x00000004
#define LED_ENABLE_1            0x00000002
#define LED_ENABLE_0            0x00000001
    uint32      SpiSlaveCfg;                /* 14 */
    uint32      GPIOMode;                   /* 18 */
#define GPIO_MODE_SPI_SSN5          (1<<31)
#define GPIO_MODE_SPI_SSN4          (1<<30)
#define GPIO_MODE_SPI_SSN3          (1<<29)
#define GPIO_MODE_SPI_SSN2          (1<<28)
#define GPIO_MODE_EBI_CS3           (1<<27)
#define GPIO_MODE_EBI_CS2           (1<<26)
#define GPIO_MODE_PCMCIA_VS2        (1<<25)
#define GPIO_MODE_PCMCIA_VS1        (1<<24)
#define GPIO_MODE_PCMCIA_CD2        (1<<23)
#define GPIO_MODE_PCMCIA_CD1        (1<<22)
#define GPIO_MODE_PCI_GNT0          (1<<20)
#define GPIO_MODE_PCI_REQ0          (1<<19)
#define GPIO_MODE_PCI_INTB          (1<<18)
#define GPIO_MODE_PCI_GNT1          (1<<17)
#define GPIO_MODE_PCI_REQ1          (1<<16)
#define GPIO_MODE_USBD_LED          (1<<14)
#define GPIO_MODE_ROBOSW_LED1       (1<<13)
#define GPIO_MODE_ROBOSW_LED0       (1<<12)
#define GPIO_MODE_ROBOSW_LED_CLK    (1<<11)
#define GPIO_MODE_ROBOSW_LED_DATA   (1<<10)
#define GPIO_MODE_EPHY3_LED         (1<<9)
#define GPIO_MODE_EPHY2_LED         (1<<8)
#define GPIO_MODE_EPHY1_LED         (1<<7)
#define GPIO_MODE_EPHY0_LED         (1<<6)
#define GPIO_MODE_INET_LED          (1<<5)
#define GPIO_MODE_SERIAL_LED_CLK    (1<<4)
#define GPIO_MODE_SERIAL_LED_DATA   (1<<3)
#define GPIO_MODE_SYS_IRQ           (1<<2)
#define GPIO_MODE_ANALOG_AFE_1      (1<<1)
#define GPIO_MODE_ANALOG_AFE_0      (1<<0)



    uint32      VregConfig;                 /* 1C */
    uint32      AuxLedInterval;             /* 20 */
#define AUX_LED_IN_7            0x80000000
#define AUX_LED_IN_6            0x40000000
#define AUX_LED_IN_5            0x20000000
#define AUX_LED_IN_4            0x10000000
#define AUX_LED_IN_MASK         0xf0000000
#define LED_IN_3                0x08000000
#define LED_IN_2                0x04000000
#define LED_IN_1                0x02000000
#define LED_IN_0                0x01000000
#define AUX_LED_TEST            0x00400000
#define USE_NEW_INTV            0x00200000
#define LED7_LNK_ORAND          0x00100000
#define LED7_LNK_MASK           0x000f0000
#define LED7_LNK_MASK_SHFT      16
#define LED7_ACT_MASK           0x0000f000
#define LED7_ACT_MASK_SHFT      12
#define AUX_FLASH_INTV          0x00000fc0
#define AUX_FLASH_INTV_SHFT     6
#define AUX_BLINK_INTV          0x0000003f
    uint32      AuxLedCtrl;                 /* 24 */
#define AUX_HW_DISAB_7          0x80000000
#define AUX_STROBE_7            0x40000000
#define AUX_MODE_7              0x30000000
#define AUX_MODE_SHFT_7         28
#define AUX_HW_DISAB_6          0x08000000
#define AUX_STROBE_6            0x04000000
#define AUX_MODE_6              0x03000000
#define AUX_MODE_SHFT_6         24
#define AUX_HW_DISAB_5          0x00800000
#define AUX_STROBE_5            0x00400000
#define AUX_MODE_5              0x00300000
#define AUX_MODE_SHFT_5         20
#define AUX_HW_DISAB_4          0x00080000
#define AUX_STROBE_4            0x00040000
#define AUX_MODE_4              0x00030000
#define AUX_MODE_SHFT_4         16
#define AUX_HW_DISAB_3          0x00008000
#define AUX_STROBE_3            0x00004000
#define AUX_MODE_3              0x00003000
#define AUX_MODE_SHFT_3         12
#define AUX_HW_DISAB_2          0x00000800
#define AUX_STROBE_2            0x00000400
#define AUX_MODE_2              0x00000300
#define AUX_MODE_SHFT_2         8
#define AUX_HW_DISAB_1          0x00000080
#define AUX_STROBE_1            0x00000040
#define AUX_MODE_1              0x00000030
#define AUX_MODE_SHFT_1         4
#define AUX_HW_DISAB_0          0x00000008
#define AUX_STROBE_0            0x00000004
#define AUX_MODE_0              0x00000003
#define AUX_MODE_SHFT_0         0

#define LED_STEADY_OFF          0x0
#define LED_FLASH               0x1
#define LED_BLINK               0x2
#define LED_STEADY_ON           0x3

    uint32      TestControl;                /* 28 */

    uint32      VDSLControl;                /* 2C */
#define VDSL_MIPS_POR_RESET		0x00000001
#define VDSL_MIPS_RESET			0x00000002
#define VDSL_CORE_RESET			0x00000004
#define VDSL_CLK_RATIO_SHIFT	8
#define VDSL_CLK_RATIO_MSK		(0x1F << VDSL_CLK_RATIO_SHIFT)
    uint32      RoboSWLEDControl;           /* 30 */
    uint32      RoboSWLEDLSR;               /* 34 */
    uint32      GPIOBaseMode;               /* 38 */
#define EN_PCI_CLK_66           (1<<20)
#define EN_UTO_CLK_FAST         (1<<19)
#define EN_UTO                  (1<<18)
#define EN_GMII2                (1<<17)
#define EN_GMII1                (1<<16)
    uint32      RoboswEphyCtrl;             /* 3C */
#define EPHY_RST_4              0x00000200
#define EPHY_RST_3              0x00000100
#define EPHY_RST_2              0x00000080
#define EPHY_RST_1              0x00000040
#define EPHY_PWR_DOWN_4         0x00000020
#define EPHY_PWR_DOWN_3         0x00000010
#define EPHY_PWR_DOWN_2         0x00000008
#define EPHY_PWR_DOWN_1         0x00000004
    uint32      StrapBus;                   /* 40 */
#define UTOPIA_MASTER_ON        (1<<14)
    uint32      StrapOverride;              /* 44 */
    uint32      unused1[2];                 /* 48 */
    uint32      SerialLed;                  /* 50 */
    uint32      SerialLedCtrl;              /* 54 */
#define SER_LED_BUSY            (1<<3)
#define SER_LED_POLARITY        (1<<2)
#define SER_LED_DIV_1           0
#define SER_LED_DIV_2           1
#define SER_LED_DIV_4           2
#define SER_LED_DIV_8           3
#define SER_LED_DIV_MASK        0x3
#define SER_LED_DIV_SHIFT       0
    uint32    unused2[4];                   /* 58 */
    uint32    DieRevID;                     /* 68 */
    uint32    DiagMemStatus;                /* 6c */
    uint32    DiagSelControl;               /* 70 */
    uint32    DiagReadBack;                 /* 74 */
    uint32    DiagReadBackHi;               /* 78 */
    uint32    DiagMiscControl;              /* 7c */
#define EPHY_SA_RESET_N           0x00000300
#define EPHY_SA_TESTEN            0x00000500
#define EPHY_SA_CLOCK_RESET       0x0000d900
} GpioControl;

#define GPIO ((volatile GpioControl * const) GPIO_BASE)

/* Number to mask conversion macro used for GPIODir and GPIOio */
#define GPIO_NUM_TOTAL_BITS_MASK        0x3f
#define GPIO_NUM_MAX_BITS_MASK          0x1f
#define GPIO_NUM_TO_MASK(X)             ( (((X) & GPIO_NUM_TOTAL_BITS_MASK) < 32) ? (1 << ((X) & GPIO_NUM_MAX_BITS_MASK)) : (0) )

/* Number to mask conversion macro used for GPIODir_high and GPIOio_high */
#define GPIO_NUM_MAX_BITS_MASK_HIGH     0x07
#define GPIO_NUM_TO_MASK_HIGH(X)        ( (((X) & GPIO_NUM_TOTAL_BITS_MASK) >= 32) ? (1 << ((X-32) & GPIO_NUM_MAX_BITS_MASK_HIGH)) : (0) )

/*
** Spi Controller
*/

/* TBD.  Taken from 6358_map.h.  Need to verify for BCM6368. */
typedef struct SpiControl {
  uint16        spiMsgCtl;              /* (0x0) control byte */
#define FULL_DUPLEX_RW                  0
#define HALF_DUPLEX_W                   1
#define HALF_DUPLEX_R                   2
#define SPI_MSG_TYPE_SHIFT              14
#define SPI_BYTE_CNT_SHIFT              0
  byte          spiMsgData[0x21e];      /* (0x02 - 0x21f) msg data */
  byte          unused0[0x1e0];
  byte          spiRxDataFifo[0x220];   /* (0x400 - 0x61f) rx data */
  byte          unused1[0xe0];

  uint16        spiCmd;                 /* (0x700): SPI command */
#define SPI_CMD_NOOP                    0
#define SPI_CMD_SOFT_RESET              1
#define SPI_CMD_HARD_RESET              2
#define SPI_CMD_START_IMMEDIATE         3

#define SPI_CMD_COMMAND_SHIFT           0
#define SPI_CMD_COMMAND_MASK            0x000f

#define SPI_CMD_DEVICE_ID_SHIFT         4
#define SPI_CMD_PREPEND_BYTE_CNT_SHIFT  8
#define SPI_CMD_ONE_BYTE_SHIFT          11
#define SPI_CMD_ONE_WIRE_SHIFT          12
#define SPI_DEV_ID_0                    0
#define SPI_DEV_ID_1                    1
#define SPI_DEV_ID_2                    2
#define SPI_DEV_ID_3                    3

  byte          spiIntStatus;           /* (0x702): SPI interrupt status */
  byte          spiMaskIntStatus;       /* (0x703): SPI masked interrupt status */

  byte          spiIntMask;             /* (0x704): SPI interrupt mask */
#define SPI_INTR_CMD_DONE               0x01
#define SPI_INTR_RX_OVERFLOW            0x02
#define SPI_INTR_INTR_TX_UNDERFLOW      0x04
#define SPI_INTR_TX_OVERFLOW            0x08
#define SPI_INTR_RX_UNDERFLOW           0x10
#define SPI_INTR_CLEAR_ALL              0x1f

  byte          spiStatus;              /* (0x705): SPI status */
#define SPI_RX_EMPTY                    0x02
#define SPI_CMD_BUSY                    0x04
#define SPI_SERIAL_BUSY                 0x08

  byte          spiClkCfg;              /* (0x706): SPI clock configuration */
#define SPI_CLK_0_391MHZ                1
#define SPI_CLK_0_781MHZ                2 /* default */
#define SPI_CLK_1_563MHZ                3
#define SPI_CLK_3_125MHZ                4
#define SPI_CLK_6_250MHZ                5
#define SPI_CLK_12_50MHZ                6
#define SPI_CLK_MASK                    0x07
#define SPI_SSOFFTIME_MASK              0x38
#define SPI_SSOFFTIME_SHIFT             3
#define SPI_BYTE_SWAP                   0x80

  byte          spiFillByte;            /* (0x707): SPI fill byte */
  byte          unused2; 
  byte          spiMsgTail;             /* (0x709): msgtail */
  byte          unused3; 
  byte          spiRxTail;              /* (0x70B): rxtail */
} SpiControl;

#define SPI ((volatile SpiControl * const) SPI_BASE)


/*
** Pcm Controller
*/

typedef struct PcmControlRegisters
{
    uint32 pcm_ctrl;                            // 00 offset from PCM_BASE
#define  PCM_ENABLE              0x80000000     // PCM block master enable
#define  PCM_ENABLE_SHIFT        31
#define  PCM_SLAVE_SEL           0x40000000     // PCM TDM slave mode select (1 - TDM slave, 0 - TDM master)
#define  PCM_SLAVE_SEL_SHIFT     30
#define  PCM_CLOCK_INV           0x20000000     // PCM SCLK invert select (1 - invert, 0 - normal)
#define  PCM_CLOCK_INV_SHIFT     29
#define  PCM_FS_INVERT           0x10000000     // PCM FS invert select (1 - invert, 0 - normal)
#define  PCM_FS_INVERT_SHIFT     28
#define  PCM_FS_FREQ_16_8        0x08000000     // PCM FS 16/8 Khz select (1 - 16Khz, 0 - 8Khz)
#define  PCM_FS_FREQ_16_8_SHIFT  27
#define  PCM_FS_LONG             0x04000000     // PCM FS long/short select (1 - long FS, 0 - short FS)
#define  PCM_FS_LONG_SHIFT       26
#define  PCM_FS_TRIG             0x02000000     // PCM FS trigger (1 - falling edge, 0 - rising edge trigger)
#define  PCM_FS_TRIG_SHIFT       25
#define  PCM_DATA_OFF            0x01000000     // PCM data offset from FS (1 - one clock from FS, 0 - no offset)
#define  PCM_DATA_OFF_SHIFT      24
#define  PCM_DATA_16_8           0x00800000     // PCM data word length (1 - 16 bits, 0 - 8 bits)
#define  PCM_DATA_16_8_SHIFT     23
#define  PCM_CLOCK_SEL           0x00700000     // PCM SCLK freq select
#define  PCM_CLOCK_SEL_SHIFT     20
                                                  // 000 - 8192 Khz
                                                  // 001 - 4096 Khz
                                                  // 010 - 2048 Khz
                                                  // 011 - 1024 Khz
                                                  // 100 - 512 Khz
                                                  // 101 - 256 Khz
                                                  // 110 - 128 Khz
                                                  // 111 - reserved
#define  PCM_LSB_FIRST           0x00040000     // PCM shift direction (1 - LSBit first, 0 - MSBit first)
#define  PCM_LSB_FIRST_SHIFT     18
#define  PCM_LOOPBACK            0x00020000     // PCM diagnostic loobback enable
#define  PCM_LOOPBACK_SHIFT      17
#define  PCM_EXTCLK_SEL          0x00010000     // PCM external timing clock select -- Maybe removed in 6368
#define  PCM_EXTCLK_SEL_SHIFT    16
#define  PCM_NTR_ENABLE          0x00008000     // PCM NTR counter enable -- Nayve removed in 6368
#define  PCM_NTR_ENABLE_SHIFT    15
#define  PCM_BITS_PER_FRAME_1024 0x00000400     // 1024 - Max
#define  PCM_BITS_PER_FRAME_256  0x00000100     // 256
#define  PCM_BITS_PER_FRAME_8    0x00000008     // 8    - Min

    uint32 pcm_chan_ctrl;                       // 04
#define  PCM_TX0_EN              0x00000001     // PCM transmit channel 0 enable
#define  PCM_TX1_EN              0x00000002     // PCM transmit channel 1 enable
#define  PCM_TX2_EN              0x00000004     // PCM transmit channel 2 enable
#define  PCM_TX3_EN              0x00000008     // PCM transmit channel 3 enable
#define  PCM_TX4_EN              0x00000010     // PCM transmit channel 4 enable
#define  PCM_TX5_EN              0x00000020     // PCM transmit channel 5 enable
#define  PCM_TX6_EN              0x00000040     // PCM transmit channel 6 enable
#define  PCM_TX7_EN              0x00000080     // PCM transmit channel 7 enable
#define  PCM_RX0_EN              0x00000100     // PCM receive channel 0 enable
#define  PCM_RX1_EN              0x00000200     // PCM receive channel 1 enable
#define  PCM_RX2_EN              0x00000400     // PCM receive channel 2 enable
#define  PCM_RX3_EN              0x00000800     // PCM receive channel 3 enable
#define  PCM_RX4_EN              0x00001000     // PCM receive channel 4 enable
#define  PCM_RX5_EN              0x00002000     // PCM receive channel 5 enable
#define  PCM_RX6_EN              0x00004000     // PCM receive channel 6 enable
#define  PCM_RX7_EN              0x00008000     // PCM receive channel 7 enable
#define  PCM_RX_PACKET_SIZE      0x00ff0000     // PCM Rx DMA quasi-packet size
#define  PCM_RX_PACKET_SIZE_SHIFT  16

    uint32 pcm_int_pending;                     // 08
    uint32 pcm_int_mask;                        // 0c
#define  PCM_TX_UNDERFLOW        0x00000001     // PCM DMA receive overflow
#define  PCM_RX_OVERFLOW         0x00000002     // PCM DMA transmit underflow
#define  PCM_TDM_FRAME           0x00000004     // PCM frame boundary
#define  PCM_RX_IRQ              0x00000008     // IUDMA interrupts
#define  PCM_TX_IRQ              0x00000010

    uint32 pcm_pll_ctrl1;                       // 10
#define  PCM_PLL_PWRDN           0x80000000     // PLL PWRDN
#define  PCM_PLL_PWRDN_CH1       0x40000000     // PLL CH PWRDN
#define  PCM_PLL_REFCMP_PWRDN    0x20000000     // PLL REFCMP PWRDN
#define  PCM_CLK16_RESET         0x10000000     // 16.382 MHz PCM interface circuitry reset. 
#define  PCM_PLL_ARESET          0x08000000     // PLL Analog Reset
#define  PCM_PLL_DRESET          0x04000000     // PLL Digital Reset

    uint32 pcm_pll_ctrl2;                       // 14
    uint32 pcm_pll_ctrl3;                       // 18
    uint32 pcm_pll_ctrl4;                       // 1c

    uint32 pcm_pll_stat;                        // 20
#define  PCM_PLL_LOCK            0x00000001     // Asserted when PLL is locked to programmed frequency

    uint32 pcm_ntr_counter;                     // 24

    uint32 unused[6];

#define  PCM_MAX_TIMESLOT_REGS   16             // Number of PCM time slot registers in the table.
                                                // Each register provides the settings for 8 timeslots (4 bits per timeslot)
    uint32 pcm_slot_alloc_tbl[PCM_MAX_TIMESLOT_REGS];
#define  PCM_TS_VALID            0x8            // valid marker for TS alloc ram entry
} PcmControlRegisters;

#define PCM ((volatile PcmControlRegisters * const) PCM_BASE)

typedef struct PcmIudmaRegisters
{
	uint16 reserved0;
   uint16 ctrlConfig;
#define BCM6368_IUDMA_REGS_CTRLCONFIG_MASTER_EN        0x0001
#define BCM6368_IUDMA_REGS_CTRLCONFIG_FLOWC_CH1_EN     0x0002
#define BCM6368_IUDMA_REGS_CTRLCONFIG_FLOWC_CH3_EN     0x0004
#define BCM6368_IUDMA_REGS_CTRLCONFIG_FLOWC_CH5_EN     0x0008
#define BCM6368_IUDMA_REGS_CTRLCONFIG_FLOWC_CH7_EN     0x0010

	// Flow control Ch1
   uint16 reserved1;
   uint16 ch1_FC_Low_Thr;

   uint16 reserved2;
   uint16 ch1_FC_High_Thr;

   uint16 reserved3;
   uint16 ch1_Buff_Alloc;

	// Flow control Ch3
	uint16 reserved4;
	uint16 ch3_FC_Low_Thr;

	uint16 reserved5;
	uint16 ch3_FC_High_Thr;

	uint16 reserved6;
	uint16 ch3_Buff_Alloc;

	// Flow control Ch5
	uint16 reserved7;
	uint16 ch5_FC_Low_Thr;

	uint16 reserved8;
	uint16 ch5_FC_High_Thr;

	uint16 reserved9;
	uint16 ch5_Buff_Alloc;

	// Flow control Ch7
	uint16 reserved10;
	uint16 ch7_FC_Low_Thr;

	uint16 reserved11;
	uint16 ch7_FC_High_Thr;

	uint16 reserved12;
	uint16 ch7_Buff_Alloc;

	// Channel resets
	uint16 reserved13;
	uint16 channel_reset;
	
	uint16 reserved14;
	uint16 channel_debug;
	
	// Spare register
	uint32 dummy1;
	
	// Interrupt status registers
	uint16 reserved15;
	uint16 gbl_int_stat;
	
	// Interrupt mask registers
	uint16 reserved16;
	uint16 gbl_int_mask;		
} PcmIudmaRegisters;

typedef struct PcmIudmaChannelCtrl
{
   uint16 reserved1;
	uint16 config;
#define BCM6368_IUDMA_CONFIG_ENDMA       0x0001
#define BCM6368_IUDMA_CONFIG_PKTHALT     0x0002
#define BCM6368_IUDMA_CONFIG_BURSTHALT   0x0004

	uint16 reserved2;
	uint16 intStat;
#define BCM6368_IUDMA_INTSTAT_BDONE   0x0001
#define BCM6368_IUDMA_INTSTAT_PDONE   0x0002
#define BCM6368_IUDMA_INTSTAT_NOTVLD  0x0004
#define BCM6368_IUDMA_INTSTAT_MASK    0x0007
#define BCM6368_IUDMA_INTSTAT_ALL     BCM6368_IUDMA_INTSTAT_MASK

	uint16 reserved3;
	uint16 intMask;
#define BCM6368_IUDMA_INTMASK_BDONE   0x0001
#define BCM6368_IUDMA_INTMASK_PDONE   0x0002
#define BCM6368_IUDMA_INTMASK_NOTVLD  0x0004

	uint32 maxBurst;
#define BCM6368_IUDMA_MAXBURST_SIZE 16 /* 32-bit words */

} PcmIudmaChannelCtrl;


typedef struct PcmIudmaStateRam
{
   uint32 baseDescPointer;                /* pointer to first buffer descriptor */

   uint32 stateBytesDoneRingOffset;       /* state info: how manu bytes done and the offset of the
                                             current descritor in process */
#define BCM6368_IUDMA_STRAM_DESC_RING_OFFSET 0x3fff


   uint32 flagsLengthStatus;              /* Length and status field of the current descriptor */

   uint32 currentBufferPointer;           /* pointer to the current descriptor */

} PcmIudmaStateRam;

#define BCM6368_MAX_PCM_DMA_CHANNELS 2

typedef struct PcmIudma
{
   PcmIudmaRegisters regs;                                        // 
   uint32 reserved1[110];                                         //            
   PcmIudmaChannelCtrl ctrl[BCM6368_MAX_PCM_DMA_CHANNELS];        //
   uint32 reserved2[120];                                         //
   PcmIudmaStateRam stram[BCM6368_MAX_PCM_DMA_CHANNELS];          //

} PcmIudma;

#define PCM_IUDMA ((volatile PcmIudma * const) PCM_DMA_BASE)


#define IUDMA_MAX_CHANNELS          32

/*
** DMA Channel Configuration (1 .. 32)
*/
typedef struct DmaChannelCfg {
  uint32        cfg;                    /* (00) assorted configuration */
#define         DMA_ENABLE      0x00000001  /* set to enable channel */
#define         DMA_PKT_HALT    0x00000002  /* idle after an EOP flag is detected */
#define         DMA_BURST_HALT  0x00000004  /* idle after finish current memory burst */
  uint32        intStat;                /* (04) interrupts control and status */
  uint32        intMask;                /* (08) interrupts mask */
#define         DMA_BUFF_DONE   0x00000001  /* buffer done */
#define         DMA_DONE        0x00000002  /* packet xfer complete */
#define         DMA_NO_DESC     0x00000004  /* no valid descriptors */
  uint32        maxBurst;               /* (0C) max burst length permitted */
} DmaChannelCfg;

/*
** DMA State RAM (1 .. 16)
*/
typedef struct DmaStateRam {
  uint32        baseDescPtr;            /* (00) descriptor ring start address */
  uint32        state_data;             /* (04) state/bytes done/ring offset */
  uint32        desc_len_status;        /* (08) buffer descriptor status and len */
  uint32        desc_base_bufptr;       /* (0C) buffer descrpitor current processing */
} DmaStateRam;


/*
** DMA Registers
*/
typedef struct DmaRegs {
    uint32 controller_cfg;              /* (00) controller configuration */
#define DMA_MASTER_EN           0x00000001
#define DMA_FLOWC_CH1_EN        0x00000002
#define DMA_FLOWC_CH3_EN        0x00000004

    // Flow control Ch1
    uint32 flowctl_ch1_thresh_lo;           /* 004 */
    uint32 flowctl_ch1_thresh_hi;           /* 008 */
    uint32 flowctl_ch1_alloc;               /* 00c */
#define DMA_BUF_ALLOC_FORCE     0x80000000

    // Flow control Ch3
    uint32 flowctl_ch3_thresh_lo;           /* 010 */
    uint32 flowctl_ch3_thresh_hi;           /* 014 */
    uint32 flowctl_ch3_alloc;               /* 018 */

    // Flow control Ch5
    uint32 flowctl_ch5_thresh_lo;           /* 01C */
    uint32 flowctl_ch5_thresh_hi;           /* 020 */
    uint32 flowctl_ch5_alloc;               /* 024 */

    // Flow control Ch7
    uint32 flowctl_ch7_thresh_lo;           /* 028 */
    uint32 flowctl_ch7_thresh_hi;           /* 02C */
    uint32 flowctl_ch7_alloc;               /* 030 */

    uint32 ctrl_channel_reset;              /* 034 */
    uint32 ctrl_channel_debug;              /* 038 */
    uint32 reserved1;                       /* 03C */
    uint32 ctrl_global_interrupt_status;    /* 040 */
    uint32 ctrl_global_interrupt_mask;      /* 044 */

    // Unused words
    uint8 reserved2[0x200-0x48];

    // Per channel registers/state ram
    DmaChannelCfg chcfg[IUDMA_MAX_CHANNELS];/* (200-3FF) Channel configuration */
    union {
        DmaStateRam     s[IUDMA_MAX_CHANNELS];
        uint32          u32[4 * IUDMA_MAX_CHANNELS];
    } stram;                                /* (400-5FF) state ram */
} DmaRegs;

/*
** DMA Buffer 
*/
typedef struct DmaDesc {
  uint16        length;                 /* in bytes of data in buffer */
#define          DMA_DESC_USEFPM    0x8000
#define          DMA_DESC_MULTICAST 0x4000
#define          DMA_DESC_BUFLENGTH 0x0fff
  uint16        status;                 /* buffer status */
#define          DMA_OWN                0x8000  /* cleared by DMA, set by SW */
#define          DMA_EOP                0x4000  /* last buffer in packet */
#define          DMA_SOP                0x2000  /* first buffer in packet */
#define          DMA_WRAP               0x1000  /* */
#define          DMA_APPEND_BRCM_TAG    0x0200
#define          DMA_APPEND_CRC         0x0100
#define          USB_ZERO_PKT           (1<< 0) // Set to send zero length packet

  uint32        address;                /* address of data */
} DmaDesc;

/*
** External Bus Interface
*/
typedef struct EbiChipSelect {
    uint32    base;                         /* base address in upper 24 bits */
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
    uint32    config;
#define EBI_ENABLE          0x00000001      /* .. enable this range */
#define EBI_WAIT_STATES     0x0000000e      /* .. mask for wait states */
#define EBI_WTST_SHIFT      1               /* .. for shifting wait states */
#define EBI_WORD_WIDE       0x00000010      /* .. 16-bit peripheral, else 8 */
#define EBI_WREN            0x00000020      /* enable posted writes */
#define EBI_POLARITY        0x00000040      /* .. set to invert something, 
                                        **    don't know what yet */
#define EBI_TS_TA_MODE      0x00000080      /* .. use TS/TA mode */
#define EBI_TS_SEL          0x00000100      /* .. drive tsize, not bs_b */
#define EBI_FIFO            0x00000200      /* .. use fifo */
#define EBI_RE              0x00000400      /* .. Reverse Endian */
#define EBI_SETUP_SHIFT     16
#define EBI_HOLD_SHIFT      20
#define EBI_SETUP_STATES    0x0f0000
#define EBI_HOLD_STATES     0xf00000
} EbiChipSelect;

typedef struct MpiRegisters {
  EbiChipSelect cs[7];                  /* size chip select configuration */
#define EBI_CS0_BASE            0
#define EBI_CS1_BASE            1
#define EBI_CS2_BASE            2
#define EBI_CS3_BASE            3
#define PCMCIA_COMMON_BASE      4
#define PCMCIA_ATTRIBUTE_BASE   5
#define PCMCIA_IO_BASE          6
  uint32        unused0[2];             /* reserved */
  uint32        ebi_control;            /* ebi control */
  uint32        unused1[4];             /* reserved */
#define EBI_ACCESS_TIMEOUT      0x000007FF
  uint32        pcmcia_cntl1;           /* pcmcia control 1 */
#define PCCARD_CARD_RESET       0x00040000
#define CARDBUS_ENABLE          0x00008000
#define PCMCIA_ENABLE           0x00004000
#define PCMCIA_GPIO_ENABLE      0x00002000
#define CARDBUS_IDSEL           0x00001F00
#define VS2_OEN                 0x00000080
#define VS1_OEN                 0x00000040
#define VS2_OUT                 0x00000020
#define VS1_OUT                 0x00000010
#define VS2_IN                  0x00000008
#define VS1_IN                  0x00000004
#define CD2_IN                  0x00000002
#define CD1_IN                  0x00000001
#define VS_MASK                 0x0000000C
#define CD_MASK                 0x00000003
  uint32        unused2;                /* reserved */
  uint32        pcmcia_cntl2;           /* pcmcia control 2 */
#define PCMCIA_BYTESWAP_DIS     0x00000002
#define PCMCIA_HALFWORD_EN      0x00000001
#define RW_ACTIVE_CNT_BIT       2
#define INACTIVE_CNT_BIT        8
#define CE_SETUP_CNT_BIT        16
#define CE_HOLD_CNT_BIT         24
  uint32        unused3[40];            /* reserved */

  uint32        sp0range;               /* PCI to internal system bus address space */
#define ADDR_SPACE_MASK         0xFFFF0000
  uint32        sp0remap;
  uint32        sp0cfg;
  uint32        sp1range;
  uint32        sp1remap;
  uint32        sp1cfg;

  uint32        EndianCfg;

  uint32        l2pcfgctl;              /* internal system bus to PCI IO/Cfg control */
#define DIR_CFG_SEL             0x80000000 /* change from PCI I/O access to PCI config access */
#define DIR_CFG_USEREG          0x40000000 /* use this register info for PCI configuration access */
#define DEVICE_NUMBER           0x00007C00 /* device number for the PCI configuration access */
#define FUNC_NUMBER             0x00000300 /* function number for the PCI configuration access */
#define REG_NUMBER              0x000000FC /* register number for the PCI configuration access */
#define CONFIG_TYPE             0x00000003 /* configuration type for the PCI configuration access */

  uint32        l2pmrange1;             /* internal system bus to PCI memory space */
#define PCI_SIZE_64K            0xFFFF0000
#define PCI_SIZE_128K           0xFFFE0000
#define PCI_SIZE_256K           0xFFFC0000
#define PCI_SIZE_512K           0xFFF80000
#define PCI_SIZE_1M             0xFFF00000
#define PCI_SIZE_2M             0xFFE00000
#define PCI_SIZE_4M             0xFFC00000
#define PCI_SIZE_8M             0xFF800000
#define PCI_SIZE_16M            0xFF000000
#define PCI_SIZE_32M            0xFE000000
  uint32        l2pmbase1;              /* kseg0 or kseg1 address & 0x1FFFFFFF */
  uint32        l2pmremap1;
#define CARDBUS_MEM             0x00000004
#define MEM_WINDOW_EN           0x00000001
  uint32        l2pmrange2;
  uint32        l2pmbase2;
  uint32        l2pmremap2;
  uint32        l2piorange;             /* internal system bus to PCI I/O space */
  uint32        l2piobase;
  uint32        l2pioremap;

  uint32        pcimodesel;
#define PCI_INT_BUS_RD_PREFETCH 0x000001F0
#define PCI_BAR2_NOSWAP         0x00000002 /* BAR at offset 0x20 */
#define PCI_BAR1_NOSWAP         0x00000001 /* BAR at affset 0x1c */

  uint32        pciintstat;             /* PCI interrupt mask/status */
#define MAILBOX1_SENT           0x08
#define MAILBOX0_SENT           0x04
#define MAILBOX1_MSG_RCV        0x02
#define MAILBOX0_MSG_RCV        0x01
  uint32        locbuscntrl;            /* internal system bus control */
#define DIR_U2P_NOSWAP          0x00000002
#define EN_PCI_GPIO             0x00000001
  uint32        locintstat;             /* internal system bus interrupt mask/status */
#define CSERR                   0x0200
#define SERR                    0x0100
#define EXT_PCI_INT             0x0080
#define DIR_FAILED              0x0040
#define DIR_COMPLETE            0x0020
#define PCI_CFG                 0x0010
  uint32        unused4[7];

  uint32        mailbox0;
  uint32        mailbox1;

  uint32        pcicfgcntrl;            /* internal system bus PCI configuration control */
#define PCI_CFG_REG_WRITE_EN    0x00000080
#define PCI_CFG_ADDR            0x0000003C
  uint32        pcicfgdata;             /* internal system bus PCI configuration data */

  uint32        locch2ctl;              /* PCI to interrnal system bus DMA (downstream) local control */
#define MPI_DMA_HALT            0x00000008  /* idle after finish current memory burst */
#define MPI_DMA_PKT_HALT        0x00000004  /* idle after an EOP flag is detected */
#define MPI_DMA_STALL           0x00000002  /* idle after an EOP flag is detected */
#define MPI_DMA_ENABLE          0x00000001  /* set to enable channel */
  uint32        locch2intStat;
#define MPI_DMA_NO_DESC         0x00000004  /* no valid descriptors */
#define MPI_DMA_DONE            0x00000002  /* packet xfer complete */
#define MPI_DMA_BUFF_DONE       0x00000001  /* buffer done */
  uint32        locch2intMask;
  uint32        unused5;
  uint32        locch2descaddr;
  uint32        locch2status1;
#define LOCAL_DESC_STATE        0xE0000000
#define PCI_DESC_STATE          0x1C000000
#define BYTE_DONE               0x03FFC000
#define RING_ADDR               0x00003FFF
  uint32        locch2status2;
#define BUFPTR_OFFSET           0x1FFF0000
#define PCI_MASTER_STATE        0x000000C0
#define LOC_MASTER_STATE        0x00000038
#define CONTROL_STATE           0x00000007
  uint32        unused6;

  uint32        locch1Ctl;              /*internal system bus to PCI DMA (upstream) local control */
#define DMA_U2P_LE              0x00000200  /* local bus is little endian */
#define DMA_U2P_NOSWAP          0x00000100  /* lccal bus is little endian but no data swapped */
  uint32        locch1intstat;
  uint32        locch1intmask;
  uint32        unused7;
  uint32        locch1descaddr;
  uint32        locch1status1;
  uint32        locch1status2;
  uint32        unused8;

  uint32        pcich1ctl;              /* internal system bus to PCI DMA PCI control */
  uint32        pcich1intstat;
  uint32        pcich1intmask;
  uint32        pcich1descaddr;
  uint32        pcich1status1;
  uint32        pcich1status2;

  uint32        pcich2Ctl;              /* PCI to internal system bus DMA PCI control */
  uint32        pcich2intstat;
  uint32        pcich2intmask;
  uint32        pcich2descaddr;
  uint32        pcich2status1;
  uint32        pcich2status2;

  uint32        perm_id;                /* permanent device and vendor id */
  uint32        perm_rev;               /* permanent revision id */
} MpiRegisters;

#define MPI ((volatile MpiRegisters * const) MPI_BASE)

/* PCI configuration address space start offset 0x40 */
#define BRCM_PCI_CONFIG_TIMER               0x40
#define BRCM_PCI_CONFIG_TIMER_RETRY_MASK	0x0000FF00
#define BRCM_PCI_CONFIG_TIMER_TRDY_MASK     0x000000FF
/*
** USB 2.0 Device Registers
*/
typedef struct UsbRegisters {
#define USBD_CONTROL_APP_RESUME     0x0002
    uint32 usbd_control;
#define USBD_STRAPS_APP_SELF_PWR    0x0400
#define USBD_STRAPS_APP_DEV_DISCON  0x0200
#define USBD_STRAPS_APP_CSRPRG_SUP  0x0100
#define USBD_STRAPS_APP_RAM_IF      0x0080
#define USBD_STRAPS_APP_DEV_RMTWKUP 0x0040
#define USBD_STRAPS_APP_PHYIF_8BIT  0x0004
#define USBD_STRAPS_FULL_SPEED      0x0003
#define USBD_STRAPS_LOW_SPEED	   	0x0002
#define USBD_STRAPS_HIGH_SPEED   	0x0000
    uint32 usbd_straps;
#define USB_ENDPOINT_0  0x01
    uint32 usbd_stall;
#define USBD_ENUM_SPEED_SHIFT    	12
#define USBD_ENUM_SPEED    			0x3000
    uint32 usbd_status;
#define USBD_SET_CSRS      			0x40
#define USBD_SUSPEND       			0x20
#define USBD_EARLY_SUSPEND 			0x10
#define USBD_SOF           			0x08
#define USBD_ENUMON        			0x04
#define USBD_SETUP         			0x02
#define USBD_USBRESET      			0x01
    uint32 usbd_events;
    uint32 usbd_events_irq;
#define USBD_USB_RESET_IRQ          0x00000001
#define USBD_USB_SETUP_IRQ       	0x00000002 // non-standard setup cmd rcvd
#define USBD_USB_ENUM_ON_IRQ        0x00000004
#define USBD_USB_SOF_IRQ            0x00000008
#define USBD_USB_EARLY_SUSPEND_IRQ  0x00000010
#define USBD_USB_SUSPEND_IRQ     	0x00000020 // non-standard setup cmd rcvd
#define USBD_USB_SET_CSRS_IRQ       0x00000040
#define USBD_USB_ERRATIC_ERR_IRQ    0x00000080
    uint32 usbd_events_irq_mask;
#define USBD_IRQCFG_ENUM_ON_FALLING_EDGE 0x00000010
    uint32 usbd_irqcfg;
	 uint32 usbd_debug;
    uint32 usbd_swcfg;
    uint32 usbd_swtxctl;
    uint32 usbd_swrxctl;
    uint32 usbd_txfifo_rwptr;
    uint32 usbd_rxfifo_rwptr;
    uint32 usbd_txfifo_st_rwptr;
    uint32 usbd_rxfifo_st_rwptr;
    uint32 usbd_txfifo0_start_stop;
    uint32 usbd_txfifo1_start_stop;
    uint32 usbd_txfifo2_start_stop;
    uint32 reserved[0x0D];
    uint32 usbd_rxfifo0_start_stop;
    uint32 usbd_rxfifo1_start_stop;
    uint32 usbd_rxfifo2_start_stop;
} UsbRegisters;

#define USB ((volatile UsbRegisters * const) USB_CTL_BASE)

typedef struct USBControl {
    uint32 BrtControl1;
    uint32 BrtControl2;
    uint32 BrtStatus1;
    uint32 BrtStatus2;
    uint32 UTMIControl1;
    uint32 TestPortControl;
    uint32 PllControl1;
    uint32 SwapControl;
#define EHCI_LOGICAL_ADDRESS_EN (1<<5)
#define EHCI_ENDIAN_SWAP        (1<<4)
#define EHCI_DATA_SWAP          (1<<3)
#define OHCI_LOGICAL_ADDRESS_EN (1<<2)
#define OHCI_ENDIAN_SWAP        (1<<1)
#define OHCI_DATA_SWAP          (1<<0)
    uint32 unused1;
    uint32 FrameAdjustValue;
    uint32 Setup;
#define USBH_IOC				(1<<4)
    uint32 MDIO;
    uint32 MDIO32;
    uint32 USBSimControl;
} USBControl;

#define USBH ((volatile USBControl * const) USBH_CFG_BASE)

#ifdef __cplusplus
}
#endif

#endif


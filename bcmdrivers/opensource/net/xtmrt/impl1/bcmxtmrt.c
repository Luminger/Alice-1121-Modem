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
/**************************************************************************
 * File Name  : bcmxtmrt.c
 *
 * Description: This file implements BCM6368 ATM/PTM network device driver
 *              runtime processing - sending and receiving data.
 ***************************************************************************/


/* Defines. */
#define CARDNAME    "bcmxtmrt"
#define VERSION     "0.1"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__


/* Includes. */
#include <bcmtypes.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <linux/ethtool.h>
#include <bcm_map_part.h>
#include <bcm_intr.h>
#include <board.h>
#include "bcmnet.h"
#include "bcmxtmcfg.h"
#include "bcmxtmrt.h"
#include "bcmxtmrtimpl.h"
#include <asm/io.h>
#include <asm/r4kcache.h>
#include <asm/uaccess.h>


/* Globals. */
static BCMXTMRT_GLOBAL_INFO g_GlobalInfo;


/* Prototypes. */
static inline void cache_wbflush_region(void *addr, void *end);
static inline void cache_wbflush_len(void *addr, int len);
int __init bcmxtmrt_init( void );
static void bcmxtmrt_cleanup( void );
static int bcmxtmrt_open( struct net_device *dev );
static int bcmxtmrt_close( struct net_device *dev );
static void bcmxtmrt_timeout( struct net_device *dev );
static struct net_device_stats *bcmxtmrt_query(struct net_device *dev);
static int bcmxtmrt_ioctl(struct net_device *dev, struct ifreq *Req, int nCmd);
static int bcmxtmrt_ethtool_ioctl(PBCMXTMRT_DEV_CONTEXT pDevCtx,void *useraddr);
#ifdef ATM_BONDING_TEST
static void sendAsmOnPort (PBCMXTMRT_DEV_CONTEXT pDevCtx);
#endif
static int bcmxtmrt_xmit( struct sk_buff *skb, struct net_device *dev);
static void AddRfc2684Hdr(struct sk_buff **ppskb,PBCMXTMRT_DEV_CONTEXT pDevCtx);
static void AssignRxBuffer(PRXBDINFO pRxBdInfo, UINT8 *pucData, UINT8 *pEnd);
static void bcmxtmrt_freeskbordata( PRXBDINFO pRxBdInfo, struct sk_buff *skb,
    unsigned nFlag );
static irqreturn_t bcmxtmrt_rxisr(int nIrq, void *pRxDma, struct pt_regs *pRegs);
static void bcmxtmrt_rxtasklet( PBCMXTMRT_GLOBAL_INFO pGi );
static void ProcessRxCell( PBCMXTMRT_GLOBAL_INFO pGi, PBCMXTMRT_DEV_CONTEXT
    pDevCtx, PRXBDINFO pRxBdInfo, UINT8 *pucData );
static void bcmxtmrt_timer( PBCMXTMRT_GLOBAL_INFO pGi );
int bcmxtmrt_request( PBCMXTMRT_DEV_CONTEXT pDevCtx, UINT32 ulCommand,
    void *pParm );
static int DoGlobInitReq( PXTMRT_GLOBAL_INIT_PARMS pGip );
static int DoCreateDeviceReq( PXTMRT_CREATE_NETWORK_DEVICE pCnd );
static int DoRegCellHdlrReq( PXTMRT_CELL_HDLR pCh );
static int DoUnregCellHdlrReq( PXTMRT_CELL_HDLR pCh );
static int DoLinkStsChangedReq( PBCMXTMRT_DEV_CONTEXT pDevCtx,
     PXTMRT_LINK_STATUS_CHANGE pLsc );
static int DoLinkUp( PBCMXTMRT_DEV_CONTEXT pDevCtx,
     PXTMRT_LINK_STATUS_CHANGE pLsc );
static int DoLinkDown( PBCMXTMRT_DEV_CONTEXT pDevCtx,
     PXTMRT_LINK_STATUS_CHANGE pLsc );
static int DoSendCellReq( PBCMXTMRT_DEV_CONTEXT pDevCtx, PXTMRT_CELL pC );
static int DoDeleteDeviceReq( PBCMXTMRT_DEV_CONTEXT pDevCtx );

/*
 *  Data integrity and cache management.
 *          - Prior to xmit, the modified portion of a data buffer that is
 *            to be transmitted needs to be wback flushed so that the data
 *            in the buffer is coherent with that in the L2 Cache.
 *            The region is demarcated by
 *                skb->data and skb->len
 *          - When a buffer is recycled, the buffer region that is handed to
 *            the DMA controller needs to be invalidated in L2 Cache.
 *            The region is demarcated by
 *                skb->head + RESERVED HEADROOM and skb->end + skb_shared_info
 *          - Given that the entire skb_shared_info is not accessed,
 *            e.g. the skb_frag_t array is only accessed if nr_frags is > 0,
 *            it is sufficient to only flush/invalidate a portion of the
 *            skb_shared_info that is placed after the skb->end.
 *
 *  Cache operations reworked so as to not perform any operation beyond
 *  the "end" demarcation.
 */

/*
 * Macros to round down and up, an address to a cachealigned address
 */
#define ROUNDDN(addr, align)  ( (addr) & ~((align) - 1) )
#define ROUNDUP(addr, align)  ( ((addr) + (align) - 1) & ~((align) - 1) )


/***************************************************************************
 * Function Name: cache_wbflush_region
 * Description  : Do MIPS flush cache operation on a buffer.
 *                if (addr == end) && (addr is cache aligned) no operation.
 *                if (addr == end) && (addr is not cache aligned) flush.
 *
 *                addr is rounded down to cache line.
 *                end is rounded up to cache line.
 *
 *                All cache lines from addr, NOT INCLUDING end are flushed. 
 * Returns      : None.
 ***************************************************************************/
static inline void cache_wbflush_region(void *addr, void *end)
{
    unsigned long dc_lsize = current_cpu_data.dcache.linesz;
    unsigned long a = ROUNDDN( (unsigned long)addr, dc_lsize );
    unsigned long e = ROUNDUP( (unsigned long)end, dc_lsize );
    while ( a < e )
    {
        flush_dcache_line(a);   /* Hit_Writeback_Inv_D */
        a += dc_lsize;
    }
}


/***************************************************************************
 * Function Name: cache_invalidate
 * Description  : Do MIPS invalidate cache operation on a buffer.
 *                if (len == 0) && (addr is cache aligned) then no operation.
 *                if (len == 0) && (addr is not cache aligned) then flush !
 *                end = addr + len, then rounded up to cacheline
 *                addr is rounded down to cache line.
 *                All cache lines from addr, NOT INCLUDING end are flushed.
 * Returns      : None.
 ***************************************************************************/
static inline void cache_wbflush_len(void *addr, int len)
{
    unsigned long dc_lsize = current_cpu_data.dcache.linesz;
    unsigned long a = ROUNDDN( (unsigned long)addr, dc_lsize );
    unsigned long e = ROUNDUP( ((unsigned long)addr + len), dc_lsize );
    while ( a < e )
    {
        flush_dcache_line(a);   /* Hit_Writeback_Inv_D */
        a += dc_lsize;
    }
}


/***************************************************************************
 * Function Name: bcmxtmrt_init
 * Description  : Called when the driver is loaded.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
int __init bcmxtmrt_init( void )
{
    UINT16 usChipId  = (PERF->RevID & 0xFFFF0000) >> 16;
    UINT16 usChipRev = (PERF->RevID & 0xFF);

    printk(CARDNAME ": Broadcom BCM%X%X ATM/PTM Network Device ", usChipId,
        usChipRev);
    printk(VER_STR "\n");

    memset(&g_GlobalInfo, 0x00, sizeof(g_GlobalInfo));

    return( 0 );
} /* bcmxtmrt_init */


/***************************************************************************
 * Function Name: bcmxtmrt_cleanup
 * Description  : Called when the driver is unloaded.
 * Returns      : None.
 ***************************************************************************/
static void bcmxtmrt_cleanup( void )
{
} /* bcmxtmrt_cleanup */


/***************************************************************************
 * Function Name: bcmxtmrt_open
 * Description  : Called to make the device operational.  Called due to shell
 *                command, "ifconfig <device_name> up".
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int bcmxtmrt_open( struct net_device *dev )
{
    int nRet = 0;
    PBCMXTMRT_DEV_CONTEXT pDevCtx = dev->priv;

    if( pDevCtx->ulAdminStatus == ADMSTS_UP )
        pDevCtx->ulOpenState = XTMRT_DEV_OPENED;
    else
        nRet = -EIO;

    netif_start_queue(dev);

    return( nRet );
} /* bcmxtmrt_open */


/***************************************************************************
 * Function Name: bcmxtmrt_close
 * Description  : Called to stop the device.  Called due to shell command,
 *                "ifconfig <device_name> down".
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int bcmxtmrt_close( struct net_device *dev )
{
    PBCMXTMRT_DEV_CONTEXT pDevCtx = dev->priv;

    pDevCtx->ulOpenState = XTMRT_DEV_CLOSED;
    netif_stop_queue(dev);
    return 0;
} /* bcmxtmrt_close */


/***************************************************************************
 * Function Name: bcmxtmrt_timeout
 * Description  : Called when there is a transmit timeout. 
 * Returns      : None.
 ***************************************************************************/
static void bcmxtmrt_timeout( struct net_device *dev )
{
    dev->trans_start = jiffies;
    netif_wake_queue(dev);
} /* bcmxtmrt_timeout */


/***************************************************************************
 * Function Name: bcmxtmrt_query
 * Description  : Called to return device statistics. 
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static struct net_device_stats *bcmxtmrt_query(struct net_device *dev)
{
    PBCMXTMRT_DEV_CONTEXT pDevCtx = dev->priv;
    struct net_device_stats *pStats = &pDevCtx->DevStats;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    UINT32 i;

    if( pDevCtx->Addr.ulTrafficType == TRAFFIC_TYPE_ATM &&
        pDevCtx->ucTxVcid != INVALID_VCID )
    {
        pStats->tx_bytes += pGi->pulMibTxOctetCountBase[pDevCtx->ucTxVcid];
    }

    /* TBD. There may be other match indicies to gather statistics for
     * depending how the Packet CMF block is setup.
     */
    for( i = 0; i < MAX_MATCH_IDS; i++ )
    {
        if( pGi->pDevCtxsByMatchId[i] == pDevCtx )
        {
            *pGi->pulMibRxMatch = i;
            pStats->rx_bytes += *pGi->pulMibRxOctetCount;
            pStats->rx_packets += *pGi->pulMibRxPacketCount;
        }
    }

    return( pStats );
} /* bcmxtmrt_query */


/***************************************************************************
 * Function Name: bcmxtmrt_ioctl
 * Description  : Driver IOCTL entry point.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int bcmxtmrt_ioctl(struct net_device *dev, struct ifreq *Req, int nCmd)
{
    PBCMXTMRT_DEV_CONTEXT pDevCtx = dev->priv;
    int *data=(int*)Req->ifr_data;
    int status;
    int nRet = 0;

    switch (nCmd)
    {
    case SIOCGLINKSTATE:
        if( pDevCtx->ulLinkState == LINK_UP )
            status = LINKSTATE_UP;
        else
            status = LINKSTATE_DOWN;
        if (copy_to_user((void*)data, (void*)&status, sizeof(int)))
            nRet = -EFAULT;
        break;

    case SIOCSCLEARMIBCNTR:
        memset(&pDevCtx->DevStats, 0, sizeof(struct net_device_stats));
        break;

    case SIOCMIBINFO:
        if (copy_to_user((void*)data, (void*)&pDevCtx->MibInfo,
            sizeof(pDevCtx->MibInfo)))
        {
            nRet = -EFAULT;
        }
        break;

    case SIOCETHTOOL:
        nRet = bcmxtmrt_ethtool_ioctl(pDevCtx, (void *) Req->ifr_data);
        break;

    default:
        nRet = -EOPNOTSUPP;    
        break;
    }

    return( nRet );
} /* bcmxtmrt_ioctl */


/***************************************************************************
 * Function Name: bcmxtmrt_ethtool_ioctl
 * Description  : Driver ethtool IOCTL entry point.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int bcmxtmrt_ethtool_ioctl(PBCMXTMRT_DEV_CONTEXT pDevCtx, void *useraddr)
{
    struct ethtool_drvinfo info;
    struct ethtool_cmd ecmd;
    unsigned long ethcmd;
    int nRet = 0;

    if( copy_from_user(&ethcmd, useraddr, sizeof(ethcmd)) == 0 )
    {
        switch (ethcmd)
        {
        case ETHTOOL_GDRVINFO:
            info.cmd = ETHTOOL_GDRVINFO;
            strncpy(info.driver, CARDNAME, sizeof(info.driver)-1);
            strncpy(info.version, VERSION, sizeof(info.version)-1);
            if (copy_to_user(useraddr, &info, sizeof(info)))
                nRet = -EFAULT;
            break;

        case ETHTOOL_GSET:
            ecmd.cmd = ETHTOOL_GSET;
            ecmd.speed = pDevCtx->MibInfo.ulIfSpeed / (1024 * 1024);
            if (copy_to_user(useraddr, &ecmd, sizeof(ecmd)))
                nRet = -EFAULT;
            break;

        default:
            nRet = -EOPNOTSUPP;    
            break;
        }
    }
    else
       nRet = -EFAULT;

    return( nRet );
}

#ifdef ATM_BONDING_TEST
static void sendAsmOnPort (PBCMXTMRT_DEV_CONTEXT pDevCtx)
{
   int nRet ;
	XTMRT_CELL  C ;
	PXTMRT_CELL pC ;
	UINT32 sendPort ;

	pDevCtx->sendAsm = 1 ;
	pC = &C ;
	sendPort = pDevCtx->portAsm ;
	memset (pC, 0, sizeof (XTMRT_CELL)) ;
	memcpy (&pC->ConnAddr, &pDevCtx->Addr, sizeof (XTM_ADDR)) ;
   pC->ucCircuitType = CTYPE_ASM_P0 + sendPort ;
	pDevCtx->portAsm = (pDevCtx->portAsm + 2) & (MAX_PHY_PORTS-1) ;
   memset (pC->ucData, 0x22, CELL_PAYLOAD_SIZE) ;
	printk ("Sending ASM on port %d \n", sendPort) ;
	nRet = DoSendCellReq (pDevCtx, pC) ;
	pDevCtx->sendAsm = 0 ;
}
#endif

/***************************************************************************
 * Function Name: bcmxtmrt_xmit
 * Description  : Check for transmitted packets to free and, if skb is
 *                non-NULL, transmit a packet.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int bcmxtmrt_xmit( struct sk_buff *skb, struct net_device *dev)
{
    PBCMXTMRT_DEV_CONTEXT pDevCtx = dev->priv;
    PTXQINFO pTqi;
    UINT32 i;

    local_bh_disable();

    /* Free packets that have been transmitted. */
    for(i=0, pTqi=pDevCtx->TxQInfos; i < pDevCtx->ulTxQInfosSize; i++, pTqi++)
    {
        while( pTqi->ulFreeBds < pTqi->ulQueueSize &&
            !(pTqi->pBds[pTqi->ulHead].status & DMA_OWN) )
        {
            if( &pTqi->ppSkbs[pTqi->ulHead] != NULL )
            {
                dev_kfree_skb_any(pTqi->ppSkbs[pTqi->ulHead]);
                pTqi->ppSkbs[pTqi->ulHead] = NULL;
            }

            if( !(pTqi->pBds[pTqi->ulHead].status & DMA_WRAP) )
                pTqi->ulHead++;
            else
                pTqi->ulHead = 0;

            pTqi->ulFreeBds++;
        }
    }

    pTqi = NULL;

    
#ifdef ATM_BONDING_TEST
    if(( skb && pDevCtx->ulTxQInfosSize ) &&
		 !(skb->data[0] & 0x01))
#else
    if( skb && pDevCtx->ulTxQInfosSize )
#endif
    {
        /* Find a transmit queue to send on.
         * TBD.  Need to support ports other than port 0 and need to support
         * PTM high/low priority.
         */
        UINT32 ulPort = 0;
#ifdef CONFIG_NETFILTER
        UINT32 ulPriority = skb->nfmark & 0x0F;

        /* bit 3-0 of the 32-bit nfmark is the atm priority,
         *    set by iptables
         * bit 7-4 is the Ethernet switch physical port number,
         *    set by lan port drivers.
         * bit 8-10 is the wanVlan priority bits
         */

        if (ulPriority <= 4 && ulPriority >= 1)
            pTqi = pDevCtx->pTxPriorities[ulPort][ulPriority];
        else
#endif
#if 1
#ifdef ATM_BONDING_TEST
		  {
			  if (pDevCtx->sendAsm == 0) {
				  sendAsmOnPort (pDevCtx) ;
			  }
		  }
#endif
#endif
        if (skb->priority <= 4 && skb->priority >= 1)
            pTqi = pDevCtx->pTxPriorities[ulPort][skb->priority];

        /* If a transmit queue was not found, use the first one. */
        if( pTqi == NULL )
            pTqi = &pDevCtx->TxQInfos[0];

#if defined(CONFIG_L2PT)
		/* Second parameter is transmit channel number */
        l2pt_emit( skb, (unsigned int)pTqi->ulPort, XDSL_BCM );
#endif

        if( (pDevCtx->ulFlags & CNI_HW_ADD_REMOVE_HEADER) == 0 &&
            pDevCtx->ulHdrType != HT_NONE
#ifdef ATM_BONDING_TEST
            &&
            pDevCtx->sendAsm != 1
#endif
            )
        {
            AddRfc2684Hdr( &skb, pDevCtx );
        }

        /* 32 bit align */
        if( ((UINT32) skb->data & 0x03) != 0 )
        {
            UINT32 ulAdjust =
                (((UINT32) skb->data + 0x03) & ~0x03) - (UINT32) skb->data;
            struct sk_buff *skb2 = skb_copy_expand(skb,0,ulAdjust,GFP_ATOMIC);
            if( skb2 )
            {
                dev_kfree_skb(skb);
                skb = skb2;
            }
        }

        if( pTqi->ulFreeBds )
        {
            volatile DmaDesc *pBd;

            /* Decrement total BD count */
            pTqi->ulFreeBds--;

#ifdef ATM_BONDING_TEST
            if (pDevCtx->sendAsm != 1) {
#endif
            if( skb->len < ETH_ZLEN &&
                (skb->protocol & ~FSTAT_CT_MASK) != SKB_PROTO_ATM_CELL )
            {
                struct sk_buff *skb2 = skb_copy_expand(skb, 0, ETH_ZLEN -
                    skb->len, GFP_ATOMIC);
                if( skb2 )
                {
                    dev_kfree_skb(skb);
                    skb = skb2;
                    memset(skb->tail, 0, ETH_ZLEN - skb->len);
                    skb_put(skb, ETH_ZLEN - skb->len);
                }
            }
#ifdef ATM_BONDING_TEST
            }
#endif

            /*
             * Prior to transmit, dirty data to be transmitted is flushed
             * A cloned sk_buff may still have read access to the data buffer.
             * When the buffer is returned to the Rx buffer pool, the region
             * between the DMA assigned data and the end+sharedinfo needs to
             * be invalidated, irrespective of whether it was also partially
             * cleaned before xmit.
             */

                /* pBd only sees skb->data, no need to start from skb->head */
            cache_wbflush_len(skb->data, skb->len);

            /* Track sent SKB, so we can release them later */
            pTqi->ppSkbs[pTqi->ulTail] = skb;

            pBd = &pTqi->pBds[pTqi->ulTail];
            pBd->address = (UINT32) VIRT_TO_PHY(skb->data);
            pBd->length  = skb->len;
            pBd->status  = (pBd->status&DMA_WRAP) | DMA_SOP | DMA_EOP |
                pDevCtx->ucTxVcid;

            if( (skb->protocol & ~FSTAT_CT_MASK) == SKB_PROTO_ATM_CELL )
            {
                pBd->status |= skb->protocol & FSTAT_CT_MASK;
                if( (pDevCtx->ulFlags & CNI_USE_ALT_FSTAT) != 0 )
                {
                    pBd->status |= FSTAT_MODE_COMMON;
                    pBd->status &= ~(FSTAT_COMMON_INS_HDR_EN |
                        FSTAT_COMMON_HDR_INDEX_MASK);
                }
            }
            else
                if( pDevCtx->Addr.ulTrafficType == TRAFFIC_TYPE_ATM )
                {
                    pBd->status |= FSTAT_CT_AAL5;
                    if( (pDevCtx->ulFlags & CNI_USE_ALT_FSTAT) != 0 )
                    {
                        pBd->status |= FSTAT_MODE_COMMON;
                        if( pDevCtx->ulHdrType != HT_NONE )
                        {
                            pBd->status |= FSTAT_COMMON_INS_HDR_EN |
                                ((pDevCtx->ulHdrType - 1) <<
                                FSTAT_COMMON_HDR_INDEX_SHIFT);
                        }
                        else
                        {
                            pBd->status &= ~(FSTAT_COMMON_INS_HDR_EN |
                                FSTAT_COMMON_HDR_INDEX_MASK);
                        }
                    }
                }
                else
                    pBd->status |= FSTAT_CT_PTM | FSTAT_PTM_ENET_FCS |
                        FSTAT_PTM_CRC;

            //printk ("BD Status = %x \n", pBd->status) ;

#if 0 /* TESTING */
            printk(">> DEBUG: Send len=%d, pBd=0x%8.8lx, tail=%lu\n",
                (int) pBd->length, (UINT32) pBd, pTqi->ulTail);
            dumpaddr(skb->data, (pBd->length < 128) ? pBd->length : 128);
            /* Do not printk access over skb->end */
#endif

            pBd->status |= DMA_OWN;

            /* advance BD pointer to next in the chain. */
            if( !(pBd->status & DMA_WRAP) )
                pTqi->ulTail++;
            else
                pTqi->ulTail = 0;

            /* Enable DMA for this channel */
            pTqi->pDma->cfg = DMA_ENABLE;

            /* Transmitted bytes are counted in hardware. */
            pDevCtx->DevStats.tx_packets++;
            pDevCtx->pDev->trans_start = jiffies;
        }
        else
        {
            /* Transmit queue is full.  Free the socket buffer.  Don't call
             * netif_stop_queue because this device may use more than one queue.
             */
            dev_kfree_skb(skb);
            pDevCtx->DevStats.tx_errors++;
        }
    }
    else
    {
        if( skb )
        {
            dev_kfree_skb(skb);
            pDevCtx->DevStats.tx_dropped++;
        }
    }

    local_bh_enable();

    return 0;
} /* bcmxtmrt_xmit */


/***************************************************************************
 * Function Name: AddRfc2684Hdr
 * Description  : Adds the RFC2684 header to an ATM packet before transmitting
 *                it.
 * Returns      : None.
 ***************************************************************************/
static void AddRfc2684Hdr(struct sk_buff **ppskb, PBCMXTMRT_DEV_CONTEXT pDevCtx)
{
    const UINT32 ulMinPktSize = 60;
    UINT32 ulHdrLens[] = {0, 10, 7, 4, 2};
    UINT8 ucHdrs[][16] =
        {{},
         {0xAA, 0xAA, 0x03, 0x00, 0x80, 0xC2, 0x00, 0x07, 0x00, 0x00},
         {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00},
         {0xFE, 0xFE, 0x03, 0xCF},
         {0x00, 0x00}};
    int minheadroom = ulHdrLens[pDevCtx->ulHdrType];
    struct sk_buff *skb = *ppskb;
    int headroom = skb_headroom(skb);

    if (headroom < minheadroom)
    {
        struct sk_buff *skb2 = skb_realloc_headroom(skb, minheadroom);

        dev_kfree_skb(skb);
        if (skb2 == NULL)
            skb = NULL;
        else
            skb = skb2;
    }
    if( skb )
    {
        int skblen;

        skb_push(skb, minheadroom);
        memcpy(skb->data, ucHdrs[pDevCtx->ulHdrType], minheadroom);
        skblen = skb->len;
        if (skblen < ulMinPktSize)
        {
            struct sk_buff *skb2=skb_copy_expand(skb, 0, ulMinPktSize -
                skb->len, GFP_ATOMIC);

            dev_kfree_skb(skb);
            if (skb2 == NULL)
                skb = NULL;
            else
            {
                skb = skb2;
                memset(skb->tail, 0, ulMinPktSize - skb->len);
                skb_put(skb, ulMinPktSize - skb->len);
            }
        }
    }

    *ppskb = skb;;
} /* AddRfc2684Hdr */


/***************************************************************************
 * Function Name: AssignRxBuffer
 * Description  : Put a data buffer back on to the receive BD ring. 
 * Returns      : None.
 ***************************************************************************/
static void AssignRxBuffer( PRXBDINFO pRxBdInfo, UINT8 *pucData, UINT8 *pEnd)
{
    volatile DmaDesc *pRxBd = pRxBdInfo->pBdTail;
    volatile DmaChannelCfg *pRxDma = pRxBdInfo->pDma;
    UINT16 usWrap = pRxBd->status & DMA_WRAP;

    /* DMA never sees RXBUF_HEAD_RESERVE at head of pucData */
    cache_wbflush_region(pucData + RXBUF_HEAD_RESERVE, pEnd);

    if( pRxBd->address == 0 )
    {
        pRxBd->address = VIRT_TO_PHY(pucData + RXBUF_HEAD_RESERVE);
        pRxBd->length  = RXBUF_SIZE;
        pRxBd->status = usWrap | DMA_OWN;

        if( usWrap )
            pRxBdInfo->pBdTail = pRxBdInfo->pBdBase;
        else
            pRxBdInfo->pBdTail++;
    }
    else
    {
        /* This should not happen. */
        printk(CARDNAME ": No place to put free buffer.\n");
    }

    /* Restart DMA in case the DMA ran out of descriptors, and
     * is currently shut down.
     */
    if( (pRxDma->intStat & DMA_NO_DESC) != 0 )
    {
        pRxDma->intStat = DMA_NO_DESC;
        pRxDma->cfg = DMA_ENABLE;
    }
}


/***************************************************************************
 * Function Name: bcmxtmrt_freeskbordata
 * Description  : Put socket buffer header back onto the free list or a data
 *                buffer back on to the BD ring. 
 * Returns      : None.
 ***************************************************************************/
static void bcmxtmrt_freeskbordata( PRXBDINFO pRxBdInfo, struct sk_buff *skb,
    unsigned nFlag )
{
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;

    local_bh_disable();

    if( nFlag & RETFREEQ_SKB )
    {
        skb->retfreeq_context = pGi->pFreeRxSkbList;
        pGi->pFreeRxSkbList = skb;
    }
    else
    {
        /* Compute region to writeback flush invalidate */
        UINT8 * pEnd = skb->end + offsetof(struct skb_shared_info, frags);
        if ( unlikely( skb_shinfo(skb)->nr_frags ))
            pEnd += ( sizeof(skb_frag_t) * MAX_SKB_FRAGS );
        AssignRxBuffer( pRxBdInfo, skb->head, pEnd );
    }

    local_bh_enable();
} /* bcmxtmrt_freeskbordata */


/***************************************************************************
 * Function Name: bcmxtmrt_rxisr
 * Description  : Hardware interrupt that is called when a packet is received
 *                on one of the receive queues.
 * Returns      : IRQ_HANDLED
 ***************************************************************************/
static irqreturn_t bcmxtmrt_rxisr(int nIrq, void *pRxDma, struct pt_regs *pRegs)
{
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;

    ((volatile DmaChannelCfg *) pRxDma)->intStat = DMA_DONE;
    pGi->ulIntEnableMask |=
        1 << (((UINT32)pRxDma-(UINT32)pGi->pRxDmaBase)/sizeof(DmaChannelCfg));
    tasklet_schedule( &pGi->RxTasklet );
    return( IRQ_HANDLED );
} /* bcmxtmrt_rxisr */


/***************************************************************************
 * Function Name: bcmxtmrt_rxtasklet
 * Description  : Linux Tasklet that processes received packets.
 * Returns      : None.
 ***************************************************************************/
static void bcmxtmrt_rxtasklet( PBCMXTMRT_GLOBAL_INFO pGi )
{
    UINT32 ulMoreToReceive;
    UINT32 ulMask;
    UINT32 i;
    PRXBDINFO pRxBdInfo;
    volatile DmaDesc *pRxBd;
    struct sk_buff *skb;
    PBCMXTMRT_DEV_CONTEXT pDevCtx;

    /* Receive packets from every receive queue in a round robin order until
     * there are no more packets to receive.
     */
    do
    {
        ulMoreToReceive = 0;

        for( i = 0, pRxBdInfo = pGi->RxBdInfos; i < MAX_RECEIVE_QUEUES;
             i++, pRxBdInfo++ )
        {
            pRxBd = pRxBdInfo->pBdHead;

            if( pRxBd && !(pRxBd->status & DMA_OWN) && pRxBd->address )
            {
                UINT8 *pucData = (UINT8 *) KSEG0ADDR(pRxBd->address);
                UINT16 usStatus = pRxBd->status;

                pRxBd->address = 0;
                pRxBd->status &= DMA_WRAP;

                /* Advance head BD pointer for this receive BD ring. */
                if( (usStatus & DMA_WRAP) != 0 )
                    pRxBdInfo->pBdHead = pRxBdInfo->pBdBase;
                else
                    pRxBdInfo->pBdHead++;

                pDevCtx = pGi->pDevCtxsByMatchId[usStatus&FSTAT_MATCH_ID_MASK];

                if( pDevCtx && (usStatus & FSTAT_ERROR) == 0 )
                {
                    if((usStatus & FSTAT_PACKET_CELL_MASK) == FSTAT_PACKET)
                    {
                        /* Get an skb to return to the network stack. */
                        skb = pGi->pFreeRxSkbList;
                        pGi->pFreeRxSkbList =
                            pGi->pFreeRxSkbList->retfreeq_context;

                        if( pDevCtx->Addr.ulTrafficType == TRAFFIC_TYPE_PTM &&
                            pRxBd->length )
                        {
                            pRxBd->length -= 4; /* subtract FCS length */
                        }

                        if( pRxBd->length < ETH_ZLEN )
                            pRxBd->length = ETH_ZLEN;

                        skb_hdrinit(RXBUF_HEAD_RESERVE, pRxBd->length +
                            SAR_DMA_MAX_BURST_LENGTH, skb, pucData, (void *)
                            bcmxtmrt_freeskbordata, pRxBdInfo, FROM_WAN);

                        __skb_trim(skb, pRxBd->length);

#if defined(CONFIG_L2PT)
                        /* Second parameter is rx channel number */
                        l2pt_init(skb, usStatus&FSTAT_MATCH_ID_MASK, XDSL_BCM);
#endif

#if 0 /* TESTING */
                        printk(">> DEBUG: Recv len=%d\n", (int) skb->len);
                        //dumpaddr(skb->data,(skb->len < 64) ? skb->len : 16);
                        dumpaddr (skb->data,skb->len) ;
                        /* Do not printk access over skb->end */
#endif
                        if( (pDevCtx->ulFlags & CNI_HW_ADD_REMOVE_HEADER) == 0 )
                        {
                            UINT32 ulHdrLens[] = {0, 10, 7, 4, 2};
                            __skb_pull(skb, ulHdrLens[pDevCtx->ulHdrType]);
                        }

                        skb->dev = pDevCtx->pDev;

                        switch( pDevCtx->ulHdrType )
                        {
                        case HT_LLC_SNAP_ROUTE_IP:
                            /* IPoA */
                            skb->protocol = htons(ETH_P_IP);
                            skb->mac.raw = skb->data;
                            break;

                        case HT_LLC_ENCAPS_PPP:
                            /*PPPoA*/
                            /* TBD.  How to handle PPPoA? */
                            break;

                        default:
                            /* bridge, MER, PPPoE */
                            skb->protocol = eth_type_trans(skb,pDevCtx->pDev);
                            break;
                        }

                        pDevCtx->pDev->last_rx = jiffies;

                        /* Give the received packet to the network stack. */
                        netif_rx(skb);
                    }
                    else /* cell */ {
#if 0
                        printk(">> DEBUG: CELL Recv len=%d\n", (int) pRxBd->length);
                        dumpaddr(pucData,(pRxBd->length < 64) ? pRxBd->length : 16);
                        //dumpaddr(pucData,pRxBd->length);
#endif
                        ProcessRxCell( pGi, pDevCtx, pRxBdInfo, pucData );
						  }
                }
                else /* packet error */
                {
#if 0
                        printk(">> DEBUG: ERR Pkt Recv len=%d\n", (int) pRxBd->length);
                        dumpaddr(pucData,(pRxBd->length < 64) ? pRxBd->length : 16);
                        cache_wbflush_len(pucData, pRxBd->length);
#endif

                    /* No cache operation as no data was accessed */
                    AssignRxBuffer(pRxBdInfo, pucData - RXBUF_HEAD_RESERVE,
                                              pucData - RXBUF_HEAD_RESERVE);

                    if( pDevCtx )
                        pDevCtx->DevStats.rx_errors++;
                }

                /* There may be more packets to receive on this Rx queue. */
                ulMoreToReceive = 1;
            }
        }
    } while( ulMoreToReceive );

    /* Renable interrupts. */
    ulMask = pGi->ulIntEnableMask;
    pGi->ulIntEnableMask = 0;
    for( i = 0; ulMask && i < MAX_RECEIVE_QUEUES; i++, ulMask >>= 1 )
        if( (ulMask & 0x01) == 0x01 )
            BcmHalInterruptEnable(SAR_RX_INT_ID_BASE + i);
} /* bcmxtmrt_rxtasklet */


/***************************************************************************
 * Function Name: ProcessRxCell
 * Description  : Processes a received cell.
 * Returns      : None.
 ***************************************************************************/
static void ProcessRxCell( PBCMXTMRT_GLOBAL_INFO pGi, PBCMXTMRT_DEV_CONTEXT
    pDevCtx, PRXBDINFO pRxBdInfo, UINT8 *pucData )
{
    const UINT16 usOamF4VciSeg = 3;
    const UINT16 usOamF4VciEnd = 4;
    UINT8 ucCts[] = {0, 0, 0, 0, CTYPE_OAM_F5_SEGMENT, CTYPE_OAM_F5_END_TO_END,
        0, 0, CTYPE_ASM_P0, CTYPE_ASM_P1, CTYPE_ASM_P2, CTYPE_ASM_P3,
        CTYPE_OAM_F4_SEGMENT, CTYPE_OAM_F4_END_TO_END};
    XTMRT_CELL Cell;
    UINT8 ucCHdr = *pucData;
    UINT8 *pucAtmHdr = pucData + sizeof(char);

    memcpy(&Cell.ConnAddr, &pDevCtx->Addr, sizeof(XTM_ADDR));

    /* Fill in the XTMRT_CELL structure */
    Cell.ConnAddr.u.Vcc.ulPortMask =
        PORT_TO_PORTID((ucCHdr & CHDR_PORT_MASK) >> CHDR_PORT_SHIFT);
    Cell.ConnAddr.u.Vcc.usVpi = (((UINT16) pucAtmHdr[0] << 8) +
        ((UINT16) pucAtmHdr[1])) >> 4;
    Cell.ConnAddr.u.Vcc.usVci = (UINT16)
        (((UINT32) (pucAtmHdr[1] & 0x0f) << 16) +
         ((UINT32) pucAtmHdr[2] << 8) +
         ((UINT32) pucAtmHdr[3])) >> 4;

    if( Cell.ConnAddr.u.Vcc.usVci == usOamF4VciSeg )
        ucCHdr = CHDR_CT_OAM_F4_SEG;
    else
        if( Cell.ConnAddr.u.Vcc.usVci == usOamF4VciEnd )
            ucCHdr = CHDR_CT_OAM_F4_E2E;

    Cell.ucCircuitType = ucCts[(ucCHdr & CHDR_CT_MASK) >> CHDR_CT_SHIFT];

    if( (ucCHdr & CHDR_ERROR) == 0 )
    {
        memcpy(Cell.ucData, pucData + sizeof(char), sizeof(Cell.ucData));

        /* Call the registered OAM or ASM callback function. */
        switch( ucCHdr & CHDR_CT_MASK )
        {
        case CHDR_CT_OAM_F5_SEG:
        case CHDR_CT_OAM_F5_E2E:
        case CHDR_CT_OAM_F4_SEG:
        case CHDR_CT_OAM_F4_E2E:
            if( pGi->pfnOamHandler )
            {
                (*pGi->pfnOamHandler) ((XTMRT_HANDLE)pDevCtx,
                    XTMRTCB_CMD_CELL_RECEIVED, &Cell,
                    pGi->pOamContext);
            }
            break;

        case CHDR_CT_ASM_P0:
        case CHDR_CT_ASM_P1:
        case CHDR_CT_ASM_P2:
        case CHDR_CT_ASM_P3:
            if( pGi->pfnAsmHandler )
            {
                (*pGi->pfnAsmHandler) ((XTMRT_HANDLE)pDevCtx,
                    XTMRTCB_CMD_CELL_RECEIVED, &Cell,
                    pGi->pAsmContext);
            }
#ifdef ATM_BONDING_TEST
				printk ("Received ASM \n") ;
#endif
            break;

        default:
            break;
        }
    }
    else
        pDevCtx->DevStats.rx_errors++;

    /* Put the buffer back onto the BD ring. */
    /* Compute region to writeback flush invalidate */
    AssignRxBuffer(pRxBdInfo, pucData - RXBUF_HEAD_RESERVE,
                              pucData - RXBUF_HEAD_RESERVE + RXBUF_SIZE);

} /* ProcessRxCell */

/***************************************************************************
 * Function Name: bcmxtmrt_timer
 * Description  : Periodic timer that calls the send function to free packets
 *                that have been transmitted.
 * Returns      : None.
 ***************************************************************************/
static void bcmxtmrt_timer( PBCMXTMRT_GLOBAL_INFO pGi )
{
    UINT32 i;

    /* Free transmitted buffers. */
    for( i = 0; i < MAX_DEV_CTXS; i++ )
        if( pGi->pDevCtxs[i] )
            bcmxtmrt_xmit( NULL, pGi->pDevCtxs[i]->pDev );

    /* Restart the timer. */
    pGi->Timer.expires = jiffies + SAR_TIMEOUT;
    add_timer(&pGi->Timer);
} /* bcmxtmrt_timer */


/***************************************************************************
 * Function Name: bcmxtmrt_request
 * Description  : Request from the bcmxtmcfg driver.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
int bcmxtmrt_request( PBCMXTMRT_DEV_CONTEXT pDevCtx, UINT32 ulCommand,
    void *pParm )
{
    int nRet = 0;

    switch( ulCommand )
    {
    case XTMRT_CMD_GLOBAL_INITIALIZATION:
        nRet = DoGlobInitReq( (PXTMRT_GLOBAL_INIT_PARMS) pParm );
        break;

    case XTMRT_CMD_CREATE_DEVICE:
        nRet = DoCreateDeviceReq( (PXTMRT_CREATE_NETWORK_DEVICE) pParm );
        break;

    case XTMRT_CMD_GET_DEVICE_STATE:
        *(UINT32 *) pParm = pDevCtx->ulOpenState;
        break;

    case XTMRT_CMD_SET_ADMIN_STATUS:
        pDevCtx->ulAdminStatus = (UINT32) pParm;
        break;

    case XTMRT_CMD_REGISTER_CELL_HANDLER:
        nRet = DoRegCellHdlrReq( (PXTMRT_CELL_HDLR) pParm );
        break;

    case XTMRT_CMD_UNREGISTER_CELL_HANDLER:
        nRet = DoUnregCellHdlrReq( (PXTMRT_CELL_HDLR) pParm );
        break;

    case XTMRT_CMD_LINK_STATUS_CHANGED:
        nRet = DoLinkStsChangedReq(pDevCtx, (PXTMRT_LINK_STATUS_CHANGE)pParm);
        break;

    case XTMRT_CMD_SEND_CELL:
        nRet = DoSendCellReq( pDevCtx, (PXTMRT_CELL) pParm );
        break;

    case XTMRT_CMD_DELETE_DEVICE:
        nRet = DoDeleteDeviceReq( pDevCtx );
        break;

    default:
        nRet = -EINVAL;
        break;
    }

    return( nRet );
} /* bcmxtmrt_request */


/***************************************************************************
 * Function Name: DoGlobInitReq
 * Description  : Processes an XTMRT_CMD_GLOBAL_INITIALIZATION command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoGlobInitReq( PXTMRT_GLOBAL_INIT_PARMS pGip )
{
    const UINT32 ulBlockSize = (64 * 1024);
    const UINT32 ulBufsPerBlock = ulBlockSize / RXBUF_ALLOC_SIZE;

    int nRet = 0;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    struct DmaDesc *pBd, *pBdBase;
    UINT32 ulBufsToAlloc;
    UINT32 ulAllocAmt;
    UINT8 *p;
    UINT32 i, j, ulSize;

    /* Save MIB counter registers. */
    pGi->pulMibTxOctetCountBase = pGip->pulMibTxOctetCountBase;
    pGi->pulMibRxMatch = pGip->pulMibRxMatch;
    pGi->pulMibRxOctetCount = pGip->pulMibRxOctetCount;
    pGi->pulMibRxPacketCount = pGip->pulMibRxPacketCount;

    /* Determine the number of receive buffers. */
    for( i = 0, ulBufsToAlloc = 0; i < MAX_RECEIVE_QUEUES; i++ )
        for( j = 0; j < pGip->ulReceiveQueueSizes[i]; j++ )
            ulBufsToAlloc++;

    if( pGi->ulDrvState != XTMRT_UNINITIALIZED )
        nRet = -EPERM;

    /* Allocate receive socket buffers. */
    if( nRet == 0 && (pGi->pRxSkbMem = (struct sk_buff *)
        kmalloc(ulBufsToAlloc * SKB_ALIGNED_SIZE, GFP_KERNEL)) != NULL )
    {
        /* Chain receive socket buffers. */
        for( i = 0, p = (UINT8 *) (((UINT32) pGi->pRxSkbMem + 0x0f) & ~0x0f);
             i < ulBufsToAlloc; i++, p += SKB_ALIGNED_SIZE )
        {
            ((struct sk_buff *) p)->retfreeq_context = pGi->pFreeRxSkbList;
            pGi->pFreeRxSkbList = (struct sk_buff *) p;
        }
    }
    else
    {
        nRet = -ENOMEM;
    }

    /* Allocate receive DMA buffer descriptors. */
    ulSize = (ulBufsToAlloc * sizeof(struct DmaDesc)) + 0x10;
    if( nRet == 0 && (pGi->pRxBdMem = (struct DmaDesc *)
        kmalloc(ulSize, GFP_KERNEL)) != NULL )
    {
        pBdBase = (struct DmaDesc *) (((UINT32) pGi->pRxBdMem + 0x0f) & ~0x0f);
        p = (UINT8 *) pGi->pRxBdMem;
        cache_wbflush_len(p, ulSize);
        pBdBase = (struct DmaDesc *) CACHE_TO_NONCACHE(pBdBase);
    }
    else
    {
        kfree(pGi->pRxSkbMem);
        pGi->pRxSkbMem = NULL;
        nRet = -ENOMEM;
    }

    /* Allocate receive data buffers. */
    if( nRet == 0 )
    {
        j = 0;
        pBd = pBdBase;
        while( ulBufsToAlloc )
        {
            ulAllocAmt = (ulBufsPerBlock < ulBufsToAlloc)
                ? ulBufsPerBlock : ulBufsToAlloc;

            ulSize = ulAllocAmt * RXBUF_ALLOC_SIZE;
            p = kmalloc(ulSize, GFP_KERNEL);
            if( p )
            {
                pGi->pBufMem[j++] = p;
                cache_wbflush_len(p, ulSize);
                p = (UINT8 *) (((UINT32) p + 0x0f) & ~0x0f);
                for( i = 0; i < ulAllocAmt; i++ )
                {
                    pBd->status = DMA_OWN;
                    pBd->length = RXBUF_SIZE;
                    pBd->address = (UINT32)VIRT_TO_PHY(p + RXBUF_HEAD_RESERVE);
                    pBd++;
                    p += RXBUF_ALLOC_SIZE;
                }
                ulBufsToAlloc -= ulAllocAmt;
            }
            else
            {
                /* Allocation error. */
                for (i = 0; i < MAX_BUFMEM_BLOCKS; i++)
                {
                    if (pGi->pBufMem[i])
                    {
                        kfree(pGi->pBufMem[i]);
                        pGi->pBufMem[i] = NULL;
                    }
                }
                kfree((const UINT8 *) pGi->pRxBdMem);
                pGi->pRxBdMem = NULL;
                nRet = -ENOMEM;
            }
        }
    }

    /* Initialize receive DMA registers. */
    if( nRet == 0 )
    {
        volatile DmaChannelCfg *pRxDma, *pTxDma;
        volatile DmaStateRam *pStRam;

        /* Clear State RAM for receive DMA Channels. */
        pGi->pDmaRegs = (DmaRegs *) SAR_DMA_BASE;
        pStRam = pGi->pDmaRegs->stram.s;
        memset((char *) &pStRam[SAR_RX_DMA_BASE_CHAN], 0x00,
            sizeof(DmaStateRam) * NR_SAR_RX_DMA_CHANS);
        pGi->pRxDmaBase = pGi->pDmaRegs->chcfg+SAR_RX_DMA_BASE_CHAN;
        pRxDma = pGi->pRxDmaBase;
        pBd = pBdBase;

        for(i=0, pRxDma=pGi->pRxDmaBase; i < MAX_RECEIVE_QUEUES; i++, pRxDma++)
        {
            pRxDma->cfg = 0;
            BcmHalInterruptDisable(SAR_RX_INT_ID_BASE + i);
            if( pGip->ulReceiveQueueSizes[i] )
            {
                PRXBDINFO pRxBdInfo = &pGi->RxBdInfos[i];

                pRxDma->maxBurst = SAR_DMA_MAX_BURST_LENGTH;
                pRxDma->intStat = DMA_DONE | DMA_NO_DESC | DMA_BUFF_DONE;
                pRxDma->intMask = DMA_DONE;
                pRxBdInfo->pBdBase = pBd;
                pRxBdInfo->pBdHead = pBd;
                pRxBdInfo->pBdTail = pBd;
                pRxBdInfo->pDma = pRxDma;
                pStRam[SAR_RX_DMA_BASE_CHAN+i].baseDescPtr = (UINT32)
                    VIRT_TO_PHY(pRxBdInfo->pBdBase);
                pBd += pGip->ulReceiveQueueSizes[i] - 1;
                pBd++->status |= DMA_WRAP;

                BcmHalMapInterrupt(bcmxtmrt_rxisr, (UINT32) pRxDma,
                    SAR_RX_INT_ID_BASE + i);
            }
            else
                memset(&pGi->RxBdInfos[i], 0x00, sizeof(RXBDINFO));
        }

        pGi->pDmaRegs->controller_cfg |= DMA_MASTER_EN;

        tasklet_init(&pGi->RxTasklet, (void *) bcmxtmrt_rxtasklet,
            (unsigned long) pGi);

        /* Clear State RAM for transmit DMA Channels. */
        memset( (char *) &pStRam[SAR_TX_DMA_BASE_CHAN], 0x00,
            sizeof(DmaStateRam) * NR_SAR_TX_DMA_CHANS );
        pGi->pTxDmaBase = pGi->pDmaRegs->chcfg+SAR_TX_DMA_BASE_CHAN;

        for(i=0, pTxDma=pGi->pTxDmaBase; i < MAX_TRANSMIT_QUEUES; i++, pTxDma++)
        {
            pTxDma->cfg = 0;
            BcmHalInterruptDisable(SAR_TX_INT_ID_BASE + i);
        }

        /* Initialize a timer function to free transmit buffers. */
        init_timer(&pGi->Timer);
        pGi->Timer.data = (unsigned long) pGi;
        pGi->Timer.function = (void *) bcmxtmrt_timer;

        pGi->ulDrvState = XTMRT_INITIALIZED;
    }

    return( nRet );
} /* DoGlobInitReq */


/***************************************************************************
 * Function Name: DoCreateDeviceReq
 * Description  : Processes an XTMRT_CMD_CREATE_DEVICE command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoCreateDeviceReq( PXTMRT_CREATE_NETWORK_DEVICE pCnd )
{
    int nRet = 0;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    PBCMXTMRT_DEV_CONTEXT pDevCtx = NULL;
    struct net_device *dev = NULL;

    if( pGi->ulDrvState != XTMRT_UNINITIALIZED &&
        (dev = alloc_netdev( sizeof(BCMXTMRT_DEV_CONTEXT),
         pCnd->szNetworkDeviceName, ether_setup )) != NULL )
    {
        dev_alloc_name(dev, dev->name);
        SET_MODULE_OWNER(dev);

        pDevCtx = (PBCMXTMRT_DEV_CONTEXT) dev->priv;
        memset(pDevCtx, 0x00, sizeof(BCMXTMRT_DEV_CONTEXT));
        memcpy(&pDevCtx->Addr, &pCnd->ConnAddr, sizeof(XTM_ADDR));
        pDevCtx->ulHdrType = pCnd->ulHeaderType;
        pDevCtx->ulFlags = pCnd->ulFlags;
        pDevCtx->pDev = dev;
        pDevCtx->ulAdminStatus = ADMSTS_UP;
        pDevCtx->ucTxVcid = INVALID_VCID;

        /* Read and display the MAC address. */
        dev->dev_addr[0] = 0xff;
        kerSysGetMacAddress( dev->dev_addr, dev->ifindex | 0x10000000 );
        if( (dev->dev_addr[0] & 0x01) == 0x01 )
        {
            printk( KERN_ERR CARDNAME": Unable to read MAC address from "
                "persistent storage.  Using default address.\n" );
            memcpy( dev->dev_addr, "\x02\x10\x18\x02\x00\x01", 6 );
        }

        printk( CARDNAME": MAC address: %2.2x %2.2x %2.2x %2.2x %2.2x "
            "%2.2x\n", dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
            dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5] );

        /* Setup the callback functions. */
        dev->open               = bcmxtmrt_open;
        dev->stop               = bcmxtmrt_close;
        dev->hard_start_xmit    = bcmxtmrt_xmit;
        dev->tx_timeout         = bcmxtmrt_timeout;
        dev->watchdog_timeo     = SAR_TIMEOUT;
        dev->get_stats          = bcmxtmrt_query;
        dev->set_multicast_list = NULL;
        dev->do_ioctl           = &bcmxtmrt_ioctl;

        if( pDevCtx->ulHdrType == HT_LLC_SNAP_ROUTE_IP ) /* IPoA */
            dev->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
        else
            if( pDevCtx->ulHdrType == HT_LLC_ENCAPS_PPP ) /* PPPoA */
            {
                /* TBD.  How to handle PPPoA? */
            }
            else /* bridge, MER, PPPoE */
                dev->flags = 0;

        /* Don't reset or enable the device yet. "Open" does that. */
        nRet = register_netdev(dev);
        if (nRet == 0) 
        {
            UINT32 i;
            for( i = 0; i < MAX_DEV_CTXS; i++ )
                if( pGi->pDevCtxs[i] == NULL )
                {
                    pGi->pDevCtxs[i] = pDevCtx;
                    break;
                }

            pCnd->hDev = (XTMRT_HANDLE) pDevCtx;
        }
        else
        {
            printk(KERN_ERR CARDNAME": register_netdev failed\n");
            free_netdev(dev);
        }

        if( nRet != 0 )
            kfree(pDevCtx);
    }
    else
    {
        printk(KERN_ERR CARDNAME": alloc_netdev failed\n");
        nRet = -ENOMEM;
    }

    return( nRet );
} /* DoCreateDeviceReq */


/***************************************************************************
 * Function Name: DoRegCellHdlrReq
 * Description  : Processes an XTMRT_CMD_REGISTER_CELL_HANDLER command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoRegCellHdlrReq( PXTMRT_CELL_HDLR pCh )
{
    int nRet = 0;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;

    switch( pCh->ulCellHandlerType )
    {
    case CELL_HDLR_OAM:
        if( pGi->pfnOamHandler == NULL )
        {
            pGi->pfnOamHandler = pCh->pfnCellHandler;
            pGi->pOamContext = pCh->pContext;
        }
        else
            nRet = -EEXIST;
        break;

    case CELL_HDLR_ASM:
        if( pGi->pfnAsmHandler == NULL )
        {
            pGi->pfnAsmHandler = pCh->pfnCellHandler;
            pGi->pAsmContext = pCh->pContext;
        }
        else
            nRet = -EEXIST;
        break;
    }

    return( nRet );
} /* DoRegCellHdlrReq */


/***************************************************************************
 * Function Name: DoUnregCellHdlrReq
 * Description  : Processes an XTMRT_CMD_UNREGISTER_CELL_HANDLER command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoUnregCellHdlrReq( PXTMRT_CELL_HDLR pCh )
{
    int nRet = 0;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;

    switch( pCh->ulCellHandlerType )
    {
    case CELL_HDLR_OAM:
        if( pGi->pfnOamHandler == pCh->pfnCellHandler )
        {
            pGi->pfnOamHandler = NULL;
            pGi->pOamContext = NULL;
        }
        else
            nRet = -EPERM;
        break;

    case CELL_HDLR_ASM:
        if( pGi->pfnAsmHandler == pCh->pfnCellHandler )
        {
            pGi->pfnAsmHandler = NULL;
            pGi->pAsmContext = NULL;
        }
        else
            nRet = -EPERM;
        break;
    }

    return( nRet );
} /* DoUnregCellHdlrReq */


/***************************************************************************
 * Function Name: DoLinkStsChangedReq
 * Description  : Processes an XTMRT_CMD_LINK_STATUS_CHANGED command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoLinkStsChangedReq( PBCMXTMRT_DEV_CONTEXT pDevCtx,
     PXTMRT_LINK_STATUS_CHANGE pLsc )
{
    int nRet = -EPERM;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    UINT32 i;

    for( i = 0; i < MAX_DEV_CTXS; i++ )
        if( pGi->pDevCtxs[i] == pDevCtx )
        {
            if( pLsc->ulLinkState == LINK_UP )
                nRet = DoLinkUp( pDevCtx, pLsc );
            else
                nRet = DoLinkDown( pDevCtx, pLsc );

            pDevCtx->MibInfo.ulIfLastChange = (jiffies * 100) / HZ;
            pDevCtx->MibInfo.ulIfSpeed = pLsc->ulLinkUsRate;
            break;
        }

    return( nRet );
} /* DoLinkStsChangedReq */


/***************************************************************************
 * Function Name: DoLinkUp
 * Description  : Processes a "link up" condition.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoLinkUp( PBCMXTMRT_DEV_CONTEXT pDevCtx,
     PXTMRT_LINK_STATUS_CHANGE pLsc )
{
    int nRet = 0;
    volatile DmaStateRam *pStRam;
    volatile DmaChannelCfg *pRxDma;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    PXTMRT_TRANSMIT_QUEUE_ID pTxQId;
    PTXQINFO pTxQInfo;
    UINT32 i, ulSize, ulNumTxBufs, ulPort;
    UINT8 *p;

    /* Determine the number of transmit buffers. */
    for( i = 0, ulNumTxBufs = 0, pTxQId = pLsc->TransitQueueIds;
         i < pLsc->ulTransmitQueueIdsSize; i++, pTxQId++ )
    {
        ulNumTxBufs += pTxQId->ulQueueSize;
    }

    /* Allocate memory for all transmit socket buffers and transmit DMA 
     * buffer descriptors.
     */
    ulSize = (ulNumTxBufs * (sizeof(struct sk_buff *)+sizeof(struct DmaDesc))) +
        (pLsc->ulTransmitQueueIdsSize * 0x10);
    pDevCtx->pTxQMem = p = kmalloc(ulSize, GFP_KERNEL);

    /* Initialize transmit DMA channel information. */
    if( pDevCtx->pTxQMem )
    {
        pDevCtx->ucTxVcid = pLsc->ucTxVcid;
        pDevCtx->ulLinkState = pLsc->ulLinkState;
        pDevCtx->ulTxQInfosSize = pLsc->ulTransmitQueueIdsSize;

        /* Use each Rx vcid as an index into an array of bcmxtmrt devices
         * context structures.
         */
        for( i = 0; i < pLsc->ulRxVcidsSize; i++ )
            pGi->pDevCtxsByMatchId[pLsc->ucRxVcids[i]] = pDevCtx;

        memset(p, 0x00, ulSize);
        cache_wbflush_len(p, ulSize);
        p = (UINT8 *) (((UINT32) p + 0x0f) & ~0x0f);
        pStRam = pGi->pDmaRegs->stram.s;
        for(i = 0,pTxQInfo = pDevCtx->TxQInfos, pTxQId = pLsc->TransitQueueIds;
            i < pDevCtx->ulTxQInfosSize; i++, pTxQInfo++, pTxQId++)
        {
            ulPort = PORTID_TO_PORT(pTxQId->ulPortId);
            if( ulPort >= MAX_PHY_PORTS ||
                pTxQId->ulSubPriority >= MAX_SUB_PRIORITIES )
            {
                printk(CARDNAME ": Invalid transmit queue port/priority\n");
                continue;
            }

            pTxQInfo->ulPort = ulPort;
            pTxQInfo->ulPtmPriority = pTxQId->ulPtmPriority;
            pTxQInfo->ulSubPriority = pTxQId->ulSubPriority;
            pTxQInfo->ulQueueSize = pTxQId->ulQueueSize;
            pTxQInfo->ulDmaIndex = pTxQId->ulQueueIndex;
            pTxQInfo->pDma = pGi->pTxDmaBase + pTxQInfo->ulDmaIndex;
            pTxQInfo->pBds = (volatile DmaDesc *) CACHE_TO_NONCACHE(p);
            pTxQInfo->pBds[pTxQInfo->ulQueueSize - 1].status |= DMA_WRAP;
            p += sizeof(struct DmaDesc) * pTxQId->ulQueueSize;
            p = (UINT8 *) (((UINT32) p + 0x0f) & ~0x0f);
            pTxQInfo->ppSkbs = (struct sk_buff **) p;
            p += sizeof(struct sk_buff *) * pTxQId->ulQueueSize;
            pTxQInfo->ulFreeBds = pTxQInfo->ulQueueSize;
            pTxQInfo->ulHead = pTxQInfo->ulTail = 0;

            pTxQInfo->pDma->cfg = 0;
            pTxQInfo->pDma->maxBurst = SAR_DMA_MAX_BURST_LENGTH;
            pTxQInfo->pDma->intStat = DMA_DONE | DMA_NO_DESC | DMA_BUFF_DONE;
            memset((UINT8 *)&pStRam[SAR_TX_DMA_BASE_CHAN+pTxQInfo->ulDmaIndex],
                0x00, sizeof(DmaStateRam));
            pStRam[SAR_TX_DMA_BASE_CHAN + pTxQInfo->ulDmaIndex].baseDescPtr =
                (UINT32) VIRT_TO_PHY(pTxQInfo->pBds);

            pDevCtx->pTxPriorities[ulPort][pTxQInfo->ulSubPriority] = pTxQInfo;
        }

        /* If it is not already there, put the driver into a "ready to send and
         * receive state".
         */
        if( pGi->ulDrvState == XTMRT_INITIALIZED )
        {
            /* Enable receive interrupts and start a timer. */
            for( i = 0, pRxDma = pGi->pRxDmaBase; i < MAX_RECEIVE_QUEUES;
                 i++, pRxDma++ )
            {
                if( pGi->RxBdInfos[i].pBdBase )
                {
                    pRxDma->cfg = DMA_ENABLE;
                    BcmHalInterruptEnable(SAR_RX_INT_ID_BASE + i);
                }
            }

            pGi->Timer.expires = jiffies + SAR_TIMEOUT;
            add_timer(&pGi->Timer);

            if( pDevCtx->ulOpenState == XTMRT_DEV_OPENED )
                netif_start_queue(pDevCtx->pDev);

            pGi->ulDrvState = XTMRT_RUNNING;
        }
    }
    else
        nRet = -ENOMEM;

    return( nRet );
} /* DoLinkUp */


/***************************************************************************
 * Function Name: DoLinkDown
 * Description  : Processes a "link down" condition.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoLinkDown( PBCMXTMRT_DEV_CONTEXT pDevCtx,
     PXTMRT_LINK_STATUS_CHANGE pLsc )
{
    int nRet = 0;
    volatile DmaStateRam  *pStRam;
    volatile DmaChannelCfg *pRxDma;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    PTXQINFO pTxQInfo;
    UINT32 i, ulStopRunning;

    /* Free all packets in transmit queues. */
    netif_stop_queue(pDevCtx->pDev);

    /* Free transmitted packets. */
    bcmxtmrt_xmit( NULL, pDevCtx->pDev );

    /* Disable transmit DMA. */
    pStRam = pGi->pDmaRegs->stram.s;
    for( i = 0, pTxQInfo = pDevCtx->TxQInfos; i < pDevCtx->ulTxQInfosSize;
         i++, pTxQInfo++ )
    {
        pTxQInfo->pDma->cfg = DMA_BURST_HALT;
        pStRam[SAR_TX_DMA_BASE_CHAN + pTxQInfo->ulDmaIndex].baseDescPtr = 0;
    }

    /* Free memory used for transmit queues. */
    kfree(pDevCtx->pTxQMem);

    /* Zero transmit related data structures. */
    pDevCtx->ulTxQInfosSize = 0;
    memset(pDevCtx->TxQInfos, 0x00, sizeof(pDevCtx->TxQInfos));
    memset(pDevCtx->pTxPriorities, 0x00, sizeof(pDevCtx->pTxPriorities));
    pDevCtx->ulLinkState = pLsc->ulLinkState;
    pDevCtx->ucTxVcid = INVALID_VCID;

    /* Zero receive vcids. */
    for( i = 0; i < MAX_MATCH_IDS; i++ )
        if( pGi->pDevCtxsByMatchId[i] == pDevCtx )
            pGi->pDevCtxsByMatchId[i] = NULL;

    /* If all links are down, put the driver into an "initialized" state. */
    for( i = 0, ulStopRunning = 1; i < MAX_DEV_CTXS; i++ )
        if( pGi->pDevCtxs[i] && pGi->pDevCtxs[i]->ulLinkState == LINK_UP )
        {
            ulStopRunning = 0;
            break;
        }

    if( ulStopRunning )
    {
        /* Disable receive interrupts and stop the timer. */
        for( i = 0, pRxDma = pGi->pRxDmaBase; i < MAX_RECEIVE_QUEUES;
             i++, pRxDma++ )
        {
            if( pGi->RxBdInfos[i].pBdBase )
            {
                pRxDma->cfg = 0;
                BcmHalInterruptDisable(SAR_RX_INT_ID_BASE + i);
            }
        }

        del_timer_sync(&pGi->Timer);

        pGi->ulDrvState = XTMRT_INITIALIZED;
    }

    netif_wake_queue(pDevCtx->pDev);

    return( nRet );
} /* DoLinkDown */


/***************************************************************************
 * Function Name: DoSendCellReq
 * Description  : Processes an XTMRT_CMD_SEND_CELL command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoSendCellReq( PBCMXTMRT_DEV_CONTEXT pDevCtx, PXTMRT_CELL pC )
{
    int nRet = 0;

    if( pDevCtx->ulLinkState == LINK_UP )
    {
        struct sk_buff *skb = dev_alloc_skb(CELL_PAYLOAD_SIZE);

        if( skb )
        {
            UINT32 i;
            UINT32 ulPort = (pC->ConnAddr.ulTrafficType == TRAFFIC_TYPE_ATM)
                ? pC->ConnAddr.u.Vcc.ulPortMask
                : pC->ConnAddr.u.Flow.ulPortMask;

            /* A network device instance can potentially have transmit queues
             * on different ports. Find a transmit queue for the port specified
             * in the cell structure.  The cell structure should only specify
             * one port.
             */
            for( i = 0; i < MAX_SUB_PRIORITIES; i++ )
            {
                if( pDevCtx->pTxPriorities[ulPort][i] )
                {
                    skb->priority = i;
                    break;
                }
            }

            skb->dev = pDevCtx->pDev;
            __skb_put(skb, CELL_PAYLOAD_SIZE);
            memcpy(skb->data, pC->ucData, CELL_PAYLOAD_SIZE);

            switch( pC->ucCircuitType )
            {
            case CTYPE_OAM_F5_SEGMENT:
                skb->protocol = FSTAT_CT_OAM_F5_SEG;
                break;

            case CTYPE_OAM_F5_END_TO_END:
                skb->protocol = FSTAT_CT_OAM_F5_E2E;
                break;

            case CTYPE_OAM_F4_SEGMENT:
                skb->protocol = FSTAT_CT_OAM_F4_SEG;
                break;

            case CTYPE_OAM_F4_END_TO_END:
                skb->protocol = FSTAT_CT_OAM_F4_E2E;
                break;

            case CTYPE_ASM_P0:
                skb->protocol = FSTAT_CT_ASM_P0;
                break;

            case CTYPE_ASM_P1:
                skb->protocol = FSTAT_CT_ASM_P1;
                break;

            case CTYPE_ASM_P2:
                skb->protocol = FSTAT_CT_ASM_P2;
                break;

            case CTYPE_ASM_P3:
                skb->protocol = FSTAT_CT_ASM_P3;
                break;
            }

            skb->protocol |= SKB_PROTO_ATM_CELL;

            bcmxtmrt_xmit( skb, pDevCtx->pDev);
        }
        else
            nRet = -ENOMEM;
    }
    else
        nRet = -EPERM;

    return( nRet );
} /* DoSendCellReq */


/***************************************************************************
 * Function Name: DoDeleteDeviceReq
 * Description  : Processes an XTMRT_CMD_DELETE_DEVICE command.
 * Returns      : 0 if successful or error status
 ***************************************************************************/
static int DoDeleteDeviceReq( PBCMXTMRT_DEV_CONTEXT pDevCtx )
{
    int nRet = -EPERM;
    PBCMXTMRT_GLOBAL_INFO pGi = &g_GlobalInfo;
    UINT32 i;

    for( i = 0; i < MAX_DEV_CTXS; i++ )
        if( pGi->pDevCtxs[i] == pDevCtx )
        {
            pGi->pDevCtxs[i] = NULL;

            kerSysReleaseMacAddress( pDevCtx->pDev->dev_addr );

            unregister_netdev( pDevCtx->pDev );
            free_netdev( pDevCtx->pDev );

            kfree(pDevCtx);
            nRet = 0;
            break;
        }

    for( i = 0; i < MAX_MATCH_IDS; i++ )
        if( pGi->pDevCtxsByMatchId[i] == pDevCtx )
            pGi->pDevCtxsByMatchId[i] = NULL;

    return( nRet );
} /* DoDeleteDeviceReq */


/***************************************************************************
 * MACRO to call driver initialization and cleanup functions.
 ***************************************************************************/
module_init(bcmxtmrt_init);
module_exit(bcmxtmrt_cleanup);
MODULE_LICENSE("Proprietary");

EXPORT_SYMBOL(bcmxtmrt_request);


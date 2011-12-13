/*
 *	Handle incoming frames
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: br_input.c,v 1.10 2001/12/24 04:50:20 davem Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/netfilter_bridge.h>
#if defined(CONFIG_MIPS_BRCM)
#include <linux/if_vlan.h>
#include <linux/timer.h>
#include <linux/igmp.h>
#include <linux/ip.h>
#endif
#include "br_private.h"

#if defined(CONFIG_MIPS_BRCM)
#define SNOOPING_BLOCKING_MODE 2
#endif

const unsigned char bridge_ula[6] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };

union ip_array {
	unsigned int ip_addr;
        unsigned char ip_ar[4];
};

#if defined(CONFIG_MIPS_BRCM)
static void addr_conv(unsigned char *in, char * out)
{
    sprintf(out, "%02x%02x%02x%02x%02x%02x", in[0], in[1], in[2], in[3], in[4], in[5]);
}

mac_addr upnp_addr = {{0x01, 0x00, 0x5e, 0x7f, 0xff, 0xfa}};
mac_addr sys1_addr = {{0x01, 0x00, 0x5e, 0x00, 0x00, 0x01}};
mac_addr sys2_addr = {{0x01, 0x00, 0x5e, 0x00, 0x00, 0x02}};
mac_addr ospf1_addr = {{0x01, 0x00, 0x5e, 0x00, 0x00, 0x05}};
mac_addr ospf2_addr = {{0x01, 0x00, 0x5e, 0x00, 0x00, 0x06}};
mac_addr ripv2_addr = {{0x01, 0x00, 0x5e, 0x00, 0x00, 0x09}};
mac_addr sys_addr = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

/* Define ipv6 multicast mac address to let them pass through control filtering.
 * All ipv6 multicast mac addresses start with 0x33 0x33. So control_filter
 * only need to compare the first 2 bytes of the address.
 */
mac_addr ipv6_mc_addr = {{0x33, 0x33, 0x00, 0x00, 0x00, 0x00}}; /* only the left two bytes are significant */

static int control_filter(unsigned char *dest)
{
   if ((!memcmp(dest, &upnp_addr, ETH_ALEN)) ||
       (!memcmp(dest, &sys1_addr, ETH_ALEN)) ||
       (!memcmp(dest, &sys2_addr, ETH_ALEN)) ||
       (!memcmp(dest, &ospf1_addr, ETH_ALEN)) ||
       (!memcmp(dest, &ospf2_addr, ETH_ALEN)) ||
       (!memcmp(dest, &sys_addr, ETH_ALEN)) ||
       (!memcmp(dest, &ripv2_addr, ETH_ALEN)) ||
       (!memcmp(dest, &ipv6_mc_addr, 2)))
      return 0;
   else
      return 1;
}

static void brcm_conv_ip_to_mac(char *ipa, char *maca)
{
   maca[0] = 0x01;
   maca[1] = 0x00;
   maca[2] = 0x5e;
   maca[3] = 0x7F & ipa[1];
   maca[4] = ipa[2];
   maca[5] = ipa[3];

   return;
}

static int br_process_igmpv3(struct net_bridge *br, struct sk_buff *skb, unsigned char *dest, struct igmpv3_report *report)
{
  struct igmpv3_grec *grec;
  int i;
  struct in_addr src;
  union ip_array igmpv3_mcast;
  int num_src;
  unsigned char tmp[6];
  struct net_bridge_mc_fdb_entry *mc_fdb;

  if(report) {
    grec = &report->grec[0];
    for(i = 0; i < report->ngrec; i++) {
      igmpv3_mcast.ip_addr = grec->grec_mca;
      brcm_conv_ip_to_mac(igmpv3_mcast.ip_ar, tmp);
      switch(grec->grec_type) {
        case IGMPV3_MODE_IS_INCLUDE:
        case IGMPV3_CHANGE_TO_INCLUDE:
        case IGMPV3_ALLOW_NEW_SOURCES:
          for(num_src = 0; num_src < grec->grec_nsrcs; num_src++) {
            src.s_addr = grec->grec_src[num_src];
            mc_fdb = br_mc_fdb_find(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, &src);
            if((NULL != mc_fdb) && 
               (mc_fdb->src_entry.filt_mode == MCAST_EXCLUDE)) {
              br_mc_fdb_remove(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_EX_CLEAR, &src);
            }
            else {
              br_mc_fdb_add(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_IN_ADD, &src);
            }
          }

          if(0 == grec->grec_nsrcs) {
            src.s_addr = 0;
            br_mc_fdb_remove(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_EX_CLEAR, &src);
          }
         break;
       
         case IGMPV3_MODE_IS_EXCLUDE:
         case IGMPV3_CHANGE_TO_EXCLUDE:
         case IGMPV3_BLOCK_OLD_SOURCES:
          for(num_src = 0; num_src < grec->grec_nsrcs; num_src++) {
            src.s_addr = grec->grec_src[num_src];
            mc_fdb = br_mc_fdb_find(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, &src);
            if((NULL != mc_fdb) && 
               (mc_fdb->src_entry.filt_mode == MCAST_INCLUDE)) {
              br_mc_fdb_remove(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_IN_CLEAR, &src);
            }
            else {
              br_mc_fdb_add(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_EX_ADD, &src);
            }
          }

          if(0 == grec->grec_nsrcs) {
            src.s_addr = 0;
            br_mc_fdb_add(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_EX_ADD, &src);
          }
        break;
      }
      grec = (struct igmpv3_grec *)((char *)grec + IGMPV3_GRP_REC_SIZE(grec));
    }
  }
  return;
}

int mc_forward(struct net_bridge *br, struct sk_buff *skb, unsigned char *dest,int forward, int clone)
{
	struct net_bridge_mc_fdb_entry *dst;
	struct list_head *lh;
	int status = 0;
	struct sk_buff *skb2;
	struct net_bridge_port *p;
	unsigned char tmp[6];
	struct igmpv3_report *report;
	struct igmpv3_grec *grec;
	int i;
	struct iphdr *pip = skb->nh.iph;
	struct in_addr src;
	union ip_array igmpv3_mcast;
        unsigned char igmp_type = 0;

	if (!snooping)
		return 0;

	if ((snooping == SNOOPING_BLOCKING_MODE) && control_filter(dest))
	    status = 1;

	if (skb->data[9] == IPPROTO_IGMP) {
	    // For proxy; need to add some intelligence here 
	    if (!br->proxy) {
		if(pip->ihl == 5) {
                  igmp_type = skb->data[20];
		} else {
                  igmp_type = skb->data[24];
		}
		if ((igmp_type == IGMPV2_HOST_MEMBERSHIP_REPORT) &&
		    (skb->protocol == __constant_htons(ETH_P_IP))) {
	            src.s_addr = 0;
		    br_mc_fdb_add(br, skb->dev->br_port, dest, skb->mac.ethernet->h_source, SNOOP_EX_ADD, &src);
                }
                else if((igmp_type == IGMPV3_HOST_MEMBERSHIP_REPORT) &&
                        (skb->protocol == __constant_htons(ETH_P_IP))) {
                    if(pip->ihl == 5) {
                      report = (struct igmpv3_report *)&skb->data[20];
                    }
                    else {
                      report = (struct igmpv3_report *)&skb->data[24];
                    }
                    if(report) {
                      br_process_igmpv3(br, skb, dest, report);
                    }
                }
		else if (igmp_type == IGMP_HOST_LEAVE_MESSAGE) {
		    tmp[0] = 0x01;
		    tmp[1] = 0x00;
		    tmp[2] = 0x5e;
                    if(pip->ihl == 5) {
                      tmp[3] = 0x7F & skb->data[24];
                      tmp[4] = skb->data[25];
                      tmp[5] = skb->data[26];
                    } 
                    else {
                      tmp[3] = 0x7F & skb->data[29];
                      tmp[4] = skb->data[30];
                      tmp[5] = skb->data[31];
                    }
	            src.s_addr = 0;
		    br_mc_fdb_remove(br, skb->dev->br_port, tmp, skb->mac.ethernet->h_source, SNOOP_EX_CLEAR, &src);
		}
		else
		    ;
	    }
	    return 0;
	}

	/*
	if (clone) {
		struct sk_buff *skb3;

		if ((skb3 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
			br->statistics.tx_dropped++;
			return;
		}

		skb = skb3;
	}
	*/
	
	list_for_each_rcu(lh, &br->mc_list) {
	    dst = (struct net_bridge_mc_fdb_entry *) list_entry(lh, struct net_bridge_mc_fdb_entry, list);
	    if (!memcmp(&dst->addr, dest, ETH_ALEN)) {
              if((dst->src_entry.filt_mode == MCAST_INCLUDE) && 
                 (pip->saddr == dst->src_entry.src.s_addr)) {

		if (!dst->dst->dirty) {
		    skb2 = skb_clone(skb, GFP_ATOMIC);
		    if (forward)
			br_forward(dst->dst, skb2);
		    else
			br_deliver(dst->dst, skb2);
		}
		dst->dst->dirty = 1;
		status = 1;
              }
              else if(dst->src_entry.filt_mode == MCAST_EXCLUDE) {
                if((0 == dst->src_entry.src.s_addr) ||
                   (pip->saddr != dst->src_entry.src.s_addr)) {

		  if (!dst->dst->dirty) {
		    skb2 = skb_clone(skb, GFP_ATOMIC);
		    if (forward)
			br_forward(dst->dst, skb2);
		    else
			br_deliver(dst->dst, skb2);
		  }
		  dst->dst->dirty = 1;
		  status = 1;
                }
                else if(pip->saddr == dst->src_entry.src.s_addr) {
                  status = 1;
                }
              }
	    }
	}
	if (status) {
	    list_for_each_entry_rcu(p, &br->port_list, list) {
		p->dirty = 0;
	  }
	}

	if ((!forward) && (status))
	kfree_skb(skb);

	return status;
}
#endif

static int br_pass_frame_up_finish(struct sk_buff *skb)
{
#ifdef CONFIG_NETFILTER_DEBUG
	skb->nf_debug = 0;
#endif
#if defined(CONFIG_MIPS_BRCM)
	/* If pass up to IP, remove VLAN header */
	if (skb->protocol == __constant_htons(ETH_P_8021Q)) {
		unsigned short proto;
		struct vlan_hdr *vhdr = (struct vlan_hdr *)(skb->data);
		proto = vhdr->h_vlan_encapsulated_proto;
		struct sk_buff *skb1=skb_copy(skb,GFP_ATOMIC);
		kfree_skb(skb);
		if (skb1) {
			memmove(skb1->data - ETH_HLEN + VLAN_HLEN,
				skb1->data - ETH_HLEN, 12);
			skb_pull(skb1, VLAN_HLEN);
			skb1->mac.raw += VLAN_HLEN;
			skb1->nh.raw += VLAN_HLEN;
			skb1->h.raw += VLAN_HLEN;
			/* make sure protocol is correct before passing up */
			skb1->protocol = proto;
			netif_rx(skb1);
		}
		/* TODO: do we need to assign skb->priority? */
	}
	else
#endif
	netif_rx(skb);

	return 0;
}

static void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb)
{
	struct net_device *indev;

	br->statistics.rx_packets++;
	br->statistics.rx_bytes += skb->len;

	indev = skb->dev;
	skb->dev = br->dev;

	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
			br_pass_frame_up_finish);
}

int br_handle_frame_finish(struct sk_buff *skb)
{
	struct net_bridge *br;
	unsigned char *dest;
#if defined(CONFIG_MIPS_BRCM)
	unsigned char *src;
#endif
	struct net_bridge_fdb_entry *dst;
	struct net_bridge_port *p;
	int passedup;

	dest = skb->mac.ethernet->h_dest;
#if defined(CONFIG_MIPS_BRCM)
	src = skb->mac.ethernet->h_source;
	struct iphdr *pip = skb->nh.iph;
        unsigned char igmp_type = 0;
#endif
	
	rcu_read_lock();
	p = skb->dev->br_port;
	smp_read_barrier_depends();

	if (p == NULL || p->state == BR_STATE_DISABLED) {
		kfree_skb(skb);
		goto out;
	}

	br = p->br;
	passedup = 0;
	if (br->dev->flags & IFF_PROMISC) {
		struct sk_buff *skb2;

		skb2 = skb_clone(skb, GFP_ATOMIC);
		if (skb2 != NULL) {
			passedup = 1;
			br_pass_frame_up(br, skb2);
		}
	}

	if (dest[0] & 1) {
#if defined(CONFIG_MIPS_BRCM)
		if (snooping && br->proxy) {
		  if (skb->data[9] == IPPROTO_IGMP) {
		    char destS[16];
		    char srcS[16];
                    
                    if(pip->ihl == 5) {
                      igmp_type = skb->data[20];
                    } else {
                      igmp_type = skb->data[24];
                    }

		    if (igmp_type == IGMP_HOST_LEAVE_MESSAGE) {
			unsigned char tmp[6];
			
                      if(pip->ihl == 5) {
                        brcm_conv_ip_to_mac(&skb->data[24], tmp);
                      } else {
                        brcm_conv_ip_to_mac(&skb->data[28], tmp);
                      }
			addr_conv(tmp, destS);
		    }
		    else
			addr_conv(dest, destS);
		    addr_conv(src, srcS);
		    sprintf(skb->extif, "%s %s %s/%s", br->dev->name, p->dev->name, destS, srcS);
		  }
		}
		if (!mc_forward(br, skb, dest, 1, !passedup))		
#endif
		br_flood_forward(br, skb, !passedup);
		if (!passedup)
			br_pass_frame_up(br, skb);
		goto out;
	}

	dst = __br_fdb_get(br, dest);
	if (dst != NULL && dst->is_local) {
		if (!passedup)
			br_pass_frame_up(br, skb);
		else
			kfree_skb(skb);
		goto out;
	}

	if (dst != NULL) {
		br_forward(dst->dst, skb);
		goto out;
	}

	br_flood_forward(br, skb, 0);

out:
	rcu_read_unlock();
	return 0;
}

int br_handle_frame(struct sk_buff *skb)
{
	unsigned char *dest;
	struct net_bridge_port *p;

	dest = skb->mac.ethernet->h_dest;

	rcu_read_lock();
	p = skb->dev->br_port;
	if (p == NULL || p->state == BR_STATE_DISABLED)
		goto err;

	if (skb->mac.ethernet->h_source[0] & 1)
		goto err;

	if (p->state == BR_STATE_LEARNING ||
	    p->state == BR_STATE_FORWARDING)
		br_fdb_insert(p->br, p, skb->mac.ethernet->h_source, 0);

	if (p->br->stp_enabled &&
	    !memcmp(dest, bridge_ula, 5) &&
	    !(dest[5] & 0xF0)) {
		if (!dest[5]) {
			NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, skb->dev, 
				NULL, br_stp_handle_bpdu);
			rcu_read_unlock();
			return 0;
		}
	}

	else if (p->state == BR_STATE_FORWARDING) {
		if (br_should_route_hook && br_should_route_hook(&skb)) {
			rcu_read_unlock();
			return -1;
		}

		if (!memcmp(p->br->dev->dev_addr, dest, ETH_ALEN))
			skb->pkt_type = PACKET_HOST;

		NF_HOOK(PF_BRIDGE, NF_BR_PRE_ROUTING, skb, skb->dev, NULL, br_handle_frame_finish);
		rcu_read_unlock();
		return 0;
	}

err:
	rcu_read_unlock();
	kfree_skb(skb);
	return 0;
}


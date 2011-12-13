/*
 * H.323 connection tracking helper
 *
 * Copyright (c) 2006 Jing Min Zhao <zhaojingmin@users.sourceforge.net>
 *
 * This source code is licensed under General Public License version 2.
 *
 * Based on the 'brute force' H.323 connection tracking module by
 * Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 * For more information, please visit http://nath323.sourceforge.net/
 *
 * Changes:
 * 	2006-02-01 - initial version 0.1
 *
 *	2006-02-20 - version 0.2
 *	  1. Changed source format to follow kernel conventions
 *	  2. Deleted some unnecessary structures
 *	  3. Minor fixes
 *
 * 	2006-03-10 - version 0.3
 * 	  1. Added support for multiple TPKTs in one packet (suggested by
 * 	     Patrick McHardy)
 * 	  2. Avoid excessive stack usage (based on Patrick McHardy's patch)
 * 	  3. Added support for non-linear skb (based on Patrick McHardy's patch)
 * 	  4. Fixed missing H.245 module owner (Patrick McHardy)
 * 	  5. Avoid long RAS expectation chains (Patrick McHardy)
 * 	  6. Fixed incorrect __exit attribute (Patrick McHardy)
 * 	  7. Eliminated unnecessary return code
 * 	  8. Fixed incorrect use of NAT data from conntrack code (suggested by
 * 	     Patrick McHardy)
 * 	  9. Fixed TTL calculation error in RCF
 * 	  10. Added TTL support in RRQ
 * 	  11. Better support for separate TPKT header and data
 *
 * 	2006-03-15 - version 0.4
 * 	  1. Added support for T.120 channels
 * 	  2. Added parameter gkrouted_only (suggested by Patrick McHardy)
 * 	  3. Splitted ASN.1 code and data (suggested by Patrick McHardy)
 * 	  4. Sort ASN.1 data to avoid forwarding declarations (suggested by
 * 	     Patrick McHardy)
 * 	  5. Reset next TPKT data length in get_tpkt_data()
 *
 * 	2006-03-20 - version 0.5
 * 	  1. Change gkrouted_only default to 1 (Patrick McHardy)
 * 	  2. Remove #ifndef offsetof in ASN.1 parser (Patrick McHardy)
 * 	  3. Initialize datalen to 0 in ras_help to silence an incorrect gcc
 * 	     warning (Patrick McHardy)
 *
 * 	2006-03-22 - version 0.6
 * 	  1. get_h225_addr incorrectly defined as 'static' (found by
 * 	     Andrew Morton)
 *
 * 	2006-03-25 - version 0.7
 * 	  1. Added new parameter 'default_rrq_ttl'.
 * 	  2. Made get_h245_addr static (suggested by Adrian Bunk)
 * 	  3. Changed to use EXPORT_SYMBOL_GPL (suggested by Adrian Bunk)
 *
 * 	2006-04-05 - version 0.8
 * 	  1. Changed default_rrq_ttl from int to uint (Patrick McHardy)
 * 	  2. Removed some trailing spaces
 *
 * 	2006-04-20 - version 0.9
 * 	  1. Added support for Call Forwarding
 *
 * 	2006-04-21 - version 1.0
 * 	  1. Bug fix of Call Forwarding
 * 	  2. Renamed confusing parameter 'cf_lan' to 'internal_net'
 *
 * 	2006-04-28 - version 1.0a
 * 	  1. Back-ported to 2.6.8
 *
 * 	2006-05-29 - version 1.1a
 * 	  1. Bugfix of T.120 channels in OLCA
 *
 *  2006-06-04 - version 1.0b
 *    1. Disable gkrouted_only to accept all kinds of calls
 *    2. Added 3 dividing types to distingush TPKT packets and updated 'get_tpkt_data'
 *       DIVTYPE_NORMAL:  tpkt header and payload are wrapped in one packet
 *       DIVTYPE_TPKTHDR: tpkt header is in the first packet and payload is
 *                        the next one
 *       DIVTYPE_Q931:    tpkt packet(size maybe is more than several kbytes)is seperated
 *                        into several parts by the tcp protocol. This dividing
 *                        method is different from the second one.
 *    3. Added 'process_q931_bf' and 'process_h245_bf' to process DIVTYPE_Q931 packet.
 *       Because the fragment is unknowned by asn1 parser, 'proces_xxx_bf' use brute
 *       force method to replace NATed IP.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/ctype.h>
#include <linux/inet.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_conntrack_h323.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

/* Parameters */
static unsigned int default_rrq_ttl = 300;
module_param(default_rrq_ttl, uint, 0600);
MODULE_PARM_DESC(default_rrq_ttl, "use this TTL if it's missing in RRQ");

static int gkrouted_only = 0;//1;
module_param(gkrouted_only, int, 0600);
MODULE_PARM_DESC(gkrouted_only, "only accept calls from gatekeeper");

static char *internal_net = NULL;
static u_int32_t internal_net_addr = 0;
static u_int32_t internal_net_mask = 0;
module_param(internal_net, charp, 0600);
MODULE_PARM_DESC(internal_net, "specify your internal network using format "
		 "address/mask. this is used by call forwarding support");

/* Hooks for NAT */
int (*set_h225_addr_hook) (struct sk_buff ** pskb,
			   unsigned char **data, int dataoff,
			   TransportAddress * addr,
			   u_int32_t ip, u_int16_t port);
int (*set_sig_addr_hook) (struct sk_buff ** pskb,
			  struct ip_conntrack * ct,
			  enum ip_conntrack_info ctinfo,
			  unsigned char **data,
			  TransportAddress * addr, int count);
int (*set_ras_addr_hook) (struct sk_buff ** pskb,
			  struct ip_conntrack * ct,
			  enum ip_conntrack_info ctinfo,
			  unsigned char **data,
			  TransportAddress * addr, int count);
int (*nat_rtp_rtcp_hook) (struct sk_buff ** pskb,
			  struct ip_conntrack * ct,
			  enum ip_conntrack_info ctinfo,
			  unsigned char **data, int dataoff,
			  H245_TransportAddress * addr,
			  u_int16_t port, u_int16_t rtp_port,
			  struct ip_conntrack_expect * rtp_exp,
			  struct ip_conntrack_expect * rtcp_exp);
int (*nat_t120_hook) (struct sk_buff ** pskb,
		      struct ip_conntrack * ct,
		      enum ip_conntrack_info ctinfo,
		      unsigned char **data, int dataoff,
		      H245_TransportAddress * addr, u_int16_t port,
		      struct ip_conntrack_expect * exp);
int (*nat_h245_hook) (struct sk_buff ** pskb,
		      struct ip_conntrack * ct,
		      enum ip_conntrack_info ctinfo,
		      unsigned char **data, int dataoff,
		      TransportAddress * addr, u_int16_t port,
		      struct ip_conntrack_expect * exp);
int (*nat_callforwarding_hook) (struct sk_buff ** pskb,
				struct ip_conntrack * ct,
				enum ip_conntrack_info ctinfo,
				unsigned char **data, int dataoff,
				TransportAddress * addr, u_int16_t port,
				struct ip_conntrack_expect * exp);
int (*nat_q931_hook) (struct sk_buff ** pskb,
		      struct ip_conntrack * ct,
		      enum ip_conntrack_info ctinfo,
		      unsigned char **data, TransportAddress * addr, int idx,
		      u_int16_t port, struct ip_conntrack_expect * exp);

static DECLARE_LOCK(ip_h323_lock);
static char *h323_buffer;

/****************************************************************************/
static int get_tpkt_data(struct sk_buff **pskb, struct ip_conntrack *ct,
			 enum ip_conntrack_info ctinfo,
			 unsigned char **data, int *datalen, int *dataoff)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	struct tcphdr tcph;
	int tcpdatalen;
	int tcpdataoff;
	unsigned char *tpkt;
	int tpktlen;
	int tpktoff = 0;

	/* Get TCP header */
	if (skb_copy_bits(*pskb, (*pskb)->nh.iph->ihl * 4,
			  &tcph, sizeof(tcph)) != 0)
		return 0;

	DEBUGP("get_tpkt_data: seq/ack_seq %u / %u\n", tcph.seq, tcph.ack_seq);

	/* Get TCP data offset */
	tcpdataoff = (*pskb)->nh.iph->ihl * 4 + tcph.doff * 4;

	/* Get TCP data length */
	tcpdatalen = (*pskb)->len - tcpdataoff;
	if (tcpdatalen <= 0)	/* No TCP data */
		goto clear_out;

	if (*data == NULL) {	/* first TPKT */
		/* Get first TPKT pointer */
		if (skb_copy_bits(*pskb, tcpdataoff, h323_buffer,
				  tcpdatalen) != 0)
			return -1;
		tpkt = h323_buffer;
		
		/* Validate TPKT identifier */
		if (tcpdatalen < 4 || tpkt[0] != 0x03 || tpkt[1] != 0 ||
			info->tpkt_len[dir] == DIVTYPE_TPKTHDR/*BRCM*/)  {

			/* Netmeeting sends TPKT header and data separately */
			if (info->tpkt_len[dir] > 0) {
				DEBUGP("get_tpkt_data: previous packet "
				       "indicated separate TPKT data of %hu "
				       "bytes; div_type %d\n", info->tpkt_len[dir], info->div_type[dir]);
				if (info->tpkt_len[dir] <= tcpdatalen) {
					/* Yes, there was a TPKT header
					 * received */
					*data = tpkt;
					*datalen = info->tpkt_len[dir];
					*dataoff = 0;
					goto out;
				}
				else
				{
					/* BRCM: there will be another part which can be
					 * received */
					*data = tpkt;
					*datalen = tcpdatalen;
					*dataoff = 0;
					info->tpkt_len[dir] = info->tpkt_len[dir] - tcpdatalen;
					return 1;
				}

				/* Fragmented TPKT */
				if (net_ratelimit())
					printk("get_tpkt_data: "
					       "fragmented TPKT\n");
				goto clear_out;
			}

			tpktoff = 0;

			/* It is not even a TPKT */
			return 0;
		}
		
	} else {		/* Next TPKT */

		/*BRCM: Do not clear that, just break when it returns from this procedure*/
		if(info->div_type[dir] == DIVTYPE_Q931 &&
		   info->tpkt_len[dir] > 0)
			return 0;

		
		tpktoff = *dataoff + *datalen;
		tcpdatalen -= tpktoff;
		if (tcpdatalen <= 4)	/* No more TPKT */
			goto clear_out;
		tpkt = *data + *datalen;

		/* Validate TPKT identifier */
		if (tpkt[0] != 0x03 || tpkt[1] != 0)
			goto clear_out;
	}

	/* Validate TPKT length */
	tpktlen = tpkt[2] * 256 + tpkt[3];
	
	if (tpktlen < 4)
		goto clear_out;
	else if (tpktlen > tcpdatalen) {
		if (tcpdatalen == 4) {	/* Separate TPKT header */
			/* Netmeeting sends TPKT header and data separately */
			DEBUGP("get_tpkt_data: separate TPKT header indicates "
			       "there will be TPKT data of %hu bytes\n",
			       tpktlen - 4);
			info->tpkt_len[dir] = tpktlen - 4;
			info->div_type[dir] = DIVTYPE_TPKTHDR;
			return 0;
		} else if (tcpdatalen > 4){ /* BRCM: brute force*/
			/* Some kind of Clients send large Q.931 pkt which is divided into 
			 * PKTS by tcp layer when it is transferred.*/
			DEBUGP("get_tpkt_data: separate Q.931 indicates "
			       "there will be TPKT data of %hu bytes left\n",
			       tpktlen - tcpdatalen);
			info->tpkt_len[dir] = tpktlen - tcpdatalen;
			info->div_type[dir] = DIVTYPE_Q931;
			*data = tpkt + 4;
			*datalen = tcpdatalen - 4;
			*dataoff = 4;
			return 1; 
		}

		if (net_ratelimit())
			printk("get_tpkt_data: incomplete TPKT (fragmented?)\n");
		goto clear_out;
	}


	/* This is the encapsulated data */
	*data = tpkt + 4;
	*datalen = tpktlen - 4;
	*dataoff = tpktoff + 4;
	info->div_type[dir] = DIVTYPE_NORMAL; /* BRCM */

out:
	/* Clear TPKT length */
	info->tpkt_len[dir] = 0;
	return 1;

clear_out:
	info->tpkt_len[dir] = 0;
	info->div_type[dir] = DIVTYPE_NORMAL; /* BRCM */
	return 0;
}

/****************************************************************************/
static int get_h245_addr(unsigned char *data, H245_TransportAddress * addr,
			 u_int32_t * ip, u_int16_t * port)
{
	unsigned char *p;

	if (addr->choice != eH245_TransportAddress_unicastAddress ||
	    addr->unicastAddress.choice != eUnicastAddress_iPAddress)
		return 0;

	p = data + addr->unicastAddress.iPAddress.network;
	*ip = htonl((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]));
	*port = (p[4] << 8) | (p[5]);

	return 1;
}

/****************************************************************************/
static int expect_rtp_rtcp(struct sk_buff **pskb, struct ip_conntrack *ct,
			   enum ip_conntrack_info ctinfo,
			   unsigned char **data, int dataoff,
			   H245_TransportAddress * addr)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	u_int16_t rtp_port;
	struct ip_conntrack_expect *rtp_exp;
	struct ip_conntrack_expect *rtcp_exp;

	/* Read RTP or RTCP address */
	if (!get_h245_addr(*data, addr, &ip, &port) ||
	    ip != ct->tuplehash[dir].tuple.src.ip || port == 0)
		return 0;

	/* RTP port is even */
	rtp_port = port & (~1);

	/* Create expect for RTP */
	if ((rtp_exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	rtp_exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	rtp_exp->tuple.src.u.udp.port = 0;
	rtp_exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
	rtp_exp->tuple.dst.u.udp.port = htons(rtp_port);
	rtp_exp->tuple.dst.protonum = IPPROTO_UDP;
	rtp_exp->mask.src.ip = 0xFFFFFFFF;
	rtp_exp->mask.src.u.udp.port = 0;
	rtp_exp->mask.dst.ip = 0xFFFFFFFF;
	rtp_exp->mask.dst.u.udp.port = 0xFFFF;
	rtp_exp->mask.dst.protonum = 0xFF;
	rtp_exp->flags = 0;

	/* Create expect for RTCP */
	if ((rtcp_exp = ip_conntrack_expect_alloc()) == NULL) {
		kfree(rtp_exp);
		return -1;
	}
	rtcp_exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	rtcp_exp->tuple.src.u.udp.port = 0;
	rtcp_exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
	rtcp_exp->tuple.dst.u.udp.port = htons(rtp_port + 1);
	rtcp_exp->tuple.dst.protonum = IPPROTO_UDP;
	rtcp_exp->mask.src.ip = 0xFFFFFFFF;
	rtcp_exp->mask.src.u.udp.port = 0;
	rtcp_exp->mask.dst.ip = 0xFFFFFFFF;
	rtcp_exp->mask.dst.u.udp.port = 0xFFFF;
	rtcp_exp->mask.dst.protonum = 0xFF;
	rtcp_exp->flags = 0;

	if (ct->tuplehash[dir].tuple.src.ip !=
	    ct->tuplehash[!dir].tuple.dst.ip && nat_rtp_rtcp_hook) {
		/* NAT needed */
		ret = nat_rtp_rtcp_hook(pskb, ct, ctinfo, data, dataoff,
					addr, port, rtp_port, rtp_exp,
					rtcp_exp);
	} else {		/* Conntrack only */
		DEBUGP("ip_ct_h323: expect RTP/RTCP "
		       "%u.%u.%u.%u->%u.%u.%u.%u:%hu/%hu\n",
		       NIPQUAD(rtp_exp->tuple.src.ip),
		       NIPQUAD(rtp_exp->tuple.dst.ip),
		       ntohs(rtp_exp->tuple.dst.u.udp.port),
		       ntohs(rtcp_exp->tuple.dst.u.udp.port));
		ip_conntrack_expect_related(rtp_exp, ct);
		ip_conntrack_expect_related(rtcp_exp, ct);
	}

	return ret;
}

/****************************************************************************/
static int expect_t120(struct sk_buff **pskb,
		       struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, int dataoff,
		       H245_TransportAddress * addr)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp = NULL;

	/* Read T.120 address */
	if (!get_h245_addr(*data, addr, &ip, &port) ||
	    ip != ct->tuplehash[dir].tuple.src.ip || port == 0)
		return 0;

	/* Create expect for T.120 connections */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	exp->tuple.src.u.tcp.port = 0;
	exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_TCP;
	exp->mask.src.ip = 0xFFFFFFFF;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = IP_CT_EXPECT_PERMANENT;	/* Accept multiple channels */

	if (ct->tuplehash[dir].tuple.src.ip !=
	    ct->tuplehash[!dir].tuple.dst.ip && nat_t120_hook) {
		/* NAT needed */
		ret = nat_t120_hook(pskb, ct, ctinfo, data, dataoff, addr,
				    port, exp);
	} else {		/* Conntrack only */
		DEBUGP("ip_ct_h323: expect T.120 "
		       "%u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
		       NIPQUAD(exp->tuple.src.ip),
		       NIPQUAD(exp->tuple.dst.ip),
		       ntohs(exp->tuple.dst.u.tcp.port));
		ip_conntrack_expect_related(exp, ct);
	}

	return ret;
}

/****************************************************************************/
static int process_h245_channel(struct sk_buff **pskb,
				struct ip_conntrack *ct,
				enum ip_conntrack_info ctinfo,
				unsigned char **data, int dataoff,
				H2250LogicalChannelParameters * channel)
{
	int ret;

	if (channel->options & eH2250LogicalChannelParameters_mediaChannel) {
		/* RTP */
		ret = expect_rtp_rtcp(pskb, ct, ctinfo, data, dataoff,
				      &channel->mediaChannel);
		if (ret < 0)
			return -1;
	}

	if (channel->
	    options & eH2250LogicalChannelParameters_mediaControlChannel) {
		/* RTCP */
		ret = expect_rtp_rtcp(pskb, ct, ctinfo, data, dataoff,
				      &channel->mediaControlChannel);
		if (ret < 0)
			return -1;
	}

	return 0;
}

/****************************************************************************/
static int process_olc(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, int dataoff,
		       OpenLogicalChannel * olc)
{
	int ret;

	DEBUGP("ip_ct_h323: OpenLogicalChannel\n");

	if (olc->forwardLogicalChannelParameters.multiplexParameters.choice ==
	    eOpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters_h2250LogicalChannelParameters)
	{
		ret = process_h245_channel(pskb, ct, ctinfo, data, dataoff,
					   &olc->
					   forwardLogicalChannelParameters.
					   multiplexParameters.
					   h2250LogicalChannelParameters);
		if (ret < 0)
			return -1;
	}

	if ((olc->options &
	     eOpenLogicalChannel_reverseLogicalChannelParameters) &&
	    (olc->reverseLogicalChannelParameters.options &
	     eOpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters)
	    && (olc->reverseLogicalChannelParameters.multiplexParameters.
		choice ==
		eOpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters_h2250LogicalChannelParameters))
	{
		ret =
		    process_h245_channel(pskb, ct, ctinfo, data, dataoff,
					 &olc->
					 reverseLogicalChannelParameters.
					 multiplexParameters.
					 h2250LogicalChannelParameters);
		if (ret < 0)
			return -1;
	}

	if ((olc->options & eOpenLogicalChannel_separateStack) &&
	    olc->forwardLogicalChannelParameters.dataType.choice ==
	    eDataType_data &&
	    olc->forwardLogicalChannelParameters.dataType.data.application.
	    choice == eDataApplicationCapability_application_t120 &&
	    olc->forwardLogicalChannelParameters.dataType.data.application.
	    t120.choice == eDataProtocolCapability_separateLANStack &&
	    olc->separateStack.networkAddress.choice ==
	    eNetworkAccessParameters_networkAddress_localAreaAddress) {
		ret = expect_t120(pskb, ct, ctinfo, data, dataoff,
				  &olc->separateStack.networkAddress.
				  localAreaAddress);
		if (ret < 0)
			return -1;
	}

	return 0;
}

/****************************************************************************/
static int process_olca(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo,
			unsigned char **data, int dataoff,
			OpenLogicalChannelAck * olca)
{
	H2250LogicalChannelAckParameters *ack;
	int ret;

	DEBUGP("ip_ct_h323: OpenLogicalChannelAck\n");

	if ((olca->options &
	     eOpenLogicalChannelAck_reverseLogicalChannelParameters) &&
	    (olca->reverseLogicalChannelParameters.options &
	     eOpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters)
	    && (olca->reverseLogicalChannelParameters.multiplexParameters.
		choice ==
		eOpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters_h2250LogicalChannelParameters))
	{
		ret = process_h245_channel(pskb, ct, ctinfo, data, dataoff,
					   &olca->
					   reverseLogicalChannelParameters.
					   multiplexParameters.
					   h2250LogicalChannelParameters);
		if (ret < 0)
			return -1;
	}

	if ((olca->options &
	     eOpenLogicalChannelAck_forwardMultiplexAckParameters) &&
	    (olca->forwardMultiplexAckParameters.choice ==
	     eOpenLogicalChannelAck_forwardMultiplexAckParameters_h2250LogicalChannelAckParameters))
	{
		ack = &olca->forwardMultiplexAckParameters.
		    h2250LogicalChannelAckParameters;
		if (ack->options &
		    eH2250LogicalChannelAckParameters_mediaChannel) {
			/* RTP */
			ret = expect_rtp_rtcp(pskb, ct, ctinfo, data, dataoff,
					      &ack->mediaChannel);
			if (ret < 0)
				return -1;
		}

		if (ack->options &
		    eH2250LogicalChannelAckParameters_mediaControlChannel) {
			/* RTCP */
			ret = expect_rtp_rtcp(pskb, ct, ctinfo, data, dataoff,
					      &ack->mediaControlChannel);
			if (ret < 0)
				return -1;
		}
	}

	if ((olca->options & eOpenLogicalChannelAck_separateStack) &&
	    olca->separateStack.networkAddress.choice ==
	    eNetworkAccessParameters_networkAddress_localAreaAddress) {
		ret = expect_t120(pskb, ct, ctinfo, data, dataoff,
				  &olca->separateStack.networkAddress.
				  localAreaAddress);
		if (ret < 0)
			return -1;
	}

	return 0;
}

/****************************************************************************/
static int process_h245(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo,
			unsigned char **data, int dataoff,
			MultimediaSystemControlMessage * mscm)
{
	switch (mscm->choice) {
	case eMultimediaSystemControlMessage_request:
		if (mscm->request.choice ==
		    eRequestMessage_openLogicalChannel) {
			return process_olc(pskb, ct, ctinfo, data, dataoff,
					   &mscm->request.openLogicalChannel);
		}
		DEBUGP("ip_ct_h323: H.245 Request %d\n",
		       mscm->request.choice);
		break;
	case eMultimediaSystemControlMessage_response:
		if (mscm->response.choice ==
		    eResponseMessage_openLogicalChannelAck) {
			return process_olca(pskb, ct, ctinfo, data, dataoff,
					    &mscm->response.
					    openLogicalChannelAck);
		}
		DEBUGP("ip_ct_h323: H.245 Response %d\n",
		       mscm->response.choice);
		break;
	default:
		DEBUGP("ip_ct_h323: H.245 signal %d\n", mscm->choice);
		break;
	}

	return 0;
}

//BRCM: Brute force
/****************************************************************************/
static int process_h245_bf(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo)
{
	int i;
	unsigned char *data, *p_ip_data;
	unsigned char *data_limit;
	u_int32_t data_ip;
	u_int32_t datalen;
	struct tcphdr *tcph;
	struct iphdr *iph = (*pskb)->nh.iph;
	int dir = CTINFO2DIR(ctinfo);

	/*do not look in packets*/
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		return 0;
	}

	tcph = skb_header_pointer(*pskb, (*pskb)->nh.iph->ihl*4,
				  sizeof(struct tcphdr), NULL);

	if (tcph == NULL)
		return -1;
	
	data = (unsigned char *) tcph + tcph->doff * 4;	
	datalen = (*pskb)->len - (*pskb)->nh.iph->ihl*4 - tcph->doff*4;
	data_limit = data + datalen - 6;
	
	for (i = 0; data <= data_limit; data++, i++) 
	{

		data_ip = *((u_int32_t *)data);
		p_ip_data = data;

		if(data_ip == ct->tuplehash[dir].tuple.src.ip)
		{
			memcpy(p_ip_data, &ct->tuplehash[!dir].tuple.dst.ip, sizeof(ct->tuplehash[!dir].tuple.dst.ip));
		}

	}

	
    //update tcp check
	(*pskb)->csum = csum_partial((char *)tcph + tcph->doff*4,
				     datalen, 0);

	tcph->check = 0;
	tcph->check = tcp_v4_check(tcph, (*pskb)->len - iph->ihl * 4, 
				  iph->saddr, iph->daddr, csum_partial((char *)tcph, 
				  tcph->doff*4,(*pskb)->csum));
	
	ip_send_check(iph);

	return 0;
}

/****************************************************************************/
static int h245_help(struct sk_buff **pskb, struct ip_conntrack *ct,
		     enum ip_conntrack_info ctinfo)
{
	static MultimediaSystemControlMessage mscm;
	unsigned char *data = NULL;
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	int datalen;
	int dataoff;
	int ret;

#if 1
    struct tcphdr tcph;
    /* Get TCP header */
	if (skb_copy_bits(*pskb, (*pskb)->nh.iph->ihl * 4,
			  &tcph, sizeof(tcph)) != 0)
		return 0;
    DEBUGP("ip_ct_h245: %u.%u.%u.%u->%u.%u.%u.%u seq/ack_seq %u / %u  %d\n", 
           NIPQUAD((*pskb)->nh.iph->saddr), NIPQUAD((*pskb)->nh.iph->daddr), tcph.seq, tcph.ack_seq, ctinfo);
#endif

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		return NF_ACCEPT;
	}
	DEBUGP("ip_ct_h245: skblen = %u\n", (*pskb)->len);

	LOCK_BH(&ip_h323_lock);

	/* Process each TPKT */
	while (get_tpkt_data(pskb, ct, ctinfo, &data, &datalen, &dataoff)) {
		DEBUGP("ip_ct_h245: TPKT %u.%u.%u.%u->%u.%u.%u.%u, datalen=%d, dataoff=%d, divtype=%d\n",
		       NIPQUAD((*pskb)->nh.iph->saddr),
		       NIPQUAD((*pskb)->nh.iph->daddr), 
		       datalen, dataoff,info->div_type[dir] );
		
		if(info->div_type[dir] != DIVTYPE_Q931)//BRCM
		{
			/* Decode H.245 signal */
			ret = DecodeMultimediaSystemControlMessage(data, datalen,
								   &mscm);
			if (ret < 0) {
				if (net_ratelimit())
					printk("ip_ct_h245: decoding error: %s\n",
					       ret == H323_ERROR_BOUND ?
					       "out of bound" : "out of range");
				/* We don't drop when decoding error */
				break;
			}

			/* Process H.245 signal */
			if (process_h245(pskb, ct, ctinfo, &data, dataoff, &mscm) < 0)
				goto drop;
		}
		else
		{
			//BRCM: brute force
			if (process_h245_bf(pskb, ct, ctinfo) < 0)
				goto drop;
			
		}
	}

	UNLOCK_BH(&ip_h323_lock);
	return NF_ACCEPT;

      drop:
	UNLOCK_BH(&ip_h323_lock);
	if (net_ratelimit())
		printk("ip_ct_h245: packet dropped\n");
	return NF_DROP;
}

/****************************************************************************/
static struct ip_conntrack_helper ip_conntrack_helper_h245 = {
	.name = "H.245",
	.flags = IP_CT_HELPER_F_REUSE_EXPECT,
	.me = THIS_MODULE,
	.max_expected = 0,
	.timeout = 240,
	.tuple = {.dst = {.protonum = IPPROTO_TCP}},
	.mask = {.src = {.u = {.tcp = {.port = 0xFFFF}}},
		 .dst = {.protonum = 0xFF}},
	.help2 = h245_help
};

/****************************************************************************/
int ip_conntrack_h245_expect(struct ip_conntrack *new)
{
	WRITE_LOCK(&ip_conntrack_lock);
	new->helper = &ip_conntrack_helper_h245;
	WRITE_UNLOCK(&ip_conntrack_lock);
	return NF_ACCEPT;
}

/****************************************************************************/
int get_h225_addr(unsigned char *data, TransportAddress * addr,
		  u_int32_t * ip, u_int16_t * port)
{
	unsigned char *p;

	if (addr->choice != eTransportAddress_ipAddress)
		return 0;

	p = data + addr->ipAddress.ip;
	*ip = htonl((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]));
	*port = (p[4] << 8) | (p[5]);

	return 1;
}

/****************************************************************************/
static int expect_h245(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, int dataoff,
		       TransportAddress * addr)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp = NULL;

	/* Read h245Address */
#ifdef CONFIG_IP_NF_H323_SIGNAL_PROXY_MODULE
	if (!get_h225_addr(*data, addr, &ip, &port) || port == 0)
#else
	if (!get_h225_addr(*data, addr, &ip, &port) ||
	    ip != ct->tuplehash[dir].tuple.src.ip || port == 0)
#endif
		return 0;

	/* Create expect for h245 connection */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = 0; //ct->tuplehash[!dir].tuple.src.ip;
	exp->tuple.src.u.tcp.port = 0;
#ifdef CONFIG_IP_NF_H323_SIGNAL_PROXY_MODULE
	exp->tuple.dst.ip = ip;
#else
	exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
#endif
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_TCP;
	exp->mask.src.ip = 0; //0xFFFFFFFF;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = 0;

	if (ct->tuplehash[dir].tuple.src.ip !=
	    ct->tuplehash[!dir].tuple.dst.ip && nat_h245_hook) {
		/* NAT needed */
		ret = nat_h245_hook(pskb, ct, ctinfo, data, dataoff, addr,
				    port, exp);
	} else {		/* Conntrack only */
		DEBUGP("ip_ct_q931: expect H.245 "
		       "%u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
		       NIPQUAD(exp->tuple.src.ip),
		       NIPQUAD(exp->tuple.dst.ip),
		       ntohs(exp->tuple.dst.u.tcp.port));
		exp->expectfn = ip_conntrack_h245_expect;
		ip_conntrack_expect_related(exp, ct);
	}

	return ret;
}

/* Forwarding declaration */
int ip_conntrack_q931_expect(struct ip_conntrack *new);

/****************************************************************************/
static int expect_callforwarding(struct sk_buff **pskb,
				 struct ip_conntrack *ct,
				 enum ip_conntrack_info ctinfo,
				 unsigned char **data, int dataoff,
				 TransportAddress * addr)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp = NULL;

	/* Read alternativeAddress */
	if (!get_h225_addr(*data, addr, &ip, &port) || port == 0)
		return 0;

	/* If the calling party is on the same side of the forward-to party,
	 * we don't need to track the second call */
	if (internal_net &&
	    ((ip & internal_net_mask) == internal_net_addr) ==
	    ((ct->tuplehash[!dir].tuple.src.ip & internal_net_mask) ==
	     internal_net_addr)) {
		DEBUGP("ip_ct_q931: Call Forwarding not tracked\n");
		return 0;
	}

	/* Create expect for the second call leg */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	exp->tuple.src.u.tcp.port = 0;
	exp->tuple.dst.ip = ip;
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_TCP;
	exp->mask.src.ip = 0xFFFFFFFF;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = 0;

	if (ct->tuplehash[dir].tuple.src.ip !=
	    ct->tuplehash[!dir].tuple.dst.ip && nat_callforwarding_hook) {
		/* Need NAT */
		ret = nat_callforwarding_hook(pskb, ct, ctinfo, data, dataoff,
					      addr, port, exp);
	} else {		/* Conntrack only */
		DEBUGP("ip_ct_q931: expect Call Forwarding "
		       "%u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
		       NIPQUAD(exp->tuple.src.ip),
		       NIPQUAD(exp->tuple.dst.ip),
		       ntohs(exp->tuple.dst.u.tcp.port));
		exp->expectfn = ip_conntrack_q931_expect;
		ip_conntrack_expect_related(exp, ct);
	}

	return ret;
}

/****************************************************************************/
static int process_setup(struct sk_buff **pskb, struct ip_conntrack *ct,
			 enum ip_conntrack_info ctinfo,
			 unsigned char **data, int dataoff,
			 Setup_UUIE * setup)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret;
	int i;
	u_int32_t ip;
	u_int16_t port;

	DEBUGP("ip_ct_q931: Setup\n");

	if (setup->options & eSetup_UUIE_h245Address) {
		ret = expect_h245(pskb, ct, ctinfo, data, dataoff,
				  &setup->h245Address);
		if (ret < 0)
			return -1;
	}

	if ((setup->options & eSetup_UUIE_destCallSignalAddress) &&
	    (set_h225_addr_hook) &&
	    get_h225_addr(*data, &setup->destCallSignalAddress, &ip, &port) &&
	    ip != ct->tuplehash[!dir].tuple.src.ip) {
		DEBUGP("ip_ct_q931: set destCallSignalAddress "
		       "%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu\n",
		       NIPQUAD(ip), port,
		       NIPQUAD(ct->tuplehash[!dir].tuple.src.ip),
		       ntohs(ct->tuplehash[!dir].tuple.src.u.tcp.port));
		ret = set_h225_addr_hook(pskb, data, dataoff,
					 &setup->destCallSignalAddress,
					 ct->tuplehash[!dir].tuple.src.ip,
					 ntohs(ct->tuplehash[!dir].tuple.src.
					       u.tcp.port));
		if (ret < 0)
			return -1;
	}

	if ((setup->options & eSetup_UUIE_sourceCallSignalAddress) &&
	    (set_h225_addr_hook) &&
	    get_h225_addr(*data, &setup->sourceCallSignalAddress, &ip, &port)
	    && ip != ct->tuplehash[!dir].tuple.dst.ip) {
		DEBUGP("ip_ct_q931: set sourceCallSignalAddress "
		       "%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu\n",
		       NIPQUAD(ip), port,
		       NIPQUAD(ct->tuplehash[!dir].tuple.dst.ip),
		       ntohs(ct->tuplehash[!dir].tuple.dst.u.tcp.port));
		ret = set_h225_addr_hook(pskb, data, dataoff,
					 &setup->sourceCallSignalAddress,
					 ct->tuplehash[!dir].tuple.dst.ip,
					 ntohs(ct->tuplehash[!dir].tuple.dst.
					       u.tcp.port));
		if (ret < 0)
			return -1;
	}

	if (setup->options & eSetup_UUIE_fastStart) {
		for (i = 0; i < setup->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &setup->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_callproceeding(struct sk_buff **pskb,
				  struct ip_conntrack *ct,
				  enum ip_conntrack_info ctinfo,
				  unsigned char **data, int dataoff,
				  CallProceeding_UUIE * callproc)
{
	int ret;
	int i;

	DEBUGP("ip_ct_q931: CallProceeding\n");

	if (callproc->options & eCallProceeding_UUIE_h245Address) {
		ret = expect_h245(pskb, ct, ctinfo, data, dataoff,
				  &callproc->h245Address);
		if (ret < 0)
			return -1;
	}

	if (callproc->options & eCallProceeding_UUIE_fastStart) {
		for (i = 0; i < callproc->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &callproc->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_connect(struct sk_buff **pskb, struct ip_conntrack *ct,
			   enum ip_conntrack_info ctinfo,
			   unsigned char **data, int dataoff,
			   Connect_UUIE * connect)
{
	int ret;
	int i;

	DEBUGP("ip_ct_q931: Connect\n");

	if (connect->options & eConnect_UUIE_h245Address) {
		ret = expect_h245(pskb, ct, ctinfo, data, dataoff,
				  &connect->h245Address);
		if (ret < 0)
			return -1;
	}

	if (connect->options & eConnect_UUIE_fastStart) {
		for (i = 0; i < connect->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &connect->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_alerting(struct sk_buff **pskb, struct ip_conntrack *ct,
			    enum ip_conntrack_info ctinfo,
			    unsigned char **data, int dataoff,
			    Alerting_UUIE * alert)
{
	int ret;
	int i;

	DEBUGP("ip_ct_q931: Alerting\n");

	if (alert->options & eAlerting_UUIE_h245Address) {
		ret = expect_h245(pskb, ct, ctinfo, data, dataoff,
				  &alert->h245Address);
		if (ret < 0)
			return -1;
	}

	if (alert->options & eAlerting_UUIE_fastStart) {
		for (i = 0; i < alert->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &alert->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_information(struct sk_buff **pskb,
			       struct ip_conntrack *ct,
			       enum ip_conntrack_info ctinfo,
			       unsigned char **data, int dataoff,
			       Information_UUIE * info)
{
	int ret;
	int i;

	DEBUGP("ip_ct_q931: Information\n");

	if (info->options & eInformation_UUIE_fastStart) {
		for (i = 0; i < info->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &info->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_facility(struct sk_buff **pskb, struct ip_conntrack *ct,
			    enum ip_conntrack_info ctinfo,
			    unsigned char **data, int dataoff,
			    Facility_UUIE * facility)
{
	int ret;
	int i;

	DEBUGP("ip_ct_q931: Facility\n");

	if (facility->reason.choice == eFacilityReason_callForwarded) {
		if (facility->options & eFacility_UUIE_alternativeAddress)
			return expect_callforwarding(pskb, ct, ctinfo, data,
						     dataoff,
						     &facility->
						     alternativeAddress);
		return 0;
	}

	if (facility->options & eFacility_UUIE_h245Address) {
		ret = expect_h245(pskb, ct, ctinfo, data, dataoff,
				  &facility->h245Address);
		if (ret < 0)
			return -1;
	}

	if (facility->options & eFacility_UUIE_fastStart) {
		for (i = 0; i < facility->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &facility->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_progress(struct sk_buff **pskb, struct ip_conntrack *ct,
			    enum ip_conntrack_info ctinfo,
			    unsigned char **data, int dataoff,
			    Progress_UUIE * progress)
{
	int ret;
	int i;

	DEBUGP("ip_ct_q931: Progress\n");

	if (progress->options & eProgress_UUIE_h245Address) {
		ret = expect_h245(pskb, ct, ctinfo, data, dataoff,
				  &progress->h245Address);
		if (ret < 0)
			return -1;
	}

	if (progress->options & eProgress_UUIE_fastStart) {
		for (i = 0; i < progress->fastStart.count; i++) {
			ret = process_olc(pskb, ct, ctinfo, data, dataoff,
					  &progress->fastStart.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int process_q931(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo,
			unsigned char **data, int dataoff, Q931 * q931)
{
	H323_UU_PDU *pdu = &q931->UUIE.h323_uu_pdu;
	int i;
	int ret = 0;

	switch (pdu->h323_message_body.choice) {
	case eH323_UU_PDU_h323_message_body_setup:
		ret = process_setup(pskb, ct, ctinfo, data, dataoff,
				    &pdu->h323_message_body.setup);
		break;
	case eH323_UU_PDU_h323_message_body_callProceeding:
		ret = process_callproceeding(pskb, ct, ctinfo, data, dataoff,
					     &pdu->h323_message_body.
					     callProceeding);
		break;
	case eH323_UU_PDU_h323_message_body_connect:
		ret = process_connect(pskb, ct, ctinfo, data, dataoff,
				      &pdu->h323_message_body.connect);
		break;
	case eH323_UU_PDU_h323_message_body_alerting:
		ret = process_alerting(pskb, ct, ctinfo, data, dataoff,
				       &pdu->h323_message_body.alerting);
		break;
	case eH323_UU_PDU_h323_message_body_information:
		ret = process_information(pskb, ct, ctinfo, data, dataoff,
					  &pdu->h323_message_body.
					  information);
		break;
	case eH323_UU_PDU_h323_message_body_facility:
		ret = process_facility(pskb, ct, ctinfo, data, dataoff,
				       &pdu->h323_message_body.facility);
		break;
	case eH323_UU_PDU_h323_message_body_progress:
		ret = process_progress(pskb, ct, ctinfo, data, dataoff,
				       &pdu->h323_message_body.progress);
		break;
	default:
		DEBUGP("ip_ct_q931: Q.931 signal %d\n",
		       pdu->h323_message_body.choice);
		break;
	}

	if (ret < 0)
		return -1;

	if (pdu->options & eH323_UU_PDU_h245Control) {
		for (i = 0; i < pdu->h245Control.count; i++) {
			ret = process_h245(pskb, ct, ctinfo, data, dataoff,
					   &pdu->h245Control.item[i]);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

//BRCM: Brute force
/****************************************************************************/
static int process_q931_bf(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo)
{
	int i;
	unsigned char *data, *p_ip_data;
	unsigned char *data_limit;
	u_int32_t data_ip;
	u_int32_t datalen;
	struct tcphdr *tcph;
	struct iphdr *iph = (*pskb)->nh.iph;
	int dir = CTINFO2DIR(ctinfo);


	/*do not look in packets*/
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		return 0;
	}

	tcph = skb_header_pointer(*pskb, (*pskb)->nh.iph->ihl*4,
				  sizeof(struct tcphdr), NULL);

	if (tcph == NULL)
		return -1;
	
	data = (unsigned char *) tcph + tcph->doff * 4;	
	datalen = (*pskb)->len - (*pskb)->nh.iph->ihl*4 - tcph->doff*4;
	data_limit = data + datalen - 6;
	
	for (i = 0; data <= data_limit; data++, i++) 
	{

		data_ip = *((u_int32_t *)data);
		p_ip_data = data;

		if(data_ip == ct->tuplehash[dir].tuple.src.ip)
		{
			memcpy(p_ip_data, &ct->tuplehash[!dir].tuple.dst.ip, sizeof(ct->tuplehash[!dir].tuple.dst.ip));
		}

	}

	
    //update tcp check
	(*pskb)->csum = csum_partial((char *)tcph + tcph->doff*4,
				     datalen, 0);

	tcph->check = 0;
	tcph->check = tcp_v4_check(tcph, (*pskb)->len - iph->ihl * 4, 
				  iph->saddr, iph->daddr, csum_partial((char *)tcph, 
				  tcph->doff*4,(*pskb)->csum));
	
	ip_send_check(iph);

	return 0;
}

/****************************************************************************/
static int q931_help(struct sk_buff **pskb, struct ip_conntrack *ct,
		     enum ip_conntrack_info ctinfo)
{
	static Q931 q931;
	unsigned char *data = NULL;
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	int datalen;
	int dataoff;
	int ret;

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		return NF_ACCEPT;
	}
	DEBUGP("ip_ct_q931: skblen = %u\n", (*pskb)->len);

	LOCK_BH(&ip_h323_lock);

	/* Process each TPKT */
	while (get_tpkt_data(pskb, ct, ctinfo, &data, &datalen, &dataoff)) {
		DEBUGP("ip_ct_q931: TPKT %u.%u.%u.%u->%u.%u.%u.%u, datalen=%d, dataoff=%d, divtype=%d\n",
		       NIPQUAD((*pskb)->nh.iph->saddr),
		       NIPQUAD((*pskb)->nh.iph->daddr), 
		       datalen, dataoff,info->div_type[dir] );

		if(info->div_type[dir] != DIVTYPE_Q931)//BRCM
		{
			/* Decode Q.931 signal */
			ret = DecodeQ931(data, datalen, &q931);
			if (ret < 0) {
				if (net_ratelimit())
					printk("ip_ct_q931: decoding error: %s\n",
					       ret == H323_ERROR_BOUND ?
					       "out of bound" : "out of range");
				/* We don't drop when decoding error */
				break;
			}

			/* Process Q.931 signal */
			if (process_q931(pskb, ct, ctinfo, &data, dataoff, &q931) < 0)
				goto drop;
		}
		else
		{	//BRCM: brute force
			if (process_q931_bf(pskb, ct, ctinfo) < 0)
				goto drop;
		}
	
	}

	UNLOCK_BH(&ip_h323_lock);
	return NF_ACCEPT;

drop:
	UNLOCK_BH(&ip_h323_lock);
	if (net_ratelimit())
		printk("ip_ct_q931: packet dropped\n");
	return NF_DROP;
}

/****************************************************************************/
static struct ip_conntrack_helper ip_conntrack_helper_q931 = {
	.name = "Q.931",
	.flags = IP_CT_HELPER_F_REUSE_EXPECT,
	.me = THIS_MODULE,
	.max_expected = 0,
	.timeout = 240,
	.tuple = {.src = {.u = {.tcp = {.port = __constant_htons(Q931_PORT)}}},
		  .dst = {.protonum = IPPROTO_TCP}},
	.mask = {.src = {.u = {.tcp = {.port = 0xFFFF}}},
		 .dst = {.protonum = 0xFF}},
	.help2 = q931_help
};

/****************************************************************************/
int ip_conntrack_q931_expect(struct ip_conntrack *new)
{
	WRITE_LOCK(&ip_conntrack_lock);
	new->helper = &ip_conntrack_helper_q931;
	WRITE_UNLOCK(&ip_conntrack_lock);
	return NF_ACCEPT;
}

/****************************************************************************/
static unsigned char *get_udp_data(struct sk_buff **pskb, int *datalen)
{
	struct udphdr uh;
	int dataoff;

	if (skb_copy_bits(*pskb, (*pskb)->nh.iph->ihl * 4, &uh, sizeof(uh)))
		return NULL;
	dataoff = (*pskb)->nh.iph->ihl * 4 + sizeof(uh);
	if (dataoff >= (*pskb)->len)
		return NULL;
	*datalen = (*pskb)->len - dataoff;
	if (skb_copy_bits(*pskb, dataoff, h323_buffer, *datalen))
		return NULL;
	return h323_buffer;
}

/****************************************************************************/
static struct ip_conntrack_expect *find_q931_expect(struct ip_conntrack *ct,
						    u_int32_t ip,
						    u_int16_t port)
{
	struct list_head *exp_entry, *next;
	struct ip_conntrack_expect *old_exp;

	port = htons(port);

	READ_LOCK(&ip_conntrack_lock);
	READ_LOCK(&ip_conntrack_expect_tuple_lock);
	list_for_each_safe(exp_entry, next, &ct->sibling_list) {
		old_exp = list_entry(exp_entry, struct ip_conntrack_expect,
				     expected_list);
		if (old_exp->sibling)
			continue;
		if (old_exp->tuple.dst.protonum == IPPROTO_TCP &&
		    old_exp->tuple.dst.u.tcp.port == port &&
		    old_exp->tuple.dst.ip == ip) {
			atomic_inc(&old_exp->use);
			READ_UNLOCK(&ip_conntrack_expect_tuple_lock);
			READ_UNLOCK(&ip_conntrack_lock);
			return old_exp;
		}
	}
	READ_UNLOCK(&ip_conntrack_expect_tuple_lock);
	READ_UNLOCK(&ip_conntrack_lock);
	return NULL;
}

/****************************************************************************/
static int set_expect_timeout(struct ip_conntrack_expect *exp,
			      unsigned timeout)
{
	if (!exp || !del_timer(&exp->timeout))
		return 0;

	exp->timeout.expires = jiffies + timeout * HZ;
	add_timer(&exp->timeout);

	return 1;
}

/****************************************************************************/
static int expect_q931(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data,
		       TransportAddress * addr, int count)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	int i;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp;

	/* Look for the first related address */
	for (i = 0; i < count; i++) {
		if (get_h225_addr(*data, &addr[i], &ip, &port) &&
		    ip == ct->tuplehash[dir].tuple.src.ip && port != 0)
			break;
	}

	if (i >= count)		/* Not found */
		return 0;

	/* Create expect for Q.931 */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = gkrouted_only ?	/* only accept calls from GK? */
	    ct->tuplehash[!dir].tuple.src.ip : 0;
	exp->tuple.src.u.tcp.port = 0;
	exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_TCP;
	exp->mask.src.ip = gkrouted_only ? 0xFFFFFFFF : 0;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = IP_CT_EXPECT_PERMANENT;	/* Accept multiple calls */

	if (nat_q931_hook) {	/* Need NAT */
		ret = nat_q931_hook(pskb, ct, ctinfo, data, addr, i,
				    port, exp);
	} else {		/* Conntrack only */
		DEBUGP("ip_ct_ras: expect Q.931 "
		       "%u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
		       NIPQUAD(exp->tuple.src.ip),
		       NIPQUAD(exp->tuple.dst.ip),
		       ntohs(exp->tuple.dst.u.tcp.port));
		exp->expectfn = ip_conntrack_q931_expect;
		ip_conntrack_expect_related(exp, ct);

		/* Save port for looking up expect in processing RCF */
		info->sig_port[dir] = info->sig_port[!dir] = port;
	}

	return ret;
}

/****************************************************************************/
static int process_grq(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, GatekeeperRequest * grq)
{
	DEBUGP("ip_ct_ras: GRQ\n");

	if (set_ras_addr_hook)	/* NATed */
		return set_ras_addr_hook(pskb, ct, ctinfo, data,
					 &grq->rasAddress, 1);
	return 0;
}

/* Declare before using */
static int ip_conntrack_ras_expect(struct ip_conntrack *new);

/****************************************************************************/
static int process_gcf(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, GatekeeperConfirm * gcf)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp;

	DEBUGP("ip_ct_ras: GCF\n");

	if (!get_h225_addr(*data, &gcf->rasAddress, &ip, &port))
		return 0;

	/* Registration port is the same as discovery port */
	if (ip == ct->tuplehash[dir].tuple.src.ip &&
	    port == ntohs(ct->tuplehash[dir].tuple.src.u.udp.port))
		return 0;
	
	/* Avoid RAS expectation loops. A GCF is never expected. */
	if (test_bit(IPS_EXPECTED_BIT, &ct->status))
		return 0;

	/* Need new expect */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	exp->tuple.src.u.tcp.port = 0;
	exp->tuple.dst.ip = ip;
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_UDP;
	exp->mask.src.ip = 0xFFFFFFFF;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = 0;
	exp->expectfn = ip_conntrack_ras_expect;
	DEBUGP("ip_ct_ras: expect RAS %u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       ntohs(exp->tuple.dst.u.tcp.port));
	ip_conntrack_expect_related(exp, ct);

	return ret;
}

/****************************************************************************/
static int process_rrq(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, RegistrationRequest * rrq)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int ret;

	DEBUGP("ip_ct_ras: RRQ\n");

	ret = expect_q931(pskb, ct, ctinfo, data,
			  rrq->callSignalAddress.item,
			  rrq->callSignalAddress.count);
	if (ret < 0)
		return -1;

	if (set_ras_addr_hook) {
		ret = set_ras_addr_hook(pskb, ct, ctinfo, data,
					rrq->rasAddress.item,
					rrq->rasAddress.count);
		if (ret < 0)
			return -1;
	}

	if (rrq->options & eRegistrationRequest_timeToLive) {
		DEBUGP("ip_ct_ras: RRQ TTL = %u seconds\n", rrq->timeToLive);
		info->timeout = rrq->timeToLive;
	} else
		info->timeout = default_rrq_ttl;

	return 0;
}

/****************************************************************************/
static int process_rcf(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, RegistrationConfirm * rcf)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	int ret;
	struct ip_conntrack_expect *exp;

	DEBUGP("ip_ct_ras: RCF\n");

	if (set_sig_addr_hook) {
		ret = set_sig_addr_hook(pskb, ct, ctinfo, data,
					rcf->callSignalAddress.item,
					rcf->callSignalAddress.count);
		if (ret < 0)
			return -1;
	}

	if (rcf->options & eRegistrationConfirm_timeToLive) {
		DEBUGP("ip_ct_ras: RCF TTL = %u seconds\n", rcf->timeToLive);
		info->timeout = rcf->timeToLive;
	}

	if (info->timeout > 0) {
		DEBUGP
		    ("ip_ct_ras: set RAS connection timeout to %u seconds\n",
		     info->timeout);
		ip_ct_refresh(ct, info->timeout * HZ);

		/* Set expect timeout */
		exp = find_q931_expect(ct, ct->tuplehash[dir].tuple.dst.ip,
				       info->sig_port[dir]);
		if (exp) {
			DEBUGP("ip_ct_ras: set Q.931 expect "
			       "(%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu) "
			       "timeout to %u seconds\n",
			       NIPQUAD(exp->tuple.src.ip),
			       ntohs(exp->tuple.src.u.tcp.port),
			       NIPQUAD(exp->tuple.dst.ip),
			       ntohs(exp->tuple.dst.u.tcp.port),
			       info->timeout);
			WRITE_LOCK(&ip_conntrack_lock);
			set_expect_timeout(exp, info->timeout);
			WRITE_UNLOCK(&ip_conntrack_lock);
			ip_conntrack_expect_put(exp);
		}
	}

	return 0;
}

/****************************************************************************/
static int process_urq(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, UnregistrationRequest * urq)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	int ret;

	DEBUGP("ip_ct_ras: URQ\n");

	if (set_sig_addr_hook) {
		ret = set_sig_addr_hook(pskb, ct, ctinfo, data,
					urq->callSignalAddress.item,
					urq->callSignalAddress.count);
		if (ret < 0)
			return -1;
	}

	/* Clear old expect */
	ip_ct_remove_expectations(ct);
	info->sig_port[dir] = 0;
	info->sig_port[!dir] = 0;

	/* Give it 30 seconds for UCF or URJ */
	ip_ct_refresh(ct, 30 * HZ);

	return 0;
}

/****************************************************************************/
static int process_arq(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, AdmissionRequest * arq)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	u_int32_t ip;
	u_int16_t port;

	DEBUGP("ip_ct_ras: ARQ\n");

	if ((arq->options & eAdmissionRequest_destCallSignalAddress) &&
	    get_h225_addr(*data, &arq->destCallSignalAddress, &ip, &port) &&
	    ip == ct->tuplehash[dir].tuple.src.ip &&
	    port == info->sig_port[dir] && set_h225_addr_hook) {
		/* Answering ARQ */
		return set_h225_addr_hook(pskb, data, 0,
					  &arq->destCallSignalAddress,
					  ct->tuplehash[!dir].tuple.dst.ip,
					  info->sig_port[!dir]);
	}

	if ((arq->options & eAdmissionRequest_srcCallSignalAddress) &&
	    get_h225_addr(*data, &arq->srcCallSignalAddress, &ip, &port) &&
	    ip == ct->tuplehash[dir].tuple.src.ip && set_h225_addr_hook) {
		/* Calling ARQ */
		return set_h225_addr_hook(pskb, data, 0,
					  &arq->srcCallSignalAddress,
					  ct->tuplehash[!dir].tuple.dst.ip,
					  port);
	}

	return 0;
}

/****************************************************************************/
static int process_acf(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, AdmissionConfirm * acf)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp;

	DEBUGP("ip_ct_ras: ACF\n");

	if (!get_h225_addr(*data, &acf->destCallSignalAddress, &ip, &port))
		return 0;

	if (ip == ct->tuplehash[dir].tuple.dst.ip) {	/* Answering ACF */
		if (set_sig_addr_hook)
			return set_sig_addr_hook(pskb, ct, ctinfo, data,
						 &acf->destCallSignalAddress,
						 1);
		return 0;
	}

	/* Need new expect */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	exp->tuple.src.u.tcp.port = 0;
	exp->tuple.dst.ip = ip;
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_TCP;
	exp->mask.src.ip = 0xFFFFFFFF;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = IP_CT_EXPECT_PERMANENT;
	exp->expectfn = ip_conntrack_q931_expect;
	DEBUGP("ip_ct_ras: expect Q.931 %u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       ntohs(exp->tuple.dst.u.tcp.port));
	ip_conntrack_expect_related(exp, ct);

	return ret;
}

/****************************************************************************/
static int process_lrq(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, LocationRequest * lrq)
{
	DEBUGP("ip_ct_ras: LRQ\n");

	if (set_ras_addr_hook)
		return set_ras_addr_hook(pskb, ct, ctinfo, data,
					 &lrq->replyAddress, 1);
	return 0;
}

/****************************************************************************/
static int process_lcf(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, LocationConfirm * lcf)
{
	int dir = CTINFO2DIR(ctinfo);
	int ret = 0;
	u_int32_t ip;
	u_int16_t port;
	struct ip_conntrack_expect *exp = NULL;

	DEBUGP("ip_ct_ras: LCF\n");

	if (!get_h225_addr(*data, &lcf->callSignalAddress, &ip, &port))
		return 0;

	/* Need new expect for call signal */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return -1;
	exp->tuple.src.ip = ct->tuplehash[!dir].tuple.src.ip;
	exp->tuple.src.u.tcp.port = 0;
	exp->tuple.dst.ip = ip;
	exp->tuple.dst.u.tcp.port = htons(port);
	exp->tuple.dst.protonum = IPPROTO_TCP;
	exp->mask.src.ip = 0xFFFFFFFF;
	exp->mask.src.u.tcp.port = 0;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.tcp.port = 0xFFFF;
	exp->mask.dst.protonum = 0xFF;
	exp->flags = IP_CT_EXPECT_PERMANENT;
	exp->expectfn = ip_conntrack_q931_expect;
	DEBUGP("ip_ct_ras: expect Q.931 %u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       ntohs(exp->tuple.dst.u.tcp.port));
	ip_conntrack_expect_related(exp, ct);

	/* Ignore rasAddress */

	return ret;
}

/****************************************************************************/
static int process_irr(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, InfoRequestResponse * irr)
{
	int ret;

	DEBUGP("ip_ct_ras: IRR\n");

	if (set_ras_addr_hook) {
		ret = set_ras_addr_hook(pskb, ct, ctinfo, data,
					&irr->rasAddress, 1);
		if (ret < 0)
			return -1;
	}

	if (set_sig_addr_hook) {
		ret = set_sig_addr_hook(pskb, ct, ctinfo, data,
					irr->callSignalAddress.item,
					irr->callSignalAddress.count);
		if (ret < 0)
			return -1;
	}

	return 0;
}

/****************************************************************************/
static int process_ras(struct sk_buff **pskb, struct ip_conntrack *ct,
		       enum ip_conntrack_info ctinfo,
		       unsigned char **data, RasMessage * ras)
{
	switch (ras->choice) {
	case eRasMessage_gatekeeperRequest:
		return process_grq(pskb, ct, ctinfo, data,
				   &ras->gatekeeperRequest);
	case eRasMessage_gatekeeperConfirm:
		return process_gcf(pskb, ct, ctinfo, data,
				   &ras->gatekeeperConfirm);
	case eRasMessage_registrationRequest:
		return process_rrq(pskb, ct, ctinfo, data,
				   &ras->registrationRequest);
	case eRasMessage_registrationConfirm:
		return process_rcf(pskb, ct, ctinfo, data,
				   &ras->registrationConfirm);
	case eRasMessage_unregistrationRequest:
		return process_urq(pskb, ct, ctinfo, data,
				   &ras->unregistrationRequest);
	case eRasMessage_admissionRequest:
		return process_arq(pskb, ct, ctinfo, data,
				   &ras->admissionRequest);
	case eRasMessage_admissionConfirm:
		return process_acf(pskb, ct, ctinfo, data,
				   &ras->admissionConfirm);
	case eRasMessage_locationRequest:
		return process_lrq(pskb, ct, ctinfo, data,
				   &ras->locationRequest);
	case eRasMessage_locationConfirm:
		return process_lcf(pskb, ct, ctinfo, data,
				   &ras->locationConfirm);
	case eRasMessage_infoRequestResponse:
		return process_irr(pskb, ct, ctinfo, data,
				   &ras->infoRequestResponse);
	default:
		DEBUGP("ip_ct_ras: RAS message %d\n", ras->choice);
		break;
	}

	return 0;
}

/****************************************************************************/
static int ras_help(struct sk_buff **pskb, struct ip_conntrack *ct,
		    enum ip_conntrack_info ctinfo)
{
	static RasMessage ras;
	unsigned char *data;
	int datalen = 0;
	int ret;

	DEBUGP("ip_ct_ras: skblen = %u\n", (*pskb)->len);

	LOCK_BH(&ip_h323_lock);

	/* Get UDP data */
	data = get_udp_data(pskb, &datalen);
	if (data == NULL)
		goto accept;
	DEBUGP("ip_ct_ras: RAS message %u.%u.%u.%u->%u.%u.%u.%u, len=%d\n",
	       NIPQUAD((*pskb)->nh.iph->saddr),
	       NIPQUAD((*pskb)->nh.iph->daddr), datalen);

	/* Decode RAS message */
	ret = DecodeRasMessage(data, datalen, &ras);
	if (ret < 0) {
		if (net_ratelimit())
			printk("ip_ct_ras: decoding error: %s\n",
			       ret == H323_ERROR_BOUND ?
			       "out of bound" : "out of range");
		goto accept;
	}

	/* Process RAS message */
	if (process_ras(pskb, ct, ctinfo, &data, &ras) < 0)
		goto drop;

      accept:
	UNLOCK_BH(&ip_h323_lock);
	return NF_ACCEPT;

      drop:
	UNLOCK_BH(&ip_h323_lock);
	if (net_ratelimit())
		printk("ip_ct_ras: packet dropped\n");
	return NF_DROP;
}

/****************************************************************************/
static struct ip_conntrack_helper ip_conntrack_helper_ras = {
	.name = "RAS",
	.flags = IP_CT_HELPER_F_REUSE_EXPECT,
	.me = THIS_MODULE,
	.max_expected = 0,
	.timeout = 240,
	.tuple = {.src = {.u = {.udp = {.port = __constant_htons(RAS_PORT)}}},
		  .dst = {.protonum = IPPROTO_UDP}},
	.mask = {.src = {.u = {.udp = {.port = 0xFFFE}}},
		 .dst = {.protonum = 0xFF}},
	.help2 = ras_help,
};

/****************************************************************************/
static int ip_conntrack_ras_expect(struct ip_conntrack *new)
{
	WRITE_LOCK(&ip_conntrack_lock);
	new->helper = &ip_conntrack_helper_ras;
	WRITE_UNLOCK(&ip_conntrack_lock);
	return NF_ACCEPT;
}

/****************************************************************************/
/* Not __exit - called from init() */
static void fini(void)
{
	ip_conntrack_helper_unregister(&ip_conntrack_helper_ras);
	ip_conntrack_helper_unregister(&ip_conntrack_helper_q931);
	kfree(h323_buffer);
	printk("ip_ct_h323: fini\n");
}

/****************************************************************************/
static int __init init(void)
{
	int ret;
	char *p;

	h323_buffer = kmalloc(65536, GFP_KERNEL);
	if (!h323_buffer)
		return -ENOMEM;
	if ((ret = ip_conntrack_helper_register(&ip_conntrack_helper_q931)) ||
	    (ret = ip_conntrack_helper_register(&ip_conntrack_helper_ras))) {
		fini();
		return ret;
	}

	if (internal_net) {
		if ((p = strchr(internal_net, '/')))
			*p++ = 0;
		if (isdigit(internal_net[0])) {
			internal_net_addr = in_aton(internal_net);
			if (p && isdigit(p[0]))
				internal_net_mask = in_aton(p);
			else
				internal_net_mask = 0xffffffff;
			internal_net_addr &= internal_net_mask;
		}
		DEBUGP("ip_ct_h323: internal_net = %u.%u.%u.%u/%u.%u.%u.%u\n",
		       NIPQUAD(internal_net_addr),
		       NIPQUAD(internal_net_mask));
	}

	printk("ip_ct_h323: init success\n");
	return 0;
}

/****************************************************************************/
module_init(init);
module_exit(fini);

EXPORT_SYMBOL_GPL(get_h225_addr);
EXPORT_SYMBOL_GPL(ip_conntrack_h245_expect);
EXPORT_SYMBOL_GPL(ip_conntrack_q931_expect);
EXPORT_SYMBOL_GPL(set_h225_addr_hook);
EXPORT_SYMBOL_GPL(set_sig_addr_hook);
EXPORT_SYMBOL_GPL(set_ras_addr_hook);
EXPORT_SYMBOL_GPL(nat_rtp_rtcp_hook);
EXPORT_SYMBOL_GPL(nat_t120_hook);
EXPORT_SYMBOL_GPL(nat_h245_hook);
EXPORT_SYMBOL_GPL(nat_callforwarding_hook);
EXPORT_SYMBOL_GPL(nat_q931_hook);

MODULE_AUTHOR("Jing Min Zhao <zhaojingmin@users.sourceforge.net>");
MODULE_DESCRIPTION("H.323 connection tracking helper");
MODULE_LICENSE("GPL");

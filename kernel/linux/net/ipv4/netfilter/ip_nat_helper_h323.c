/*
 * H.323 extension for NAT alteration.
 *
 * Copyright (c) 2006 Jing Min Zhao <zhaojingmin@users.sourceforge.net>
 *
 * This source code is licensed under General Public License version 2.
 *
 * Based on the 'brute force' H.323 NAT module by
 * Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 * Changes:
 *	2006-02-01 - initial version 0.1
 *
 *	2006-02-20 - version 0.2
 *	  1. Changed source format to follow kernel conventions
 *	  2. Deleted some unnecessary structures
 *	  3. Minor fixes
 *
 * 	2006-03-10 - version 0.3
 * 	  1. Added support for multiple TPKTs in one packet (suggested by
 * 	     Patrick McHardy)
 * 	  2. Added support for non-linear skb (based on Patrick McHardy's patch)
 * 	  3. Eliminated unnecessary return code
 *
 * 	2006-03-15 - version 0.4
 * 	  1. Added support for T.120 channels
 * 	  2. Added parameter gkrouted_only (suggested by Patrick McHardy)
 *
 * 	2006-03-25 - version 0.7
 * 	  1. Made get_h245_addr static (suggested by Adrian Bunk)
 * 	  2. Move some extern function prototypes to ip_conntrack_h323.h
 * 	     (suggested by Adrian Bunk)
 *
 * 	2006-04-05 - version 0.8
 * 	  1. Removed some trailing spaces
 *
 * 	2006-04-20 - version 0.9
 * 	  1. Added support for Call Forwarding
 *
 * 	2006-04-28 - version 1.0a
 * 	  1. Back-ported to 2.6.8
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_nat_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_h323.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif


/****************************************************************************/
static int ip_nat_follow_master(struct ip_conntrack *ct)
{
	struct ip_conntrack_expect *exp = ct->master;
	struct ip_nat_multi_range mr;
	unsigned int ret;

	/* Change src to where master sends to */
	mr.rangesize = 1;
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS;
	mr.range[0].min_ip = mr.range[0].max_ip
	    = exp->expectant->tuplehash[!exp->dir].tuple.dst.ip;

	/* hook doesn't matter, but it has to do source manip */
	ret = ip_nat_setup_info(ct, &mr, NF_IP_POST_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;

	/* For DST manip, map port here to where it's expected. */
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS |
	    IP_NAT_RANGE_PROTO_SPECIFIED;
	mr.range[0].min = mr.range[0].max = exp->saved_addr.u;
	mr.range[0].min_ip = mr.range[0].max_ip
	    = exp->expectant->tuplehash[!exp->dir].tuple.src.ip;
	/* hook doesn't matter, but it has to do destination manip */
	return ip_nat_setup_info(ct, &mr, NF_IP_PRE_ROUTING);
}

/****************************************************************************/
static int set_addr(struct sk_buff **pskb,
		    unsigned char **data, int dataoff,
		    unsigned int addroff, u_int32_t ip, u_int16_t port)
{
	enum ip_conntrack_info ctinfo;
	struct ip_conntrack *ct = ip_conntrack_get(*pskb, &ctinfo);
	struct {
		u_int32_t ip;
		u_int16_t port;
	} __attribute__ ((__packed__)) buf;

	buf.ip = ip;
	buf.port = htons(port);
	addroff += dataoff;

	if ((*pskb)->nh.iph->protocol == IPPROTO_TCP) {
		if (!ip_nat_mangle_tcp_packet(pskb, ct, ctinfo,
					      addroff, sizeof(buf),
					      (char *) &buf, sizeof(buf))) {
			if (net_ratelimit())
				printk("ip_nat_h323: ip_nat_mangle_tcp_packet"
				       " error\n");
			return -1;
		}
	} else {
		if (!ip_nat_mangle_udp_packet(pskb, ct, ctinfo,
					      addroff, sizeof(buf),
					      (char *) &buf, sizeof(buf))) {
			if (net_ratelimit())
				printk("ip_nat_h323: ip_nat_mangle_udp_packet"
				       " error\n");
			return -1;
		}
	}

	return 0;
}

/****************************************************************************/
static int set_h225_addr(struct sk_buff **pskb,
			 unsigned char **data, int dataoff,
			 TransportAddress * addr,
			 u_int32_t ip, u_int16_t port)
{
	return set_addr(pskb, data, dataoff, addr->ipAddress.ip, ip, port);
}

/****************************************************************************/
static int set_h245_addr(struct sk_buff **pskb,
			 unsigned char **data, int dataoff,
			 H245_TransportAddress * addr,
			 u_int32_t ip, u_int16_t port)
{
	return set_addr(pskb, data, dataoff,
			addr->unicastAddress.iPAddress.network, ip, port);
}

/****************************************************************************/
static int set_sig_addr(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo,
			unsigned char **data,
			TransportAddress * addr, int count)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	int dir = CTINFO2DIR(ctinfo);
	u_int16_t port;
	u_int32_t ip;
	int i;

	for (i = 0; i < count; i++) {
		if (get_h225_addr(*data, &addr[i], &ip, &port)) {
			if (ip == ct->tuplehash[dir].tuple.src.ip &&
			    port == info->sig_port[dir]) {
				/* GW->GK */

				/* Fix for Gnomemeeting */
				if (i > 0 &&
				    get_h225_addr(*data, &addr[0],
						  &ip, &port) &&
				    (ntohl(ip) & 0xff000000) == 0x7f000000)
					i = 0;

				DEBUGP
				    ("ip_nat_ras: set signal address "
				     "%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu\n",
				     NIPQUAD(ip), port,
				     NIPQUAD(ct->tuplehash[!dir].tuple.dst.
					     ip), info->sig_port[!dir]);
				return set_h225_addr(pskb, data, 0, &addr[i],
						     ct->tuplehash[!dir].
						     tuple.dst.ip,
						     info->sig_port[!dir]);
			} else if (ip == ct->tuplehash[dir].tuple.dst.ip &&
				   port == info->sig_port[dir]) {
				/* GK->GW */
				DEBUGP
				    ("ip_nat_ras: set signal address "
				     "%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu\n",
				     NIPQUAD(ip), port,
				     NIPQUAD(ct->tuplehash[!dir].tuple.src.
					     ip), info->sig_port[!dir]);
				return set_h225_addr(pskb, data, 0, &addr[i],
						     ct->tuplehash[!dir].
						     tuple.src.ip,
						     info->sig_port[!dir]);
			}
		}
	}

	return 0;
}

/****************************************************************************/
static int set_ras_addr(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo,
			unsigned char **data,
			TransportAddress * addr, int count)
{
	int dir = CTINFO2DIR(ctinfo);
	int i;
	u_int32_t ip;
	u_int16_t port;

	for (i = 0; i < count; i++) {
		if (get_h225_addr(*data, &addr[i], &ip, &port) &&
		    ip == ct->tuplehash[dir].tuple.src.ip &&
		    port == ntohs(ct->tuplehash[dir].tuple.src.u.udp.port)) {
			DEBUGP("ip_nat_ras: set rasAddress "
			       "%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu\n",
			       NIPQUAD(ip), port,
			       NIPQUAD(ct->tuplehash[!dir].tuple.dst.ip),
			       ntohs(ct->tuplehash[!dir].tuple.dst.u.udp.
				     port));
			return set_h225_addr(pskb, data, 0, &addr[i],
					     ct->tuplehash[!dir].tuple.dst.ip,
					     ntohs(ct->tuplehash[!dir].tuple.
						   dst.u.udp.port));
		}
	}

	return 0;
}

/****************************************************************************/
static struct ip_conntrack_expect *find_old_expect(struct ip_conntrack_expect
						   *exp,
						   struct ip_conntrack *ct)
{
	struct list_head *exp_entry, *next;
	struct ip_conntrack_expect *old_exp;

	READ_LOCK(&ip_conntrack_lock);
	READ_LOCK(&ip_conntrack_expect_tuple_lock);
	list_for_each_safe(exp_entry, next, &ct->sibling_list) {
		old_exp = list_entry(exp_entry, struct ip_conntrack_expect,
				     expected_list);
		if (old_exp->sibling)
			continue;
		if (old_exp->dir == exp->dir &&
		    old_exp->saved_addr.ip == exp->saved_addr.ip &&
		    old_exp->saved_addr.u.all == exp->saved_addr.u.all &&
		    old_exp->tuple.dst.protonum == exp->tuple.dst.protonum) {
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
static int nat_rtp_rtcp(struct sk_buff **pskb, struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo,
			unsigned char **data, int dataoff,
			H245_TransportAddress * addr,
			u_int16_t port, u_int16_t rtp_port,
			struct ip_conntrack_expect *rtp_exp,
			struct ip_conntrack_expect *rtcp_exp)
{
	struct ip_conntrack_expect *old_exp;
	int dir = CTINFO2DIR(ctinfo);
	u_int16_t nated_port = 0;

	/* Set expectations for NAT */
	rtp_exp->saved_addr.u.udp.port = rtp_exp->tuple.dst.u.udp.port;
	rtp_exp->saved_addr.ip = rtp_exp->tuple.dst.ip;
	rtp_exp->expectfn = ip_nat_follow_master;
	rtp_exp->dir = !dir;
	rtcp_exp->saved_addr.u.udp.port = rtcp_exp->tuple.dst.u.udp.port;
	rtcp_exp->saved_addr.ip = rtcp_exp->tuple.dst.ip;
	rtcp_exp->expectfn = ip_nat_follow_master;
	rtcp_exp->dir = !dir;

	/* Find existing expect */
	if ((old_exp = find_old_expect(rtp_exp, ct))) {
		rtp_exp->tuple.dst.u.udp.port = old_exp->tuple.dst.u.udp.port;
		nated_port = ntohs(old_exp->tuple.dst.u.udp.port);
		ip_conntrack_expect_put(old_exp);
		rtcp_exp->tuple.dst.u.udp.port = htons(nated_port + 1);
		goto success;
	}

	/* Try to get same port: if not, try to change it. */
	for (nated_port = rtp_port; nated_port; nated_port += 2) {
		rtp_exp->tuple.dst.u.udp.port = htons(nated_port);
		old_exp = ip_conntrack_expect_find_get(&rtp_exp->tuple);
		if (old_exp) {
			ip_conntrack_expect_put(old_exp);
			continue;
		}
		rtcp_exp->tuple.dst.u.udp.port = htons(nated_port + 1);
		old_exp = ip_conntrack_expect_find_get(&rtcp_exp->tuple);
		if (!old_exp)
			goto success;
		ip_conntrack_expect_put(old_exp);
	}

	if (net_ratelimit())
		printk("ip_nat_h323: out of RTP/RTCP ports\n");

	kfree(rtp_exp);
	kfree(rtcp_exp);

	return -1;

      success:
	DEBUGP("ip_nat_h323: expect RTP/RTCP "
	       "%u.%u.%u.%u->%u.%u.%u.%u:%hu/%hu\n",
	       NIPQUAD(rtp_exp->tuple.src.ip), NIPQUAD(rtp_exp->tuple.dst.ip),
	       nated_port, nated_port + 1);
	ip_conntrack_expect_related(rtp_exp, ct);
	ip_conntrack_expect_related(rtcp_exp, ct);

	DEBUGP("ip_nat_h323: set_h245_addr dataoff %d\n", dataoff);
	set_h245_addr(pskb, data, dataoff, addr,
		      ct->tuplehash[!dir].tuple.dst.ip,
		      (port & 1) ? nated_port + 1 : nated_port);
	return 0;
}

/****************************************************************************/
static int nat_t120(struct sk_buff **pskb, struct ip_conntrack *ct,
		    enum ip_conntrack_info ctinfo,
		    unsigned char **data, int dataoff,
		    H245_TransportAddress * addr, u_int16_t port,
		    struct ip_conntrack_expect *exp)
{
	struct ip_conntrack_expect *old_exp;
	int dir = CTINFO2DIR(ctinfo);
	u_int16_t nated_port;

	/* Set expectations for NAT */
	exp->saved_addr.u.tcp.port = exp->tuple.dst.u.tcp.port;
	exp->saved_addr.ip = exp->tuple.dst.ip;
	exp->expectfn = ip_nat_follow_master;
	exp->dir = !dir;

	/* Find existing expect */
	if ((old_exp = find_old_expect(exp, ct))) {
		exp->tuple.dst.u.tcp.port = old_exp->tuple.dst.u.tcp.port;
		nated_port = ntohs(old_exp->tuple.dst.u.tcp.port);
		ip_conntrack_expect_put(old_exp);
		goto success;
	}

	/* Try to get same port: if not, try to change it. */
	for (nated_port = port; nated_port != 0; nated_port++) {
		exp->tuple.dst.u.tcp.port = htons(nated_port);
		old_exp = ip_conntrack_expect_find_get(&exp->tuple);
		if (!old_exp)
			goto success;
		ip_conntrack_expect_put(old_exp);
	}

	if (net_ratelimit())
		printk("ip_nat_h323: out of T.120 ports\n");

	kfree(exp);

	return -1;

      success:
	DEBUGP("ip_nat_h323: expect T.120 %u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       nated_port);
	ip_conntrack_expect_related(exp, ct);
	set_h245_addr(pskb, data, dataoff, addr,
		      ct->tuplehash[!dir].tuple.dst.ip, nated_port);
	return 0;
}

/****************************************************************************
 * This conntrack expect function replaces ip_conntrack_h245_expect()
 * which was set by ip_conntrack_helper_h323.c. It calls both
 * ip_nat_follow_master() and ip_conntrack_h245_expect()
 ****************************************************************************/
static int ip_nat_h245_expect(struct ip_conntrack *new)
{
#ifdef CONFIG_IP_NF_H323_SIGNAL_PROXY_MODULE
	struct ip_conntrack_expect *this = new->master;
	struct ip_nat_multi_range mr;
	unsigned int ret;

	/* Change src to where master sends to */
	mr.rangesize = 1;
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS;
	mr.range[0].min_ip = mr.range[0].max_ip =
	    new->tuplehash[!this->dir].tuple.src.ip;

	/* hook doesn't matter, but it has to do source manip */
	ret = ip_nat_setup_info(new, &mr, NF_IP_POST_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;

	/* For DST manip, map port here to where it's expected. */
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS |
	    IP_NAT_RANGE_PROTO_SPECIFIED;
	mr.range[0].min = mr.range[0].max = this->saved_addr.u;
	mr.range[0].min_ip = mr.range[0].max_ip = this->saved_addr.ip;

	/* hook doesn't matter, but it has to do destination manip */
	ret = ip_nat_setup_info(new, &mr, NF_IP_PRE_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;
#else
	ip_nat_follow_master(new);
#endif
	return ip_conntrack_h245_expect(new);
}

/****************************************************************************/
static int nat_h245(struct sk_buff **pskb, struct ip_conntrack *ct,
		    enum ip_conntrack_info ctinfo,
		    unsigned char **data, int dataoff,
		    TransportAddress * addr, u_int16_t port,
		    struct ip_conntrack_expect *exp)
{
	struct ip_conntrack_expect *old_exp;
	int dir = CTINFO2DIR(ctinfo);
	u_int16_t nated_port;

	/* Set expectations for NAT */
	exp->saved_addr.u.tcp.port = exp->tuple.dst.u.tcp.port;
	exp->saved_addr.ip = exp->tuple.dst.ip;
#ifdef CONFIG_IP_NF_H323_SIGNAL_PROXY_MODULE
	exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
#endif
	exp->expectfn = ip_nat_h245_expect;
	exp->dir = !dir;

	/* Find existing expect */
	if ((old_exp = find_old_expect(exp, ct))) {
		exp->tuple.dst.u.tcp.port = old_exp->tuple.dst.u.tcp.port;
		nated_port = ntohs(old_exp->tuple.dst.u.tcp.port);
		ip_conntrack_expect_put(old_exp);
		goto success;
	}

	/* Try to get same port: if not, try to change it. */
	for (nated_port = port; nated_port != 0; nated_port++) {
		exp->tuple.dst.u.tcp.port = htons(nated_port);
		old_exp = ip_conntrack_expect_find_get(&exp->tuple);
		if (!old_exp)
			goto success;
		ip_conntrack_expect_put(old_exp);
	}

	if (net_ratelimit())
		printk("ip_nat_q931: out of H.245 ports\n");

	kfree(exp);

	return -1;

      success:
	DEBUGP("ip_nat_h323: expect H.245 %u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       nated_port);
	ip_conntrack_expect_related(exp, ct);
	set_h225_addr(pskb, data, dataoff, addr,
		      ct->tuplehash[!dir].tuple.dst.ip, nated_port);

	return 0;
}

/****************************************************************************
 * This conntrack expect function replaces ip_conntrack_q931_expect()
 * which was set by ip_conntrack_helper_h323.c.
 ****************************************************************************/
static int ip_nat_q931_expect(struct ip_conntrack *new)
{
	struct ip_conntrack_expect *this = new->master;
	struct ip_nat_multi_range mr;
	unsigned int ret;

	DEBUGP("Leon ip_nat_q931_expect: step 1\n");
	
	if (this->tuple.src.ip != 0) {	/* Only accept calls from GK */
		ip_nat_follow_master(new);
		goto out;
	}

	/* Change src to where master sends to */
	mr.rangesize = 1;
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS;
	mr.range[0].min_ip = mr.range[0].max_ip =
	    new->tuplehash[!this->dir].tuple.src.ip;

	/* hook doesn't matter, but it has to do source manip */
	ret = ip_nat_setup_info(new, &mr, NF_IP_POST_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;

	/* For DST manip, map port here to where it's expected. */
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS |
	    IP_NAT_RANGE_PROTO_SPECIFIED;
	mr.range[0].min = mr.range[0].max = this->saved_addr.u;
	mr.range[0].min_ip = mr.range[0].max_ip =
	    this->expectant->tuplehash[!this->dir].tuple.src.ip;

	/* hook doesn't matter, but it has to do destination manip */
	ret = ip_nat_setup_info(new, &mr, NF_IP_PRE_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;

      out:
	return ip_conntrack_q931_expect(new);
}

/****************************************************************************/
static int nat_q931(struct sk_buff **pskb, struct ip_conntrack *ct,
		    enum ip_conntrack_info ctinfo,
		    unsigned char **data, TransportAddress * addr, int idx,
		    u_int16_t port, struct ip_conntrack_expect *exp)
{
	struct ip_ct_h323_master *info = &ct->help.ct_h323_info;
	struct ip_conntrack_expect *old_exp;
	int dir = CTINFO2DIR(ctinfo);
	u_int16_t nated_port;
	u_int32_t ip;

	/* Set expectations for NAT */
	exp->saved_addr.u.tcp.port = exp->tuple.dst.u.tcp.port;
	exp->saved_addr.ip = exp->tuple.dst.ip;
	exp->expectfn = ip_nat_q931_expect;
	exp->dir = !dir;

	DEBUGP("nat_q931: expect Q.931 %u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       nated_port);

	/* Find existing expect */
	if ((old_exp = find_old_expect(exp, ct))) {
		exp->tuple.dst.u.tcp.port = old_exp->tuple.dst.u.tcp.port;
		nated_port = ntohs(old_exp->tuple.dst.u.tcp.port);
		ip_conntrack_expect_put(old_exp);
		goto success;
	}

	/* Try to get same port: if not, try to change it. */
	for (nated_port = port; nated_port != 0; nated_port++) {
		exp->tuple.dst.u.tcp.port = htons(nated_port);
		old_exp = ip_conntrack_expect_find_get(&exp->tuple);
		if (!old_exp)
			goto success;
		ip_conntrack_expect_put(old_exp);
	}

	if (net_ratelimit())
		printk("nat_q931: out of Q.931 ports\n");

	kfree(exp);

	return -1;

      success:
	DEBUGP("nat_q931: expect Q.931 %u.%u.%u.%u->%u.%u.%u.%u:%hu success\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       nated_port);
	ip_conntrack_expect_related(exp, ct);
	set_h225_addr(pskb, data, 0, &addr[idx],
		      ct->tuplehash[!dir].tuple.dst.ip, nated_port);

	/* Save ports */
	info->sig_port[dir] = port;
	info->sig_port[!dir] = nated_port;

	/* Fix for Gnomemeeting */
	if (idx > 0 && get_h225_addr(*data, &addr[0], &ip, &port) &&
	    (ntohl(ip) & 0xff000000) == 0x7f000000)
		set_h225_addr_hook(pskb, data, 0, &addr[0],
				   ct->tuplehash[!dir].tuple.dst.ip,
				   nated_port);
	return 0;
}

/****************************************************************************/
static int ip_nat_callforwarding_expect(struct ip_conntrack *new)
{
	struct ip_conntrack_expect *this = new->master;
	struct ip_nat_multi_range mr;
	unsigned int ret;

	/* Change src to where master sends to */
	mr.rangesize = 1;
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS;
	mr.range[0].min_ip = mr.range[0].max_ip =
	    new->tuplehash[!this->dir].tuple.src.ip;

	/* hook doesn't matter, but it has to do source manip */
	ret = ip_nat_setup_info(new, &mr, NF_IP_POST_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;

	/* For DST manip, map port here to where it's expected. */
	mr.range[0].flags = IP_NAT_RANGE_MAP_IPS |
	    IP_NAT_RANGE_PROTO_SPECIFIED;
	mr.range[0].min = mr.range[0].max = this->saved_addr.u;
	mr.range[0].min_ip = mr.range[0].max_ip = this->saved_addr.ip;

	/* hook doesn't matter, but it has to do destination manip */
	ret = ip_nat_setup_info(new, &mr, NF_IP_PRE_ROUTING);
	if (ret != NF_ACCEPT)
		return ret;

	return ip_conntrack_q931_expect(new);
}

/****************************************************************************/
static int nat_callforwarding(struct sk_buff **pskb, struct ip_conntrack *ct,
			      enum ip_conntrack_info ctinfo,
			      unsigned char **data, int dataoff,
			      TransportAddress * addr, u_int16_t port,
			      struct ip_conntrack_expect *exp)
{
	struct ip_conntrack_expect *old_exp;
	int dir = CTINFO2DIR(ctinfo);
	u_int16_t nated_port;

	/* Set expectations for NAT */
	exp->saved_addr.ip = exp->tuple.dst.ip;
	exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
	exp->saved_addr.u.tcp.port = exp->tuple.dst.u.tcp.port;
	exp->expectfn = ip_nat_callforwarding_expect;
	exp->dir = !dir;

	/* Try to get same port: if not, try to change it. */
	for (nated_port = port; nated_port != 0; nated_port++) {
		exp->tuple.dst.u.tcp.port = htons(nated_port);
		old_exp = ip_conntrack_expect_find_get(&exp->tuple);
		if (!old_exp)
			goto success;
		ip_conntrack_expect_put(old_exp);
	}

	if (net_ratelimit())
		printk("ip_nat_q931: out of Call Forwarding ports\n");

	kfree(exp);

	return -1;

      success:
	DEBUGP("ip_nat_q931: expect Call Forwarding "
	       "%u.%u.%u.%u->%u.%u.%u.%u:%hu\n",
	       NIPQUAD(exp->tuple.src.ip), NIPQUAD(exp->tuple.dst.ip),
	       nated_port);
	ip_conntrack_expect_related(exp, ct);
	set_h225_addr(pskb, data, dataoff, addr,
		      ct->tuplehash[!dir].tuple.dst.ip, nated_port);

	return 0;
}

/****************************************************************************/
static int __init init(void)
{
	BUG_ON(set_h225_addr_hook != NULL);
	BUG_ON(set_sig_addr_hook != NULL);
	BUG_ON(set_ras_addr_hook != NULL);
	BUG_ON(nat_rtp_rtcp_hook != NULL);
	BUG_ON(nat_t120_hook != NULL);
	BUG_ON(nat_h245_hook != NULL);
	BUG_ON(nat_callforwarding_hook != NULL);
	BUG_ON(nat_q931_hook != NULL);

	set_h225_addr_hook = set_h225_addr;
	set_sig_addr_hook = set_sig_addr;
	set_ras_addr_hook = set_ras_addr;
	nat_rtp_rtcp_hook = nat_rtp_rtcp;
	nat_t120_hook = nat_t120;
	nat_h245_hook = nat_h245;
	nat_callforwarding_hook = nat_callforwarding;
	nat_q931_hook = nat_q931;

	printk("ip_nat_h323: init success\n");
	return 0;
}

/****************************************************************************/
static void __exit fini(void)
{
	set_h225_addr_hook = NULL;
	set_sig_addr_hook = NULL;
	set_ras_addr_hook = NULL;
	nat_rtp_rtcp_hook = NULL;
	nat_t120_hook = NULL;
	nat_h245_hook = NULL;
	nat_callforwarding_hook = NULL;
	nat_q931_hook = NULL;
	synchronize_net();
}

/****************************************************************************/
module_init(init);
module_exit(fini);

MODULE_AUTHOR("Jing Min Zhao <zhaojingmin@users.sourceforge.net>");
MODULE_DESCRIPTION("H.323 NAT helper");
MODULE_LICENSE("GPL");

/*
 * H.323 'brute force' extension for H.323 connection tracking.
 * Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 * (c) 2005 Max Kellermann <max@duempel.org>
 *
 * Based on ip_masq_h323.c for 2.2 kernels from CoRiTel, Sofia project.
 * (http://www.coritel.it/projects/sofia/nat/)
 * Uses Sampsa Ranta's excellent idea on using expectfn to 'bind'
 * the unregistered helpers to the conntrack entries.
 */


#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <net/tcp.h>

#include <linux/netfilter_ipv4/lockhelp.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_conntrack_h323.h>

MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>");
MODULE_DESCRIPTION("H.323 'brute force' connection tracking module");
MODULE_LICENSE("GPL");

/* This is slow, but it's simple. --RR */
static char h323_buffer[65536];
static DECLARE_LOCK(h323_buffer_lock);

DECLARE_LOCK(ip_h323_lock);
struct module *ip_conntrack_h323 = THIS_MODULE;

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

/* FIXME: This should be in userspace.  Later. */
static int h245_help(struct sk_buff *skb,
		     struct ip_conntrack *ct,
		     enum ip_conntrack_info ctinfo)
{
	struct iphdr *iph = skb->nh.iph;
	struct tcphdr _tcph, *tcph;
	unsigned char *data;
	unsigned char *data_limit;
	unsigned dataoff, datalen;
	int dir = CTINFO2DIR(ctinfo);
	struct ip_ct_h225_master *info = &ct->help.ct_h225_info;
	struct ip_conntrack_expect *exp;
	struct ip_ct_h225_expect *exp_info;
	u_int16_t data_port;
	u_int32_t data_ip;
	unsigned int i;
	int ret;

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		DEBUGP("ct_h245_help: Conntrackinfo = %u\n", ctinfo);
		return NF_ACCEPT;
	}

	tcph = skb_header_pointer(skb, skb->nh.iph->ihl*4,
				  sizeof(_tcph), &_tcph);
	if (tcph == NULL)
		return NF_ACCEPT;

	DEBUGP("ct_h245_help: help entered %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n",
		NIPQUAD(iph->saddr), ntohs(tcph->source),
		NIPQUAD(iph->daddr), ntohs(tcph->dest));

	dataoff = skb->nh.iph->ihl*4 + tcph->doff*4;
	/* No data? */
	if (dataoff >= skb->len) {
		DEBUGP("ct_h245_help: skblen = %u\n", skb->len);
		return NF_ACCEPT;
	}
	datalen = skb->len - dataoff;

	LOCK_BH(&h323_buffer_lock);
	data = skb_header_pointer(skb, dataoff,
				  datalen, h323_buffer);
	BUG_ON(data == NULL);

	data_limit = data + datalen - 6;
	/* bytes: 0123   45
	          ipadrr port */
	for (i = 0; data <= data_limit; data++, i++) {
		data_ip = *((u_int32_t *)data);
		if (data_ip == iph->saddr) {
			data_port = *((u_int16_t *)(data + 4));

			/* update the H.225 info */
			DEBUGP("ct_h245_help: new RTCP/RTP requested %u.%u.%u.%u:->%u.%u.%u.%u:%u\n",
				NIPQUAD(ct->tuplehash[!dir].tuple.src.ip),
				NIPQUAD(iph->saddr), ntohs(data_port));

			exp = ip_conntrack_expect_alloc();
			if (exp == NULL) {
				ret = NF_ACCEPT;
				goto out;
			}

			exp_info = &exp->help.exp_h225_info;

			LOCK_BH(&ip_h323_lock);
			info->is_h225 = H225_PORT + 1;
			exp_info->port = data_port;
			exp_info->dir = dir;
			exp_info->offset = i;

			exp->seq = ntohl(tcph->seq) + i;

			exp->tuple = ((struct ip_conntrack_tuple)
				{ { ct->tuplehash[!dir].tuple.src.ip,
				    { 0 } },
				  { data_ip,
				    { .tcp = { data_port } },
				    IPPROTO_UDP }});
			exp->mask = ((struct ip_conntrack_tuple)
				{ { 0xFFFFFFFF, { 0 } },
				  { 0xFFFFFFFF, { .tcp = { 0xFFFF } }, 0xFFFF }});

			exp->expectfn = NULL;

			/* Ignore failure; should only happen with NAT */
			ip_conntrack_expect_related(exp, ct);

			UNLOCK_BH(&ip_h323_lock);
		}
	}

	ret = NF_ACCEPT;
 out:
	UNLOCK_BH(&h323_buffer_lock);
	return ret;
}

/* H.245 helper is not registered! */
static struct ip_conntrack_helper h245 = {
	.name = "H.245",
	.flags = IP_CT_HELPER_F_REUSE_EXPECT,
	.max_expected = 8,
	.timeout = 1,
	.tuple = { .dst = { .protonum = IPPROTO_TCP } },
	.mask = { .src = { .u = { 0xFFFF } },
		  .dst = { .protonum = 0xFFFF } },
	.help = h245_help
};

static int h225_expect(struct ip_conntrack *ct)
{
	WRITE_LOCK(&ip_conntrack_lock);
	ct->helper = &h245;
	DEBUGP("h225_expect: helper for %p added\n", ct);
	WRITE_UNLOCK(&ip_conntrack_lock);

	return NF_ACCEPT;	/* unused */
}

/* FIXME: This should be in userspace.  Later. */
static int h225_help(struct sk_buff *skb,
		     struct ip_conntrack *ct,
		     enum ip_conntrack_info ctinfo)
{
	struct iphdr *iph = skb->nh.iph;
	struct tcphdr _tcph, *tcph;
	unsigned char *data;
	unsigned char *data_limit;
	unsigned dataoff, datalen;
	int dir = CTINFO2DIR(ctinfo);
	struct ip_ct_h225_master *info = &ct->help.ct_h225_info;
	struct ip_conntrack_expect *exp;
	struct ip_ct_h225_expect *exp_info;
	u_int16_t data_port;
	u_int32_t data_ip;
	unsigned int i;
	int ret;

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		DEBUGP("ct_h225_help: Conntrackinfo = %u\n", ctinfo);
		return NF_ACCEPT;
	}

	tcph = skb_header_pointer(skb, skb->nh.iph->ihl*4,
				  sizeof(_tcph), &_tcph);
	if (tcph == NULL)
		return NF_ACCEPT;

	DEBUGP("ct_h225_help: help entered %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n",
		NIPQUAD(iph->saddr), ntohs(tcph->source),
		NIPQUAD(iph->daddr), ntohs(tcph->dest));

	dataoff = skb->nh.iph->ihl*4 + tcph->doff*4;
	/* No data? */
	if (dataoff >= skb->len) {
		DEBUGP("ct_h225_help: skblen = %u\n", skb->len);
		return NF_ACCEPT;
	}
	datalen = skb->len - dataoff;

	LOCK_BH(&h323_buffer_lock);
	data = skb_header_pointer(skb, dataoff,
				  datalen, h323_buffer);
	BUG_ON(data == NULL);

	data_limit = data + datalen - 6;
	/* bytes: 0123   45
	          ipadrr port */
	for (i = 0; data <= data_limit; data++, i++) {
		data_ip = *((u_int32_t *)data);
		if (data_ip == iph->saddr) {
			data_port = *((u_int16_t *)(data + 4));
			if (data_port == tcph->source) {
				/* Signal address */
				DEBUGP("ct_h225_help: sourceCallSignalAddress from %u.%u.%u.%u\n",
					NIPQUAD(iph->saddr));
				/* Update the H.225 info so that NAT can mangle the address/port
				   even when we have no expected connection! */
#ifdef CONFIG_IP_NF_NAT_NEEDED
				LOCK_BH(&ip_h323_lock);
				info->dir = dir;
				info->seq[IP_CT_DIR_ORIGINAL] = ntohl(tcph->seq) + i;
				info->offset[IP_CT_DIR_ORIGINAL] = i;
				UNLOCK_BH(&ip_h323_lock);
#endif
			} else {
				/* update the H.225 info */
				exp = ip_conntrack_expect_alloc();
				if (exp == NULL) {
					ret = NF_ACCEPT;
					goto out;
				}

				exp_info = &exp->help.exp_h225_info;

				LOCK_BH(&ip_h323_lock);
				info->is_h225 = H225_PORT;
				exp_info->port = data_port;
				exp_info->dir = dir;
				exp_info->offset = i;

				exp->seq = ntohl(tcph->seq) + i;

				exp->tuple = ((struct ip_conntrack_tuple)
					{ { ct->tuplehash[!dir].tuple.src.ip,
					    { 0 } },
					  { data_ip,
					    { .tcp = { data_port } },
					    IPPROTO_TCP }});
				exp->mask = ((struct ip_conntrack_tuple)
					{ { 0xFFFFFFFF, { 0 } },
					  { 0xFFFFFFFF, { .tcp = { 0xFFFF } }, 0xFFFF }});

				exp->expectfn = h225_expect;

				/* Ignore failure */
				ip_conntrack_expect_related(exp, ct);

				DEBUGP("ct_h225_help: new H.245 requested %u.%u.%u.%u->%u.%u.%u.%u:%u\n",
					NIPQUAD(ct->tuplehash[!dir].tuple.src.ip),
					NIPQUAD(iph->saddr), ntohs(data_port));

				UNLOCK_BH(&ip_h323_lock);
			}
#ifdef CONFIG_IP_NF_NAT_NEEDED
		} else if (data_ip == iph->daddr) {
			data_port = *((u_int16_t *)(data + 4));
			if (data_port == tcph->dest) {
				/* Signal address */
				DEBUGP("ct_h225_help: destCallSignalAddress %u.%u.%u.%u\n",
					NIPQUAD(iph->daddr));
				/* Update the H.225 info so that NAT can mangle the address/port
				   even when we have no expected connection! */
				LOCK_BH(&ip_h323_lock);
				info->dir = dir;
				info->seq[IP_CT_DIR_REPLY] = ntohl(tcph->seq) + i;
				info->offset[IP_CT_DIR_REPLY] = i;
				UNLOCK_BH(&ip_h323_lock);
			}
#endif
		}
	}

	ret = NF_ACCEPT;
 out:
	UNLOCK_BH(&h323_buffer_lock);
	return ret;
}

static struct ip_conntrack_helper h225 = {
	.name = "H.225",
	.flags = IP_CT_HELPER_F_REUSE_EXPECT,
	.me = THIS_MODULE,
	.max_expected = 2,
	.timeout = 240,
	.tuple = { .src = { .ip = 0, 
		 	    .u = { .tcp = { .port =  
				    __constant_htons(H225_PORT) } } 
			  }, 
		   .dst = { .ip = 0, 
			    .u = { .all = 0 },
			    .protonum = IPPROTO_TCP
			  } 
		 },
	.mask = { .src = { .ip = 0, 
			   .u = { .tcp = { .port = 0xffff } } 
			 }, 
		  .dst = { .ip = 0, 
			   .u = { .all = 0 },
			   .protonum = 0xffff 
		 	 } 
		},
	.help = h225_help
};

static int __init init(void)
{
	printk("ip_conntrack_h323: init \n");
	return ip_conntrack_helper_register(&h225);
}

static void __exit fini(void)
{
	/* Unregister H.225 helper */
	ip_conntrack_helper_unregister(&h225);
}

EXPORT_SYMBOL(ip_h323_lock);

PROVIDES_CONNTRACK(h225);
PROVIDES_CONNTRACK(h245);
module_init(init);
module_exit(fini);

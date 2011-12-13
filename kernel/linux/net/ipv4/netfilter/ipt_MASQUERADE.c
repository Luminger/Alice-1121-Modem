/* Masquerade.  Simple mapping which alters range to a local IP address
   (depending on route). */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_tables.h>

/* BCM-NAT */
#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ip_conntrack_lock)
#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&ip_conntrack_lock)
#include <linux/netdevice.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_nat_helper.h>
#include <linux/netfilter_ipv4/listhelp.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables MASQUERADE target module");

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

/* Lock protects masq region inside conntrack */
static DECLARE_RWLOCK(masq_lock);

/* BCM-NAT */
static char nat_mode = '0';
static struct proc_dir_entry* nat_mode_proc;

/****************************************************************************/
static int 
bcm_nat_expect(struct ip_conntrack *ct)
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
static int
bcm_nat_help(struct sk_buff **pskb, struct ip_conntrack *ct,
	      enum ip_conntrack_info ctinfo)
{
	int dir = CTINFO2DIR(ctinfo);
	struct ip_conntrack_expect *exp;
	
	if (ctinfo == IP_CT_ESTABLISHED || dir != IP_CT_DIR_ORIGINAL)
		return NF_ACCEPT;

	DEBUGP("bcm_nat: help: packet[%d bytes] "
	       "%u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu, "
	       "reply: %u.%u.%u.%u:%hu->%u.%u.%u.%u:%hu\n",
	       (*pskb)->len,
	       NIPQUAD(ct->tuplehash[dir].tuple.src.ip),
	       ntohs(ct->tuplehash[dir].tuple.src.u.udp.port),
	       NIPQUAD(ct->tuplehash[dir].tuple.dst.ip),
	       ntohs(ct->tuplehash[dir].tuple.dst.u.udp.port),
	       NIPQUAD(ct->tuplehash[!dir].tuple.src.ip),
	       ntohs(ct->tuplehash[!dir].tuple.src.u.udp.port),
	       NIPQUAD(ct->tuplehash[!dir].tuple.dst.ip),
	       ntohs(ct->tuplehash[!dir].tuple.dst.u.udp.port));
	
	/* Create expect */
	if ((exp = ip_conntrack_expect_alloc()) == NULL)
		return NF_ACCEPT;

	exp->tuple.dst.ip = ct->tuplehash[!dir].tuple.dst.ip;
	exp->tuple.dst.u = ct->tuplehash[!dir].tuple.dst.u;
	exp->tuple.dst.protonum = ct->tuplehash[!dir].tuple.dst.protonum;
	exp->mask.dst.ip = 0xFFFFFFFF;
	exp->mask.dst.u.udp.port= 0xFFFF;
	exp->mask.dst.protonum = 0xFFFF;
	exp->expectfn = bcm_nat_expect;
	exp->dir = !dir;
	exp->flags = IP_CT_EXPECT_PERMANENT;
	exp->saved_addr.ip = ct->tuplehash[dir].tuple.src.ip;
	exp->saved_addr.u = ct->tuplehash[dir].tuple.src.u;

	/* Setup expect */
	ip_conntrack_expect_related(exp, ct);
	DEBUGP("bcm_nat: expect setup\n");

	return NF_ACCEPT;
}

/****************************************************************************/
static struct ip_conntrack_helper ip_conntrack_helper_bcm_nat = {
	.name = "BCM-NAT",
	.flags = IP_CT_HELPER_F_REUSE_EXPECT,
	.me = THIS_MODULE,
	.max_expected = 0,
	.timeout = 0,
	.help2 = bcm_nat_help,
};

/****************************************************************************/
static inline int
unhelp(struct ip_conntrack_tuple_hash * i,
       const struct ip_conntrack_helper * me)
{
	if (i->ctrack->helper == me) {
		/* Get rid of any expected. */
		ip_ct_remove_expectations(i->ctrack);
		/* And *then* set helper to NULL */
		i->ctrack->helper = NULL;
	}
	return 0;
}

/****************************************************************************/
void
unregister_bcm_nat_helper(void)
{
	unsigned int i;

	/* Need write lock here, to delete helper. */
	WRITE_LOCK(&ip_conntrack_lock);

	/* Get rid of expecteds, set helpers to NULL. */
	for (i = 0; i < ip_conntrack_htable_size; i++)
		LIST_FIND_W(&ip_conntrack_hash[i], unhelp,
			    struct ip_conntrack_tuple_hash *,
			    &ip_conntrack_helper_bcm_nat);
	
	WRITE_UNLOCK(&ip_conntrack_lock);

	/* Someone could be still looking at the helper in a bh. */
	synchronize_net();
}

/****************************************************************************/
static inline int
exp_cmp(const struct ip_conntrack_expect * exp, u_int32_t ip, u_int16_t port,
	u_int16_t proto)
{
	return exp->tuple.dst.ip == ip && exp->tuple.dst.u.udp.port == port &&
	       exp->tuple.dst.protonum == proto;
}

/****************************************************************************/
static inline int
exp_src_cmp(const struct ip_conntrack_expect * exp, 
	    const struct ip_conntrack_tuple * tp)
{
	return exp->saved_addr.ip == tp->src.ip &&
	       exp->saved_addr.u.udp.port == tp->src.u.udp.port &&
	       exp->tuple.dst.protonum == tp->dst.protonum;
}

/****************************************************************************/
static int
nat_mode_read(char* page, char** start, off_t off, int count, int* eof,
	      void * data)
{
	int len = 2;
	
	if (off == 0) {
		page[0] = nat_mode;
		page[1] = '\n';
	}

	if (len <= off + count)
		*eof = 1;
	*start = page + off;
	len -= off;
	if (len > count)
		len = count;
	if (len < 0)
		len = 0;
	return len;
}
		
/****************************************************************************/
static int
nat_mode_write(struct file * file, const char __user * buffer,
	       unsigned long count, void * data)
{
	char mode;
	
	if (copy_from_user(&mode, buffer, 1))
		return -EFAULT;
	
	if (mode != '0' && mode != '3')
		return -EINVAL;

	if (mode == '0' && mode != nat_mode)
		unregister_bcm_nat_helper();

	nat_mode = mode;
	DEBUGP("bcm_nat: nat_mode changed to %c\n", nat_mode);
	
	return count;
}

/****************************************************************************/
static int
register_proc_entries(void)
{
	nat_mode_proc = create_proc_entry("nat_mode", 0644, proc_net);

	if (!nat_mode_proc) {
		if (net_ratelimit())
			printk("bcm_nat: register nat_mode proc entry "
			       "failed\n");
		return 0;
	}

	nat_mode_proc->owner = THIS_MODULE;
	nat_mode_proc->read_proc = nat_mode_read;
	nat_mode_proc->write_proc = nat_mode_write;

	return 1;
}

/****************************************************************************/
static void
unregister_proc_entries(void)
{
	if (!nat_mode_proc)
		return;
	remove_proc_entry("nat_mode", proc_net);
}
/****************************************************************************/

/* FIXME: Multiple targets. --RR */
static int
masquerade_check(const char *tablename,
		 const struct ipt_entry *e,
		 void *targinfo,
		 unsigned int targinfosize,
		 unsigned int hook_mask)
{
	const struct ip_nat_multi_range *mr = targinfo;

	if (strcmp(tablename, "nat") != 0) {
		DEBUGP("masquerade_check: bad table `%s'.\n", tablename);
		return 0;
	}
	if (targinfosize != IPT_ALIGN(sizeof(*mr))) {
		DEBUGP("masquerade_check: size %u != %u.\n",
		       targinfosize, sizeof(*mr));
		return 0;
	}
	if (hook_mask & ~(1 << NF_IP_POST_ROUTING)) {
		DEBUGP("masquerade_check: bad hooks %x.\n", hook_mask);
		return 0;
	}
	if (mr->range[0].flags & IP_NAT_RANGE_MAP_IPS) {
		DEBUGP("masquerade_check: bad MAP_IPS.\n");
		return 0;
	}
	if (mr->rangesize != 1) {
		DEBUGP("masquerade_check: bad rangesize %u.\n", mr->rangesize);
		return 0;
	}
	return 1;
}

static unsigned int
masquerade_target(struct sk_buff **pskb,
		  const struct net_device *in,
		  const struct net_device *out,
		  unsigned int hooknum,
		  const void *targinfo,
		  void *userinfo)
{
	struct ip_conntrack *ct;
	enum ip_conntrack_info ctinfo;
	const struct ip_nat_multi_range *mr;
	struct ip_nat_multi_range newrange;
	u_int32_t newsrc;
	struct rtable *rt;

	IP_NF_ASSERT(hooknum == NF_IP_POST_ROUTING);

	/* FIXME: For the moment, don't do local packets, breaks
	   testsuite for 2.3.49 --RR */
	if ((*pskb)->sk)
		return NF_ACCEPT;

	ct = ip_conntrack_get(*pskb, &ctinfo);
	IP_NF_ASSERT(ct && (ctinfo == IP_CT_NEW
				  || ctinfo == IP_CT_RELATED));

	mr = targinfo;

	{
		struct flowi fl = { .nl_u = { .ip4_u =
					      { .daddr = (*pskb)->nh.iph->daddr,
						.tos = (RT_TOS((*pskb)->nh.iph->tos) |
							RTO_CONN),
#ifdef CONFIG_IP_ROUTE_FWMARK
						.fwmark = (*pskb)->nfmark
#endif
					      } } };
		if (ip_route_output_key(&rt, &fl) != 0) {
			/* Funky routing can do this. */
			if (net_ratelimit())
				printk("MASQUERADE:"
				       " No route: Rusty's brain broke!\n");
			return NF_DROP;
		}
		if (rt->u.dst.dev != out) {
			if (net_ratelimit())
				printk("MASQUERADE:"
				       " Route sent us somewhere else.\n");
			ip_rt_put(rt);
			return NF_DROP;
		}
	}

	newsrc = rt->rt_src;
	DEBUGP("newsrc = %u.%u.%u.%u\n", NIPQUAD(newsrc));
	ip_rt_put(rt);

	WRITE_LOCK(&masq_lock);
	ct->nat.masq_index = out->ifindex;
	WRITE_UNLOCK(&masq_lock);
	
	/* BCM-NAT */
	if (nat_mode != '0' &&
	    ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum == 
	    IPPROTO_UDP ) {
		unsigned int ret;
		u_int16_t minport, maxport;
		struct ip_conntrack_expect *exp;
		
		/* Choose port */
		READ_LOCK(&ip_conntrack_lock);
		READ_LOCK(&ip_conntrack_expect_tuple_lock);
		exp = LIST_FIND(&ip_conntrack_expect_list, exp_src_cmp,
				struct ip_conntrack_expect *,
				&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
		if (exp) {
			minport = maxport = exp->tuple.dst.u.udp.port;
			DEBUGP("bcm_nat: existing mapped port = %hu\n",
			       ntohs(minport));
		} else {
			u_int16_t newport, tmpport;
		
			minport = mr->range[0].min.udp.port == 0? 
				ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.
				u.udp.port : mr->range[0].min.udp.port;
			maxport = mr->range[0].max.udp.port == 0? 
				htons(65535) : mr->range[0].max.udp.port;

			for (newport = ntohs(minport),tmpport = ntohs(maxport); 
			     newport <= tmpport; newport++) {
				if (!LIST_FIND(&ip_conntrack_expect_list, 
					       exp_cmp,
					       struct ip_conntrack_expect *,
					       newsrc, htons(newport), ct->
					       tuplehash[IP_CT_DIR_ORIGINAL].
					       tuple.dst.protonum)) {
					DEBUGP("bcm_nat: new mapped port = "
					       "%hu\n", newport);
					minport = maxport = htons(newport);
					break;
				}
			}
		}
		READ_UNLOCK(&ip_conntrack_expect_tuple_lock);
		READ_UNLOCK(&ip_conntrack_lock);

		newrange = ((struct ip_nat_multi_range)
				{1, {{mr->range[0].flags | 
				IP_NAT_RANGE_MAP_IPS |
				IP_NAT_RANGE_PROTO_SPECIFIED,
				newsrc, newsrc,
				{.udp = {minport}}, {.udp = {maxport}}}}});
	
		/* Set ct helper */
		ret = ip_nat_setup_info(ct, &newrange, hooknum);
		if (ret == NF_ACCEPT) {
			ct->helper = &ip_conntrack_helper_bcm_nat;
			DEBUGP("bcm_nat: helper set\n");
		}
		return ret;
	}

	/* Transfer from original range. */
	newrange = ((struct ip_nat_multi_range)
		{ 1, { { mr->range[0].flags | IP_NAT_RANGE_MAP_IPS,
			 newsrc, newsrc,
			 mr->range[0].min, mr->range[0].max } } });

	/* Hand modified range to generic setup. */
	return ip_nat_setup_info(ct, &newrange, hooknum);
}

static inline int
device_cmp(const struct ip_conntrack *i, void *_ina)
{
	int ret = 0;
	struct in_ifaddr *ina = _ina;

	READ_LOCK(&masq_lock);
	/* If it's masquerading out this interface with a different address,
	   or we don't know the new address of this interface. */
	if (i->nat.masq_index == ina->ifa_dev->dev->ifindex
	    && i->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip != ina->ifa_address)
		ret = 1;
	READ_UNLOCK(&masq_lock);

	return ret;
}

static int masq_inet_event(struct notifier_block *this,
			   unsigned long event,
			   void *ptr)
{
	/* For some configurations, interfaces often come back with
	 * the same address.  If not, clean up old conntrack
	 * entries. */
	if (event == NETDEV_UP)
		ip_ct_selective_cleanup(device_cmp, ptr);

	return NOTIFY_DONE;
}

static struct notifier_block masq_inet_notifier = {
	.notifier_call	= masq_inet_event,
};

static struct ipt_target masquerade = {
	.name		= "MASQUERADE",
	.target		= masquerade_target,
	.checkentry	= masquerade_check,
	.me		= THIS_MODULE,
};

static int __init init(void)
{
	int ret;

	ret = ipt_register_target(&masquerade);

	if (ret == 0)
		/* Register IP address change reports */
		register_inetaddr_notifier(&masq_inet_notifier);
	
	/* BCM-NAT */	
	register_proc_entries();

	return ret;
}

static void __exit fini(void)
{
	/* BCM-NAT */	
	unregister_proc_entries();
	unregister_bcm_nat_helper();

	ipt_unregister_target(&masquerade);
	unregister_inetaddr_notifier(&masq_inet_notifier);	
}

module_init(init);
module_exit(fini);

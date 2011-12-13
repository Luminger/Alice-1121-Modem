/*
 * This is a module which is used for queueing IPv4 packets and
 * communicating with userspace via netlink.
 *
 * (C) 2000-2002 James Morris <jmorris@intercode.com.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 2000-03-27: Simplified code (thanks to Andi Kleen for clues).
 * 2000-05-20: Fixed notifier problems (following Miguel Freitas' report).
 * 2000-06-19: Fixed so nfmark is copied to metadata (reported by Sebastian 
 *             Zander).
 * 2000-08-01: Added Nick Williams' MAC support.
 * 2002-06-25: Code cleanup.
 *
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/notifier.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_core.h>
#include <linux/netfilter_ipv4/ip_queue.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netlink.h>
#include <linux/spinlock.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/security.h>
#include <net/sock.h>
#include <net/route.h>

#define IPQ_QMAX_DEFAULT 1024
#define IPQ_PROC_FS_NAME "ip_queue"
#define NET_IPQ_QMAX 2088
#define NET_IPQ_QMAX_NAME "ip_queue_maxlen"

struct ipq_rt_info {
	__u8 tos;
	__u32 daddr;
	__u32 saddr;
};

struct ipq_queue_entry {
	struct list_head list;
	struct nf_info *info;
	struct sk_buff *skb;
	struct ipq_rt_info rt_info;
};

typedef int (*ipq_cmpfn)(struct ipq_queue_entry *, unsigned long);

static unsigned char copy_mode = IPQ_COPY_NONE;
static unsigned int queue_maxlen = IPQ_QMAX_DEFAULT;
static rwlock_t queue_lock = RW_LOCK_UNLOCKED;
static int peer_pid;
static unsigned int copy_range;
static unsigned int queue_total;
static struct sock *ipqnl;
static LIST_HEAD(queue_list);
static DECLARE_MUTEX(ipqnl_sem);

/* DYNAHELPER */
#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

/* Dynahelper tracker status */
#define DH_STAT_UNLOADED 0
#define DH_STAT_LOADING 1
#define DH_STAT_LOADED 2

struct dh_tracker {
	struct list_head list;
	char proto[DYNAHELPER_MAXPROTONAMELEN + 1];
	unsigned long timeout;
	int stat;
	u_int32_t ct_refcount;
	u_int32_t nat_refcount;
	struct module *ct_module;
	struct module *nat_module;
	struct timer_list timer;
};

static rwlock_t dh_lock = RW_LOCK_UNLOCKED;
static LIST_HEAD(dh_trackers);
static int dh_pid;
static struct sock *dhnl;
static DECLARE_MUTEX(dhnl_sem);

/****************************************************************************/
static void dh_send_msg(struct dh_tracker *tracker, int type)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	struct ipq_packet_msg *pmsg;

	if (!dhnl)
		return;

	skb = alloc_skb(NLMSG_SPACE(sizeof(*pmsg)), GFP_KERNEL);
	if (!skb) {
		printk(KERN_ERR "ipt_DYNAHELPER: alloc_skb() error\n");
		return;
	}
	nlh = __nlmsg_put(skb, 0, 0, type, sizeof(*pmsg));
	pmsg = NLMSG_DATA(nlh);
	pmsg->mark = (unsigned long) tracker;
	strcpy(pmsg->indev_name, tracker->proto);
	netlink_unicast(dhnl, skb, dh_pid, MSG_DONTWAIT);
}

/****************************************************************************/
static inline void load_modules(struct dh_tracker *tracker)
{
	dh_send_msg(tracker, IPQM_DYNAHELPER_LOAD);
	DEBUGP("ipt_DYNAHELPER: load helper %s\n", tracker->proto);
}

/****************************************************************************/
static inline void unload_modules(struct dh_tracker *tracker)
{
	dh_send_msg(tracker, IPQM_DYNAHELPER_UNLOAD);
	DEBUGP("ipt_DYNAHELPER: unload helper %s\n", tracker->proto);
}

/****************************************************************************/
static void dh_timer_handler(unsigned long ul_tracker)
{
	struct dh_tracker *tracker = (void *) ul_tracker;

	DEBUGP("ipt_DYNAHELPER: helper %s timed out\n", tracker->proto);

	write_lock_bh(&dh_lock);

	unload_modules(tracker);
	tracker->stat = DH_STAT_UNLOADED;
	tracker->ct_module = NULL;
	tracker->nat_module = NULL;
	tracker->ct_refcount = 0;
	tracker->nat_refcount = 0;

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_start_timer(struct dh_tracker *tracker)
{
	if (!tracker->timeout)
		return;

	if (timer_pending(&tracker->timer))
		return;

	tracker->timer.expires = jiffies + tracker->timeout * HZ;
	add_timer(&tracker->timer);
	DEBUGP("ipt_DYNAHELPER: helper %s idle timer started\n",
	       tracker->proto);
}

/****************************************************************************/
static void dh_stop_timer(struct dh_tracker *tracker)
{
	if (timer_pending(&tracker->timer)) {
		del_timer(&tracker->timer);
		DEBUGP("ipt_DYNAHELPER: helper %s idle timer stopped\n",
		       tracker->proto);
	}
}

/****************************************************************************/
static struct dh_tracker *dh_create_tracker(struct ipt_dynahelper_info *info)
{
	struct dh_tracker *tracker;

	tracker = kmalloc(sizeof(struct dh_tracker), GFP_ATOMIC);
	if (!tracker) {
		if (net_ratelimit())
			printk(KERN_ERR "ipt_DYNAHELPER: OOM\n");
		return NULL;
	}
	memset(tracker, 0, sizeof(struct dh_tracker));
	strcpy(tracker->proto, info->proto);
	tracker->timeout = info->timeout;
	tracker->stat = DH_STAT_UNLOADED;
	init_timer(&tracker->timer);
	tracker->timer.data = (unsigned long) tracker;
	tracker->timer.function = dh_timer_handler;
	list_add(&tracker->list, &dh_trackers);
	DEBUGP("ipt_DYNAHELPER: tracker for helper %s created\n",
	       tracker->proto);

	return tracker;
}

/****************************************************************************/
static void dh_destroy_trackers(void)
{
	struct dh_tracker *tracker;
	struct dh_tracker *tmp;

	list_for_each_entry_safe(tracker, tmp, &dh_trackers, list) {
		list_del(&tracker->list);
		del_timer(&tracker->timer);
		kfree(tracker);
	}
}

/****************************************************************************/
static inline struct dh_tracker *dh_find_tracker_by_proto(char *proto)
{
	struct dh_tracker *tracker;

	list_for_each_entry(tracker, &dh_trackers, list) {
		if (strcmp(tracker->proto, proto) == 0)
			return tracker;
	}
	return NULL;
}

/****************************************************************************/
static inline struct dh_tracker *dh_find_tracker_by_mark(unsigned long mark)
{
	struct dh_tracker *tracker;

	list_for_each_entry(tracker, &dh_trackers, list) {
		if (tracker == (struct dh_tracker*)mark)
			return tracker;
	}
	return NULL;
}

/****************************************************************************/
static inline struct dh_tracker *dh_find_tracker_by_ct(struct module *m)
{
	struct dh_tracker *tracker;

	list_for_each_entry(tracker, &dh_trackers, list) {
		if (tracker->stat != DH_STAT_UNLOADED &&
		    tracker->ct_module == m)
			return tracker;
	}
	return NULL;
}

/****************************************************************************/
static inline struct dh_tracker *dh_find_tracker_by_nat(struct module *m)
{
	struct dh_tracker *tracker;

	list_for_each_entry(tracker, &dh_trackers, list) {
		if (tracker->stat != DH_STAT_UNLOADED &&
		    tracker->nat_module == m)
			return tracker;
	}
	return NULL;
}

/****************************************************************************/
static unsigned int dh_target(struct sk_buff **pskb,
			      const struct net_device *in,
			      const struct net_device *out,
			      unsigned int hooknum,
			      const void *targinfo, void *userinfo)
{
	struct ipt_dynahelper_info *info =
	    (struct ipt_dynahelper_info *) targinfo;
	struct dh_tracker *tracker = info->tracker;

	DEBUGP("ipt_DYNAHELPER: target: tracker=%p, timeout=%lu, proto=%s\n",
	       tracker, info->timeout, info->proto);

	write_lock_bh(&dh_lock);

	/* Is the user space daemon runing? */
	if (!dh_pid) {
		DEBUGP("ipt_DYNAHELPER: dynahelper not running\n");
		goto pass_it;
	}

	/* Lookup by proto name */
	if (!tracker) {
		tracker = dh_find_tracker_by_proto(info->proto);
		if (!tracker) {	/* We need to create a new tracker */
			tracker = dh_create_tracker(info);
			if (!tracker)
				goto pass_it;
			info->tracker = (void *) tracker;
		}
	}

	switch (tracker->stat) {
	case DH_STAT_LOADED:
		DEBUGP("ipt_DYNAHELPER: helper %s already loaded\n",
		       tracker->proto);
		goto pass_it;
	case DH_STAT_LOADING:
		DEBUGP("ipt_DYNAHELPER: helper %s in loading\n",
		       tracker->proto);
		goto queue_it;
	case DH_STAT_UNLOADED:
		DEBUGP("ipt_DYNAHELPER: helper %s not loaded\n",
		       tracker->proto);
		if (tracker->timeout != info->timeout)
			tracker->timeout = info->timeout;
		tracker->stat = DH_STAT_LOADING;
		load_modules(tracker);
		goto queue_it;
	}

pass_it:
	write_unlock_bh(&dh_lock);
	return IPT_CONTINUE;

queue_it:
	write_unlock_bh(&dh_lock);
	(*pskb)->nfmark = (unsigned long) tracker;
	(*pskb)->nfcache |= NFC_DYNAHELPER;
	return NF_QUEUE;
}

/****************************************************************************/
static int dh_checkentry(const char *tablename, const struct ipt_entry *e,
			 void *targinfo, unsigned int targinfosize,
			 unsigned int hook_mask)
{
	DEBUGP("ipt_DYNAHELPER: checkentry\n");

	if (targinfosize != IPT_ALIGN(sizeof(struct ipt_dynahelper_info))) {
		DEBUGP("ipt_DYNAHELPER: targinfosize %u != %u\n",
		       targinfosize,
		       IPT_ALIGN(sizeof(struct ipt_dynahelper_info)));
		return 0;
	}

	/* Only allow these for raw. */
	if (strcmp(tablename, "raw") != 0) {
		DEBUGP("ipt_DYNAHELPER: wrong table %s\n", tablename);
		return 0;
	}

	if (hook_mask & ~((1 << NF_IP_PRE_ROUTING) | (1 << NF_IP_LOCAL_OUT))) {
		DEBUGP("ipt_DYNAHELPER: hook mask 0x%x bad\n", hook_mask);
		return 0;
	}
#ifndef CONFIG_IP_NF_NAT_LOCAL
	if (hook_mask & (1 << NF_IP_LOCAL_OUT)) {
		DEBUGP
		    ("ipt_DYNAHELPER: CONFIG_IP_NF_NAT_LOCAL not enabled\n");
		return 0;
	}
#endif

	return 1;
}

/****************************************************************************/
static void dh_trackct(struct module *m)
{
	char *proto;
	struct dh_tracker *tracker;

	if (!m)
		return;

	if (strncmp(m->name, "ip_conntrack_", 13))
		return;
	proto = &m->name[13];

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_proto(proto);
	if (tracker &&
	    tracker->stat != DH_STAT_UNLOADED && tracker->ct_module != m) {
		tracker->ct_module = m;
		DEBUGP("ipt_DYNAHELPER: helper %s registered ct\n", proto);
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_untrackct(struct module *m)
{
	struct dh_tracker *tracker;

	if (!m)
		return;

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_ct(m);
	if (tracker) {
		DEBUGP("ipt_DYNAHELPER: helper %s unregistered ct\n",
		       tracker->proto);
		tracker->ct_refcount = 0;
		tracker->ct_module = NULL;
		if (tracker->nat_module == NULL) {
			tracker->stat = DH_STAT_UNLOADED;
			tracker->nat_refcount = 0;
			dh_stop_timer(tracker);
			DEBUGP("ipt_DYNAHELPER: helper %s unloaded\n",
			       tracker->proto);
		} else if (tracker->nat_refcount == 0) {
			dh_start_timer(tracker);
		}
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_tracknat(struct module *m)
{
	char *proto;
	struct dh_tracker *tracker;

	if (!m)
		return;

	if (strncmp(m->name, "ip_nat_", 7))
		return;
	proto = &m->name[7];

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_proto(proto);
	if (tracker &&
	    tracker->stat != DH_STAT_UNLOADED && tracker->nat_module != m) {
		tracker->nat_module = m;
		DEBUGP("ipt_DYNAHELPER: helper %s registered nat\n", proto);
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_untracknat(struct module *m)
{
	struct dh_tracker *tracker;

	if (!m)
		return;

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_nat(m);
	if (tracker) {
		DEBUGP("ipt_DYNAHELPER: helper %s unregistered nat\n",
		       tracker->proto);
		tracker->nat_refcount = 0;
		tracker->nat_module = NULL;
		if (tracker->ct_module == NULL) {
			tracker->stat = DH_STAT_UNLOADED;
			tracker->ct_refcount = 0;
			dh_stop_timer(tracker);
			DEBUGP("ipt_DYNAHELPER: helper %s unloaded\n",
			       tracker->proto);
		} else if (tracker->ct_refcount == 0) {
			dh_start_timer(tracker);
		}
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_refct(struct module *m)
{
	struct dh_tracker *tracker;

	if (!m)
		return;

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_ct(m);
	if (tracker) {
		DEBUGP("ipt_DYNAHELPER: helper %s referenced ct\n",
		       tracker->proto);
		tracker->ct_refcount++;
		dh_stop_timer(tracker);
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_unrefct(struct module *m)
{
	struct dh_tracker *tracker;

	if (!m)
		return;

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_ct(m);
	if (tracker) {
		DEBUGP("ipt_DYNAHELPER: helper %s unreferenced ct\n",
		       tracker->proto);
		if (tracker->ct_refcount)
			tracker->ct_refcount--;
		if (!tracker->ct_refcount && !tracker->nat_refcount)
			dh_start_timer(tracker);
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_refnat(struct module *m)
{
	struct dh_tracker *tracker;

	if (!m)
		return;

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_nat(m);
	if (tracker) {
		DEBUGP("ipt_DYNAHELPER: helper %s referenced nat\n",
		       tracker->proto);
		tracker->nat_refcount++;
		dh_stop_timer(tracker);
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_unrefnat(struct module *m)
{
	struct dh_tracker *tracker;

	if (!m)
		return;

	write_lock_bh(&dh_lock);

	tracker = dh_find_tracker_by_nat(m);
	if (tracker) {
		DEBUGP("ipt_DYNAHELPER: helper %s unreferenced nat\n",
		       tracker->proto);
		if (tracker->nat_refcount)
			tracker->nat_refcount--;
		if (!tracker->nat_refcount && !tracker->ct_refcount)
			dh_start_timer(tracker);
	}

	write_unlock_bh(&dh_lock);
}

/* END OF DYNAHELPER */

static void
ipq_issue_verdict(struct ipq_queue_entry *entry, int verdict)
{
	nf_reinject(entry->skb, entry->info, verdict);
	kfree(entry);
}

static inline int
__ipq_enqueue_entry(struct ipq_queue_entry *entry)
{
       if (queue_total >= queue_maxlen) {
               if (net_ratelimit()) 
                       printk(KERN_WARNING "ip_queue: full at %d entries, "
                              "dropping packet(s).\n", queue_total);
               return -ENOSPC;
       }
       list_add(&entry->list, &queue_list);
       queue_total++;
       return 0;
}

/*
 * Find and return a queued entry matched by cmpfn, or return the last
 * entry if cmpfn is NULL.
 */
static inline struct ipq_queue_entry *
__ipq_find_entry(ipq_cmpfn cmpfn, unsigned long data)
{
	struct list_head *p;

	list_for_each_prev(p, &queue_list) {
		struct ipq_queue_entry *entry = (struct ipq_queue_entry *)p;
		
		if (!cmpfn || cmpfn(entry, data))
			return entry;
	}
	return NULL;
}

static inline void
__ipq_dequeue_entry(struct ipq_queue_entry *entry)
{
	list_del(&entry->list);
	queue_total--;
}

static inline struct ipq_queue_entry *
__ipq_find_dequeue_entry(ipq_cmpfn cmpfn, unsigned long data)
{
	struct ipq_queue_entry *entry;

	entry = __ipq_find_entry(cmpfn, data);
	if (entry == NULL)
		return NULL;

	__ipq_dequeue_entry(entry);
	return entry;
}


static inline void
__ipq_flush(int verdict)
{
	struct ipq_queue_entry *entry;
	
	while ((entry = __ipq_find_dequeue_entry(NULL, 0)))
		ipq_issue_verdict(entry, verdict);
}

static inline int
__ipq_set_mode(unsigned char mode, unsigned int range)
{
	int status = 0;
	
	switch(mode) {
	case IPQ_COPY_NONE:
	case IPQ_COPY_META:
		copy_mode = mode;
		copy_range = 0;
		break;
		
	case IPQ_COPY_PACKET:
		copy_mode = mode;
		copy_range = range;
		if (copy_range > 0xFFFF)
			copy_range = 0xFFFF;
		break;
		
	default:
		status = -EINVAL;

	}
	return status;
}

static inline void
__ipq_reset(void)
{
	peer_pid = 0;
	__ipq_set_mode(IPQ_COPY_NONE, 0);
	__ipq_flush(NF_DROP);
}

static struct ipq_queue_entry *
ipq_find_dequeue_entry(ipq_cmpfn cmpfn, unsigned long data)
{
	struct ipq_queue_entry *entry;
	
	write_lock_bh(&queue_lock);
	entry = __ipq_find_dequeue_entry(cmpfn, data);
	write_unlock_bh(&queue_lock);
	return entry;
}

static void
ipq_flush(int verdict)
{
	write_lock_bh(&queue_lock);
	__ipq_flush(verdict);
	write_unlock_bh(&queue_lock);
}

static struct sk_buff *
ipq_build_packet_message(struct ipq_queue_entry *entry, int *errp)
{
	unsigned char *old_tail;
	size_t size = 0;
	size_t data_len = 0;
	struct sk_buff *skb;
	struct ipq_packet_msg *pmsg;
	struct nlmsghdr *nlh;

	read_lock_bh(&queue_lock);
	
	switch (copy_mode) {
	case IPQ_COPY_META:
	case IPQ_COPY_NONE:
		size = NLMSG_SPACE(sizeof(*pmsg));
		data_len = 0;
		break;
	
	case IPQ_COPY_PACKET:
		if (copy_range == 0 || copy_range > entry->skb->len)
			data_len = entry->skb->len;
		else
			data_len = copy_range;
		
		size = NLMSG_SPACE(sizeof(*pmsg) + data_len);
		break;
	
	default:
		*errp = -EINVAL;
		read_unlock_bh(&queue_lock);
		return NULL;
	}

	read_unlock_bh(&queue_lock);

	/* DYNAHELPER */
	if (entry->skb->nfcache & NFC_DYNAHELPER) {
		size = NLMSG_SPACE(sizeof(*pmsg));
		data_len = 0;
	}

	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb)
		goto nlmsg_failure;
		
	old_tail= skb->tail;
	nlh = NLMSG_PUT(skb, 0, 0, IPQM_PACKET, size - sizeof(*nlh));
	pmsg = NLMSG_DATA(nlh);
	memset(pmsg, 0, sizeof(*pmsg));

	pmsg->packet_id       = (unsigned long )entry;
	pmsg->data_len        = data_len;
	pmsg->timestamp_sec   = entry->skb->stamp.tv_sec;
	pmsg->timestamp_usec  = entry->skb->stamp.tv_usec;
	pmsg->mark            = entry->skb->nfmark;
	pmsg->hook            = entry->info->hook;
	pmsg->hw_protocol     = entry->skb->protocol;
	
	if (entry->info->indev)
		strcpy(pmsg->indev_name, entry->info->indev->name);
	else
		pmsg->indev_name[0] = '\0';
	
	if (entry->info->outdev)
		strcpy(pmsg->outdev_name, entry->info->outdev->name);
	else
		pmsg->outdev_name[0] = '\0';
	
	if (entry->info->indev && entry->skb->dev) {
		pmsg->hw_type = entry->skb->dev->type;
		if (entry->skb->dev->hard_header_parse)
			pmsg->hw_addrlen =
				entry->skb->dev->hard_header_parse(entry->skb,
				                                   pmsg->hw_addr);
	}
	
	if (data_len)
		memcpy(pmsg->payload, entry->skb->data, data_len);
		
	nlh->nlmsg_len = skb->tail - old_tail;
	return skb;

nlmsg_failure:
	if (skb)
		kfree_skb(skb);
	*errp = -EINVAL;
	printk(KERN_ERR "ip_queue: error creating packet message\n");
	return NULL;
}

static int
ipq_enqueue_packet(struct sk_buff *skb, struct nf_info *info, void *data)
{
	int status = -EINVAL;
	struct sk_buff *nskb;
	struct ipq_queue_entry *entry;

	if (copy_mode == IPQ_COPY_NONE && !dh_pid)
		return -EAGAIN;

	entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
	if (entry == NULL) {
		printk(KERN_ERR "ip_queue: OOM in ipq_enqueue_packet()\n");
		return -ENOMEM;
	}
#if defined(CONFIG_MIPS_BRCM)
	        skb->nat_cache_add = NULL;
#endif

	entry->info = info;
	entry->skb = skb;

	if (entry->info->hook == NF_IP_LOCAL_OUT) {
		struct iphdr *iph = skb->nh.iph;

		entry->rt_info.tos = iph->tos;
		entry->rt_info.daddr = iph->daddr;
		entry->rt_info.saddr = iph->saddr;
	}

	nskb = ipq_build_packet_message(entry, &status);
	if (nskb == NULL)
		goto err_out_free;
		
	write_lock_bh(&queue_lock);
	
	/* DYNAHELPER */
	if (skb->nfcache & NFC_DYNAHELPER) {
		read_lock_bh(&dh_lock);
		if (dh_pid) {
			status = netlink_unicast(dhnl, nskb, dh_pid,
						 MSG_DONTWAIT);
			read_unlock_bh(&dh_lock);
			goto check_status;
		}
		read_unlock_bh(&dh_lock);
	}

	if (!peer_pid)
		goto err_out_free_nskb; 

 	/* netlink_unicast will either free the nskb or attach it to a socket */ 
	status = netlink_unicast(ipqnl, nskb, peer_pid, MSG_DONTWAIT);

check_status:
	if (status < 0)
		goto err_out_unlock;
	
	status = __ipq_enqueue_entry(entry);
	if (status < 0)
		goto err_out_unlock;

	write_unlock_bh(&queue_lock);
	return status;

err_out_free_nskb:
	kfree_skb(nskb); 
	
err_out_unlock:
	write_unlock_bh(&queue_lock);

err_out_free:
	kfree(entry);
	return status;
}

static int
ipq_mangle_ipv4(ipq_verdict_msg_t *v, struct ipq_queue_entry *e)
{
	int diff;
	struct iphdr *user_iph = (struct iphdr *)v->payload;

	if (v->data_len < sizeof(*user_iph))
		return 0;
	diff = v->data_len - e->skb->len;
	if (diff < 0)
		skb_trim(e->skb, v->data_len);
	else if (diff > 0) {
		if (v->data_len > 0xFFFF)
			return -EINVAL;
		if (diff > skb_tailroom(e->skb)) {
			struct sk_buff *newskb;
			
			newskb = skb_copy_expand(e->skb,
			                         skb_headroom(e->skb),
			                         diff,
			                         GFP_ATOMIC);
			if (newskb == NULL) {
				printk(KERN_WARNING "ip_queue: OOM "
				      "in mangle, dropping packet\n");
				return -ENOMEM;
			}
			if (e->skb->sk)
				skb_set_owner_w(newskb, e->skb->sk);
			kfree_skb(e->skb);
			e->skb = newskb;
		}
		skb_put(e->skb, diff);
	}
	memcpy(e->skb->data, v->payload, v->data_len);
	e->skb->nfcache |= NFC_ALTERED;

	/*
	 * Extra routing may needed on local out, as the QUEUE target never
	 * returns control to the table.
	 */
	if (e->info->hook == NF_IP_LOCAL_OUT) {
		struct iphdr *iph = e->skb->nh.iph;

		if (!(iph->tos == e->rt_info.tos
		      && iph->daddr == e->rt_info.daddr
		      && iph->saddr == e->rt_info.saddr))
			return ip_route_me_harder(&e->skb);
	}
	return 0;
}

static inline int
id_cmp(struct ipq_queue_entry *e, unsigned long id)
{
	return (id == (unsigned long )e);
}

static int
ipq_set_verdict(struct ipq_verdict_msg *vmsg, unsigned int len)
{
	struct ipq_queue_entry *entry;

	if (vmsg->value > NF_MAX_VERDICT)
		return -EINVAL;

	entry = ipq_find_dequeue_entry(id_cmp, vmsg->id);
	if (entry == NULL)
		return -ENOENT;
	else {
		int verdict = vmsg->value;
		
		if (vmsg->data_len && vmsg->data_len == len)
			if (ipq_mangle_ipv4(vmsg, entry) < 0)
				verdict = NF_DROP;
		
		ipq_issue_verdict(entry, verdict);
		return 0;
	}
}

static int
ipq_set_mode(unsigned char mode, unsigned int range)
{
	int status;

	write_lock_bh(&queue_lock);
	status = __ipq_set_mode(mode, range);
	write_unlock_bh(&queue_lock);
	return status;
}

static int
ipq_receive_peer(struct ipq_peer_msg *pmsg,
                 unsigned char type, unsigned int len)
{
	int status = 0;

	if (len < sizeof(*pmsg))
		return -EINVAL;

	switch (type) {
	case IPQM_MODE:
		status = ipq_set_mode(pmsg->msg.mode.value,
		                      pmsg->msg.mode.range);
		break;
		
	case IPQM_VERDICT:
		if (pmsg->msg.verdict.value > NF_MAX_VERDICT)
			status = -EINVAL;
		else
			status = ipq_set_verdict(&pmsg->msg.verdict,
			                         len - sizeof(*pmsg));
			break;
	default:
		status = -EINVAL;
	}
	return status;
}

static int
dev_cmp(struct ipq_queue_entry *entry, unsigned long ifindex)
{
	if (entry->info->indev)
		if (entry->info->indev->ifindex == ifindex)
			return 1;
			
	if (entry->info->outdev)
		if (entry->info->outdev->ifindex == ifindex)
			return 1;

	return 0;
}

static void
ipq_dev_drop(int ifindex)
{
	struct ipq_queue_entry *entry;
	
	while ((entry = ipq_find_dequeue_entry(dev_cmp, ifindex)) != NULL)
		ipq_issue_verdict(entry, NF_DROP);
}

#define RCV_SKB_FAIL(err) do { netlink_ack(skb, nlh, (err)); return; } while (0)

static inline void
ipq_rcv_skb(struct sk_buff *skb)
{
	int status, type, pid, flags, nlmsglen, skblen;
	struct nlmsghdr *nlh;

	skblen = skb->len;
	if (skblen < sizeof(*nlh))
		return;

	nlh = (struct nlmsghdr *)skb->data;
	nlmsglen = nlh->nlmsg_len;
	if (nlmsglen < sizeof(*nlh) || skblen < nlmsglen)
		return;

	pid = nlh->nlmsg_pid;
	flags = nlh->nlmsg_flags;
	
	if(pid <= 0 || !(flags & NLM_F_REQUEST) || flags & NLM_F_MULTI)
		RCV_SKB_FAIL(-EINVAL);
		
	if (flags & MSG_TRUNC)
		RCV_SKB_FAIL(-ECOMM);
		
	type = nlh->nlmsg_type;
	if (type < NLMSG_NOOP || type >= IPQM_MAX)
		RCV_SKB_FAIL(-EINVAL);
		
	if (type <= IPQM_BASE)
		return;
		
	if (security_netlink_recv(skb))
		RCV_SKB_FAIL(-EPERM);
	
	write_lock_bh(&queue_lock);
	
	if (peer_pid) {
		if (peer_pid != pid) {
			write_unlock_bh(&queue_lock);
			RCV_SKB_FAIL(-EBUSY);
		}
	}
	else
		peer_pid = pid;
		
	write_unlock_bh(&queue_lock);
	
	status = ipq_receive_peer(NLMSG_DATA(nlh), type,
	                          skblen - NLMSG_LENGTH(0));
	if (status < 0)
		RCV_SKB_FAIL(status);
		
	if (flags & NLM_F_ACK)
		netlink_ack(skb, nlh, 0);
        return;
}

static void
ipq_rcv_sk(struct sock *sk, int len)
{
	do {
		struct sk_buff *skb;

		if (down_trylock(&ipqnl_sem))
			return;
			
		while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
			ipq_rcv_skb(skb);
			kfree_skb(skb);
		}
		
		up(&ipqnl_sem);

	} while (ipqnl && ipqnl->sk_receive_queue.qlen);
}

static int
ipq_rcv_dev_event(struct notifier_block *this,
                  unsigned long event, void *ptr)
{
	struct net_device *dev = ptr;

	/* Drop any packets associated with the downed device */
	if (event == NETDEV_DOWN)
		ipq_dev_drop(dev->ifindex);
	return NOTIFY_DONE;
}

/* DYNAHELPER */
/****************************************************************************/
static void dh_receive_msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	struct ipq_peer_msg *pmsg;

	if (skb->len < NLMSG_SPACE(sizeof(*pmsg)))
		return;

	nlh = (struct nlmsghdr *) skb->data;
	if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(*pmsg)))
		return;
	pmsg = NLMSG_DATA(nlh);

	write_lock_bh(&dh_lock);

	if (nlh->nlmsg_type == IPQM_MODE) {
		if (nlh->nlmsg_pid != dh_pid) {
			dh_pid = nlh->nlmsg_pid;
			DEBUGP("ipt_DYNAHELPER: dynahelper %d connected\n",
			       dh_pid);
		}
	} else if (nlh->nlmsg_type == IPQM_VERDICT) {
		if (nlh->nlmsg_pid == 0 || nlh->nlmsg_pid != dh_pid) {
			write_unlock_bh(&dh_lock);
			return;
		}

		if (pmsg->msg.verdict.value == NF_DYNAHELPER_LOADED) {
			struct dh_tracker *tracker =
				dh_find_tracker_by_mark(pmsg->msg.verdict.id);
			if (tracker && tracker->stat == DH_STAT_LOADING) {
				DEBUGP("ipt_DYNAHELPER: helper %s loaded\n",
				       tracker->proto);
				tracker->stat = DH_STAT_LOADED;
				if (!tracker->ct_refcount &&
				    !tracker->nat_refcount)
					dh_start_timer(tracker);
			}
		} else {
			struct ipq_queue_entry * entry =
				ipq_find_dequeue_entry(id_cmp,
						       pmsg->msg.verdict.id);
			if (entry) {
				entry->skb->nfcache &= ~NFC_DYNAHELPER;
				entry->skb->nfmark = 0;
				ipq_issue_verdict(entry,
						  pmsg->msg.verdict.value);
			}
		}
	}

	write_unlock_bh(&dh_lock);
}

/****************************************************************************/
static void dh_receive_handler(struct sock *sk, int len)
{
	do {
		struct sk_buff *skb;

		if (down_trylock(&dhnl_sem))
			return;

		while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {
			dh_receive_msg(skb);
			kfree_skb(skb);
		}

		up(&dhnl_sem);

	} while (dhnl && dhnl->sk_receive_queue.qlen);
}

/****************************************************************************/
static int dh_event_handler(struct notifier_block *this, unsigned long event,
			    void *ptr)
{
	struct netlink_notify *n = ptr;
	struct dh_tracker *tracker;

	if (event == NETLINK_URELEASE && n->protocol == NETLINK_DYNAHELPER &&
	    n->pid) {
		write_lock_bh(&dh_lock);
		if (n->pid == dh_pid) {
			DEBUGP("ipt_DYNAHELPER: dynahelper terminated\n");
			dh_pid = 0;
			/* Set loading helpers' stat */
			list_for_each_entry(tracker, &dh_trackers, list) {
				if (tracker->stat != DH_STAT_LOADING)
					continue;
				if (!tracker->ct_module &&
				    !tracker->nat_module)
					tracker->stat = DH_STAT_UNLOADED;
				else {
					tracker->stat = DH_STAT_LOADED;
					if (!tracker->ct_refcount &&
					    !tracker->nat_refcount)
						dh_start_timer(tracker);
				}
			}
		}
		write_unlock_bh(&dh_lock);
	}
	return NOTIFY_DONE;
}

/****************************************************************************/
static struct ipt_target ipt_dynahelper_reg = {
	.name = "DYNAHELPER",
	.target = dh_target,
	.checkentry = dh_checkentry,
	.me = THIS_MODULE,
};

static struct notifier_block ipq_dev_notifier = {
	.notifier_call	= ipq_rcv_dev_event,
};

static int
ipq_rcv_nl_event(struct notifier_block *this,
                 unsigned long event, void *ptr)
{
	struct netlink_notify *n = ptr;

	/* DYNAHELPER */
	dh_event_handler(this, event, ptr);

	if (event == NETLINK_URELEASE &&
	    n->protocol == NETLINK_FIREWALL && n->pid) {
		write_lock_bh(&queue_lock);
		if (n->pid == peer_pid)
			__ipq_reset();
		write_unlock_bh(&queue_lock);
	}
	return NOTIFY_DONE;
}

static struct notifier_block ipq_nl_notifier = {
	.notifier_call	= ipq_rcv_nl_event,
};

static struct ctl_table_header *ipq_sysctl_header;

static ctl_table ipq_table[] = {
	{
		.ctl_name	= NET_IPQ_QMAX,
		.procname	= NET_IPQ_QMAX_NAME,
		.data		= &queue_maxlen,
		.maxlen		= sizeof(queue_maxlen),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
 	{ .ctl_name = 0 }
};

static ctl_table ipq_dir_table[] = {
	{
		.ctl_name	= NET_IPV4,
		.procname	= "ipv4",
		.mode		= 0555,
		.child		= ipq_table
	},
	{ .ctl_name = 0 }
};

static ctl_table ipq_root_table[] = {
	{
		.ctl_name	= CTL_NET,
		.procname	= "net",
		.mode		= 0555,
		.child		= ipq_dir_table
	},
	{ .ctl_name = 0 }
};

static int
ipq_get_info(char *buffer, char **start, off_t offset, int length)
{
	int len;

	read_lock_bh(&queue_lock);
	
	len = sprintf(buffer,
	              "Peer PID          : %d\n"
	              "Copy mode         : %hu\n"
	              "Copy range        : %u\n"
	              "Queue length      : %u\n"
	              "Queue max. length : %u\n",
	              peer_pid,
	              copy_mode,
	              copy_range,
	              queue_total,
	              queue_maxlen);

	read_unlock_bh(&queue_lock);
	
	*start = buffer + offset;
	len -= offset;
	if (len > length)
		len = length;
	else if (len < 0)
		len = 0;
	return len;
}

static int
init_or_cleanup(int init)
{
	int status = -ENOMEM;
	struct proc_dir_entry *proc;
	
	if (!init)
		goto cleanup_dh;

	netlink_register_notifier(&ipq_nl_notifier);
	ipqnl = netlink_kernel_create(NETLINK_FIREWALL, ipq_rcv_sk);
	if (ipqnl == NULL) {
		printk(KERN_ERR "ip_queue: failed to create netlink socket\n");
		goto cleanup_netlink_notifier;
	}

	proc = proc_net_create(IPQ_PROC_FS_NAME, 0, ipq_get_info);
	if (proc)
		proc->owner = THIS_MODULE;
	else {
		printk(KERN_ERR "ip_queue: failed to create proc entry\n");
		goto cleanup_ipqnl;
	}
	
	register_netdevice_notifier(&ipq_dev_notifier);
	ipq_sysctl_header = register_sysctl_table(ipq_root_table, 0);
	
	status = nf_register_queue_handler(PF_INET, ipq_enqueue_packet, NULL);
	if (status < 0) {
		printk(KERN_ERR "ip_queue: failed to register queue handler\n");
		goto cleanup_sysctl;
	}

	/* DYNAHELPER */
	dhnl = netlink_kernel_create(NETLINK_DYNAHELPER, dh_receive_handler);
	if (dhnl == NULL) {
		printk(KERN_ERR "ipt_DYNAHELPER: failed to create netlink "
		       "socket\n");
		goto cleanup;
	}

	status = ipt_register_target(&ipt_dynahelper_reg);
	if (status < 0) {
		printk(KERN_ERR
		       "ipt_DYNAHELPER: failed to register target\n");
		goto cleanup_dh_netlink;
	}

	/* Set hooks */
	dynahelper_trackct = dh_trackct;
	dynahelper_untrackct = dh_untrackct;
	dynahelper_refct = dh_refct;
	dynahelper_unrefct = dh_unrefct;
	dynahelper_tracknat = dh_tracknat;
	dynahelper_untracknat = dh_untracknat;
	dynahelper_refnat = dh_refnat;
	dynahelper_unrefnat = dh_unrefnat;

	return status;

cleanup_dh:
	/* Reset hooks */
	dynahelper_trackct = NULL;
	dynahelper_untrackct = NULL;
	dynahelper_refct = NULL;
	dynahelper_unrefct = NULL;
	dynahelper_tracknat = NULL;
	dynahelper_untracknat = NULL;
	dynahelper_refnat = NULL;
	dynahelper_unrefnat = NULL;
	ipt_unregister_target(&ipt_dynahelper_reg);

cleanup_dh_netlink:
	sock_release(dhnl->sk_socket);
	down(&dhnl_sem);
	up(&dhnl_sem);
	dh_destroy_trackers();

cleanup:
	nf_unregister_queue_handler(PF_INET);
	synchronize_net();
	ipq_flush(NF_DROP);
	
cleanup_sysctl:
	unregister_sysctl_table(ipq_sysctl_header);
	unregister_netdevice_notifier(&ipq_dev_notifier);
	proc_net_remove(IPQ_PROC_FS_NAME);
	
cleanup_ipqnl:
	sock_release(ipqnl->sk_socket);
	down(&ipqnl_sem);
	up(&ipqnl_sem);
	
cleanup_netlink_notifier:
	netlink_unregister_notifier(&ipq_nl_notifier);
	return status;
}

static int __init init(void)
{
	
	return init_or_cleanup(1);
}

static void __exit fini(void)
{
	init_or_cleanup(0);
}

MODULE_DESCRIPTION("IPv4 packet queue handler");
MODULE_AUTHOR("James Morris <jmorris@intercode.com.au>");
MODULE_LICENSE("GPL");

module_init(init);
module_exit(fini);

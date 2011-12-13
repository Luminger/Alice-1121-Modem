/* SIP extension for UDP NAT alteration.
 *
 * (C) 2006 by polowang  <polowang@broadcom.com>
 * based on RR's ip_nat_ftp.c and other modules.
 *
 */

#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/udp.h>

#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_sip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("polowang <polowang@broadcom.com>");
MODULE_DESCRIPTION("SIP NAT helper");
#if DEBUG_SIP_ALG
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

#define MAX_PORTS	8
static int ports[MAX_PORTS];
static int ports_c;
static char sip_buffer[sizeof("nnn.nnn.nnn.nnn:nnnnn")];	
extern struct sip_header_nfo ct_sip_hdrs[];
extern struct ip_ct_sip_data ct_sip_data[];
DECLARE_LOCK_EXTERN(sipbf_lock);
static unsigned int sip_nat_expected(struct sk_buff **pskb,unsigned int hooknum,struct ip_conntrack *ct,struct ip_nat_info *info);
static int mangle_content_len(struct sk_buff **pskb, enum ip_conntrack_info ctinfo, struct ip_conntrack *ct);
static unsigned int mangle_invite(struct sk_buff **pskb,enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			u_int32_t newip,
			u_int16_t newport,
			 struct ip_conntrack_expect *exp);
static void sip_conntrack_add(u_int16_t index,u_int16_t ct_type,struct ip_conntrack *ct);
static void sip_conntrack_aged(struct ip_conntrack *ct);
static unsigned int 
sip_help(struct ip_conntrack *ct,
	      struct ip_conntrack_expect *exp,
	      struct ip_nat_info *info,
	      enum ip_conntrack_info ctinfo,
	      unsigned int hooknum,
	      struct sk_buff **pskb);
static unsigned int
sip_nat_expected(struct sk_buff **pskb,
		 unsigned int hooknum,
		 struct ip_conntrack *ct,
		 struct ip_nat_info *info);

static struct ip_nat_helper sipext = {
	.list = { NULL, NULL },
	.name = "sipext",
	.flags = IP_NAT_HELPER_F_ALWAYS,
	.me = THIS_MODULE,
	.tuple = { .src = { .ip = 0, 
		 	    .u = { .udp= { .port = 0 } } 
			  }, 
		   .dst = { .ip = 0, 
			    .u = { .all = 0 },
			    .protonum = IPPROTO_UDP
			  } 
		 },
	.mask = { .src = { .ip = 0, 
			   .u = { .udp = { .port = 0xffff } } 
			 }, 
		  .dst = { .ip = 0, 
			   .u = { .all = 0 },
			   .protonum = 0xffff 
		 	 } 
		},
	.help = sip_help,
	.expect = sip_nat_expected
};

static unsigned int mangle_sip_packet(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct, 
			const char *dptr, size_t dlen,
			char *buffer, int bufflen,
			struct sip_header_nfo *hnfo)
{
    char tmpbuffer[sizeof("nnn.nnn.nnn.nnn:nnnnn")];
    const char *p;
    unsigned int matchlen, matchoff;
    p = dptr;
    if (ct_sip_get_info(dptr, dlen, &matchoff, &matchlen, hnfo) <= 0)
        return 0;

    memset(tmpbuffer,0,sizeof("nnn.nnn.nnn.nnn:nnnnn"));
    if (matchlen > sizeof("nnn.nnn.nnn.nnn:nnnnn"))
        matchlen = sizeof("nnn.nnn.nnn.nnn:nnnnn");
    memcpy(tmpbuffer,p + matchoff,matchlen);
    if (isDbgAlg(IP_ALG_DBG_SIP))
        printk("replaced buffer = %s\n",tmpbuffer);
    
    if (!ip_nat_mangle_udp_packet(pskb, ct, ctinfo,matchoff, matchlen, buffer, bufflen))
    {
        return 0;
    }

    /* We need to reload this. Thanks Patrick. */
    dptr = (*pskb)->data + (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr); 
    return 1;
}

static unsigned int mangle_contact(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			u_int32_t newip,
			u_int16_t newport,
			 struct ip_conntrack_expect *exp)
{
    unsigned int dataoff;
    const char *dptr;
    unsigned int bufflen;
    int ret = 0;

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return ret;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return ret;
    }

    memset(sip_buffer,0, sizeof("nnn.nnn.nnn.nnn:nnnnn"));
    bufflen = sprintf(sip_buffer, "%u.%u.%u.%u:%u", NIPQUAD(newip), ntohs(newport));
    if (isDbgAlg(IP_ALG_DBG_SIP))
        printk("ip_nat_sip:buffer = %s\n",sip_buffer);

    if (mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_CONTACT]) > 0)
    {
        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("OK for INVITE\n");
    }		
    
    return ret;
}

static unsigned int mangle_invite(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			u_int32_t newip,
			u_int16_t newport,
			 struct ip_conntrack_expect *exp)
{
    unsigned int dataoff;
    const char *dptr;
    int bufflen;
    int ret = 0;

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return ret;
    }
	
    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return ret;
    }

    memset(sip_buffer,0, sizeof("nnn.nnn.nnn.nnn:nnnnn"));
    bufflen = sprintf(sip_buffer, "%u.%u.%u.%u:%u", NIPQUAD(newip), ntohs(newport));
    if (isDbgAlg(IP_ALG_DBG_SIP))
        printk("ip_nat_sip:buffer = %s\n",sip_buffer);

    if (mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_INVITE]) > 0)
    {
        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("OK for INVITE\n");
    }		
    
    return ret;
}

static unsigned int ip_nat_sip(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			u_int32_t newip,
			u_int16_t newport,
			 struct ip_conntrack_expect *exp)
{
    unsigned int dataoff;
    const char *dptr;
    int bufflen;
    int ret = 0;

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return ret;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return ret;
    }

    memset(sip_buffer,0, sizeof("nnn.nnn.nnn.nnn:nnnnn"));
    if (0 != newport)
    {
        bufflen = sprintf(sip_buffer, "%u.%u.%u.%u:%u", NIPQUAD(newip), ntohs(newport));
    }
    else
    {
        bufflen = sprintf(sip_buffer, "%u.%u.%u.%u", NIPQUAD(newip));
    }

    if (isDbgAlg(IP_ALG_DBG_SIP))
    {
        printk("ip_nat_sip:replace buffer = %s\n",sip_buffer);
    }
	
    if (mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_VIA]) > 0)
    {
        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("OK for VIA\n");
    }

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return ret;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return ret;
    }

    //if (isDbgAlg(IP_ALG_DBG_SIP))	
    //    printk("ip_nat_sip:pskb = %p,*pskb = %p,(*pskb)->data = %p,(*pskb)->len = %d,dptr = %p\n",pskb,*pskb,(*pskb)->data,(*pskb)->len,dptr);	
    if (mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_CONTACT]) > 0)
    {
        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("OK for Contact\n");
    }

    return ret;
}

static unsigned int ip_sip_route(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			u_int32_t newip,
			u_int16_t newport,
			 struct ip_conntrack_expect *exp)
{
    unsigned int dataoff;
    const char *dptr;
    int bufflen;
    int ret = 0;

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return ret;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return ret;
    }

    memset(sip_buffer,0, sizeof("nnn.nnn.nnn.nnn:nnnnn"));
    if (0 != newport)
    {
        bufflen = sprintf(sip_buffer, "%u.%u.%u.%u:%u", NIPQUAD(newip), ntohs(newport));
    }
    else
    {
        bufflen = sprintf(sip_buffer, "%u.%u.%u.%u", NIPQUAD(newip));
    }
 
    if (isDbgAlg(IP_ALG_DBG_SIP))
    {
        printk("ip_nat_sip:replace buffer = %s\n",sip_buffer);
        //printk("ip_nat_sip:pskb = %p,*pskb = %p,(*pskb)->data = %p,(*pskb)->len = %d,dptr = %p\n",pskb,*pskb,(*pskb)->data,(*pskb)->len,dptr);
    }
	
    if (mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_ROUTE]) > 0)
    {
        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("OK for VIA\n");
    }

    return ret;
}

/*
(1)changed via
(2)changed contact
(3)changed owner
(4)changed connect
*/
static int ip_nat_sdp(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct,
			u_int32_t newip,
			u_int16_t newport,
			 struct ip_conntrack_expect *exp)
{
    unsigned int bufflen;
    unsigned int dataoff;
    const char *dptr;

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return 0;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return 0;
    }

    memset(sip_buffer,0,sizeof("nnn.nnn.nnn.nnn:nnnnn"));
    /* Mangle owner and contact info. */
    bufflen = sprintf(sip_buffer, "%u.%u.%u.%u", NIPQUAD(newip));
    mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_OWNER]);

    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
	return 0;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return 0;
    }
	
    mangle_sip_packet(pskb, ctinfo, ct, dptr, (*pskb)->len - dataoff, sip_buffer, bufflen, &ct_sip_hdrs[POS_CONECTION]);
    return mangle_content_len(pskb, ctinfo, ct);   
}

static int mangle_content_len(struct sk_buff **pskb, 
			enum ip_conntrack_info ctinfo,
			struct ip_conntrack *ct)
{
    unsigned int dataoff, matchoff, matchlen;
    char buffer[sizeof("65536")];
    unsigned int bufflen;
    const char *dptr;
    memset(buffer,0,sizeof("65536"));
    dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);

    if (dataoff >= (*pskb)->len)
    {
        printk("skb->len = %u\n", (*pskb)->len);
        return 0;
    }

    if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
        dptr = (*pskb)->data + dataoff;
    else
    {
        printk("Copy of skbuff not supported yet.\n");
        return 0;
    }
	
    /* Get actual SDP lenght */
    if (ct_sip_get_info(dptr, (*pskb)->len - dataoff, &matchoff, &matchlen, &ct_sip_hdrs[POS_SDP_HEADER]) > 0)
    {
        /*
          since ct_sip_get_info() give us a pointer passing 'v='
          we need to add 2 bytes in this count. 
        */
        int c_len = (*pskb)->len - dataoff - matchoff + 2;
		
        /* Now, update SDP lenght */
        if (ct_sip_get_info(dptr, (*pskb)->len - dataoff, &matchoff, &matchlen, &ct_sip_hdrs[POS_CONTENT]) > 0)
        {
            bufflen = sprintf(buffer, "%u", c_len);
            return ip_nat_mangle_udp_packet(pskb, ct, ctinfo, matchoff,	matchlen, buffer, bufflen);
        }
    }

    return 0;
}

static void sip_conntrack_aged(struct ip_conntrack *ct)
{
	u_int16_t i;
	if (NULL == ct)
		return;

       struct ip_ct_sip_master *ct_sip_info = &ct->help.ct_sip_info;
	if (isDbgAlg(IP_ALG_DBG_SIP))
		printk("sip_conntrack_aged:index %d\n",ct_sip_info->index);
	ct_sip_info->flags = ct_sip_info->flags&(~SIP_CT_INVIT_200OK);
       if (ct_sip_info->index > MAX_PORT_MAPS)
	   	return;
       if (ct_sip_data[ct_sip_info->index].used <= 0)
	   	return;

	if (ct_sip_data[ct_sip_info->index].ctl_audio)
		ip_conntrack_aged(ct_sip_data[ct_sip_info->index].ctl_audio);
	if (ct_sip_data[ct_sip_info->index].ctl_video)
		ip_conntrack_aged(ct_sip_data[ct_sip_info->index].ctl_video);
	if (ct_sip_data[ct_sip_info->index].ct_audio)
		ip_conntrack_aged(ct_sip_data[ct_sip_info->index].ct_audio);
	if (ct_sip_data[ct_sip_info->index].ct_video)
		ip_conntrack_aged(ct_sip_data[ct_sip_info->index].ct_video);

	ct_sip_data[ct_sip_info->index].ctl_audio = NULL;
	ct_sip_data[ct_sip_info->index].ctl_video = NULL;
	ct_sip_data[ct_sip_info->index].ct_audio  = NULL;
	ct_sip_data[ct_sip_info->index].ct_video  = NULL;
	ct_sip_data[ct_sip_info->index].used = 0;

	for (i = 0;i < MAX_PORT_MAPS;i++)
	{
		if (ct_sip_data[i].used <= 0) 
			if (isDbgAlg(IP_ALG_DBG_SIP))
				printk("\nct_sip_tbl:index %d unused\n",i);
	}
}

static void sip_conntrack_add(u_int16_t index,u_int16_t ct_type,struct ip_conntrack *ct)
{
	if (NULL == ct)
		return;

       if (index > MAX_PORT_MAPS)
	   	return;
	if (ct_sip_data[index].used <= 0)
		return;
	if (isDbgAlg(IP_ALG_DBG_SIP))
		printk("sip_conntrack_add:index %d,ct_type = %d\n",index,ct_type);
 	if (ct_type == SIP_CTL_AUDIO)
		ct_sip_data[index].ctl_audio  = ct;
	if (ct_type == SIP_CT_AUDIO)
		ct_sip_data[index].ct_audio = ct;
	if (ct_type == SIP_CTL_VIDEO)
		ct_sip_data[index].ctl_video  = ct;
	if (ct_type == SIP_CT_VIDEO)
		ct_sip_data[index].ct_video = ct;
}

static unsigned int 
sip_help(struct ip_conntrack *ct,
	      struct ip_conntrack_expect *exp,
	      struct ip_nat_info *info,
	      enum ip_conntrack_info ctinfo,
	      unsigned int hooknum,
	      struct sk_buff **pskb)
{
        unsigned int dataoff,sip_status;
        size_t datalen;
        const char *dptr;
        int matchoff, matchlen;
        int dir = CTINFO2DIR(ctinfo);
        int pkt_type;

        struct ip_conntrack_tuple newtuple;
        struct ip_ct_sip_expect *exp_sip_info = NULL;
        struct ip_ct_sip_master *ct_sip_info = &ct->help.ct_sip_info;
        u_int16_t port = 0;
        u_int32_t newip = 0;
        struct ip_conntrack *master = NULL;

        if (!((hooknum == NF_IP_POST_ROUTING && dir == IP_CT_DIR_ORIGINAL)
            || (hooknum == NF_IP_PRE_ROUTING && dir == IP_CT_DIR_REPLY))) 
            return NF_ACCEPT;

        if ((hooknum == NF_IP_POST_ROUTING) && (dir == IP_CT_DIR_ORIGINAL))
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("go to ip_nat_sip.c: NF_IP_POST_ROUTING-IP_CT_DIR_ORIGINAL\n");
        }
	
        if((hooknum == NF_IP_PRE_ROUTING) && dir == (IP_CT_DIR_REPLY)) 
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("go to ip_nat_sip.c:NF_IP_PRE_ROUTING-IP_CT_DIR_REPLY\n");
        }
	
        if (!exp)
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("no conntrack expectation to modify\n");
        }

        dataoff = (*pskb)->nh.iph->ihl*4 + sizeof(struct udphdr);
        if (dataoff >= (*pskb)->len)
        {
            printk("skb->len = %u\n", (*pskb)->len);		
            return NF_ACCEPT;
        }

        if ((dataoff + (*pskb)->len - dataoff) <= skb_headlen(*pskb))
            dptr = (*pskb)->data + dataoff;
        else
        {
            printk("Copy of skbuff not supported yet.\n");
            return NF_ACCEPT;
        }

        datalen = (*pskb)->len - dataoff;
        if (exp)
            exp_sip_info = &exp->help.exp_sip_info;	

        LOCK_BH(&sipbf_lock);	
	
        if (memcmp(dptr, "REGISTER sip:", sizeof("REGISTER sip:") - 1) == 0)
        {
            if (exp)
            {
                    exp->tuple.dst.ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                    exp->mask.dst.ip = 0XFFFFFFFF;

                    if (IP_CT_SIP_WAIT_REGREPLY == exp_sip_info->siptype)
                    {
                        newtuple.src.ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.ip;
                        newtuple.dst.ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                        newip = newtuple.dst.ip;
                    }

                    newtuple.dst.protonum = IPPROTO_UDP;
                    newtuple.src.u.udp.port = exp->tuple.src.u.udp.port;

                    /* Try to get same port: if not, try to change it. */
                    for (port = exp_sip_info->port; port != 0; port++)
                    {
                        newtuple.dst.u.udp.port = htons(port);
                        if (ip_conntrack_change_expect(exp, &newtuple) == 0)
                            break;
                    }

                    if (port == 0)
                    {
                        if (isDbgAlg(IP_ALG_DBG_SIP))
                            printk("Warning:port = 0\n");
                        port     = ct_sip_info-> port;
                    }
                    
                    if (isDbgAlg(IP_ALG_DBG_SIP))
                    {
                        printk("exp:dst-protonum = %d;src = %X:%d;dst = %X:%d\n",exp->tuple.dst.protonum,
                            exp->tuple.src.ip,exp->tuple.src.u.udp.port,exp->tuple.dst.ip,exp->tuple.dst.u.udp.port);

                        printk("exp mask:dst-protonum = %d;src = %X:%d;dst = %X:%d\n",exp->mask.dst.protonum,
                            exp->mask.src.ip,exp->mask.src.u.udp.port,exp->mask.dst.ip,exp->mask.dst.u.udp.port);
                    }
            }
            else
            {
                newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                port     = ct_sip_info-> port;
            }

            /*mangle the packets and changed the port*/
            ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
        }

        if (memcmp(dptr, "INVITE sip:", sizeof("INVITE sip:") - 1) == 0)
        {
            /*
              change ip address and port
              (1)Via
              (2)Contact
              (3)Owner
            */
            if (ct_sip_info->invitdir ==  IP_CT_SIP_INVIT_PUB2PRI)
            {
                /*
                  change ip address from public to private
                */
                master = master_ct(ct);
                if (NULL != master)
                {
                    newip =  ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.ip;
                    port = ct_sip_info->port;
                }
                else
                {
                    newip =  ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                    port = ct_sip_info->port;
                }

                mangle_invite(pskb,ctinfo,ct,newip,port,exp);
            }
            else
            {
                /*
                  change ip address from private to public
                */
                newip =  ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                if (exp)
                {
                    exp->tuple.dst.ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                    exp->tuple.dst.u.udp.port = htons(exp_sip_info->port);

                    newtuple.src.ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.ip;
                    newtuple.dst.ip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                    exp->mask.src.u.udp.port = 0XFFFF;
	
                    exp->mask.src.ip = 0XFFFFFFFF;
                    exp->mask.dst.protonum = 0XFFFF;
                    exp->mask.dst.u.udp.port = 0XFFFF;
                    exp->mask.dst.ip = 0XFFFFFFFF;
		
                    newtuple.dst.protonum = IPPROTO_UDP;
                    newtuple.src.u.udp.port = exp->tuple.src.u.udp.port;

                    /* Try to get same port: if not, try to change it. */
                    for (port = exp_sip_info->port; port != 0; port++)
                    {
                         newtuple.dst.u.udp.port = htons(port);
                         if (ip_conntrack_change_expect(exp, &newtuple) == 0)
                             break;
                    }

                    if (port == 0)
                    {
                        if (isDbgAlg(IP_ALG_DBG_SIP))
                            printk("Warning:port = 0\n");
                        port = exp_sip_info->port;
                    }

                    if (isDbgAlg(IP_ALG_DBG_SIP))
                    {
                        printk("exp:dst-protonum = %d;src = %X:%d;dst = %X:%d\n",exp->tuple.dst.protonum,exp->tuple.src.ip,exp->tuple.src.u.udp.port,
                                exp->tuple.dst.ip,exp->tuple.dst.u.udp.port);

                        printk("exp mask:dst-protonum = %d;src = %X:%d;dst = %X:%d\n",exp->mask.dst.protonum,exp->mask.src.ip,exp->mask.src.u.udp.port,
                                exp->mask.dst.ip,exp->mask.dst.u.udp.port);
                    }
                }
                else
                   port = ct_sip_info->port;
			
                if (isDbgAlg(IP_ALG_DBG_SIP))
                    printk("newip = %X, port = %d\n",newip,port);
                ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
                ip_nat_sdp(pskb,ctinfo,ct,newip,port,exp);
            }
        }

        if (memcmp(dptr, "SIP/2.0", sizeof("SIP/2.0") - 1) == 0)
        {
            struct ip_ct_sip_master *ct_sip_info;
            struct ip_conntrack *master;

            if (ct_sip_get_info(dptr, datalen, &matchoff, &matchlen, &ct_sip_hdrs[POS_STATUS]) > 0)
            {
                sip_status = simple_strtoul((dptr + matchoff), NULL, 10);
            }
            else
                sip_status = 0;

            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("ip_nat_sip:sip_status = %d\n",sip_status);

            pkt_type = sip_getpkt_type(dptr,datalen);
            if (PKT_REGISTER == pkt_type)
            {
                master = master_ct(ct);
                if (master)
                {
                    ct_sip_info = &master->help.ct_sip_info;
                    /*register packet's reponse*/
                    newip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                    port   = ct_sip_info->port;
                }
                else
                {
                    ct_sip_info = &ct->help.ct_sip_info;
                    /*register packet's reponse*/
                    newip = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                    port   = ct_sip_info->port;	
                }

                if (isDbgAlg(IP_ALG_DBG_SIP))
                    printk("ip_nat_sip:newip = %X, port = %d\n",newip,port);
                ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
            }
		
            if (PKT_INVITE == pkt_type)
            {
                /*invite packet's reponse*/
                master = master_ct(ct);
                if (NULL != master)
                {
                    ct_sip_info = &master->help.ct_sip_info;			
                }
                else
                {
                    ct_sip_info = &ct->help.ct_sip_info;				
                }

                if (200 == sip_status)
                {
                    /*changed packets content and added expection ip conntrack*/
                    //ct_sip_info = &ct->help.ct_sip_info;
                    if (IP_CT_SIP_INVIT_PRI2PUB == ct_sip_info->invitdir)
                    {
                        if ((IP_CT_DIR_ORIGINAL == dir)&&(NULL != master))
                            set_expected_rtp(pskb,ct,ctinfo,ct_sip_info);
			   if ((IP_CT_DIR_REPLY == dir)&&(NULL == master))
                            set_expected_rtp(pskb,ct,ctinfo,ct_sip_info);
                    }

		      if (IP_CT_SIP_INVIT_PUB2PRI == ct_sip_info->invitdir)
                    {
                        if ((IP_CT_DIR_ORIGINAL == dir)&&(NULL == master))
                            set_expected_rtp(pskb,ct,ctinfo,ct_sip_info);
                    }
                }
			
                if (IP_CT_SIP_INVIT_PRI2PUB == ct_sip_info->invitdir)
                {
                    if (master)
                    {
                        newip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                    }
                    else
                        newip = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                    port   = ct_sip_info->port;
                    ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
                }
                else
                {
                    if (200 == sip_status)
                    {
                        newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                        port   = ct_sip_info->port;
                        mangle_contact(pskb,ctinfo,ct,newip,port,exp);
                        ip_nat_sdp(pskb,ctinfo,ct,newip,port,exp);
                    }
                }
            }

	     if (PKT_BYE == pkt_type)
            {
                master = master_ct(ct);
                if ((NULL !=master)&&(dir == IP_CT_DIR_REPLY))
                {
                    ct_sip_info = &master->help.ct_sip_info;
                    /*register packet's reponse*/
                    newip = master->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                    port   = ct_sip_info->port;
		      if (isDbgAlg(IP_ALG_DBG_SIP))
                        printk("ip_nat_sip:bye:newip = %X, port = %d\n",newip,port);
                    ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
                }

                if ((NULL ==master)&&(dir == IP_CT_DIR_ORIGINAL))
                {
                    ct_sip_info = &ct->help.ct_sip_info;
                    /*register packet's reponse*/
                    newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                    port   = ct_sip_info->port;	
		      if (isDbgAlg(IP_ALG_DBG_SIP))
                        printk("ip_nat_sip:bye:newip = %X, port = %d\n",newip,port);
                    ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
                }

                if (200 == sip_status)
                {
 			if (NULL == master)
			{
				sip_conntrack_aged(ct);
			}
			else
			{
				sip_conntrack_aged(master);
			}
               }
            }			
        }
	
        if (memcmp(dptr, "ACK", sizeof("ACK") - 1) == 0)
        {
            master = master_ct(ct);
            if ((!master)&&(dir == IP_CT_DIR_ORIGINAL))
            {
                ct_sip_info = &ct->help.ct_sip_info;
                /*register packet's reponse*/
                newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
                port   = ct_sip_info->port;
                if (isDbgAlg(IP_ALG_DBG_SIP))
                    printk("ip_nat_sip:ACK newip = %X, port = %d\n",newip,port);
                 ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
                 ip_sip_route(pskb,ctinfo,ct,newip,0,exp);
            }
        }	

	if (memcmp(dptr, "BYE sip:", sizeof("BYE sip:") - 1) == 0)
	{
		/*deleted video/voice/expect conntrack*/
		master = master_ct(ct);
		if ((!master)&&(dir == IP_CT_DIR_ORIGINAL))
		{
			/*initially bye*/
			ct_sip_info = &ct->help.ct_sip_info;
			/*register packet's reponse*/
			newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.ip;
			port   = ct_sip_info->port;
			if (isDbgAlg(IP_ALG_DBG_SIP))
				printk("ip_nat_sip:bye: newip = %X, port = %d\n",newip,port);
			ip_nat_sip(pskb,ctinfo,ct,newip,port,exp);
		}

		/**/
		if (NULL == master)
		{
			sip_conntrack_aged(ct);
		}
		else
		{
			sip_conntrack_aged(master);
		}
	}
	
        UNLOCK_BH(&sipbf_lock);
        return NF_ACCEPT;
}


static unsigned int
sip_nat_expected(struct sk_buff **pskb,
		 unsigned int hooknum,
		 struct ip_conntrack *ct,
		 struct ip_nat_info *info)
{
        struct ip_nat_multi_range mr;
        u_int32_t newdstip, newsrcip, newip;
        uint16_t port = 0;
        uint16_t srcport = 0;
        //struct ip_ct_sip_expect *exp_sip_info;
        struct ip_ct_sip_master *ct_sip_info;
        unsigned int ret = NF_ACCEPT;
        struct ip_conntrack *master = master_ct(ct);

        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("call sip_nat_expected\n");
        IP_NF_ASSERT(info);
        IP_NF_ASSERT(master);

        IP_NF_ASSERT(!(info->initialized & (1<<HOOK2MANIP(hooknum))));
        ct_sip_info = &master->help.ct_sip_info;
        LOCK_BH(&sipbf_lock);
        if (IP_CT_SIP_WAIT_REGREPLY == ct_sip_info->siptype)
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("siptype = IP_CT_SIP_WAIT_REGREPLY\n");
            newdstip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
            newsrcip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip;
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("nat_expected: registry reply %u.%u.%u.%u->%u.%u.%u.%u\n",NIPQUAD(newsrcip), NIPQUAD(newdstip));
            port = ct_sip_info->port;
        } 

        if (IP_CT_SIP_WAIT_INVITREPLY == ct_sip_info->siptype)
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("siptype = %d, invitedir = %d\n",ct_sip_info->siptype,ct_sip_info->invitdir);

            if (IP_CT_SIP_INVIT_PRI2PUB == ct_sip_info->invitdir)
            {
                newdstip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                newsrcip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip;
                if (isDbgAlg(IP_ALG_DBG_SIP))
                    printk("nat_expected: invite reply %u.%u.%u.%u->%u.%u.%u.%u\n",NIPQUAD(newsrcip), NIPQUAD(newdstip));
            }
            else
            {
                //newdstip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip;
                //newsrcip = master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip;			
            }
            port = ct_sip_info->port;
        }

        if (IP_CT_SIP_WAIT_INVITVIDEO == ct_sip_info->siptype)
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("siptype = IP_CT_SIP_WAIT_INVITVIDEO\n");
            newdstip = ct_sip_info->privateIP;
            newsrcip = ct_sip_info->publicIP;
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("nat_expected: registry reply %u.%u.%u.%u->%u.%u.%u.%u\n",NIPQUAD(newsrcip), NIPQUAD(newdstip));
            
            port = ct_sip_info->videoPort;
            if (htons(ct_sip_info->videoPort) == ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port)
            {
                port = ct_sip_info->videoPort;
                srcport = ct_sip_info->videoRPort;
                sip_conntrack_add(ct_sip_info->index,SIP_CT_VIDEO,ct);
            }
            else
            {
                port = ct_sip_info->videoPort + 1;
		srcport = ct_sip_info->videoRPort + 1;
                sip_conntrack_add(ct_sip_info->index,SIP_CTL_VIDEO,ct);
            }
        }

        if (IP_CT_SIP_WAIT_INVITAUDIO == ct_sip_info->siptype)
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("siptype = IP_CT_SIP_WAIT_INVITAUDIO\n");
            newdstip = ct_sip_info->privateIP;
            newsrcip = ct_sip_info->publicIP;
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("nat_expected: registry reply %u.%u.%u.%u->%u.%u.%u.%u\n",NIPQUAD(newsrcip), NIPQUAD(newdstip));

            if (htons(ct_sip_info->voicePort) == ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port)
            {
                port = ct_sip_info->voicePort;
		  srcport = ct_sip_info->voiceRPort;
               sip_conntrack_add(ct_sip_info->index,SIP_CT_AUDIO,ct);
            }
            else
            {
                port = ct_sip_info->voicePort + 1;
		srcport = ct_sip_info->voiceRPort + 1;
                sip_conntrack_add(ct_sip_info->index,SIP_CTL_AUDIO,ct);
            }
        }

        if (IP_CT_SIP_WAIT_BOTHSTREAM == ct_sip_info->siptype)
        {
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("siptype = IP_CT_SIP_WAIT_BOTHSTREAM\n");
            newdstip = ct_sip_info->privateIP;
            newsrcip = ct_sip_info->publicIP;
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("nat_expected: registry reply %u.%u.%u.%u->%u.%u.%u.%u\n",NIPQUAD(newsrcip), NIPQUAD(newdstip));

            if (htons(ct_sip_info->videoPort) == ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port)
            {
                port = ct_sip_info->videoPort;
                srcport = ct_sip_info->videoRPort;
                sip_conntrack_add(ct_sip_info->index,SIP_CT_VIDEO,ct);
            }

            if (htons((ct_sip_info->videoPort + 1)) == ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port)
            {
                port = ct_sip_info->videoPort + 1;
		  srcport = ct_sip_info->videoRPort + 1;
               sip_conntrack_add(ct_sip_info->index,SIP_CTL_VIDEO,ct);
            }		

            if (htons(ct_sip_info->voicePort) == ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port)
            {
                port = ct_sip_info->voicePort;
	         srcport = ct_sip_info->voiceRPort;
               sip_conntrack_add(ct_sip_info->index,SIP_CT_AUDIO,ct);
            }
            else
            {
                port = ct_sip_info->voicePort + 1;
		  srcport = ct_sip_info->voiceRPort + 1;
               sip_conntrack_add(ct_sip_info->index,SIP_CTL_AUDIO,ct);
            }
        }
		
        UNLOCK_BH(&sipbf_lock);
        if (isDbgAlg(IP_ALG_DBG_SIP))
            printk("hooknum = %d\n",hooknum);
	
        if (HOOK2MANIP(hooknum) == IP_NAT_MANIP_SRC)
        {
            //if ((IP_CT_SIP_WAIT_REGREPLY == ct_sip_info->siptype)||(IP_CT_SIP_WAIT_INVITREPLY == ct_sip_info->siptype))
                 return NF_ACCEPT;
            //newip = newsrcip;
	     //port    = srcport;	
        }
        else
        {
            newip = newdstip;
            if (isDbgAlg(IP_ALG_DBG_SIP))
                printk("IP_NAT_MANIP_DST\n");
        }
	
        if (isDbgAlg(IP_ALG_DBG_SIP))	
            printk("nat_expected: IP to %u.%u.%u.%u:%d\n", NIPQUAD(newip),port);

        mr.rangesize = 1;
        /* We don't want to manip the per-protocol, just the IPs... */
        mr.range[0].flags = IP_NAT_RANGE_MAP_IPS;
        mr.range[0].min_ip = mr.range[0].max_ip = newip;
        /*
         ... unless we're doing a MANIP_DST, in which case, make
         sure we map to the correct port
        */
        if (HOOK2MANIP(hooknum) == IP_NAT_MANIP_DST)
        {
            mr.range[0].flags |= IP_NAT_RANGE_PROTO_SPECIFIED;
            mr.range[0].min = mr.range[0].max
                            = ((union ip_conntrack_manip_proto)
                              { .udp = { htons(port) } });
        }

        ret =  ip_nat_setup_info(ct, &mr, hooknum);
        if ((IP_CT_SIP_WAIT_REGREPLY == ct_sip_info->siptype)||(IP_CT_SIP_WAIT_INVITREPLY == ct_sip_info->siptype))
            info->helper = &sipext;
	return ret;
}

static struct ip_nat_helper sip[MAX_PORTS];
static char sip_names[MAX_PORTS][10];

static void fini(void)
{
        int i = 0;
        for (; i < ports_c; i++)
        {
            printk("unregistering helper for port %d\n", ports[i]);
            ip_nat_helper_unregister(&sip[i]);
        } 
}

static int __init init(void)
{
        int i, ret;
        char *tmpname;

        if (ports_c == 0)
            ports[ports_c++] = SIP_PORT;

        for (i = 0; i < ports_c; i++)
        {
            /* Create helper structure */
            memset(&sip[i], 0, sizeof(struct ip_conntrack_helper));

            sip[i].tuple.dst.protonum = IPPROTO_UDP;
            sip[i].tuple.src.u.udp.port = htons(ports[i]);
            sip[i].mask.dst.protonum = 0xFFFF;
            sip[i].mask.src.u.udp.port = 0xFFFF;
            sip[i].me = THIS_MODULE;
            sip[i].help =sip_help;
            sip[i].flags = IP_NAT_HELPER_F_ALWAYS;
            sip[i].expect = sip_nat_expected;

            tmpname = &sip_names[i][0];
            if (ports[i] == SIP_PORT)
               sprintf(tmpname, "sip");
            else
               sprintf(tmpname, "sip-%d", i);
            sip[i].name = tmpname;

            ret=ip_nat_helper_register(&sip[i]);
            if (ret)
            {
                printk("ERROR registering helper for port %d\n",ports[i]);
                fini();
                return(ret);
            }
        }
        
        return(0);
}
module_init(init);
module_exit(fini);

#ifndef __IP_CONNTRACK_SIP_H__
#define __IP_CONNTRACK_SIP_H__
/* SIP tracking. */

#ifdef __KERNEL__
#include <linux/netfilter_ipv4/lockhelp.h>

DECLARE_LOCK_EXTERN(sipbf_lock);

#define SIP_PORT	5060

#endif /* __KERNEL__ */

#define SIP_TIMEOUT	3600

#define POS_VIA		0
#define POS_CONTACT	1
#define POS_CONTENT	2
#define POS_MEDIA	3
#define POS_OWNER	4
#define POS_CONECTION	5
#define POS_REQ_HEADER	6
#define POS_SDP_HEADER	7
#define POS_REGISTER	8
#define POS_MVIDEO		9
#define POS_CALLID		10
#define POS_STATUS		11
#define POS_CSEQREG		12
#define POS_CSEQINVT	13
#define POS_INVITE		14
#define POS_ROUTE		15
#define POS_CSEQBYE		16

#define MAX_SIP_CALLID_LEN 64
#define PKT_REGISTER	1
#define PKT_INVITE		2
#define PKT_BYE			3

#define MAX_PORT_MAPS		16

#define SIP_CTL_AUDIO		1
#define SIP_CT_AUDIO		2
#define SIP_CTL_VIDEO		3
#define SIP_CT_VIDEO		4

#define SIP_CT_INVIT_200OK 		0X0001

enum ip_ct_sip_type
{
	IP_CT_SIP_WAIT_REGREPLY,
	IP_CT_SIP_WAIT_INVITREPLY,
	IP_CT_SIP_WAIT_INVITAUDIO,
	IP_CT_SIP_WAIT_INVITVIDEO,
	IP_CT_SIP_WAIT_BOTHSTREAM,
};

enum ip_ct_sip_invit_dir
{
	IP_CT_SIP_INVIT_IDLE,
	IP_CT_SIP_INVIT_PRI2PUB,
	IP_CT_SIP_INVIT_PUB2PRI,	
};

/* This structure is per expected connection */
struct ip_ct_sip_expect
{
	u_int16_t port; 
	u_int16_t voicePort;/*port for voice*/
	u_int16_t videoPort;/*port for voice*/
	u_int16_t voiceRPort; /*port of remote peer*/
	u_int16_t videoRPort; /*port of reomote peer*/
	u_int32_t publicIP;  /*remote ip*/
	u_int32_t privateIP;/*local ip in private network behind cpe*/
	enum ip_ct_sip_type siptype;
	enum ip_ct_sip_invit_dir invitdir;
	//char    callid[MAX_SIP_CALLID_LEN];
};

struct ip_ct_sip_master
{
	u_int16_t port;
	u_int16_t index;
	u_int16_t flags;/*0,not received 200OK for invite,1,received  200OK*/
	u_int16_t voicePort;/*port for voice*/
	u_int16_t videoPort;/*port for voice*/
	u_int16_t voiceRPort; /*pad*/
	u_int16_t videoRPort; /*pad*/
	u_int32_t publicIP;  /*remote ip*/
	u_int32_t privateIP;/*local ip in private network behind cpe*/
	enum ip_ct_sip_type siptype;
	enum ip_ct_sip_invit_dir invitdir;
};

struct ip_ct_sip_data
{
	u_int16_t client_map_port;
	struct ip_conntrack *ct_audio;
	struct ip_conntrack *ct_video;
	struct ip_conntrack *ctl_audio;
	struct ip_conntrack *ctl_video;
	int used;
};

struct sip_header_nfo {
	const char *lname;
	size_t lnlen;
	const char *sname;
	size_t snlen;
	const char *ln_str;
	size_t ln_strlen;
	int (*match_len)(const char *, const char *, int *);

};

extern int ct_sip_get_info(const char *dptr, size_t dlen, 
				unsigned int *matchoff, 
				unsigned int *matchlen, 
				struct sip_header_nfo *hnfo);

extern int sip_getpkt_type(const char *dptr,size_t datalen);

extern int set_expected_rtp(struct sk_buff **pskb, 
			struct ip_conntrack *ct,
			enum ip_conntrack_info ctinfo, 
			struct ip_ct_sip_master *ct_sip_info
			);
			
/* get line lenght until first CR or LF seen. */
static __inline__ int ct_sip_lnlen(const char *line, const char *limit)
{
        const char *k = line;

        while ((line <= limit) && (*line == '\r' || *line == '\n'))
                line++;

        while (line <= limit)
	{
                if (*line == '\r' || *line == '\n')
                        break;
                line++;
        }
        return line - k;
}

/* Linear string search, case sensitive. */
static __inline__ 
const char *ct_sip_search(const char *needle, const char *haystack, 
			size_t needle_len, size_t haystack_len) 
{
	const char *limit = haystack + (haystack_len - needle_len);

	while (haystack <= limit) {
		if (memcmp(haystack, needle, needle_len) == 0)
			return haystack;
		haystack++;
	}
	return NULL;
}

#endif /* __IP_CONNTRACK_SIP_H__ */

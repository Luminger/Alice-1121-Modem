/* 
 * files.c -- DHCP server file manipulation *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 */
 
#include <stdio.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "debug.h"
#include "dhcpd.h"
#include "files.h"
#include "options.h"
#include "leases.h"
#include "static_leases.h"

// brcm
#include "board_api.h"


/* on these functions, make sure you datatype matches */
static int read_ip(char *line, void *arg)
{
	struct in_addr *addr = arg;
	inet_aton(line, addr);
	return 1;
}

//For static IP lease
static int read_mac(const char *line, void *arg)
{
	uint8_t *mac_bytes = arg;
	struct ether_addr *temp_ether_addr;
	int retval = 1;

	temp_ether_addr = ether_aton(line);

	if(temp_ether_addr == NULL)
		retval = 0;
	else
		memcpy(mac_bytes, temp_ether_addr, 6);

	return retval;
}

static int read_str(char *line, void *arg)
{
	char **dest = arg;
	int i;
	
	if (*dest) free(*dest);

	*dest = strdup(line);
	
	/* elimate trailing whitespace */
	for (i = strlen(*dest) - 1; i > 0 && isspace((*dest)[i]); i--);
	(*dest)[i > 0 ? i + 1 : 0] = '\0';
	return 1;
}


static int read_u32(char *line, void *arg)
{
	u_int32_t *dest = arg;
	*dest = strtoul(line, NULL, 0);
	return 1;
}


static int read_yn(char *line, void *arg)
{
	char *dest = arg;
	if (!strcasecmp("yes", line) || !strcmp("1", line) || !strcasecmp("true", line))
		*dest = 1;
	else if (!strcasecmp("no", line) || !strcmp("0", line) || !strcasecmp("false", line))
		*dest = 0;
	else return 0;
	
	return 1;
}


/* read a dhcp option and add it to opt_list */
static int read_opt(char *line, void *arg)
{
	struct option_set **opt_list = arg;
	char *opt, *val;
	char fail;
	struct dhcp_option *option = NULL;
	int length = 0;
	char buffer[255];
	u_int16_t result_u16;
	int16_t result_s16;
	u_int32_t result_u32;
	int32_t result_s32;
	
	int i;
	
	if (!(opt = strtok(line, " \t="))) return 0;
	
	for (i = 0; options[i].code; i++)
		if (!strcmp(options[i].name, opt)) {
			option = &(options[i]);
			break;
		}
		
	if (!option) return 0;
	
	do {
		val = strtok(NULL, ", \t");
		if (val) {
			fail = 0;
			length = 0;
			switch (option->flags & TYPE_MASK) {
			case OPTION_IP:
				read_ip(val, buffer);
				break;
			case OPTION_IP_PAIR:
				read_ip(val, buffer);
				if ((val = strtok(NULL, ", \t/-")))
					read_ip(val, buffer + 4);
				else fail = 1;
				break;
			case OPTION_STRING:
				length = strlen(val);
				if (length > 254) length = 254;
				memcpy(buffer, val, length);
				break;
			case OPTION_BOOLEAN:
				if (!read_yn(val, buffer)) fail = 1;
				break;
			case OPTION_U8:
				buffer[0] = strtoul(val, NULL, 0);
				break;
			case OPTION_U16:
				result_u16 = htons(strtoul(val, NULL, 0));
				memcpy(buffer, &result_u16, 2);
				break;
			case OPTION_S16:
				result_s16 = htons(strtol(val, NULL, 0));
				memcpy(buffer, &result_s16, 2);
				break;
			case OPTION_U32:
				result_u32 = htonl(strtoul(val, NULL, 0));
				memcpy(buffer, &result_u32, 4);
				break;
			case OPTION_S32:
				result_s32 = htonl(strtol(val, NULL, 0));	
				memcpy(buffer, &result_s32, 4);
				break;
			default:
				break;
			}
			length += option_lengths[option->flags & TYPE_MASK];
			if (!fail)
				attach_option(opt_list, option, buffer, length);
		} else fail = 1;
	} while (!fail && option->flags & OPTION_LIST);
	return 1;
}

//For static IP lease
static int read_staticlease(const char *const_line, void *arg)
{

	char *line;
	char *mac_string;
	char *ip_string;
	uint8_t *mac_bytes;
	uint32_t *ip;


	/* Allocate memory for addresses */
	mac_bytes = xmalloc(sizeof(unsigned char) * 8);
	ip = xmalloc(sizeof(uint32_t));

	/* Read mac */
	line = (char *) const_line;
	mac_string = strtok(line, " \t");
	read_mac(mac_string, mac_bytes);

	/* Read ip */
	ip_string = strtok(NULL, " \t");
	read_ip(ip_string, ip);

	addStaticLease(arg, mac_bytes, ip);

#ifdef UDHCP_DEBUG
	printStaticLeases(arg);
#endif

	return 1;

}


static struct config_keyword keywords[] = {
	/* keyword	handler   variable address		default */
	{"start",	read_ip,  &(server_config.start),	"192.168.0.20"},
	{"end",		read_ip,  &(server_config.end),		"192.168.0.254"},
	{"interface",	read_str, &(server_config.interface),	"eth0"},
	{"option",	read_opt, &(server_config.options),	""},
	{"opt",		read_opt, &(server_config.options),	""},
	{"max_leases",	read_u32, &(server_config.max_leases),	"254"},
	{"remaining",	read_yn,  &(server_config.remaining),	"yes"},
	{"auto_time",	read_u32, &(server_config.auto_time),	"7200"},
	{"decline_time",read_u32, &(server_config.decline_time),"3600"},
	{"conflict_time",read_u32,&(server_config.conflict_time),"3600"},
	{"offer_time",	read_u32, &(server_config.offer_time),	"60"},
	{"min_lease",	read_u32, &(server_config.min_lease),	"60"},
	{"lease_file",	read_str, &(server_config.lease_file),	"/etc/udhcpd.leases"},
	{"pidfile",	read_str, &(server_config.pidfile),	"/var/run/udhcpd.pid"},
	{"notify_file", read_str, &(server_config.notify_file),	""},
	{"siaddr",	read_ip,  &(server_config.siaddr),	"0.0.0.0"},
	{"sname",	read_str, &(server_config.sname),	""},
	{"boot_file",	read_str, &(server_config.boot_file),	""},
	//For static IP lease
	{"static_lease",read_staticlease, &(server_config.static_leases),	""},
	// BRCM
	{"vendorid",read_str, &(server_config.vendorid),	""},
	{"decline_file",read_str, &(server_config.decline_file),""},
	{"",		NULL, 	  NULL,				""}
};


int read_config(char *file)
{
	FILE *in;
	char buffer[80], *token, *line;
	int i;

	for (i = 0; strlen(keywords[i].keyword); i++)
		if (strlen(keywords[i].def))
			keywords[i].handler(keywords[i].def, keywords[i].var);

	if (!(in = fopen(file, "r"))) {
		LOG(LOG_ERR, "unable to open config file: %s", file);
		return 0;
	}
	
	while (fgets(buffer, 80, in)) {
		if (strchr(buffer, '\n')) *(strchr(buffer, '\n')) = '\0';
		if (strchr(buffer, '#')) *(strchr(buffer, '#')) = '\0';
		token = buffer + strspn(buffer, " \t");
		if (*token == '\0') continue;
		line = token + strcspn(token, " \t=");
		if (*line == '\0') continue;
		*line = '\0';
		line++;
		line = line + strspn(line, " \t=");
		if (*line == '\0') continue;
		
		
		
		for (i = 0; strlen(keywords[i].keyword); i++)
			if (!strcasecmp(token, keywords[i].keyword))
				keywords[i].handler(line, keywords[i].var);
	}
	fclose(in);
	return 1;
}


/* the dummy var is here so this can be a signal handler */
void write_leases(int dummy)
{
	FILE *fp;
	unsigned int i;
	char buf[255];
	time_t curr = time(0);
	unsigned long lease_time;
	
	dummy = 0;
	
	if (!(fp = fopen(server_config.lease_file, "w"))) {
		LOG(LOG_ERR, "Unable to open %s for writing", server_config.lease_file);
		return;
	}
	
	for (i = 0; i < server_config.max_leases; i++) {
		if (leases[i].yiaddr != 0) {
			if (server_config.remaining) {
				if (lease_expired(&(leases[i])))
					lease_time = 0;
				else lease_time = leases[i].expires - curr;
			} else lease_time = leases[i].expires;
			lease_time = htonl(lease_time);
			fwrite(leases[i].chaddr, 16, 1, fp);
			fwrite(&(leases[i].yiaddr), 4, 1, fp);
			fwrite(&lease_time, 4, 1, fp);
			fwrite(leases[i].hostname, 64, 1, fp);
		}
	}
	fclose(fp);
	
	if (server_config.notify_file) {
		sprintf(buf, "%s %s", server_config.notify_file, server_config.lease_file);
		system(buf);
	}
}


void read_leases(char *file)
{
	FILE *fp;
	unsigned int i = 0;
	time_t curr = time(0);
	struct dhcpOfferedAddr lease;
	
	if (!(fp = fopen(file, "r"))) {
		LOG(LOG_ERR, "Unable to open %s for reading", file);
		return;
	}
	
	while (i < server_config.max_leases && (fread(&lease, sizeof lease, 1, fp) == 1)) {
		if (lease.yiaddr >= server_config.start && lease.yiaddr <= server_config.end) {
			leases[i].yiaddr = lease.yiaddr;
			leases[i].expires = ntohl(lease.expires);	
			if (server_config.remaining) leases[i].expires += curr;
			memcpy(leases[i].chaddr, lease.chaddr, sizeof(lease.chaddr));
			i++;
		}
	}
	
	DEBUG(LOG_INFO, "Read %d leases", i);
	
	if (i == server_config.max_leases) {
		if (fgetc(fp) == EOF)
			/* might be helpfull to drop expired leases */
			LOG(LOG_WARNING, "Too many leases while loading %s\n", file);
	}
	fclose(fp);
}
		
// BRCM_begin
/* the dummy var is here so this can be a signal handler */
void write_decline(int dummy)
{
	FILE *fp;
        int   f;
	
	dummy = 0;
	if (!(fp = fopen(server_config.decline_file, "w"))) {
		LOG(LOG_ERR, "Unable to open %s for writing", server_config.lease_file);
		return;
	}

	fwrite(declines->chaddr, 16, 1, fp);
	fwrite(declines->vendorid, 64, 1, fp);
	fclose(fp);
	if( (f = open( "/dev/brcmboard", O_RDWR )) != -1 )
	{
		ioctl( f, BOARD_IOCTL_WAKEUP_MONITOR_TASK, NULL);
		close(f);
	}

}

int read_vendor_id_config(char *file)
{
	FILE *in;
	char buffer[64];
	int i = 0;

	if (!(in = fopen(file, "r"))) {
		LOG(LOG_ERR, "unable to open config file: %s", file);
		return 0;
	}
	
	memset(buffer, 0, 64);
        for (i = 0; i < MAX_VENDOR_IDS; i++) {
		memset(vendor_id_config[i].vendorid, 0, 64);
        }
        i = 0;
	while (fgets(buffer, 64, in)) {
		if (i < MAX_VENDOR_IDS) {
			memset(vendor_id_config[i].vendorid, 0, 64);
			if (buffer[strlen(buffer)-1] == '\n') {
				buffer[strlen(buffer)-1] = '\0';
			}
			memcpy(vendor_id_config[i].vendorid, buffer, strlen(buffer));
			i++;
			memset(buffer, 0, 64);
		} else {
			break;
		}
	}
	fclose(in);
        //unlink(file);
	return 1;
}

/* get signal to write viTable to file */
void write_viTable(int dummy)
{
	FILE *fp;
	int count;
	pVI_OPTION_INFO pPtr=NULL;
	struct dhcpOfferedAddr *lease = NULL;

	if (!(fp = fopen("/var/udhcpd/managable.device", "w+"))) {
		LOG(LOG_ERR, "Unable to open %s for writing", "/var/udhcpd/managable.device");
		return;
	}
	count = viList->count;
	fprintf(fp,"NumberOfDevices %d\n",count);
	if (count > 0) {
	  pPtr = viList->pHead;
	  while (pPtr) {
	    if ((lease = find_lease_by_yiaddr(pPtr->ipAddr)) &&
		lease_expired(lease)) {
	      strcpy(pPtr->oui,"");
	      strcpy(pPtr->serialNumber,"");
	      strcpy(pPtr->productClass,"");
	    }
	    fprintf(fp,"IPaddr %x Enterprise %d OUI %s SerialNumber %s ProductClass %s\n",
		    pPtr->ipAddr,pPtr->enterprise,pPtr->oui,pPtr->serialNumber,
		    pPtr->productClass);
	    pPtr = pPtr->next;
	  }
	}
	fclose(fp);
}
// BRCM_end

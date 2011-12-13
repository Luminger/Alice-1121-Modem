/*
 * Project: ReAim
 *
 * Release: 0.8 pre
 *
 * Homepage:  http://reaim.sourceforge.net/
 *
 * Copyright: Mark Cooke, November 2001.  All Rights Reserved.
 * 
 * License: GNU General Public License, Version 2.
 *
 * CVS Data: $Id: reaim.c,v 1.30 2003/03/25 12:35:31 mark-c Exp $
 *
 * Description:
 *
 * ReAIM is a simple transparent AIM proxy.  It sits between a local area
 * network, and the internet, usually running on the firewall / NAT server.
 *
 * It also has a filter module for MSN Messenger file transfer support.
 *
 * It accepts transparently redirected port 5190 (AIM server communications)
 * traffic, and transparently forwards this to the usual destination with one
 * caveat.
 *
 * The direct-communications setup packet is mangled to use the (routable)
 * internet-visible IP address, rather than the client's NAT / private network
 * address.
 *
 * Given the change in more recent clients to allow the user to set a source
 * port, instead of just using 4443/5566, the proxy also forces the port to
 * be 4443 as part of the DCC address mangling.
 *
 * This allows the second part of this proxy - a port 4443/5566
 * forwarding service - to catch the remote party DCC connection, and forward
 * it to the internal host.  It is slightly cludgy, as the AIM clients do not
 * send a 'hello, it's me' packet when they direct-connect, so the proxy can't
 * tell where to forward the connection to until the remote party sends a
 * message.  Fortunately, just starting to type sends a message we can use
 * to make the onward part of the proxied connection.
 *
 * To make this less cludgy, it is possible to open a range of incoming ports,
 * and for the proxy to use the next unused port in this range for each DCC.
 * This is a 'TODO' high on my list of things to do to ReAIM.
 *
 * As a compile option, setting LAX_SECURITY will allow a DCC to match at the
 * initial connection point if there is only 1 pending DCC request.
 * 
 * As a compile option, setting LOOSE_CLIENT_IP will disable the paranoid
 * checking that the IP address being overwritten in the DCC setup packet is
 * the same as the source IP address of the client socket.  This allows
 * multi-homed hosts to work, and also network topologies that include
 * multiple-NAT devices.
 *
 * Usage:
 *
 * reaim [optional dotted quad for routable IP]
 *
 * Notes:
 *
 * This proxy requires knowledge of the external IP address, and also for the
 * original destination to be available.  Using iptables allows us to use
 * SO_ORIGINAL_DST to retrieve this.  Hence, this proxy is a 2.4.x / iptables
 * only proxy.  More specifically, it does -not- work with 2.4.x / ipchains
 * support.  Suitable modifications to the gettranssockname function may
 * permit this to work with ipchains on 2.2.x, where the getsockname() call
 * is overridden to be the original destination (not confirmed).
 *
 * Example usage where eth0 is LAN, ppp0 is internet:
 *
 * 1. Redirect eth0 traffic to eth0 port 5190, and make sure it's accepted.
 * iptables -t nat    -A PREROUTING -i eth0 -p tcp --dport 5190 -j REDIRECT --to-ports 5190
 * iptables -t filter -A INPUT      -i eth0 -p tcp --dport 5190 -j ACCEPT
 *
 * 2. Allow ppp0 to accept traffic for port 4443 (AIM DCC)
 * iptables -t filter -A INPUT      -i ppp0 -p tcp --dport 4443 -j ACCEPT
 *
 * 3. Allow ppp0 to accept traffic for port 5566 (AIM Sharing)
 * iptables -t filter -A INPUT      -i ppp0 -p tcp --dport 5566 -j ACCEPT
 *
 * 4. Redirect eth0 traffic to eth0 port 5190, and make sure it's accepted.
 * iptables -t nat    -A PREROUTING -i eth0 -p tcp --dport 1863 -j REDIRECT --to-ports 1863
 * iptables -t filter -A INPUT      -i eth0 -p tcp --dport 1863 -j ACCEPT
 *
 * 5. Allow ppp0 to accept traffic for port 1864 (MSN DCC)
 * iptables -t filter -A INPUT      -i ppp0 -p tcp --dport 1864 -j ACCEPT
 *
 * 6. Allow dynamic dcc traffic on ports 40000 - 40099
 * iptables -t filter -A INPUT      -i ppp0 -p tcp --dport 40000:40099 -j ACCEPT
 *
 * Detailed AIM Manglement Notes:
 *
 * The proxy keeps three lists of connections (socket pairs).
 *
 * 1. LAN clients connections to port 5190 (and the onward connection).
 * 2. Established connections on port 4443 (and the onward connection).
 * 3. Established connections on port 5566 (and the onward connection).
 *
 * When a client starts a connection, the proxy accepts the redirected
 * connection on it's port 5190.  It immediately attempts to open the
 * onward connection to the original destination, and any packets arriving
 * in either direction are inspected, optionally modified, and then passed
 * to the other half of the socket-pair.
 *
 * When a packet indicates a DC setup request, an entry is made in the
 * pending DC setup list, and appropriate parameters from the request are
 * recorded.  Note that the DC setup request is -mangled- to use the
 * externally addressable IP address, and a dynamic port, so the remote
 * party isn't trying to connect to a 10.x.x.x etc IP address.
 *
 * When a connection is received on the dynamic port (or 4443 for buggy
 * clients), the proxy accepts the connection, and searches for a match
 * in the pending DC list.  If a match is found, the onward portion of
 * the DC is established, and unfiltered direct communications between
 * the parties is established.
 *
 * Connections to port 5566 are handled virtually identically.
 *
 * DC Setup Packet Format:
 *
 * With many thanks to the people working on the reverse engineering of
 * the AIM OSCAR protocol.
 *
 *
 * EXTRA...EXTRA...
 *
 * Because of the nice flexibility of the filter scheme, I added support for
 * MSN proxying too.  The MSN Messenger uses a pseudo HTTP style scheme, with
 * headers indicating the message type, parameters, etc.
 *
 * For MSN DCC setup, we look for the text/xmsmsgsinvite message, and 'fix'
 * both the port and IP address, again with a dynamic dcc port from our range.
 *
 * TODO:
 *
 * A whole lot.  This is currently little more than a complete hack that
 * happens to work for me.  I'll be working on the code to do more error
 * checking, structure clean up, as time permits.
 *
 * I have plans to add a 'filter chain' type system to both the 5190 and 4443
 * port proxies, and eventually maybe generalise the code to be a more
 * generic, open source licensed, proxy-based firewalling system...maybe!
 *
 * Final words:
 *
 * No warranty.  Patches, bug reports, success stories, and other contributions
 * most welcome.
 */

/* NOTE:  NetBSD patch provided by David Vyskocil.  Thanks!!!
 *
 * Compile with USE_IPF defined for NetBSD/IPF support.
 */

 /* NetBSD proxy redirect(ipnat) and firewall(ipf) setup
 *
 * assuming 'ne0' is the LAN device name and 'tun0' the internet connection
 *
 * /etc/ipnat.conf rules... (redirect for transparent proxying)
 *
 * rdr ne0 0/0 port 1863 -> 127.0.0.1 port 1863 tcp
 * rdr ne0 0/0 port 5190 -> 127.0.0.1 port 5190 tcp
 *
 * /etc/ipf.conf rules... (firewall/logger)
 *
 * pass in quick log on ne0 proto tcp from any to any port = 1863
 * pass in quick log on ne0 proto tcp from any to any port = 5190
 * pass in quick log on tun0 proto tcp from any to any port = 1864
 * pass in quick log on tun0 proto tcp from any to any port = 4443
 * pass in quick log on tun0 proto tcp from any to any port = 5566
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <ctype.h>

#include <getopt.h>
#include <sys/ioctl.h>

#include <unistd.h>

// Command line options
#define _GNU_SOURCE
#include <getopt.h>

static struct option long_options[] = {

  { "disable-msn", 1, 0, 'm' },
  { "disable-aim", 1, 0, 'a' },
  { "external-ip", 1, 0, 'e' },
  { "help",        1, 0, 'h' },
  { "logfile",     1, 0, 'l' },
  { 0,0,0,0 }
};

#ifdef USE_IPF

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_compat.h>
#include <netinet/ip_fil.h>
#include <netinet/ip_nat.h>
#define PLATFORM "/IPF"

#else

// Nasty kernel include in a userspace program
// but we need the original destination information....
#include <linux/netfilter_ipv4.h>
#define PLATFORM "/NetFilter"

#endif

#define REAIM_VERSION "0.8 pre"

#define OSCAR 5190
#define DCC   4443
#define SHARE 5566

#define MSN     1863   // Where we listen for client messages
#define MSNRX   1864   // Where we listen for MSN DCCs

#define INC_MIN     40000   // Minimum port for incoming connections
#define INC_MAX     40099   // Maximum port for incoming connections
#define INC_TIMEOUT 60      // 60 second timeout for dynamic DCCs

#define LAX_SECURITY     1
#define LOOSE_CLIENT_IP  1
#define LOOSE_PORT_SCAN  0
#define DEBUG_PKT_DUMPS  1
#define USE_DYNAMIC_DCC  1
#define PERIODIC_LOG     1

int enable_msn  = 1;
int enable_aim  = 1;
int enable_fork = 1;
// BRCM begin
int enable_log  = 0;
// BRCM end

// Trillian headers for MSN support don't end with two \n\r pairs,
// but only one, so are not distinguishable from the message easily.
// Define this to skip the strict end-of-headers check.
#define TRILLIAN_HEADER_SUPPORT 1

#define TS_CLIENT 0
#define TS_SERVER 1

// This defines the size of the buffers used when proxying
// connections.  Each proxied connection uses 2 of these
// buffers.
#define PROXY_BUFFER_SIZE   4096

#ifndef LOG_FILE
#define LOG_FILE "/var/log/reaim.log"
#endif

char *log_file = LOG_FILE;

// Filter group IDs - needed for stateful storage access
#define GRP_AIM    1
#define GRP_ICQ    2
#define GRP_MSN    3

// Set to 1 to enable additional logging for debugging purposes
#define VERBOSE_LOGGING 0

// Timeout for making onward connections
#define CONNECT_TIMEOUT 10

// Maximum number of filters on each open socket.
// TODO: remove this coded limit
#define MAX_FILTERS 4

// Logging routine.  Borrowed from the squirm proxy.
int write_log(const char *logfile, const char *format, ...);


typedef struct _filter_chain           filter_chain;
typedef struct _filter_state           filter_state;
typedef struct _proxy_track_struct     proxy_track_struct;
typedef struct _user_track_struct      user_track_struct;
typedef struct _pending_dc_struct      pending_dc_struct;
typedef struct _listen_track_struct    listen_track_struct;
typedef struct _pending_connect_struct pending_connect_struct;

/*----------------------------------------------------------------------------
  The filter scheme for the code is a simple procedure that takes a proxy
  data buffer, current and maximum length, and the connection tracking
  structure.

  Each filter is called sequentially, and can mangle the provided buffer.
----------------------------------------------------------------------------*/
typedef unsigned int (filter_proc)(unsigned char *buff,
				   unsigned int buff_len,
				   unsigned int max_len,
				   proxy_track_struct *pt);

typedef void (listen_proc)(listen_track_struct *lt);

typedef void (connect_proc)(pending_connect_struct *pc);


struct _filter_chain {
    filter_proc    *proc[MAX_FILTERS]; // Procedures for filtering
};

struct _filter_state {
    filter_state   *next;         // Next state storage
    unsigned int    filter_group; // Filter ID number
    void           *group_state;  // Persistent state
};

/*
 * Generic proxied socket handler.
 *
 */
struct _proxy_track_struct {
    proxy_track_struct *next;  // Next proxy connection in linked list.
    
    unsigned int magic;        // Magic number linking other lists.
    unsigned int cleanup;      // Set to 1 to garbage collect this entry.
    unsigned int user;         // Set to 1 if user connection.
    
    int sockets[2];                  // The rx / tx sockets
    unsigned char *data_buff[2];     // Receive/Transmit pending data buffers
    unsigned int   data_offset[2];   // Offset to start of data in buffers
    unsigned int   data_len[2];      // Remaining bytes in pending buffers
    unsigned int   data_max[2];      // Buffer sizes
    filter_chain  *filter[2];        // Attached filters
    filter_state   state;            // Stateful storage buffers
};

struct  _user_track_struct {
    user_track_struct *next; // Next entry in the list

    unsigned int magic;      // Magic connection tracking number
    unsigned int cleanup;    // Set to 1 to clean up this entry

    unsigned char *msg;      // Connection setup message
    unsigned char *user;     // Screen name of the user
};

struct _pending_dc_struct {

    pending_dc_struct *next; // Next pending connection
    
    unsigned int cleanup;    // Set to 1 to clean up this entry

    unsigned short port;     // Port we're listenning on.
    unsigned short orig_port;// Original port the client was on.

    unsigned int magic;      // The user_track_struct magic number
    unsigned char id[8];     // The DC setup message we're waiting on

    struct sockaddr_in src;  // Original Client Source address
};

struct _listen_track_struct {
    
    listen_track_struct *next; // Next listenning socket
    listen_proc         *proc; // Procedure to call
    
    unsigned int      cleanup; // 1 if inactive
    int                  port; // Port this is listenning on.
    int                socket; // Listening socket handle.
    int               oneshot; // 1 if one-shot listener.
    long              timeout; // One-shot timeout (0 is infinite)
};

struct _pending_connect_struct {
  pending_connect_struct *next;  // Next pending connection
  unsigned int magic;            // user_track_struct magic number
  int          socket;           // Socket handle
  int          client_sock;      // Client socket
  connect_proc *proc;            // Bottom half handler
  void         *proc_data;       // Data for the bottom half handler
  filter_chain *filter[2];       // Establish connection filters
  long          timeout;         // Timeout time
  struct sockaddr_in src;        // Source address
  struct sockaddr_in dst;        // Destination address
};

#define AIM_CMDSTART(buff)      buff[0]
#define AIM_CHANNEL(buff)       buff[1]
#define AIM_SEQID(buff)         ((unsigned short) buff[2]  + (buff[3] << 8))
#define AIM_FNAC_LEN(buff)      ((unsigned short) buff[5]  + (buff[4] << 8))
#define AIM_FNAC_FAMILY(buff)   ((unsigned short) buff[7]  + (buff[6] << 8))
#define AIM_FNAC_SUBTYPE(buff)  ((unsigned short) buff[9]  + (buff[8] << 8))
#define AIM_FNAC_FLAGS(buff)    ((unsigned short) buff[11] + (buff[10] << 8))
#define AIM_SNAC_ID(buff)       ((unsigned int)   buff[15] + (buff[14] << 8) + (buff[13] << 16) + (buff[12] << 24))
#define AIM_SNAC_COOKIE(buff)   (&buff[16])   // The 8 byte SNAC cookie start ptr
#define AIM_SNAC_CHAN(buff)     ((unsigned short) buff[25] + 256*buff[24])
#define AIM_BUD_NAME_LEN(buff)  buff[26]
#define AIM_BUD_NAME(buff)      (&buff[27])
#define AIM_BUD_NEXT(buff)      (27+AIM_BUD_NAME_LEN(buff))
#define AIM_SNAC_TLV_TYPE(buff) ((unsigned short) buff[AIM_BUD_NEXT(buff)+1] + (buff[AIM_BUD_NEXT(buff)+0] << 8))
#define AIM_SNAC_TLV_NEXT(buff) ((unsigned short) buff[AIM_BUD_NEXT(buff)+3] + (buff[AIM_BUD_NEXT(buff)+2] << 8))
#define AIM_SNAC_DATA_OFF(buff) (AIM_BUD_NEXT(buff)+4)
#define DCC_SNAC_COOKIE(buff)   (&buff[12])
#define DCC_BUD_NAME(buff)      (&buff[44])  // Appears to be null terminated this time...

/*===========================================================================*/
// The linked lists used to track state
/*===========================================================================*/

proxy_track_struct pt_list;       // Generic proxy socket list-head.
user_track_struct  user_list;     // List head.  User tracking information
listen_track_struct lt_list;      // List head.  Listenning sockets
pending_connect_struct pc_list;   // List head.  Pending connect()'s
pending_dc_struct  dc_list;       // List head.  Pending incoming connections

unsigned int       ts_magic        = 1; // Next free magic number

char *             outside_ip      = NULL;

filter_chain       oscar_tx;   // LAN client to AIM filter
filter_chain       oscar_rx;   // AIM to LAN client filter
filter_chain       share_tx;   // LAN to Remote filter
filter_chain       share_rx;   // Remote to LAN filter
filter_chain       msn_tx;     // LAN to Remote filter
filter_chain       msn_rx;     // Remote to LAN filter
filter_chain       msnrx_tx;   // LAN to Remote filter
filter_chain       msnrx_rx;   // Remote to LAN filter

static int pending_hup = 0; // Not zero if a HUP was received.

listen_track_struct* create_listener_between(int port_min, int port_max, listen_proc lp);

/*===========================================================================*/


#ifdef USE_IPF

/* 
 * We use IP Filter on NetBSD to track original destination 
 *
 * Descriptions: use ioctl for /dev/ipnat to extract original destination
 *               from kernel.
 *    Arguments: (socket file descriptor, 
 *                current destination address, 
 *                the length)
 * Side Effects: replace sa to original destination
 * Return Value: 0 (success) or -1 (failure)
 */
int
get_realip_from_ipf(fd, sa, salen)
     int fd;
     struct sockaddr *sa;
     int *salen;
{

    struct	sockaddr_in	sin, sloc;
    struct	sockaddr_in	*sinp;
    natlookup_t	natlook;
    natlookup_t	*natlookp = &natlook;
    int	namelen, natfd;
    /*  
    * get IP# and port # of the remote end of the connection (at the
    * origin).
    */
    namelen = sizeof(sin);
    if (getpeername(fd, (struct sockaddr *)&sin, &namelen) == -1) {
	perror("getpeername");
	write_log(log_file, "getpeername failed: %s", strerror(errno));
	exit(-1);
	}
    /*
    * get IP# and port # of the local end of the connection (at the
    * man-in-the-middle).
    */
    namelen = sizeof(sin);
    if (getsockname(fd, (struct sockaddr *)&sloc, &namelen) == -1) {
	perror("getsockname");
	write_log(log_file, "getsockname failed: %s", strerror(errno));
	exit(-1);
	}
    /*
    * Build up the NAT natlookup structure.
    */
    bzero((char *)&natlook, sizeof(natlook));
    natlook.nl_outip = sin.sin_addr;
    natlook.nl_inip = sloc.sin_addr;
    natlook.nl_flags = IPN_TCP;
    natlook.nl_outport = sin.sin_port;
    natlook.nl_inport = sloc.sin_port;
    /*
    * Open the NAT device and lookup the mapping pair.
    */
    natfd = open(IPL_NAT, O_RDONLY);
    if (ioctl(natfd, SIOCGNATL, &natlookp) == -1) {
	perror("ioctl(SIOCGNATL)");
	write_log(log_file, "ioctl(SIOCGNATL) failed: %s", strerror(errno));
	close(natfd);
	exit(-1);
	}
    close(natfd);
/*
    fprintf(stderr, "from addr:%s port:%d\n", inet_ntoa(natlook.nl_inip), ntohs(natlook.nl_inport));
    fprintf(stderr, "as   addr:%s port:%d\n", inet_ntoa(natlook.nl_outip), ntohs(natlook.nl_outport));
    fprintf(stderr, "real addr:%s port:%d\n", inet_ntoa(natlook.nl_realip), ntohs(natlook.nl_realport));
*/
    sinp = (struct sockaddr_in *) sa;
    sinp->sin_addr = natlookp->nl_realip;
    sinp->sin_port = natlookp->nl_realport;
    sinp->sin_family = AF_INET;
    return(0);
}

#endif

/*===========================================================================*
 *  int gettranssockname(int fd, struct sockaddr *sa, socklen_t *salen)
 *
 *    Query the network stack for the original destination of the socket
 *    passed.  This is used for building the onward portion of the filter
 *    proxy.
 *===========================================================================*/

int gettranssockname(int fd, struct sockaddr *sa, socklen_t *salen)
{
    if (*salen != sizeof(struct sockaddr_in)) {
	errno = EINVAL;
	return -1;
    }
    
    // Try to ask for the original destination...
#ifdef USE_IPF
    if (!get_realip_from_ipf(fd, sa, salen))
	return 0;
#else
    if (!getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, sa, salen))
	return 0;
#endif
    
    // We have to explode if we can't read it back.
    return -1;
}

/*===========================================================================*
 *  void dump_packet(const char *buff, int len)
 *
 *    Print out the contents of the packet.
 *===========================================================================*/
void dump_packet(const char *buff, int len)
{
    char dump_start[32768], *dump_buff = dump_start;
    int i, j;

    for (i = 0; i < len; i ++) {
	
	if ((i % 16) == 0) {
	    sprintf(dump_buff, "%.4X    ", i);
	    dump_buff += 8;
	}
	
	sprintf(dump_buff, "%02x ", (unsigned int) buff[i] & 0xff);
	dump_buff += 3;
	
	if ((i % 16) == 15) {
	    
	    sprintf(dump_buff, "    ");
	    dump_buff += 4;
	    
	    for (j = -15; j < 1; j++) {
		if (isalnum(buff[i+j]))
		    *dump_buff = buff[i+j];
		else
		    *dump_buff = '.';

		dump_buff ++;
	    }
	    
	    *dump_buff++ = '\n';
	}
    }
    *dump_buff++ = '\n';
    *dump_buff++ = '\0';

    write_log(log_file, "Packet Dump:\n%s", dump_start);
}


/*===========================================================================*
 *  void dump_packet(const char *buff, int len)
 *
 *    Print out the contents of the packet.
 *===========================================================================*/

int write_log(const char *logfile, const char *format, ...)
{
    va_list args;
    FILE *log_handle;
    char *date_str;
    char msg_buff[32768];
    time_t current_time;
    
    va_start(args, format);
    
    if (vsprintf(msg_buff, format, args) > 32767)
	strcpy(msg_buff, "[WARNING] Insufficient log buffer size");
    
    va_end(args);
    
    current_time = time(NULL);
    date_str = ctime(&current_time);
    
// BRCM begin
// Only write to log file when it's enabled, otherwise write to
// stderr when enable_fork == 0, otherwise throw it away.
    if (enable_log) {
       log_handle = fopen(logfile, "at");
       if (log_handle == NULL) {
          if (enable_fork == 0)
	  fprintf(stderr, "[%.*s] %s\n", (int) strlen(date_str) - 1, date_str, msg_buff);
          return 0;
       }
    
       // strlen - 1 to kill the trailing \n in the data string.
       fprintf(log_handle, "[%.*s] %s\n", (int) strlen(date_str) - 1, date_str, msg_buff);
       fclose(log_handle);
       return 1;
   }
   else {
       if (enable_fork == 0)
          fprintf(stderr, "[%.*s] %s\n", (int) strlen(date_str) - 1, date_str, msg_buff);
       return 0;
   }
// BRCM end
}

#if PERIODIC_LOG
/*===========================================================================*
 *  void periodic_log()
 *
 *  Print out some stats to the reaim log periodically.
 *===========================================================================*/

void periodic_log() {

  int n_pc, n_pt, n_lt, n_pdc;
  listen_track_struct *lt;
  pending_dc_struct *pdc;
  proxy_track_struct *pt;
  pending_connect_struct *pc;
  
  // Count listen_track_struct
  n_lt = 0;
  lt = lt_list.next;
  while(lt != NULL) {
    n_lt ++;
    lt = lt->next;
  }

  // Count pending_dc_struct
  n_pdc = 0;
  pdc = dc_list.next;
  while(pdc != NULL) {
    n_pdc ++;
    pdc = pdc->next;
  }

  // Count proxy_track_struct
  n_pt = 0;
  pt = pt_list.next;
  while(pt != NULL) {
    n_pt ++;
    pt = pt->next;
  }

  // Count pending_connect_struct
  n_pc = 0;
  pc = pc_list.next;
  while(pc != NULL) {
    n_pc ++;
    pc = pc->next;
  }

  write_log(log_file, "[STATS] Proxied Connections: %d, Pending DCCs: %d, Pending connect()s: %d, Listening Sockets: %d",
	    n_pt, n_pdc, n_pc, n_lt);
}

#endif

/*===========================================================================*
 *  int net_listen(int port, char *ifs)
 *
 *    Open a network socket listenning on the port requested.  This function
 *    returns the socket handle, or -1 if the socket could not be openned for
 *    some reason.
 *
 *    The function binds to the address specified in the ifs parameter.
 *===========================================================================*/

int net_listen(int port, const char *ifs)
{
    int hSock;
    struct sockaddr_in server;
    int option;
    
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == -1) {
        write_log(log_file, "[FATAL] Could create a socket for %s:%d",
		  ifs, port);
	return -1;
    }
    
    option = 1;
    if (setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (void*) &option,
		   sizeof(option)) < 0) {
        write_log(log_file, "[FATAL] Could not set socket options for %s:%d",
		  ifs, port);
	return -1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ifs);
    server.sin_port = htons(port);
    if (bind(hSock, (struct sockaddr*) &server, sizeof(server)) == -1) {
        write_log(log_file, "[FATAL] Could not create listening socket on %s:%d",
		  ifs, port);
	close(hSock);
	return -1;
    }
    
    if (listen(hSock, 5) == -1) {
        write_log(log_file, "[FATAL] Could not listen on %s:%d",
		  ifs, port);
	close(hSock);
	return -1;
    }
    
    /* Set the socket to non blocking, and close it if we can't */
    if (fcntl (hSock, F_SETFL, O_NONBLOCK) == -1) {
        write_log(log_file, "[FATAL] Could not make socket non-blocking for %s:%d",
		  ifs, port);
	close(hSock);
	return -1;
    }
    
    return hSock;
}

/*===========================================================================*
 *  int net_listen_between(int port_min, int port_max)
 *
 *    Open a network socket listenning on a port in the range requested.
 *    This function returns the socket handle, or -1 if the socket could not
 *    be openned for some reason.
 *
 *    The function binds to the address specified in the ifs parameter, and
 *    returns the port bound.
 *===========================================================================*/
int net_listen_between(int port_min, int port_max, const char *ifs, int *port)
{
    int hSock;
    struct sockaddr_in server;
    int option, i;
    
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == -1) {
        write_log(log_file, "[FATAL] Could create a socket for %s:%d",
		  ifs, port);
	return -1;
    }
    
    option = 1;
    if (setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (void*) &option,
		   sizeof(option)) < 0) {
        write_log(log_file, "[FATAL] Could not set socket options for %s:%d",
		  ifs, port);
	return -1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ifs);
    
    /* Look for an unused port in our permitted range. */
    for (i = port_min ; i <= port_max; i++) {
      server.sin_port = htons(i);
      if (bind(hSock, (struct sockaddr*) &server, sizeof(server)) != -1)
	break;
    }
    
    // Didn't find one.
    if (i > port_max) {
      write_log(log_file, "[FATAL] Could not create listening socket on %s:%d-%d",
		ifs, port_min, port_max);
      close(hSock);
      return -1;
    }
    
    if (listen(hSock, 5) == -1) {
        write_log(log_file, "[FATAL] Could not listen on %s:%d",
		  ifs, port);
	close(hSock);
	return -1;
    }
    
    /* Set the socket to non blocking, and close it if we can't */
    if (fcntl (hSock, F_SETFL, O_NONBLOCK) == -1) {
        write_log(log_file, "[FATAL] Could not make socket non-blocking for %s:%d",
		  ifs, port);
	close(hSock);
	return -1;
    }
    
    // Inform caller of the port we bound to.
    *port = i;
    
    return hSock;
}


/*===========================================================================*
 *  int net_connect_to(int port, const char *ifs)
 *
 *    Open a network socket to the port and dotted-quad address string given.
 *    The returned socket is set to non-blocking and keep-alive if the socket
 *    was created.  -1 is returned on failure.
 *
 * DEPRECIATED: This function can block.  Should be using either
 *              net_connect_outbound_nb or net_connect_inbound_nb
 *===========================================================================*/

int net_connect_to(int port, const char *ifs)
{
    int hSock;
    struct sockaddr_in server;
    int option;
    
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == -1) {
	write_log(log_file, "Connection [%s:%d] failed [create]: %s [%d]",
		  ifs, port, strerror(errno), errno);
	return -1;
    }
    
    option = 1;
    if (setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (void*) &option,
		   sizeof(option)) < 0) {
	write_log(log_file, "Connection [%s:%d] failed [options]: %s [%d]",
		  ifs, port, strerror(errno), errno);
	close(hSock);
	return -1;
    }
    
    option = 1;
    if (setsockopt(hSock, SOL_SOCKET, SO_KEEPALIVE, (void*) &option,
		   sizeof(option)) < 0) {
	write_log(log_file, "Connection [%s:%d] failed [options]: %s [%d]",
		  ifs, port, strerror(errno), errno);
	close(hSock);
	return -1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ifs);
    server.sin_port = htons(port);
    if (connect(hSock, (struct sockaddr*) &server, sizeof(server)) == -1) {
	write_log(log_file, "Connection [%s:%d] failed [conn]: %s [%d]",
		  ifs, port, strerror(errno), errno);
	close(hSock);
	return -1;
    }
    
    /* Set the socket to non blocking, and close it if we can't */
    if (fcntl (hSock, F_SETFL, O_NONBLOCK) == -1) {
	write_log(log_file, "Connection [%s:%d] closed [options]: %s [%d]",
		  ifs, port, strerror(errno), errno);
	close(hSock);
	return -1;
    }
    
    return hSock;
}

/*-------------------------------------------------------------------------
  void* net_open_connection(int hSock)
  
  Open the next connection to the socket specified.  The socket supplied
  must be in a listenning state.  This function returns a newly created
  socket structure with information about the new connection, or -1 if
  the socket could not be openned for some reason.
  
  The returned socket is setup for non-blocking, keep-alive.
-------------------------------------------------------------------------*/
int net_open_connection(int hSock, struct sockaddr_in *addr, int *salen)
{
    int hNewSock;
    int option;
    
    hNewSock = accept(hSock, (struct sockaddr *) addr, salen);
    if (hNewSock == -1) {
	write_log(log_file, "Accept failed [accept]: %s [%d]",
		  strerror(errno), errno);
	return -1;
    }
    
    /* Set the socket to non blocking, and close it if we can't */
    if (fcntl (hNewSock, F_SETFL, O_NONBLOCK) == -1) {
	write_log(log_file, "Accept failed [options] [From %s:%d]: %s [%d]",
		  inet_ntoa(addr->sin_addr), ntohs(addr->sin_port),
		  strerror(errno), errno);
	close(hNewSock);
	return -1;
    }
    
    option = 1;
    if (setsockopt(hNewSock, SOL_SOCKET, SO_KEEPALIVE, (void*) &option,
		   sizeof(option)) < 0) {
	write_log(log_file, "Accept failed [options] [From %s:%d]: %s [%d]",
		  inet_ntoa(addr->sin_addr), ntohs(addr->sin_port),
		  strerror(errno), errno);
	close(hNewSock);
	return -1;
    }
    
    return hNewSock;
}

/*-------------------------------------------------------------------------
  filter_state* find_state(proxy_track_struct *pts,
                           unsigned int  id)
  
  Search the list of filter state structures for the PTS, and return a
  pointer to the appropriate one.

  Return NULL if it does not exist.
-------------------------------------------------------------------------*/

filter_state *find_pts_state(proxy_track_struct *pts, unsigned int group_id)
{
    filter_state *scan;
    
    scan = pts->state.next;
    
    while((scan) && (scan->filter_group != group_id))
	scan = scan->next;
    
    return scan;
}

/*-------------------------------------------------------------------------
  int add_pts_state(proxy_track_struct *pts,
                    unsigned int  id,
	            filter_state *add_state)
  
  Search the list of filter state structures for the PTS, and if a filter
  state structure doesn't already exist, add the provided one.

  Return 0 if the new data was not added.  1 if it was.
-------------------------------------------------------------------------*/

int add_pts_state(proxy_track_struct *pts, unsigned int group_id, filter_state *add_state)
{
    if (find_pts_state(pts, group_id))
	return 0;

    add_state->next = pts->state.next;
    pts->state.next = add_state;

    return 1;
}

/*-------------------------------------------------------------------------
  filter_state* del_pts_state(proxy_track_struct *pts,
                              unsigned int  id)
  
  Search the list of filter state structures for the PTS, and if a filter
  state structure exists, remove it from the list and return it.

  Return NULL if there was no pts data.
-------------------------------------------------------------------------*/

filter_state * del_pts_state(proxy_track_struct *pts, unsigned int group_id)
{
    filter_state *scan, *prev;
    
    prev = &pts->state;
    scan = pts->state.next;
    
    while(scan) {
	
	if (scan->filter_group == group_id) {
	    prev->next = scan->next;
	    return scan;
	}
	
	prev = scan;
	scan = scan->next;
    }
    
    return NULL;
}

/*-------------------------------------------------------------------------
  proxy_track_struct* pts_allocate()
  
  Allocate the buffers, setup counters, sizes, empty filters for a new
  proxy tracking structure.

  Return NULL if we can't allocate memory for some reason.
-------------------------------------------------------------------------*/

proxy_track_struct *pts_allocate()
{
    proxy_track_struct *pt;
    int sock;
    
    // Setup the tracking structure
    pt = calloc(sizeof(proxy_track_struct),1);
    if (!pt) {
	write_log(log_file, "[WARNING] Memory allocation failed for "
		  "proxy tracking structure.");
	return NULL;
    }
    
    pt->next       = NULL;
    pt->cleanup    = 0;
    pt->user       = 0;
    pt->state.next = NULL;
    
    // Setup the new user socket list.
    for (sock = 0; sock < 2; sock ++) {
	pt->sockets[sock]      = -1;
	pt->data_max[sock]     = PROXY_BUFFER_SIZE;
	pt->data_offset[sock]  = 0;
	pt->data_len[sock]     = 0;
	pt->data_buff[sock]    = malloc(pt->data_max[sock]);

	if (!pt->data_buff[sock]) {

	    int fs;
	    
	    for (fs = 0; fs < sock; fs ++)
		free(pt->data_buff[fs]);
	    free(pt);
	    
	    write_log(log_file, "[WARNING] Memory allocation failed for "
		      "proxy tracking data buffers.");
	    
	    return NULL;
	}

	pt->filter[sock] = NULL;
    }
    
    return pt;
}

/*-------------------------------------------------------------------------
  int net_connect_outbound_nb()
  
  Setup an outbound non-blocking connection attempt.

  Return 0 on a failure.  Note that is it upto the caller to free
  proc_data if this function fails.
-------------------------------------------------------------------------*/

int net_connect_outbound_nb(listen_track_struct *lt,
		            connect_proc         proc,
		            void                *proc_data,
		            filter_chain        *tx_filter,
		            filter_chain        *rx_filter,
		            int                  force_dst_port)
{
    int new_client, new_server;
    int salen, option;
    pending_connect_struct *pc;
    char *tmpstr;
    struct timeval tv_now;
    
    // Allocate pending connection tracker
    pc = calloc(sizeof(pending_connect_struct), 1);
    if (!pc) {
      return 0;
    }

    gettimeofday(&tv_now, NULL);

    // Add to the pc list.  This will do garbage collection
    pc->socket      = -1;
    pc->client_sock = -1;
    pc->proc        = proc;
    pc->proc_data   = proc_data;
    pc->filter[0]   = tx_filter;
    pc->filter[1]   = rx_filter;
    pc->timeout     = tv_now.tv_sec + CONNECT_TIMEOUT;
    pc->next        = pc_list.next;
    pc_list.next    = pc;
    
    // Open the pending connection (and log any errors)
    salen = sizeof(pc->src);
    new_client = net_open_connection(lt->socket, &pc->src, &salen);
    if (new_client == -1) {
	return 0;
    }
    
    // Read the original destination...
    salen = sizeof(pc->dst);
    if (gettranssockname(new_client, (struct sockaddr*) &pc->dst, &salen) == -1) {
	write_log(log_file, "[CONN_NB] Connection [%s:%d -> ip.ip.ip.ip:port via proxy] failed "
		  "[Could not determine original destination]: %s [%d]",
		  inet_ntoa(pc->src.sin_addr), ntohs(pc->src.sin_port),
		  strerror(errno), errno);
	close(new_client);
	return 0;
    }

    if (force_dst_port != -1) {
      if (force_dst_port != ntohs(pc->dst.sin_port)) {

	tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
        write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy]  "
		  "[Forcing target port to %d]",
		  tmpstr,                      ntohs(pc->src.sin_port),
		  inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		  force_dst_port);
	free(tmpstr);
	
	pc->dst.sin_port = htons(force_dst_port);
      }
    }
    
    // Create the onward socket
    new_server = socket(PF_INET, SOCK_STREAM, 0);
    if (new_server == -1) {

	tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
        write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] failed "
		  "[Could not create onward socket]: %s [%d]",
		  tmpstr,                      ntohs(pc->src.sin_port),
		  inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		  strerror(errno), errno);
	free(tmpstr);
	close(new_client);
	return 0;
    }

    // Record the socket handles in the track struct
    pc->socket      = new_server;
    pc->client_sock = new_client;
    
    option = 1;
    setsockopt(new_server, SOL_SOCKET, SO_REUSEADDR, (void*) &option, sizeof(option));
    
    option = 1;
    setsockopt(new_server, SOL_SOCKET, SO_KEEPALIVE, (void*) &option, sizeof(option));
    
    fcntl(new_server, F_SETFL, O_NONBLOCK);
    
    if (connect(new_server, (struct sockaddr*) &pc->dst, sizeof(pc->dst)) == -1) {
      
      // If it's pending, we're done, and the bottom half will be called from the
      // main select() loop
      if (errno == EINPROGRESS) {

	tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
	write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] created [pending connection] (%d %d)",
		  tmpstr,                      ntohs(pc->src.sin_port),
		  inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		  pc->client_sock, pc->socket);
	free(tmpstr);
	return 1;
      }
      
      // On other errors, kill the connection.
      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Connect returned error]: %s [%d]",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		strerror(errno), errno);
      free(tmpstr);
      
      // Cleanup of the pending connect struct is done in the main loop
      close(new_client);
      close(new_server);
      pc->socket = -1;
      pc->client_sock = -1;
      return 0;
    }
    
    // The connection succeeded immediately.  Aren't we lucky.
    tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
    write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] created [connection made]",
	      tmpstr,                      ntohs(pc->src.sin_port),
	      inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port));
    free(tmpstr);
    
    // As we connected immediately, call the processing routine directly.
    pc->proc(pc);
    
    return 1;
}

/*-------------------------------------------------------------------------
  int net_connect_inbound_nb()
  
  Setup an inbound non-blocking connection attempt.
  
  Return 0 on a failure.  Note that is it upto the caller to free
  proc_data if this function fails.
-------------------------------------------------------------------------*/

int net_connect_inbound_nb(listen_track_struct *lt,
		           connect_proc         proc,
		           pending_dc_struct   *pdc,
		           filter_chain        *tx_filter,
		           filter_chain        *rx_filter,
			   int                  force_dst_port)
{
    int new_client, new_server;
    int salen, option;
    pending_connect_struct *pc;
    char *tmpstr;
    struct timeval tv_now;

    // We have to clean up the pdc in here.
    pdc->cleanup = 1;
    
    // Allocate pending connection tracker
    pc = calloc(sizeof(pending_connect_struct), 1);
    if (!pc) {
      return 0;
    }

    gettimeofday(&tv_now, NULL);

    // Add to the pc list.  This will do garbage collection
    pc->socket      = -1;
    pc->client_sock = -1;
    pc->proc        = proc;
    pc->proc_data   = NULL;
    pc->filter[0]   = tx_filter;
    pc->filter[1]   = rx_filter;
    pc->timeout     = tv_now.tv_sec + CONNECT_TIMEOUT;
    pc->next        = pc_list.next;
    pc_list.next    = pc;
    
    // Open the pending connection (and log any errors)
    salen = sizeof(pc->src);
    new_client = net_open_connection(lt->socket, &pc->src, &salen);
    if (new_client == -1) {
	return 0;
    }

    // The pdc contains the destination address / port

    memcpy(&pc->dst, &pdc->src, sizeof(pdc->src));
    pc->dst.sin_port = htons(pdc->orig_port);
    
    if (force_dst_port != -1) {
      if (force_dst_port != ntohs(pc->dst.sin_port)) {

	tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
        write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy]  "
		  "[Forcing target port to %d]",
		  tmpstr,                      ntohs(pc->src.sin_port),
		  inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		  force_dst_port);
	free(tmpstr);
	
	pc->dst.sin_port = htons(force_dst_port);
      }
    }
    
    // Create the onward socket
    new_server = socket(PF_INET, SOCK_STREAM, 0);
    if (new_server == -1) {

	tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
        write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] failed "
		  "[Could not create onward socket]: %s [%d]",
		  tmpstr,                      ntohs(pc->src.sin_port),
		  inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		  strerror(errno), errno);
	free(tmpstr);
	close(new_client);
	return 0;
    }

    // Record the socket handles in the track struct
    pc->socket      = new_server;
    pc->client_sock = new_client;
    
    option = 1;
    setsockopt(new_server, SOL_SOCKET, SO_REUSEADDR, (void*) &option, sizeof(option));
    
    option = 1;
    setsockopt(new_server, SOL_SOCKET, SO_KEEPALIVE, (void*) &option, sizeof(option));
    
    fcntl(new_server, F_SETFL, O_NONBLOCK);
    
    if (connect(new_server, (struct sockaddr*) &pc->dst, sizeof(pc->dst)) == -1) {
      
      // If it's pending, we're done, and the bottom half will be called from the
      // main select() loop
      if (errno == EINPROGRESS) {

	tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
	write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] created [pending connection] (%d %d)",
		  tmpstr,                      ntohs(pc->src.sin_port),
		  inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		  pc->client_sock, pc->socket);
	free(tmpstr);
	return 1;
      }
      
      // On other errors, kill the connection.
      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Connect returned error]: %s [%d]",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		strerror(errno), errno);
      free(tmpstr);
      
      // Cleanup of the pending connect struct is done in the main loop
      close(new_client);
      close(new_server);
      pc->socket = -1;
      pc->client_sock = -1;
      return 0;
    }
    
    // The connection succeeded immediately.  Aren't we lucky.
    tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
    write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] created [connection made]",
	      tmpstr,                      ntohs(pc->src.sin_port),
	      inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port));
    free(tmpstr);
    
    // As we connected immediately, call the processing routine directly.
    pc->proc(pc);
    
    return 1;
}

/*-------------------------------------------------------------------------
  void* conn_open_outbound_generic(pending_connect_struct *pc)
  
  The onward connection has completed.  This function cleans up if it
  failed, or allocates the tracking and filter structures as needed if
  the connection is established.

  Either way, we have to set the pending connection structure to the
  cleanup state, so we can garbage collect it.
-------------------------------------------------------------------------*/
void conn_open_outbound_generic(pending_connect_struct *pc)
{
  int se, sel;
  user_track_struct  *ts;
  proxy_track_struct *pt;
  char *tmpstr;

  // Check if the connection succeeded or failed as we did an
  // asynchronous connect.
  if (getsockopt(pc->socket, SOL_SOCKET, SO_ERROR, &se, &sel) == -1) {

      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Unable to get connect status]: %s [%d] (%d:%d)",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		strerror(errno), errno,
		pc->client_sock, pc->socket);
      free(tmpstr);

      close(pc->client_sock);
      close(pc->socket);
      pc->socket = -1;
      pc->client_sock = -1;
      return;
  }
  
  // se is non zero if the connection failed.
  if (se != 0) {

      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Connect returned error]: %s [%d] (%d %d)",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		strerror(errno), errno,
		pc->client_sock, pc->socket);
      free(tmpstr);
      close(pc->client_sock);
      close(pc->socket);
      pc->socket      = -1;
      pc->client_sock = -1;
      return;
  }
  
  // Setup the tracking structure
  ts = calloc(sizeof(user_track_struct), 1);
  ts->magic = ts_magic ++;
  ts->next = user_list.next;
  user_list.next = ts;
  
  pt = pts_allocate();
  if (!pt) {
    
      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Failed to allocate tracking struct] (%d %d)",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		pc->client_sock, pc->socket);
      free(tmpstr);
    
      ts->cleanup = 1;
      close(pc->socket);
      close(pc->client_sock);
      pc->socket = -1;
      pc->client_sock = -1;
      return;
  }
  
  pt->sockets[0] = pc->client_sock;
  pt->sockets[1] = pc->socket;
  pt->filter[0] = pc->filter[0];
  pt->filter[1] = pc->filter[1];
  pt->user  = 1;
  pt->magic = ts->magic;
  
  pt->next = pt_list.next;
  pt_list.next = pt;
  
  tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
  write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] established "
	    "[magic = %d] (%d %d)",
	    tmpstr,                      ntohs(pc->src.sin_port),
	    inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
	    pt->magic, pc->client_sock, pc->socket);
  free(tmpstr);
  
  // Flag the pending connection for garbage collect without
  // closing the connections (which we just handed off to the
  // pt tracking struct)
  pc->socket = -1;
  pc->client_sock = -1;
}


/*-------------------------------------------------------------------------
  void* conn_open_inbound_generic(pending_connect_struct *pc)
  
  The onward connection has completed.  This function cleans up if it
  failed, or allocates the tracking and filter structures as needed if
  the connection is established.

  Either way, we have to set the pending connection structure to the
  cleanup state, so we can garbage collect it.
-------------------------------------------------------------------------*/
void conn_open_inbound_generic(pending_connect_struct *pc)
{
  int se, sel;
  proxy_track_struct *pt;
  char *tmpstr;

  // Check if the connection succeeded or failed as we did an
  // asynchronous connect.
  if (getsockopt(pc->socket, SOL_SOCKET, SO_ERROR, &se, &sel) == -1) {

      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Unable to get connect status]: %s [%d] (%d:%d)",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		strerror(errno), errno,
		pc->client_sock, pc->socket);
      free(tmpstr);

      close(pc->client_sock);
      close(pc->socket);
      pc->socket = -1;
      pc->client_sock = -1;
      return;
  }
  
  // se is non zero if the connection failed.
  if (se != 0) {

      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Connect returned error]: %s [%d] (%d %d)",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		strerror(errno), errno,
		pc->client_sock, pc->socket);
      free(tmpstr);
      close(pc->client_sock);
      close(pc->socket);
      pc->socket      = -1;
      pc->client_sock = -1;
      return;
  }
  
  pt = pts_allocate();
  if (!pt) {
    
      tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
      write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] failed "
		"[Failed to allocate tracking struct] (%d %d)",
		tmpstr,                      ntohs(pc->src.sin_port),
		inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
		pc->client_sock, pc->socket);
      free(tmpstr);
    
      close(pc->socket);
      close(pc->client_sock);
      pc->socket = -1;
      pc->client_sock = -1;
      return;
  }
  
  pt->sockets[0] = pc->client_sock;
  pt->sockets[1] = pc->socket;
  pt->filter[0] = pc->filter[0];
  pt->filter[1] = pc->filter[1];
  
  pt->next = pt_list.next;
  pt_list.next = pt;
  
  tmpstr = strdup(inet_ntoa(pc->src.sin_addr));
  write_log(log_file, "[CONN_BH] Connection [%s:%d -> %s:%d via proxy] established "
	    "(%d %d)",
	    tmpstr,                      ntohs(pc->src.sin_port),
	    inet_ntoa(pc->dst.sin_addr), ntohs(pc->dst.sin_port),
	    pc->client_sock, pc->socket);
  free(tmpstr);
  
  // Flag the pending connection for garbage collect without
  // closing the connections (which we just handed off to the
  // pt tracking struct)
  pc->socket = -1;
  pc->client_sock = -1;
}


/*-------------------------------------------------------------------------
  void net_open_aim_oscar(listen_track_struct *lt)

  Setup a generic open and callback pair which will attach the oscar filter
  if the connection is successfully established.

  This should only be called for connections heading out of the LAN to
  the off-site servers.
-------------------------------------------------------------------------*/

void net_open_aim_oscar(listen_track_struct *lt)
{

  net_connect_outbound_nb(lt, conn_open_outbound_generic, NULL, &oscar_tx, &oscar_rx, lt->port);

}


/*-------------------------------------------------------------------------
  void net_open_log_attempt(listen_track_struct *lt)
  
  This function is used for ports we are listenning to but never expecting
  to be connected to.  We log the attempt for diagnostic purposes.
-------------------------------------------------------------------------*/
void net_open_log_attempt(listen_track_struct *lt)
{
  struct sockaddr_in src_addr;
  int salen;
  int new_remote;
  
  // Open the pending connection....
  salen = sizeof(src_addr);
  new_remote = net_open_connection(lt->socket, &src_addr, &salen);
  if (new_remote == -1)
    return;
  close(new_remote);
  
  write_log(log_file, "[WARNING] Unexpected connection or attempted port scan [%s:%d -> proxy:%d]",
	    inet_ntoa(src_addr.sin_addr),
	    ntohs(src_addr.sin_port),
	    lt->port);
}


/*-------------------------------------------------------------------------
  void net_open_aim(listen_track_struct *lt)
  
  Open the direct connection attempt from the remote party, and alloc
  the generic proxy_track struct.
  
  This routine should be able to open a connection back to the inside as
  there should be only 1 pending connection for each listening port.

  A warning is logged if this isn't the case.

  This should only ever be called for incoming requests from offsite hosts.
-------------------------------------------------------------------------*/
void net_open_aim(listen_track_struct *lt)
{
    pending_dc_struct *pms, *first;
    int count;

    // Look for a pending connection for this port
    pms = dc_list.next;
    first = NULL;
    count = 0;
    
    while(pms) {
      
      if (pms->cleanup == 0) {
	
	if (pms->port == lt->port) {
	  count ++;
	  first = pms;
	}
	
      }
      
      pms = pms->next;
    }
    
    switch(count) {
    case 1:
      // Okay.  All is well...

      net_connect_inbound_nb(lt, conn_open_inbound_generic, first, &share_tx, &share_rx, -1);

      return;
      
    case 0:
      // We are probably being scanned...but we shouldn't be listening if
      // there is no pending connection...  Just flag a warning.
      net_open_log_attempt(lt);
      break;
      
    default:
      // Too many pending connections in the list.

      // Shouldn't ever get here.  Should never have more than 1 pending connection
      // per port.
      write_log(log_file, "[WARNING] Couldn't find exactly 1 connection in pending list. [proxy:%d]",
		lt->port);
      break;
    }
}


/*-------------------------------------------------------------------------
  void net_open_buggy_aim_dcc(listen_track_struct *lt)
  
  AIM appears to ignore the port specified in a direct connection setup.
  This breaks the dynamic dcc mapping, so we have this function to trap
  the buggy connection attempts to port 4443, and to do a fuzzy search
  of the pending connections list for a match.

  This routine can only open a connection back to the inside if
  there is only 1 pending connection for an inside machine on port 4443.

  A warning is logged if this isn't the case.

  This should only ever be called for incoming requests from offsite hosts.
-------------------------------------------------------------------------*/
void net_open_buggy_aim_dcc(listen_track_struct *lt)
{
    pending_dc_struct *pms, *first, *loose;
    listen_track_struct *ls;
    int count, loose_count;

    // Look for a pending connection for this port
    pms = dc_list.next;
    first = NULL;
    count = 0;
    loose = NULL;
    loose_count = 0;
    
    while(pms) {
      
      if (pms->cleanup == 0) {
	
	if (pms->orig_port == lt->port) {
	  count ++;
	  first = pms;
	}

	// Buggy client hack
	loose_count++;
	loose = pms;
	
      }
      
      pms = pms->next;
    }
    
    switch(count) {
    case 1:
      // Okay.  All is well...apart from the buggy AIM client
      
      write_log(log_file, "[WARNING] Unexpected connection to port %d.  Single match for buggy DCC found.",
		lt->port);
      
      // Note we have to manually clean up the dynamic dcc listener as
      // we picked up the DCC on port 4443 rather than the one we were
      // supposed to be contacted on.

      // Search for the listen tracker on the port in the pending_dc struct.
      ls = lt_list.next;
      while(ls) {
	if (ls->port == first->port)
	  ls->cleanup = 1;

	ls = ls->next;
      }

      net_connect_inbound_nb(lt, conn_open_inbound_generic, first, &share_tx, &share_rx, -1);

      return;
      
    case 0:
      // Didn't find any matches with strict port checking.  Try loose!
      if (loose_count == 1) {
	
	// Note we have to manually clean up the dynamic dcc listener as
	// we picked up the DCC on 4443 rather than the one we were
	// supposed to be contacted on.
	
	// Search for the listen tracker on the port in the pending_dc struct.
	ls = lt_list.next;
	while(ls) {
	  if (ls->port == loose->port)
	    ls->cleanup = 1;
	  
	  ls = ls->next;
	}
	
	write_log(log_file, "[WARNING] Unexpected connection to port %d.  Loose single match for buggy DCC. (Expected %d)",
		  lt->port, loose->port);
	
	net_connect_inbound_nb(lt, conn_open_inbound_generic, loose, &share_tx, &share_rx, DCC);
	
	return;
	
      }
    default:
      // Log the attempt.  We can get here if there is more than one DCC pending
      // or we're being port scanned.
      net_open_log_attempt(lt);
      break;
    }
}


/*-------------------------------------------------------------------------
  void net_open_msn(listen_track_struct *lt)
  
  Open the connection attempt from the LAN client, create the onward
  socket for the proxying, and allocate a tracking structure.
  
  If any steps fail, the client connection is terminated.
-------------------------------------------------------------------------*/

void net_open_msn(listen_track_struct *lt)
{

  net_connect_outbound_nb(lt, conn_open_outbound_generic, NULL, &msn_tx, &msn_rx, lt->port);

} 

/*-------------------------------------------------------------------------
  void net_open_msnrx(listen_track_struct *lt)
  
  Open the direct connection attempt from the remote party, and allocate
  the connection tracking structure.
  
  We know exactly which host to connect to as we are using dynamic dcc
  ports - one per connection.  Anything else causes an error to be
  flagged.
-------------------------------------------------------------------------*/
void net_open_msnrx(listen_track_struct *lt)
{
    pending_dc_struct *pms, *first;
    int count;

    // Look for a pending connection for this port
    pms = dc_list.next;
    first = NULL;
    count = 0;
    
    while(pms) {
      
      if (pms->cleanup == 0) {
	
	if (pms->port == lt->port) {
	  count ++;
	  first = pms;
	}
	
      }
      
      pms = pms->next;
    }
    
    switch(count) {
    case 1:
      // Okay.  All is well...

      net_connect_inbound_nb(lt, conn_open_inbound_generic, first, &msnrx_tx, &msnrx_rx, -1);

      return;
      
    case 0:
    default:
      // No/too many pending connections in the list.

      // Shouldn't ever get here.  Should never have more than 1 pending connection
      // per port.
      write_log(log_file, "[WARNING] Couldn't find exactly 1 connection in pending list. [proxy:%d]",
		lt->port);
      break;
    }
}

/*-------------------------------------------------------------------------
  unsigned int fp_process(unsigned char *buff,
                          unsigned int len,
			  unsigned int max,
			  filter_chain *fc,
			  proxy_track_struct *pt)

  Pass the packet through the filter chain.  Nice and simple really.

  A NULL filter chain is valid, and mean 'pass unchanged'
  
-------------------------------------------------------------------------*/

unsigned int fp_process(unsigned char *buff,
                        unsigned int len, unsigned int max,
			filter_chain *fc,
			proxy_track_struct *pt)
{
    int i;
    
    // Pass the data unchanged if there's no filter chain.
    if (!fc)
        return len;
    
    // Should be a little less lazy about using a fixed upper size limit
    // but there's no point making it complex for now.
    for (i = 0; (i < MAX_FILTERS) && fc->proc[i]; i++)
	len = fc->proc[i](buff, len, max, pt);
    
    return len;
}

/*-------------------------------------------------------------------------
  int fp_locate(unsigned char *data_buff, unsigned int data_len,
		unsigned char *find_buff, unsigned int find_len,
		unsigned char *mask_buff)
			  
  Filter helper:  Binary search for find_buff in data_buff, and return the
                  offset.  The buffers are masked with the mask_buff.
  
  Returns -1 if the sub-string can't be found.
-------------------------------------------------------------------------*/

int fp_locate(unsigned char *data_buff, unsigned int data_len,
	      unsigned char *find_buff, unsigned int find_len,
	      unsigned char *mask_buff)
{
  unsigned int s, i;

  if (find_len > data_len)
    return -1;
  
  for (s = 0; s < data_len - find_len; s++) {
    
    for (i = 0; i < find_len; i++) {
      
      if ((data_buff[s+i] & mask_buff[i]) != (find_buff[i] & mask_buff[i]))
	break;
    }
    
    if (i == find_len)
      return s;
  }
  
  return -1;
}

/*-------------------------------------------------------------------------
  unsigned int fp_oscar_tx(unsigned char *buff,
                           unsigned int len,
			   unsigned int max,
			   proxy_track_struct *pt)
			  
  Filter procedure: Search for and modify OSCAR IP/Port specifications in
                    passing packets.
  
  If the packet does not appear to be a file transfer setup, return it
  unchanged.
  
  If it appears to have an IP/port setting, setup a listen socket for
  the anticipated return packet.

-------------------------------------------------------------------------*/

unsigned int fp_oscar_tx(unsigned char *buff,
			 unsigned int len, unsigned int max,
			 proxy_track_struct *pt)
{
#if USE_DYNAMIC_DCC
    listen_track_struct *lt;
    struct timeval tv_now;
#endif
    
    // Not long enough for us to check properly...
    if (len < 26)
	return len;
    
    // Check its long enough for the TLV_TYPE
    if (len < AIM_BUD_NEXT(buff) + 2)
	return len;
    
    // Does it look like a potential Direct Connection setup packet ?
    if ((AIM_CMDSTART(buff)        == 0x2a) &&
	(AIM_CHANNEL(buff)         == 0x02) &&
	(AIM_FNAC_FAMILY(buff)     == 0x0004) &&
	(AIM_FNAC_SUBTYPE(buff)    == 0x0006) &&
	(AIM_FNAC_FLAGS(buff)      == 0x0000) &&
	(AIM_SNAC_CHAN(buff)       == 0x0002) &&
	(AIM_SNAC_TLV_TYPE(buff)   == 0x0005)) {

        int alloc_port, orig_dcc_port;
	int ip[4], ip_offset, src_ip[4], offset;
	struct sockaddr_in src_addr;
	int salen;
	unsigned char scan[12], mask[12];
	pending_dc_struct *pdc;

	// Read the source IP for the client connection.
	// We use this for extra sanity checks in locating the client IP
	// in the setup packet
	salen = sizeof(struct sockaddr_in);
	if (getpeername(pt->sockets[TS_CLIENT], (struct sockaddr*) &src_addr, &salen) == -1) {
	    return len;
	}
	
	sscanf(inet_ntoa(src_addr.sin_addr), "%d.%d.%d.%d", &src_ip[0], &src_ip[1], &src_ip[2], &src_ip[3]);

	// Look for a sequence in the packet that is 00 03 00 04 IP IP IP IP 00 05 00 02 ?? ??
	scan[0] = 0x00;
	scan[1] = 0x03;
	scan[2] = 0x00;
	scan[3] = 0x04;
	scan[4] = src_ip[0];
	scan[5] = src_ip[1];
	scan[6] = src_ip[2];
	scan[7] = src_ip[3];
	scan[8] = 0x00;
	scan[9] = 0x05;
	scan[10] = 0x00;
	scan[11] = 0x02;

	// Setup the mask
	memset(mask, 255, sizeof(mask));
	mask[4] = mask[5] = mask[6] = mask[7] = 0;
	
	// Search for the IP/port address sequence
	offset = fp_locate(buff, len, scan, 12, mask);
	if (offset == -1)
	  return len;

	// Extract the original DCC port on the inside host
	orig_dcc_port = (buff[offset + 12] << 8) | buff[offset + 13];

	// Point to the actual address by adjusting over the search preamble.
	ip_offset = offset + 4;
	
	// Okay.  Determine which address to overwrite the client IP with...
	if (outside_ip == NULL) {
	    
	    // Make a best guess here because we weren't told....
	    struct sockaddr_in svr_addr;
	    
	    // Read the source address of the connection to the server
	    // and give up if we can't...
	    salen = sizeof(struct sockaddr_in);
	    if (getsockname(pt->sockets[TS_SERVER], (struct sockaddr*) &svr_addr, &salen) == -1) {
		write_log(log_file, "Found a DCC F setup, but couldn't get the external IP - dodo mode.");
		return len;
	    }
	    
	    sscanf(inet_ntoa(svr_addr.sin_addr), "%d.%d.%d.%d",
		   &ip[0], &ip[1], &ip[2], &ip[3]);
	    
	} else {
	    
	    if (sscanf(outside_ip, "%d.%d.%d.%d",
		       &ip[0], &ip[1], &ip[2], &ip[3]) != 4) {
		write_log(log_file, "Bad external proxy IP address specified - [%s] Dodo mode",
			  outside_ip);
		return len;
	    }
	}
	
#if USE_DYNAMIC_DCC
	lt = create_listener_between(INC_MIN, INC_MAX, net_open_aim);
	// Couldn't create onward, so can't mangle this request.
	if (!lt)
	  return len;
	
	// Setup as a 1 shot with a 30 second limit.
	lt->oneshot = 1;
	gettimeofday(&tv_now, NULL);
	lt->timeout = tv_now.tv_sec + INC_TIMEOUT;
	
	alloc_port = lt->port;
#else
	alloc_port = SHARE;
#endif
	
	// Mangle the data so it has the outside IP address
	// and port in the right place.
	buff[ip_offset + 0] = ip[0];
	buff[ip_offset + 1] = ip[1];
	buff[ip_offset + 2] = ip[2];
	buff[ip_offset + 3] = ip[3];
	buff[ip_offset + 8] = (alloc_port >> 8) & 0xff;
	buff[ip_offset + 9] = alloc_port & 0xff;
	
	// Report the mangling we just did
	write_log(log_file,"[MASQ] DCC F Setup IP Changed "
		  "[%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d] [offset=%d] [buddy=%.*s]",
		  src_ip[0], src_ip[1], src_ip[2], src_ip[3], orig_dcc_port,
		  ip[0], ip[1], ip[2], ip[3], alloc_port,
		  ip_offset,
		  AIM_BUD_NAME_LEN(buff), AIM_BUD_NAME(buff));
	
	// Allocate a pending_dc_struct here.
	pdc = malloc(sizeof(pending_dc_struct));
	memset(pdc, 0, sizeof(pending_dc_struct));

	pdc->magic = pt->magic;
	pdc->id[0] = AIM_SNAC_COOKIE(buff)[0];
	pdc->id[1] = AIM_SNAC_COOKIE(buff)[1];
	pdc->id[2] = AIM_SNAC_COOKIE(buff)[2];
	pdc->id[3] = AIM_SNAC_COOKIE(buff)[3];
	pdc->id[4] = AIM_SNAC_COOKIE(buff)[4];
	pdc->id[5] = AIM_SNAC_COOKIE(buff)[5];
	pdc->id[6] = AIM_SNAC_COOKIE(buff)[6];
	pdc->id[7] = AIM_SNAC_COOKIE(buff)[7];

	pdc->port = alloc_port;
	pdc->orig_port = orig_dcc_port;

	// Copy the original source address information into the DC struct
	// We need this to establish the back connection.
	memcpy(&pdc->src, &src_addr, sizeof(src_addr));
	
	pdc->next = dc_list.next;
	dc_list.next = pdc;
    }
    
    return len;
}


/*-------------------------------------------------------------------------
  unsigned int fp_msn_tx_dcc_setup(unsigned char *buff,
                                   unsigned int len, unsigned int max,
		                   proxy_track_struct *pt)
  
  This function examines data flowing out from this machine for answers
  to direct connection setup queries.
  
  If it looks like a DCC request, we search for the IP address and port
  being sent to the remote party, and we overwrite it with the external
  IP address, and a port from the dynamic dcc range.

  Having done that, we setup a pending connection structure, and create
  the listenner that will process the incoming request from the remote
  party. (See net_open_msnrx for the incoming request processing)
-------------------------------------------------------------------------*/

unsigned int fp_msn_tx_dcc_setup(unsigned char *buff,
			 unsigned int len, unsigned int max,
			 proxy_track_struct *pt)
{
    int port, cookie;

    int ip[4], src_ip[4];
    struct sockaddr_in src_addr;
    int salen;
    pending_dc_struct *pdc;
    char copy_msg[2048], *pHdrs[32], *pMsgs[32], *scan, tmpbuf[256];
    int hdr_fields, msg_fields, find, dcc_port;
#if USE_DYNAMIC_DCC
    listen_track_struct *lt;
    struct timeval tv_now;
#endif
    
    // Not long enough for a dcc setup request or too long...
    if (len < 85 || len >= sizeof(copy_msg))
	return len;
    
    memcpy(copy_msg, buff, len);
    copy_msg[len] = '\0';

    hdr_fields = 0;
    
    for (scan = copy_msg;
	 (scan != NULL) &&
	 (scan[0] != '\0') &&
	     (scan[0] != '\r') &&
	     (scan[1] != '\n');) {

	pHdrs[hdr_fields] = scan;
	hdr_fields ++;
	if (hdr_fields == 32)
	    return len;
	
	scan = strstr(scan, "\r\n");
	if (scan) {
	    *scan = '\0'; // NULL terminate the pHdr
	    scan += 2;    // Shuffle past the \r\n
	}
    }
    
    // Badly formed buffer
    if (!scan || scan[0] == '\0')
	return len;

    scan += 2; // Skip the \r\n separator

    msg_fields = 0;
    
    for (;
	 (scan != NULL) &&
	 (scan[0] != '\0') &&
	     (scan[0] != '\r') &&
	     (scan[1] != '\n');) {

	pMsgs[msg_fields] = scan;
	msg_fields ++;
	if (msg_fields == 32)
	    return len;
	
	scan = strstr(scan, "\r\n");
	if (scan) {
	    *scan = '\0'; // NULL terminate the pMsg
	    scan += 2;    // Shuffle past the \r\n
	}
    }

#if TRILLIAN_HEADER_SUPPORT
    // Trillian doesn't end the headers with two \n\r pairs as
    // it should, so we do actually reach the end of the buffer
    // when looking for headers above.  If we're compiling with
    // TRILLIAN support, skip this check, even though it potentially
    // confuses the header parsing with the user message.

#else
    // Badly formed buffer without two \n\r pairs at the end of the headers.
    if (!scan || scan[0] == '\0')
	return len;
#endif

    // Look for the invite / accepted message....

    for (find = 0; find < hdr_fields; find ++) {
	if (strcmp(pHdrs[find],
		   "Content-Type: text/x-msmsgsinvite; charset=UTF-8") == 0)
	    break;
    }
    
    if (find == hdr_fields)
	return len;

    for (find = 0; find < msg_fields; find ++) {
	if (strcmp(pMsgs[find],
		   "Invitation-Command: ACCEPT") == 0)
	    break;
    }

    if (find == msg_fields)
	return len;

    // Find the IP address header in the packet
    for (find = 0; find < msg_fields; find ++) {
	if (strncmp(pMsgs[find], "IP-Address: ",
		    strlen("IP-Address: ")) == 0) {

	    if (sscanf(pMsgs[find], "IP-Address: %d.%d.%d.%d",
		       &ip[0], &ip[1], &ip[2], &ip[3]) != 4) {
		write_log(log_file, "MSN DCC: Badly formed IP Address - dodo mode");
		return len;
	    }
	    
	    break;
	}
    }
    
    if (find == msg_fields)
	return len;
    
    // Find the IP address header in the packet
    for (find = 0; find < msg_fields; find ++) {
	if (strncmp(pMsgs[find], "Port: ",
		    strlen("Port: ")) == 0) {
	    
	    if (sscanf(pMsgs[find], "Port: %d",
		       &port) != 1) {
		write_log(log_file, "MSN DCC: Badly formed IP port specifier - dodo mode");
		return len;
	    }
	    
	    break;
	}
    }
    
    if (find == msg_fields)
	return len;
    
    // Find the AuthCookie in the packet
    for (find = 0; find < msg_fields; find ++) {
	if (strncmp(pMsgs[find], "AuthCookie: ",
		    strlen("AuthCookie: ")) == 0) {

	    if (sscanf(pMsgs[find], "AuthCookie: %d",
		       &cookie) != 1) {
		write_log(log_file, "MSN DCC: no cookie found - dodo mode");
		return len;
	    }
	    
	    break;
	}
    }
    
    if (find == msg_fields)
	return len;
    
    // Read the source IP for the client connection.
    // We use this for extra sanity checks in locating the client IP in
    // the setup packet
    salen = sizeof(struct sockaddr_in);
    if (getpeername(pt->sockets[TS_CLIENT], (struct sockaddr*) &src_addr, &salen) == -1) {
	return len;
    }
    
    sscanf(inet_ntoa(src_addr.sin_addr), "%d.%d.%d.%d",
	   &src_ip[0], &src_ip[1], &src_ip[2], &src_ip[3]);
    
    if ((ip[0] != src_ip[0]) || (ip[1] != src_ip[1]) ||
	(ip[2] != src_ip[2]) || (ip[3] != src_ip[3])) {
#if LOOSE_CLIENT_IP
	// If strict checking is on, just note the discrepency...
	write_log(log_file, "MSN Loose DCC Source. Expecting %d.%d.%d.%d, found %d.%d.%d.%d. Continuing mangle.", 
		  src_ip[0], src_ip[1], src_ip[2], src_ip[3],
		  ip[0], ip[1], ip[2], ip[3]);
#else
	// If strict checking is on, complain and don't mangle.
	write_log(log_file, "MSN Loose DCC Source. Expecting %d.%d.%d.%d, found %d.%d.%d.%d. Not overwriting.", 
		  src_ip[0], src_ip[1], src_ip[2], src_ip[3],
		  ip[0], ip[1], ip[2], ip[3]);
	return len;
#endif
    }
    
    // Okay.  Now to determine which address to overwrite the client IP with...
    if (outside_ip == NULL) {
	    
	// Make a best guess here because we weren't told....
	struct sockaddr_in svr_addr;
	
	// Read the source address of the server connection...
	salen = sizeof(struct sockaddr_in);
	if (getsockname(pt->sockets[TS_SERVER], (struct sockaddr*) &svr_addr, &salen) == -1) {
	    write_log(log_file, "MSN Found a DC setup, but couldn't work out the external IP - dodo mode");
	    return len;
	}
	
	sscanf(inet_ntoa(svr_addr.sin_addr), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	
    } else {
	sscanf(outside_ip, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    }
    
#if USE_DYNAMIC_DCC
    lt = create_listener_between(INC_MIN, INC_MAX, net_open_msnrx);
    // Couldn't create onward, so can't mangle this request.
    if (!lt)
      return len;
    
    // Setup as a 1 shot with a 30 second limit.
    lt->oneshot = 1;
    gettimeofday(&tv_now, NULL);
    lt->timeout = tv_now.tv_sec + INC_TIMEOUT;
    
    dcc_port = lt->port;
#else
    dcc_port = MSNRX;
#endif

    // Create the new packet, now we have all the data.
    
    write_log(log_file, "MSN DCC Setup IP Changed...%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d",
	      src_ip[0], src_ip[1], src_ip[2], src_ip[3], port,
	      ip[0], ip[1], ip[2], ip[3], dcc_port);
    
    // copy over the headers
    for (find = 0, scan = buff, len = 0; find < hdr_fields; find++) {
	
	sprintf(scan, "%s\r\n", pHdrs[find]);
	scan = &scan[(strlen(pHdrs[find]) + 2)];
	len += (strlen(pHdrs[find]) + 2);
    }
    
    sprintf(scan, "\r\n");
    scan = &scan[2];
    len += 2;
    
    // Copy over and mangle as required the msg fields
    for (find = 0; find < msg_fields; find ++) {
	
	int done = 0;
	
	if (strncmp(pMsgs[find], "IP-Address: ",
		    strlen("IP-Address: ")) == 0) {
	    
	    sprintf(tmpbuf, "IP-Address: %d.%d.%d.%d",
		    ip[0], ip[1], ip[2], ip[3]);
	    
	    sprintf(scan, "%s\r\n", tmpbuf);
	    scan = &scan[(strlen(tmpbuf) + 2)];
	    len += (strlen(tmpbuf) + 2);
	    done = 1;
	    
	} else if (strncmp(pMsgs[find], "Port: ",
		    strlen("Port: ")) == 0) {
	    
	    sprintf(tmpbuf, "Port: %d", dcc_port);
	    
	    sprintf(scan, "%s\r\n", tmpbuf);
	    scan = &scan[(strlen(tmpbuf) + 2)];
	    len += (strlen(tmpbuf) + 2);
	    done = 1;
	    
	}
	
	if (!done) {
	    // Just copy over the field unchanged
	    sprintf(scan, "%s\r\n", pMsgs[find]);
	    scan = &scan[(strlen(pMsgs[find]) + 2)];
	    len += (strlen(pMsgs[find]) + 2);
	    
	}
    }

    // Finish off the command building
    sprintf(scan, "\r\n");
    scan = &scan[2];
    len += 2;
    
    // Allocate a pending_dc_struct here.
    pdc = calloc(sizeof(pending_dc_struct), 1);
    if (!pdc) {
      // Damn.  Ran out of RAM.  All hell is about to break loose.
      // There won't be a direct connect struct for this DCC, so
      // the DCC will fail, but it should only fail with an error
      // in the net_open_msnrx procedure.
      return len;
    }
    
    pdc->orig_port  = port;
    pdc->port       = dcc_port;
    
    // Copy the original source address information into the DC struct
    // We need this to establish the back connection.
    memcpy(&pdc->src, &src_addr, sizeof(src_addr));
    
    // Add to pending DC list
    pdc->next    = dc_list.next;
    dc_list.next = pdc;
    
    return len;
}

/*
 * Read in a filter configuration / control file
 *
 * Also dynamic link any modules when I get that far...
 *
 * For now, just do it statically.
 */
void setup_filters(const char *ini_file)
{
    // AIM FILTERS
    memset(&oscar_tx, 0, sizeof(oscar_tx));
#if USE_DYNAMIC_DCC
    oscar_tx.proc[0] = fp_oscar_tx;                    // LAN client to AIM filter
#else
    oscar_tx.proc[0] = fp_oscar_tx;                    // LAN client to AIM filter
#endif

    memset(&oscar_rx, 0, sizeof(oscar_rx));

    //share_tx;   // LAN to Remote filter
    memset(&share_tx, 0, sizeof(share_tx));

    //share_rx;   // Remote to LAN filter
    memset(&share_rx, 0, sizeof(share_rx));

    // MSN FILTERS
    memset(&msn_tx, 0, sizeof(msn_tx));
    msn_tx.proc[0] = fp_msn_tx_dcc_setup;  // LAN client to MSG filter

    //msn_rx;
    memset(&msn_rx, 0, sizeof(msn_rx));

    //msnrx_tx;   // MSN file xfer things
    memset(&msnrx_tx, 0, sizeof(msnrx_tx));

    //msnrx_rx;   
    memset(&msnrx_rx, 0, sizeof(msnrx_rx));
}

/*-------------------------------------------------------------------------
 * Open a new listen socket, and attach a track struct to the
 * linked list of tracking structures.
 *
 * Return NULL on failure, or the newly allocated and added
 * track struct on success.
 *-----------------------------------------------------------------------*/
listen_track_struct* create_listener_on(int port, listen_proc lp)
{
  listen_track_struct *lt;
  
  // Allocate a new track structure if possible.
  lt = malloc(sizeof(listen_track_struct));
  if (!lt)
    return NULL;
  memset(lt, 0, sizeof(listen_track_struct));
  
  lt->proc = lp;
  lt->port = port;
  lt->socket = net_listen(port, "0.0.0.0");
  if (lt->socket == -1) {
    free(lt);
    return NULL;
  }
  
  // Add to the tracked socket list.
  lt->next = lt_list.next;
  lt_list.next = lt;
  
  return lt;
}

/*-------------------------------------------------------------------------
 * Open a new listen socket, and attach a track struct to the
 * linked list of tracking structures.
 *
 * Return NULL on failure, or the newly allocated and added
 * track struct on success.
 *
 * This call searches the specified port range for an open port to use.
 * The port allocated is returned in the listen_track_struct for the
 * caller.
 *-----------------------------------------------------------------------*/
listen_track_struct* create_listener_between(int port_min, int port_max, listen_proc lp)
{
  listen_track_struct *lt;
  
  // Allocate a new track structure if possible.
  lt = malloc(sizeof(listen_track_struct));
  if (!lt)
    return NULL;
  memset(lt, 0, sizeof(listen_track_struct));
  
  lt->oneshot = 1;
  lt->proc = lp;
  lt->socket = net_listen_between(port_min, port_max, "0.0.0.0", &lt->port);
  if (lt->socket == -1) {
    free(lt);
    return NULL;
  }
  
  // Add to the tracked socket list.
  lt->next = lt_list.next;
  lt_list.next = lt;
  
  return lt;
}

/*
 * Read in a filter configuration / control file
 *
 * Also dynamic link any modules when I get that far...
 *
 * For now, just do it statically.
 */
int setup_listenners(const char *ini_file)
{
  int n_lt;
  listen_track_struct *lt;
  
  //
  // Interception for data heading out to external hosts.
  //
  // These procedures attach the appropriate filtering
  // routines for that type of connection.
  //

  if (enable_msn) {
    create_listener_on(MSN,     net_open_msn);
#if USE_DYNAMIC_DCC
    // Log invalid attempts
    create_listener_on(MSNRX,   net_open_log_attempt);
#else
    create_listener_on(MSNRX,   net_open_msnrx);
#endif
  } else {
    write_log(log_file, "[STARTUP] Support for MSN is disabled");
  }
  
  if (enable_aim) {
    create_listener_on(OSCAR,   net_open_aim_oscar);
#if USE_DYNAMIC_DCC
    // Log invalid shares
    create_listener_on(SHARE,   net_open_log_attempt);
    // Some AIM clients are buggy, so watch for that
    create_listener_on(DCC,     net_open_buggy_aim_dcc);
#else
    create_listener_on(SHARE,   net_open_aim);
    create_listener_on(DCC,     net_open_aim);
#endif
  } else {
    write_log(log_file, "[STARTUP] Support for AIM is disabled");
  }

  // Count listen_track_struct
  n_lt = 0;
  lt = lt_list.next;
  while(lt != NULL) {
    n_lt ++;
    lt = lt->next;
  }
  
  return n_lt;
}

void close_listenners(const char *ini_file)
{
    listen_track_struct *lt;
    
    // Run around the listenners list and free any listenners
    while (lt_list.next) {

      lt = lt_list.next;
      lt_list.next = lt->next;

      close(lt->socket);
      free(lt);
    }
}

/* HUP Signal Handler */
void sig_hup(int i)
{
  pending_hup = 1;
}

/* Setup as a demon.  Parent process is terminated */
void demonize()
{
    pid_t pid;
    int sockets, max_sockets;

    /* Fork and close the parent */
    pid = fork();
    
    /* Parent has pid of child.  Child has pid == 0 */
    if (pid != 0) {
      _exit(0); // close the parent
    }
    
    // Setup as a process leader...
    setsid();
    
    /* Fork and close the process leader */
    pid = fork();

    /* Parent has pid of child.  Child has pid == 0 */
    if (pid != 0) {
      _exit(0); // close the parent
    }
    
    /* Change to root directory */
    chdir("/");

    /* Close any open sockets.  Including stdout/stderr/stdin.
     * Perhaps I should reopen 0/1/2, but ReAIM doesn't use them
     */
    max_sockets = sysconf(_SC_OPEN_MAX);
    for (sockets = 0; sockets < max_sockets; sockets ++)
      close(sockets);
}

void usage()
{
  printf("Usage: reaim [-h] [-m] [-a] [-e <ip>] [-l log] [-d]\n"
	 "\n"
	 "  -h | --help                   : Show this text.\n"
	 "  -m | --disable-msn            : Disable MSN support.\n"
	 "  -a | --disable-aim            : Disable AIM support.\n"
	 "  -e <ip> | --external-ip <ip>  : Manually set the override ip address.\n"
	 "  -l <log> | --logfile <log>    : Set logfile location.\n"
	 "                                    Default: %s\n"
	 "  -d                            : Do not fork into the background.\n",
	 LOG_FILE);
}

int parse_options(int argc, char * argv[])
{
// BRCM begin      
  int option_index = 0, lose = 0;
// BRCM end 
  int c;
  
  while (1) {
    
    c = getopt_long(argc, argv, "mae:hl:d",
                    long_options, &option_index);
    
    if (c == -1)
      break;
    
    switch(c) {
    case 'm':
      enable_msn = 0;
      break;
    case 'a':
      enable_aim = 0;
      break;
    case 'e':
      outside_ip = strdup(optarg);
      break;
    case 'h':
      usage();
      return 0;
      break;
    case 'l':
// BRCM begin      
      enable_log = 1;
      if (strlen(optarg) > 0) 
         log_file = strdup(optarg);
// BRCM end      
      break;
    case 'd':
      enable_fork = 0;
      break;
    default:
      lose = 1;
      break;
    }
  }

// BRCM begin      
  if (lose || optind < argc) {
     usage();
     return 0;
  }     
// BRCM end      
  
  return 1;
}

#ifdef BUILD_STATIC
int reaim_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    fd_set rfds, efds, wfds;
    struct timeval tv_sel, tv_now;
    int sv;
    struct sigaction set_act, old_act;
    proxy_track_struct *pt, *prev_pt;
    listen_track_struct *lt, *plt;
    pending_connect_struct *pc;
    int max_fd, sock;
    pending_dc_struct *pdc, *prev_pdc;
#if PERIODIC_LOG
    struct timeval tv_stats;
    tv_stats.tv_sec = 0;
#endif
    
    if (!parse_options(argc, argv))
      exit(-5);
    
    if (write_log(log_file,
		  "[STARTUP] Started AIM-Fix Proxy [%s%s]",
		  REAIM_VERSION, PLATFORM) == 0) {
//BRCM begin	    
#if 0
      if (enable_fork) {
	fprintf(stderr,
		"[STARTUP] Started AIM-Fix Proxy [%s%s]\n"
		"[WARNING] Unable to write log to %s.\n"
		"[WARNING] Proxy will continue, but logging will not work.\n",
		REAIM_VERSION, PLATFORM, log_file);
      } else {
	// In the case of invalid log file but not forking, write_log
	// writes to stderr
	write_log(log_file,
		  "[WARNING] Unable to write log to %s.\n", log_file);
	write_log(log_file,
		  "[WARNING] Proxy will run, but logging will not work.\n");
      }
#endif      
//BRCM end	  
    };
    
    if (enable_fork)
      demonize();
    
    /* Setup the action table for SIGPIPE - ignore. */
    memset(&set_act,0,sizeof(set_act));
    set_act.sa_handler  = SIG_IGN;
    sigemptyset(&set_act.sa_mask);
    set_act.sa_flags    = 0;
    sigaction(SIGPIPE, &set_act, &old_act);
    
    /* Setup the action table for SIGHUP - rebind the listenning sockets. */
    memset(&set_act,0,sizeof(set_act));
    set_act.sa_handler  = sig_hup;
    sigemptyset(&set_act.sa_mask);
    set_act.sa_flags    = 0;
    sigaction(SIGHUP, &set_act, &old_act);

    if (outside_ip)
      write_log(log_file, "[STARTUP] Using %s for the external IP address.", outside_ip);
    else
      write_log(log_file, "[STARTUP] Auto-detecting the external IP address.");
    
    setup_filters("");
    
    if (setup_listenners("") == 0) {
      write_log(log_file, "[WARNING] Not listenning on any sockets.");
      write_log(log_file, "[WARNING] Already running / misconfigured ?");
    }
    
    // - Wait for connections to the redirected port
    // - Setup the onward connection to listen
    // - Make onward connection to source
    // - Push around any packets
    
    while(1) {

      // Check for HUP...
      if (pending_hup) {
	write_log(log_file, "[SIGNAL] SIGHUP received.  Rebinding listenning sockets.");
	close_listenners("");
	setup_listenners("");
	pending_hup = 0;
      }
      
      // Setup the list of sockets we are monitoring
      FD_ZERO(&rfds);
      FD_ZERO(&efds);
      FD_ZERO(&wfds);
      
      max_fd = 0;
      
      // Get the current time
      gettimeofday(&tv_now, NULL);
      
#if PERIODIC_LOG
      // Periodic stats.
      // 1 line every 5 minutes.
      // Useful for finding leaks in the list handling.
      if (tv_stats.tv_sec < tv_now.tv_sec) {
	periodic_log();
	tv_stats.tv_sec = tv_now.tv_sec + 60*5;
      }
#endif


      // Process the listen_track list.
      // - Timeout processing
      // - Cleanup processing
      // - Add any listenners to the socket list.
      plt = &lt_list;
      lt = lt_list.next;
      while(lt != NULL) {
	
	if ((lt->timeout != 0) && (lt->timeout < tv_now.tv_sec)) {
	  write_log(log_file, "[TIMEOUT] Listen on proxy:%d timed out.", lt->port);
	  lt->cleanup = 1;
	}
	
	if (lt->cleanup) {
	  
	  plt->next = lt->next;
	  
	  close(lt->socket);
	  
	  // As we just stopped listening to lt->port, kill any
	  // pending connects for that port too.
	  pdc = dc_list.next;
	  while(pdc) {
	    if (pdc->port == lt->port)
	      pdc->cleanup = 1;
	    
	    pdc = pdc->next;
	  }
	  
	  free(lt);
	  
	} else {
	  
	  if (lt->socket > max_fd)
	    max_fd = lt->socket;
	  
	  FD_SET(lt->socket, &rfds);
	  
	  plt = lt;
	}
	
	// Advance loop
	lt = plt->next;
      }
      
      // Cleanup any pending_dc_list entries that are flagged for cleanup
      prev_pdc = &dc_list;
      pdc = dc_list.next;
      while(pdc) {
	if (pdc->cleanup) {
	  pending_dc_struct *next_dc;
	  
	  prev_pdc->next = pdc->next;

	  next_dc = pdc->next;
	  free(pdc);
	  pdc = next_dc;
	} else {
	  prev_pdc = pdc;
	  pdc = pdc->next;
	}
      }
      
	pt = pt_list.next;
	prev_pt = &pt_list;
	while(pt != NULL) {

	    // Cleanup any proxy connections that are flagged...
	    if (pt->cleanup) {

		proxy_track_struct *clean_pt = pt;
		
		write_log(log_file, "[Cleanup] proxy_track_struct [magic=%d] [user=%d]",
			  pt->magic, pt->user);

		// Adjust the linked list
		prev_pt->next = pt->next;
		pt = pt->next;

		// Clean up the proxy track structure data
		
		if (clean_pt->data_buff[0])
		    free(clean_pt->data_buff[0]);

		if (clean_pt->data_buff[1])
		    free(clean_pt->data_buff[1]);

		if (clean_pt->sockets[0] != -1)
		    close(clean_pt->sockets[0]);
		if (clean_pt->sockets[1] != -1)
		    close(clean_pt->sockets[1]);

		// If it's the user-associated socket, zap the user
		if (clean_pt->user == 1) {

		    user_track_struct *user_ut = user_list.next;

		    while(user_ut) {
			if (user_ut->magic == clean_pt->magic)
			    user_ut->cleanup = 1;
			
			user_ut = user_ut->next;
		    }
		}
		
		free(clean_pt);
		
		continue;
	    }

	    // Add the sockets to the read/write selection arrays
	    // as appropriate.
	    for (sock = 0; sock < 2; sock ++) {

		if (pt->sockets[sock] != -1) {
		    FD_SET(pt->sockets[sock], &rfds);
		    
		    if (pt->data_len[sock] > 0)
			FD_SET(pt->sockets[sock], &wfds);
		    
		    if (pt->sockets[sock] > max_fd)
			max_fd = pt->sockets[sock];
		}
	    }

	    prev_pt = pt;
	    pt = pt->next;
	}
	
	// Add any pending connection sockets to the write check

	pc = &pc_list;
	while(pc->next) {
	  if (pc->next->socket == -1) {

	    // Free the finished up socket
	    pending_connect_struct *pct;

	    pct = pc->next;
	    pc->next = pct->next;
	    free(pct);

	  } else {

	    // Check for a timeout.
	    if (pc->next->timeout < tv_now.tv_sec) {

	      char *tmpstr;
	      tmpstr = strdup(inet_ntoa(pc->next->src.sin_addr));
	      write_log(log_file, "[CONN_NB] Connection [%s:%d -> %s:%d via proxy] failed "
			"[timeout reached] (%d %d)",
			tmpstr,                      ntohs(pc->next->src.sin_port),
			inet_ntoa(pc->next->dst.sin_addr), ntohs(pc->next->dst.sin_port),
			pc->next->client_sock, pc->next->socket);
	      free(tmpstr);
	      
	      close(pc->next->socket);
	      close(pc->next->client_sock);
	      pc->next->client_sock = -1;
	      pc->next->socket = -1;

	    } else {
	      
	      // Add the socket
	      FD_SET(pc->next->socket, &wfds);
	      if (pc->next->socket > max_fd)
		max_fd = pc->next->socket;
	    }
	    
	    pc = pc->next;
	  }
	}
	
	// Wait for some activity
	tv_sel.tv_usec = 0;
	tv_sel.tv_sec  = 1;
	
	sv = select(max_fd+1,&rfds,&wfds,&efds,&tv_sel);
	switch(sv) {
	  
	case -1:
	    /* Some socket error, maybe */
	    if (errno != EINTR) {
		perror("[FATAL] Server socket died: ");
		exit(1);
	    }
	    break;
	case 0:
	    // Just timed out with nothing going on.
	    break;
	    
	default:
	    
	    // Process listenners...
	    lt = lt_list.next;
	    while(lt != NULL) {
	      
	      if ((lt->socket != -1) &&
		  (FD_ISSET(lt->socket, &rfds))) {
		lt->proc(lt);
		if (lt->oneshot)
		  lt->cleanup = 1;
	      }
	      
	      lt = lt->next;
	    }
	    
	    // Check pending connection sockets
	    pc = &pc_list;
	    while(pc->next) {
	      if (pc->next->socket != -1) {
		
		if (FD_ISSET(pc->next->socket, &wfds)) {
		  pc->next->proc(pc->next);
		}
	      }
	      
	      pc = pc->next;
	    }
	    
	    // Check generic proxied port pairs
            pt = pt_list.next;
	    while(pt) {

		for (sock = 0; sock < 2; sock ++) {
		    
		    // On Write list ?  We must have pending data for it...
		    if ((pt->sockets[sock] != -1) &&
			(pt->cleanup == 0) &&
			FD_ISSET(pt->sockets[sock], &wfds)) {
			
			int txs;
			
			txs = write(pt->sockets[sock],
				    &pt->data_buff[sock][pt->data_offset[sock]],
				    pt->data_len[sock]);
			if (txs < 0) {
			    if ((errno != EINTR) && (errno != EAGAIN)) {
				write_log(log_file, "Socket write failure! cleaning up!");
				pt->cleanup = 1;
			    }
			} else {
			    pt->data_len[sock] -= txs;
			    if (pt->data_len[sock] == 0)
				pt->data_offset[sock] = 0;
			    else
				pt->data_offset[sock] += txs;
			}
		    }
		    
		    // On read list, we have data for the other half...
		    //
		    // We only read it if the other half's write buffer is
		    // completely empty.
		    //
		    if ((pt->sockets[sock] != -1) &&
			(pt->cleanup == 0) &&
			FD_ISSET(pt->sockets[sock], &rfds) &&
			(pt->data_len[sock ^ 1] == 0)) {
			
			int rxs;

			// Read into the other half's data buffer.
			rxs = read(pt->sockets[sock],
				   pt->data_buff[sock^1],
				   pt->data_max[sock^1]);
			if (rxs == -1) {
			    if (errno == EAGAIN)
				write_log(log_file, "FOOIE! FD_ISSET, but read() returned EAGAIN [%d]!",
					  pt->sockets[sock]);
			    if ((errno != EINTR) && (errno != EAGAIN)) {
				pt->cleanup = 1;
			    }
			}
			
			if (rxs == 0) {
			    write_log(log_file, "socket closed.  cleaning up.....");
			    pt->cleanup = 1;
			}
			
			if (rxs > 0) {
			    
			    pt->data_len[sock^1] =
				fp_process(pt->data_buff[sock^1],
					   rxs,
					   pt->data_max[sock^1],
					   pt->filter[sock],
					   pt);
			}
		    }
		    
		} // End of for(sock...)
		
		pt = pt->next;
		
	    } // End of while(pt)

	    break;
	} // End of switch(sv)
	
    } // End of while(1)
    
    return 0;
}

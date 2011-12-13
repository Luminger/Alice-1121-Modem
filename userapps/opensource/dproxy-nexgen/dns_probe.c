#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <syslog.h>

#include <net/if.h>
#include <linux/sockios.h>
#include <sys/uio.h>

#include "conf.h"

void dns_probe()
{
   debug("dns_probe: start.........\n");
   if(!doit(1))
   {
      name_server = config.name_server;
      debug("dns_probe: Primary Server %s...........OK\n", name_server);
   }
   else if(!doit(2))
   {
      name_server = config.name_server2;
      debug("dns_probe: Secondary Server %s.........OK\n", name_server);
   }
   else
   {
      name_server = NULL;
      debug("dns_probe: Primary/Secondary Server....Down\n", name_server);
   }
}


/*
** If verify = 0, we are in the loop after we know both ip address of dns servers are good.
** If verify =1 , we are verifying the primary dns server's ip address
** If verify =2 , we are verifying the secondary dns server's ip address
**
** -1 failed; 0 success;
*/
int doit(int verify)
{
	struct sockaddr_in	cli_addr;
	struct sockaddr_in	serv_addr;
	int ret;
	int ntries = 0;
    int sockfd;

	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	if ( verify == 2) /* check the IP address of secondary dns server */
          serv_addr.sin_addr.s_addr = inet_addr(config.name_server2);
	else
          serv_addr.sin_addr.s_addr = inet_addr(config.name_server);

	serv_addr.sin_port = htons(PORT); /* 53 */

	if ( (sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		perror("socket");

	bzero((char *)&cli_addr,sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(0); 
	if ( bind(sockfd,(struct sockaddr *)&cli_addr,sizeof(cli_addr)) <0)
		perror("bind");

	/*
	** Do dns type A query every  30 secs to ensure the health of named
	** If you do not receive a response retry 3 times and then switch to
	** the secondary DNS server. DNS uses UDP and these packets can be
	** lost on the internet so it is safer to retry a couple of times
	** before giving it up.
	*/
	for ( ; ntries < 3; ntries++ ) {
		ret = do_dns_query(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
		if ( !ret ) { 
			//printf("dns query failed \n");
			syslog(LOG_WARNING,"dns query failed \n");
			//close(sockfd);
			//return -1;
			continue;
		} else {
			close(sockfd);
			return 0;
		}
	}
	close (sockfd);
	return -1;
}

int
do_dns_query(int sockfd, struct sockaddr *serv_addr,int servlen)
{
	int nbytes;
	char buf[128];
	char rbuf[256];
	char *ptr;
	short one,try=0;
	short n=0;
	short answers;


	ptr = buf;
	*((u_short *)ptr) = htons(1234); /* id */
	ptr += 2;
	*((u_short *)ptr) = htons(0x0100); /* flag */
	ptr += 2;
	*((u_short *)ptr) = htons(1); /* number of questions */
	ptr += 2;
	*((u_short *)ptr) = htons(0); /* number of answers RR */
	ptr += 2;
	*((u_short *)ptr) = htons(0); /* number of Authority RR */
	ptr += 2;
	*((u_short *)ptr) = htons(0); /* number of Additional RR */
	ptr += 2;

	memcpy(ptr,"\001a\014root-servers\003net\000",20);
	//memcpy(ptr,"\003www\005yahoo\003com\000",15);
	ptr += 20;
	//ptr += 15;
	one = htons(1);
	memcpy(ptr,&one,2);		/* query type = A */
	ptr += 2;
	memcpy(ptr,&one,2);		/* query class = 1 (IP addr) */
	ptr += 2;

	//nbytes = 31;
	nbytes = 36;
	if ( sendto(sockfd,buf,nbytes,0,serv_addr,servlen) != n)
	//	perror("sendto");

	fcntl(sockfd,F_SETFL,O_NONBLOCK);
	//printf("recvfrom.. \n");
tryagain:
	try++;
	n = recvfrom(sockfd,rbuf,128,0,(struct sockaddr *)0,(int *)0);
	if ( n == -1 && errno == EAGAIN && try <3) {
		sleep(1);
		goto tryagain;
	}
	else if (n >= 0) { /* good answer */
	  rbuf[n]=0;
	  answers = htons(*(short *)&rbuf[6]);
	  //printf("n=%d answer=%d \n",n,htons(*(short *)&rbuf[6]));
	  if ( answers >= 1) /* good we got an answer */
		return 1;
	  else
		return 0;
	}
	  else
		return 0;
}
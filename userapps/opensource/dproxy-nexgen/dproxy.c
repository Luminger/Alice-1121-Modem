#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <syslog.h>

#include "dproxy.h"
#include "dns_decode.h"
#include "cache.h"
#include "conf.h"
#include "dns_list.h"
#include "dns_construct.h"
#include "dns_io.h"
#include "dns_dyn_cache.h"
/*****************************************************************************/
/*****************************************************************************/
int dns_main_quit;
int dns_sock;
fd_set rfds;
dns_request_t *dns_request_list;
#ifdef DNS_DYN_CACHE
dns_dyn_cache_t *dns_dyn_cache_list;
int dns_dyn_cache_entries;
#endif 

//BRCM
char *name_server = NULL;

//BRCM
int dns_wanup = 0;

//BRCM
void get_staticDns()
{
  FILE* pFile = NULL;
  char  nameSvrLine[CONF_PATH_LEN];
  char  nameBuf[64];
  
  if((pFile = fopen( STATIC_DNS_FILE_DEFAULT , "r")) != NULL)
  {
      if(fgets(nameSvrLine, CONF_PATH_LEN, pFile))
      {
          nameSvrLine[strlen(nameSvrLine)-1] = 0;
          sscanf(nameSvrLine,"%s %s",nameBuf, config.name_server);
      }

      if(fgets(nameSvrLine, CONF_PATH_LEN, pFile))
      {
          nameSvrLine[strlen(nameSvrLine)-1] = 0;
          sscanf(nameSvrLine,"%s %s",nameBuf, config.name_server2);
      }

      debug("DProxy: Get static dns1 %s, dns2 %s from %s\n", 
               config.name_server, config.name_server2, STATIC_DNS_FILE_DEFAULT);
      
      fclose(pFile);

      name_server = config.name_server;

      //If dproxy gets static DNS from file, dproxy will regard that wan is in up status 
      dns_wanup = 1;
  }
  else
  {
      dns_wanup = 0; 
  }
}

/*****************************************************************************/
int is_connected()
{
  FILE *fp;
#if 0 //BRCM
  if(!config.ppp_detect)return 1;

  fp = fopen( config.ppp_device_file, "r" );
  if(!fp)return 0;
  fclose(fp);
  return 1;
#endif
  //BRCM
  return dns_wanup;
}
/*****************************************************************************/
int dns_init()
{
  struct sockaddr_in sa;
  struct in_addr ip;

  /* Clear it out */
  memset((void *)&sa, 0, sizeof(sa));
    
  dns_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  /* Error */
  if( dns_sock < 0 ){
	 debug_perror("Could not create socket");
	 exit(1);
  } 

  ip.s_addr = INADDR_ANY;
  sa.sin_family = AF_INET;
  memcpy((void *)&sa.sin_addr, (void *)&ip, sizeof(struct in_addr));
  sa.sin_port = htons(PORT);
  
  /* bind() the socket to the interface */
  if (bind(dns_sock, (struct sockaddr *)&sa, sizeof(struct sockaddr)) < 0){
	 debug_perror("dns_init: bind: Could not bind to port");
	 exit(1);
  }

  dns_main_quit = 0;

  FD_ZERO( &rfds );
  FD_SET( dns_sock, &rfds );

  dns_request_list = NULL;

#ifdef DNS_DYN_CACHE
  dns_dyn_cache_list = NULL;
  dns_dyn_cache_entries = 0;
#endif

  cache_purge( config.purge_time );

  //BCM
  get_staticDns();
  
  return 1;
}
/*****************************************************************************/
void dns_handle_new_query(dns_request_t *m)
{
  struct in_addr in;
  int retval = -1;
  dns_dyn_cache_t *dns_entry;
  time_t t;
 
  if((m->message.question[0].type != A) &&
     (m->message.question[0].type != PTR))
    return;
    
  /*BRCM: support IPv4 only*/
  if( m->message.question[0].type == A /*|| m->message.question[0].type == AAA*/){
      /* standard query */
      retval = cache_lookup_name( m->cname, m->ip );
      
#ifdef DNS_DYN_CACHE
      if(1 != retval) {
        dns_entry = dns_dyn_cache_list_find(dns_dyn_cache_list, m);
        if(dns_entry) {
          strncpy(m->ip, inet_ntoa(dns_entry->ip_addr), 20);
          time(&t);
          m->ttl = abs(dns_entry->timestamp - t);
          retval = 1;
        }
        else
          retval = 0;
      }
#endif

  }else if( m->message.question[0].type == PTR ) {
      /* reverse lookup */
      retval = cache_lookup_ip( m->ip, m->cname );

#ifdef DNS_DYN_CACHE
      if(1 != retval) {
        dns_entry = dns_dyn_cache_list_find(dns_dyn_cache_list, m);
        if(dns_entry) {
          strncpy(m->cname, dns_entry->cname, strlen(dns_entry->cname));
          time(&t);
          m->ttl = abs(dns_entry->timestamp - t);
          retval = 1;
        }
        else 
          retval = 0;
      }
#endif

  }
  else //BRCM
  {
      retval = 0;
  }

  debug(".......... %s ---- %s\n", m->cname, m->ip );
  
  switch( retval )
  {
     case 0:
        if( is_connected() && name_server ){
    	    debug("Adding to list-> id: %d\n", m->message.header.id);
    	    dns_request_list = dns_list_add( dns_request_list, m );
    	    /* relay the query untouched */
    	    inet_aton(name_server, &in );
    	    dns_write_packet( dns_sock, in, PORT, m );
        }else{
            debug("dns_handle_new_query, No DNS information......\n");
            inet_aton("128.9.0.107", &in );
            debug("dns_handle_new_query, Sent Request To 128.9.0.107\n");				
            dns_write_packet( dns_sock, in, PORT, m );				
            return;
        }
        break;
     case 1:
        dns_construct_reply( m );
        dns_write_packet( dns_sock, m->src_addr, m->src_port, m );
        debug("Cache hit\n");
        break;
     default:
        debug("Unknown query type: %d\n", m->message.question[0].type );
   }
}
/*****************************************************************************/
void dns_handle_request(dns_request_t *m)
{
  dns_request_t *ptr = NULL;

  /* request may be a new query or a answer from the upstream server */
  ptr = dns_list_find_by_id( dns_request_list, m );

  if( ptr != NULL ){
      debug("Found query in list; id 0x%04x flags 0x%04x\n ip %s --- cname %s\n", 
             m->message.header.id, m->message.header.flags.flags, m->ip, m->cname);
      
      /* message may be a response */
      if( m->message.header.flags.flags & 0x8000)
      {
          dns_write_packet( dns_sock, ptr->src_addr, ptr->src_port, m );
          debug("Replying with answer from %s, id 0x%04x\n", inet_ntoa( m->src_addr), m->message.header.id);
          dns_request_list = dns_list_remove( dns_request_list, ptr );         
      #if 0 //BRCM: Don't write to cache for saving device resource.
          if( m->message.header.flags.f.rcode == 0 ){ // lookup was succesful
              debug("Cache append: %s ----> %s\n", m->cname, m->ip );
              cache_name_append( m->cname, m->ip );
          }
      #endif
#ifdef DNS_DYN_CACHE
        dns_dyn_cache_list = 
                 dns_dyn_cache_list_add(dns_dyn_cache_list, m);
#endif
      }
      else
      {
          debug("Duplicate query\n");
          dns_write_packet( dns_sock, ptr->src_addr, ptr->src_port, m );
      }
  }
  else
  {
      dns_handle_new_query( m );
  }
}
/*****************************************************************************/
int dns_main_loop()
{
    struct timeval tv;
    fd_set active_rfds;
    int retval;
    dns_request_t m;
    dns_request_t *ptr, *next;

    //BRCM
    int probe_time = PROBE_TIMEOUT;   
    int purge_time = PURGE_TIMEOUT;

    if(is_connected())
        dns_probe();

    while( !dns_main_quit )
    {

        /* set the one second time out */
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        /* now copy the main rfds in the active one as it gets modified by select*/
        active_rfds = rfds;

        retval = select( FD_SETSIZE, &active_rfds, NULL, NULL, &tv );

        if (retval){
            /* data is now available */
            bzero(&m, sizeof(dns_request_t));
            dns_read_packet( dns_sock, &m );
            dns_handle_request( &m );
        }else{
            /* select time out */
            ptr = dns_request_list;
            while( ptr ){
                next = ptr->next;
                ptr->time_pending++;
                if( ptr->time_pending > DNS_TIMEOUT ){
                    debug("Request timed out\n");
                    /* send error back */
                    dns_construct_error_reply(ptr);
                    dns_write_packet( dns_sock, ptr->src_addr, ptr->src_port, ptr );
                    dns_request_list = dns_list_remove( dns_request_list, ptr );
                }
                ptr = next;
            } /* while(ptr) */

            probe_time--;
            if(!probe_time)
            {
                if(is_connected())
                    dns_probe();
                probe_time = PROBE_TIMEOUT;
            }

            /* purge cache */
            purge_time--;
            if( !purge_time ){
                cache_purge( config.purge_time );
                purge_time = PURGE_TIMEOUT;
            }
        } /* if (retval) */
    }  
  return 0;
}

//BRCM
#ifdef DPROXY_DEBUG
/*****************************************************************************/
void debug_perror( char * msg ) {
	debug( "%s : %s\n" , msg , strerror(errno) );
}

/*****************************************************************************/
void debug(char *fmt, ...)
{
#define MAX_MESG_LEN 1024
  
  va_list args;
  char text[ MAX_MESG_LEN ];
  
  sprintf( text, "[ %d ]: ", getpid());
  va_start (args, fmt);
  vsnprintf( &text[strlen(text)], MAX_MESG_LEN - strlen(text), fmt, args);	 
  va_end (args);
  
  printf(text);
#if 0 //BRCM 
  if( config.debug_file[0] ){
	 FILE *fp;
	 fp = fopen( config.debug_file, "a");
	 if(!fp){
		syslog( LOG_ERR, "could not open log file %m" );
		return;
	 }
	 fprintf( fp, "%s", text);
	 fclose(fp);
  }
  

  /** if not in daemon-mode stderr was not closed, use it. */
  if( ! config.daemon_mode ) {
	 fprintf( stderr, "%s", text);
  }
#endif
}

#endif
/*****************************************************************************
 * print usage informations to stderr.
 * 
 *****************************************************************************/
void usage(char * program , char * message ) {
  fprintf(stderr,"%s\n" , message );
  fprintf(stderr,"usage : %s [-c <config-file>] [-d] [-h] [-P]\n", program );
  fprintf(stderr,"\t-c <config-file>\tread configuration from <config-file>\n");
  fprintf(stderr,"\t-d \t\trun in debug (=non-daemon) mode.\n");
  fprintf(stderr,"\t-D \t\tDomain Name");
  fprintf(stderr,"\t-P \t\tprint configuration on stdout and exit.\n");
  fprintf(stderr,"\t-h \t\tthis message.\n");
}
/*****************************************************************************
 * get commandline options.
 * 
 * @return 0 on success, < 0 on error.
 *****************************************************************************/
int get_options( int argc, char ** argv ) 
{
  char c = 0;
  int not_daemon = 0;
  int want_printout = 0;
  char * progname = argv[0];

  conf_defaults();
#if 1 
  while( (c = getopt( argc, argv, "cD:dhP")) != EOF ) {
    switch(c) {
	 case 'c':
  		conf_load(optarg);
		break;
	 case 'd':
		not_daemon = 1;
		break;
     case 'D':
        strcpy(config.domain_name, optarg);
        break;
	 case 'h':
		usage(progname,"");
		return -1;
	 case 'P':
		want_printout = 1;
		break;
	 default:
		usage(progname,"");
		return -1;
    }
  }
#endif

#if 0  
  /** unset daemon-mode if -d was given. */
  if( not_daemon ) {
	 config.daemon_mode = 0;
  }
  
  if( want_printout ) {
	 conf_print();
	 exit(0);
  }
#endif 

  return 0;
}
/*****************************************************************************/
void sig_hup (int signo)
{
  signal(SIGHUP, sig_hup); /* set this for the next sighup */
  conf_load (config.config_file);
}
/*****************************************************************************/
int main(int argc, char **argv)
{

  /* get commandline options, load config if needed. */
  if(get_options( argc, argv ) < 0 ) {
	  exit(1);
  }

  signal(SIGHUP, sig_hup);

  dns_init();

//BRCM: Don't fork a task again!
#if 0 
  if (config.daemon_mode) {
    /* Standard fork and background code */
    switch (fork()) {
	 case -1:	/* Oh shit, something went wrong */
		debug_perror("fork");
		exit(-1);
	 case 0:	/* Child: close off stdout, stdin and stderr */
		close(0);
		close(1);
		close(2);
		break;
	 default:	/* Parent: Just exit */
		exit(0);
    }
  }
#endif

  dns_main_loop();

  return 0;
}


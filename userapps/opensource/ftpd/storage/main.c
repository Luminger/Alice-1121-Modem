/*

bftpd Copyright (C) 1999-2003 Max-Wilhelm Bruker

This program is is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2 of the
License as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <config.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_ASM_SOCKET_H
#include <asm/socket.h>
#endif
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_WAIT_H
# include <wait.h>
#else
# ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
# endif
#endif

#include "main.h"
#include "cwd.h"
#include "mystring.h"
#include "logging.h"
#include "dirlist.h"
#include "bftpdutmp.h"
#include "options.h"
#include "login.h"
#include "list.h"

//brcm
#include "fwsyscall.h"
#include "../../broadcom/cfm/inc/cfmInetd.h"

#define FTPD_TIMEOUT    300

// brcm int global_argc;
//brcm char **global_argv;
struct sockaddr_in name;
int isparent = 1;
int sock     = -1;
int listensocket;
// brcm FILE *passwdfile = NULL, *groupfile = NULL, *devnull;
struct sockaddr_in remotename;
char *remotehostname;
int control_timeout, data_timeout;
int alarm_type = 0;

//inetd struct bftpd_list_element *child_list;

/* Command line parameters */
// brcm char *configpath = NULL;    
int daemonmode = 1;         // brcm was 0;
int glbAccessMode;          // global for login check
char glbIfName[32];

#if 0 // brcm bengin
void print_file(int number, char *filename)
{
	FILE *phile;
	char foo[256];
	phile = fopen(filename, "r");
	if (phile) {
		while (fgets(foo, sizeof(foo), phile)) {
			foo[strlen(foo) - 1] = '\0';
			control_printf(SL_SUCCESS, "%i-%s", number, foo);
		}
		fclose(phile);
	}
}
#endif // brcm end

void end_child()
{
#if 0 //brcm	
   if (passwdfile)
		fclose(passwdfile);
	if (groupfile)
		fclose(groupfile);

	config_end();
	bftpd_log("Quitting.\n");
   bftpd_statuslog(1, 0, "quit");
   // brcm bftpdutmp_end();
	log_end();
	login_end();
   bftpd_cwd_end();
#endif //brcm
   if (daemonmode) {
#if 1 //inetd
      remove("/var/run/ftpd_pid");
	   close(APP_LISTEN_FD);
//	   close(IPC_CONN_FD);
      if (sock > 0)
#endif
         close(sock);
#if 0 //brcm
      close(0);
		close(1);
		close(2);
#endif //brcm
	}
}

#if 0 //inetd
void handler_sigchld(int sig)
{
	pid_t pid;
	int i;
	struct bftpd_childpid *childpid;

	pid = wait(NULL);					/* Get the child's return code so that the zombie dies */

	for (i = 0; i < bftpd_list_count(child_list); i++) {
		childpid = bftpd_list_get(child_list, i);
		if (childpid->pid == pid) {
			close(childpid->sock);
			bftpd_list_del(&child_list, i);
			free(childpid);
         break;
		}
	}
}
#endif

void handler_sigterm(int signum)
{
	exit(0);					/* Force normal termination so that end_child() is called */
}

void handler_sigalrm(int signum)
{
    if (alarm_type) {
        close(alarm_type);
//brcm        bftpd_log("Kicked from the server due to data connection timeout.\n");
        control_printf(SL_FAILURE, "421 Kicked from the server due to data connection timeout.");
        exit(0);
    } else {
//brcm        bftpd_log("Kicked from the server due to control connection timeout.\n");
        control_printf(SL_FAILURE, "421 Kicked from the server due to control connection timeout.");
        exit(0);
    }
}
#if 0 //brcm
void init_everything()
{

	if (!daemonmode) {
		config_init();
        hidegroups_init();
    }

	log_init();
    bftpdutmp_init();
	login_init();
}
#endif //brcm

int main(int argc, char **argv)
{
	char str[MAXCMD + 1];
	static struct hostent *he;
	int i = 1, port;
   FILE    *pid_fp;

	// brcm int retval;
#if 0 // brcm begin
	while (((retval = getopt(argc, argv, "c:hdDin"))) > -1) {
		switch (retval) {
			case 'h':
				printf(
					"Usage: %s [-h] [-i|-d|-D] [-c <filename>|-n]\n"
					"-h print this help\n"
					"-i (default) run from inetd\n"
					"-d daemon mode: fork() and run in TCP listen mode\n"
					"-D run in TCP listen mode, but don't pre-fork()\n"
					"-c read the config file named \"filename\" instead of " PATH_BFTPD_CONF "\n"
					"-n no config file, use defaults\n", argv[0]);
				return 0;
			case 'i': daemonmode = 0; break;
			case 'd': daemonmode = 1; break;
			case 'D': daemonmode = 2; break;
			case 'c': configpath = strdup(optarg); break;
			case 'n': configpath = NULL; break;
		}
	}
#endif //brcm end

	if (daemonmode) {
		struct sockaddr_in new;

#if 0 //inetd
		struct sockaddr_in myaddr;

		if (daemonmode == 1) {
         if (fork()) {
				exit(0);  /* Exit from parent process */
         }
			setsid();
         int pid = 0;
         if ((pid = fork()) != 0) {
            if (!(pid_fp = fopen("/var/run/ftpd_pid", "w"))) {
               perror("/var/run/ftpd_pid");
               return 0;
            }
            fprintf(pid_fp, "%d\n", pid);
            fclose(pid_fp);
			   return 0;
         }
      }

		signal(SIGCHLD, handler_sigchld);
#if 0 //brcm		
      config_init();
    	chdir("/");
      hidegroups_init();
#endif //brcm

		listensocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef SO_REUSEADDR
		setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, (void *) &i, sizeof(i));
#endif
#ifdef SO_REUSEPORT
		setsockopt(listensocket, SOL_SOCKET, SO_REUSEPORT, (void *) &i, sizeof(i));
#endif
		memset((void *) &myaddr, 0, sizeof(myaddr));
#if 0 //brcm
      if (!((port = strtoul(config_getoption("PORT"), NULL, 10)))) 
         port = 21;
      if (!strcasecmp(config_getoption("BIND_TO_ADDR"), "any")
            || !config_getoption("BIND_TO_ADDR")[0])
			myaddr.sin_addr.s_addr = INADDR_ANY;
		else
			myaddr.sin_addr.s_addr = inet_addr(config_getoption("BIND_TO_ADDR"));
#endif //brcm use default beloww (3 lines)
      port = 21;
		myaddr.sin_port = htons(port);
      myaddr.sin_addr.s_addr = INADDR_ANY;

      if (bind(listensocket, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
         fprintf(stderr, "Bind failed: %s\n", strerror(errno));
         exit(1);
      }
      if (listen(listensocket, 5)) {
         fprintf(stderr, "Listen failed: %s\n", strerror(errno));
         exit(1);
      }
#if 0
      for (i = 0; i < 3; i++) {
         close(i);		/* Remove fd pointing to the console */
         open("/dev/null", O_RDWR);	/* Create fd pointing nowhere */
      }
#endif
#endif   //inetd

#if 1 //inetd
      fd_set          readFds;
      struct timeval  timer;
      pid_t           pid;
      int             width;
      int             selret;

      atexit(end_child);
      signal(SIGTERM, handler_sigterm);
      signal(SIGALRM, handler_sigalrm);

      listensocket = APP_LISTEN_FD;

		/* We have to do this in Linux, reinitialize timer each iteration */
		timer.tv_sec  = FTPD_TIMEOUT;
		timer.tv_usec = 0;

    	FD_ZERO(&readFds);

    	/* initialize bit mask */
	   width = 0;
		/* select on the listening socket */
		FD_SET(listensocket, &readFds);
      if (listensocket > width)
         width = listensocket;
      width++;

		/* pend, waiting for one or more fds to become ready */
//		selret = select(width, &readFds, NULL, NULL, &timer);
		selret = select(width, &readFds, NULL, NULL, NULL);
      if (selret <= 0)
      {
         exit(0);
      }
      
      i = sizeof(new);
      sock = accept(listensocket, (struct sockaddr *) &new, &i);
#endif
#if 0 //inetd
      i = sizeof(new);
		while ((sock = accept(listensocket, (struct sockaddr *) &new, &i))) {
			pid_t pid;
#endif
			/* If accept() becomes interrupted by SIGCHLD, it will return -1.
			 * So in order not to create a child process when that happens,
			 * we have to check if accept() returned an error.
			 */
			if (sock > 0) {
#if 0 //inetd
            if (bftpd_list_count(child_list) > 0) {
               printf("Only allow one ftp session.\n");
               close(sock);
               continue;
            }
#endif
            glbAccessMode = bcmCheckEnable("ftp", ((struct sockaddr_in *)&new)->sin_addr);
            if (glbAccessMode == CLI_ACCESS_DISABLED) {
               close(sock);
//inetd               continue;
               exit(0);
            }
            if (glbAccessMode == CLI_ACCESS_REMOTE) {
               glbIfName[0] = '\0';
               if ((bcmGetIntfNameSocket(sock, glbIfName) != 0) || glbIfName[0] == '\0') {
                  printf("Failed to get remote ifc name!\n");
                  close(sock);
//inetd                  continue;
                  exit(0);
               }
            }
#if 0 //inetd
				pid = fork();
				if (!pid) {
#endif
#if 0 //brcm
               close(0);
               close(1);
               close(2);
#endif //brcm
               isparent = 0;
               dup2(sock, fileno(stdin));
               dup2(sock, fileno(stderr));
#if 0 //inetd
					break;
            } else {
               struct bftpd_childpid *tmp_pid = malloc(sizeof(struct bftpd_childpid));
               tmp_pid->pid = pid;
               tmp_pid->sock = sock;
               bftpd_list_add(&child_list, tmp_pid);
            }
         }  //if (sock > 0)
#endif
#if 1 //inetd
            close(listensocket);
         }  //if (sock > 0)
         else
            exit(0);
#endif
//		}  //while ((sock = accept(listensocket,.....
   }  //if (daemonmode...

#if 0 //brcm	
   devnull = fopen("/dev/null", "w");

   global_argc = argc;
   global_argv = argv;
   init_everything();
#endif //brcm
#if 0 //inetd
   atexit(end_child);

   signal(SIGTERM, handler_sigterm);
   signal(SIGALRM, handler_sigalrm);
#endif
#if 0 //brcm set the default
   control_timeout = strtoul(config_getoption("CONTROL_TIMEOUT"), NULL, 0);
   if (!control_timeout)
      control_timeout = 300;

   data_timeout = strtoul(config_getoption("DATA_TIMEOUT"), NULL, 0);
   if (!data_timeout)
      data_timeout = 300;
   xfer_bufsize = strtoul(config_getoption("XFER_BUFSIZE"), NULL, 0);
   if (!xfer_bufsize)
      xfer_bufsize = 4*1024; //brcm 4mb for now
#endif //brcm set the default beloww (3 lines)
   control_timeout = FTPD_TIMEOUT;
   data_timeout = FTPD_TIMEOUT;
//   xfer_bufsize = 4*1024;
   xfer_bufsize = 512;

   i = sizeof(remotename);
   if (getpeername(fileno(stderr), (struct sockaddr *) &remotename, &i)) {
      control_printf(SL_FAILURE, "421-Could not get peer IP address.\r\n421 %s.",
		               strerror(errno));
      return 0;
   }

	i = 1;
   setsockopt(fileno(stdin), SOL_SOCKET, SO_OOBINLINE, (void *) &i, sizeof(i));
   setsockopt(fileno(stdin), SOL_SOCKET, SO_KEEPALIVE, (void *) &i, sizeof(i));
#if 0 //brcm
	/* If option is set, determine the client FQDN */
	if (!strcasecmp((char *) config_getoption("RESOLVE_CLIENT_IP"), "yes")) {
		if ((he = gethostbyaddr((char *) &remotename.sin_addr,
								sizeof(struct in_addr), AF_INET)))
			remotehostname = strdup(he->h_name);
		else
			remotehostname = strdup(inet_ntoa(remotename.sin_addr));
	} else
#endif //brcm use the default below
   remotehostname = strdup(inet_ntoa(remotename.sin_addr));

#if 0 //brcm
   bftpd_log("Incoming connection from %s.\n", remotehostname);
   bftpd_statuslog(1, 0, "connect %s", remotehostname);
#endif //brcm
   i = sizeof(name);
   getsockname(fileno(stdin), (struct sockaddr *) &name, &i);


//brcm	print_file(220, config_getoption("MOTD_GLOBAL"));

   /* Parse hello message */
   // brcm strcpy(str, (char *) config_getoption("HELLO_STRING"));
   strcpy(str, "Ftp firmware update utility");     //brcm
   //brcm replace(str, "%v", VERSION);
   if (strstr(str, "%h")) {
      if ((he = gethostbyaddr((char *) &name.sin_addr, sizeof(struct in_addr), AF_INET)))
			replace(str, "%h", he->h_name);
		else
			replace(str, "%h", (char *) inet_ntoa(name.sin_addr));
   }

   replace(str, "%i", (char *) inet_ntoa(name.sin_addr));
   control_printf(SL_SUCCESS, "220 %s", str);

   /* Read lines from client and execute appropriate commands */
   while (fgets(str, sizeof(str), stdin)) {
// printf("after while (fgets(str = %s, stdin\n", str);  // brcm
      alarm(control_timeout);
      str[strlen(str) - 2] = 0;
//brcm        bftpd_statuslog(2, 0, "%s", str);
#ifdef DEBUG
//		bftpd_log("Processing command: %s\n", str);
#endif
      parsecmd(str);
   }
   return 0;
}

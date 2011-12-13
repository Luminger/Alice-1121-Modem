/* vi: set sw=4 ts=4: */
/*
 * ftpget
 *
 * Mini implementation of FTP to retrieve a remote file.
 *
 * Copyright (C) 2002 Jeff Angielski, The PTR Group <jeff@theptrgroup.com>
 * Copyright (C) 2002 Glenn McGrath <bug1@iinet.net.au>
 *
 * Based on wget.c by Chip Rosenthal Covad Communications
 * <chip@laserlink.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "busybox.h"

typedef struct ftp_host_info_s {
	char *user;
	char *password;
	struct sockaddr_in *s_in;
} ftp_host_info_t;

#ifdef CPE_DSL_MIB
static char verbose_flag = 0;
static char do_continue = 0;

static int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf)
{
	if (verbose_flag) {
		bb_error_msg("cmd %s%s", s1, s2);
	}

	if (s1) {
		if (s2) {
			fprintf(stream, "%s%s\r\n", s1, s2);
		} else {
			fprintf(stream, "%s\r\n", s1);
		}
	}
	do {
		char *buf_ptr;

		if (fgets(buf, 510, stream) == NULL) {
			bb_perror_msg_and_die("fgets()");
		}
		buf_ptr = strstr(buf, "\r\n");
		if (buf_ptr) {
			*buf_ptr = '\0';
		}
	} while (! isdigit(buf[0]) || buf[3] != ' ');

	return atoi(buf);
}

static int xconnect_ftpdata(ftp_host_info_t *server, const char *buf)
{
	char *buf_ptr;
	unsigned short port_num;

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;

	server->s_in->sin_port=htons(port_num);
	return(xconnect(server->s_in));
}

#ifdef CONFIG_FTPGET
//BRCM begin  
#include "../../ftpd/fwsyscall.h"
#include "bcmTag.h"

// following are from #include "/opt/CommEngine/userapps/broadcom/snmp/agent/include/cpeDslMib.h"
#define CPE_FTP_NAME_MAX_LEN                 128
#define CPE_FTP_PASSWORD_MAX_LEN             128
#define CPE_FTP_FILE_NAME_MAX_LEN            255
#define CPE_FTP_FILE_NAME_MIN_LEN            4
#define CPE_FTP_DEFAULT_FILE                 "ftpFile"
#define CPE_FTP_ADMIN_STATUS_STOP            1
#define CPE_FTP_ADMIN_STATUS_UPGRADE         2
#define CPE_FTP_ADMIN_STATUS_FTPTEST         3
#define CPE_FTP_OPER_STATUS_NORMAL           1
#define CPE_FTP_OPER_STATUS_CONNECT_SUCCESS  2
#define CPE_FTP_OPER_STATUS_CONECT_FAILURE   3
#define CPE_FTP_OPER_STATUS_DOWNLOADING      4
#define CPE_FTP_OPER_STATUS_DOWNLOAD_SUCCESS 5
#define CPE_FTP_OPER_STATUS_DOWNLOAD_FAILURE 6
#define CPE_FTP_OPER_STATUS_SAVING           7
#define CPE_FTP_OPER_STATUS_SAVE_FAILURE     8
#define CPE_FTP_OPER_STATUS_UPGRADE_SUCCESS  9
#define CPE_FTP_OPER_STATUS_UPGRADE_FAIL     10

#define FTP_RESULT_UPDATE_INTERVAL     1
// use soem globals to reduce the messiness of change the original tftp function.
static int glbNeedReset = FALSE;
static char *glbImagePtr = NULL;
static int glbUploadSize = 0;
static char *glbCurPtr = NULL;
static unsigned int glbTotalAllocatedSize = 0;
static int glbStartTime;
static int glbTotal_size = 0;
static int glbRead_total = 0;

extern char glbIfName[];
extern UPLOAD_RESULT verifyTag(PFILE_TAG pTag, int passNumber);

static FILE *ftp_login(ftp_host_info_t *server)
{
	FILE *control_stream;
	char buf[512];
    int sock;

	/* Connect to the command socket */
	control_stream = fdopen(xconnect(server->s_in), "r+");
	if (control_stream == NULL) {
		bb_perror_msg_and_die("Couldnt open control stream");
	}

    // get sock from control_stream and then get ifc the sock is sitting on.
    sock = fileno(control_stream);
    glbIfName[0] = '\0';
    if ((bcmGetIntfNameSocket(sock, glbIfName) != 0) || glbIfName[0] == '\0')
        printf("Failed to get remote ifc name!\n");

	if (ftpcmd(NULL, NULL, control_stream, buf) != 220) {
		bb_error_msg_and_die("%s", buf + 4);
	}

	/*  Login to the server */
    switch (ftpcmd("USER ", server->user, control_stream, buf)) {
	case 230:
		break;
	case 331:
		if (ftpcmd("PASS ", server->password, control_stream, buf) != 230) {
			bb_error_msg_and_die("PASS error: %s", buf + 4);
		}
		break;
	default:
		bb_error_msg_and_die("USER error: %s", buf + 4);
	}

	ftpcmd("TYPE I", NULL, control_stream, buf);

	return(control_stream);
}

static void ftp_log(int status, int totalSize, int doneSize, int elapseTime)
{
    static FILE *ftpFile = NULL;    // store ftp statistics
    ftpFile = fopen ("/var/ftpStats", "w");
    if (ftpFile == NULL)
        bb_error_msg_and_die("ftp error: failed to open file\n");
  
    fprintf(ftpFile, "operStatus = %d totalSize = %d doneSize = %d elapseTime = %d\n", 
        status, totalSize/1024, doneSize/1024, elapseTime);
    fclose(ftpFile);
}

static int myWrite(char *inBuf, int inBufLen)
{
   if (glbCurPtr == NULL) 
   {
	  killAllApps();

      glbNeedReset = TRUE;
      // The first data that is downloaded is the FILE_TAG structure.
      // After the entire FILE_TAG structure has been downloaded, use
      // the totalImageSize field to determine the size of inBuf to
      // allocate.
      PFILE_TAG pTag = (PFILE_TAG) inBuf;

      if (verifyTag(pTag, 1) == UPLOAD_OK) 
      {
         // if chip id mismatched.
         if (checkChipId(pTag->chipId, pTag->signiture_2) != 0)
            return -1;
         // add tag len plus some inBuf for over flow during the sending...
         glbTotalAllocatedSize =  atoi(pTag->totalImageLen) + TAG_LEN + 8;
         printf( "Allocating %d bytes for broadcom image.\n", glbTotalAllocatedSize );
      }
      // try to allocate the memory for .w image or psi, same as flash size + some overhead
      else 
      {   
         glbTotalAllocatedSize = sysFlashSizeGet() + TOKEN_LEN;
         printf("Allocating %d bytes for flash image.\n", glbTotalAllocatedSize);
      }
      glbCurPtr = (char *) malloc(glbTotalAllocatedSize);
      if (glbCurPtr == NULL) 
      {
         printf("Not enough memory error.");   
         return -1;
      }
      printf("Memory allocated\n");
      glbImagePtr = glbCurPtr;
   } // if glbCurPtr == NULL

   memcpy(glbCurPtr, inBuf, inBufLen);
   glbCurPtr += inBufLen;
   glbUploadSize += inBufLen;
   if (glbUploadSize > glbTotalAllocatedSize) // try not to over flow the inBuf
   {       
       printf("Failed on corrupted data or over sized image size.\n");
       return -1;
   }
   return inBufLen;
}


// from copyfd.c function bb_full_fd_action
static size_t brcm_get_ftp_data(int src_fd, const size_t size, int ftp_upgrade)
{
	size_t read_total = 0;
    struct timeval tim;

    gettimeofday(&tim, NULL);
    glbStartTime = (int) tim.tv_sec;
    int ftp_status = CPE_FTP_OPER_STATUS_NORMAL;
    FILE *ftpPid = NULL;
    int elapseTime, currentTime;

    // init ftp log
    ftp_log(ftp_status, (int)size, 0,  0);

    if ((ftpPid = fopen ("/var/ftpPid", "w")) != NULL) {
        fprintf(ftpPid,"%d\n",getpid());
        (void)fclose(ftpPid);
    }
    else {
        bb_perror_msg(bb_msg_write_error);
        return read_total;
    }

    RESERVE_CONFIG_BUFFER(buffer,BUFSIZ);
  
    glbTotal_size = (int) size;
    currentTime = glbStartTime;
	while ((size == 0) || (read_total < size)) {
		size_t read_try;
		ssize_t read_actual;

 		if ((size == 0) || (size - read_total > BUFSIZ)) {
			read_try = BUFSIZ;
		} else {
			read_try = size - read_total;
		}

		read_actual = safe_read(src_fd, buffer, read_try);

		if (read_actual > 0) {
            if (ftp_upgrade) {
			    if (myWrite(buffer, (int)read_actual) != (int) read_actual) {
				    bb_perror_msg(bb_msg_write_error);	/* match Read error below */
				    break;
			    }
            }
		}
		else if (read_actual == 0) {
			if (size) {
				bb_error_msg("Unable to read all data");
			}
			break;
		} else {
			/* read_actual < 0 */
			bb_perror_msg("Read error");
			break;
		}

	  read_total += read_actual;
      glbRead_total = (int) read_total;
      gettimeofday(&tim, NULL);
      if (currentTime < (int) tim.tv_sec) {
         currentTime = (int) tim.tv_sec;
         ftp_log(CPE_FTP_OPER_STATUS_DOWNLOADING, glbTotal_size, glbRead_total, (currentTime - glbStartTime));
      }
	}

	RELEASE_CONFIG_BUFFER(buffer);


    gettimeofday(&tim, NULL);
    elapseTime = (int) tim.tv_sec - glbStartTime;
    printf(" *** received ftp size = %d, need size = %d and %d seconds elapsed\n", read_total, size, elapseTime);
    if (read_total != size) {
        ftp_log(CPE_FTP_OPER_STATUS_DOWNLOAD_FAILURE, (int)size, read_total, elapseTime);
        return(read_total);
    }
    
    // for ftp test only
    if (!ftp_upgrade) {
        ftp_log(CPE_FTP_OPER_STATUS_DOWNLOAD_SUCCESS, (int)size, read_total, elapseTime);
        return(read_total);
    }

    return(read_total);
}

void brcm_flashImage(int size, int read_total, int elapseTime)
{
    PARSE_RESULT imageType = NO_IMAGE_FORMAT;
	int fBufType = BUF_IMAGES;

    // for ftp firmware upgrade, need to do the image check and flash if ftp suceeds
    if (glbImagePtr != NULL) {
        if ((imageType = parseImageData(glbImagePtr, glbUploadSize, fBufType)) != NO_IMAGE_FORMAT) {
            ftp_log(CPE_FTP_OPER_STATUS_UPGRADE_SUCCESS, size, read_total, elapseTime);
            //ftp_log(CPE_FTP_OPER_STATUS_SAVING, size, read_total, elapseTime);
            sleep(3);  // for ftpStats to be read by snmp
            flashImage(glbImagePtr, imageType, glbUploadSize);
        }
        else {
            printf("Illegal firmware image\n");
            ftp_log(CPE_FTP_OPER_STATUS_UPGRADE_FAIL, size, read_total, elapseTime);
        }
    }
    else {
        printf("ftp firmware upgrade failed - no memory allocated.\n");
        ftp_log(CPE_FTP_OPER_STATUS_UPGRADE_FAIL, size, read_total, elapseTime);
    }

    if (glbImagePtr)
        free(glbImagePtr);

    if (glbNeedReset && imageType != PSI_IMAGE_FORMAT)
        sysMipsSoftReset();
}

static int ftp_recieve(ftp_host_info_t *server, FILE *control_stream,
		const char *local_path, char *server_path, int ftp_upgrade)
{
	char buf[512];
	off_t filesize = 0;
	int fd_data;
	// int fd_local = -1;
	off_t beg_range = 0;
    struct timeval tim;
    int elapseTime;

	/* Connect to the data socket */
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
		bb_error_msg_and_die("PASV error: %s", buf + 4);
	}
	fd_data = xconnect_ftpdata(server, buf);

	if (ftpcmd("SIZE ", server_path, control_stream, buf) == 213) {
		unsigned long value=filesize;
		if (safe_strtoul(buf + 4, &value))
			bb_error_msg_and_die("SIZE error: %s", buf + 4);
		filesize = value;
	}

// BRCM begin
#if 0
	if ((local_path[0] == '-') && (local_path[1] == '\0')) {
		fd_local = STDOUT_FILENO;
		do_continue = 0;
	}
#endif 
// BRCM end
	if (do_continue) {
		struct stat sbuf;
		if (lstat(local_path, &sbuf) < 0) {
			bb_perror_msg_and_die("fstat()");
		}
		if (sbuf.st_size > 0) {
			beg_range = sbuf.st_size;
		} else {
			do_continue = 0;
		}
	}

	if (do_continue) {
		sprintf(buf, "REST %ld", (long)beg_range);
		if (ftpcmd(buf, NULL, control_stream, buf) != 350) {
			do_continue = 0;
		} else {
			filesize -= beg_range;
		}
	}

	if (ftpcmd("RETR ", server_path, control_stream, buf) > 150) {
		bb_error_msg_and_die("RETR error: %s", buf + 4);
	}
//BRCM begin
#if 0
	/* only make a local file if we know that one exists on the remote server */
	if (fd_local == -1) {
		if (do_continue) {
			fd_local = bb_xopen(local_path, O_APPEND | O_WRONLY);
		} else {
			fd_local = bb_xopen(local_path, O_CREAT | O_TRUNC | O_WRONLY);
		}
	}
#endif

    if (filesize == 0) {     // not getting it from SIZE command, try it on RETR buf
        char *ptr = NULL, *ptr2 = NULL;
        ptr = strchr(buf, '(');
        if (ptr) {
            unsigned long value=filesize;
            ptr2 = strchr(ptr, ' ');
            if (ptr2)
                *ptr2 = '\0';
		    if (safe_strtoul((ptr+1), &value))
			    bb_error_msg_and_die("SIZE error: %s", buf);
		    filesize = value;
        }
        else
            printf("No size info in RETR command\n");
    }

	/* get the file */
	if (brcm_get_ftp_data(fd_data, filesize, ftp_upgrade) != filesize) {
		exit(EXIT_FAILURE);
	}
//BRCM end

	/* close it all down */
	close(fd_data);
	if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
		bb_error_msg_and_die("ftp error: %s", buf + 4);
	}

	ftpcmd("QUIT", NULL, control_stream, buf);

    printf("ftp test suceeds\n");

    gettimeofday(&tim, NULL);
    elapseTime = (int) tim.tv_sec - glbStartTime;
    if (ftp_upgrade)
        brcm_flashImage(filesize, filesize, elapseTime);

	return(EXIT_SUCCESS);
}
#endif

#ifdef CONFIG_FTPPUT
static int ftp_send(ftp_host_info_t *server, FILE *control_stream,
		const char *server_path, char *local_path)
{
	struct stat sbuf;
	char buf[512];
	int fd_data;
	int fd_local;
	int response;

	/*  Connect to the data socket */
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227) {
		bb_error_msg_and_die("PASV error: %s", buf + 4);
	}
	fd_data = xconnect_ftpdata(server, buf);

	if (ftpcmd("CWD ", server_path, control_stream, buf) != 250) {
		bb_error_msg_and_die("CWD error: %s", buf + 4);
	}
    /* get the local file */
	if ((local_path[0] == '-') && (local_path[1] == '\0')) {
		fd_local = STDIN_FILENO;
	} else {
		fd_local = bb_xopen(local_path, O_RDONLY);
		fstat(fd_local, &sbuf);

		sprintf(buf, "ALLO %lu", (unsigned long)sbuf.st_size);
		response = ftpcmd(buf, NULL, control_stream, buf);
		switch (response) {
		case 200:
		case 202:
			break;
		default:
			close(fd_local);
			bb_error_msg_and_die("ALLO error: %s", buf + 4);
			break;
		}
	}
	response = ftpcmd("STOR ", local_path, control_stream, buf);
	switch (response) {
	case 125:
	case 150:
		break;
	default:
		close(fd_local);
		bb_error_msg_and_die("STOR error: %s", buf + 4);
	}

	/* transfer the file  */
	if (bb_copyfd_eof(fd_local, fd_data) == -1) {
		exit(EXIT_FAILURE);
	}

	/* close it all down */
	close(fd_data);
	if (ftpcmd(NULL, NULL, control_stream, buf) != 226) {
		bb_error_msg_and_die("error: %s", buf + 4);
	}
	ftpcmd("QUIT", NULL, control_stream, buf);
    
	return(EXIT_SUCCESS);
}
#endif

#define FTPGETPUT_OPT_CONTINUE	        1
#define FTPGETPUT_OPT_VERBOSE	        2
#define FTPGETPUT_OPT_FIRWARE_UPGRADE   4
#define FTPGETPUT_OPT_USER	            8
#define FTPGETPUT_OPT_PASSWORD	        16
#define FTPGETPUT_OPT_PORT	            32

static const struct option ftpgetput_long_options[] = {
	{"continue", 1, NULL, 'c'},
	{"verbose", 0, NULL, 'v'},
    {"firmware", 0, NULL, 'f'},
	{"username", 1, NULL, 'u'},
	{"password", 1, NULL, 'p'},
	{"port", 1, NULL, 'P'},
	{0, 0, 0, 0}
};

int ftpgetput_main(int argc, char **argv)
{
	/* content-length of the file */
	unsigned long opt;
	char *port = "ftp";
    int ftp_upgrade = 0;

	/* socket to ftp server */
	FILE *control_stream;
	struct sockaddr_in s_in;

	/* continue a prev transfer (-c) */
	ftp_host_info_t *server;

	int (*ftp_action)(ftp_host_info_t *, FILE *, const char *, char *, int) = NULL;

	/* Check to see if the command is ftpget or ftput */
#ifdef CONFIG_FTPPUT
# ifdef CONFIG_FTPGET
	if (bb_applet_name[3] == 'p') {
		ftp_action = ftp_send;
	}
# else
	ftp_action = ftp_send;
# endif
#endif
#ifdef CONFIG_FTPGET
# ifdef CONFIG_FTPPUT
	if (bb_applet_name[3] == 'g') {
		ftp_action = ftp_recieve;
	}
# else
	ftp_action = ftp_recieve;
# endif
#endif

	/* Set default values */
	server = xmalloc(sizeof(ftp_host_info_t));
	server->user = "anonymous";
	server->password = "busybox@";
	verbose_flag = 0;
    
	/*
	 * Decipher the command line
	 */
	bb_applet_long_options = ftpgetput_long_options;
    opt = bb_getopt_ulflags(argc, argv, "cvfu:p:P:", &server->user, &server->password, &port);

	/* Process the non-option command line arguments */
	if (argc - optind != 3) {
		bb_show_usage();
	}
	if (opt & FTPGETPUT_OPT_CONTINUE) {
		do_continue = 1;
	}
	if (opt & FTPGETPUT_OPT_VERBOSE) {
		verbose_flag = 1;
	}
	if (opt & FTPGETPUT_OPT_FIRWARE_UPGRADE) {
		ftp_upgrade = 1;
	}
	/* We want to do exactly _one_ DNS lookup, since some
	 * sites (i.e. ftp.us.debian.org) use round-robin DNS
	 * and we want to connect to only one IP... */
	server->s_in = &s_in;
	bb_lookup_host(&s_in, argv[optind]);
	s_in.sin_port = bb_lookup_port(port, "tcp", 21);
	if (verbose_flag) {
		printf("Connecting to %s[%s]:%d\n",
				argv[optind], inet_ntoa(s_in.sin_addr), ntohs(s_in.sin_port));
	}

	/*  Connect/Setup/Configure the FTP session */
	control_stream = ftp_login(server);

	return(ftp_action(server, control_stream, argv[optind + 1], argv[optind + 2], ftp_upgrade));
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
#else
int ftpgetput_main(int argc, char **argv)
{
   return 0;
}
#endif

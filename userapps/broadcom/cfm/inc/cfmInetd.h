
/*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2006 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : cfmInetd.h
 *
 * Description: 
 * $Revision: $
 * $Id: $
 *----------------------------------------------------------------------*/

#ifndef __CFM_INETD_H__
#define __CFM_INETD_H__

//Inetd support
#define APP_LISTEN_FD	3
//#define IPC_CONN_FD		4

#define CLI_PIDFILE     "/var/run/cli_pid"
#define TELNETD_PIDFILE "/var/run/telnetd_pid"
#define SSHD_PIDFILE    "/var/run/sshd_pid"
#define FTPD_PIDFILE    "/var/run/ftpd_pid"
#define TFTPD_PIDFILE   "/var/run/tftpd_pid"

void cfmInetd(void);

#endif   /* __CFM_INETD_H__ */

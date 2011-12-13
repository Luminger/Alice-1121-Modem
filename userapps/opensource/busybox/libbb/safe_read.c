/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
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
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "libbb.h"



ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;
#if 1 //inetd
   struct timeval  timer;
   fd_set          readFds;
   int             width;
   int             rv;

   while (1)
   {
      /* We have to do this in Linux, reinitialize timer each iteration */
      timer.tv_sec  = 300;
      timer.tv_usec = 0;

      //clear bits in read bit mask
      FD_ZERO(&readFds);

      //initialize bit mask
      FD_SET(fd, &readFds);
      width = fd + 1;

      //pend, waiting for one or more fds to become ready
      rv = select(width, &readFds, NULL, NULL, &timer);
      if (rv == 0)
      {
         printf("Shell exited after timing out\n");
         exit(0);
      }
      else if ((rv > 0) && FD_ISSET(fd, &readFds))
      {
         n = read(fd, buf, count);
         if (n >= 0 || errno != EINTR)
            break;
      }
   }
#else
	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);
#endif

	return n;
}


/* END CODE */
/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/

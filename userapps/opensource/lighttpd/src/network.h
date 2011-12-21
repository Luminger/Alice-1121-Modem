#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "server.h"

	
#if defined(__UCLIBC_HAS_IPV6__)
#  ifndef IPV6_V6ONLY
#    define IPV6_V6ONLY 26
#  endif
#endif

int network_write_chunkqueue(server *srv, connection *con, chunkqueue *c);

int network_init(server *srv);
int network_close(server *srv);

int network_register_fdevents(server *srv);

#endif

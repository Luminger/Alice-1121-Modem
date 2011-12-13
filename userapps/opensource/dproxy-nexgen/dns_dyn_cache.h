#include <sys/socket.h>
#include <netinet/in.h>

#define DNS_DYN_CACHE_MAX_ENTRIES 100

typedef struct dns_dyn_cache
{
  struct dns_dyn_cache *next;
  time_t timestamp;
  struct in_addr ip_addr;
  char cname[0];
} dns_dyn_cache_t;


dns_dyn_cache_t *dns_dyn_cache_list_add(dns_dyn_cache_t *list, dns_request_t *m);
dns_dyn_cache_t *dns_dyn_cache_list_find(dns_dyn_cache_t *list, dns_request_t *m);
dns_dyn_cache_t *dns_dyn_cache_list_remove(dns_dyn_cache_t *list, dns_dyn_cache_t *m );
dns_dyn_cache_t *dns_dyn_cache_list_ttl_purge(dns_dyn_cache_t *list);
void dns_dyn_cache_list_print(dns_dyn_cache_t *list);

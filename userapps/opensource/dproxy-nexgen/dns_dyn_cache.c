#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dproxy.h"
#include "dns_decode.h"
/*
#include "cache.h"
#include "conf.h"
#include "dns_list.h" */
#include "dns_construct.h"
#include "dns_io.h"
#include "dns_dyn_cache.h"

#ifdef DNS_DYN_CACHE
extern int dns_dyn_cache_entries;

/*****************************************************************************/
dns_dyn_cache_t *dns_dyn_cache_list_add(dns_dyn_cache_t *list, dns_request_t *m)
{
  dns_dyn_cache_t *new_node = NULL;
  time_t t;
  struct in_addr addr;

  assert(NULL != m);

  if(0 == inet_aton(m->ip, &addr))
    return list;
  
  new_node = dns_dyn_cache_list_find(list, m);

  if(new_node)
    return list;
  
  new_node = NULL;

  /* remove an entry if max limit on entries reached */
  if(dns_dyn_cache_entries >= DNS_DYN_CACHE_MAX_ENTRIES) {
    list = dns_dyn_cache_list_ttl_purge(list); 
  }

  new_node = (dns_dyn_cache_t *)malloc(sizeof(dns_dyn_cache_t) 
                                  + strlen(m->cname) + 1);
  if( !new_node) return list;

  debug( "Addr: %x\n", new_node);

  bzero(new_node, (sizeof(dns_dyn_cache_t) + strlen(m->cname) + 1));
  time(&t);
  new_node->timestamp = t + m->ttl;
  inet_aton(m->ip, &(new_node->ip_addr));
  strncpy(new_node->cname, m->cname, strlen(m->cname));
  m->cname[strlen(m->cname)] = '\0';

  new_node->next = list;
  dns_dyn_cache_entries++;

  return new_node;
}
/*****************************************************************************/
dns_dyn_cache_t *dns_dyn_cache_list_find(dns_dyn_cache_t *list, dns_request_t *m)
{ 
  struct in_addr ip_addr;

  assert(NULL != m);

  while(list != NULL) {
     if( m->message.question[0].type == A) {
       if(0 == strncmp(list->cname, m->cname, strlen(m->cname))) {
         return list;
       }
     } else if( m->message.question[0].type == PTR ) {
       inet_aton(m->ip, &ip_addr);
       if(list->ip_addr.s_addr == ip_addr.s_addr) {
         return list;
       }
     }
     list = list->next;
  }
  return NULL;
}
/*****************************************************************************/
dns_dyn_cache_t *dns_dyn_cache_list_remove(dns_dyn_cache_t *list, dns_dyn_cache_t *m )
{
  dns_dyn_cache_t *prev, *retval;

  if( !m )return list;

  prev = list;
  while (prev){
    if( prev->next == m)break;
    prev = prev->next;
  }

  if( !prev ){ //must be at start of list
    retval = m->next;
    free( m );
    m = NULL;
  }else{
    prev->next = m->next;
    retval = list;
    free( m );
    m = NULL;
  }
  dns_dyn_cache_entries--;
  return retval;
}
/*****************************************************************************/
dns_dyn_cache_t *dns_dyn_cache_list_ttl_purge(dns_dyn_cache_t *list)
{
  dns_dyn_cache_t *list_entry = NULL;
  dns_dyn_cache_t *list_entry_next = NULL;
  dns_dyn_cache_t *list_entry_exp_ttl = NULL;
  dns_dyn_cache_t *prev = NULL;
  time_t t;

  time(&t);
  list_entry_exp_ttl = list;
  prev = list;
  for(list_entry = list; (list_entry != NULL); ) {
     list_entry_next = list_entry->next;
     /* remove expired entries */
     if (list_entry->timestamp < t) {
       if(list_entry == list) {
         list = list_entry->next;
         prev = list;
       }
       else {
         prev->next = list_entry->next;
       }
       free(list_entry);
       list_entry = NULL;
       dns_dyn_cache_entries--;
     }

     if((list_entry) && 
        (list_entry_exp_ttl->timestamp > list_entry->timestamp)) {
       list_entry_exp_ttl = list_entry;
     }

     if(list_entry)
       prev = list_entry;

     list_entry = list_entry_next;
  }

  if(dns_dyn_cache_entries >= DNS_DYN_CACHE_MAX_ENTRIES) {
    list = dns_dyn_cache_list_remove(list, list_entry_exp_ttl);
  }

  return list;
}
/*****************************************************************************/
void dns_dyn_cache_list_print(dns_dyn_cache_t *list)
{
  time_t t;
  debug("Dumping dyn_cache list:\n");

  time(&t);
  while(list) {
    debug("    ID: %p Name: %s IP: %s TTL %d\n", list,
	  list->cname, inet_ntoa(list->ip_addr), list->timestamp -t);
    list = list->next;
  }
}
#endif

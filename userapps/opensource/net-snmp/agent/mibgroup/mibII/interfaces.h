/*
 *  Interfaces MIB group interface - interfaces.h
 *
 */
#ifndef _MIBGROUP_INTERFACES_H
#define _MIBGROUP_INTERFACES_H

config_require(util_funcs)
    config_arch_require(solaris2, kernel_sunos5)
#ifdef hpux11
#include <sys/mib.h>
#else
     struct in_ifaddr;
     struct ifnet;
#endif

     int             Interface_Scan_Get_Count(void);
     int             Interface_Index_By_Name(char *, int);
     void            Interface_Scan_Init(void);
#if defined(linux) || defined(sunV3)
     struct in_ifaddr {
         int             dummy;
     };
#endif
#if defined(hpux11)
     int             Interface_Scan_Next(short *, char *, nmapi_phystat *);
#else
     int             Interface_Scan_Next(short *, char *, struct ifnet *,
                                         struct in_ifaddr *);
#endif

     void            init_interfaces(void);
     extern FindVarMethod var_interfaces;
     extern FindVarMethod var_ifEntry;

extern int getAtmIntfAdminStatus(void);
extern int setAtmIntfAdminStatus(int status);
extern int getAtmIntfOperStatus(void);
extern int getAdslOperStatus(void);
extern int getAdslAdminStatus(void);
extern int setAdslAdminStatus(int status);
extern unsigned long getAdslIfSpeed(void);

#define IFNUMBER        0
#define IFINDEX         1
#define IFDESCR         2
#define IFTYPE          3
#define IFMTU           4
#define IFSPEED         5
#define IFPHYSADDRESS   6
#define IFADMINSTATUS   7
#define IFOPERSTATUS    8
#define IFLASTCHANGE    9
#define IFINOCTETS      10
#define IFINUCASTPKTS   11
#define IFINNUCASTPKTS  12
#define IFINDISCARDS    13
#define IFINERRORS      14
#define IFINUNKNOWNPROTOS 15
#define IFOUTOCTETS     16
#define IFOUTUCASTPKTS  17
#define IFOUTNUCASTPKTS 18
#define IFOUTDISCARDS   19
#define IFOUTERRORS     20
#define IFOUTQLEN       21
#define IFSPECIFIC      22

/* brcm: some definitions IANAifType */
#define IF_IANAIFTYPE_ETHERNETCSMACD    6
#define IF_IANAIFTYPE_TOKENRING         9
#define IF_IANAIFTYPE_FDDI              15
#define IF_IANAIFTYPE_PPP               23
#define IF_IANAIFTYPE_SOFTWARELOOPBACK  24
#define IF_IANAIFTYPE_SLIP              28
#define IF_IANAIFTYPE_ARCNETPLUS        35
#define IF_IANAIFTYPE_ATM               37
#define IF_IANAIFTYPE_LOCALTALK         42
#define IF_IANAIFTYPE_HIPPI             47
#define IF_IANAIFTYPE_AAL5              49
#define IF_IANAIFTYPE_ADSL              94
#define IF_IANAIFTYPE_TUNNEL            131
/* some status definition */
#define IF_ADMIN_STATUS_UP              1
#define IF_ADMIN_STATUS_DOWN            2
#define IF_ADMIN_STATUS_TESTING         3
#define IF_OPER_STATUS_UP               1
#define IF_OPER_STATUS_DOWN             2
#define IF_OPER_STATUS_TESTING          3
#define IF_OPER_STATUS_UNKNOWN          4
#define IF_OPER_STATUS_DORMANT          5
#define IF_OPER_STATUS_NOTPRESENT       6
#define IF_OPER_STATUS_LOWER_LAYER_DOWN 7
/* some private device names */
#define IF_BRCM_DSL_DEV_NAME            "dsl0"
#define IF_BRCM_ATM_DEV_NAME            "atm0"
#define IF_BRCM_ATM_CPCS_DEV_NAME       "cpcs0"

#ifdef linux
/*
 * this struct ifnet is cloned from the generic type and somewhat modified.
 * it will not work for other un*x'es...
 */

     struct ifnet {
         char           *if_name;       /* name, e.g. ``en'' or ``lo'' */
         char           *if_unit;       /* sub-unit for lower level driver */
       //brcm if_mtu should be 32 bits not 16
       //short           if_mtu;        /* maximum transmission unit */
         int             if_mtu;        /* maximum transmission unit */
         short           if_flags;      /* up/down, broadcast, etc. */
         int             if_metric;     /* routing metric (external only) */
         char            if_hwaddr[6];  /* ethernet address */
         int             if_type;       /* interface type: 1=generic,
                                         * 28=slip, ether=6, loopback=24 */
         u_long          if_speed;      /* interface speed: in bits/sec */

         struct sockaddr if_addr;       /* interface's address */
         struct sockaddr ifu_broadaddr; /* broadcast address */
         struct sockaddr ia_subnetmask; /* interface's mask */

         struct ifqueue {
             int             ifq_len;
             int             ifq_drops;
         } if_snd;              /* output queue */
         u_long          if_ibytes;     /* octets received on interface */
         u_long          if_ipackets;   /* packets received on interface */
         u_long          if_ierrors;    /* input errors on interface */
         u_long          if_iqdrops;    /* input queue overruns */
         u_long          if_obytes;     /* octets sent on interface */
         u_long          if_opackets;   /* packets sent on interface */
         u_long          if_oerrors;    /* output errors on interface */
         u_long          if_collisions; /* collisions on csma interfaces */
         u_long          if_lastchange; /* TimeTicks (1/100ths of a second since last change) */
         /*
          * end statistics 
          */
         struct ifnet   *if_next;
     };
#endif
#endif                          /* _MIBGROUP_INTERFACES_H */

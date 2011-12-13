#ifdef SNMP_AUTOCONFIG
/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile:$
 * $brcm_Revision:$
 * $brcm_Date:$
 *
 * [File Description:]
 *
 * Revision History: 
 *
 * $brcm_Log:$
 *
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <atm.h>
#include <linux/atmdev.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "mibdefs.h"
#include <net-snmp/library/asn1.h>
#include <net-snmp/library/snmp.h>
#include <net-snmp/library/snmp_api.h>
#include <net-snmp/library/snmp_debug.h>
#include <net-snmp/library/snmp_client.h>
#include <net-snmp/library/system.h>
#include <net-snmp/library/data_list.h>
//#include "keytools.h"
//#include "tools.h"
//#include "scapi.h"
//#include "transform_oids.h"
#include "autoconfig.h"
#include "atmForum.h"
#include "atmMibDefs.h"

netsnmp_session *autoconfigSession=(netsnmp_session*)NULL;
pAUTOCONFIG_ATTACHPOINT_INFO pAutoconfigAttachPointInfo=NULL;
pAUTOCONFIG_CONFIG_INFO pAutoconfigConfigInfo=NULL;
/* list of VCC that may or may not have service type info */
netsnmp_data_list *pAutoconfigVccList=NULL;
/* contain serviceType, and profile info per VCC */
netsnmp_data_list *pAutoconfigServiceConnList=NULL;
netsnmp_data_list *pAutoconfigAALProfileList=NULL;
netsnmp_data_list *pAutoconfigServiceTypeList=NULL;
netsnmp_data_list *pAutoconfigRequestList=NULL;
int vccVarFlag=0;
int state=0;
AUTOCONFIG_VC_INFO autoconfigVcChangeInfo;

#define INDIVIDUAL_COLUMN
#define TEST_DEBUG
#define TEST 
#ifdef TEST
time_t  now;
int autoconfigSocket = 0;
int autoconfig_processRequestMessages(netsnmp_pdu *pdu,netsnmp_session * session,void *magic);
#endif /* TEST */

static void autoconfig_freeListNode(void *node);


/*****************************************************************************************
 * Summary: autoconfig_init() is the entry point for autoconfiguration module             
 * Description: open session for receiving/send ILMI messages over ILMI PVC.
 *              Before this function is called, ADSL link needs to be UP, 
 *              an ILMI PVC has already been created.
 * Parameters: 
 *              input: pvcAddrStr -- string "interface.vpi.vci" i.e. 0.0.16
 *              output: rStatus -1 for error; 0 success.
******************************************************************************************/
int autoconfig_init(char *pvcAddrStr)
{
  int rStatus=0;
  netsnmp_session session;
  //  netsnmp_transport *autoconfigTransport;
  //  struct session_list *autoconfigSessionList;

  netsnmp_transport *transport;

  /* allocate resources: these should be done inside state machines states */
  if ((pAutoconfigAttachPointInfo = malloc(sizeof(AUTOCONFIG_ATTACHPOINT_INFO))) == NULL) {
    DEBUGMSGTL(("atmforum/autconfig_init","unable to allocate AUTOCONFIG_ATTACHPOINT_INFO\n"));
    rStatus = -1;
  }
  else
    memset((char*)pAutoconfigAttachPointInfo,0,sizeof(AUTOCONFIG_ATTACHPOINT_INFO));

  if ((pAutoconfigConfigInfo = malloc(sizeof(AUTOCONFIG_CONFIG_INFO))) == NULL) {
    DEBUGMSGTL(("atmforum/autconfig_init","unable to allocate AUTOCONFIG_CONFIG_INFO\n"));
    rStatus = -1;
  }
  else
    memset((char*)pAutoconfigConfigInfo,0,sizeof(AUTOCONFIG_CONFIG_INFO));

#ifdef TEST_DEBUG
  printf("autoconfig_init(%s)\n",pvcAddrStr);
#endif

  /* open session for ILMI messages reception and transmission */
  snmp_sess_init(&session);
  autoconfig_initSession(&session,pvcAddrStr);
  autoconfigSession = snmp_open(&session);

  if (!autoconfigSession) {
    DEBUGMSGTL(("atmforum/autoconfig_init","unable to open ILMI session\n"));
    rStatus = -1;
  }
  else 
    printf("snmp_session opened for %s\n",pvcAddrStr);

  /* also add a session for incoming port; by default only a udp port is being listened on */
  transport = netsnmp_tdomain_transport("udp:162", 1, "udp");
  snmp_sess_init(&session);
  autoconfig_initSession(&session,"udp:162");
  snmp_add(&session,transport,NULL,NULL);

#if 0
  /* these are not needed */
  /* net session list pointer to find out transport */
  autoconfigSessionList = snmp_sess_pointer(autoconfigSession);
  autoconfigTransport = snmp_sess_transport(autoconfigSessionList);

  if (autoconfigTransport != NULL)
    autoconfigSocket = autoconfigTransport->sock;
#endif



  printf("calling autoconfig_sendTrap\n");
  autoconfig_sendTrap(0,0,0,0,0);

  printf("calling autoconfig_sendAttachPointRequest()\n");
  autoconfig_sendAttachPointRequest();

#if 0
  /* DSLscope test */
  printf("calling autoconfig_sendConfigRequest()\n");
  autoconfig_sendConfigRequest();

  printf("calling autoconfig_sendServiceConnInfoRequest(0,0,41)\n");
  autoconfig_sendServiceConnInfoRequest(0,0,41);

  printf("calling autoconfig_sendVccTableRequest(0,0,41)\n");
  autoconfig_sendVccTableRequest(0,0,41);

  printf("calling autoconfig_sendServiceTypeRequest(1)\n");
  autoconfig_sendServiceTypeRequest(1);

  printf("calling autoconfig_sendAal5ProfileRequest(1)\n");
  autoconfig_sendAal5ProfileRequest(1);
  /* DSLscope test */
  time(&now);
  printf("time sent request = %s\n",ctime(&now));
#endif /* 0 just for DSLScope testing*/
  return rStatus;
}

void autoconfig_initSession(netsnmp_session *session, char *addrStr)
{
  session->peername= addrStr;
  session->version = SNMP_VERSION_1;
  session->community = (unsigned char *)"ILMI";
  session->community_len = strlen("ILMI");
  session->rcvMsgMaxSize = AUTOCONFIG_MAX_ILMI_MSG_LEN;
  session->sndMsgMaxSize = AUTOCONFIG_MAX_ILMI_MSG_LEN;
  session->timeout = AUTOCONFIG_MESSAGE_TIMEOUT;
  session->retries = 0;    /* we don't want SNMP agent to send retry for us */
  session->callback = (netsnmp_callback)autoconfig_input;
}

/*****************************************************************************************
 * Summary: autoconfig_input() is the callback function upon receiving messages on transport
 * Description: This function is called by SNMP daemon after it parses the PDU.
 * Parameters: 
 *              inputs:
 *                op -- indicate the operation of this call (timeout, receive messages...
 *                session -- the session this PDU is receive on
 *                reqid -- an integer request id of this PDU
 *                pdu -- the received pdu
 *                magic -- internal data
******************************************************************************************/
int autoconfig_input(int op,netsnmp_session * session,int reqid, netsnmp_pdu *pdu,void *magic)
{
#ifdef TEST_DEBUG
  printf("autoconfig_input(): op %d, session %x, reqid %x, pdu %x, magic %x\n",op,(int)session,reqid,
         (int)pdu,(int)magic);
#endif

  switch (op)
    {
    case NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE:
#ifdef TEST_DEBUG
      printf("autoconfig_input(): message received = %s\n",ctime(&now));
      printf("received pdu: reqid %x\n",reqid);
#endif
      switch (pdu->command)
        {
        case SNMP_MSG_TRAP:
#ifdef TEST_DEBUG
          printf("pdu->version %d, command %d, trap type %d, specific_type %d\n",
                 (int)pdu->version,(int)pdu->command,(int)pdu->trap_type,(int)pdu->specific_type);
#endif
          /* either cold start or Vcc change trap */
          DEBUGMSGTL(("atmforum/autoconfig_input","receiving TRAP pdu type %d\n",pdu->trap_type));
          if (pdu->trap_type == SNMP_TRAP_COLDSTART) {
          }
          else if (pdu->trap_type == SNMP_TRAP_ENTERPRISESPECIFIC) {
            /* atmfVccChange trap */
            if (pdu->specific_type == AUTOCONFIG_ATMF_VCC_CHANGE_TRAP) {
              autoconfig_receiveVccChangeTrap(pdu,&autoconfigVcChangeInfo);
            }
          }
          else
            DEBUGMSGTL(("atmforum/autoconfig_input","receiving trap type %d pdu: ignore\n",pdu->trap_type));
          break;
#ifdef TEST
        case SNMP_MSG_GET:
        case SNMP_MSG_GETNEXT:
          DEBUGMSGTL(("atmforum/autoconfig_input","receiving request pdu\n"));
          autoconfig_processRequestMessages(pdu,session,magic);
          break;
#endif
        case SNMP_MSG_RESPONSE:
          /* responses to one of our requests? */
          /* make sure reqid is equal to reqid of request whose response is expected */
          DEBUGMSGTL(("atmforum/autoconfig_input","receiving response pdu\n"));
          autoconfig_processResponseMessages(pdu,reqid);
          break;
        default:
          /* anything else, we do not care */
          DEBUGMSGTL(("atmforum/autoconfig_input","receiving pdu with command %x\n",pdu->command));
      }
      break;
    case NETSNMP_CALLBACK_OP_TIMED_OUT:
#ifdef TEST_DEBUG
      time(&now);
      printf("autoconfig_input(): time out request = %s\n",ctime(&now));
#endif
      break;
    case NETSNMP_CALLBACK_OP_SEND_FAILED:
      break;
    case NETSNMP_CALLBACK_OP_CONNECT:
      break;
    case NETSNMP_CALLBACK_OP_DISCONNECT:
      break;
    }
  /* caller expects 1 if ok, 0 for keep pending? */
  return 1;
}


/*****************************************************************************************
 * ACTIONS                                                                               *
 *****************************************************************************************/

/*****************************************************************************************
 * Summary: autoconfig_clearAttachPoint() 
 * Description: clear attachment point info
 * Parameters: 
 *              input: pAUTOCONFIG_ATTACHPOINT_INFO point
******************************************************************************************/
void autoconfig_clearAttachPoint(pAUTOCONFIG_ATTACHPOINT_INFO point)
{
  memset((void*)point,0, sizeof(AUTOCONFIG_ATTACHPOINT_INFO));
}

/*****************************************************************************************
 * Summary: autoconfig_setAttachPoint() 
 * Description: copy received attachment point over to local storage
 * Parameters: 
 *              input: pAUTOCONFIG_ATTACHPOINT_INFO point
******************************************************************************************/
void autoconfig_setAttachPoint(pAUTOCONFIG_ATTACHPOINT_INFO point)
{
  memcpy((void*)pAutoconfigAttachPointInfo,(void*)point,
         sizeof(AUTOCONFIG_ATTACHPOINT_INFO));
}

/*****************************************************************************************
 * Summary: autoconfig_compareAttachPoint() 
 * Description: Compares 2 attachment point to see if they are the same
 * Parameters: 
 *              input: pAUTOCONFIG_ATTACHPOINT_INFO point1, and point2
 *              output: rStatus 0 if equal, 1 if not
******************************************************************************************/
int autoconfig_compareAttachPoint(pAUTOCONFIG_ATTACHPOINT_INFO point1,
                                  pAUTOCONFIG_ATTACHPOINT_INFO point2)
{
  return (memcmp((void*)point1,(void*)point2, sizeof(AUTOCONFIG_ATTACHPOINT_INFO)));
}


static void autoconfig_freeListNode(void *node)
{
  netsnmp_data_list *n = (netsnmp_data_list *)node;

  if (!n && (n->data)) 
    free(n->data);
}



/*****************************************************************************************
 * Summary: autoconfig_addToRequestList
 * Description: this function is called whenever a request message is sent out successfully.
 *              The link list is the generic netsnmp_data_list; each entry is
 *              indexed by request id.
 * Parameters:  
 *              input: integer requestId
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
void autoconfig_addToRequestList (int requestId)
{
  netsnmp_data_list *node;
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];

  DEBUGMSGTL(("autoconfig_addToRequestList","requestId: %x\n",(int)requestId));

  sprintf(name, "%x", requestId);
  /* check for existing node with this index; if yes, do nothing and return; */
  node = netsnmp_get_list_node(pAutoconfigRequestList,name);
  if (node != NULL) {
    return;
  }
  node = netsnmp_create_data_list(name,NULL,NULL);
  if (node) {
    netsnmp_add_list_data(&pAutoconfigRequestList,node);
  }
}

/*****************************************************************************************
 * Summary: autoconfig_removeFromRequestList
 * Description: this function is called whenever a response message is received.
 *              The link list is the generic netsnmp_data_list; each entry is
 *              indexed by request id.
 * Parameters:  
 *              input: integer requestId
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
void autoconfig_removeFromRequestList (int requestId)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];

  DEBUGMSGTL(("autoconfig_removeFromRequestList","requestId: %x\n",(int)requestId));

  sprintf(name, "%x", requestId);
  if (netsnmp_remove_list_node(&pAutoconfigRequestList,name) != 0) {
    DEBUGMSGTL(("autoconfig_removeFromRequestList","no such request id %x found\n",(int)requestId));
  }
}

/*****************************************************************************************
 * Summary: autoconfig_sendTrap() is the function to call to send trap messages
 * Description: this function takes in trap specific info, compose the trap messages
 *              and the message out
 * Parameters: 
 *              input: trap is either enterprise or generic trap
 *                     specific is 0 is generic trap; otherwise, the specific enterprise
 *                     enterprise is the enterprise oid; if generic trap, it's 0
 *                     length of enterpise oid or 0
 *                     vars is variable list if any
 *              output: rStatus -1 for error; 0 success.
******************************************************************************************/
int autoconfig_sendTrap(int trap,
                        int specific,
                        oid * enterprise, int enterprise_length,
                        netsnmp_variable_list * vars)
{
    netsnmp_pdu    *pdu;
    in_addr_t      *pdu_in_addr_t;
    int             rStatus=AUTOCONFIG_SUCCESS;

    printf("autoconfig_sendTrap(): sending trap through send_enterprise_trap_vars()\n");
    send_enterprise_trap_vars(trap,
                              specific,
                              enterprise, enterprise_length,
                              vars);

    pdu = snmp_pdu_create(SNMP_MSG_TRAP);

#ifdef TEST_DEBUG
    printf("autoconfig_sendTrap %x\n",(int)pdu);
#endif
    pdu->version = SNMP_VERSION_1;
    pdu_in_addr_t = (in_addr_t *) pdu->agent_addr;
#ifdef SNMP_TRANSPORT_AAL5PVC_DOMAIN
    pdu_in_addr_t = 0;
#else
    *pdu_in_addr_t = get_myaddr();
#endif
    pdu->trap_type = trap;
    pdu->specific_type = specific;
    pdu->time = get_uptime();

    switch (trap) 
      {
      case SNMP_TRAP_COLDSTART:
        pdu->enterprise = (oid*)NULL;
        pdu->variables = (netsnmp_variable_list*)NULL;
        break;
      case SNMP_TRAP_AUTHFAIL:
        break;
      case SNMP_TRAP_ENTERPRISESPECIFIC:
        if (snmp_clone_mem((void **) &pdu->enterprise,
                           enterprise, enterprise_length*sizeof(oid))) {
          snmp_free_pdu(pdu);
          rStatus = AUTOCONFIG_ERROR;
        }
        pdu->variables = snmp_clone_varbind(vars);
        if (!pdu->variables) {
          snmp_free_pdu(pdu);
          rStatus = AUTOCONFIG_ERROR;
        }
      } /* switch trap */
    if (rStatus != AUTOCONFIG_ERROR) {
      rStatus = snmp_send(autoconfigSession, pdu);
      if (!rStatus) {
        DEBUGMSGTL(("atmforum/autoconfig_sendTrap","error sending trap pdu\n"));
        snmp_free_pdu(pdu);
      }
    } /* not autoconfig_error */
    return (rStatus);
}

int autoconfig_sendColdStart()
{
  return (autoconfig_sendTrap(0,0,0,0,0));
}

/*****************************************************************************************
 * Summary: autoconfig_sendConfigFailTrap
 * Description: this function composes a config fail trap and send it out.
 *              ConfigFailTrap composes of 2 variables: confFailReason and confFailOid.
 * Parameters:  
 *              input: int reason; see reasons in autoconfig.h
 *                     pointer to pdu->variables that causes the problem
 *                     flag to indicate if vars is a table or just an entry
 *              output: rStatus -1 for error; 0 on success
******************************************************************************************/
int autoconfig_sendConfigFailTrap(int reason, netsnmp_variable_list *vars)
{
  netsnmp_variable_list reasonVar, oidVar;
  oid atmForumEnterpriseOid[] = { AUTOCONFIG_ATMFORUM_ENTERPRISE_OID };
  int atmForumEnterpriseOidLen = OID_LENGTH(atmForumEnterpriseOid);
  oid atmForumFailReasonOid[] = { AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 12 };
  oid atmForumFailOidOid[] = { AUTOCONFIG_ATMF_CONFIG_GROUP_OID, 13 };

  /* composing the variable list to send */
  memset(&reasonVar,0,sizeof(netsnmp_variable_list));
  memset(&oidVar,0,sizeof(netsnmp_variable_list));
  reasonVar.type = ASN_INTEGER;
  snmp_set_var_objid(&reasonVar, atmForumFailReasonOid,OID_LENGTH(atmForumFailReasonOid));
  snmp_set_var_value(&reasonVar, (u_char *)&reason, sizeof(int));
  reasonVar.next_variable = &oidVar;
  snmp_set_var_objid(&oidVar, atmForumFailOidOid,OID_LENGTH(atmForumFailReasonOid));
  oidVar.type = ASN_OBJECT_ID;
  if (vars != NULL) {
    snmp_set_var_value(&oidVar,(u_char*) vars->name,(sizeof(oid)*vars->name_length));
    oidVar.next_variable = NULL;
  }
  return (autoconfig_sendTrap(SNMP_TRAP_ENTERPRISESPECIFIC,AUTOCONFIG_ATMF_CONFIG_FAIL_TRAP,
                              atmForumEnterpriseOid,atmForumEnterpriseOidLen,&reasonVar));
}

int autoconfig_response()
{
  return 0;
}

/*****************************************************************************************
 * Summary: autoconfig_sendAttachPointRequest
 * Description: this function sends request with bindings for objects sysUpTime,
 *              atmfMySystemIdentifier, atmfPortMyIfIndex
 * Parameters:  
 *              input: pointer to result pdu
 *              output: rStatus -1 for error; requestId on success
******************************************************************************************/
int autoconfig_sendAttachPointRequest()
{
  netsnmp_pdu    *pdu=NULL;
  int             rStatus=0;

  oid autoconfigMySysIdOid[] = { AUTOCONFIG_ATMF_MYSYSID_OID, 0 };
  /* assuming that port index is 0 which means the interface that received this request */
  /* if index needs to be found, attachpoint request cannot be done in 1 message */
  oid autoconfigMyIfIndexOid[] = { AUTOCONFIG_ATMF_PORT_MYID_OID, 0};
  oid autoconifgSysUpTimeOid[] = { AUTOCONFIG_SYSUPTIME_OID, 0 };

  pdu = snmp_pdu_create(SNMP_MSG_GET);
  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_sendAttachPointRequest","unable to alloc pdu\n"));
    rStatus = -1;
  }
  else {
    pdu->version = SNMP_VERSION_1;
    if (snmp_add_null_var(pdu,autoconifgSysUpTimeOid,OID_LENGTH(autoconifgSysUpTimeOid)) == NULL) 
      rStatus = -1;

    if (rStatus != -1) {
      if (snmp_add_null_var(pdu,autoconfigMySysIdOid,OID_LENGTH(autoconfigMySysIdOid)) == NULL)
        rStatus = -1;
    }
    if (rStatus != -1) {
      if (snmp_add_null_var(pdu,autoconfigMyIfIndexOid,OID_LENGTH(autoconfigMyIfIndexOid)) == NULL)
        rStatus = -1;
    }
  }

  if (rStatus != -1) {
    rStatus = snmp_send(autoconfigSession, pdu);
    if (rStatus == 0) {
      pdu->variables = NULL;
      snmp_free_pdu(pdu);
      DEBUGMSGTL(("atmforum/autoconfigsendAttachPointRequest","unable to send pdu, rStatus %d\n",rStatus));
      rStatus = -1;
    }
    else {
      autoconfig_addToRequestList(rStatus);
    }
  }
  return (rStatus);
}

/*****************************************************************************************
 * Summary: autoconfig_sendConfigRequest
 * Description: this function send request with bindings for objects in atmfLayer Group
 * Parameters:  
 *              input: pointer to result pdu
 *              output: rStatus -1 for error; request id on success.     
******************************************************************************************/
int autoconfig_sendConfigRequest()
{
  netsnmp_pdu    *pdu=NULL;
  int             rStatus=0;

#ifdef INDIVIDUAL_COLUMN
  oid autoconfigAtmLayerIndexOid[] = { AUTOCONFIG_ATMF_ATMLAYER_ENTRY_OID, 1, 0 };
#else
  oid autoconfigAtmLayerIndexOid[] = { AUTOCONFIG_ATMF_ATMLAYER_ENTRY_OID };
#endif
  int autoconfigAtmfLayerOidLength = OID_LENGTH(autoconfigAtmLayerIndexOid);
  int column;

#ifdef INDIVIDUAL_COLUMN
  pdu = snmp_pdu_create(SNMP_MSG_GET);
#else
  pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
#endif
  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_sendConfigRequest","unable to alloc pdu\n"));
    rStatus = -1;
  }
  else {
    pdu->version = SNMP_VERSION_1;
#ifdef INDIVIDUAL_COLUMN
    for (column = 1; column <= AUTOCONFIG_ATMF_ATMLAYER_ENTRY_SIZE; column++) {
      autoconfigAtmLayerIndexOid[autoconfigAtmfLayerOidLength-2] = (oid)column;
      if (snmp_add_null_var(pdu,autoconfigAtmLayerIndexOid,autoconfigAtmfLayerOidLength) == NULL) {
        rStatus = -1;
        break;
      }
    }
#else
    if (snmp_add_null_var(pdu,autoconfigAtmLayerIndexOid,autoconfigAtmfLayerOidLength) == NULL) {
      rStatus = -1;
    }
#endif
  }
  if (rStatus != -1) {
    rStatus = snmp_send(autoconfigSession, pdu);
    if (rStatus == 0) {
      pdu->variables = NULL;
      snmp_free_pdu(pdu);
      DEBUGMSGTL(("atmforum/autoconfig_sendConfigRequest","unable to send pdu, rStatus %d\n",rStatus));
      rStatus = -1;
    }
    else {
      autoconfig_addToRequestList(rStatus);
    }
  }
  return (rStatus);
}

/*****************************************************************************************
 * Summary: autoconfig_sendServiceConnInfoRequest
 * Description: this function sends request with bindings for objects in serviceConnInfo
 * Parameters:  
 *              input: pointer to result pdu
 *              output: rStatus -1 for error; request id on success.     
******************************************************************************************/
int autoconfig_sendServiceConnInfoRequest(int port, int vpi, int vci)
{
  netsnmp_pdu    *pdu=NULL;
  int             rStatus=0;
#ifdef INDIVIDUAL_COLUMN
  /* how do we know what the indexes are? */
  oid autoconfigServiceConnInfoOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_OID, 1, 0, 0, 41 };
  oid autoconfigServiceConnInfoEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_OID };
  int autoconfigServiceConnInfoEntryOidLength = OID_LENGTH(autoconfigServiceConnInfoEntryOid);
#ifdef ADD_LATER
  oid autoconfigServiceConnInfoExtOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_EXT_ENTRY_OID, 1 };
#endif
#else
  oid autoconfigServiceConnInfoOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_OID };
#ifdef ADD_LATER
  oid autoconfigServiceConnInfoExtOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_EXT_ENTRY_OID };
#endif
#endif
  int autoconfigServiceConnInfoOidLength = OID_LENGTH(autoconfigServiceConnInfoOid);

  int column;

#ifdef INDIVIDUAL_COLUMN
  pdu = snmp_pdu_create(SNMP_MSG_GET);
#else
  pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
#endif
  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_sendServiceConnInfoRequest","unable to alloc pdu\n"));
    rStatus = -1;
  }
  else {
    pdu->version = SNMP_VERSION_1;
#ifdef INDIVIDUAL_COLUMN
    for (column = 1; column <= AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_ENTRY_SIZE ; column++) {
      autoconfigServiceConnInfoOid[autoconfigServiceConnInfoEntryOidLength] = (oid)column;
      autoconfigServiceConnInfoOid[autoconfigServiceConnInfoEntryOidLength+1] = (oid)port;
      autoconfigServiceConnInfoOid[autoconfigServiceConnInfoEntryOidLength+2] = (oid)vpi;
      autoconfigServiceConnInfoOid[autoconfigServiceConnInfoEntryOidLength+3] = (oid)vci;
      if (snmp_add_null_var(pdu,autoconfigServiceConnInfoOid,autoconfigServiceConnInfoOidLength) == NULL) {
        rStatus = -1;
        break;
      }
    }
#else
    if (snmp_add_null_var(pdu,autoconfigServiceConnInfoOid,autoconfigServiceConnInfoOidLength) == NULL) {
      rStatus = -1;
    }
#endif
#ifdef ADD_LATER
    if (rStatus != -1)
      if (snmp_add_null_var(pdu,autoconfigServiceConnInfoExtOid,
                            OID_LENGTH(autoconfigServiceConnInfoExtOid)) == NULL) {
        rStatus = -1;
      }
#endif
  }
  if (rStatus != -1) {
    rStatus = snmp_send(autoconfigSession, pdu);
    if (rStatus == 0) {
      pdu->variables = NULL;
      snmp_free_pdu(pdu);
      DEBUGMSGTL(("atmforum/autoconfig_sendServiceConnInfoRequest","unable to send pdu, rStatus %d\n",rStatus));
      rStatus = -1;
    }
    else {
      autoconfig_addToRequestList(rStatus);
    }
  }
  return (rStatus);
}

/*****************************************************************************************
 * Summary: autoconfig_sendVccTableRequest
 * Description: this function sends request with bindings for objects in atmfVccTable;
 *              2 messages need to be sent; unused and absolete objects are not requested.
 *              The indexes are retrieved from connServiceTable; or if it's null, do a get
 *              next ?
 * Parameters:  
 *              output: rStatus -1 for error; request id on success.     
******************************************************************************************/
int autoconfig_sendVccTableRequest(int port, int vpi, int vci)
{
  netsnmp_pdu    *pdu=NULL, *pdu_next=NULL;
  int             rStatus=0;
  netsnmp_variable_list *ret;
  oid autoconfigAtmfVccRowOid[] = { AUTOCONFIG_ATMF_VCC_ENTRY_OID, 1, 0, 0, 41 };
  oid autoconfigAtmfVccEntryOid[] = { AUTOCONFIG_ATMF_VCC_ENTRY_OID };
  int autoconfigAtmfVccRowOidLength = OID_LENGTH(autoconfigAtmfVccRowOid);
  int autoconfigAtmfVccEntryOidLength = OID_LENGTH(autoconfigAtmfVccEntryOid);
  int column;

  pdu = snmp_pdu_create(SNMP_MSG_GET);
  pdu_next = snmp_pdu_create(SNMP_MSG_GET);
  if ((pdu == NULL) || (pdu_next == NULL)) {
    DEBUGMSGTL(("atmforum/autoconfig_sendVccTableRequest","unable to alloc pdu\n"));
    rStatus = -1;
  }
  else {
    pdu->version = SNMP_VERSION_1;
    for (column = 1; column <= AUTOCONFIG_ATMF_VCC_ENTRY_ENTRY_SIZE; column++) {
      /* we are skipping the obsolete or deprecated objects */
      if ((column == AUTOCONFIG_ATMF_VCC_QOS_CATEGORY) ||
          (column == AUTOCONFIG_ATMF_VCC_TX_QOS_CLASS) ||
          (column == AUTOCONFIG_ATMF_VCC_RX_QOS_CLASS) ||
          (column == AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_DESCRIPTOR_TYPE) ||
          (column == AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_DESCRIPTOR_TYPE) ||
          (column == AUTOCONFIG_ATMF_VCC_BEST_EFFORT_INDICATOR) ||
          (column == AUTOCONFIG_ATMF_VCC_SERVICE_CATEGORY))
        continue;
      autoconfigAtmfVccRowOid[autoconfigAtmfVccEntryOidLength] = (oid)column;
      autoconfigAtmfVccRowOid[autoconfigAtmfVccEntryOidLength+1] = (oid)port;
      autoconfigAtmfVccRowOid[autoconfigAtmfVccEntryOidLength+2] = (oid)vpi;
      autoconfigAtmfVccRowOid[autoconfigAtmfVccEntryOidLength+3] = (oid)vci;
      if (column < COLUMN_ATMFVPCRECEIVETRAFFICDESCRIPTORTYPE)
        ret = snmp_add_null_var(pdu,autoconfigAtmfVccRowOid,autoconfigAtmfVccRowOidLength);
      else
        ret = snmp_add_null_var(pdu_next,autoconfigAtmfVccRowOid,autoconfigAtmfVccRowOidLength);
      if (ret == NULL) {
        rStatus = -1;
        break;
      }
    }
  }
  if (rStatus != -1) {
    rStatus = snmp_send(autoconfigSession, pdu);
    if (rStatus == 0) {
      pdu->variables = NULL;
      snmp_free_pdu(pdu);
      DEBUGMSGTL(("atmforum/autoconfig_sendVccTableRequest","unable to send pdu, rStatus %d\n",rStatus));
      rStatus = -1;
    }
    else {
      autoconfig_addToRequestList(rStatus);

      rStatus = snmp_send(autoconfigSession, pdu_next);
      if (rStatus == 0) {
        pdu_next->variables = NULL;
        snmp_free_pdu(pdu_next);
        DEBUGMSGTL(("atmforum/autoconfig_sendVccTableRequest","unable to send pdu_next, rStatus %d\n",rStatus));
        rStatus = -1;
      }
      else {
        autoconfig_addToRequestList(rStatus);
      }
    }
  }
  return (rStatus);
}

/*****************************************************************************************
 * Summary: autoconfig_sendServiceTypeRequest
 * Description: this function sends request with bindings for objects in serviceTypeTable
 * Parameters:  
 *              input: pointer to result pdu
 *              output: rStatus -1 for error; request id on success.     
******************************************************************************************/
int autoconfig_sendServiceTypeRequest(int index)
{
  netsnmp_pdu    *pdu=NULL;
  int             rStatus=0;
  oid autoconfigServiceTypeEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_OID };
  oid autoconfigServiceTypeRowOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_OID, 1, 0 };
  oid autoconfigServiceTypeExtEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_OID };
  oid autoconfigServiceTypeExtRowOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_OID, 1, 0 };
  int autoconfigServiceTypeEntryOidLength = OID_LENGTH(autoconfigServiceTypeEntryOid);
  int autoconfigServiceTypeRowOidLength = OID_LENGTH(autoconfigServiceTypeRowOid);
  int autoconfigServiceTypeExtEntryOidLength = OID_LENGTH(autoconfigServiceTypeExtEntryOid);
  int autoconfigServiceTypeExtRowOidLength = OID_LENGTH(autoconfigServiceTypeExtRowOid);
  int column;

  pdu = snmp_pdu_create(SNMP_MSG_GET);
  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_sendServiceTypeRequest","unable to alloc pdu\n"));
    rStatus = -1;
  }
  else {
    pdu->version = SNMP_VERSION_1;
    for (column = 1; column <= AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_SIZE; column++) {
      autoconfigServiceTypeRowOid[autoconfigServiceTypeEntryOidLength] = (oid)column;
      autoconfigServiceTypeRowOid[autoconfigServiceTypeEntryOidLength+1] = (oid)index;
      if (snmp_add_null_var(pdu,autoconfigServiceTypeRowOid,autoconfigServiceTypeRowOidLength) == NULL) {
        rStatus = -1;
        break;
      }
    } /* for whole table */
    for (column = 1; column <= AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_SIZE; column++) {
      autoconfigServiceTypeExtRowOid[autoconfigServiceTypeExtEntryOidLength] = (oid)column;
      autoconfigServiceTypeExtRowOid[autoconfigServiceTypeExtEntryOidLength+1] = (oid)index;
      if (snmp_add_null_var(pdu,autoconfigServiceTypeExtRowOid,autoconfigServiceTypeExtRowOidLength) == NULL) {
        rStatus = -1;
        break;
      }
    } /* for ext table */
  }
  if (rStatus != -1) {
    rStatus = snmp_send(autoconfigSession, pdu);
    if (rStatus == 0) {
      pdu->variables = NULL;
      snmp_free_pdu(pdu);
      DEBUGMSGTL(("atmforum/autoconfig_sendServiceTypeRequest","unable to send pdu, rStatus %d\n",rStatus));
      rStatus = -1;
    }
    else {
      autoconfig_addToRequestList(rStatus);
    }
  }
  return (rStatus);
}

/*****************************************************************************************
 * Summary: autoconfig_sendAal5ProfileRequest
 * Description: this function sends request with bindings for objects in aal5ProfileTable
 * Parameters:  
 *              input: pointer to result pdu
 *              output: rStatus -1 for error; request id on success.     
******************************************************************************************/
int autoconfig_sendAal5ProfileRequest(int index)
{
  netsnmp_pdu    *pdu=NULL;
  int             rStatus=0;
  oid autoconfigAal5ProfileEntryOid[] = { AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_OID };
  int autoconfigAal5ProfileEntryOidLength = OID_LENGTH(autoconfigAal5ProfileEntryOid);
  oid autoconfigAal5ProfileRowOid[] = { AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_OID, 1, 0 };
  int autoconfigAal5ProfileRowOidLength = OID_LENGTH(autoconfigAal5ProfileRowOid);
  int column;

  pdu = snmp_pdu_create(SNMP_MSG_GET);
  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_sendAal5ProfileRequest","unable to alloc pdu\n"));
    rStatus = -1;
  }
  else {
    pdu->version = SNMP_VERSION_1;
    for (column = 1; column <= AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_SIZE; column++) {
      autoconfigAal5ProfileRowOid[autoconfigAal5ProfileEntryOidLength] = (oid)column;
      autoconfigAal5ProfileRowOid[autoconfigAal5ProfileEntryOidLength+1] = (oid)index;
      if (snmp_add_null_var(pdu,autoconfigAal5ProfileRowOid,autoconfigAal5ProfileRowOidLength) == NULL) {
        rStatus = -1;
        break;
      }
    }
    /* we are not supporting AAL2 Trunking; therefore, there is no need to get extension of AAL5Profile */
  }
  if (rStatus != -1) {
    rStatus = snmp_send(autoconfigSession, pdu);
    if (rStatus == 0) {
      pdu->variables = NULL;
      snmp_free_pdu(pdu);
      DEBUGMSGTL(("atmforum/autoconfig_sendAal5ProfileRequest","unable to send pdu, rStatus %d\n",rStatus));
      rStatus = -1;
    }
    else {
      autoconfig_addToRequestList(rStatus);
    }
  }
  return (rStatus);
}

/*****************************************************************************************
 * Summary: autoconfig_setConfigurationInfo() 
 * Description: copy received configuration info over to local storage
 * Parameters: 
 *              input: pointer to received AUTOCONFIG_CONFIG_INFO 
******************************************************************************************/
void autoconfig_setConfigurationInfo(pAUTOCONFIG_CONFIG_INFO info)
{
  memcpy((void*)pAutoconfigConfigInfo,(void*)info,
         sizeof(AUTOCONFIG_CONFIG_INFO));
}

/*****************************************************************************************
 * Summary: autoconfig_setServiceConnInfo
 * Description: this function is called to add a serviceConnInfoEntry to the
 *              serviceConnList.   This function should be called right after a
 *              serviceConnInfoResponse is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing of port.vpi.vci for serviceConnList.
 *              input: pointer to received service connection info structure
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
void autoconfig_setServiceConnInfo(pAUTOCONFIG_SERVICE_CONN_INFO entry)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_SERVICE_CONN_INFO data;

  DEBUGMSGTL(("autoconfig_setServiceConnInfo","entry: %x\n",(int)entry));
  /* first init the rest of the fields in entry */
  if (entry == NULL)
    return;

  sprintf(name, "%d.%d.%d", entry->atmfServicePortIndex,entry->atmfServiceVpi,entry->atmfServiceVci);
  
  /* check for existing node with this index; if yes, remove and then add new node */
  node = netsnmp_get_list_node(pAutoconfigServiceConnList,name);
  if (node != NULL) {
    netsnmp_remove_list_node(&pAutoconfigServiceConnList,name);
  }
  data = malloc(sizeof(AUTOCONFIG_SERVICE_CONN_INFO));
  if (data)
    memcpy(data,entry,sizeof(AUTOCONFIG_SERVICE_CONN_INFO));
  else
    return;

  node = netsnmp_create_data_list(name,(void*)data,autoconfig_freeListNode);
  if (node != NULL) {
    netsnmp_add_list_data(&pAutoconfigServiceConnList,node);
  }
}

/*****************************************************************************************
 * Summary: autoconfig_setAALProfileInfo
 * Description: this function is called to add a profileEntry to the
 *              aalProfileList.   This function should be called right after a
 *              aalProfileResponse is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing index for aalProfileList.
 * Parameters:  
 *              input: pointer to received pAUTOCONFIG_AAL_PROFILE_INFO
******************************************************************************************/
void autoconfig_setAALProfileInfo(pAUTOCONFIG_AAL_PROFILE_INFO entry)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_AAL_PROFILE_INFO data;

  DEBUGMSGTL(("autoconfig_setAALProfile","entry: %x\n",(int)entry));
  /* first init the rest of the fields in entry */
  if (entry == NULL)
    return;

  if (entry->type == ATMF_AAL5_PROFILE) 
    sprintf(name, "%d", entry->aal5.atmfAAL5ProfileIndex);

  /* multiple connections could be using the same AALProfile; there could be multiple
     add of the same entry.
     1. if entry is not in the list, add it.
     2. if entry of this index is in the list, compare it with existing entry.
        If they are the same, increase reference count and return.
        If they are different, keep the reference count and overwrite old entry with new data */
  entry->referenceCount = 1;

  /* check for existing node with this index; if yes, remove and then add new node */
  node = netsnmp_get_list_node(pAutoconfigAALProfileList,name);
  if (node != NULL) {
    data = (pAUTOCONFIG_AAL_PROFILE_INFO)node->data;
    /* not comparing reference count */
    if (memcmp(data,entry,(sizeof(AUTOCONFIG_AAL_PROFILE_INFO)-4)) == 0) {
      data->referenceCount++;
      return;
    }
    else {
      entry->referenceCount = data->referenceCount;
      netsnmp_remove_list_node(&pAutoconfigAALProfileList,name);
    }
  }
  data = malloc(sizeof(AUTOCONFIG_AAL_PROFILE_INFO));
  if (data)
    memcpy(data,entry,sizeof(AUTOCONFIG_AAL_PROFILE_INFO));
  else
    return;
  node = netsnmp_create_data_list(name,(void*)data,autoconfig_freeListNode);
  if (node) {
    netsnmp_add_list_data(&pAutoconfigAALProfileList,node);
  }
}

/*****************************************************************************************
 * Summary: autoconfig_setServiceTypeInfo
 * Description: this function is called to add a service type info to the
 *              serviceTypeList.   This function should be called right after a
 *              serviceTypeResponse is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing index for serviceTypeEntry
 * Parameters:  
 *              input: pointer to received pAUTOCONFIG_SERVICE_TYPE_INFO
 ******************************************************************************************/
void autoconfig_setServiceTypeInfo(pAUTOCONFIG_SERVICE_TYPE_INFO entry)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_SERVICE_TYPE_INFO data;

  DEBUGMSGTL(("autoconfig_setServiceTypeInfo","entry: %x\n",(int)entry));
  /* first init the rest of the fields in entry */
  if (entry == NULL)
    return;

  /* multiple connections could be using the same serviceType; there could be multiple
     add of the same entry.
     1. if entry is not in the list, add it.
     2. if entry of this index is in the list, compare it with existing entry.
        If they are the same, increase reference count and return.
        If they are different, keep the reference count and overwrite old entry with new data */
  entry->referenceCount = 1;
  sprintf(name, "%d", entry->atmfServiceTypeIndex);

  /* check for existing node with this index; if yes, remove and then add new node */
  node = netsnmp_get_list_node(pAutoconfigServiceTypeList,name);
  if (node != NULL) {
    data = (pAUTOCONFIG_SERVICE_TYPE_INFO)node->data;
    /* not comparing reference count */
    if (memcmp(data,entry,(sizeof(AUTOCONFIG_SERVICE_TYPE_INFO)-4)) == 0) {
      data->referenceCount++;
      return;
    }
    else {
      entry->referenceCount = data->referenceCount;
      netsnmp_remove_list_node(&pAutoconfigServiceTypeList,name);
    }
  }
  data = malloc(sizeof(AUTOCONFIG_SERVICE_TYPE_INFO));
  if (data)
    memcpy(data,entry,sizeof(AUTOCONFIG_SERVICE_TYPE_INFO));
  else
    return;
  node = netsnmp_create_data_list(name,(void*)data,autoconfig_freeListNode);
  if (node) {
    netsnmp_add_list_data(&pAutoconfigServiceTypeList,node);
  }
}

/*****************************************************************************************
 * Summary: autoconfig_setVccInfo
 * Description: this function is called to add a VccEntry to the
 *              vccList.   This function should be called right after a
 *              vccTableResponse is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing port.vpi.vci.
 * Parameters:  
 *              input: pointer to received pAUTOCONFIG_VCC_INFO
******************************************************************************************/
void autoconfig_setVccInfo(pAUTOCONFIG_VCC_INFO entry)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_VCC_INFO data;

  DEBUGMSGTL(("autoconfig_setVccInfo","entry: %x\n",(int)entry));
  /* first init the rest of the fields in entry */
  if (entry == NULL)
    return;

  sprintf(name, "%d.%d.%d", entry->atmfVccPortIndex,entry->atmfVccVpi,entry->atmfVccVci);

  /* check for existing node with this index; if yes, remove and then add new node */
  node = netsnmp_get_list_node(pAutoconfigVccList,name);
  if (node != NULL) {
    netsnmp_remove_list_node(&pAutoconfigVccList,name);
  }
  data = malloc(sizeof(AUTOCONFIG_VCC_INFO));
  if (data)
    memcpy(data,entry,sizeof(AUTOCONFIG_VCC_INFO));
  else
    return;
  node = netsnmp_create_data_list(name,(void*)data,autoconfig_freeListNode);
  if (node) {
    netsnmp_add_list_data(&pAutoconfigVccList,node);
  }
}

/*****************************************************************************************
 * Summary: autoconfig_clearServiceConnInfo
 * Description: this function is called to delete a service connection info from the
 *              list.   This essentially also delete references to serviceType and aalProfile
 *              entries as well.   This function should be called right after a
 *              vccChangeTrap is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing "port.vpi.vci"
 * Parameters:  
 *              input: pointer to received AUTOCONFIG_VC_INFO
 ******************************************************************************************/
void autoconfig_clearServiceConnInfo(pAUTOCONFIG_VC_INFO entry)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_SERVICE_CONN_INFO data;

  DEBUGMSGTL(("autoconfig_clearServiceConnInfo","entry: %x\n",(int)entry));
  /* first init the rest of the fields in entry */
  if (entry == NULL)
    return;

  sprintf(name, "%d.%d.%d", entry->port,entry->vpi,entry->vci);
  node = netsnmp_get_list_node(pAutoconfigServiceConnList,name);
  if (node != NULL) {
    data = (pAUTOCONFIG_SERVICE_CONN_INFO)node->data;
    if (data->atmfServiceConnServiceIndex)
      autoconfig_clearServiceTypeInfo(data->atmfServiceConnServiceIndex);
    if (data->atmfServiceConnAALIndex)
      autoconfig_clearAALProfileInfo(data->atmfServiceConnServiceIndex);
    netsnmp_remove_list_node(&pAutoconfigServiceConnList,name);
  }
  else
    DEBUGMSGTL(("autoconfig_clearServiceConnInfo","no such connInfo name %s found\n",name));
}

/*****************************************************************************************
 * Summary: autoconfig_clearServiceTypeInfo
 * Description: this function is called to delete a service type info from the
 *              list.   This function should be called right after a
 *              vccChangeTrap is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing "index"
 * Parameters:  
 *              input: index
 ******************************************************************************************/
void autoconfig_clearServiceTypeInfo(int index)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_SERVICE_TYPE_INFO data;

  DEBUGMSGTL(("autoconfig_clearServiceTypeInfo","index: %x\n",(int)index));

  /* multiple connections could be using the same serviceType; one needs to be careful
     not removing entry when it's still being used 
     1. if entry's reference count is 1, safe to remove.
     2. if entry, decrease reference count only, return */

  sprintf(name, "%d", index);

  node = netsnmp_get_list_node(pAutoconfigServiceTypeList,name);
  if (node != NULL) {
    data = (pAUTOCONFIG_SERVICE_TYPE_INFO)node->data;
    if (data->referenceCount > 1) {
      data->referenceCount--;
    }
    else {
      netsnmp_remove_list_node(&pAutoconfigServiceTypeList,name);
    }
  }
}

/*****************************************************************************************
 * Summary: autoconfig_clearAALProfileInfo
 * Description: this function is called to delete a aal profile type info from the
 *              list.   This function should be called right after a
 *              vccChangeTrap is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing "index"
 * Parameters:  
 *              input: index
 ******************************************************************************************/
void autoconfig_clearAALProfileInfo(int index)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;
  pAUTOCONFIG_AAL_PROFILE_INFO data;

  DEBUGMSGTL(("autoconfig_clearAALProfile","index: %d\n",index));

  sprintf(name, "%d", index);

  /* multiple connections could be using the same AalTypeEntry; one needs to be careful
     not removing entry when it's still being used 
     1. if entry's reference count is 1, safe to remove.
     2. if entry, decrease reference count only, return */

  node = netsnmp_get_list_node(pAutoconfigAALProfileList,name);
  if (node != NULL) {
    data = (pAUTOCONFIG_AAL_PROFILE_INFO)node->data;
    if (data->referenceCount > 1) {
      data->referenceCount--;
    }
    else {
      netsnmp_remove_list_node(&pAutoconfigAALProfileList,name);
    }
  }
}

/*****************************************************************************************
 * Summary: autoconfig_clearVccInfo
 * Description: this function is called to delete a Vcc info from the
 *              list.   This function should be called right after a
 *              vccChangeTrap is received from remote end.
 *              The link list is the generic netsnmp_data_list where name is the index.
 *              Name is a string composing "port.vpi.vci"
 * Parameters:  
 *              input: index
 ******************************************************************************************/
void autoconfig_clearVccInfo(int port, int vpi, int vci)
{
  char name[AUTOCONIFG_DISPLAY_STRING_MAX];
  netsnmp_data_list *node;

  DEBUGMSGTL(("autoconfig_clearVccInfo","port/vpi/vci: %d/%d/%d\n",port,vpi,vci));
  /* first init the rest of the fields in entry */
  sprintf(name, "%d.%d.%d", port,vpi,vci);

  /* check for existing node with this index; if yes, remove and then add new node */
  node = netsnmp_get_list_node(pAutoconfigVccList,name);
  if (node != NULL) {
    netsnmp_remove_list_node(&pAutoconfigVccList,name);
  }
}

/*****************************************************************************************
 * Summary: autoconfig_clearConfigurationInfo() 
 * Description: clear local storage of received configuration
 * Parameters: 
 *              input: pointer to received AUTOCONFIG_CONFIG_INFO 
******************************************************************************************/
void autoconfig_clearConfigurationInfo(pAUTOCONFIG_CONFIG_INFO info)
{
  memset(info,0,sizeof(AUTOCONFIG_CONFIG_INFO));
}

/*****************************************************************************************
 * Summary: autoconfig_clearAllInfo() 
 * Description: clear all local storage of received configuration
 * Parameters: 
******************************************************************************************/
void autoconfig_clearAllInfo()
{
  /* clear all allocated by init routines */
  netsnmp_free_all_list_data(pAutoconfigVccList);
  netsnmp_free_all_list_data(pAutoconfigServiceTypeList);
  netsnmp_free_all_list_data(pAutoconfigAALProfileList);
  netsnmp_free_all_list_data(pAutoconfigRequestList);
  netsnmp_free_all_list_data(pAutoconfigServiceConnList);
}

/*****************************************************************************************
 * EVENTS                                                                                *
 *****************************************************************************************/


/*****************************************************************************************
 * Summary: autoconfig_receiveVccChangeTrap
 * Description: this function parses the incoming VCC Change Trap message from remote peer.
 *              The port, vpi, vci and operStatus are retrieved and returned.
 * Parameters:  
 *              input: pointer to received pdu, pointer to AUTOCONFIG_VC_INFO
 *              output: rStatus -1 for error; 0 on success.     
******************************************************************************************/
int autoconfig_receiveVccChangeTrap(netsnmp_pdu *pdu, pAUTOCONFIG_VC_INFO vcInfo)
{
  netsnmp_variable_list *vars;
  oid portOid[] = {AUTOCONFIG_ATMF_PORT_INDEX_OID};
  oid vpiOid[] = {AUTOCONFIG_ATMF_VCC_VPI_OID};
  oid vciOid[] = {AUTOCONFIG_ATMF_VCC_VCI_OID};
  oid operStatusOid[] = {AUTOCONFIG_ATMF_VCC_OPER_STATUS_OID};

  DEBUGMSGTL(("atmforum/autoconfigProcessVccChangeTrap","entry\n"));

  if (vcInfo == NULL)
    return -1;
  memset(vcInfo, 0, sizeof(AUTOCONFIG_VC_INFO));

  /* looking for atmfVccPortIndex, atmfVccVci, atmfVccVpi, and atmfVccOperStatus */
  /* 1. if VCC is found in table, then this VCC has been changed or deleted; remove it,
     restart the auto config process for this VCC again. */
  /* 2. if VCC is not found, this is a new VCC */
  for (vars = pdu->variables; vars; vars = vars->next_variable) {
    /* Looking for AUTOCONFIG_ATMF_PORT_INDEX_OID, AUTOCONFIG_ATMF_VCC_VPI_OID, 
       AUTOCONFIG_ATMF_VCC_VCI_OID, AUTOCONFIG_ATMF_VCC_OPER_STATUS_OID */
    if (snmp_oid_compare(portOid,AUTOCONFIG_ATMF_PORT_INDEX_OID_LEN,
                         vars->name, vars->name_length) == 0) {
      vcInfo->port = *(vars->val.integer);
      DEBUGMSGTL(("atmforum/autoconfigProcessVccChangeTrap","portIndex %d\n",vcInfo->port));

#ifdef TEST_DEBUG
      printf("autoconfigProcessVccChangeTrap(): portIndex %d\n",vcInfo->port);
#endif
    }
    else if (snmp_oid_compare(vpiOid,AUTOCONFIG_ATMF_VCC_VPI_OID_LEN,
                              vars->name,vars->name_length) == 0) {
      vcInfo->vpi = *(vars->val.integer);
      DEBUGMSGTL(("atmforum/autoconfigProcessVccChangeTrap","vpi %d\n",vcInfo->vpi));
#ifdef TEST_DEBUG
      printf("autoconfigProcessVccChangeTrap(): vpi %d\n",vcInfo->vpi);
#endif
    }
    else if (snmp_oid_compare(vciOid,AUTOCONFIG_ATMF_VCC_VCI_OID_LEN,
                              vars->name,vars->name_length) == 0) {
      vcInfo->vci = *(vars->val.integer);
      DEBUGMSGTL(("atmforum/autoconfigProcessVccChangeTrap","vci %d\n",vcInfo->vci));
#ifdef TEST_DEBUG
      printf("autoconfigProcessVccChangeTrap(): vci %d\n",vcInfo->vci);
#endif
    }
    else if (snmp_oid_compare(operStatusOid,AUTOCONFIG_ATMF_VCC_OPER_STATUS_OID_LEN,
                              vars->name,vars->name_length) == 0) {
      vcInfo->operStatus = *(vars->val.integer);
#ifdef TEST_DEBUG
      printf("autoconfigProcessVccChangeTrap(): operStatus %d\n",vcInfo->operStatus);
#endif
    }
#ifdef TEST_DEBUG
    else
      printf("autoconfigProcessVccChangeTrap(): unrecognizable OID parameter\n");
#endif
  } /* for vars */
  return 1;
}

#ifdef TEST
int autoconfig_processRequestMessages(netsnmp_pdu *pdu,netsnmp_session * session,void *magic)
{
  int rStatus, status;
  netsnmp_agent_session *asp;

  if (magic == NULL) {
    asp = init_agent_snmp_session(session, pdu);
    status = SNMP_ERR_NOERROR;
  } else {
    asp = (netsnmp_agent_session *) magic;
    status = asp->status;
  }
  rStatus = netsnmp_handle_request(asp, status);

  /*
   * done
   */
  DEBUGMSGTL(("autoconfigProcessRequestMessages", "end asp = %08p, rStatus = %d, status %d\n",
              asp,rStatus,status));
  
  return rStatus;
}
#endif /* TEST */

int autoconfig_processResponseMessages(netsnmp_pdu *pdu, int reqid)
{
  int rStatus;
#ifdef TEST
  AUTOCONFIG_SERVICE_CONN_INFO *data;
  netsnmp_data_list *ptr;
#endif

  DEBUGMSGTL(("atmforum/autoconfigProcessResponseMessage","entry\n"));

  if (pdu->errstat == SNMP_ERR_NOERROR) {
    rStatus = autoconfig_receiveAttachPointResponse(pdu);
#ifdef TEST_DEBUG
    printf("called receiveAttachPointResponse, rStatus %x\n",rStatus);
#endif
    if (rStatus > 0) {
      autoconfig_displayAttachPointParams();
      if (rStatus == AUTOCONFIG_ATTACHPOINT_INFO_ALL_SET) {
        autoconfig_removeFromRequestList(reqid);

#ifdef TEST
#ifdef TEST_DEBUG
        printf("\n****************************************************************\n");
        printf("calling autoconfig_sendConfigRequest\n");
        printf("\n****************************************************************\n");
#endif
        autoconfig_sendConfigRequest();
        state = AUTOCONFIG_TEST_EXPECT_CONFIG;
#endif /*TEST */
      } /* all set for attachpoint */
      return rStatus;
    } /* rStatus > 0 */
    rStatus = autoconfig_receiveConfigResponse(pdu);

#ifdef TEST_DEBUG
    printf("called autoconfig_receiveConfigResponse, rStatus %x\n",rStatus);
#endif

    if (rStatus > 0) {
      autoconfig_displayConfigParams();
#ifdef TEST
      if ((rStatus == AUTOCONFIG_CONFIG_INFO_ALL_SET) && 
          (state == AUTOCONFIG_TEST_EXPECT_CONFIG)) {
#else
      if (rStatus == AUTOCONFIG_CONFIG_INFO_ALL_SET) {
#endif
        autoconfig_removeFromRequestList(reqid);

#ifdef TEST
#ifdef TEST_DEBUG
        printf("\n****************************************************************\n");
        printf("calling autoconfig_sendServiceConnInfoRequest(port 0, vpi 0, vci 40\n");
        printf("\n****************************************************************\n");
#endif
        autoconfig_sendServiceConnInfoRequest(0,0,41);
        state = AUTOCONFIG_TEST_EXPECT_SERVICE_CONN1;
#endif /* TEST */
      } /* all set */
      return rStatus;
    } /* rStatus > 0 */
    rStatus = autoconfig_receiveServiceConnInfoResponse(pdu);

#ifdef TEST_DEBUG
    printf("called autoconfig_receiveServiceConnResponse, rStatus %x\n",rStatus);
#endif

    if (rStatus > 0) {
      autoconfig_displayServiceConnList();

#ifdef TEST
      if ((rStatus == AUTOCONFIG_SERVICE_CONNINFO_ALL_SET) &&
          (state == AUTOCONFIG_TEST_EXPECT_SERVICE_CONN1)) {
#else
      if (rStatus == AUTOCONFIG_SERVICE_CONNINFO_ALL_SET) {
#endif /*TEST */
        autoconfig_removeFromRequestList(reqid);  /* first conn request */

#ifdef TEST
#ifdef TEST_DEBUG
        printf("\n****************************************************************\n");
        printf("calling autoconfig_sendServiceConnInfoRequest(port 0, vpi 2, vci 35\n");
        printf("\n****************************************************************\n");
#endif
        autoconfig_sendServiceConnInfoRequest(0,2,35);
        rStatus = 1;
        state = AUTOCONFIG_TEST_EXPECT_SERVICE_CONN2;
#endif /* TEST */
      } /* all set */

#ifdef TEST
      if ((rStatus == AUTOCONFIG_SERVICE_CONNINFO_ALL_SET) &&
          (state == AUTOCONFIG_TEST_EXPECT_SERVICE_CONN2)) {
        autoconfig_removeFromRequestList(reqid); /* second conn request */
        for (ptr= pAutoconfigServiceConnList; ptr; ptr=ptr->next) {
          data = (pAUTOCONFIG_SERVICE_CONN_INFO)ptr->data;

#ifdef TEST_DEBUG
          printf("\n****************************************************************\n");
          printf("calling autoconfig_sendAal5ProfileRequest(index %d,data %x)\n",
                 data->atmfServiceConnAALIndex,(int)data);
#endif
          if (data != NULL) {
#ifdef TEST_DEBUG
            printf("data->atmfServiceConnAALIndex %d\n",data->atmfServiceConnAALIndex);
            printf("\n****************************************************************\n");
#endif
            autoconfig_sendAal5ProfileRequest(data->atmfServiceConnAALIndex);
          }
          else {
            printf("index hardcoded to 1\n");
            autoconfig_sendAal5ProfileRequest(1);
          }
        } /* for */
        state = AUTOCONFIG_TEST_EXPECT_PROFILE;
      } /* conn service info all set */
#endif /*TEST */
      return rStatus;
    } /* rStatus > 0 */

    rStatus = autoconfig_receiveAal5ProfileResponse(pdu);

#ifdef TEST_DEBUG
    printf("called autoconfig_receiveAal5ProfileResponse, rStatus %x\n",rStatus);
#endif

    if (rStatus > 0) {
      autoconfig_displayAALProfileList();
      if (rStatus == AUTOCONFIG_AAL5_PROFILE_ALL_SET) {
        autoconfig_removeFromRequestList(reqid);

#ifdef TEST
        if (pAutoconfigRequestList == NULL) {
          for (ptr= pAutoconfigServiceConnList; ptr; ptr=ptr->next) {
            data = (pAUTOCONFIG_SERVICE_CONN_INFO)ptr->data;
            if (data != NULL) {

#ifdef TEST_DEBUG
              printf("\n****************************************************************\n");
              printf("calling serviceTypeRequest(index %d)\n",data->atmfServiceConnServiceIndex);
              printf("\n****************************************************************\n");
#endif

              autoconfig_sendServiceTypeRequest(data->atmfServiceConnServiceIndex);
            }
            else {
              printf("index hardcoded to 1\n");
              autoconfig_sendServiceTypeRequest(1);
            }
          } /* for */
          state = AUTOCONFIG_TEST_EXPECT_SERVICE_TYPE;
        } /* no more pending request */
#endif /* TEST */
      } /* all set */
      return rStatus;
    } /* rStatus > 0 */
    rStatus = autoconfig_receiveServiceTypeResponse(pdu);

#ifdef TEST_DEBUG
    printf("called autoconfig_receiveServiceTypeResponse, rStatus %x\n",rStatus);
#endif

    if (rStatus > 0) {
      autoconfig_displayServiceTypeList();
      if (rStatus == AUTOCONFIG_SERVICE_TYPE_ALL_SET) {
        autoconfig_removeFromRequestList(reqid);

#ifdef TEST
        if (pAutoconfigRequestList == NULL) {
          for (ptr= pAutoconfigServiceConnList; ptr; ptr=ptr->next) {
            data = (pAUTOCONFIG_SERVICE_CONN_INFO)ptr->data;
            if (data != NULL) {

#ifdef TEST_DEBUG
              printf("\n****************************************************************\n");
              printf("calling autoconfig_sendVccRequest(port %d, vpi/vci %d/%d)\n",
                     data->atmfServicePortIndex,data->atmfServiceVpi,
                     data->atmfServiceVci);
              printf("\n****************************************************************\n");
#endif

              autoconfig_sendVccTableRequest(data->atmfServicePortIndex,data->atmfServiceVpi,data->atmfServiceVci);
            }
            else {
              printf("index to vcc hardcoded to 0.0.41\n");
              autoconfig_sendVccTableRequest(0,0,41);
            }
          } /* for */
          state = AUTOCONFIG_TEST_EXPECT_VCC_TABLE;
        } /* no more pending request */
#endif /* TEST */
      } /* all set */
      return rStatus;
    } /* if rStatus > 0 */
    rStatus = autoconfig_receiveVccTableResponse(pdu,&vccVarFlag);
    if ((rStatus > 0) && (vccVarFlag == AUTOCONFIG_VCC_ALL_SET)) {
      autoconfig_displayVccList();
      autoconfig_removeFromRequestList(reqid);
      vccVarFlag = 0;
      return rStatus;
    }
    if (rStatus <= 0) {
#ifdef TEST_DEBUG
      printf("atmforum/autoconfigProcessResponseMessage(end): unrecognizable message\n");
#endif
      DEBUGMSGTL(("atmforum/autoconfigProcessResponseMessage","unrecognizable message\n"));
      rStatus = AUTOCONFIG_ERROR;
    }
  } /* no error */
  else {
    /* error in response */
#ifdef TEST_DEBUG
    printf("atmforum/autoconfigProcessResponseMessages: error in packet(%s)\n",
           snmp_errstring(pdu->errstat));
#endif

    DEBUGMSGTL(("atmforum/autoconfigProcessResponseMessages","error in packet.\n"));
    DEBUGMSGTL(("Reason:", "%s\n",snmp_errstring(pdu->errstat)));
    rStatus = AUTOCONFIG_ERROR;
  }
  return rStatus;
}


/*****************************************************************************************
 * EVENTS                                                                                *
 ****************************************************************************************/

/*****************************************************************************************
 * Summary: autoconfig_receiveAttachPointResponse
 * Description: this function parse attach point response and store received info
 * Parameters:  
 *              input: pointer to received pdu
 *              output: rStatus oidReceive count on Success or AUTOCONFIG_ERROR
 *                      oidReceive is a bit mask indicating the receipt of each requested
 *                      OID.   If the oidReceive flag is not all set, then very likely
 *                      there are more response messages to process.
******************************************************************************************/
int autoconfig_receiveAttachPointResponse(netsnmp_pdu *pdu)
{
  netsnmp_variable_list *vars;
  int             rStatus = AUTOCONFIG_SUCCESS;
  int autoconfigAttachPointOidReceive=0;
  oid autoconifgSysUpTimeOid[] = { AUTOCONFIG_SYSUPTIME_OID, 0 };
  int sysUptimeLen =   OID_LENGTH(autoconifgSysUpTimeOid);
  oid autoconfigMySysIdOid[] = { AUTOCONFIG_ATMF_MYSYSID_OID, 0 };
  int mySysIdLen = OID_LENGTH(autoconfigMySysIdOid);
  oid autoconfigMyIfIndexOid[] = { AUTOCONFIG_ATMF_PORT_MYID_OID, 0 };
  int myIfIndexOidLen = OID_LENGTH(autoconfigMyIfIndexOid);

  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_receiveAttachPointResponse","entry: pdu is NULL\n"));
    rStatus = AUTOCONFIG_ERROR;
  }
  else if (pdu->errstat != SNMP_ERR_NOERROR) {
    DEBUGMSGTL(("atmforum/autoconfig_receiveAttachPointResponse","entry: pdu error %d\n",pdu->errstat));
    rStatus = AUTOCONFIG_ERROR;
  }

  if (rStatus != AUTOCONFIG_ERROR) {
    for (vars = pdu->variables; vars; vars = vars->next_variable) {
      if (netsnmp_oid_equals(autoconfigMySysIdOid,mySysIdLen,vars->name,vars->name_length) == 0) {
        memcpy(pAutoconfigAttachPointInfo->atmfMySystemIdentifier,
               vars->val.string,AUTOCONFIG_SYS_ID_LEN);
        autoconfigAttachPointOidReceive |= AUTOCONFIG_ATTACHPOINT_INFO_MY_SYS_ID_SET;
        DEBUGMSGTL(("atmforum/autoconfig_receiveAttachPointResponse","mySysId %s\n",
                    pAutoconfigAttachPointInfo->atmfMySystemIdentifier));
      }
      else if (netsnmp_oid_equals(autoconfigMyIfIndexOid,myIfIndexOidLen,vars->name,vars->name_length) == 0) {
        pAutoconfigAttachPointInfo->atmfPortMyIfIdentifier = *(vars->val.integer);
        autoconfigAttachPointOidReceive |= AUTOCONFIG_ATTACHPOINT_INFO_MY_IF_ID_SET;
        DEBUGMSGTL(("atmforum/autoconfig_receiveAttachPointResponse","myPortId %d\n",
                    pAutoconfigAttachPointInfo->atmfPortMyIfIdentifier));
      }
      else if (netsnmp_oid_equals(autoconifgSysUpTimeOid,sysUptimeLen,vars->name,vars->name_length) == 0) {
        pAutoconfigAttachPointInfo->sysUptime = *(vars->val.integer);
        autoconfigAttachPointOidReceive |= AUTOCONFIG_ATTACHPOINT_INFO_SYS_UPTIME_SET;
        DEBUGMSGTL(("atmforum/autoconfig_receiveAttachPointResponse","SysUpTime %d\n",
                    pAutoconfigAttachPointInfo->sysUptime));
      }
      rStatus = autoconfigAttachPointOidReceive;
    }
  }
  return rStatus;
}

/*****************************************************************************************
 * Summary: autoconfig_receiveConfigResponse
 * Description: this function parse config response and store received info
 *              into struct AUTOCONFIG_CONFIG_INFO pAutoconfigConfigInfo.
 *              Config information can come in 2 SNMP messages.   All variables should
 *              to be received before the moving to the next state.
 * Parameters:  
 *              input: pointer to received pdu
 *              output: rStatus oidReceive count on Success or AUTOCONFIG_ERROR
 *                      oidReceive is a bit mask indicating the receipt of each requested
 *                      OID.   If the oidReceive count is not all set, then very likely
 *                      there are more response messages to process.
 ******************************************************************************************/
int autoconfig_receiveConfigResponse(netsnmp_pdu *pdu)
{
  netsnmp_variable_list *vars;
  int             rStatus=AUTOCONFIG_SUCCESS;
  int autoconfigConfigOidReceive=0;
  oid autoconfigAtmLayerOid[] = { AUTOCONFIG_ATMF_ATMLAYER_ENTRY_OID };
  int autoconfigAtmfLayerOidLength = OID_LENGTH(autoconfigAtmLayerOid);
  int lastOid;

  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/receiveConfigResponse","entry: pdu is NULL\n"));
    rStatus = -1;
  }
  else if (pdu->errstat != SNMP_ERR_NOERROR) {
    DEBUGMSGTL(("atmforum/receiveConfigResponse","entry: pdu error %d\n",pdu->errstat));
    rStatus = -1;
  }
  if (rStatus == AUTOCONFIG_SUCCESS) {
    for (vars = pdu->variables; vars; vars = vars->next_variable) {
      /* compare up to the last oid */
      if (netsnmp_oid_is_subtree(autoconfigAtmLayerOid,autoconfigAtmfLayerOidLength,
                                 vars->name,vars->name_length) == 0) {
          /* always has 0 add the end for get request of entry */
          lastOid = (int)vars->name[autoconfigAtmfLayerOidLength];
          switch (lastOid)
            {
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_INDEX:
              pAutoconfigConfigInfo->atmfAtmLayerIndex = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_INDEX_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerIndex %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerIndex));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VPCS:
              pAutoconfigConfigInfo->atmfAtmLayerMaxVpcs = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_VPCS_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxVpcs %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxVpcs));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VCCS:
              pAutoconfigConfigInfo->atmfAtmLayerMaxVccs = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_VCCS_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxVccs %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxVccs));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_CONFIGURED_VPCS:
              pAutoconfigConfigInfo->atmfAtmLayerConfiguredVpcs = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_CONFIGURED_VPCS_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerConfiguredVpcs %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerConfiguredVpcs));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_CONFIGURED_VCCS:
              pAutoconfigConfigInfo->atmfAtmLayerConfiguredVccs = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_CONFIGURED_VCCS_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerConfiguredVccs %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerConfiguredVccs));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VPI_BITS:
              pAutoconfigConfigInfo->atmfAtmLayerMaxVpibits = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_VPI_BITS_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxVpibits %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxVpibits));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_VCI_BITS:
              pAutoconfigConfigInfo->atmfAtmLayerMaxVcibits = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_VCI_BITS_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxVcibits %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxVcibits));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_UNI_TYPE:
              pAutoconfigConfigInfo->atmfAtmLayerUniType = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_UNI_TYPE_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerUniType %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerUniType));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_UNI_VERSION:
              pAutoconfigConfigInfo->atmfAtmLayerUniVersion = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_UNI_VERSION_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerUniVersion %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerUniVersion));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_DEVICE_TYPE:
              pAutoconfigConfigInfo->atmfAtmLayerDeviceType = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_DEVICE_TYPE_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerDeviceType %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerDeviceType));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_ILMI_VERSION:
              pAutoconfigConfigInfo->atmfAtmLayerIlmiVersion = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_ILMI_VERSION_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerIlmiVersion %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerIlmiVersion));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_NNI_SIG_VERSION:
              pAutoconfigConfigInfo->atmfAtmLayerNniSigVersion = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_NNI_SIG_VERSION_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerNniSigVersion %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerNniSigVersion));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_SVPC_VPI:
              pAutoconfigConfigInfo->atmfAtmLayerMaxSvpcVpi = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_SVPC_VPI_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxSvpcVpi %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxSvpcVpi));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_SVCC_VPI:
              pAutoconfigConfigInfo->atmfAtmLayerMaxSvccVpi = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_SVCC_VPI_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxSvccVpi %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxSvccVpi));
              break;
            case AUTOCONFIG_ATMF_ATMLAYER_ENTRY_MAX_SVCC_VCI:
              pAutoconfigConfigInfo->atmfAtmLayerMaxSvccVci = *(vars->val.integer);
              autoconfigConfigOidReceive |= AUTOCONFIG_CONFIG_INFO_MAX_SVCC_VCI_SET;
              DEBUGMSGTL(("atmforum/receiveConfigResponse","atmLayerMaxSvccVci %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerMaxSvccVci));
              break;
            } /* switch */
        } /* if isSubtree */
    } /* for loop */
  } /* if rStatus == success */
  if (rStatus == AUTOCONFIG_SUCCESS)
    rStatus = autoconfigConfigOidReceive;
  return rStatus;
}


/*****************************************************************************************
 * Summary: autoconfig_receiveServiceConnInfoResponse
 * Description: this function parse service connInfo response and store received info
 * Parameters:  
 *              input: pointer to received pdu
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
int autoconfig_receiveServiceConnInfoResponse(netsnmp_pdu *pdu)
{
  netsnmp_variable_list *vars;
  int             rStatus=AUTOCONFIG_SUCCESS;
  int autoconfigServiceConnInfoOidReceive=0;
  AUTOCONFIG_SERVICE_CONN_INFO receiveInfo;
  oid autoconfigServiceConnInfoOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_OID };
  int autoconfigServiceConnInfoOidLength = OID_LENGTH(autoconfigServiceConnInfoOid);
  int lastOid, len, cfgOk=AUTOCONFIG_SUCCESS;
  int oidSet = 0;

  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/autoconfig_receiveServiceConnInfoResponse","entry: pdu is NULL\n"));
    rStatus = -1;
  }
  else if (pdu->errstat != SNMP_ERR_NOERROR) {
    DEBUGMSGTL(("atmforum/autoconfig_receiveServiceConnInfoResponse","entry: pdu error %d\n",pdu->errstat));
    rStatus = -1;
  }

  DEBUGMSGTL(("atmforum/autoconfig_receiveServiceConnInfoResponse","entry\n"));

  if (rStatus == AUTOCONFIG_SUCCESS) {
    for (vars = pdu->variables; vars; vars = vars->next_variable) {
      if (netsnmp_oid_is_subtree(autoconfigServiceConnInfoOid,autoconfigServiceConnInfoOidLength,
                                 vars->name,vars->name_length) == 0) {
        lastOid = (int)vars->name[autoconfigServiceConnInfoOidLength];
        switch (lastOid)
          {
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_INDEX:
            receiveInfo.atmfServicePortIndex = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_PORT_INDEX_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServicePortIndex %d\n",
                        receiveInfo.atmfServicePortIndex));
            break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_VPI:
            receiveInfo.atmfServiceVpi = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_VPI_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse"," atmfServiceVpi %d\n",
                        receiveInfo.atmfServiceVpi));
            break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_VCI:
            receiveInfo.atmfServiceVci = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_VCI_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServiceVci %d\n",
                        receiveInfo.atmfServiceVci));
              break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_SIGNAL_ID:
            receiveInfo.atmfServiceSignalId = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_SIGNAL_ID_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServiceSignalId %d\n",
                        receiveInfo.atmfServiceSignalId));
            break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_SERVICE_INDEX:
            receiveInfo.atmfServiceConnServiceIndex = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_SERVICE_INDEX_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServiceConnServiceIndex %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerConfiguredVccs));
            break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_SERVICE_NAME:
            len = vars->val_len;
            if (len > AUTOCONIFG_DISPLAY_STRING_MAX)
              len = AUTOCONIFG_DISPLAY_STRING_MAX;
            memcpy(receiveInfo.atmfServiceConnServiceName,vars->val.string,len);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_SERVICE_CONN_NAME_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServiceConnServiceIndex %d\n",
                          pAutoconfigConfigInfo->atmfAtmLayerConfiguredVccs));
            break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_AAL_TYPE:
            receiveInfo.atmfServiceConnAALType = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_AAL_TYPE_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServiceConnAALType %d\n",
                        receiveInfo.atmfServiceConnAALType));
              break;
          case AUTOCONFIG_ATMF_SERVICE_CONNINFO_AAL_INDEX:
            receiveInfo.atmfServiceConnAALIndex = *(vars->val.integer);
            autoconfigServiceConnInfoOidReceive |= AUTOCONFIG_SERVICE_CONNINFO_AAL_INDEX_SET;
            DEBUGMSGTL(("atmforum/receiveServiceConnInfoResponse","atmfServiceConnAALIndex %d\n",
                        receiveInfo.atmfServiceConnAALIndex));
              break;
          } /* switch */
      } /* if subtree */
    } /* for all variables */
    if (rStatus == AUTOCONFIG_SUCCESS) {
      rStatus = autoconfigServiceConnInfoOidReceive;
      oidSet = autoconfigServiceConnInfoOidReceive;
    }
  } /* if rStatus == success */

  if (oidSet)
    cfgOk = autoconfig_verifyReceiveConnInfoCfg(oidSet,&receiveInfo,pdu);
    
#ifdef TEST
  /* this portion of code should be move to the state machine: after the state receives
     serviceConnInfo successfully, it should add this connInfo to the pAutoconfigServiceConnList */
  /* verify receive info; if error, send config fail trap */
  if ((rStatus == AUTOCONFIG_SERVICE_CONNINFO_ALL_SET) && (cfgOk == AUTOCONFIG_SUCCESS))
    {
      autoconfig_setServiceConnInfo(&receiveInfo);
    }
#endif
  return rStatus;
}

/*****************************************************************************************
 * Summary: autoconfig_receiveAal5ProfileResponse
 * Description: this function parse AAL5 profile response and store received info
 * Parameters:  
 *              input: pointer to received pdu
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
int autoconfig_receiveAal5ProfileResponse(netsnmp_pdu *pdu)
{
  netsnmp_variable_list *vars;
  int rStatus=AUTOCONFIG_SUCCESS;
  int oidReceive=0;
  AUTOCONFIG_AAL_PROFILE_INFO receiveInfo;
  oid autoconfigAal5ProfileEntryOid[] = { AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_OID };
  int autoconfigAal5ProfileEntryOidLength = OID_LENGTH(autoconfigAal5ProfileEntryOid);
  int lastOid;
  
  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","entry: pdu is NULL\n"));
    rStatus = -1;
  }
  else if (pdu->errstat != SNMP_ERR_NOERROR) {
    DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","entry: pdu error %d\n",pdu->errstat));
    rStatus = -1;
  }
  receiveInfo.type = ATMF_AAL5_PROFILE;

  if (rStatus == AUTOCONFIG_SUCCESS) {
    for (vars = pdu->variables; vars; vars = vars->next_variable) {
      if (netsnmp_oid_is_subtree(autoconfigAal5ProfileEntryOid,autoconfigAal5ProfileEntryOidLength,
                                 vars->name,vars->name_length) == 0) {
        lastOid = (int)vars->name[autoconfigAal5ProfileEntryOidLength];
        switch (lastOid)
          {
          case AUTOCONFIG_ATMF_AAL5_PROFILE_ENTRY_INDEX:
            receiveInfo.aal5.atmfAAL5ProfileIndex = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_AAL5_PROFILE_INDEX_SET;
            DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","atmfAAL5ProfileIndex %d\n",
                        receiveInfo.aal5.atmfAAL5ProfileIndex));
            break;
          case AUTOCONFIG_ATMF_AAL5_PROFILE_SDU_SIZE_FORWARD:
            receiveInfo.aal5.atmfAAL5MaxCpcsSduSizeForward = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_AAL5_MAX_SDU_SIZE_FORWARD_SET;
            DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","atmfAAL5MaxCpcsSduSizeForward %d\n",
                        receiveInfo.aal5.atmfAAL5MaxCpcsSduSizeForward));
            break;
          case AUTOCONFIG_ATMF_AAL5_PROFILE_SDU_SIZE_BACKWARD:
            receiveInfo.aal5.atmfAAL5MaxCpcsSduSizeBackward = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_AAL5_MAX_SDU_SIZE_BACKWARD_SET;
            DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","atmfAAL5MaxCpcsSduSizeBackward %d\n",
                        receiveInfo.aal5.atmfAAL5MaxCpcsSduSizeBackward));
              break;
          case AUTOCONFIG_ATMF_AAL5_PROFILE_AAL_MODE:
            receiveInfo.aal5.atmfAAL5AALMode = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_AAL5_AAL_MODE_SET;
            DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","atmfAAL5AALMode %d\n",
                        receiveInfo.aal5.atmfAAL5AALMode));
            break;
          case AUTOCONFIG_ATMF_AAL5_PROFILE_SSCS_TYPE:
            receiveInfo.aal5.atmfAAL5SscsType = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_AAL5_SSCS_TYPE_SET;
            DEBUGMSGTL(("atmforum/receiveAal5ProfileResponse","atmfAAL5SscsType %d\n",
                          receiveInfo.aal5.atmfAAL5SscsType));
            break;
          } /* switch */
      } /* if subtree */
    } /* for all variables */
    if (rStatus == AUTOCONFIG_SUCCESS) 
      rStatus = oidReceive;
  } /* if rStatus == success */

#ifdef TEST
  /* this portion of code should be move to the state machine: after the state receives
     serviceConnInfo successfully, it should add this connInfo to the pAutoconfigAALProfileList */
  if (rStatus == AUTOCONFIG_AAL5_PROFILE_ALL_SET) {
    autoconfig_setAALProfileInfo(&receiveInfo);
  } /* profile info all set */
#endif
  return rStatus;
}

/*****************************************************************************************
 * Summary: autoconfig_receiveServiceTypeResponse
 * Description: this function parse service type response and store received info
 * Parameters:  
 *              input: pointer to received pdu
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
int autoconfig_receiveServiceTypeResponse(netsnmp_pdu *pdu)
{
  netsnmp_variable_list *vars;
  int rStatus=AUTOCONFIG_SUCCESS;
  int oidReceive=0;
  AUTOCONFIG_SERVICE_TYPE_INFO receiveInfo;
  oid autoconfigServiceTypeEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_OID };
  int autoconfigServiceTypeEntryOidLength = OID_LENGTH(autoconfigServiceTypeEntryOid);
  oid autoconfigServiceTypeExtEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_OID };
  int autoconfigServiceTypeExtEntryOidLength = OID_LENGTH(autoconfigServiceTypeExtEntryOid);
  int lastOid, len;

  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","entry: pdu is NULL\n"));
    rStatus = -1;
  }
  else if (pdu->errstat != SNMP_ERR_NOERROR) {
    DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","entry: pdu error %d\n",pdu->errstat));
    rStatus = -1;
  }
  if (rStatus == AUTOCONFIG_SUCCESS) {
    for (vars = pdu->variables; vars; vars = vars->next_variable) {
      if (netsnmp_oid_is_subtree(autoconfigServiceTypeEntryOid,autoconfigServiceTypeEntryOidLength,
                                 vars->name,vars->name_length) == 0) {
        lastOid = (int)vars->name[autoconfigServiceTypeEntryOidLength];
        switch (lastOid)
          {
          case AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_INDEX:
            receiveInfo.atmfServiceTypeIndex = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_INDEX_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceTypeIndex %d\n",
                        receiveInfo.atmfServiceTypeIndex));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_PROVIDER_NAME:
            len = vars->val_len;
            if (len > AUTOCONIFG_DISPLAY_STRING_MAX)
              len = AUTOCONIFG_DISPLAY_STRING_MAX;
            memcpy(receiveInfo.atmfServiceProviderName,vars->val.string,len);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_PROVIDER_NAME_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceProviderName %s\n",
                        receiveInfo.atmfServiceProviderName));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_SERVICE_NAME:
            len = vars->val_len;
            if (len > AUTOCONIFG_DISPLAY_STRING_MAX)
              len = AUTOCONIFG_DISPLAY_STRING_MAX;
            memcpy(receiveInfo.atmfServiceName,vars->val.string,len);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_NAME_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceName %s\n",
                        receiveInfo.atmfServiceName));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_SERVICE_SUBNAME:
            len = vars->val_len;
            if (len > AUTOCONIFG_DISPLAY_STRING_MAX)
              len = AUTOCONIFG_DISPLAY_STRING_MAX;
            memcpy(receiveInfo.atmfServiceSubName,vars->val.string,len);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_SUB_NAME_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceSubName %s\n",
                        receiveInfo.atmfServiceSubName));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_CLIENT:
            len = vars->val_len;
            if (len > AUTOCONIFG_DISPLAY_STRING_MAX)
              len = AUTOCONIFG_DISPLAY_STRING_MAX;
            memcpy(receiveInfo.atmfServiceClient,vars->val.string,len);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_CLIENT_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceClient %s\n",
                        receiveInfo.atmfServiceClient));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_TM_CATEGORY:
            receiveInfo.atmfServiceTMCategory = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_TM_CATEGORY_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceTMCategory %d\n",
                          receiveInfo.atmfServiceTMCategory));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_TM_CONFORMANCEDEF:
            receiveInfo.atmfServiceTMConformanceDef = *(vars->val.integer);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_TM_CONFORM_DEF_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfServiceTMConformanceDef %d\n",
                          receiveInfo.atmfServiceTMConformanceDef));
            break;
          } /* switch */
      } /* if subtree */
      else if (netsnmp_oid_is_subtree(autoconfigServiceTypeExtEntryOid,autoconfigServiceTypeExtEntryOidLength,
                                      vars->name,vars->name_length) == 0) {
        lastOid = (int)vars->name[autoconfigServiceTypeExtEntryOidLength];
        switch (lastOid)
          {
          case AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_LAYER2_ID:
            len = vars->val_len;
            if (len > AUTOCONIFG_OCTETS_STRING_MAX)
              len = AUTOCONIFG_OCTETS_STRING_MAX;
            memcpy(receiveInfo.atmfAtmServiceLayer2ProtocolId,vars->val.string,len);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_EXT_LAYER2_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfAtmServiceLayer2ProtocolId %s\n",
                          receiveInfo.atmfAtmServiceLayer2ProtocolId));
            break;
          case AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_LAYER3_ID:
            len = vars->val_len;
            if (len > AUTOCONIFG_OCTETS_STRING_MAX)
              len = AUTOCONIFG_OCTETS_STRING_MAX;
            memcpy(receiveInfo.atmfAtmServiceLayer3ProtocolId,vars->val.string,len);
            oidReceive |= AUTOCONFIG_SERVICE_TYPE_EXT_LAYER3_SET;
            DEBUGMSGTL(("atmforum/receiveServiceTypeResponse","atmfAtmServiceLayer3ProtocolId %s\n",
                          receiveInfo.atmfAtmServiceLayer3ProtocolId));
            break;
          } /* switch */        
      } /* if ext table subtree */
    } /* for all variables */
    if (rStatus == AUTOCONFIG_SUCCESS) 
      rStatus = oidReceive;
  } /* if rStatus == success */

#ifdef TEST
  /* this portion of code should be move to the state machine: after the state receives
     serviceConnInfo successfully, it should add this connInfo to the pAutoconfigAALProfileList */
  if (rStatus == AUTOCONFIG_SERVICE_TYPE_ALL_SET) {
    autoconfig_setServiceTypeInfo(&receiveInfo);
  } /* service type all set */
#endif
  return rStatus;
}

/*****************************************************************************************
 * Summary: autoconfig_receiveVccResponse
 * Description: this function parse vcc response and store received info
 * Parameters:  
 *              input: pointer to received pdu
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
int autoconfig_receiveVccTableResponse(netsnmp_pdu *pdu, int *oidReceive)
{
  netsnmp_variable_list *vars;
  int             rStatus=AUTOCONFIG_SUCCESS;
  static AUTOCONFIG_VCC_INFO receiveInfo;
  oid autoconfigVccEntryOid[] = { AUTOCONFIG_ATMF_VCC_ENTRY_OID };
  int autoconfigVccEntryOidLength = OID_LENGTH(autoconfigVccEntryOid);
  int lastOid;

  if (pdu == NULL) {
    DEBUGMSGTL(("atmforum/receiveVccResponse","entry: pdu is NULL\n"));
    rStatus = -1;
  }
  else if (pdu->errstat != SNMP_ERR_NOERROR) {
    DEBUGMSGTL(("atmforum/autoconfig_receiveVccResponse","entry: pdu error %d\n",pdu->errstat));
    rStatus = -1;
  }
  if (rStatus == AUTOCONFIG_SUCCESS) {
    for (vars = pdu->variables; vars; vars = vars->next_variable) {
      if (netsnmp_oid_is_subtree(autoconfigVccEntryOid,autoconfigVccEntryOidLength,
                                 vars->name,vars->name_length) == 0) {
        lastOid = (int)vars->name[autoconfigVccEntryOidLength];
        switch (lastOid)
          {
          case AUTOCONFIG_ATMF_VCC_PORT_INDEX:
            receiveInfo.atmfVccPortIndex = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_PORT_INDEX_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccPortIndex %d\n",
                        receiveInfo.atmfVccPortIndex));
            break;
          case AUTOCONFIG_ATMF_VCC_VPI:
            receiveInfo.atmfVccVpi = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_VPI_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse"," atmfVccVpi %d\n",
                        receiveInfo.atmfVccVpi));
            break;
          case AUTOCONFIG_ATMF_VCC_VCI:
            receiveInfo.atmfVccVci = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_VCI_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccVci %d\n",
                        receiveInfo.atmfVccVci));
            break;
          case AUTOCONFIG_ATMF_VCC_OPER_STATUS:
            receiveInfo.atmfVccOperStatus = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_OPER_STATUS_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccOperStatus %d\n",
                        receiveInfo.atmfVccOperStatus));
            break;
          case AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_DESCRIPTOR_TYPE:
          case AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_DESCRIPTOR_TYPE:
          case COLUMN_ATMFVCCQOSCATEGORY:
          case COLUMN_ATMFVCCTRANSMITQOSCLASS:
          case COLUMN_ATMFVCCRECEIVEQOSCLASS:
          case AUTOCONFIG_ATMF_VCC_BEST_EFFORT_INDICATOR:
          case AUTOCONFIG_ATMF_VCC_SERVICE_CATEGORY:
            /* ignore; these are either obsolete or not-used objects */
            break;
          case AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM1:
            receiveInfo.atmfVccTransmitTrafficDescriptorParm1 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_TX_TRAFFIC_PARM1_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccTransmitTrafficDescriptorParm1 %d\n",
                        receiveInfo.atmfVccTransmitTrafficDescriptorParm1));
            break;
          case AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM2:
            receiveInfo.atmfVccTransmitTrafficDescriptorParm2 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_TX_TRAFFIC_PARM2_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccTransmitTrafficDescriptorParm2 %d\n",
                        receiveInfo.atmfVccTransmitTrafficDescriptorParm2));
            break;
          case AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM3:
            receiveInfo.atmfVccTransmitTrafficDescriptorParm3 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_TX_TRAFFIC_PARM3_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccTransmitTrafficDescriptorParm3 %d\n",
                        receiveInfo.atmfVccTransmitTrafficDescriptorParm3));
            break;
          case AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM4:
            receiveInfo.atmfVccTransmitTrafficDescriptorParm4 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_TX_TRAFFIC_PARM4_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccTransmitTrafficDescriptorParm4 %d\n",
                        receiveInfo.atmfVccTransmitTrafficDescriptorParm4));
            break;
          case AUTOCONFIG_ATMF_VCC_TX_TRAFFIC_PARM5:
            receiveInfo.atmfVccTransmitTrafficDescriptorParm5 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_TX_TRAFFIC_PARM5_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccTransmitTrafficDescriptorParm5 %d\n",
                        receiveInfo.atmfVccTransmitTrafficDescriptorParm5));
            break;
          case AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM1:
            receiveInfo.atmfVccReceiveTrafficDescriptorParm1 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_RX_TRAFFIC_PARM1_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccReceiveTrafficDescriptorParm1 %d\n",
                        receiveInfo.atmfVccReceiveTrafficDescriptorParm1));
            break;
          case AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM2:
            receiveInfo.atmfVccReceiveTrafficDescriptorParm2 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_RX_TRAFFIC_PARM2_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccReceiveTrafficDescriptorParm2 %d\n",
                        receiveInfo.atmfVccReceiveTrafficDescriptorParm2));
            break;
          case AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM3:
            receiveInfo.atmfVccReceiveTrafficDescriptorParm3 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_RX_TRAFFIC_PARM3_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccReceiveTrafficDescriptorParm3 %d\n",
                        receiveInfo.atmfVccReceiveTrafficDescriptorParm3));
            break;
          case AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM4:
            receiveInfo.atmfVccReceiveTrafficDescriptorParm4 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_RX_TRAFFIC_PARM4_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccReceiveTrafficDescriptorParm4 %d\n",
                        receiveInfo.atmfVccReceiveTrafficDescriptorParm4));
            break;
          case AUTOCONFIG_ATMF_VCC_RX_TRAFFIC_PARM5:
            receiveInfo.atmfVccReceiveTrafficDescriptorParm5 = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_RX_TRAFFIC_PARM5_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccReceiveTrafficDescriptorParm5 %d\n",
                        receiveInfo.atmfVccReceiveTrafficDescriptorParm5));
            break;
          case AUTOCONFIG_ATMF_VCC_TX_FRAME_DISCARD:
            receiveInfo.atmfVccTransmitFrameDiscard = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_TX_FRAME_DISCARD_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccTransmitFrameDiscard %d\n",
                        receiveInfo.atmfVccTransmitFrameDiscard));
            break;
          case AUTOCONFIG_ATMF_VCC_RX_FRAME_DISCARD:
            receiveInfo.atmfVccReceiveFrameDiscard = *(vars->val.integer);
            *oidReceive |= AUTOCONFIG_VCC_RX_FRAME_DISCARD_SET;
            DEBUGMSGTL(("atmforum/receiveVccResponse","atmfVccReceiveFrameDiscard %d\n",
                        receiveInfo.atmfVccReceiveFrameDiscard));
            break;
          } /* switch */
      } /* if subtree */
    } /* for all variables */
    if (rStatus == AUTOCONFIG_SUCCESS) 
      rStatus = *oidReceive;
  } /* if rStatus == success */

#ifdef TEST
  /* this portion of code should be move to the state machine: after the state receives
     serviceConnInfo successfully, it should add this connInfo to the pAutoconfigServiceConnList */
  if (rStatus == AUTOCONFIG_VCC_ALL_SET) {
    autoconfig_setVccInfo(&receiveInfo);
    memset((void*)&receiveInfo,0,sizeof(receiveInfo));
  }
#endif
  return rStatus;
}

/*****************************************************************************************
 * Summary: autoconconfig_verifyReceiveConnInfoCfg
 * Description: this function verifies received connection info information from remote
 *              access node.    It sends autoconfig fail trap to remote side if an error
 *              is found.
 * Parameters:  
 *              input: oidSet - integer indicating how many fields have been received.
 *                      receiveInfo - pointer to received autoconfigServiceConnInfo
 *                      pointer to received pdu
 *              output: rStatus AUTOCONFIG_SUCCESS and AUTOCONFIG_ERROR
******************************************************************************************/
int autoconfig_verifyReceiveConnInfoCfg(int oidSet, pAUTOCONFIG_SERVICE_CONN_INFO receiveInfo,
                                        netsnmp_pdu *pdu)
{
  int rStatus = AUTOCONFIG_SUCCESS;
  netsnmp_variable_list *vars;
  int currentVccCount;
  int lastOid;
  oid autoconfigServiceConnInfoOid[] = { AUTOCONFIG_ATMF_SERVICE_CONNINFO_ENTRY_OID };
  int autoconfigServiceConnInfoOidLength = OID_LENGTH(autoconfigServiceConnInfoOid);
  int maxVpi = (2 << ATM_INTF_CURRENT_MAX_VPI_BITS);
  int maxVci = (2 << ATM_INTF_CURRENT_MAX_VCI_BITS);

  if (oidSet != AUTOCONFIG_SERVICE_CONNINFO_ALL_SET) {
    DEBUGMSGTL(("atmforum/autoconfig_verifyReceiveConnInfoCfg","incomplete_info: oidSet %d\n",
                oidSet));
    rStatus = AUTOCONFIG_ERROR;
    autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_INCOMPLETE_INFO,pdu->variables);
  }
  else {
    /* does this VPI/VCI exceeds modem max supported VCs? if yes, send AUTOCONFIG_ATMF_CONFIG_FAIL_TOO_MANY_PVCS */
    currentVccCount = atmMibHelper_getTotalVccCount();
    if (currentVccCount == ATM_INTF_MAX_VCCS) {
      DEBUGMSGTL(("atmforum/autoconfig_verifyReceiveConnInfoCfg","totalVccExceeded %d\n",
                  currentVccCount));
      rStatus = AUTOCONFIG_ERROR;
      autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_TOO_MANY_PVCS,pdu->variables);
    }
    if (rStatus != AUTOCONFIG_ERROR) {
      /* is this a valid VPI/VCI? if no, send AUTOCONFIG_ATMF_CONFIG_FAIL_INVALID_VPI_VCI */
      if ((receiveInfo->atmfServiceVpi < 0) || (receiveInfo->atmfServiceVpi >= maxVpi)) {
        DEBUGMSGTL(("atmforum/autoconfig_verifyReceiveConnInfoCfg","invalid VPI %d\n",
                    receiveInfo->atmfServiceVpi)); 
        rStatus = AUTOCONFIG_ERROR;
        for (vars = pdu->variables; vars; vars = vars->next_variable) {
          lastOid = (int)vars->name[autoconfigServiceConnInfoOidLength];
          if (lastOid == AUTOCONFIG_ATMF_SERVICE_CONNINFO_VPI) 
            break;
        }
        autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_INVALID_VPI_VCI,vars);
      }
      else if 
        ((receiveInfo->atmfServiceVci < 32) ||
         (receiveInfo->atmfServiceVci >= maxVci)) {
        DEBUGMSGTL(("atmforum/autoconfig_verifyReceiveConnInfoCfg","invalid VCI %d (32-%d)\n",
                    receiveInfo->atmfServiceVci,maxVci));
        rStatus = AUTOCONFIG_ERROR;
        for (vars = pdu->variables; vars; vars = vars->next_variable) {
          lastOid = (int)vars->name[autoconfigServiceConnInfoOidLength];
          if (lastOid == AUTOCONFIG_ATMF_SERVICE_CONNINFO_VCI) {
            break;
          }
        } /* for */
        autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_INVALID_VPI_VCI,vars);
      } /* if vci error */
    } /* if rstatus error */
    if (rStatus != AUTOCONFIG_ERROR) {
      /* if atmfServiceConnAALType != AAL5, send AUTOCONFIG_ATMF_CONFIG_FAIL_UNSUPPORTED_AAL */
      if (receiveInfo->atmfServiceConnAALType != ATMF_CONN_AAL_TYPE_AAL5) {
        DEBUGMSGTL(("atmforum/autoconfig_verifyReceiveConnInfoCfg","invalid AAL %d\n",
                    receiveInfo->atmfServiceConnAALType));
        for (vars = pdu->variables; vars; vars = vars->next_variable) {
          lastOid = (int)vars->name[autoconfigServiceConnInfoOidLength];
          if (lastOid == AUTOCONFIG_ATMF_SERVICE_CONNINFO_AAL_TYPE) {
            break;
          }
        }
        rStatus = AUTOCONFIG_ERROR;
        autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_UNSUPPORTED_AAL,vars);
      } /* if  receive non aal5*/
    }  /* rStatus != error */
  } 
  return (rStatus);
}

int autoconconfig_verifyReceiveServiceTypeCfg(int oidSet, pAUTOCONFIG_SERVICE_TYPE_INFO receiveInfo,
                                              netsnmp_pdu *pdu)
{
  netsnmp_variable_list *vars=NULL;
  int rStatus = AUTOCONFIG_SUCCESS;
  int lastOid;
  oid autoconfigServiceTypeEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_ENTRY_OID };
  int autoconfigServiceTypeEntryOidLength = OID_LENGTH(autoconfigServiceTypeEntryOid);
  oid autoconfigServiceTypeExtEntryOid[] = { AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_ENTRY_OID };
  int autoconfigServiceTypeExtEntryOidLength = OID_LENGTH(autoconfigServiceTypeExtEntryOid);

  if (oidSet != AUTOCONFIG_SERVICE_TYPE_ALL_SET) {
    rStatus = AUTOCONFIG_ERROR;
    autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_INCOMPLETE_INFO,pdu->variables);
  }
  else {
    /* if atmfAtmServiceLayer2ProtocolId and/or atmfAtmServiceLayer3ProtocolId are not supported, send 
       AUTOCONFIG_ATMF_CONFIG_FAIL_UNSUPPORTED_CLIENT_PROTOCOL */
    if ((strncasecmp(receiveInfo->atmfAtmServiceLayer2ProtocolId,
                     AUTOCONFIG_LAYER2_LLC_ENCAP,1)) ||
        (strncasecmp(receiveInfo->atmfAtmServiceLayer2ProtocolId,
                     AUTOCONFIG_LAYER2_NULL_ENCAP,1))) {
      for (vars = pdu->variables; vars; vars = vars->next_variable) {
        lastOid = (int)vars->name[autoconfigServiceTypeExtEntryOidLength];
        if (lastOid == AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_LAYER2_ID) {
          break;
        }
      }
      rStatus = AUTOCONFIG_ERROR;
    } /* layer 2 */
    else if ((strncasecmp(receiveInfo->atmfAtmServiceLayer3ProtocolId,
                          AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_NO_FCS,
                          strlen(AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_NO_FCS))) && 
             (strncasecmp(receiveInfo->atmfAtmServiceLayer3ProtocolId,
                          AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_NO_FCS_IP,
                          strlen(AUTOCONFIG_LAYER3_BRIDGE_LLC_SNAP_NO_FCS_IP))) &&
             (strncasecmp(receiveInfo->atmfAtmServiceLayer3ProtocolId,
                          AUTOCONFIG_LAYER3_PPPOE_LLC_SNAP_NO_FCS,
                          strlen(AUTOCONFIG_LAYER3_PPPOE_LLC_SNAP_NO_FCS))) && 
             (strncasecmp(receiveInfo->atmfAtmServiceLayer3ProtocolId,
                          AUTOCONFIG_LAYER3_PPPOA,
                          strlen(AUTOCONFIG_LAYER3_PPPOA))) && 
             (strncasecmp(receiveInfo->atmfAtmServiceLayer3ProtocolId,
                          AUTOCONFIG_LAYER3_IPOA_LLC_SNAP,
                          strlen(AUTOCONFIG_LAYER3_IPOA_LLC_SNAP))) && 
             (strncasecmp(receiveInfo->atmfAtmServiceLayer3ProtocolId,
                          AUTOCONFIG_LAYER3_IPOA_NULL,
                          strlen(AUTOCONFIG_LAYER3_IPOA_NULL)))) {
      for (vars = pdu->variables; vars; vars = vars->next_variable) {
        lastOid = (int)vars->name[autoconfigServiceTypeExtEntryOidLength];
        if (lastOid == AUTOCONFIG_ATMF_SERVICE_TYPE_EXT_LAYER3_ID) {
          break;
        }
      }
      rStatus = AUTOCONFIG_ERROR;
    }   /* layer 3 */
    if (rStatus == AUTOCONFIG_ERROR)
      autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_UNSUPPORTED_CLIENT_PROTOCOL,vars);
    else {
      /* if atmfServiceTMCategory is not supported, send
         AUTOCONFIG_ATMF_CONFIG_FAIL_UNSOPPORTED_SERVICE_CAT */
      if (((receiveInfo->atmfServiceTMCategory < ATMF_TDT_SERVICE_OTHER) ||
           (receiveInfo->atmfServiceTMCategory > ATMF_TDT_SERVICE_NRT_VBR)) &&
          (receiveInfo->atmfServiceTMCategory != ATMF_TDT_SERVICE_UBR)) {
        for (vars = pdu->variables; vars; vars = vars->next_variable) {
          lastOid = (int)vars->name[autoconfigServiceTypeEntryOidLength];
          if (lastOid == AUTOCONFIG_ATMF_SERVICE_TYPE_TM_CATEGORY) {
            break;
          }
        }
        rStatus = AUTOCONFIG_ERROR;
        autoconfig_sendConfigFailTrap(AUTOCONFIG_ATMF_CONFIG_FAIL_UNSOPPORTED_SERVICE_CAT,vars);
      } /* if service cat error */
    }
  } /* service cat or layer2 error? */
  return (rStatus);
}

/*****************************************************************************************
 * DEBUG routines                                                                        *
 ****************************************************************************************/
void autoconfig_displayAttachPointParams(void)
{
  printf("\n################################################################\n");
  printf("autconfig_displayAttachPointParams()\n");
  if (pAutoconfigAttachPointInfo != NULL) {
    printf("Peer's attach point:\n");
    printf("atmfPortMyIfIdentifier %d, atmfMySystemIdentifier %s, sysUptime %d\n",
           pAutoconfigAttachPointInfo->atmfPortMyIfIdentifier,
           pAutoconfigAttachPointInfo->atmfMySystemIdentifier,
           pAutoconfigAttachPointInfo->sysUptime);
  }
  else
    printf("pAutoconfigAttachPointInfo is NULL\n");
  printf("\n################################################################\n");
}

void autoconfig_displayConfigParams(void)
{
  printf("\n################################################################\n");
  printf("autconfig_displayConfigParams()\n");
  if (pAutoconfigConfigInfo != NULL) {
    printf("Peer's AtmLayer Configuration:\n");
    printf("Index %d, maxVpcs %d, maxVccs %d, configuredVpcs %d, configuredVccs %d\n",
           pAutoconfigConfigInfo->atmfAtmLayerIndex,pAutoconfigConfigInfo->atmfAtmLayerMaxVpcs,
           pAutoconfigConfigInfo->atmfAtmLayerMaxVccs,pAutoconfigConfigInfo->atmfAtmLayerConfiguredVpcs,
           pAutoconfigConfigInfo->atmfAtmLayerConfiguredVccs);
    printf("maxVpibits %d, maxVcibits %d, uniType %d, uniVersion %d\n",
           pAutoconfigConfigInfo->atmfAtmLayerMaxVpibits,pAutoconfigConfigInfo->atmfAtmLayerMaxVcibits,
           pAutoconfigConfigInfo->atmfAtmLayerUniType,pAutoconfigConfigInfo->atmfAtmLayerUniVersion);
    printf("deviceType %d, ilmiVersion %d, nniVersion %d, maxSvpcVpi %d\n",
           pAutoconfigConfigInfo->atmfAtmLayerDeviceType,pAutoconfigConfigInfo->atmfAtmLayerIlmiVersion,
           pAutoconfigConfigInfo->atmfAtmLayerNniSigVersion,pAutoconfigConfigInfo->atmfAtmLayerMaxSvpcVpi);
    printf("maxSvccVpi %d, maxSvccVci %d\n",pAutoconfigConfigInfo->atmfAtmLayerMaxSvccVpi,
           pAutoconfigConfigInfo->atmfAtmLayerMaxSvccVci);
  }
  else
    printf("pAutoconfigConfigInfo is NULL\n");
  printf("\n################################################################\n");
}

void autoconfig_displayServiceConnList(void)
{
  netsnmp_data_list *ptr;
  pAUTOCONFIG_SERVICE_CONN_INFO pInfo;

  printf("\n################################################################\n");
  printf("autoconfig_displayServiceConnList(list %x)\n",(int)pAutoconfigServiceConnList);
  if (pAutoconfigServiceConnList) {
    for (ptr= pAutoconfigServiceConnList; ptr; ptr=ptr->next) {
      pInfo = (pAUTOCONFIG_SERVICE_CONN_INFO)ptr->data;
      printf("\n***********************************************\n");
      printf("atmfServicePortIndex %d, vpi/vci %d/%d\n",
             pInfo->atmfServicePortIndex,pInfo->atmfServiceVpi,pInfo->atmfServiceVci);
      printf("signalId %d, connServiceIndex %d\n",
             pInfo->atmfServiceSignalId, pInfo->atmfServiceConnServiceIndex);
      printf("connServiceName %s, aalType %d, aalIndex %d\n",pInfo->atmfServiceConnServiceName,
             pInfo->atmfServiceConnAALType,pInfo->atmfServiceConnAALIndex);
      printf("\n***********************************************\n");
    } /* each entry in serviceConnList */
  }
  else
    printf("pAutoconfigServiceConnList is NULL\n");
  printf("\n################################################################\n");
}

void autoconfig_displayAALProfileList(void)
{
  netsnmp_data_list *ptr;
  pAUTOCONFIG_AAL_PROFILE_INFO pInfo;

  printf("\n################################################################\n");  
  printf("autoconfig_displayAALProfileList(list %x)\n",(int)pAutoconfigAALProfileList);
  if (pAutoconfigAALProfileList) {
    for (ptr= pAutoconfigAALProfileList; ptr; ptr=ptr->next) {
      pInfo = (pAUTOCONFIG_AAL_PROFILE_INFO)ptr->data;
      printf("\n***********************************************\n");
      if (pInfo->type == ATMF_AAL5_PROFILE) {
        printf("atmfAAL5ProfileIndex %d, atmfAAL5MaxCpcsSduSizeForward %d\n",
               pInfo->aal5.atmfAAL5ProfileIndex,pInfo->aal5.atmfAAL5MaxCpcsSduSizeForward);
        printf("atmfAAL5MaxCpcsSduSizeBackward %d, atmfAAL5AALMode %d\n",
               pInfo->aal5.atmfAAL5MaxCpcsSduSizeBackward,pInfo->aal5.atmfAAL5AALMode);
        printf("atmfAAL5SscsType %d\n",pInfo->aal5.atmfAAL5SscsType);
      }
      printf("\n***********************************************\n");
    } /* each entry in AALProfileList */
  }
  else
    printf("pAutoconfigAALProfileList is NULL\n");
  printf("\n################################################################\n");
}

void autoconfig_displayServiceTypeList(void)
{
  netsnmp_data_list *ptr;
  pAUTOCONFIG_SERVICE_TYPE_INFO pInfo;
  
  printf("\n################################################################\n");
  printf("autoconfig_displayServiceTypeList(list %x)\n",(int)pAutoconfigServiceTypeList);
  if (pAutoconfigServiceTypeList) {
    for (ptr= pAutoconfigServiceTypeList; ptr; ptr=ptr->next) {
      pInfo = (pAUTOCONFIG_SERVICE_TYPE_INFO)ptr->data;
      printf("\n***********************************************\n");
      printf("atmfServiceTypeIndex %d, atmfServiceProviderName %s\n",
             pInfo->atmfServiceTypeIndex,pInfo->atmfServiceProviderName);
      printf("atmfServiceName %s, atmfServiceSubName %s\n",
             pInfo->atmfServiceName,pInfo->atmfServiceSubName);
      printf("atmfServiceClient %s\n",pInfo->atmfServiceClient);
      printf("atmfServiceTMCategory %d, atmfServiceTMConformanceDef %d\n",
             pInfo->atmfServiceTMCategory,pInfo->atmfServiceTMConformanceDef);
      printf("atmfAtmServiceLayer2ProtocolId %s\n",pInfo->atmfAtmServiceLayer2ProtocolId);
      printf("atmfAtmServiceLayer3ProtocolId %s\n",pInfo->atmfAtmServiceLayer3ProtocolId);
      printf("Reference Count %d\n",pInfo->referenceCount);
      printf("\n***********************************************\n");
    } /* each entry in serviceTypeList */
  }
  else
    printf("pAutoconfigServiceTypeList is NULL\n");
  printf("\n################################################################\n");
}

void autoconfig_displayVccList(void)
{
  netsnmp_data_list *ptr;
  pAUTOCONFIG_VCC_INFO pInfo;

  printf("\n################################################################\n");
  printf("autoconfig_displayVccList(list %x)\n",(int)pAutoconfigVccList);
  if (pAutoconfigVccList) {
    for (ptr= pAutoconfigVccList; ptr; ptr=ptr->next) {
      pInfo = (pAUTOCONFIG_VCC_INFO)ptr->data;
      printf("\n***********************************************\n");
      printf("atmfVccPortIndex %d, vpi/vci %d/%d\n",
             pInfo->atmfVccPortIndex,pInfo->atmfVccVpi,pInfo->atmfVccVci);
      printf("operStatus %d, TransmitTrafficDescriptorType %d\n",
             pInfo->atmfVccOperStatus,pInfo->atmfVccTransmitTrafficDescriptorType);
      printf("Parm1 %d, Parm2 %d, Parm3 %d, Parm4 %d, Parm5 %d\n",
             pInfo->atmfVccTransmitTrafficDescriptorParm1,
             pInfo->atmfVccTransmitTrafficDescriptorParm2,
             pInfo->atmfVccTransmitTrafficDescriptorParm3,
             pInfo->atmfVccTransmitTrafficDescriptorParm4,
             pInfo->atmfVccTransmitTrafficDescriptorParm5);
      printf("atmfVccReceiveTrafficDescriptorType %d\n",
             pInfo->atmfVccReceiveTrafficDescriptorType);
      printf("Parm1 %d, Parm2 %d, Parm3 %d, Parm4 %d, Parm5 %d\n",
             pInfo->atmfVccReceiveTrafficDescriptorParm1,
             pInfo->atmfVccReceiveTrafficDescriptorParm2,
             pInfo->atmfVccReceiveTrafficDescriptorParm3,
             pInfo->atmfVccReceiveTrafficDescriptorParm4,
             pInfo->atmfVccReceiveTrafficDescriptorParm5);
      printf("atmfVccTransmitQosClass %d, atmfVccBestEffortIndicator %d\n",
             pInfo->atmfVccTransmitQosClass, pInfo->atmfVccBestEffortIndicator);
      printf("atmfVccTransmitFrameDiscard %d, atmfVccReceiveFrameDiscard %d\n",
             pInfo->atmfVccTransmitFrameDiscard, pInfo->atmfVccReceiveFrameDiscard);
      printf("atmfVccServiceCategory %d\n", pInfo->atmfVccServiceCategory);
      printf("\n***********************************************\n");
    } /* each entry in serviceConnList */
  }
  else
    printf("pAutoconfigVccList is NULL\n");
  printf("\n################################################################\n");
}

void autoconfig_displayRequestList(void)
{
  netsnmp_data_list *ptr;

  printf("\n################################################################\n");
  printf("autoconfig_displayRequestList(list %x)\n",(int)pAutoconfigRequestList);
  if (pAutoconfigRequestList) {
    for (ptr= pAutoconfigRequestList; ptr; ptr=ptr->next) 
      printf("requestId %s\n",ptr->name);
  }
  else
    printf("pAutoconfigRequestList is NULL\n");
  printf("\n################################################################\n");
}

/******************************************************************************************/

/*****************************************************************************************
 * Summary: autoconfig_loop() is the statemachine loop
 * Description: 
 *              
 * Parameters: 
 *              
 *              output: rStatus -1 for error; 0 success.
******************************************************************************************/
void autoconfig_loop(void)
{
#if 0
  int state = S1;
  struct itimerval setpoint;

  //  esi_oid = get_esi( fd, itf);
  //	apoint->atmfMySystemIdentifier.octs = &(apoint->sysIdMem[0]);
  setpoint.it_interval.tv_sec = 30;
  setpoint.it_interval.tv_usec = 0;
  setpoint.it_value.tv_sec = 30;
  setpoint.it_value.tv_usec = 0;
  signal( SIGALRM, handler );
  setitimer( ITIMER_REAL, &setpoint, NULL );
  
  for( ;; ) 
    state = state_functions[state]( fd, itf, msgs );
#endif
}

#endif /*SNMP_AUTOCONFIG */

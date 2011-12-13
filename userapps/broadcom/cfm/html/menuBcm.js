var MENU_OPTION_USER              = 0;
var MENU_OPTION_STANDARD          = 1;
var MENU_OPTION_PROTOCOL          = 2;
var MENU_OPTION_FIREWALL          = 3;
var MENU_OPTION_NAT               = 4;
var MENU_OPTION_IP_EXTENSION      = 5;
var MENU_OPTION_WIRELESS          = 6;
var MENU_OPTION_VOICE             = 7;
var MENU_OPTION_SNMP              = 8;
var MENU_OPTION_UPNP              = 9;
var MENU_OPTION_DDNSD             = 10;
var MENU_OPTION_SNTP              = 11;
var MENU_OPTION_EBTABLES          = 12;
var MENU_OPTION_BRIDGE            = 13;
var MENU_OPTION_TOD               = 14;
var MENU_OPTION_SIPROXD           = 15;
var MENU_OPTION_DHCPEN            = 16;
var MENU_OPTION_QOS               = 17;
var MENU_OPTION_PORTMAP           = 18;
var MENU_OPTION_IPP               = 19;
var MENU_OPTION_WIRELESS_SES      = 20;
var MENU_OPTION_RIP               = 21;
var MENU_OPTION_IPSEC             = 22;
var MENU_OPTION_CERT              = 23;
var MENU_OPTION_WL_QOS            = 24;
var MENU_OPTION_TR69C             = 25;
var MENU_OPTION_VDSL              = 26;
var MENU_OPTION_IPV6_SUPPORT      = 27;
var MENU_OPTION_IPV6_ENABLE       = 28;
var MENU_OPTION_WIRELESS_NUM_ADAPTOR      = 29;
var MENU_OPTION_POLICY_ROUTING    = 30;
var MENU_DARE_TRACEPING           = 31;
var MENU_OPTION_DARE_PPTP         = 32;

 var wlItemsCgiCmd = new Array(
 	                    'wlswitchinterface0.wl',
                           'wlswitchinterface1.wl',
                           'wlswitchinterface2.wl',
                           'wlswitchinterface3.wl'
               
                          );

 var wlmenuTitle = new Array(
 	                    'wl0',
                           'wl1',
                           'wl2',
                           'wl3'
               
                          );

function menuAdmin(options) {
   var std = options[MENU_OPTION_STANDARD];
   var proto = options[MENU_OPTION_PROTOCOL];
   var firewall = options[MENU_OPTION_FIREWALL];
   var nat = options[MENU_OPTION_NAT];
   var ipExt = options[MENU_OPTION_IP_EXTENSION];
   var wireless = options[MENU_OPTION_WIRELESS];
   var voice = options[MENU_OPTION_VOICE];
   var snmp = options[MENU_OPTION_SNMP];
   var ddnsd = options[MENU_OPTION_DDNSD];
   var sntp = options[MENU_OPTION_SNTP];
   var ebtables = options[MENU_OPTION_EBTABLES];
   var bridge = options[MENU_OPTION_BRIDGE];
   var tod = options[MENU_OPTION_TOD];
   var siproxd = options[MENU_OPTION_SIPROXD];
   var QosEnabled = options[MENU_OPTION_QOS];
   var vlanconfig = options[MENU_OPTION_PORTMAP];
   var ipp = options[MENU_OPTION_IPP];
   var wireless_ses = options[MENU_OPTION_WIRELESS_SES];
   var rip = options[MENU_OPTION_RIP];
   var ipsec = options[MENU_OPTION_IPSEC];
   var certificate = options[MENU_OPTION_CERT];
   var wlqos = options[MENU_OPTION_WL_QOS];
   var tr69c = options[MENU_OPTION_TR69C];
   var ipv6Support = options[MENU_OPTION_IPV6_SUPPORT];
   var ipv6Enable = options[MENU_OPTION_IPV6_ENABLE];
   var vdslIncluded = options[MENU_OPTION_VDSL];
   var numWl = options[MENU_OPTION_WIRELESS_NUM_ADAPTOR];
   var pr = options[MENU_OPTION_POLICY_ROUTING];
   var traceping = options[MENU_DARE_TRACEPING];
   var pptp = options[MENU_OPTION_DARE_PPTP];
   
   var i = 0;
   
   // Configure quick setup wizard
   if ( proto == 'Not Applicable' )
      nodeQuickSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_QUICK_SETUP), 'vpivci.cgi'));

   nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'wancfg.cmd'));
   insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_WAN),'wancfg.cmd'));
   insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_LAN),'lancfg2.html'));

   // Configure security menu
   // If firewall is enabled and not in ipExt mode enable firewall menus
   // if (proto != 'Bridge' && ipExt != '1' ) {
   if ( proto != 'Not Applicable' && ipExt != '1' ) {
      // If NAT is On enable virtual server menus
      if ( nat == '1' ) {
         nodeNat = insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_SC_NAT), 'scvrtsrv.cmd?action=view'));
         insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_SC_VIRTUAL_SERVER), 'scvrtsrv.cmd?action=view'));
         insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_SC_PORT_TRIGGER), 'scprttrg.cmd?action=view'));
         insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_SC_DMZ_HOST), 'scdmz.html'));
         if ( siproxd == '1' )
            insDoc(nodeNat, gLnk('R', getMenuTitle(MENU_ALGS), 'algcfg.html'));
      }
      if ( proto != 'Bridge' && firewall == '1' ) {
         nodeFirewall = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_SC_SECURITY), 'scoutflt.cmd?action=view'));
         nodeIpFlt = insFld(nodeFirewall, gFld(getMenuTitle(MENU_SC_IP_FILTER), 'scoutflt.cmd?action=view'));
         insDoc(nodeIpFlt, gLnk('R', getMenuTitle(MENU_SC_OUTGOING), 'scoutflt.cmd?action=view'));
         insDoc(nodeIpFlt, gLnk('R', getMenuTitle(MENU_SC_INCOMING), 'scinflt.cmd?action=view'));
      } else {
         if ( ebtables == '1' ) {
            if ( bridge == '1' )
               nodeFirewall = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_SC_SECURITY),'scmacflt.cmd?action=view'));
//            else if ( tod == '1' )
//               nodeFirewall = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_TOD),'todmngr.tod?action=view'));
         }
      }

      // Configure MAC filtering menu
      if ( ebtables == '1' ) {
         if ( bridge == '1' )
            insFld(nodeFirewall, gFld(getMenuTitle(MENU_MAC_FILTER),'scmacflt.cmd?action=view'));
         if ( tod == '1' ){
            nodeParentalControl = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_TOD),'todmngr.tod?action=view'));
	    insDoc(nodeParentalControl, gLnk('R', getMenuTitle(MENU_TOD_URL), 'todmngr.tod?action=urlview'));	//UrlFilter modification
	 }
      }
   }

   // Configure QoS class menu
   nodeQos = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_QOS),'qosqmgmt.html'));
   insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_QUEUE), 'qosqueue.cmd?action=view'));
   insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_CLASS), 'qoscls.cmd?action=view'));

   
   // Configure routing menu
   nodeRouting = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_ROUTING), 'rtdefaultcfg.html'));
   insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_DEFAULT_ROUTE), 'rtdefaultcfg.html'));
   insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE),'rtroutecfg.cmd?action=viewcfg'));
   if (pr == '1' )
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_POLICY_ROUTING),'prmngr.pr?action=view'));	//PolicyRouting modification

   if ( (proto == 'PPPoE' && ipExt == '0') ||
        (proto == 'PPPoA' && ipExt == '0') ||
        (proto == 'MER') ||
        (proto == 'IPoA') ) {
      // configure rip
      if ( rip == '1' )
         insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_RIP),'ripcfg.cmd?action=view'));
      // configure dns server
      nodeDnsSetup = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DNS), 'dnscfg.html'));
      insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DNS_SETUP), 'dnscfg.html'));
      // configure ddns client
      if ( ddnsd == '1' )
         insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DDNS), 'ddnsmngr.cmd'));
   }

   // configure ipv6 default gateway
   if ( ipv6Enable == '1' ) {
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_DEFAULT_ROUTE6), 'rtdefaultcfg6.html'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE6), 'rtroute6cfg.cmd?action=viewcfg'));
   }

   // Configure ADSL Setting Menu based on Annex
   if ( std == 'annex_c' )
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfgc.html'));
   else
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfg.html'));

   insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DARE_PPTP), 'pptp.html'));
   // Configure print server
   if ( ipp == '1' )
      insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_IPP), 'ippcfg.html'));

   if ( wireless == '1' ) {

       if(numWl != '1')
           wlanMenu = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));
 
       for(i = 0; i < parseInt(numWl); i++)
       {
   // Configure wireless menu
            if(numWl == '1')
                nodeWireless = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));
	     else
		  nodeWireless = insFld(wlanMenu, gFld(wlmenuTitle[i], wlItemsCgiCmd[i]));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_BASIC), 'wlcfg.html'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SECURITY), 'wlsecurity.html'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_MAC_FILTERING), 'wlmacflt.cmd?action=view'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_WDS), 'wlwds.cmd?action=view'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_ADVANCED), 'wlcfgadv.html'));
      //SUPPORT_SES
      if ( wireless_ses == '1' ) { 
         insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SES), 'wlses.html'));      
      }
      
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_STATION_LIST), 'wlstationlist.cmd'));
   


        }
   }

   // Configure voice menu
   if ( voice == 'mgcp' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voicemgcpview.cmd'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_MGCP), 'voicemgcpview.cmd'));
      //insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_STATS), 'voicestats45.html'));
   }
   else if ( voice == 'sip' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voicesipview.cmd'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_BASIC), 'voicesipview.cmd?view=basic'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_ADVANCED), 'voicesipview.cmd?view=advanced'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_DEBUG), 'voicesipview.cmd?view=debug'));
   }

   // Configure VLAN port mapping menu
   if ( vlanconfig == '1' ) {
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_PORTMAP),'portmap.cmd'));
   }

   if ( ipsec == '1' ) {
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_SC_IPSEC), 'ipsec.cmd?action=view'));
   }
   if (certificate == '1')  {
      nodeCert = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_CERT), 'certlocal.cmd?action=view'));
      insDoc(nodeCert, gLnk('R', getMenuTitle(MENU_CERT_LOCAL), 'certlocal.cmd?action=view'));
      insDoc(nodeCert, gLnk('R', getMenuTitle(MENU_CERT_CA), 'certca.cmd?action=view'));
   }

   if ( ipv6Support == '1' )
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_IPV6),'ipv6setup.html'));

   // Configure diagnostics menu
   nodeDiagnostics = insFld(foldersTree, gFld(getMenuTitle(MENU_DIAGNOSTICS), 'diag.html'));

   if (vdslIncluded == 'true') {
      insDoc(nodeDiagnostics, gLnk('R', getMenuTitle(MENU_DIAGNET),'diag.html'));
      insDoc(nodeDiagnostics, gLnk('R', getMenuTitle(MENU_DIAG8021AG),'diag8021ag.html'));
   }

   // Configure management menu
   nodeMngr = insFld(foldersTree, gFld(getMenuTitle(MENU_MANAGEMENT), 'backupsettings.html'));

   nodeSettings = insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SETTINGS), 'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_BACKUP),'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_UPDATE),'updatesettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_DEFAULT), 'defaultsettings.html'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SYSTEM_LOG), 'logintro.html'));
   if ( snmp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SNMP), 'snmpconfig.html'));
   if ( tr69c == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TR69C), 'tr69cfg.html'));
   if ( sntp == '1' && proto != 'Bridge' && !(proto=='PPPoE' && ipExt=='1') && !(proto=='PPPoA' && ipExt=='1') )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_SNTP), 'sntpcfg.html'));

   nodeAccCntr = insFld(nodeMngr, gFld(getMenuTitle(MENU_ACC_CNTR), 'scsrvcntr.cmd?action=view'));
   insDoc(nodeAccCntr, gLnk('R', getMenuTitle(MENU_ACC_CNTR_SERVICE), 'scsrvcntr.cmd?action=view'));
   insDoc(nodeAccCntr, gLnk('R', getMenuTitle(MENU_ACC_CNTR_IP_ADDR), 'scacccntr.cmd?action=view'));
   insDoc(nodeAccCntr, gLnk('R', getMenuTitle(MENU_ACC_CNTR_PASSWORD), 'password.html'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_UPDATE_SOFTWARE), 'upload.html'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_RESET_ROUTER), 'resetrouter.html'));
   if ( traceping == '1')
      insFld(nodeMngr, gFld(getMenuTitle(MENU_DARE_TRACE_PING), 'pingtrace.html'));
}

function menuSupport(options) {
   var std = options[MENU_OPTION_STANDARD];
   var proto = options[MENU_OPTION_PROTOCOL];
   var ipExt = options[MENU_OPTION_IP_EXTENSION];
   var wireless = options[MENU_OPTION_WIRELESS];
   var voice = options[MENU_OPTION_VOICE];
   var snmp = options[MENU_OPTION_SNMP];
   var ddnsd = options[MENU_OPTION_DDNSD];
   var sntp = options[MENU_OPTION_SNTP];
   var QosEnabled = options[MENU_OPTION_QOS];
   var ipp = options[MENU_OPTION_IPP];
   var rip = options[MENU_OPTION_RIP];
   var tr69c = options[MENU_OPTION_TR69C];
   var ipv6Support = options[MENU_OPTION_IPV6_SUPPORT];
   var ipv6Enable = options[MENU_OPTION_IPV6_ENABLE];
   var numWl = options[MENU_OPTION_WIRELESS_NUM_ADAPTOR];

   // Configure quick setup wizard
   if ( proto == 'Not Applicable' )
      nodeQuickSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_QUICK_SETUP), 'vpivci.cgi'));

   // Configure advanced setup
   nodeAdvancedSetup = insFld(foldersTree, gFld(getMenuTitle(MENU_ADVANCED_SETUP), 'wancfg.cmd'));
   insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_WAN),'wancfg.cmd'));
   insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_LAN),'lancfg2.html'));

   // Configure QoS class menu
   nodeQos = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_QOS),'qosqmgmt.html'));
   insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_QUEUE), 'qosqueue.cmd?action=view'));
   insDoc(nodeQos, gLnk('R', getMenuTitle(MENU_QOS_CLASS), 'qoscls.cmd?action=view'));
   
   // Configure routing menu
   nodeRouting = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_ROUTING), 'rtdefaultcfg.html'));
   insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_DEFAULT_ROUTE), 'rtdefaultcfg.html'));
   insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE),'rtroutecfg.cmd?action=viewcfg'));

   if ( (proto == 'PPPoE' && ipExt == '0') ||
        (proto == 'PPPoA' && ipExt == '0') ||
        (proto == 'MER') ||
        (proto == 'IPoA') ) {
      // configure rip
      if ( rip == '1' )
         insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_RIP),'ripcfg.cmd?action=view'));
      // configure dns server
      nodeDnsSetup = insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DNS), 'dnscfg.html'));
      insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DNS_SETUP), 'dnscfg.html'));
      // configure ddns client
      if ( ddnsd == '1' )
         insDoc(nodeDnsSetup, gLnk('R', getMenuTitle(MENU_DDNS), 'ddnsmngr.cmd'));
   }

   // configure ipv6
   if ( ipv6Enable == '1' )	{
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_DEFAULT_ROUTE6), 'rtdefaultcfg6.html'));
      insDoc(nodeRouting, gLnk('R', getMenuTitle(MENU_RT_STATIC_ROUTE6),'rtroute6cfg.cmd?action=viewcfg'));
   }

   // Configure ADSL Setting Menu based on Annex
   if ( std == 'annex_c' )
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfgc.html'));
   else
      insFld(nodeAdvancedSetup, gFld(getMenuTitle(MENU_DSL), 'adslcfg.html'));

   // Configure print server
   if ( ipp == '1' )
      insDoc(nodeAdvancedSetup, gFld(getMenuTitle(MENU_IPP), 'ippcfg.html'));

   // Configure wireless menu
   if ( wireless == '1' ) {

  	if(numWl != '1')
           wlanMenu = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));
 

     for(i = 0; i < parseInt(numWl); i++)
     {

	   //nodeWireless = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), 'wlcfg.html'));
	 if(numWl == '1')
                nodeWireless = insFld(foldersTree, gFld(getMenuTitle(MENU_WIRELESS_SETTINGS), wlItemsCgiCmd[0]));
	     else
		  nodeWireless = insFld(wlanMenu, gFld(wlmenuTitle[i], wlItemsCgiCmd[i]));
		 
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_BASIC), 'wlcfg.html'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_SECURITY), 'wlsecurity.html'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_MAC_FILTERING), 'wlmacflt.cmd?action=view'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_WDS), 'wlwds.cmd?action=view'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_ADVANCED), 'wlcfgadv.html'));
      insDoc(nodeWireless, gLnk('R', getMenuTitle(MENU_WL_STATION_LIST), 'wlstationlist.cmd'));
     }
   }

   // Configure voice menu
   if ( voice == 'mgcp' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voicemgcpview.cmd'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_MGCP), 'voicemgcpview.cmd'));
      //insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_STATS), 'voicestats45.html'));
   }
   else if ( voice == 'sip' ) {
      nodeVoice = insFld(foldersTree, gFld(getMenuTitle(MENU_VOICE_SETTINGS), 'voicesipview.cmd'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_BASIC), 'voicesipview.cmd?view=basic'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_ADVANCED), 'voicesipview.cmd?view=advanced'));
      insDoc(nodeVoice, gLnk('R', getMenuTitle(MENU_VOICE_SIP_DEBUG), 'voicesipview.cmd?view=debug'));
   }

   if ( ipv6Support == '1' )
      insDoc(nodeAdvancedSetup, gLnk('R', getMenuTitle(MENU_IPV6),'ipv6setup.html'));

   // Configure diagnostics menu
   nodeDiagnostics = insFld(foldersTree, gFld(getMenuTitle(MENU_DIAGNOSTICS), 'diag.html'));

   // Configure management menu
   nodeMngr = insFld(foldersTree, gFld(getMenuTitle(MENU_MANAGEMENT), 'backupsettings.html'));
   nodeSettings = insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SETTINGS), 'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_BACKUP),'backupsettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_UPDATE),'updatesettings.html'));
   insDoc(nodeSettings, gLnk('R', getMenuTitle(MENU_TL_SETTINGS_DEFAULT), 'defaultsettings.html'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SYSTEM_LOG), 'logintro.html'));
   if ( snmp == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SNMP), 'snmpconfig.html'));
   if ( tr69c == '1' )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_TR69C), 'tr69cfg.html'));
   if ( sntp == '1' && proto != 'Bridge' && !(proto=='PPPoE' && ipExt=='1') && !(proto=='PPPoA' && ipExt=='1') )
      insFld(nodeMngr, gFld(getMenuTitle(MENU_SNTP), 'sntpcfg.html'));

   nodeAccCntr = insFld(nodeMngr, gFld(getMenuTitle(MENU_ACC_CNTR), 'scsrvcntr.cmd?action=view'));
   insDoc(nodeAccCntr, gLnk('R', getMenuTitle(MENU_ACC_CNTR_SERVICE), 'scsrvcntr.cmd?action=view'));
   insDoc(nodeAccCntr, gLnk('R', getMenuTitle(MENU_ACC_CNTR_IP_ADDR), 'scacccntr.cmd?action=view'));

   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_UPDATE_SOFTWARE), 'upload.html'));
   insFld(nodeMngr, gFld(getMenuTitle(MENU_RESET_ROUTER), 'resetrouter.html'));
}

function menuUser() {
   // Configure diagnostics menu
   nodeDiagnostics = insFld(foldersTree, gFld(getMenuTitle(MENU_DIAGNOSTICS), 'diag.html'));

   // Configure management menu
   nodeMngr = insFld(foldersTree, gFld(getMenuTitle(MENU_MANAGEMENT), 'logintro.html'));
   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SYSTEM_LOG), 'logintro.html'));
   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_SNMP), 'snmpconfig.html'));
   insFld(nodeMngr, gFld(getMenuTitle(MENU_TL_UPDATE_SOFTWARE), 'upload.html'));
}

function createBcmMenu(options) {
   var user = options[MENU_OPTION_USER];
   var proto = options[MENU_OPTION_PROTOCOL];
   var ipExt = options[MENU_OPTION_IP_EXTENSION];
   var dhcpen = options[MENU_OPTION_DHCPEN];
   var vdslIncluded = options[MENU_OPTION_VDSL];

   foldersTree = gFld('', 'info.html');
   // device info menu
   nodeDeviceInfo = insFld(foldersTree, gFld(getMenuTitle(MENU_DEVICE_INFO), 'info.html'));
   // device summary menu
   insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DEVICE_SUMMARY), 'info.html'));
   // device wan menu
   insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DEVICE_WAN), 'wancfg.cmd?action=view'));
   // device statistics menu
   nodeSts = insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_STATISTICS), 'statsifc.html'));
   insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_LAN), 'statsifc.html'));
   insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_WAN), 'statswan.cmd'));
   insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_ATM), 'statsatm.cmd'));
   insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_ADSL), 'statsadsl.html'));
   if (vdslIncluded == 'true')
   insDoc(nodeSts, gLnk('R', getMenuTitle(MENU_ST_VDSL), 'statsvdsl.html'));
   // device route menu
   insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DEVICE_ROUTE), 'rtroutecfg.cmd?action=view'));
   insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_RT_ARP),'arpview.cmd'));
   // dhcp info
   if (!(proto == 'Bridge' || ipExt == '1') && dhcpen == '1') {
      insFld(nodeDeviceInfo, gFld(getMenuTitle(MENU_DHCPINFO),'dhcpinfo.html'));
   }
   if ( user == 'admin' )
      menuAdmin(options);
   else if ( user == 'support' )
      menuSupport(options);
   else if ( user == 'user' )
      menuUser();
}

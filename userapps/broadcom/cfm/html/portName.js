// PLEASE NOTE THAT A SPACE BETWEEN TWO WORDS IS TAKEN AS TWO SEPERATE
// WORDS. PLEASE KEEP THIS FILE CONSISTENT.
var portName_L = new Array();
var portName_U = new Array();

// For each Linux interface name in the boardparms.c file, add
// a corresponding Linux interface name and a user-friendly name
// of its equivalent that must be displayed on the WEB UI. It is
// best if the new inteface names are added at the end.

// Wireless interfac Linux interface name
portName_wlPrefix = 'wl'; 
portName_L[0]    = 'wl0';
// Wireless interface user-friendly name
portName_U[0]     = 'Wireless';

// USB interface Linux interface name
portName_L[1]    = 'usb0';
// USB user-friendly name
portName_U[1]    = 'USB';

// Board ID 96338SV Linux name
portName_L[2]     = '96338SV|eth0';
// Board ID 96338SV user-friendly name
portName_U[2]     = '96338SV|ENET';

//Board ID 96338L-2M-8M Linux name
portName_L[3]     = '96338L-2M-8M|eth0';
//Board ID 96338L-2M-8M user-friendly name
portName_U[3]     = '96338L-2M-8M|ENET';

// Board ID 96348R Linux name
portName_L[4]    = '96348R|eth0';
// Board ID 96348R user-friendly name
portName_U[4]    = '96348R|ENET';

// Board ID 96348LV Linux name
portName_L[5]    = '96348LV|eth0';
portName_L[6]    = '96348LV|eth1';
// Board ID 96348LV user-friendly name
portName_U[5]    = '96348LV|ENET1';
portName_U[6]    = '96348LV|ENET2';

// Board ID 96348GW Linux name
portName_L[7]    = '96348GW|eth0';
portName_L[8]    = '96348GW|eth1';
portName_L[9]    = '96348GW|eth1.2';
portName_L[10]    = '96348GW|eth1.3';
portName_L[11]    = '96348GW|eth1.4';
// Board ID 96348GW user-friendly name
portName_U[7]    = '96348GW|ENET4';
portName_U[8]    = '96348GW|ENET(1-3)';
portName_U[9]    = '96348GW|ENET1';
portName_U[10]    = '96348GW|ENET2';
portName_U[11]    = '96348GW|ENET3';

// Board ID 96348GW-10 Linux name
portName_L[12]    = '96348GW-10|eth0';
portName_L[13]    = '96348GW-10|eth1';
portName_L[14]    = '96348GW-10|eth1.2';
portName_L[15]    = '96348GW-10|eth1.3';
portName_L[16]    = '96348GW-10|eth1.4';
// Board ID 96348GW-10 user-friendly name
portName_U[12]    = '96348GW-10|ENET4';
portName_U[13]    = '96348GW-10|ENET(1-3)';
portName_U[14]    = '96348GW-10|ENET1';
portName_U[15]    = '96348GW-10|ENET2';
portName_U[16]    = '96348GW-10|ENET3';

// Board ID 96348GW-11 Linux name
portName_L[17]    = '96348GW-11|eth0';
portName_L[18]    = '96348GW-11|eth0.2';
portName_L[19]    = '96348GW-11|eth0.3';
portName_L[20]    = '96348GW-11|eth0.4';
portName_L[21]    = '96348GW-11|eth0.5';
// Board ID 96348GW-11 user-friendly name
portName_U[17]    = '96348GW-11|ENET(1-4)';
portName_U[18]    = '96348GW-11|ENET4';
portName_U[19]    = '96348GW-11|ENET3';
portName_U[20]    = '96348GW-11|ENET2';
portName_U[21]    = '96348GW-11|ENET1';

// Board ID 96348SV Linux name
portName_L[22]    = '96348SV|eth0';
portName_L[23]    = '96348SV|eth1';
// Board ID 96348SV user-friendly name
portName_U[22]    = '96348SV|ENET1';
portName_U[23]    = '96348SV|ENET2';

// Board ID 96358GW Linux name
portName_L[24]    = '96358GW|eth0';
portName_L[25]    = '96358GW|eth1';
portName_L[26]    = '96358GW|eth1.2';
portName_L[27]    = '96358GW|eth1.3';
portName_L[28]    = '96358GW|eth1.4';
// Board ID 96358GW user-friendly name
portName_U[24]    = '96358GW|ENET4';
portName_U[25]    = '96358GW|ENET(1-3)';
portName_U[26]    = '96358GW|ENET1';
portName_U[27]    = '96358GW|ENET2';
portName_U[28]    = '96358GW|ENET3';

// Board ID 96358VW Linux name
portName_L[29]    = '96358VW|eth0';
portName_L[30]    = '96358VW|eth1';
portName_L[31]    = '96358VW|eth1.2';
portName_L[32]    = '96358VW|eth1.3';
portName_L[33]    = '96358VW|eth1.4';
// Board ID 96358VW user-friendly name
portName_U[29]    = '96358VW|ENET4';
portName_U[30]    = '96358VW|ENET(1-3)';
portName_U[31]    = '96358VW|ENET1';
portName_U[32]    = '96358VW|ENET2';
portName_U[33]    = '96358VW|ENET3';

// Board ID 96348GW3 Linux name
portName_L[34]    = '96348W3|eth0';
portName_L[35]    = '96348W3|eth1';
portName_L[36]    = '96348W3|eth1.2';
portName_L[37]    = '96348W3|eth1.3';
portName_L[38]    = '96348W3|eth1.4';
// Board ID 96348GW3 user-friendly name
portName_U[34]    = '96348W3|ENET4';
portName_U[35]    = '96348W3|ENET(1-3)';
portName_U[36]    = '96348W3|ENET1';
portName_U[37]    = '96348W3|ENET2';
portName_U[38]    = '96348W3|ENET3';

// Board ID 96348W3-10 Linux name
portName_L[39]    = '96348W3-10|eth0';
portName_L[40]    = '96348W3-10|eth1';
portName_L[41]    = '96348W3-10|eth1.2';
portName_L[42]    = '96348W3-10|eth1.3';
portName_L[43]    = '96348W3-10|eth1.4';
// Board ID 6348W3-10 user-friendly name
portName_U[39]    = '96348W3-10|ENET4';
portName_U[40]    = '96348W3-10|ENET(1-3)';
portName_U[41]    = '96348W3-10|ENET1';
portName_U[42]    = '96348W3-10|ENET2';
portName_U[43]    = '96348W3-10|ENET3';

// Board ID 96358VW2 Linux name
portName_L[44]    = '96358VW2|eth0';
portName_L[45]    = '96358VW2|eth1';
portName_L[46]    = '96358VW2|eth1.2';
portName_L[47]    = '96358VW2|eth1.3';
portName_L[48]    = '96358VW2|eth1.4';
// Board ID 96358VW2 user-friendly name
portName_U[44]    = '96358VW2|ENET4';
portName_U[45]    = '96358VW2|ENET(1-3)';
portName_U[46]    = '96358VW2|ENET1';
portName_U[47]    = '96358VW2|ENET2';
portName_U[48]    = '96358VW2|ENET3';


// Board ID 96338GW2 Linux name
portName_L[49]    = '96338W2_E7T|eth0';
portName_L[50]    = '96338W2_E7T|eth0.2';
portName_L[51]    = '96338W2_E7T|eth0.3';
portName_L[52]    = '96338W2_E7T|eth0.4';
portName_L[53]    = '96338W2_E7T|eth0.5';
// Board ID 96338GW2 user-friendly name
portName_U[49]    = '96338W2_E7T|eth0';
portName_U[50]    = '96338W2_E7T|LAN1';
portName_U[51]    = '96338W2_E7T|LAN2';
portName_U[52]    = '96338W2_E7T|LAN3';
portName_U[53]    = '96338W2_E7T|LAN4';

// Board ID 96338E_E7R Linux name
portName_L[54]    = '96338E_E7R|eth0';
portName_L[55]    = '96338E_E7R|eth0.2';
portName_L[56]    = '96338E_E7R|eth0.3';
portName_L[57]    = '96338E_E7R|eth0.4';
portName_L[58]    = '96338E_E7R|eth0.5';
// Board ID 96338E_E7R user-friendly name
portName_U[54]    = '96338E_E7R|eth0';
portName_U[55]    = '96338E_E7R|LAN1';
portName_U[56]    = '96338E_E7R|LAN2';
portName_U[57]    = '96338E_E7R|LAN3';
portName_U[58]    = '96338E_E7R|LAN4';

// Board ID 96338E Linux name
portName_L[59]    = '96338E|eth0';
portName_L[60]    = '96338E|eth0.2';
portName_L[61]    = '96338E|eth0.3';
portName_L[62]    = '96338E|eth0.4';
portName_L[63]    = '96338E|eth0.5';
// Board ID 96338E user-friendly name
portName_U[59]    = '96338E|eth0';
portName_U[60]    = '96338E|LAN1';
portName_U[61]    = '96338E|LAN2';
portName_U[62]    = '96338E|LAN3';
portName_U[63]    = '96338E|LAN4';

function getUNameByLName(name) {
   var index = 0;
   var uName   = '';

   // SafetyNet if someone sends a name without prefixing the
   // board ID and |, then return that name.
   if (name.indexOf('|') == -1)
      return name;
   if (name.indexOf('wl0.3') != -1) {
      return 'Wireless_Guest2(SSID4)';
   }
   else if (name.indexOf('wl0.2') != -1) {
      return 'Wireless_Guest1(SSID3)';
   }
   else if (name.indexOf('wl0.1') != -1) {
      return 'Wireless_Guest(SSID2)';
   }
   else if (name.indexOf('wl0') != -1) {
      return 'Wireless(SSID1)';
   }
    else if (name.indexOf('wl1.3') != -1) {
      return 'Wireless1_Guest2(SSID4)';
   }
    else if (name.indexOf('wl1.2') != -1) {
      return 'Wireless1_Guest1(SSID3)';
   }
    else if (name.indexOf('wl1.1') != -1) {
      return 'Wireless1_Guest(SSID2)';
   }
    else if (name.indexOf('wl1') != -1) {
      return 'Wireless1(SSID1)';
   }
   if (name.indexOf('usb0') != -1) {
      return 'USB';
   }
   for (index = 0; index < portName_L.length; index++) {
      if (portName_L[index] == name) {
         uName = portName_U[index].split('|');
         return uName[1];
      }
   }
   uName = name.split('|');
   return uName[1];
}

function getLNameByUName(name) {
   var index = 0;
   var brdIntf = name.split('|');
   var lName   = '';
   var uName   = '';

   // SafetyNet if someone sends a name without prefixing the
   // board ID and |, then return that name.
   if (name.indexOf('|') == -1)
      return name;
   if (name.indexOf('Wireless_Guest2(SSID4)') != -1)
      return 'wl0.3';
   if (name.indexOf('Wireless_Guest1(SSID3)') != -1)
      return 'wl0.2';
   if (name.indexOf('Wireless_Guest(SSID2)') != -1)
      return 'wl0.1';
   if (name.indexOf('Wireless(SSID1)') != -1)
      return 'wl0';
   if (name.indexOf('Wireless1_Guest2(SSID4)') != -1)
      return 'wl1.3';
   if (name.indexOf('Wireless1_Guest1(SSID3)') != -1)
      return 'wl1.2';
   if (name.indexOf('Wireless1_Guest(SSID2)') != -1)
      return 'wl1.1';
   if (name.indexOf('Wireless1(SSID1)') != -1)
      return 'wl1';
   if (name.indexOf('USB') != -1)
      return 'usb0';
   for (index = 0; index < portName_U.length; index++) {
      uName = portName_U[index].split('|');
      if (portName_U[index] == name) {
         lName = portName_L[index].split('|');
         return lName[1];
      }
   }
   lName = name.split('|');
   return lName[1];
}

function portNameDontCareSts(name) {
	
   // return true if don't care 	
   if (name.indexOf(portName_wlPrefix) != -1) {	
	return true;
   }
   
   return false;
}

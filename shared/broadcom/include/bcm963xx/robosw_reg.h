/*
<:copyright-broadcom 
 
 Copyright (c) 2007 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
*/

#ifndef __ROBOSW_REG_H
#define __ROBOSW_REG_H

void robosw_init_quad_phy(void);
void robosw_init_all(void);

// These macros make offset validation vs. data sheet easier.

#define group(type, name, start_offset, next) type name[(next - start_offset) / sizeof(type)]
#define entry(type, name, start_offset, next) type name

typedef struct RoboSwitch {
  group(byte,     PortCtrl,     0x0000, 0x0009);
  group(byte,     Reserved0009, 0x0009, 0x000b);
  entry(byte,     SwitchMode,   0x000b, 0x000c);
  entry(uint16,   PauseQuanta,  0x000c, 0x000e);
  entry(byte,     ImpOverride,  0x000e, 0x000f);
  entry(byte,     LedRefresh,   0x000f, 0x0010);
  entry(uint16,   LedFunc0,     0x0010, 0x0012);
  entry(uint16,   LedFunc1,     0x0012, 0x0014);
  entry(uint16,   LedFuncMap,   0x0014, 0x0016);
  entry(uint16,   LedEnableMap, 0x0016, 0x0018);
  entry(uint16,   LedMap0,      0x0018, 0x001a);
  entry(uint16,   LedMap1,      0x001a, 0x001c);
  group(byte,     Reserved001c, 0x001c, 0x0020);
  group(byte,     Reserved0020, 0x0020, 0x0021);
  entry(byte,     ForwardCtrl,  0x0021, 0x0022);
  group(byte,     Reserved0022, 0x0022, 0x0024);
  entry(uint16,   ProtSelect,   0x0024, 0x0026);
  entry(uint16,   WanSelect,    0x0026, 0x0028);
  entry(uint32,   PauseCap,     0x0028, 0x002c);
  group(byte,     Reserved002c, 0x002c, 0x002f);
  entry(byte,     MultiCtrl,    0x002f, 0x0030);
  group(byte,     Reserved0030, 0x0030, 0x0031);
  entry(byte,     TxqFlush,     0x0031, 0x0032);
  entry(uint16,   UniFail,      0x0032, 0x0034);
  entry(uint16,   MultiFail,    0x0034, 0x0036);
  entry(uint16,   MlfIpmc,      0x0036, 0x0038);
  entry(uint16,   PausePassRx,  0x0038, 0x003a);
  entry(uint16,   PausePassTx,  0x003a, 0x003c);
  entry(uint16,   DisableLearn, 0x003c, 0x003e);
  group(byte,     Reserved003e, 0x003e, 0x004a);
  entry(uint16,   PllTest,      0x004a, 0x004c);
  group(byte,     Reserved004c, 0x004c, 0x0058);
  group(byte,     PortOverride, 0x0058, 0x0060);
  entry(byte,     ImpRgmiiCtrl, 0x0060, 0x0061);
  group(byte,     Reserved0061, 0x0061, 0x0079);
  entry(byte,     SWResetCtrl,  0x0079, 0x007a);
  group(byte,     Reserved007a, 0x007a, 0x00a0);
  entry(uint32,   SwpktCtrl0,   0x00a0, 0x00a4);
  entry(uint32,   SwpktCtrl1,   0x00a4, 0x00a8);
  group(byte,     Reserved00a8, 0x00a8, 0x0200);
  entry(byte,     GlbMgmt,      0x0200, 0x0201);
  entry(byte,     ChpBoxID,     0x0201, 0x0202);
  entry(byte,     MngPID,       0x0202, 0x0203);

} RoboSwitch;

#define SWITCH ((volatile RoboSwitch * const)SWITCH_BASE)

#define PortCtrl_Forwarding   0xa0
#define PortCtrl_Learning     0x80
#define PortCtrl_Listening    0x60
#define PortCtrl_Blocking     0x40
#define PortCtrl_Disable      0x20
#define PortCtrl_RxUcstEn     0x10
#define PortCtrl_RxMcstEn     0x08
#define PortCtrl_RxBcstEn     0x04
#define PortCtrl_DisableTx    0x02
#define PortCtrl_DisableRx    0x01

#define SwitchMode_FwdgEn     0x02
#define SwitchMode_ManageMode 0x01

#define ImpOverride_Force     0x80
#define ImpOverride_TxFlow    0x20
#define ImpOverrode_RxFlow    0x10
#define ImpOverride_1000Mbs   0x08
#define ImpOverride_100Mbs    0x04
#define ImpOverride_10Mbs     0x00
#define ImpOverride_Fdx       0x02
#define ImpOverride_Linkup    0x01

#define PortOverride_Enable   0x40
#define PortOverride_TxFlow   0x20
#define PortOverride_RxFlow   0x10
#define PortOverride_1000Mbs  0x08
#define PortOverride_100Mbs   0x04
#define PortOverride_10Mbs    0x00
#define PortOverride_Fdx      0x02
#define PortOverride_Linkup   0x01

#define GlbMgmt_EnableImp     0x80
#define GlbMgmt_IgmpSnooping  0x08
#define GlbMgmt_ReceiveBpdu   0x02
#define GlbMgmt_ResetMib      0x01

#endif

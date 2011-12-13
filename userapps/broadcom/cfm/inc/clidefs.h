/*
<:copyright-gpl
 Copyright 2004 Broadcom Corp. All Rights Reserved.

 This program is free software; you can distribute it and/or modify it
 under the terms of the GNU General Public License (Version 2) as
 published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
:>
*/

/*****************************************************************************
//
******************************************************************************
//
//  Filename:       clidefs.h
//  Author:         Peter T. Tran
//  Creation Date:  12/26/01
//
******************************************************************************
//  Description:
//      Define the global types, enums. and constants for CLI.
//
*****************************************************************************/

#ifndef __CLIDEFS_H__
#define __CLIDEFS_H__

#include "bcmtypes.h"

/********************** Global Constants ************************************/

#define CLI_MAX_BUF_SZ       128
#define CLI_BACKSPACE        '\x08'

/********************** Global Enums ****************************************/

typedef enum {
   CLI_FALSE = 0,
   CLI_TRUE
} CLI_BOOL;

typedef enum {
   CLI_ACCESS_DISABLED = 0,
   CLI_ACCESS_LOCAL,
   CLI_ACCESS_REMOTE,
   CLI_ACCESS_CONSOLE,
   CLI_ACCESS_REMOTE_SSH,     // for SSHD
} CLI_ACCESS_WHERE;


typedef enum {
   CLI_STS_OK = 0,
   CLI_STS_ERR_GENERAL,
   CLI_STS_ERR_MEMORY,
   CLI_STS_ERR_FIND,
   CLI_STS_ERR_MENU_EXIST,
   CLI_STS_ERR_CMD_EXIST
} CLI_STATUS;

#ifdef BUILD_MENU_DRIVEN_CLI
typedef enum {
   CLI_PROCESS_PROMPT = 0,
   CLI_PROCESS_HELP,
   CLI_PROCESS_LINE
} CLI_PROCESS;

/********************** Global Types ****************************************/

typedef CLI_BOOL (*CLI_VALIDATE_FNC) (char* inputParam);

typedef struct {
   char* prompt;
   char  param[CLI_MAX_BUF_SZ];
   CLI_VALIDATE_FNC validateFnc;
} PARAMS_SETTING, *PPARAMS_SETTING;

typedef CLI_STATUS (*CLI_FNC) (char* cmdName, int help, void* object);

typedef struct {
   char* itemName;
   char* itemMenu;
   char* cmdName;
   char* cmdHelp;
   void* cmdObject;
   CLI_FNC cliFnc;
} CLI_MENU_ITEM, *PCLI_MENU_ITEM;

typedef struct {
   char*          menuName;
   int            itemNumMax;
   PCLI_MENU_ITEM items;
} CLI_MENU, *PCLI_MENU;
#endif /* BUILD_MENU_DRIVEN_CLI */

#endif

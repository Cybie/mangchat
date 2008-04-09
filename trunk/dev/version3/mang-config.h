/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#ifndef _MANG_CONFIG_H
#define _MANG_CONFIG_H

#define MANG_VERSION 3-0-0-0

#define IRC_CMD_PING		"PING"
#define IRC_CMD_JOIN		"JOIN"
#define IRC_CMD_PRIVMSG		"PRIVMSG"

#define IRC_CMD_NICK		"NICK"
#define IRC_CMD_ACTION		"ACTION"

#define IRC_CMD_ICC			"001"
#define IRC_CMD_ERR_1		"401"


#define IRC_USR_LOGIN		"login"

//#define USE_UTF8 // Uncomment this line to enable UTF-8 conversion
#ifdef USE_UTF8
	#include "iconv.h"
#endif

#define MC_IRC_SERVER "irc.evenet.org"
#define MC_IRC_CHANNEL "#evenet"

enum MANG_CACTION
{
    CHANNEL_JOIN,
    CHANNEL_LEAVE,
};

enum MANG_MSG_TYPE
{
    MSG_TYPE_PRIV = 0,
    MSG_TYPE_NOTICE = 1,
    MSG_TYPE_ACTION = 2,
};

#endif

//DEV
#include <iostream>
#include <stdio.h>
using namespace std;
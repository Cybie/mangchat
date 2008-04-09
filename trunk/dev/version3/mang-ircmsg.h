/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#ifndef _MANG_IRCMSG_H
#define _MANG_IRCMSG_H

#include "mang-irc.h"




class MangIrcMsg
{

	public:
		MangIrcMsg();				/* Class Denstructor */
		MangIrcMsg(MangIrc *_irc);	/* Class Denstructor */
		~MangIrcMsg();				/* Class Denstructor */
	public:
		void Process(char *data);	/* Class Denstructor */
	
	private:
		void Decode(char *data);	/* Class Denstructor */

		void Command(MANG_MSG_DATA *__MMD, char *data); /* Class Denstructor */



	protected:
		MangIrc *irc;				/* Class Denstructor */
		MANG_MSG_DATA *_MMD;		/* Class Denstructor */

};

#endif

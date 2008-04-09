/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#ifndef _MANG_IRC_H
#define _MANG_IRC_H

#include "mang-config.h"
#include "mang-socket.h"

struct MANG_MSG_DATA
/* Struct : IRC Message Data */
{
	MANG_MSG_DATA()
	{
		sender = NULL;
		nick = (char*)("");
		command = (char*)("");
		message = (char*)("");
		dest = NULL;
		host = NULL;
	};
	char *sender;					// Contains the senders name and host;
	char *nick;
	char *command;	// Contains the command receieved from the other party.
	char *message;	// Contains the remainder of the data we receieved.
	char *dest;

	char *host;
};


class MangIrc : public MangSock
{

	public:
		MangIrc();
		~MangIrc();
	
	public:
		void SplitIn2(char *data, char *part1, char *part2);

	public:
		void Init();
		void Login(const char *_user, const char *_pass, const char *_nick);

		void SendTo(const char *channel, int mtype, const char *data, ...);
		void SendData(const char *data, ...);
		
		std::string MakeString(const char *data, ...);

	public:
		void cmdPING(char *MMD);			/* Class Denstructor */
		void cmdFalse(MANG_MSG_DATA *MMD);			/* Class Denstructor */
		void cmdJoin(MANG_MSG_DATA *MMD);				/* Class Denstructor */
		void cmdPart(MANG_MSG_DATA *MMD);				/* Class Denstructor */
		void cmdPrivmsg(MANG_MSG_DATA *MMD);			/* Class Denstructor */

		void cmdNICK(MANG_MSG_DATA *MMD);				/* Class Denstructor */

		void cmdDefault(MANG_MSG_DATA *MMD);				/* Class Denstructor */

		void onConnect();

	private:
		char *user;
		char *pass;
		char *nick;

};

#endif

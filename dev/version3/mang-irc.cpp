/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#include "mang-irc.h"
#include "mang-ircmsg.h"

MangIrc::MangIrc()
/* Class Constructor */
{
	user = "mangchat3";
	pass = "mangchat3";
	nick = "mangchat3";
}


MangIrc::~MangIrc()
/* Class Destructor */
{
}

void MangIrc::Init()
/* Initialize the IRC client, connect and receieve data. */
{
	// Initialize the sockets.
	if(this->Initialize())
	{
		printf("Initliazed\n");

		printf("Attempting connection\n");

		// Connect to the irc-server
		if(!this->Connect(MC_IRC_SERVER, 6667))
		{
			printf("Error connecting\n");
			return;
		}
		
		printf("Connection success\n");

		// We are connected send a message to let the server know we are there.
		this->Transmit("HELLO\n");

		// Login to the server.
		this->Login(user, pass, nick);
		
		// Buffer for receieved data.
		char recvdata[MAXDATASIZE];

		// We keep receievng data from the socket.
		while(this->Receieve(recvdata))
		{
			// Create an instance from the MangIrcMsg class
			MangIrcMsg MIM(this);
			// and process the data.
			MIM.Process(recvdata);	
		}
		// Disconnected.
		this->Close();
	}
	printf("Disconnected.\n");
}

void MangIrc::Login(const char *_user, const char *_pass, const char *_nick)
/* IRC Client login, pass, nick, user */
{
	// Send the password fist, appearantly adviced todo first. true?
    this->SendData("PASS %s", _pass);

	// Send nickname
	this->SendData("NICK %s", _nick);

	// Inlcude the hostname in the user command
    char hostname[128];
    gethostname(hostname, sizeof(hostname));
	// USER <username> <hostname> <servername> <realname> (RFC1459)

	this->SendData("USER %s %s %s :MangChat 3.00 by Cybrax", _user, hostname, _nick);
}

void MangIrc::SendTo(const char *channel, int mtype, const char *data, ...)
{
    va_list ap;
    char ircMsg[MAXDATASIZE - 32];
    va_start(ap, data);
    vsnprintf(ircMsg, MAXDATASIZE - 32, data, ap );
    va_end(ap);

	char sndMsg[MAXDATASIZE];

	switch(mtype)
	{
		case MSG_TYPE_NOTICE:
			strcpy(sndMsg, "NOTICE");
			break;
		default:
			strcpy(sndMsg, "PRIVMSG");
	}

	strcat(sndMsg, " ");
	strcat(sndMsg, channel);
	strcat(sndMsg, " :");

	if(mtype == MSG_TYPE_ACTION)
		strcat(sndMsg, "\001ACTION ");
	strcat(sndMsg, ircMsg);

	if(mtype == MSG_TYPE_ACTION)
		strcat(sndMsg, "\001");
	//printf(sndMsg);

	this->SendData(sndMsg);
	
}

void MangIrc::SendData(const char *data, ...)
/* Send data over the socket to the IRC server */
{
    va_list ap;
    char ircMsg[MAXDATASIZE];
    va_start(ap, data);
    vsnprintf(ircMsg, MAXDATASIZE, data, ap );
    va_end(ap);

	// Transmit the raw data.
	this->Transmit(ircMsg);
	// RFC protocol demands \r\n at the end of each command.
	this->Transmit("\n");
}

std::string MangIrc::MakeString(const char *data, ...)
/* Desc */
{
    va_list ap;
    char strMsg[1024];
    va_start(ap, data);
    vsnprintf(strMsg, 1024, data, ap );
    va_end(ap);

	std::string retval = strMsg;
	return retval;
}


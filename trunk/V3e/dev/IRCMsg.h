#ifndef _MC_IRC_MSG_H
#define _MC_IRC_MSG_H

#include "ClientSocket.h"
#include "IRCClient.h"
//class IRCClient;

enum MsgType
{
	MSG_PRIV,
	MSG_NOTICE,
	MSG_JOIN,
	MSG_KICK,
	MSG_QUIT
};

class IRCMsg
{
	public:
		IRCMsg(void){ };
		IRCMsg(IRCClient *_irc);
		~IRCMsg(void);

	public:
		ClientSocket *GetSession() { return irc->GetSocket(); };
		IRCClient *GetClient() { return irc; };

	public:
		void Send(char *data)
		{
			GetSession()->Send(data);
		}
		void SendW(char *data, ...);
		void Send_Channel(char *msg, char *chn = NULL, char *mtype = "PRIVMSG");

	public:
		char *data;
		char *ident;
		char *nick;
		char *user;
		char *host;
		char *cmd;
		char *channel;
		char *chat;
		char *extra1;
		char *extra2;

		char *pch;

		bool pm;

	private:
		IRCClient *irc;
};

#endif

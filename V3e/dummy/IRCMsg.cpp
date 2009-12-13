#include "IRCMsg.h"
#include "IRCFunc.h"

IRCMsg::IRCMsg(IRCClient *_irc)
{
	irc = _irc;
	chat = NULL;
	data = NULL;
	nick = NULL;
	user = NULL;
	host = NULL;
	cmd = NULL;
	channel = NULL;
	extra1 = NULL;
	extra2 = NULL;
	ident = NULL;
}

IRCMsg::~IRCMsg(void)
{
	if(data) delete data;
	//if(nick)	delete nick;
	//if(user)	delete user;
	//if(host)	delete host;
	//if(cmd)		delete cmd;
	//if(channel) delete channel;
	if(chat) delete chat;
	//if(extra1)	delete extra1;
	//if(extra2)	delete extra2;
	if(ident) delete ident;
}

void IRCMsg::SendW(char *text, ...)
{
	char data[1024];
	va_list ap;
    va_start(ap, text);
    vsnprintf(data, 1024, text, ap );
    va_end(ap);
	Send(data);
}

void IRCMsg::Send_Channel(char *msg, char *chn, char *mtype)
{
	char *chan = chn;
	if(chan == NULL)
	{
		if(channel[0] == '#')
			chan = channel;
		else
			chan = nick;
	}

	char data[1024];
	MakeMsgW(data, "%s %s :%s", mtype, chan, msg);
	printf(data);
	GetSession()->Send(data);
}


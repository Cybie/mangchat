#include "IRCMsg.h"
#include "IRCFunc.h"

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
	GetSession()->Send(data);
}


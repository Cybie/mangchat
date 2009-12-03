#include "IRC.h"

#include <Policies/SingletonImp.h>

#include "IRCMsg.h"

INSTANTIATE_SINGLETON_1( IRC );

void IRC::run() 
// Thread Entry 
{
    ACE_Based::Thread::Sleep(500);
    sLog.outString("\n%s\n%s\n%s\n%s",
        "***************************************",
        "**   MangChat Threaded IRC Client    **",
        "**     With Enhanced GM Control.     **",
        "***************************************");
    sLog.outString("****** MangChat: %s ********", MANGCHAT_VERSION);

	this->client.run();
}

void IRC::DoMessage()
/* Message WOW->IRC */
{
	IRCMsg m;
	m.CMD = "PRIVMSG";
	m.USR = "#mc";
	m.MSG = "Message from wow";
	m.Send(true);
}

void IRC::DoAction(ACTION_TYPE tAction)
/* Action WOW->IRC */
{
	switch(tAction)
	{
		case A_CH_JOIN:
			break;
		case A_CH_PART:
			break;
		case A_PLR_LEVEL:
			break;
		default:
			DoMessage();
	}
}


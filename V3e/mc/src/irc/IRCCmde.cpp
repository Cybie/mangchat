#include "IRCCmd.h"

void IRCCmd::ICC()
{
	msg->GetSock()->Send("JOIN #mc");
}

void IRCCmd::Join()
{
	if(msg->USR == "MANGCHAT")
		msg->GetSock()->Send("PRIVMSG #mc :Whhhhhaaazzzzaaaa");
	else
		msg->GetSock()->Send("PRIVMSG #mc :hello " + msg->USR + " Welcome to the channel");
}

void IRCCmd::PrivMsg()
{
	size_t p3 = msg->DATA.find(":");
	msg->MSG = msg->DATA.substr(p3 + 1, msg->DATA.size() - p3 - 1);
	msg->FROM = msg->DATA.substr(0, p3 - 1);
	
	if(msg->FROM == "MANGCHAT")
	// PM
	{
		if(msg->MSG.find("\001VERSION\001") < msg->MSG.size())
		{
			IRCMsg m = *msg;
			m.Send("VERSION MangChat v3.0e 2009 Cybrax Cyberspace");
		}
		else
		{
			CommandValid(1);
		}
	}	
	else
	// CHANNEL
	{
		CommandValid();
	}
}

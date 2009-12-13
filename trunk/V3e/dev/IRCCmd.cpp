#include "IRCCmd.h"
#include "IRCCmde.h"
#include "IRCFunc.h"

IRCCmd::~IRCCmd(void)
{
	if(msg)
		delete msg;
}

void IRCCmd::OnDefault()
{
	//printf("**Unhandled\n");
}

void IRCCmd::OnConnect()
{
	msg->Send("JOIN #cymc");
}

void IRCCmd::OnJoin()
{
	if(!strcmp(msg->nick, "CYMC"))
	{
		msg->SendW("PRIVMSG %s :Whhhhaaazzzzzzaaaa", msg->chat);
	}
	else
	{
		msg->SendW("PRIVMSG %s :Hello %s", msg->chat, msg->nick);
	}
}

void IRCCmd::OnPart()
{
	msg->SendW("PRIVMSG %s :Goodbye: %s has left the building", msg->channel, msg->nick);
}

void IRCCmd::OnNick()
{
	msg->SendW("PRIVMSG #cymc :Nickchange detected %s to %s", msg->nick, msg->chat);
}

void IRCCmd::OnKick()
{
	if(!strcmp(msg->extra1, "CYMC"))
	{
		msg->SendW("JOIN %s", msg->channel);
	}
	else
	{
		msg->SendW("PRIVMSG %s : %s was kicked by %s, reason: %s", msg->channel, msg->extra1, msg->nick, msg->chat);
	}
}

void IRCCmd::OnPrivMsg()
{
	if(!strcmp(msg->channel, "CYMC"))
	{
		msg->pm = true;
		if(!strncmp(msg->chat, "\001VERSION", 8))
			msg->Send_Channel("\001VERSION MangChat v3.0 ©2009 Cybrax\001", NULL, "NOTICE");

		if(!strncmp(msg->chat, "\001DCC", 4))
			msg->Send_Channel("Hey u wanna DCC", NULL, "NOTICE");
	}
	else
		msg->pm = false;

	if(msg->chat[0] == '.')
	{
		IRCCmde cmd(msg);
		msg->chat += 1;

		char *command = Parse(msg->chat, " ", true);
		
		if(!strcmp(command, "help"))
		{
			cmd.DoHelp(command);
		}
		else if(!strcmp(command, "login"))
		{
			if(!msg->pm)
				msg->Send_Channel("this command requires a pm");
			else
			{
				char *user = Parse(command);
				char *pass = Parse(command);			
				cmd.DoLogin(user, pass);
			}
		}
	}
}

void IRCCmd::OnNotice()
{
}

void IRCCmd::OnQuit()
{
}

void IRCCmd::OnMode()
{
}
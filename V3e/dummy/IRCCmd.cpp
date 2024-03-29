#include "IRCCmd.h"
#include "IRCCmde.h"
#include "IRCFunc.h"

IRCCmd::~IRCCmd(void)
{
}

void IRCCmd::OnPrivMsg()
{
	if(!strcmp(msg->channel, "CYMC"))
	{
		if(!strncmp(msg->chat, "\001VERSION", 8))
		{
			msg->Send_Channel("\001VERSION MangChat v3.0 ©2009 Cybrax\001", NULL, "NOTICE");
			return;
		}

		if(!strncmp(msg->chat, "\001DCC", 4))
		{
			msg->Send_Channel("Hey u wanna DCC", NULL, "NOTICE");
			return;
		}
		msg->pm = true;
	}
	else
		msg->pm = false;

	if(msg->chat[0] == '.')
	{
		if(strlen(msg->chat) <= 1)
			return;

		IRCCmde cmd(msg);
		char *chat = msg->chat + 1;

		char *command = Parse(chat, " ", true);

		if(!strcmp(command, "about"))
		{
			msg->Send_Channel("MangChat v3.0 Extreme (C) 2009.");
			msg->Send_Channel("Created by Cybrax Cyberspace.");
			msg->Send_Channel("http://www.cybrax.com/ <info@cybrax.com>");
		}
		else if(!strcmp(command, "help"))
			cmd.DoHelp(command);
		else if(!strcmp(command, "logout"))
			msg->Send_Channel("You are now logged out.");
		else if(!strcmp(command, "login"))
		{
			if(!msg->pm)
				msg->Send_Channel("this command requires a pm");
			else
				cmd.DoLogin(command);
		}
		else
			msg->Send_Channel("Unknown Command");
	}
}

void IRCCmd::OnConnect()
{
	msg->Send("JOIN #cymc");
}

void IRCCmd::OnJoin()
{
	if(!strcmp(msg->nick, "CYMC"))
		msg->SendW("PRIVMSG %s :Whhhhaaazzzzzzaaaa", msg->chat);
	else
		msg->SendW("PRIVMSG %s :Hello %s", msg->chat, msg->nick);
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
		msg->SendW("JOIN %s", msg->channel);
	else
		msg->SendW("PRIVMSG %s : %s was kicked by %s, reason: %s", msg->channel, msg->extra1, msg->nick, msg->chat);
}

void IRCCmd::OnNotice()
{
}

void IRCCmd::OnQuit()
{
	msg->SendW("PRIVMSG %s :Goodbye: %s has left the building : %s", msg->channel, msg->nick, msg->chat);
}

void IRCCmd::OnMode()
{
}

void IRCCmd::OnDefault()
{
	//printf("**Unhandled\n");
}

void IRCCmd::OnNickTaken()
{
	
}
#include "IRCClient.h"
#include "IRCCmd.h"
#include "IRCFunc.h"

void IRCClient::run() 
{
	bActive = true;
    while(this->bActive)
    {
		Sock.Connect("127.0.0.1", 6667);
		Sock.Send("PASS CYMC");
		Sock.Send("NICK CYMC");
		Sock.Send("USER CYMC Cybrax VisualDreams :MangChat 3.01 (C) Cybrax");
		
		int nRecv = 1;
		while(nRecv && this->bActive)
		{
			char Buffer[BUFFER_SIZE + 1];
			memset(Buffer, 0, BUFFER_SIZE + 1);
			nRecv = Sock._Recv(Buffer, BUFFER_SIZE);
			//Buffer[nRecv] = '\0';		
			while(char *pch = strstr(Buffer, "\r\n"))
			{
				*pch = '\0';
				Handle_IRC(Buffer);
				strcpy(Buffer, pch + 2);
			}
		} 
		Sock.Close();
	} 
}

void IRCClient::Handle_IRC(char *line)
{
	if(!strncmp(line, "ERROR", 5))
		return;
	else if(!strncmp(line, "PING", 4))
	{
		char *pong = strstr (line, "PING");
		strncpy(pong, "PONG", 4);
		this->Sock.Send(pong);
	}
	else if(line[0] == ':')
	{
		line += 1;
		IRCMsg msg(this);

		//printf("%s\n", line);

		// Raw line for debug
		msg.data =  new char[strlen(line) + 1];
		strcpy(msg.data, line);
		
		// Get the chat text first, after first occurence of :
		// So we can split by " " without messing up chat
		if(char *chat = strchr(line, ':'))
		{
			chat += 1;
			msg.chat = new char[strlen(chat) + 1];
			strcpy(msg.chat, chat);
		}

		char *pch = NULL;
		char *ident = pch = Parse(line, " ", true);

		// Full Ident
		msg.ident = new char[strlen(ident)+1];
		strcpy(msg.ident, ident);	

		// Command
		msg.cmd = Parse(pch); 
		// Channel
		msg.channel = Parse(pch);
		// Extra1
		msg.extra1 = Parse(pch);
		// Extra2
		msg.extra2 = Parse(pch);
		// Nick
		msg.nick = Parse(ident, "!", true);
		// User/Host
		msg.user = Parse(ident, "@");
		if(msg.user == "")
		{
			msg.user = msg.nick;
			msg.host = msg.nick;
		}
		else
			msg.host = Parse(ident, "@");

		IRCCmd CMD(&msg);

		if(!strcmp(msg.cmd, "001"))
			CMD.OnConnect();
		else if(!strcmp(msg.cmd, "002"))
			CMD.OnRFC(/*Host info*/);
		else if(!strcmp(msg.cmd, "003"))
			CMD.OnRFC(/*Server info*/);
		else if(!strcmp(msg.cmd, "251"))
			CMD.OnRFC(/*Online Users*/);
		else if(!strcmp(msg.cmd, "252"))
			CMD.OnRFC(/*Online Operators*/);
		else if(!strcmp(msg.cmd, "252"))
			CMD.OnRFC(/*Online Channels*/);
		else if(!strcmp(msg.cmd, "255"))
			CMD.OnRFC(/*Clients/Servers*/);
		else if(!strcmp(msg.cmd, "265"))
			CMD.OnRFC(/*Local Users*/);
		else if(!strcmp(msg.cmd, "266"))
			CMD.OnRFC(/*Global Users*/);
		else if(!strcmp(msg.cmd, "332"))
			CMD.OnRFC(/*Channel Topic*/);
		else if(!strcmp(msg.cmd, "353"))
			CMD.OnRFC(/*Channel Users start*/);
		else if(!strcmp(msg.cmd, "366"))
			CMD.OnRFC(/*Channel Users end*/);
		else if(!strcmp(msg.cmd, "372"))
			CMD.OnRFC(/*MOTD Start*/);
		else if(!strcmp(msg.cmd, "367"))
			CMD.OnRFC(/*MOTD End*/);
		else if(!strcmp(msg.cmd, "433"))
			CMD.OnNickTaken(); 
		else if(!strcmp(msg.cmd, "439"))
			CMD.OnRFC(/*Processing Connection*/);
		else if(!strcmp(msg.cmd, "PRIVMSG"))
			CMD.OnPrivMsg();
		else if(!strcmp(msg.cmd, "NOTICE"))
			CMD.OnNotice();
		else if(!strcmp(msg.cmd, "JOIN"))
			CMD.OnJoin();
		else if(!strcmp(msg.cmd, "PART"))
			CMD.OnPart();
		else if(!strcmp(msg.cmd, "NICK"))
			CMD.OnNick();
		else if(!strcmp(msg.cmd, "KICK"))
			CMD.OnKick();
		else if(!strcmp(msg.cmd, "MODE"))
			CMD.OnMode();
		else if(!strcmp(msg.cmd, "QUIT"))
			CMD.OnQuit();
		else if(!strcmp(msg.cmd, "SQUIT"))
			CMD.OnQuit();
		else if(!strcmp(msg.cmd, "USER"))
			CMD.OnDefault();
		else if(!strcmp(msg.cmd, "TOPIC"))
			CMD.OnDefault();
		else if(!strcmp(msg.cmd, "NAMES"))
			CMD.OnDefault();
		else if(!strcmp(msg.cmd, "INVITE"))
			CMD.OnDefault();
		else if(!strcmp(msg.cmd, "VERSION"))
			CMD.OnDefault();
		else
			CMD.OnDefault();
	}
}

IClient *IRCClient::GetUser(char *username)
{
	for(std::list<IClient *>::iterator i=UserList.begin(); i!=UserList.end();i++)
	{
		if(!strcmp(username, (*i)->user))
			return (*i);
	}			
	return NULL;
}

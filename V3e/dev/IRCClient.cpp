#include "IRCClient.h"
#include "IRCCmd.h"
#include "IRCFunc.h"

void IRCClient::run() 
{
	this->bActive = true;
    while(this->bActive)
    {
		this->Sock.Connect("irc.freenode.org", 6667);
		this->Sock.Send("PASS MC3");
		this->Sock.Send("NICK MC3");
		this->Sock.Send("USER MC3 Cybrax VisualDreams :MangChat 3.01 (C) Cybrax");
		
		int nRecv = 1;

		char Buffer[BUFFER_SIZE + 1];
		while(nRecv && this->bActive)
		{
			memset(Buffer, 0, BUFFER_SIZE + 1);
			nRecv = this->Sock._Recv(Buffer, BUFFER_SIZE);
			Buffer[nRecv] = '\0';
			char *buffer = Buffer;
			while(char *pch = strstr(buffer, "\r\n"))
			{
				*pch = '\0';
				Handle_IRC(buffer);
				buffer = pch + 2;		
			}
		} 
		this->Sock.Close();
	} 
}

void IRCClient::Handle_IRC(char *line)
{
	if(!strncmp(line, "ERROR", 5))
		return;
	else if(!strncmp(line, "PING", 4))
	{
		char *pch = strstr (line, "PING");
		strncpy(pch, "PONG", 4);
		this->Sock.Send(pch);
	}
	else if(line[0] == ':')
	{
		line += 1;
		IRCMsg *msg = new IRCMsg(this);

		msg->data =  new char[strlen(line)];
		strcpy(msg->data, line);
		char *pch = NULL;
		msg->chat = strchr(line, ':');
		if(msg->chat) msg->chat += 1;
		char *ident = pch = Parse(line, " ", true);
		msg->cmd = Parse(pch);
		msg->channel = Parse(pch);
		msg->extra1 = Parse(pch);
		msg->extra2 = Parse(pch);
		msg->nick = Parse(ident, "!", true);
		msg->user = Parse(ident, "@");
		if(msg->user == "")
		{
			msg->user = msg->nick;
			msg->host = msg->nick;
		}
		else
			msg->host = Parse(ident, "@");

		IRCCmd CMD(msg);

		//printf("%s\n",msg->data);

		if(!strcmp(msg->cmd, "001"))
			CMD.OnConnect();
		else if(!strcmp(msg->cmd, "PRIVMSG"))
			CMD.OnPrivMsg();
		else if(!strcmp(msg->cmd, "NOTICE"))
			CMD.OnNotice();
		else if(!strcmp(msg->cmd, "JOIN"))
			CMD.OnJoin();
		else if(!strcmp(msg->cmd, "PART"))
			CMD.OnPart();
		else if(!strcmp(msg->cmd, "NICK"))
			CMD.OnNick();
		else if(!strcmp(msg->cmd, "KICK"))
			CMD.OnKick();
		else if(!strcmp(msg->cmd, "MODE"))
			CMD.OnMode();
		else if(!strcmp(msg->cmd, "QUIT"))
			CMD.OnQuit();
		else
			CMD.OnDefault();
	}
}


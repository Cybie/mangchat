#pragma once

#include "ClientSocket.h"

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
	IRCMsg(void);
	IRCMsg(ClientSocket *_sock);
	IRCMsg(const char *to, MsgType mt = MSG_PRIV);
	~IRCMsg(void);

public:
	std::string DATA;
	std::string USR;
	std::string HOST;
    std::string CMD;
	std::string MSG;
	std::string FROM;

public:
	void Append(const char *append)
	{
		IRC.append(append);
	}
	void Text(const char *text) { MSG = text; };
	const char *Get(bool bConstruct = false)
	{
		if(bConstruct)
		{
			IRC = CMD;
			IRC += " ";
			IRC += ChanOrPM();
			IRC += " :";
			IRC += MSG;
		}
		return IRC.c_str();
	}
	void Send(bool bConstruct = false)
	{
		if(bConstruct)Get(bConstruct);
		Sock->Send((char *)IRC.c_str());
	}
	void Send(const char *snd)
	{
		MSG = snd;
		Send(true);
	}
	ClientSocket *GetSock() { return this->Sock; };
private:
	const char *ChanOrPM();
	std::string IRC;

	ClientSocket *Sock;
};

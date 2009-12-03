#pragma once

#include "IRCMsg.h"
#include "ClientSocket.h"

class IRCCmd
{
public:
	IRCCmd(IRCMsg *_msg);
	IRCCmd(void){};
	~IRCCmd(void);

public:
	void ICC();
	void Join();
	void PrivMsg();

private:
	bool CommandValid(int nType = 0);
	char *Parse(char *src, bool bInit = false, char *bRest = NULL, char *delim = " ");

private:
	IRCMsg *msg;
};

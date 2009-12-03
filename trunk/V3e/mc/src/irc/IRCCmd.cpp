#include "IRCCmd.h"

IRCCmd::IRCCmd(IRCMsg *_msg)
{
	msg = _msg;
}

IRCCmd::~IRCCmd(void)
{
	if(msg)
		delete msg;
}

bool IRCCmd::CommandValid(int nType)
{
	if(msg->MSG[0] == '.')
	{
		msg->MSG.erase(0,1);
		char *command = Parse((char *)msg->MSG.c_str(), true);

		if(!strncmp(command, "login", 5))
		{
			if(nType == 0)
			{
				IRCMsg m = *msg;
				m.Send("You can only use this command in a PM");
				return false;
			}
			char *user = Parse(command);
			char *pass = Parse(command);

			if(user == "" || pass == "")
			{
				IRCMsg m = *msg;
				m.Send("Please specify a username or password");
				return false;
			}

			IRCMsg m = *msg;
			m.Send("Authentication Successful, Welcome to MangChat.");

			char rest[256] = "";
			Parse(command, NULL, rest);
			printf(rest);

			// DoLogin();
		}
	}
	return false;
}

char *IRCCmd::Parse(char *src, bool bInit, char *bRest, char *delim)
/* */
{
	if(bRest != NULL)
	{
		src = strtok (NULL, delim);
		while (src != NULL)
		{
			strcat(bRest, src);
			strcat(bRest, delim);
			src = strtok (NULL, delim);
		}
		return bRest;
	}
	if(bInit)
	{
		char *pch = strtok(src, delim);
		return pch;
	}
	else
	{
		src = strtok(NULL, delim);
		if(src == NULL)
			return "";
		else
			return src;
	}
}

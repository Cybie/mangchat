#include "IRCCmde.h"
#include "IRCFunc.h"
#include "IRCClient.h"

IRCCmde::~IRCCmde(void)
{
	//if(msg)
	//	delete msg;
}

bool IRCCmde::DoLogin(char *user, char *pass)
{
	if(user == "" || pass == "")
	{
		msg->Send_Channel("login <username> <password>");
		return false;
	}

	IClient *NewClient = new IClient();
	NewClient->user = new char[strlen(user)];
	strcpy(NewClient->user, user);
	NewClient->level = 3;
	msg->GetClient()->AddUser(NewClient);

	return true;
}

void IRCCmde::DoHelp(char *param)
{
	msg->Send_Channel("MangChat 3.0 (C) Cybrax Cyberspace");
	msg->Send_Channel(".help .login .logout .about");
}
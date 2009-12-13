#include "IRCCmde.h"
#include "IRCFunc.h"
#include "IRCClient.h"

IRCCmde::~IRCCmde(void)
{
	//if(msg)
	//	delete msg;
}

bool IRCCmde::DoLogin(char *param)
{
	char *user = Parse(param, " ");
	char *pass = Parse(param, " ");
	printf("-%s,%s-", user, pass);
	if(user == "" || pass == "")
	{
		msg->Send_Channel("login <username> <password>");
		return false;
	}

	IClient *NewClient = new IClient();
	NewClient->user = new char[strlen(user) + 1];
	strcpy(NewClient->user, user);
	NewClient->ident = new char[strlen(msg->ident) + 1];
	strcpy(NewClient->ident, msg->ident);
	NewClient->level = 3;
	msg->GetClient()->AddUser(NewClient);

	msg->SendW("PRIVMSG %s :Welcome %s you are bound to : %s", msg->nick, user, msg->ident);

	return true;
}

void IRCCmde::DoHelp(char *param)
{
	msg->Send_Channel("MangChat 3.0 (C) Cybrax Cyberspace");
	msg->Send_Channel(".help .login .logout .about");
}
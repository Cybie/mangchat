#include "IRC.h"

bool IRC::IsStopped = false;
bool IRC::IsActive = true;

DWORD WINAPI worker1(LPVOID param);
DWORD WINAPI worker2(LPVOID param);

void IRC::run() 
{
	CreateThread(NULL, 0, worker1, NULL, 0, NULL);
	CreateThread(NULL, 0, worker2, NULL, 0, NULL);
}

DWORD WINAPI worker1(LPVOID param)
{
	IRCClient irc;
	irc.run();

	return 0;
}

DWORD WINAPI worker2(LPVOID param)
{
	IRCIdent ircid;
	ircid.run();

	return 0;
}

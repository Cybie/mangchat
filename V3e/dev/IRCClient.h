#ifndef _MC_CLIENT_H
#define _MC_CLIENT_H

#include "IRCConf.h"
#include "ClientSocket.h"

struct IClient
{
	char *user;
	int level;
};

struct IChannel
{
	char *name_irc;
	char *name_wow;
	int mask;
	bool join;

};

class IRCClient
{
    public:
		IRCClient(){};
        ~IRCClient(){};
        void run();

	public:
		ClientSocket *GetSocket() { return &Sock; };
		void AddUser(IClient *newclient)
		{ UserList.push_back(newclient); };
		
		IClient *GetUser(char *username)
		{
			for(std::list<IClient *>::iterator i=UserList.begin(); i!=UserList.end();i++)
			{
				if(!strcmp(username, (*i)->user))
					return (*i);
			}			
			return NULL;
		};

    private:
        bool bActive;
        bool Init();

	private:
		void Handle_IRC(char *line);

    private:
		ClientSocket Sock;
		
		std::list<IClient *> UserList;
		std::list<IChannel *> ChannelList;
};

#endif
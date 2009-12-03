#ifndef _MC_IRC_H
#define _MC_IRC_H

#include "IRCClient.h"
#include <Policies/Singleton.h>

class IRC : public ACE_Based::Runnable
{
    public:
		IRC(){};
        ~IRC(){};
        void run();

	public:
		IRCClient *GetClient() { return &client; };

		void DoMessage();
		void DoAction(ACTION_TYPE tAction);

	private:
		IRCClient client;
};

#endif

#define sIRC MaNGOS::Singleton<IRC>::Instance()

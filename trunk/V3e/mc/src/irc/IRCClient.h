#ifndef _MC_CLIENT_H
#define _MC_CLIENT_H

#include "IRCConf.h"
#include "IRCLog.h"
#include "ClientSocket.h"

#include <Player.h>

//struct a {};
enum ACTION_TYPE
{
	A_CH_JOIN,
	A_CH_PART,
	A_PLR_LEVEL
};

class IRCClient
{
    public:
		IRCClient(){};
        ~IRCClient(){};
        void run();

	public:
		void Send_WoW_Player(std::string sPlayer, std::string sMsg);
		void Send_WoW_Player(Player *plr, std::string sMsg);

	private:
		void Handle_IRC(std::string& sData);
		void Send(std::string s);
		void Disconnect(){};

    private:
        bool bActive;
        bool bConn;
        bool Init();

    private:
		ClientSocket Sock;
		IRCLog iLog;
};

#endif


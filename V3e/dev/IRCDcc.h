#ifndef _MC_IRC_DCC_H
#define _MC_IRC_DCC_H

#include "ServerSocket.h"

class IRCDcc
{
    public:
		IRCDcc(){};
        ~IRCDcc(){};
        void run();
	
	private:
		ServerSocket *Sock;
};

#endif

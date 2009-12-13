#ifndef _MC_IRC_ID_H
#define _MC_IRC_ID_H

#include "ServerSocket.h"

class IRCIdent// : public ACE_BASED:Runable
{
    public:
		IRCIdent(){};
        ~IRCIdent(){};
        void run();
	
	private:
		ServerSocket *Sock;
		bool IsStopped;
};

#endif

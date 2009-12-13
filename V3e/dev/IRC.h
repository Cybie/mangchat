#ifndef _MC_IRC_H
#define _MC_IRC_H

#include "common.h"
#include "IRCClient.h"
#include "IRCIdent.h"

class IRC
{
    public:
		IRC(){};
        ~IRC(){};
        void run();

	public:
		static bool IsStopped;
		static bool IsActive;
};

#endif
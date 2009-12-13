#ifndef _MC_IRC_CMDE_H
#define _MC_IRC_CMDE_H

#include "IRCMsg.h"

class IRCCmde
{
	public:
		IRCCmde(void){};
		IRCCmde(IRCMsg *_msg){ msg = _msg;  };
		~IRCCmde(void);

	public:
		void DoHelp(char *param);
		bool DoLogin(char *param);
	
	private:
		IRCMsg *msg;
};

#endif

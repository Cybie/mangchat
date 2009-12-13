#ifndef _MC_IRC_CMD_H
#define _MC_IRC_CMD_H

#include "IRCMsg.h"

class IRCCmd
{
	public:
		IRCCmd(void){};
		IRCCmd(IRCMsg *_msg){ msg = _msg;  };
		~IRCCmd(void);

	public:
		void OnDefault();
		void OnPrivMsg();
		void OnNotice();
		void OnConnect();
		void OnJoin();
		void OnPart();
		void OnNick();
		void OnKick();
		void OnMode();
		void OnQuit();
	
	private:
		IRCMsg *msg;
};

#endif

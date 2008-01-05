#include "IRCClient.h"
#include "IRCCmd.h"

class mcs_OnlinePlayers : public ZThread::Runnable
{
public:
	// IRCClient Constructor
	mcs_OnlinePlayers();
	mcs_OnlinePlayers(_CDATA *_CD);
	// IRCClient Destructor
	~mcs_OnlinePlayers();
	// ZThread Entry
	void run();
public:
	_CDATA *CD;
};
/*
 * MangChat v1.0 By Cybrax (VisualDreams)
 *
 * This Software Is Currently Still Under Development. (Tested In WindowsXP/Debian Linux)
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify
 * It Under The Terms Of The GNU General Public License
 * --- MangChat Copyright (C) 2007 VisualDreams <http://dev.visualdreams.nl> ---
 * ---- Written and Developed by Cybrax. cybrax_vd@hotmail.com
 * ----- MangChat v0.1 was enhanced by Lice <lice@yeuxverts.net>, Dj_baby & Sanaell
 * ------ MangChat 1.0 was further developed by |Death| <death@hell360.net>
 * With Help And Support From The MaNGOS Project Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */
#include "IRCClient.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#include <locale.h>
#include <wchar.h>
// #define LANG se_SE.UTF-8

#include "Policies/SingletonImp.h"
INSTANTIATE_SINGLETON_1( IRCClient );
#ifdef WIN32
 #define Delay(x) Sleep(x)
 #else
 #define Delay(x) sleep(x / 1000)
#endif
// IRCClient Constructor
IRCClient::IRCClient(){}
// IRCClient Destructor
IRCClient::~IRCClient(){}
// ZThread Entry
// This function is called when the thread is created in Master.cpp (mangosd)
void IRCClient::run()
{
	// before we begin we wait a few seconds
	// mangos is still starting up and max screw
	// up the console text
	Delay(1500);
    sLog.outString("\n%s\n%s\n%s\n%s\n%s\n",
    "***************************************",
    "#    MANGCHAT Threaded IRC CLient     #",
    "#     With enhanched GM Control.      #",
    "***************************************",
    "***** MangChat: Version 1.0.0.0 *******");
	// Initialize connection count 0
	int cCount = 0;
	// Clean Up MySQL Tables
	sLog.outString("*** MangChat: Cleaning Up Inchan Table*");
	WorldDatabase.PExecute("DELETE FROM `IRC_Inchan`");
	// Load The Configuration From mangosd.conf
	sLog.outString("*** MangChat: Loading Configuration ***");
	LoadConfig();	
	// Added by me for support for Swedish Charset
// 	ln_
	localeInfo = _wsetlocale(LC_ALL, L"USA");
	localeInfo_start = "USA"; // = (char *) "Swedish";
	sLog.outString("Language -> [%s]",localeInfo_start);
	if(localeInfo == NULL) 
	{
		fprintf(stderr, "[ERROR] Can't set the specified locale! "
              "Check LANG, LC_CTYPE, LC_ALL.\n");
	}
	else
	{
		wprintf(L"%ls -> [%ls]\n", L"We know have support for charset", localeInfo);

	}
	
	sIRC._Max_Script_Inst = 0;
	// Create a loop to keep the thread running untill active is set to false
	while(sIRC.Active && !World::m_stopEvent)
	{
		// Initialize socket library
		if(this->InitSock())
		{
			// Connect To The IRC Server			
			sLog.outString("*** MangChat: Connection Try # %d ******", cCount);
			if(this->Connect(sIRC._Host.c_str(), sIRC._Port))
			{
				// On connection success reset the connection counter
				cCount = 0;
				sLog.outString("*** MangChat: Connected And Logging In*");
				// Login to the IRC server
				if(this->Login(sIRC._Nick, sIRC._User, sIRC._Pass))
				{
					sLog.outString("*** MangChat: Logged In And Running!! *");
					// While we are connected to the irc server keep listening for data on the socket
					while(sIRC.Connected && !World::m_stopEvent){ sIRC.SockRecv(); }
				}
				sLog.outString("*** MangChat: Connection To IRC Server Lost! ***");
			}
			// When an error occures or connection lost cleanup
			Disconnect();
			// Increase the connection counter
			cCount++;
			// if MAX_CONNECT_ATTEMPT is reached stop trying
			if(sIRC._MCA != 0 && cCount == sIRC._MCA)
				sIRC.Active = false;
			// If we need to reattempt a connection wait WAIT_CONNECT_TIME milli seconds before we try again
			if(sIRC.Active)
				Delay(WAIT_CONNECT_TIME);
		}
		else
		{
			// Socket could not initialize cancel
			sIRC.Active = false;
			sLog.outError("** MangChat: Could not initialize socket");
		}
	}
	// we need to keep the thread alive or mangos will crash
	// when sending chat or join/leave channels.
	// even when we are not connected the functions must still
	// be availlable where chat is sent to so we keep it running
	while(!World::m_stopEvent){};
}

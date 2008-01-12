#include "MCGame.h"
#include "IRCClient.h"

MC_Game::MC_Game() {}

MC_Game::~MC_Game()
{
}

void MC_Game::run()
{
    sIRC.Send_IRC_Channel("hell360-wow", "A new game will start in 10 seconds type 'join' to play");
    ZThread::Thread::sleep(10000);

    std::string playerlist = "";

    for(std::list<gPlayer*>::iterator i=sIRC.GamePlayers.begin(); i!=sIRC.GamePlayers.end();i++)
    {
        playerlist.append((*i)->name);

        char *betval = "";

        itoa((*i)->cBet, betval, 10);
        
        playerlist.append("(" + (std::string)betval + ") ");
    }

    sIRC.Send_IRC_Channel("hell360-wow", "Game starting with :" + playerlist);

}
#ifndef _IRC_CLIENT_GAME
#define _IRC_CLIENT_GAME

#include "Common.h"

struct gPlayer
{
    std::string name;
    int cBank;
    int cCash;
    int cBet;

    int BetValue;
};

class MC_Game : public ZThread::Runnable
{
    public:
        MC_Game();
        ~MC_Game();
        void run();
};

#endif

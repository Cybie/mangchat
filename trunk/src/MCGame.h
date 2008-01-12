#ifndef _IRC_CLIENT_GAME
#define _IRC_CLIENT_GAME

#include "Common.h"

struct PlayCard
{
    std::string fullName;
    int cSuit;
    int cRank;

    int cID;

    PlayCard(int _cID, int _suit, int _rank, const char *fname)
    {
        cSuit = _suit;
        cRank = _rank;

        fullName = fname;

        cID = _cID;
    }
};

struct gPlayer
{
    std::string name;
    int cBank;
    int cCash;
    int cBet;

    int BetValue;

    std::list<PlayCard*> PlayerHand;

    PlayCard *Card1;
    PlayCard *Card2;
};

struct gDealer
{
    PlayCard *Cards[5];
};

class MC_Game : public ZThread::Runnable
{
    public:
        MC_Game();
        ~MC_Game();
        void run();

    public:
        void PokerGame();
        void CheckHand(PlayCard *Card1, PlayCard *Card2);
        
    private:
        std::string CardsSuit[4]; // ={"spades", "diamonds", "clubs", "hearts"};
        std::string CardsRank[13]; //={"ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "jack", "queen", "king"};

    private:
        std::list<PlayCard*> GameDeck;
        std::list<PlayCard*> Dealer;

};

#endif

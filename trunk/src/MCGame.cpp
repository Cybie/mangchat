#include "MCGame.h"
#include "IRCClient.h"

#define GAME_CHANNEL "cybrax"

MC_Game::MC_Game()
{

    CardsSuit[0] = "spades";
    CardsSuit[1] = "diamonds";
    CardsSuit[2] = "clubs";
    CardsSuit[3] = "hearts";

    CardsRank[0] = "ace";
    CardsRank[1] = "2";
    CardsRank[2] = "3";
    CardsRank[3] = "4";
    CardsRank[4] = "5";
    CardsRank[5] = "6";
    CardsRank[6] = "7";
    CardsRank[7] = "8";
    CardsRank[8] = "9";
    CardsRank[9] = "10";
    CardsRank[10] = "jack";
    CardsRank[11] = "queen";
    CardsRank[12] = "king";
}

MC_Game::~MC_Game()
{



}

void MC_Game::run()
{
    int pokerPot = 0;

    sIRC.Send_IRC_Channel(GAME_CHANNEL, "A new game will start in 10 seconds type 'join' to play");
    ZThread::Thread::sleep(10000);

    std::string playerlist = "";

    for(std::list<gPlayer*>::iterator i=sIRC.GamePlayers.begin(); i!=sIRC.GamePlayers.end();i++)
    {
        playerlist.append((*i)->name);

       // pokerPot += (*i)->cBet;
        
        playerlist.append("(1000) ");
    }

    sIRC.Send_IRC_Channel(GAME_CHANNEL, "Game starting with :" + playerlist);

    PokerGame();
}

void MC_Game::PokerGame()
{
    srand(time(0));
    int deck[52];

//    gDealer _Dealer;

    for(int i = 0;i < 52;i++)
        deck[i] = i;

    for(int s = 0;s < 10;s++)
    {
        for(int i = 0;i < 52;i++)
        {
            int j = rand() % 52;
            int temp = deck[i];
            deck[i] = deck[j];
            deck[j] = temp;
        }        
    }

    for(int i = 0;i < 52;i++)
    {
        int cSuit = deck[i] / 13;
        int cRank = deck[i] % 13;

        std::string cardName = CardsRank[cRank] + " of " + CardsSuit[cSuit];

        PlayCard *NewCard = new PlayCard(deck[i], cSuit, cRank, cardName.c_str());
        GameDeck.push_back(NewCard);
    }

    std::string playcards = "";
    for(std::list<gPlayer*>::iterator i=sIRC.GamePlayers.begin(); i!=sIRC.GamePlayers.end();i++)
    {
        std::string phand = "";

        std::list<PlayCard*>::iterator SwapCard1 = GameDeck.begin();       
        phand.append((*SwapCard1)->fullName);
        (*i)->Card1 = (*SwapCard1); //(*i)->PlayerHand.push_back((*SwapCard1));
        GameDeck.erase(GameDeck.begin());

        std::list<PlayCard*>::iterator SwapCard2 = GameDeck.begin();       
        phand.append(", " + (*SwapCard2)->fullName);
        (*i)->Card2 = (*SwapCard2);// (*i)->PlayerHand.push_back((*SwapCard2));
        GameDeck.erase(GameDeck.begin());

        sIRC.Send_IRC_Channel(GAME_CHANNEL, (*i)->name + " has: " + phand);
    }

    int handcount = 0;

    for(std::list<PlayCard*>::iterator i=GameDeck.begin(); i!=GameDeck.end();i++)
    {        
        Dealer.push_back((*i));
//        _Dealer.Cards[i] = (*i);
        GameDeck.erase(i, i);

        handcount++;
        if(handcount == 5)
            break;
    }

    for(std::list<PlayCard*>::iterator i=Dealer.begin(); i!=Dealer.end();i++)
    {
        playcards.append((*i)->fullName);
        playcards.append(", ");
    }

    sIRC.Send_IRC_Channel(GAME_CHANNEL, "Dealer :" + playcards);

    for(std::list<gPlayer*>::iterator i=sIRC.GamePlayers.begin(); i!=sIRC.GamePlayers.end();i++)
    {
      //  CheckHand(Dealer, (*i)->Card1, (*i)->Card1);
    }


    // delete Card allocated by new
    for(std::list<PlayCard*>::iterator i=GameDeck.begin(); i!=GameDeck.end();i++)
        delete (*i);

    // delete pointers from deck
    for(std::list<PlayCard*>::iterator i=Dealer.begin(); i!=Dealer.end();i++)
        delete (*i);

    // delete players allocated by new ands card moved from deck
    for(std::list<gPlayer*>::iterator i=sIRC.GamePlayers.begin(); i!=sIRC.GamePlayers.end();i++)
    {
        delete (*i)->Card1;
        delete (*i)->Card2;
        delete (*i);
    }

    // Clear playerlist
    sIRC.GamePlayers.clear();

    // Relese game
    sIRC.Script_Lock[MCS_Poker_Game] = false;

}


void MC_Game::CheckHand(PlayCard *Card1, PlayCard *Card2)
{
    PlayCard *FullHand[7];

    FullHand[0] = Card1;
    FullHand[1] = Card2;

    int dCount = 2;
    for(std::list<PlayCard*>::iterator i=Dealer.begin(); i!=Dealer.end();i++)
    {
        FullHand[dCount] = (PlayCard*)(*i);
        dCount++;
    }    

    /*
    int R[5];
    int S[5];
    for(i=0; i<5; i++)
    {
        R[i] = deck[i]%13;
        S[i] = deck[i]/13;
    }

    bool swapped = false;
    do
    {
        // 1 pass of the bubble sort
        swapped = false;
        for(int i=0; i<4; i++)
        {
            if(R[i] > R[i+1])
            {
                int temp = R[i];
                R[i] = R[i+1];
                R[i+1] = temp;
                swapped = true;
            }
        }
    }
    while(swapped == true);
    */

}

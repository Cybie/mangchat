#ifndef _IRC_CLIENT_H
#define _IRC_CLIENT_H

#include "Policies/Singleton.h"
#include "../Player.h"
#include "MCGame.h"
#include "IRCLog.h"

//Uncomment this to enable UTF-8 conversion
#define USE_UTF8

#ifdef USE_UTF8
#include "iconv.h"
#endif


using namespace std;
// The maximum ammount of channels used
// in the channel array you can have as much channels as you
// want, but it is important to always have at least equal or more
// channels then you specify in your mangosd.conf
#define MAX_CONF_CHANNELS 10
#define MAX_CHAT_LINES 10
// time we need to wait before we try another connecton attempt
// Default is 30 seconds
#define MAX_SCRIPT_INST 10
// CLINES is used for the default chatlines
// By using the GetChatLine function its easier and faster
// to receieve the line you need.
enum CLINES
{
    IRC_WOW = 0,
    WOW_IRC = 1,
    JOIN_WOW = 2,
    JOIN_IRC = 3,
    LEAVE_WOW = 4,
    LEAVE_IRC = 5,
    CHANGE_NICK = 6
};                                                          // Chatlines
// CACTION is used by the Handle_WoW_Channel function
// this function is called in channel.h when a player
// joins or leave a channel inside the client.
enum CACTION
{
    CHANNEL_JOIN,
    CHANNEL_LEAVE,
};
enum CIMSG
{
    MSG_PRIV = 0,
    MSG_NOTICE = 1,
    MSG_ACTION = 2,
};

enum script_Names
{
    MCS_Players_Online  = 0,
    MCS_Poker_Game      = 1,
    MCS_PlaceHolder_2   = 2,
};

// IRCClient main class
class IRCClient : public ZThread::Runnable
{
    public:
        // IRCClient Constructor
        IRCClient();
        // IRCClient Destructor
        ~IRCClient();
        // ZThread Entry
        void run();
    public:
        // IRCClient active
        bool    Active;
        // Connected to IRC
        bool    Connected;
        // Socket indentifier
        int     SOCKET;
        fd_set  sfdset;
        // Send data to IRC, in addition the endline is added \n
        bool    SendIRC(std::string data);
        // This function is called in ChatHandler.cpp and processes the chat from game to IRC
        void    Send_WoW_IRC(Player *plr, std::string Channel, std::string Msg);
        // Sends a message to all players on the specified channel
        void    Send_WoW_Channel(const char *channel, std::string chat);
        // Send a system message to all players
        void    Send_WoW_System(std::string Message);
        // Send a message to the specified IRC channel
        void    Send_IRC_Channel(std::string sChannel, std::string sMsg, bool NoPrefix = false, int nType = MSG_PRIV);
        // Sends a message to all IRC Channels
        void    Send_IRC_Channels(std::string sMsg);
        std::string MakeMsg(std::string msg, std::string var, std::string val)
        {
            std::size_t start = msg.find(var);
            if (start != std::string::npos)
                msg.replace(start, var.length(), val);
            return msg;
        }
        void    Send_WoW_Player(string sPlayer, string sMsg);
        void    Send_WoW_Player(Player *plr, string sMsg);

        // This function is called in Channel.cpp and processes Join/leave messages
        void    Handle_WoW_Channel(std::string Channel, Player *plr, int nAction);
        void ResetIRC();
    public:
        void AutoJoinChannel(Player *plr);
        bool ConvertUTF8(const char* tocode, const char* fromcode, const char *chat, std::string &converted_utf);

    public:
        bool Script_Lock[5];

    public:
        // IRC Server host
        string  _Host;
        // IRC Server Port
        int _Port;
        // IRC Username
        string  _User;
        // IRC Password
        string  _Pass;
        // IRC Nickname
        string  _Nick;
        // Authentication type
        int _Auth;
        // IRC Connect code
        string  _ICC;
        // IRC Default channel
        string  _defchan;
        // IRC Leave Default channel
        int _ldefc;
        // Wait Connect Time
        int _wct;
        // BotMask Options
        int Botmask;
        // IRC Channel count
        // DO NOT CHANGE THIS
        // if you wish to handle more then 10 channes
        // change MAX_CONF_CHANNELS from the defines.
        int _chan_count;
        // IRC Channel list
        // Array to store our IRC channels
        // each element will corrospond
        // with _wow_chan array below.
        std::string _irc_chan[MAX_CONF_CHANNELS];
        // Game Channel list
        std::string _wow_chan[MAX_CONF_CHANNELS];
        // AutoJoin Options
		int ajoin;
		string ajchan;
		// Online Command Max Results
		int onlrslt;
        // Channel OnJoin/Restart/Kick Messages
        string  JoinMsg;
        string  RstMsg;
        string  kikmsg;
        // Misc Options
        string  ojGM1;
        string  ojGM2;
        string  ojGM3;
        string  ojGM4;
        string  ojGM5;
        string  logfile;
		int     games;
		int     gmlog;
		// IRC Commands Security Level
        int     CACCT;
		int     CFUN;
        int     CHELP;
        int     CITEM;
        int     CJAIL;
        int     CKICK;
        int     _KILL;
        int     CLEVEL;
        int     CMONEY;
        int     CMUTE;
        int     CPLAYER;
        int     CPM;
        int     CRESTART;
        int     CREVIVE;
        int     CSAVEALL;
        int     CSHUTDOWN;
        int     CSPELL;
        int     CSYSMSG;
        int     CTELE;
        int     CWHO;
        // BotMask
        int     BOTMASK;
        // Max connect attempt
        int     _MCA;
        // Auto rejoin when kicked from irc
        int     _autojoinkick;
        // IRC Command prefix
        string  _cmd_prefx;
        // contains the Chatlines
        // Array that contains our chatlines from the conf file
        // To increase this value change the MAX_CHAT_LINE define above
        // Make sure the number of elements must match your items
        // (remeber this starts at 0 so 0..9 is 10 items)
        // and that you load the line in the LoadConfig function.
        string  ILINES[MAX_CHAT_LINES];
        string  GetChatLine(int nItem);

        int _Max_Script_Inst;
        // MAX_SCRIPT_INST

        IRCLog iLog;

public:
    std::list<gPlayer*> GamePlayers;

    private:
        // Load configuration from the mangosd.conf file
        // to be used with the irc client.
        bool    LoadConfig();
        // Returns default chatline based on enum CLINES
        // Initialize socket library
        bool    InitSock();
        // Connect to IRC Server
        bool    Connect(const char *cHost, int nPort);
        // Login to IRC Server
        bool    Login(std::string sNick, std::string sUser, std::string sPass);
        // Send raw data to IRC
        bool    SendData(const char *data);
        // Disconnect from IRC and cleanup socket
        void    Disconnect();
        // Processes the data receieved from IRC
        void    Handle_IRC(std::string sData);
        // Receieves data from the socket.
        void    SockRecv();
};
#endif

#define sIRC MaNGOS::Singleton<IRCClient>::Instance()

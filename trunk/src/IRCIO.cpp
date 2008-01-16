#include "IRCClient.h"
#include "IRCCmd.h"
#include "IRCFunc.h"
#include "../ObjectAccessor.h"
#include "../WorldPacket.h"
#include "../ChannelMgr.h"
#include "../Config/ConfigEnv.h"
#include "../Channel.h"

IRCCmd Command;
void IRCClient::Handle_IRC(std::string sData)
{
    sLog.outDebug(sData.c_str());
    // If first 5 chars are ERROR then something is wrong
    // either link is being closed, nickserv ghost command, etc...
    if(sData.substr(0, 5) == "ERROR")
    {
        Disconnect();
        return;
    }
    if(sData.substr(0, 4) == "PING")
    {
        // if the first 4 characters contain PING
        // the server is checking if we are still alive
        // sen back PONG back plus whatever the server send with it
        SendIRC("PONG " + sData.substr(4, sData.size() - 4));
    }
    else
    {
        // if the first line contains : its an irc message
        // such as private messages channel join etc.
        if(sData.substr(0, 1) == ":")
        {
            // find the spaces in the receieved line
            size_t p1 = sData.find(" ");
            size_t p2 = sData.find(" ", p1 + 1);
            // because the irc protocol uses simple spaces
            // to seperate data we can easy pick them out
            // since we know the position of the spaces
            std::string USR = sData.substr(1, p1 - 1);
            std::string CMD = sData.substr(p1 + 1, p2 - p1 - 1);
            // trasform the commands to lowercase to make sure they always match
            std::transform(CMD.begin(), CMD.end(), CMD.begin(), towlower);
            // Extract the username from the first part
            std::string szUser = GetUser(USR);
            // if we receieved the internet connect code
            // we know for sure that were in and we can
            // authenticate ourself.
            if(CMD == sIRC._ICC)
            {
                // _Auth is defined in mangosd.conf (irc.auth)
                // 0 do not authenticate
                // 1 use nickserv
                // 2 use quakenet
                // aditionally you can provide you own authentication method here
                switch(sIRC._Auth)
                {
                    case 1:
                        SendIRC("PRIVMSG nickserv :IDENTIFY " + sIRC._Pass);
                        break;
                    case 2:
                        SendIRC("PRIVMSG Q@CServe.quakenet.org :AUTH " + sIRC._Nick + " " + sIRC._Pass);
                        break;
                }
                // if we join a default channel leave this now.
                if(sIRC._ldefc==1)
                    SendIRC("PART #" + sIRC._defchan);
                // Loop thru the channel array and send a command to join them on IRC.
                for(int i=1;i < sIRC._chan_count + 1;i++)
                {
                    SendIRC("JOIN #" + sIRC._irc_chan[i]);
                    Send_IRC_Channel(sIRC._irc_chan[i], sIRC.JoinMsg);
                }
            }
            // IRC Command channel join.
            // This command is receieved when MangChat joins a channel
            // or someone else on irc joins the channel
            if(CMD == "join")
            {
                size_t p = sData.find(":", p1);
                std::string CHAN = sData.substr(p + 1, sData.size() - p - 2);
                // if the user is us it means we join the channel
                if ((szUser == sIRC._Nick) )
                {
                    // its us that joined the channel
                    Send_IRC_Channel(CHAN, sIRC.JoinMsg, true);
                }
                else
                {
                    // if the user is not us its someone else that joins
                    // so we construct a message and send this to the clients.
                    // MangChat now uses Send_WoW_Channel to send to the client
                    // this makes MangChat handle the packets instead of previously the world.
                    if((sIRC.BOTMASK & 2) != 0)
                        Send_WoW_Channel(GetWoWChannel(CHAN).c_str(), IRCcol2WoW(MakeMsg(MakeMsg(GetChatLine(JOIN_IRC), "$Name", szUser), "$Channel", GetWoWChannel(CHAN))));
                }
            }
            // someone on irc left the channel
            if(CMD == "part" || CMD == "quit")
            {
                size_t p3 = sData.find(" ", p2 + 1);
                std::string CHAN = sData.substr(p2 + 1, p3 - p2 - 1);
                // Logout IRC Nick From MangChat If User Leaves Or Quits IRC.
                if(Command.IsLoggedIn(szUser))
                {
                    _CDATA CDATA;
                    CDATA.USER      = szUser;
                    Command.Handle_Logout(&CDATA);
                }
                if((sIRC.BOTMASK & 2) != 0)
                    Send_WoW_Channel(GetWoWChannel(CHAN).c_str(), IRCcol2WoW(MakeMsg(MakeMsg(GetChatLine(LEAVE_IRC), "$Name", szUser), "$Channel", GetWoWChannel(CHAN))));
                // Construct a message and inform the clients on the same channel.
            }
            if (CMD == "nick" && (sIRC.BOTMASK & 128) != 0)
            {
                MakeMsg(MakeMsg(GetChatLine(CHANGE_NICK), "$Name", szUser), "$NewName", sData.substr(sData.find(":", p2) + 1));
            }
            // someone was kicked from irc
            if (CMD == "kick")
            {
                // extract the details
                size_t p3 = sData.find(" ", p2 + 1);
                size_t p4 = sData.find(" ", p3 + 1);
                size_t p5 = sData.find(":", p4);
                std::string CHAN = sData.substr(p2 + 1, p3 - p2 - 1);
                std::string WHO = sData.substr(p3 + 1, p4 - p3 - 1);
                std::string BY = sData.substr(p4 + 1, sData.size() - p4 - 1);
                // if the one kicked was us
                if(WHO == sIRC._Nick)
                {
                    // and autojoin is enabled
                    // return to the channel
                    if(sIRC._autojoinkick == 1)
                    {
                        SendIRC("JOIN " + CHAN);
                        Send_IRC_Channel(CHAN, sIRC.kikmsg, true);
                    }
                }
                else
                {
                    // if it is not us who was kicked we need to inform the clients someone
                    // was removed from the channel
                    // construct a message and send it to the players.
                    Send_WoW_Channel(GetWoWChannel(CHAN).c_str(), "<IRC>[" + WHO + "]: Was Kicked From " + CHAN + " By: " + szUser);
                }
            }
            // a chat message was receieved.
            if(CMD == "privmsg")
            {
                // extract the values
                size_t p = sData.find(" ", p2 + 1);
                std::string FROM = sData.substr(p2 + 1, p - p2 - 1);
                std::string CHAT = sData.substr(p + 2, sData.size() - p - 3);
                // if this is our username it means we recieved a PM
                if(FROM == sIRC._Nick)
                {
                    if(CHAT.find("\001VERSION\001") < CHAT.size())
                    {
                        Send_IRC_Channel(szUser, "\001VERSION MangChat v1.0 ©2007 Cybrax @ VisualDreams, Enhanced By: |Death|\001", true, MSG_NOTICE);
                    }
                    // a pm is required for certain commands
                    // such as login. to validate the command
                    // we send it to the command class wich handles
                    // evrything else.
                    Command.IsValid(szUser, FROM, CHAT);
                }
                else
                {
                    // if our name is not in it, it means we receieved chat on one of the channels
                    // magchat is in. the first thing we do is check if it is a command or not
                    if(!Command.IsValid(szUser, FROM, CHAT))
                        Send_WoW_Channel(GetWoWChannel(FROM).c_str(), IRCcol2WoW(MakeMsg(MakeMsg(GetChatLine(IRC_WOW), "$Name", szUser), "$Msg", CHAT)));
                    // if we indeed receieved a command we do not want to display this to the players
                    // so only incanse the isvalid command returns false it will be sent to all player.
                    // the isvalid function will automaitcly process the command on true.
                }
            }
        }
    }
}

// This function is called in Channel.h
// based on nAction it will inform the people on
// irc when someone leaves one of the game channels.
// nAction is based on the struct CACTION
void IRCClient::Handle_WoW_Channel(std::string Channel, Player *plr, int nAction)
{
    // make sure that we are connected
    if(sIRC.Connected && (sIRC.BOTMASK & 1)!= 0)
    {
        if(Channel_Valid(Channel))
        {
            std::string GMRank = "";
            std::string pname = plr->GetName();
            bool DoGMAnnounce = false;
            if (plr->GetSession()->GetSecurity() > 0 && (sIRC.BOTMASK & 8)!= 0)
                DoGMAnnounce = true;
            if (plr->isGameMaster() && (sIRC.BOTMASK & 16)!= 0)
                DoGMAnnounce = true;
            if(DoGMAnnounce)
            {
                switch(plr->GetSession()->GetSecurity())    //switch case to determine what rank the "gm" is
                {
                    case 0: GMRank = "";break;
                    case 1: GMRank = sIRC.ojGM1;break;
                    case 2: GMRank = sIRC.ojGM2;break;
                    case 3: GMRank = sIRC.ojGM3;break;
                    case 4: GMRank = sIRC.ojGM4;break;
                    case 5: GMRank = sIRC.ojGM5;break;
                }
            }
            std::string ChatTag = "";
            switch (plr->GetTeam())
            {
                case 67:ChatTag.append("\0034");break;      //horde
                case 469:ChatTag.append("\00312");break;    //alliance
            }
            std::string query = "INSERT INTO `IRC_Inchan` VALUES (%d,'"+pname+"','"+Channel+"')";
            std::string lchan = "DELETE FROM `IRC_Inchan` WHERE `guid` = %d AND `channel` = '"+Channel+"'";
            switch(nAction)
            {
                case CHANNEL_JOIN:
                    Send_IRC_Channel(GetIRCChannel(Channel), ChatTag + MakeMsg(MakeMsg(MakeMsg(GetChatLine(JOIN_WOW), "$Name", plr->GetName()), "$Channel", Channel), "$GM", GMRank));
                    WorldDatabase.PExecute(lchan.c_str(), plr->GetGUID());
                    WorldDatabase.PExecute(query.c_str(), plr->GetGUID());
                    break;
                case CHANNEL_LEAVE:
                    Send_IRC_Channel(GetIRCChannel(Channel), ChatTag + MakeMsg(MakeMsg(MakeMsg(GetChatLine(LEAVE_WOW), "$Name", plr->GetName()), "$Channel", Channel), "$GM", GMRank));
                    WorldDatabase.PExecute(lchan.c_str(), plr->GetGUID());
                    break;
            }
        }
    }
}

// This function sends chat to a irc channel or user
// to prevent the # beeing appended to send a msg to a user
// set the NoPrefix to true
void IRCClient::Send_IRC_Channel(std::string sChannel, std::string sMsg, bool NoPrefix, int nType)
{

	// Creates the string that recives if all works
	std::string sMsg2 = sMsg;
	// This does the thing !!
	if(Converter("char", "UTF-8", sMsg2.c_str(), sMsg2))
        sMsg=sMsg2;

    std::string mType = "PRIVMSG";
    switch(nType)
    {
        case MSG_NOTICE:
            mType = "NOTICE";
            break;
        case MSG_ACTION:
            mType = "ACTION";
            break;
    }
    if(sIRC.Connected)
    {
        if(NoPrefix)
        {
            if(sChannel.substr(0,1) != "#" && (sIRC.BOTMASK & 32)!= 0)
                mType = "NOTICE";
            SendIRC(mType + " " + sChannel + " :" + sMsg);
        }
        else
            SendIRC(mType + " #" + sChannel + " :" + sMsg);
    }
}

// This function sends a message to all irc channels
// that mangchat has in its configuration
void IRCClient::Send_IRC_Channels(std::string sMsg)
{
    for(int i=1;i < sIRC._chan_count + 1;i++)
        Send_IRC_Channel(sIRC._irc_chan[i], sMsg);
}

// This function is called in ChatHandler.cpp, any channel chat from wow will come
// to this function, validates the channel and constructs a message that is send to IRC
void IRCClient::Send_WoW_IRC(Player *plr, std::string Channel, std::string Msg)
{
    // Check if the channel exist in our configuration
    if(Channel_Valid(Channel))
        Send_IRC_Channel(GetIRCChannel(Channel), MakeMsgP(WOW_IRC, Msg, plr));
}

void IRCClient::Send_WoW_Player(std::string sPlayer, std::string sMsg)
{
    normalizePlayerName(sPlayer);
    if (Player* plr = ObjectAccessor::Instance().FindPlayerByName(sPlayer.c_str()))
        Send_WoW_Player(plr, sMsg);
}

void IRCClient::Send_WoW_Player(Player *plr, string sMsg)
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);
    data << (uint8)CHAT_MSG_SYSTEM;
    data << (uint32)LANG_UNIVERSAL;
    data << (uint64)plr->GetGUID();
    data << (uint32)0;
    data << (uint64)plr->GetGUID();
    data << (uint32)(sMsg.length()+1);
    data << sMsg;
    data << (uint8)0;
    plr->GetSession()->SendPacket(&data);
}

// This function will construct and send a packet to all players
// on the given channel ingame. (previuosly found in world.cpp)
// it loops thru all sessions and checks if they are on the channel
// if so construct a packet and send it.
void IRCClient::Send_WoW_Channel(const char *channel, std::string chat)
{
    if(!(strlen(channel) > 0))
        return;
    HashMapHolder<Player>::MapType& m = ObjectAccessor::Instance().GetPlayers();
    for(HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (itr->second && itr->second->GetSession()->GetPlayer() && itr->second->GetSession()->GetPlayer()->IsInWorld())
        {
            if(ChannelMgr* cMgr = channelMgr(itr->second->GetSession()->GetPlayer()->GetTeam()))
            {
                if(Channel *chn = cMgr->GetChannel(channel, itr->second->GetSession()->GetPlayer()))
                {
                    WorldPacket data;
                    data.Initialize(SMSG_MESSAGECHAT);
                    data << (uint8)CHAT_MSG_CHANNEL;
                    data << (uint32)LANG_UNIVERSAL;
                    data << (uint64)0;
                    data << (uint32)0;
                    data << channel;
                    data << (uint64)0;
                    data << (uint32) (strlen(chat.c_str()) + 1);
                    data << chat.c_str();
                    data << (uint8)0;
                    itr->second->GetSession()->SendPacket(&data);
                }
            }
        }
    }
}

void IRCClient::Send_WoW_System(std::string Message)
{
    HashMapHolder<Player>::MapType& m = ObjectAccessor::Instance().GetPlayers();
    for(HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (itr->second && itr->second->GetSession()->GetPlayer() && itr->second->GetSession()->GetPlayer()->IsInWorld())
        {
            WorldPacket data;
            data.Initialize(CHAT_MSG_SYSTEM);
            data << (uint8)CHAT_MSG_SYSTEM;
            data << (uint32)LANG_UNIVERSAL;
            data << (uint64)0;
            data << (uint32)0;
            data << (uint64)0;
            data << (uint32) (strlen(Message.c_str()) + 1);
            data << Message.c_str();
            data << (uint8)0;
            itr->second->GetSession()->SendPacket(&data);
        }
    }
}

void IRCClient::ResetIRC()
{
    SendData("QUIT");
    Disconnect();
}

#define CHAT_INVITE_NOTICE 0x18

// this function should be called on player login Player::AddToWorld
void IRCClient::AutoJoinChannel(Player *plr)
{
    //this will work if at least 1 player is logged in regrdless if he is on the channel ir not
    // the first person that login empty server is the one with bad luck and wont be invivted, 
    // if at least 1 player is online the player will be inited to the chanel

    std::string m_name = "world";
    WorldPacket data;
    data.Initialize(SMSG_CHANNEL_NOTIFY, 1+m_name.size()+1);
    data << uint8(CHAT_INVITE_NOTICE);
    data << m_name.c_str();

    HashMapHolder<Player>::MapType& m = ObjectAccessor::Instance().GetPlayers();
    for(HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (itr->second && itr->second->GetSession()->GetPlayer() && itr->second->GetSession()->GetPlayer()->IsInWorld())
        {
            data << uint64(itr->second->GetGUID());
            break;
        }
    }
    plr->GetSession()->SendPacket(&data);
}

/*
void ConvertCToT(TCHAR* pszDest, const CHAR* pszSrc)
{
	for(int i = 0; i < strlen(pszSrc); i++)
		pszDest[i] = (TCHAR) pszSrc[i];
}
*/

bool IRCClient::Converter(const char* tocode, const char* fromcode, const char *chat, std::string &converted_utf)
{
    // extern void error();
    iconv_t cd;
    cd	= iconv_open(tocode, fromcode);
    if (cd != (iconv_t) -1)
    {
	    // Yes whe now can convert chars into UTF-8

	    size_t size_orig = strlen(chat);
	    size_t size_new = (size_t) 2040;

	    char just_test[2048] = "";
	    char *chat_converted = just_test;
	    size_t size_converted;

	    // Lets convert from Chars to UTF-8
	    size_converted = iconv(cd, &chat, &size_orig, &chat_converted, &size_new);
	
	    // If its converted right lets send it back
	    if(size_converted != -1)
        {
		    // Yes it was converted right and we can safley return the chars to bee sent into the Wow channel
		    converted_utf = just_test;
	    }
        else
        {
    		// Debug ínfo of something goes wrong. A good thing to have when testing right now
	    	printf("Couldent convert the chars ??\nError will follow -> ");
		    int error_number = errno;
    		printf("[%i]\n",error_number);
	    	if(error_number == EINVAL) printf("Input conversion stopped due to an incomplete character or shift sequence at the end of the input buffer.\n");
		    if(error_number == EILSEQ) printf("Input conversion stopped due to an input byte that does not belong to the input codeset.\n");
    		if(error_number == E2BIG)  printf("Input conversion stopped due to lack of space in the output buffer. inbytesleft has the number of bytes to be converted.\n");
	    	return false;
	    }	

	    // Lets turn of the convert string. This will be moved later for better support
	    iconv_close(cd);
	    return true;
    }
    else
    {
		printf("Coulden even start to Convert HMM (MAJOR ERROR)!! ?? !!\n");
		return false;
	}
}

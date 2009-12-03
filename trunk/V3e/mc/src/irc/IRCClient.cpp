#include "IRCClient.h"
#include "IRCCmd.h"

#include <Config/ConfigEnv.h>
#include <World.h>
#include <ObjectMgr.h>
#include <MapManager.h>
#include <ObjectAccessor.h>
#include <WorldPacket.h>
#include <ChannelMgr.h>
#include <Channel.h>

void IRCClient::run() 
// Thread Entry 
{
//	this->iLog.WriteLog(" %s : ****** MaNGOS With MangChat Has Been Started ******", sIRC.iLog.GetLogDateTimeStr().c_str());
   
    while(this->bActive && !World::IsStopped())
    {
		this->Sock.Connect("192.168.0.1", 6667);
		this->Sock.Send("PASS MANGCHAT");
		this->Sock.Send("NICK MANGCHAT");
		this->Sock.Send("USER MANGCHAT Cybrax VisualDreams :MangChat 3.01");

		char Buffer[BUFFER_SIZE];
		int nRecv = 1;
		while(nRecv && this->bActive)
		{
			memset(Buffer, 0, BUFFER_SIZE);
			nRecv = this->Sock._Recv(Buffer, BUFFER_SIZE);
			std::string reply = "";
			std::istringstream iss(Buffer);
			while(getline(iss, reply))
				this->Handle_IRC(reply);
		}
     }
}

void IRCClient::Handle_IRC(std::string& sData)
{
	if(sData.substr(0, 5) == "ERROR")
        Disconnect();
	else if(sData.substr(0, 4) == "PING")
		Send("PONG " + sData.substr(4, sData.size() - 4));
	else if(sData[0] == ':')
    {
		if (sData[sData.size() - 1] == '\x0d' || sData[sData.size() - 1] == '\x0a')
			sData = sData.substr(0, sData.size() - 1);

		//printf("%s\n", sData.c_str());

		IRCMsg *msg = new IRCMsg(&Sock);

        size_t p1 = sData.find(" ");
        size_t p2 = sData.find(" ", p1 + 1);

        msg->USR = sData.substr(1, p1 - 1);
		size_t p3 = msg->USR.find("!");
		msg->HOST = msg->USR.substr(p3 + 1, msg->USR.size() - p3 - 1);
		msg->USR = msg->USR.substr(0, p3);

        msg->CMD = sData.substr(p1 + 1, p2 - p1 - 1);
		msg->DATA = sData.substr(p2 + 1, sData.size() - p2 - p1 - 1);

		IRCCmd *ic = new IRCCmd(msg);

		//std::transform(msg->CMD.begin(), msg->CMD.end(), msg->CMD.begin(), towupper);

		if(msg->CMD == "001")
			ic->ICC();
		else if(msg->CMD == "JOIN")
			ic->Join();
		else if(msg->CMD == "PRIVMSG")
			ic->PrivMsg();
		else
			ic->PrivMsg();
		
		delete ic;
	}
}

void IRCClient::Send(std::string s)
{
	Sock.Send((char *)s.c_str());
}

void IRCClient::Send_WoW_Player(std::string sPlayer, std::string sMsg)
{
    normalizePlayerName(sPlayer);
    if (Player* plr = ObjectAccessor::Instance().FindPlayerByName(sPlayer.c_str()))
        Send_WoW_Player(plr, sMsg);
}

void IRCClient::Send_WoW_Player(Player *plr, std::string sMsg)
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

#include "MCS_OnlinePlayers.h"

#include "../MapManager.h"
#include "../ObjectMgr.h"
#include "../Config/ConfigEnv.h"

mcs_OnlinePlayers::mcs_OnlinePlayers() { CD = NULL; }

mcs_OnlinePlayers::mcs_OnlinePlayers(_CDATA *_CD)
{
    CD = new _CDATA();
    CD->CMD = _CD->CMD;
    CD->FROM = _CD->FROM;
    CD->PARAMS = _CD->PARAMS;
    CD->PCOUNT = _CD->PCOUNT;
    CD->USER = _CD->USER;

}

mcs_OnlinePlayers::~mcs_OnlinePlayers()
{
    if(CD)
        delete CD;
}

void mcs_OnlinePlayers::run()
{

    int OnlineCount = 0;
    std::string IRCOut = "";
    HashMapHolder<Player>::MapType& m = ObjectAccessor::Instance().GetPlayers();
    for(HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        if (itr->second && itr->second->GetSession()->GetPlayer() && itr->second->GetSession()->GetPlayer()->IsInWorld())
        {
            OnlineCount++;
            Player *plr = itr->second->GetSession()->GetPlayer();
            std::string ChatTag = " ";

            if(itr->second->GetSession()->GetSecurity() > sConfig.GetIntDefault("OnlineGM", 2))
                ChatTag.append("\002\0039<GM>\003\002");

            if(plr->isAFK())
                ChatTag.append("\002\0037<AFK>\003\002");
            else if(plr->isDND())
                ChatTag.append("\002\0037<DND>\003\002");

            // why is this here..??
            //if(itr->second->GetSession()->GetSecurity() > 0)
            //	ChatTag.append("");

            switch (plr->GetTeam())
            {
                case 67:ChatTag.append("\0034");break;      //horde
                case 469:ChatTag.append("\00312");break;    //alliance
            }

            IRCOut.append(IRCCmd::MakeMsg("%s\002%s\003\017\002(%d)\002\017", ChatTag.c_str(), plr->GetName(), plr->getLevel()));

            if(OnlineCount % 10 == 0)
            {
                sIRC.Send_IRC_Channel(IRCCmd::ChanOrPM(CD), IRCCmd::MakeMsg("\002 %s", IRCOut.c_str()), true);
                IRCOut = "";
                ZThread::Thread::sleep(1000);
            }
        }
    }
    // Remainder in IRCOUT && Total plyersonline
    sIRC.Send_IRC_Channel(IRCCmd::ChanOrPM(CD), IRCCmd::MakeMsg("\002 %s \002Players Online(%d)", IRCOut.c_str(), OnlineCount), true);

    sIRC.Script_Lock[MCS_Players_Online] = false;
}

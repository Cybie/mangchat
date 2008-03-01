#include "IRCClient.h"
#include "Config/ConfigEnv.h"
bool IRCClient::LoadConfig()
{
    int ConfCnt = 0;
    sIRC._chan_count = 0;
    if(sConfig.GetIntDefault("irc.active", 1) == 1)
        sIRC.Active = true;
    else
        sIRC.Active = false;
    sIRC._Host = sConfig.GetStringDefault("irc.host", "irc.freenode.net");
    if(sIRC._Host.size() > 0)
        ConfCnt++;
    sIRC._Port = sConfig.GetIntDefault("irc.port", 6667);
    sIRC._User = sConfig.GetStringDefault("irc.user", "cybmc");
    sIRC._Pass = sConfig.GetStringDefault("irc.pass", "cybmc");
    sIRC._Nick = sConfig.GetStringDefault("irc.nick", "cybmc");
    sIRC._Auth = sConfig.GetIntDefault("irc.auth", 0);
    sIRC._ICC = sConfig.GetStringDefault("irc.icc", "001");
    sIRC._defchan = sConfig.GetStringDefault("irc.defchan", "lobby");
    sIRC._ldefc = sConfig.GetIntDefault("irc.ldef", 0);
    sIRC._wct = sConfig.GetIntDefault("irc.wct", 30000);
   	sIRC.ajoin = sConfig.GetIntDefault("irc.ajoin", 1);
    sIRC.ajchan = sConfig.GetStringDefault("irc.ajchan", "world");
    sIRC.onlrslt = sConfig.GetIntDefault("irc.online.result", 10);
    sIRC.BOTMASK = sConfig.GetIntDefault("Botmask", 0);
    sIRC.logfile = sConfig.GetStringDefault("irc.logfile.prefix", "IRC_");
	for(int i = 1; i < MAX_CONF_CHANNELS;i++)
    {
        std::ostringstream ss;
        ss << i;
        std::string ci = "irc.chan_" + ss.str();
        std::string t_chan = sConfig.GetStringDefault(ci.c_str(), "");
        if(t_chan.size() > 0)
        {
            sIRC._chan_count++;
            sIRC._irc_chan[sIRC._chan_count] = t_chan;
            ci = "wow.chan_" + ss.str();
            sIRC._wow_chan[sIRC._chan_count] = sConfig.GetStringDefault(ci.c_str(), t_chan.c_str());
        }
    }
    sIRC.JoinMsg = sConfig.GetStringDefault("irc.joinmsg", "Whhaaazzzzaaaa, MangChat 1.0 Baby!!");
    sIRC.RstMsg  = sConfig.GetStringDefault("irc.rstmsg", "MangChat Is Restarting, I Will Be Right Back!");
    sIRC.kikmsg = sConfig.GetStringDefault("irc.kickmsg", "Do Not Kick Me Again, Severe Actions Will Be Taken!");
    // IRC LINES
    sIRC.ILINES[WOW_IRC] = sConfig.GetStringDefault("chat.wow_irc", "\003<WoW>[\002$Name($Level)\002\003] $Msg");
    sIRC.ILINES[IRC_WOW] = sConfig.GetStringDefault("chat.irc_wow", "\003<IRC>[$Name]: $Msg");
    sIRC.ILINES[JOIN_WOW] = sConfig.GetStringDefault("chat.join_wow", "\00312>>\00304 $Name \003Joined The Channel!");
    sIRC.ILINES[JOIN_IRC] = sConfig.GetStringDefault("chat.join_irc", "\003[$Name]: Has Joined IRC!");
    sIRC.ILINES[LEAVE_WOW] = sConfig.GetStringDefault("chat.leave_wow", "\00312<<\00304 $Name \003Left The Channel!");
    sIRC.ILINES[LEAVE_IRC] = sConfig.GetStringDefault("chat.leave_irc", "\003[$Name]: Has Left IRC!");
    sIRC.ILINES[CHANGE_NICK] = sConfig.GetStringDefault("chat.change_nick", "\003<> $Name Is Now Known As $NewName!");
    // MangChat Options
    sIRC._MCA = sConfig.GetIntDefault("irc.maxattempt", 10);
    sIRC._autojoinkick = sConfig.GetIntDefault("irc.autojoin_kick", 1);
    sIRC._cmd_prefx = sConfig.GetStringDefault("irc.command_prefix", ".");
    // Misc Options
    sIRC.games = sConfig.GetIntDefault("irc.fun.games", 0);
	sIRC.gmlog = sConfig.GetIntDefault("irc.gmlog", 1);
	sIRC.BOTMASK   = sConfig.GetIntDefault("BotMask", 0);
    sIRC.ojGM1 = sConfig.GetStringDefault("irc.gm1", "[Moderator]");
    sIRC.ojGM2 = sConfig.GetStringDefault("irc.gm2", "[Game Master]");
    sIRC.ojGM3 = sConfig.GetStringDefault("irc.gm3", "[BugTracker]");
    sIRC.ojGM4 = sConfig.GetStringDefault("irc.gm4", "[DevTeam Admin]");
    sIRC.ojGM5 = sConfig.GetStringDefault("irc.gm5", "[Root Admin]");
    // REQUIRED GM LEVEL}
    QueryResult *result = WorldDatabase.PQuery("SELECT `Command`, `gmlevel` FROM `IRC_Commands` ORDER BY `Command`");
    if(result)
    {
        Field *fields = result->Fetch();
        for (uint64 i=0; i < result->GetRowCount(); i++)
        {
            std::string command = fields[0].GetCppString();
            uint32 gmlvl = fields[1].GetUInt32();
            if(command == "acct") sIRC.CACCT = gmlvl;
            if(command == "fun") sIRC.CFUN = gmlvl;
            if(command == "item") sIRC.CITEM = gmlvl;
            if(command == "jail") sIRC.CJAIL = gmlvl;
            if(command == "kick") sIRC.CKICK = gmlvl;
            if(command == "kill") sIRC._KILL = gmlvl;
            if(command == "level") sIRC.CLEVEL = gmlvl;
            if(command == "money") sIRC.CMONEY = gmlvl;
            if(command == "mute") sIRC.CMUTE = gmlvl;
            if(command == "player") sIRC.CPLAYER = gmlvl;
            if(command == "pm") sIRC.CPM = gmlvl;
            if(command == "restart") sIRC.CRESTART = gmlvl;
            if(command == "revive") sIRC.CREVIVE = gmlvl;
            if(command == "saveall") sIRC.CSAVEALL = gmlvl;
            if(command == "shutdown") sIRC.CSHUTDOWN = gmlvl;
            if(command == "spell") sIRC.CSPELL = gmlvl;
            if(command == "sysmsg") sIRC.CSYSMSG = gmlvl;
            if(command == "tele") sIRC.CTELE = gmlvl;
            if(command == "who") sIRC.CWHO = gmlvl;
            result->NextRow();
        }
        delete result;
    }
    else
    {
        sIRC.CACCT     = 3;
		sIRC.CFUN      = 3;
        sIRC.CITEM     = 3;
        sIRC.CJAIL     = 3;
        sIRC.CKICK     = 3;
        sIRC._KILL     = 3;
        sIRC.CLEVEL    = 3;
        sIRC.CMONEY    = 3;
        sIRC.CMUTE     = 3;
        sIRC.CPLAYER   = 3;
        sIRC.CPM       = 3;
        sIRC.CRESTART  = 3;
        sIRC.CREVIVE   = 3;
        sIRC.CSAVEALL  = 3;
        sIRC.CSHUTDOWN = 3;
        sIRC.CSPELL    = 3;
        sIRC.CSYSMSG   = 3;
        sIRC.CTELE     = 3;
        sIRC.CWHO      = 3;
    }
    return true;
}

std::string IRCClient::GetChatLine(int nItem)
{
    return sIRC.ILINES[nItem];
}

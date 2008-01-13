#include "IRCCmd.h"
#include "IRCClient.h"
#include "Database/DatabaseEnv.h"
// Constructor
IRCCmd::IRCCmd(){}
// Destructor
IRCCmd::~IRCCmd(){}
bool IRCCmd::ParamsValid(_CDATA *CD, int pCnt)
{
	CD->PCOUNT = pCnt;
	if(CD->PARAMS.size() == 0)
		return false;
	return ValidParams(CD->PARAMS, pCnt);
}
int IRCCmd::ParamsValid(_CDATA *CD, int pCnt, int rLev)
{
	//CD->PCOUNT = pCnt;
	if(!CanUse(CD->USER, rLev))
		return E_AUTH;
	if(pCnt == 0)
		return E_OK;
        if(CD->PARAMS.size() == 0)
		return E_SIZE;
	if(!ValidParams(CD->PARAMS, pCnt))
		return E_IVALID;
	return E_OK;
}
// This function checks if chat from irc is a command or not
// return true on yes and false on no
bool IRCCmd::IsValid(std::string USER, std::string FROM, std::string CHAT)
{
	// If the first line of our chat is the command prefix we have a command
	if(CHAT.substr(0, 1) == sIRC._cmd_prefx && CHAT.size() > 1)
	{
		_CDATA CDATA;
		bool cValid    = false;
		bool AuthValid = true;
		std::string* _PARAMS = getArray(CHAT, 2);
		CDATA.USER		= USER;
		CDATA.FROM		= FROM;
		CDATA.PCOUNT	= 0;
		CDATA.CMD		= _PARAMS[0].substr(1, _PARAMS[0].size() - 1);
		CDATA.PARAMS	        = _PARAMS[1];
		if(CDATA.CMD == "login")
		{
			if(FROM == sIRC._Nick)
			{
				if(ParamsValid(&CDATA, 2))
					Handle_Login(&CDATA);
				else
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"login <Player> <Password> )", true);
			}
			else
				sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Please Send A PM To Login!", true, MSG_NOTICE);
			cValid = true;
		}
		else if(CDATA.CMD == "logout")
		{
			if(FROM == sIRC._Nick)
                        {
                            Handle_Logout(&CDATA);
                        }
                        else
				sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Please Send A PM To Logout!", true, MSG_NOTICE);
			cValid = true;
		}
		else if(CDATA.CMD == "fun")
		{
			switch(ParamsValid(&CDATA, 2, sIRC.CFUN))
			{
				case E_OK:
					Fun_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"fun <Player> <Sound/Say> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
        else if(CDATA.CMD == "help")
		{
			CDATA.PCOUNT = 2;
			Help_IRC(&CDATA);
			cValid = true;
		}
        else if(CDATA.CMD == "inchan")
		{
			CDATA.PCOUNT = 1;
			Inchan_Server(&CDATA);
			cValid = true;
		}
		else if(CDATA.CMD == "info")
		{
			Info_Server(&CDATA);
			cValid = true;
		}
		else if(CDATA.CMD == "item")
		{
			CDATA.PCOUNT = 3;
			switch(ParamsValid(&CDATA, 1, sIRC.CITEM))
			{
				case E_OK:
					Item_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"item <Player> <add> <ItemID/[ItemName]> <Amount> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "jail")
		{
			CDATA.PCOUNT = 3;
			switch(ParamsValid(&CDATA, 1, sIRC.CJAIL))
			{
				case E_OK:
					Jail_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"jail <Player> <release/Reason>)", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "kick")
		{
			CDATA.PCOUNT = 2;
			switch(ParamsValid(&CDATA, 1, sIRC.CKICK))
			{
				case E_OK:
					Kick_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"kick <Player> <Reason> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "kill")
		{
			CDATA.PCOUNT = 2;
			switch(ParamsValid(&CDATA, 1, sIRC._KILL))
			{
				case E_OK:
					Kill_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"kill <Player> <Reason> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "level")
		{
			CDATA.PCOUNT = 2;
			switch(ParamsValid(&CDATA, 1, sIRC.CLEVEL))
			{
				case E_OK:
					Level_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"level <Player> <NewLevel> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "money")
		{
			CDATA.PCOUNT = 2;
			switch(ParamsValid(&CDATA, 1, sIRC.CMONEY))
			{
				case E_OK:
					Money_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"money <Player> <(-)Money> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "mute")
		{
			switch(ParamsValid(&CDATA, 2, sIRC.CMUTE))
			{
				case E_OK:
					Mute_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"mute <Player> <release/TimeInMins> <Reason> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "online")
		{
			CDATA.PCOUNT = 3;
			Online_Players(&CDATA);
			cValid = true;
		}
		else if(CDATA.CMD == "player")
		{
			switch(ParamsValid(&CDATA, 1, sIRC.CPLAYER))
			{
				case E_OK:
					Player_Info(&CDATA);
					break;
          		case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"player <Player> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "pm")
		{
			switch(ParamsValid(&CDATA, 2, sIRC.CPM))
			{
				case E_OK:
					PM_Player(&CDATA);
					break;
	  			case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"pm <Player> <Message> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "restart")
		{
			switch(ParamsValid(&CDATA, 0, sIRC.CRESTART))
			{
				case E_OK:
					sIRC.Send_IRC_Channels(sIRC.RstMsg);
					sIRC.ResetIRC();
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "revive")
		{
			CDATA.PCOUNT = 2;
			switch(ParamsValid(&CDATA, 1, sIRC.CREVIVE))
			{
				case E_OK:
					Revive_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"revive <Player> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "saveall")
		{
			switch(ParamsValid(&CDATA, 0, sIRC.CSAVEALL))
			{
				case E_OK:
					Saveall_Player(&CDATA);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "shutdown")
		{
			switch(ParamsValid(&CDATA, 1, sIRC.CSHUTDOWN))
			{
				case E_OK:
					Shutdown_Mangos(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"shutdown <TimeInSeconds> )", true, MSG_NOTICE);
					break;				
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
  		else if(CDATA.CMD == "spell")
		{
			switch(ParamsValid(&CDATA, 2, sIRC.CSPELL))
			{
				case E_OK:
					Spell_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"spell <Player> <Cast/Learn/UnLearn> <SpellID> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "sysmsg")
		{
			CDATA.PCOUNT = 2;
			switch(ParamsValid(&CDATA, 1, sIRC.CSYSMSG))
			{
				case E_OK:
					Sysmsg_Server(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"sysmsg <a/n> <Message> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "tele")
		{
			switch(ParamsValid(&CDATA, 2, sIRC.CTELE))
			{
				case E_OK:
					Tele_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"tele <Player> <l/c/r/to> <Loc.Name/MAPID X Y Z/Recall/Player> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
        else if(CDATA.CMD == "who")
		{
			switch(ParamsValid(&CDATA, 0, sIRC.CWHO))
			{
				case E_OK:
					Who_Logged(&CDATA);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "zbuff")
		{
			switch(ParamsValid(&CDATA, 1, sIRC.CZBUFF))
			{
				case E_OK:
					Zbuff_Player(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"zbuff <Player> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		else if(CDATA.CMD == "lang")
		{
			switch(ParamsValid(&CDATA, 1, 3))
			{
				case E_OK:
					change_lang(&CDATA);
					break;
				case E_SIZE:
					sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"lang <language> )", true, MSG_NOTICE);
					break;
				case E_AUTH:
					AuthValid = false;
					break;
			}
			cValid = true;
		}
		if(!AuthValid && IsLoggedIn(USER))
			sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Access Denied! Your Security Level Is Too Low To Use This Command!", true, MSG_NOTICE);
		if(cValid == false && (sIRC.BOTMASK & 4) != 0)
			sIRC.Send_IRC_Channel(USER, " 4[ERROR] : Unknown Command!", true, MSG_NOTICE);
		return cValid;
	}
	return false;
}
bool IRCCmd::CanUse(std::string USER, int nLevel)
{
	if(IsLoggedIn(USER))
	{
		if(GetLevel(USER) >= nLevel)
			return true;
		else
			return false;
	}
	else
		sIRC.Send_IRC_Channel(USER, " 4[ERROR] : You Are Not Logged In!", true, MSG_NOTICE);
	return false;
}
std::string IRCCmd::ChanOrPM(_CDATA *CD)
{
	if(CD->FROM == sIRC._Nick)
		return CD->USER;
	else
		return CD->FROM;
}
Player *IRCCmd::GetPlayer(std::string WHO)
{
	normalizePlayerName(WHO);
	return ObjectAccessor::Instance().FindPlayerByName(WHO.c_str());
}
bool IRCCmd::IsLoggedIn(std::string USER)
{
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if((CLIENTS[i].LoggedIn) && (CLIENTS[i].Name == USER))
		{
			return true;
		}
	}
	return false;
}
int IRCCmd::GetLevel(std::string sName)
{
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if(CLIENTS[i].Name == sName)
		{
			return CLIENTS[i].GMLevel;
		}
	}
	return 0;
}
std::string IRCCmd::GetAccName(std::string sName)
{
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if(CLIENTS[i].Name == sName)
		{
			return CLIENTS[i].UName;
		}
	}
	return "";
}
bool IRCCmd::ValidParams(std::string PARAMS, int nCount)
{
	if(nCount == 1 && PARAMS.size() == 0)
		return false;
	int pcount = 0;
	size_t p = -1;
	for(int i = 0;i < nCount;i++)
	{
		p = PARAMS.find(" ", p + 1);
		if(p == -1)
			break;
		else
			pcount++;
	}
	nCount--;
	if(pcount >= nCount)
		return true;
	else
		return false;
}
std::string* IRCCmd::getArray(std::string PARAMS, int nCount)
{
	std::string *array = new std::string[nCount];
	if(PARAMS.size() > 0)
	{
		int pcnt = 0;
		size_t ps = 0;
		size_t pc = -1;
		for(int i = 0;i < nCount;i++)
		{
			pc = PARAMS.find(" ", pc + 1);
			if(i + 1 == nCount && nCount != 1)
			{
				if(ps > 0 && pc > 0)
					array[i] = PARAMS.substr(ps, PARAMS.size() - ps);
			}
			else
				array[i] = PARAMS.substr(ps, pc - ps);
			ps = pc + 1;
		}
	}
	return array;
}
std::string IRCCmd::MakeMsg(const char *sLine, ... )
{
    va_list ap;
    char tmpoutp[1024];
	va_start(ap, sLine);
	vsnprintf(tmpoutp, 1024, sLine, ap );
    va_end(ap);
	std::string outp = tmpoutp;
	return outp;
}

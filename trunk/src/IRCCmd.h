#ifndef _IRC_CMD_H
#define _IRC_CMD_H
#define MAX_CLIENTS 50
#include "Common.h"
#include "../Player.h"
#include "../ObjectAccessor.h"
struct _client
{
	bool		LoggedIn;
	std::string Name;
	std::string UName;
	int		GMLevel;
};
struct _CDATA
{
	std::string CMD;
	std::string USER;
	std::string FROM;
	std::string PARAMS;
	int PCOUNT;
};
enum APVERR
{
	E_OK,
	E_SIZE,
	E_AUTH,
	E_IVALID,
};
enum ESOUNDS
{
	S_ENTERWORLD	        = 602,
	S_QUESTFAILED	        = 847,
	S_INVITE		= 880,
	S_LEVELUP		= 888,
	S_COINSOUND		= 895,
	S_WHISPER		= 3081,
	S_STEALTH		= 3325,
};
class IRCCmd
{
public:
        IRCCmd();
        ~IRCCmd();
	bool	IsValid(std::string USER, std::string FROM, std::string CHAT);
	void	Handle_Logout(_CDATA *CD);
	bool	IsLoggedIn(std::string USER);

public:
	static std::string MakeMsg(const char *sLine, ... );
	static std::string ChanOrPM(_CDATA *CD);

private:
	void	Handle_Login(_CDATA *CD);
	void	Online_Players(_CDATA *CD);
	int	GetLevel(std::string sName);
        // InGame Commands
	void	Fun_Player(_CDATA *CD);
	void	Help_IRC(_CDATA *CD);
    void	Item_Player(_CDATA *CD);
	void	Inchan_Server(_CDATA *CD);
    void	Info_Server(_CDATA *CD);
	void	Jail_Player(_CDATA *CD);
	void	Kick_Player(_CDATA *CD);
	void	Kill_Player(_CDATA *CD);
	void	Level_Player(_CDATA *CD);
	void	Money_Player(_CDATA *CD);
	void	Mute_Player(_CDATA *CD);
	void	Player_Info(_CDATA *CD);
	void	PM_Player(_CDATA *CD);
	void	Revive_Player(_CDATA *CD);
	void	Saveall_Player(_CDATA *CD);
	void	Shutdown_Mangos(_CDATA *CD);
	void	Spell_Player(_CDATA *CD);
    void	Sysmsg_Server(_CDATA *CD);
    void	Tele_Player(_CDATA *CD);
	void	Who_Logged(_CDATA *CD);
	void	Zbuff_Player(_CDATA *CD);
	bool    CanUse(std::string USER, int nLevel);
	bool    ValidParams(std::string PARAMS, int nCount = 1);
	bool    ParamsValid(_CDATA *CD, int pCnt);
	int     ParamsValid(_CDATA *CD, int pCnt, int rLev);
	Player* GetPlayer(std::string WHO);
    std::string GetAccName(std::string sName);
	std::string* getArray(std::string PARAMS, int nCount = 1);
	_client CLIENTS[MAX_CLIENTS];
};
#endif

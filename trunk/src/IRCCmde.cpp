#include "IRCCmd.h"
#include "IRCClient.h"
#include "WorldPacket.h"
#include "../Chat.h"
#include "../MapManager.h"
#include "Database/DatabaseEnv.h"
#include "../World.h"
#include "../Guild.h"
#include "../ObjectMgr.h"
#include "../Language.h"
#include "../SpellAuras.h"
#include "../Config/ConfigEnv.h"

#define Send_Player(p, m)	        sIRC.Send_WoW_Player(p, m)
#define Send_IRC(c, m, b)		sIRC.Send_IRC_Channel(c, m, b)
#define Send_IRCA(c, m, b, t)		sIRC.Send_IRC_Channel(c, m, b, t)
#ifdef WIN32
 #define Delay(x) Sleep(x)
#else
 #define Delay(x) sleep(x / 1000)
#endif
void IRCCmd::Handle_Login(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 2);
	if(!IsLoggedIn(CD->USER))
	{
		QueryResult *result = loginDatabase.PQuery("SELECT `gmlevel` FROM `account` WHERE `username`='%s' AND `I`=SHA1(CONCAT(UPPER(`username`),':',UPPER('%s')));", _PARAMS[0].c_str(), _PARAMS[1].c_str());
		if (result)
		{
			Field *fields = result->Fetch();
			int GMLevel = fields[0].GetInt16();
			if(GMLevel >= 0)
			{
				for(int i = 0;i < MAX_CLIENTS;i++)
				{
					if(!CLIENTS[i].LoggedIn)
					{
						CLIENTS[i].LoggedIn = true;
						CLIENTS[i].Name     = CD->USER;
						CLIENTS[i].UName    = _PARAMS[0];
						CLIENTS[i].GMLevel  = fields[0].GetInt16();
						Send_IRC(ChanOrPM(CD), MakeMsg("You Are Now Logged In As %s, Welcome To MangChat Admin Mode.", _PARAMS[0].c_str()), true);
						break;
					}
				}
			}
			else
			Send_IRC(ChanOrPM(CD), " 4[ERROR] : Access Denied!", true);
		    delete result;
		}
		else
		Send_IRC(ChanOrPM(CD), " 4[ERROR] : Login Failed!", true);
	}
	else
	Send_IRC(CD->USER, " 4[ERROR] : You Are Already Logged In As "+ _PARAMS[0] +"!", true);
}
void IRCCmd::Handle_Logout(_CDATA *CD)
{
	if(IsLoggedIn(CD->USER))
	{
		for(int i = 0;i < MAX_CLIENTS;i++)
		{
			if((CLIENTS[i].LoggedIn) && (CLIENTS[i].Name == CD->USER))
			{
				CLIENTS[i].GMLevel = -1;
				CLIENTS[i].Name = "-";
				CLIENTS[i].LoggedIn = false;
				Send_IRC(CD->USER, "You Are Now Logged Out!", true);
				break;
			}
		}
	}
	else
	{
		Send_IRC(CD->USER, " 4[ERROR] : You Are Not Logged In!", true);
	}
}
void IRCCmd::Fun_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 3);		
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{    
	    if(_PARAMS[1] == "say")
		{
			plr->Say(_PARAMS[2], LANG_UNIVERSAL);
		}
		if(_PARAMS[1] == "sound")
		{
			uint32 sndid = atoi(_PARAMS[2].c_str());
			plr->PlaySound(sndid ,true);
		}
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : Player Not Online!", true, MSG_NOTICE);
}
void IRCCmd::Help_IRC(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 1);
	QueryResult *result = WorldDatabase.PQuery("SELECT `Command`, `Description`, `gmlevel` FROM `IRC_Commands`");
	if(result)
	{
		if(IsLoggedIn(CD->USER))
		{
			if(_PARAMS[0] == "")
			{
				QueryResult *result = WorldDatabase.PQuery("SELECT * FROM `IRC_Commands` WHERE `gmlevel` <= %u ORDER BY `Command`", GetLevel(CD->USER));
				if(result)
				{
					std::string output = "MangChat IRC Commands: ";
					for (uint64 i=0; i < result->GetRowCount(); i++)
					{
						Field *fields = result->Fetch();
						output += fields[0].GetCppString() + ", ";
						result->NextRow();
					}
					delete result;
					Send_IRCA(CD->USER, output, true, MSG_NOTICE);
				}	
			}
			else
			{
				QueryResult *result = WorldDatabase.PQuery("SELECT `Description`, `gmlevel` FROM `IRC_Commands` WHERE `Command` = '%s'", _PARAMS[0].c_str());
				if(result)
				{
					Field *fields = result->Fetch();			
					if(fields[1].GetUInt32() > GetLevel(CD->USER))
					{	
						Send_IRCA(CD->USER, " You Do Not Have Access To That Command, So No Help Is Available.", true, MSG_NOTICE);
						return;
					}
					if(result)
					{
						std::string cmdhlp = fields[0].GetCppString();
						delete result;
						Send_IRCA(CD->USER, cmdhlp, true, MSG_NOTICE);
					}		
				}
				else
				Send_IRCA(CD->USER, " 4[ERROR] : No Such Command Exists, Please Check The Spelling And Try Again.", true, MSG_NOTICE);
			}
		}
		else if(!IsLoggedIn(CD->USER))
		{
			if(_PARAMS[0] == "")
			{
				QueryResult *result = WorldDatabase.PQuery("SELECT * FROM `IRC_Commands` WHERE `gmlevel` = 0 ORDER BY `Command`");
				if(result)
				{
					std::string output = "MangChat IRC Commands: ";
					for (uint64 i=0; i < result->GetRowCount(); i++)
					{
						Field *fields = result->Fetch();
						output += fields[0].GetCppString() + ", ";
						result->NextRow();
					}
					delete result;
					Send_IRCA(CD->USER, output, true, MSG_NOTICE);
					Send_IRCA(CD->USER, "You Are Currently Not Logged In, Please Login To See A Complete List Of Commands Available To You.", true, MSG_NOTICE);
				}
			}
			else
			{
				QueryResult *result = WorldDatabase.PQuery("SELECT `Description`, `gmlevel` FROM `IRC_Commands` WHERE `Command` = '%s'", _PARAMS[0].c_str());			
				if(result)
				{
					Field *fields = result->Fetch();
					if(fields[1].GetUInt32() > 0)
					{	
						Send_IRCA(CD->USER, " You Do Not Have Access To That Command, So No Help Is Available.", true, MSG_NOTICE);
						return;
					}
					std::string cmdhlp = fields[0].GetCppString();
					delete result;
					Send_IRCA(CD->USER, cmdhlp, true, MSG_NOTICE);
				}
				else
				Send_IRCA(CD->USER, " 4[ERROR] : No Such Command Exists, Please Check The Spelling And Try Again.", true, MSG_NOTICE);
			}
		}
	}
	else
	Send_IRC(ChanOrPM(CD), " 4[ERROR] : Database Error! Please Make Sure You Used IRC_Commands.sql, You Must Have A Table In Your World Database (IRC_Commands)!", true);
}
void IRCCmd::Inchan_Server(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 1);
	if(_PARAMS[0] == "")
	{
		Send_IRCA(CD->USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"inchan <ChannelName> )", true, MSG_NOTICE);
		return;
	}
	QueryResult *result = WorldDatabase.PQuery("SELECT * FROM `IRC_Inchan` WHERE `channel` = '%s' ORDER BY `name`", _PARAMS[0].c_str());
	if(result)
	{
	    Field *fields = result->Fetch();
		std::string output = "Players In The [ "+fields[2].GetCppString()+" ] Channel: ";
		for (uint64 i=0; i < result->GetRowCount(); i++)
		{
			output += fields[1].GetCppString() + ", ";
			result->NextRow();
		}
		delete result;
		Send_IRC(ChanOrPM(CD), output, true);
	}
	else
	Send_IRCA(CD->USER, "No Players Are Currently In [ "+_PARAMS[0]+" ] Channel!", true, MSG_NOTICE);
}
void IRCCmd::Info_Server(_CDATA *CD)
{
        std::string* _PARAMS = getArray(CD->PARAMS, 1);
        char clientsNum [50];
        sprintf(clientsNum, "%u", sWorld.GetActiveSessionCount());
        char maxClientsNum [50];
        sprintf(maxClientsNum, "%u", sWorld.GetMaxSessionCount());
        std::string str = secsToTimeString(sWorld.GetUptime());
        Send_IRC(ChanOrPM(CD), " Number Of Players Online: " + (std::string)clientsNum + ". (Max Since Last Restart: " + (std::string)maxClientsNum + ")" + " Uptime: " + str, true);
}
void IRCCmd::Item_Player(_CDATA *CD)
{
    std::string* _PARAMS = getArray(CD->PARAMS, 3);
    std::string player  = _PARAMS[0];
    normalizePlayerName(player);
    uint64 guid = objmgr.GetPlayerGUIDByName(player.c_str());
    Player *chr = objmgr.GetPlayer(guid);
	if(_PARAMS[1] == "add")
	{
	    std::string s_param  = _PARAMS[2];
		char *args = (char*)s_param.c_str();
		uint32 itemId = 0;
	    if(args[0]=='[')
	    {
	        char* citemName = citemName = strtok((char*)args, "]");
	        if(citemName && citemName[0])
	        {
	            std::string itemName = citemName+1;
	            WorldDatabase.escape_string(itemName);
	            QueryResult *result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
	            if (!result)
	            {
	                Send_IRCA(CD->USER, " 4[ERROR] : Item Not Found!", true, MSG_NOTICE);
	                return;
	            }
	            itemId = result->Fetch()->GetUInt16();
	            delete result;
	        }
	        else
	        {
	            Send_IRCA(CD->USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"item <Player> <add> [Exact Item Name] <Amount> )", true, MSG_NOTICE);
	            return;
		    }
	    }
	    else
	    {
	        std::string itemName = s_param;
	        WorldDatabase.escape_string(itemName);
	        QueryResult *result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
	        if (result)
	        {
	            itemId = result->Fetch()->GetUInt16();
	        }
	        delete result;
	
	        char* cId = strtok(args, " ");
	        if(!cId)
	        {
	            Send_IRCA(CD->USER, " 4[ERROR] : Syntax Error! ( "+sIRC._cmd_prefx+"item <Player> <add> <ItemID> <Amount> )", true, MSG_NOTICE);
	            return;
	        }
	        itemId = atol(cId);
		}
	    char* ccount = strtok(NULL, " ");
	    int32 count = 1;
	    if (ccount) { count = atol(ccount); }
	    if (count < 1) { count = 1; }
	    Player* plTarget = chr;
	    if ( !plTarget )
	    {
			Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Not Online!", true, MSG_NOTICE);
			return;
		}
		sLog.outDetail(LANG_ADDITEM, itemId, count);
		ItemPrototype const *pProto = objmgr.GetItemPrototype(itemId);
		if(!pProto)
	    {
	        Send_IRCA(CD->USER, " 4[ERROR] : Invalid Item!", true, MSG_NOTICE);
	        return;
	    }
	    uint32 countForStore = count;
	    // item used in local operations and in add item notifier
	    Item* item = NULL;
	    // if possible create full stacks for better performance
	    while(countForStore >= pProto->Stackable)
	    {
			uint16 dest;
	        uint8 msg = plTarget->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, pProto->Stackable, false );
	        if( msg == EQUIP_ERR_OK )
	        {
	            item = plTarget->StoreNewItem( dest, itemId, pProto->Stackable, true, Item::GenerateItemRandomPropertyId(itemId));
	            countForStore-= pProto->Stackable;
	        }
	        else
	            break;
	    }
	    // create remaining items
	    if(countForStore > 0 && countForStore < pProto->Stackable)
	    {
	        uint16 dest;
	        uint8 msg = plTarget->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, countForStore, false );
	        // if can add all countForStore items
	        if( msg == EQUIP_ERR_OK )
	        {
	            item = plTarget->StoreNewItem( dest, itemId, countForStore, true, Item::GenerateItemRandomPropertyId(itemId));
	            countForStore = 0;
	        }
		}
	    // ok search place for add only part from countForStore items in not full stacks
	    while(countForStore > 0)
	    {
	        // find not full stack (last possable place for times after prev. checks)
	        uint16 dest;
	        uint8 msg = plTarget->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, 1, false );
	        if( msg == EQUIP_ERR_OK )                           // found
	        {
	            // we can fill this stack to max stack size
	            Item* itemStack = plTarget->GetItemByPos(dest);
	            if(itemStack)
	            {
	                uint32 countForStack = pProto->Stackable - itemStack->GetCount();
	                // recheck with real item amount
	                uint8 msg = plTarget->CanStoreNewItem( itemStack->GetBagSlot(), itemStack->GetSlot(), dest, itemId, countForStack, false );
	                if( msg == EQUIP_ERR_OK )
	                {
	                    item = plTarget->StoreNewItem( dest, itemId, countForStack, true, Item::GenerateItemRandomPropertyId(itemId));
	                    countForStore-= countForStack;
	                }
	                else
	                    break;                                  // not possible with correct work
	            }
	            else
	                break;                                      // not possible with correct work
	        }
	        else
	            break;
	    }
	    if(uint32(count) > countForStore && item)
	    {
	        plTarget->SendNewItem(item,count - countForStore,true,false);
	        QueryResult *result = WorldDatabase.PQuery("SELECT name FROM item_template WHERE entry = %d", itemId);
			char* dbitemname = NULL;
			if (result)
			{
		        dbitemname = (char*)result->Fetch()->GetString();
	        }
		    std::string iinfo = " 13[" +player+ "] : Has Been Given Item "+dbitemname+". From: "+CD->USER.c_str()+".";
	        Send_IRC(ChanOrPM(CD), iinfo, true);
	        delete result;
		}
		if(countForStore > 0)
		{
			char s_countForStore[255];
			sprintf(s_countForStore,"%d",countForStore);
		    std::string ierror = " 13["+player+"] : Could Not Create All Items! " +s_countForStore+ " Item(s) Were Not Created!";
	        Send_IRC(ChanOrPM(CD), ierror, true);
	    }
	}
}
void IRCCmd::Jail_Player(_CDATA *CD)
{
	if(ValidParams(CD->PARAMS, 1))
	{
		std::string* _PARAMS = getArray(CD->PARAMS, 2);
		if (Player* plr = GetPlayer(_PARAMS[0]))
		{
			uint64 guid = objmgr.GetPlayerGUIDByName(_PARAMS[0]);
			std::string sReason = "";
			if(_PARAMS[1] == "release") 
			{
				 float rmapid, rposx, rposy, rposz, rposo = 0;
				 CharacterDatabase.escape_string(_PARAMS[0]);  
		         QueryResult *result = CharacterDatabase.PQuery( "SELECT `map`, `position_x`, `position_y`, `position_z` FROM `character_homebind` WHERE `guid` = '" I64FMTD "'", (uint64)guid );
					if(result) 
					{
						Field *fields = result->Fetch();
						rmapid = fields[0].GetFloat();
						rposx = fields[1].GetFloat();
						rposy = fields[2].GetFloat();
						rposz = fields[3].GetFloat();
						delete result; 
						plr->SetMovement(MOVE_UNROOT);
						plr->TeleportTo(rmapid, rposx, rposy, rposz, rposo);
						plr->RemoveAurasDueToSpell(42201);
						plr->RemoveAurasDueToSpell(23775);
						plr->RemoveAurasDueToSpell(9454);
						Send_Player(plr, MakeMsg("You Have Been Released By: %s.", CD->USER.c_str()));
						sReason = " 13["+_PARAMS[0]+"] : Has Been Released By: "+CD->USER+".";
						Send_IRC(ChanOrPM(CD), sReason, true);
					}
			}			
			else
			{
				if(_PARAMS[1] == "")
					_PARAMS[1] = "No Reason Given.";
				plr->TeleportTo(13, 13, 13, 13, 0, true, true);
				plr->SetMovement(MOVE_ROOT);
				plr->CastSpell(plr, 42201, true);
				plr->CastSpell(plr, 23775, true);
				plr->CastSpell(plr, 9454, true);
				Send_Player(plr, MakeMsg("You Have Been Jailed By: %s. Reason: %s.", CD->USER.c_str(), _PARAMS[1].c_str()));
				sReason = " 13["+_PARAMS[0]+"] : Has Been Jailed By: "+CD->USER+". Reason: "+_PARAMS[1]+".";
				Send_IRC(ChanOrPM(CD), sReason, true);
			}
		}
		else
		Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Not Online!", true, MSG_NOTICE);
	}
}
void IRCCmd::Kick_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, CD->PCOUNT);
	if(_PARAMS[1] == "")
		_PARAMS[1] = "No Reason Given.";
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{
		plr->GetSession()->KickPlayer();
		Send_IRC(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Been Kicked By: "+CD->USER+". Reason: "+_PARAMS[1]+".", true);
	}
	else
		Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Not Online!", true, MSG_NOTICE);
}
void IRCCmd::Kill_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, CD->PCOUNT);
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{
		if(plr->isAlive())
		{
			plr->DealDamage(plr, plr->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_NORMAL, NULL, 0, false);
			plr->SaveToDB();
        		if(_PARAMS[1] == "")
				_PARAMS[1] = "No Reason Given.";

			Send_IRC(ChanOrPM(CD), MakeMsg(" 13[%s] : Has Been Killed By: %s.", _PARAMS[0].c_str(), CD->USER.c_str()) +  +  + " Reason: "+_PARAMS[1]+".", true);
			Send_Player(plr, MakeMsg("You Have Been Killed By: %s. Reason: %s.", CD->USER.c_str(), _PARAMS[1].c_str()));
		}
		else
		Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Already Dead!", true, MSG_NOTICE);
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Not Online!", true, MSG_NOTICE);
}
void IRCCmd::Level_Player(_CDATA *CD)
{
    std::string* _PARAMS = getArray(CD->PARAMS, CD->PCOUNT);
    std::string player  = _PARAMS[0];
    normalizePlayerName(player);
    uint64 guid = objmgr.GetPlayerGUIDByName(player.c_str());
    std::string s_newlevel  = _PARAMS[1];
    uint8 i_newlvl = atoi(s_newlevel.c_str());
    if(!guid)
    {
        Send_IRCA(CD->USER, " 4[ERROR] : Player Not Found!", true, MSG_NOTICE);
        return;
    } else if ( i_newlvl < 1 || i_newlvl > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL) )
    {
        Send_IRCA(CD->USER, MakeMsg(" 4[ERROR] : Level Must Be Between 1 And %i!",sConfig.GetIntDefault("MaxPlayerLevel", 70)), true, MSG_NOTICE);
        return;
    } else
    {
        Player *chr = objmgr.GetPlayer(guid);
        int32 i_oldlvl = chr ? chr->getLevel() : Player::GetUInt32ValueFromDB(UNIT_FIELD_LEVEL,guid);
        if(chr)
        {
            chr->GiveLevel(i_newlvl);
            chr->InitTalentForLevel();
            chr->SetUInt32Value(PLAYER_XP,0);
            WorldPacket data;
			ChatHandler CH(chr->GetSession());
            if(i_oldlvl == i_newlvl)
				CH.FillSystemMessageData(&data, LANG_YOURS_LEVEL_PROGRESS_RESET);
            else
            if(i_oldlvl < i_newlvl)
                CH.FillSystemMessageData(&data, fmtstring(LANG_YOURS_LEVEL_UP,i_newlvl-i_oldlvl));
            else
            if(i_oldlvl > i_newlvl)
                CH.FillSystemMessageData(&data, fmtstring(LANG_YOURS_LEVEL_DOWN,i_newlvl-i_oldlvl));
             chr->GetSession()->SendPacket( &data );
        }
        else
        {
            Tokens values;
            Player::LoadValuesArrayFromDB(values,guid);
            Player::SetUInt32ValueInArray(values,UNIT_FIELD_LEVEL,i_newlvl);
            Player::SetUInt32ValueInArray(values,PLAYER_XP,0);
            Player::SaveValuesArrayInDB(values,guid);
        }
    }
	Send_IRC(ChanOrPM(CD), " 13["+player+"] : Has Been Leveled To "+s_newlevel+". By: "+CD->USER+".", true);
}
void IRCCmd::Money_Player(_CDATA *CD)
{
    std::string* _PARAMS = getArray(CD->PARAMS, 2);
    std::string player  = _PARAMS[0];
    normalizePlayerName(player);
    uint64 guid = objmgr.GetPlayerGUIDByName(player.c_str());
    Player *chr = objmgr.GetPlayer(guid);

    std::string s_money  = _PARAMS[1];
    int32 money = atoi(s_money.c_str());
    unsigned int gold = money / 10000;
    unsigned int silv = (money % 10000) / 100;
    unsigned int cop = (money % 10000) % 100;
    char tempgold [100];
    sprintf(tempgold, "\x2\x3\x30\x37%ug \x3\x31\x34%us \x3\x30\x35%uc\xF", gold, silv, cop);
    if(!guid)
    {
        Send_IRCA(CD->USER, " 4[ERROR] : Player Not Found!", true, MSG_NOTICE);
    } 
	else
	{
		Player *chr = objmgr.GetPlayer(guid);
        CharacterDatabase.escape_string(player);  
		std::string sqlquery = "SELECT SUBSTRING_INDEX(SUBSTRING_INDEX(data, ' ' , 1325), ' ' , -1) AS `gold` FROM `character` WHERE `name` = '"+player+"';";
		QueryResult *result = CharacterDatabase.Query(sqlquery.c_str());
	  if(result) 
	  {
		Field *fields = result->Fetch();
		uint32 moneyuser = fields[0].GetInt32();
		delete result;     
		int32 addmoney = money;
        //uint32 moneyuser = chr->GetMoney();
        int32 newmoney = moneyuser + addmoney;
        char s_newmoney[255];
        sprintf(s_newmoney,"%d",newmoney);
		if(addmoney < 0)
        {
            sLog.outDetail(LANG_CURRENT_MONEY, moneyuser, addmoney, newmoney);
            if(newmoney <= 0 )
            {
				Send_IRC(ChanOrPM(CD), " 13["+player+"] : Has Had All Money Taken By: "+CD->USER.c_str()+".", true);
                if(chr) 
				{
					chr->SetMoney(0);
					Send_Player(chr, MakeMsg("You Have Been Liquidated By: %s. Total Money Is Now 0.", CD->USER.c_str()));
				}
				else
				CharacterDatabase.PExecute("UPDATE `character` SET data=concat(substring_index(data,' ',1325-1),' ','%u',' ', right(data,length(data)-length(substring_index(data,' ',1325))-1) ) where guid='%u'",newmoney, guid );
            }
            else
            {
				Send_IRC(ChanOrPM(CD), " 13["+player+"] : Has Had ("+tempgold+"13) Taken From Them By: "+CD->USER.c_str()+".", true);
                if(chr) 
				{
					chr->SetMoney( newmoney );
					Send_Player(chr, MakeMsg("You Have Had %s Copper Taken From You By: %s.", _PARAMS[1].c_str(), CD->USER.c_str()));
				}
				else
				CharacterDatabase.PExecute("UPDATE `character` SET data=concat(substring_index(data,' ',1325-1),' ','%u',' ', right(data,length(data)-length(substring_index(data,' ',1325))-1) ) where guid='%u'",newmoney, guid );
            }
        }
        else
        {
            Send_IRC(ChanOrPM(CD), " 13["+player+"] : Has Been Given ("+tempgold+"13) From: "+CD->USER.c_str()+".", true);
            if(chr) 
				{
					chr->ModifyMoney( addmoney );
					Send_Player(chr, MakeMsg("You Have Been Given %s Copper. From: %s.", _PARAMS[1].c_str(), CD->USER.c_str()));
				}				
			else
			CharacterDatabase.PExecute("UPDATE `character` SET data=concat(substring_index(data,' ',1325-1),' ','%u',' ', right(data,length(data)-length(substring_index(data,' ',1325))-1) ) where guid='%u'",newmoney, guid );
		}
	   }
	  else
	  Send_IRCA(CD->USER, " 4[ERROR] : No Result!", true, MSG_NOTICE);
	}
}
void IRCCmd::Mute_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 3);
	normalizePlayerName(_PARAMS[0]);
	uint64 guid = objmgr.GetPlayerGUIDByName(_PARAMS[0]);
	if(guid)
	{
		if(_PARAMS[1] == "release")
		{
            Player* plr = objmgr.GetPlayer(guid);
			uint32 account_id = 0;
			account_id = objmgr.GetPlayerAccountIdByGUID(guid);
			// Send_IRC(ChanOrPM(CD), " 13 [ T E S T ]" ,true);
			loginDatabase.PExecute("UPDATE `account` SET `mutetime` = '0' WHERE `id` = '%u'", account_id );
			Send_IRC(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Been UnMuted By: "+CD->USER+"." ,true);
			if(plr) 
			{
				plr->GetSession()->m_muteTime = 0;
				Send_Player(plr, MakeMsg("You Have Been UnMuted By: %s.", CD->USER.c_str()));
			}
		}
		else
		{
			if(_PARAMS[2] == "")
				_PARAMS[2] = "No Reason Given";			
			Player* plr = objmgr.GetPlayer(guid);
			time_t mutetime = time(NULL) + atoi(_PARAMS[1].c_str())*60;
            uint32 account_id = 0;
			account_id = objmgr.GetPlayerAccountIdByGUID(guid);
            if(plr) plr->GetSession()->m_muteTime = mutetime;
            loginDatabase.PExecute("UPDATE `account` SET `mutetime` = " I64FMTD " WHERE `id` = '%u'",uint64(mutetime), account_id );
			Send_IRC(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Been Muted By: "+CD->USER+". For: "+_PARAMS[1]+" Minutes. Reason: "+_PARAMS[2] ,true);
			if(plr) Send_Player(plr, MakeMsg("You Have Been Muted By: %s. For: %s Minutes. Reason: %s", CD->USER.c_str(), _PARAMS[1].c_str(), _PARAMS[2].c_str()));
		}
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : Player Does Not Exist!", true, MSG_NOTICE);
}

#include "MCS_OnlinePlayers.h"

void IRCCmd::Online_Players(_CDATA *CD)
{
	if(!sIRC.Script_Lock[MCS_Players_Online])
	{
		sIRC.Script_Lock[MCS_Players_Online] = true;
		ZThread::Thread script(new mcs_OnlinePlayers(CD));
	}

	/*
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
			
			if(plr->isAFK())
				ChatTag.append("7<AFK>");
			if(plr->isDND())
				ChatTag.append("7<DND>");
			if(itr->second->GetSession()->GetSecurity() > sConfig.GetIntDefault("OnlineGM", 2))
				ChatTag.append("9<GM>");
			if(itr->second->GetSession()->GetSecurity() > 0)
				ChatTag.append("");
            
			switch (plr->GetTeam())
            {
				case 67:ChatTag.append("4");break; //horde
                case 469:ChatTag.append("12");break; //alliance
            }
			IRCOut.append(MakeMsg("%s%s(%d)", ChatTag.c_str(), plr->GetName(), plr->getLevel()));

			if(OnlineCount % 10 == 0)
			{
				Send_IRC(ChanOrPM(CD), MakeMsg(" %s", IRCOut.c_str()), true);
				IRCOut = "";
			}
        }
    }
	// Remainder in IRCOUT && Total plyersonline
	Send_IRC(ChanOrPM(CD), MakeMsg(" %s Players Online(%d)", IRCOut.c_str(), OnlineCount), true);
	*/
}
void IRCCmd::Player_Info(_CDATA *CD)
{
    std::string* _PARAMS = getArray(CD->PARAMS, 1);
    std::string pname = _PARAMS[0];
    uint64 guid = objmgr.GetPlayerGUIDByName(pname);
    CharacterDatabase.escape_string(pname);  
	std::string sqlquery = "SELECT `guid`, `account`, `name`, `race`, `class`, `online`, SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ' , 35), ' ' , -1) AS `level`, SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ' , 236), ' ' , -1) AS `guildid`, SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ' , 237), ' ' , -1) AS `guildrank`, SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ' , 863), ' ' , -1) AS `xp`, SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ' , 864), ' ' , -1) AS `maxxp`, SUBSTRING_INDEX(SUBSTRING_INDEX(data, ' ' , 1333), ' ' , -1) AS `gold`, SUBSTRING_INDEX(SUBSTRING_INDEX(`data`, ' ' , 1385), ' ' , -1) AS `hk` FROM `character` WHERE `name` LIKE '" + pname + "';";
    QueryResult *result = CharacterDatabase.Query(sqlquery.c_str());
        Player *chr = objmgr.GetPlayer(guid);
        if(result) {
        Field *fields = result->Fetch();
        std::string pguid = fields[0].GetCppString();
        std::string pacct = fields[1].GetCppString();
        pname = fields[2].GetCppString();
        uint32 praceid = fields[3].GetUInt32();
        uint32 pclassid = fields[4].GetUInt32();
        std::string ponline = (fields[5].GetInt32() == 1 ? "\x3\x30\x33Online" : "\x3\x30\x34Offline\xF");
        std::string plevel = fields[6].GetCppString();
        uint32 pguildid = fields[7].GetUInt32();
        uint32 pguildrank = fields[8].GetUInt32();
        std::string pxp = fields[9].GetCppString();
        std::string pmaxxp = fields[10].GetCppString();
        unsigned int money = fields[11].GetInt32();
        std::string hk = fields[12].GetCppString();
        delete result;
	std::string sqlquery = "SELECT `gmlevel` FROM `account` WHERE `id` = '" + pacct + "';";
    QueryResult *result = loginDatabase.Query(sqlquery.c_str());  
		Field *fields2 = result->Fetch();
        std::string pgmlvl = fields2[0].GetCppString();
		delete result;
		std::string guildinfo = "";
        if (pguildid != 0)
        {
            Guild* guild = objmgr.GetGuildById(pguildid);
            if (guild)
            {
                guildinfo = " " + guild->GetRankName(pguildrank) + " Of " + guild->GetName();
            }
        }
		else guildinfo = " None";
        ChrRacesEntry const* prace = sChrRacesStore.LookupEntry(praceid);
        ChrClassesEntry const* pclass = sChrClassesStore.LookupEntry(pclassid);

		if (atoi(plevel.c_str()) < sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
            plevel += " (" + pxp + "/" + pmaxxp + ")";
        unsigned int gold = money / 10000;
        unsigned int silv = (money % 10000) / 100;
        unsigned int cop = (money % 10000) % 100;
        char tempgold [100];
        sprintf(tempgold, "\x2\x3\x30\x37%ug \x3\x31\x34%us \x3\x30\x35%uc\xF", gold, silv, cop);
        if (ponline == "\x3\x30\x33Online")
        {
            Player * plr = ObjectAccessor::Instance().FindPlayerByName(pname.c_str());
            if (plr)
            {
                AreaTableEntry const* area = GetAreaEntryByAreaID(plr->GetAreaId());
                ponline += " in " + (std::string) area->area_name[sWorld.GetDBClang()];
                if (area->zone != 0)
                {
                    AreaTableEntry const* zone = GetAreaEntryByAreaID(area->zone);
                    ponline += " (" + (std::string)zone->area_name[sWorld.GetDBClang()] + ")";
                }
            }
        }
		std::string pinfo  = "\x2 About Player:\x3\x31\x30 " +pname+ "\xF |\x2 GM Level:\x3\x31\x30 " +pgmlvl+ "\xF |\x2 AcctID:\x3\x31\x30 " +pacct+ "\xF |\x2 CharID:\x3\x31\x30 " +pguid+ " \xF |\x2 Guild Info:\x2\x3\x31\x30" +guildinfo;
        std::string pinfo2 = "\x2 Race:\x2\x3\x31\x30 " + (std::string)prace->name[sWorld.GetDBClang()] + "\xF |\x2 Class:\x2\x3\x31\x30 " + (std::string)pclass->name[sWorld.GetDBClang()] + "\xF |\x2 Level:\x2\x3\x31\x30 " + plevel + "\xF |\x2 Money:\x2 " + tempgold + "\xF |\x2 Status:\x2 " + ponline;
//        pinfo3 = " :" + " \x2Honor Kills:\x2\x3\x31\x30 " + hk;
        Send_IRC(ChanOrPM(CD),pinfo ,true);
        Send_IRC(ChanOrPM(CD),pinfo2 ,true);
//        Send_IRC(ChanOrPM(CD),pinfo3 ,true);
    }
    else
        Send_IRCA(CD->USER, " 4[ERROR] : Unknown Player: " + pname ,true, MSG_NOTICE);
}
void IRCCmd::PM_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 2);
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{
       if(plr->isAcceptWhispers())
       {
		   std::string sMsg = MakeMsg("|cffFE87FD[<IRC>%s] Whispers: %s|r", CD->USER.c_str(), _PARAMS[1].c_str());
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
		   plr->PlaySound(3081, true);
		   Send_IRC(ChanOrPM(CD), " 13To ["+_PARAMS[0]+"] : "+_PARAMS[1]+".",true);
	  }
	  else
	  Send_IRCA(CD->USER, " 4[ERROR] : Is Not Accepting Private Messages!", true, MSG_NOTICE);
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : Player not online!", true, MSG_NOTICE);
}
void IRCCmd::Revive_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, CD->PCOUNT);
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{
		if(plr->isDead())
		{
	        plr->ResurrectPlayer(0.5f);
	        plr->SpawnCorpseBones();
                plr->SaveToDB();
				sIRC.Send_IRC_Channel(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Been Revived By: " + CD->USER, true);
			Send_Player(plr, MakeMsg("You Have Been Revived By: %s.", CD->USER.c_str()));
		}
		else
		Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Not Dead!", true, MSG_NOTICE);
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : "+_PARAMS[0]+" Is Not Online!", true, MSG_NOTICE);
}
void IRCCmd::Saveall_Player(_CDATA *CD)
{
	ObjectAccessor::Instance().SaveAllPlayers();
	Send_IRC(ChanOrPM(CD), " 13["+CD->USER+"] : Has Saved All Players!",true);
}
void IRCCmd::Shutdown_Mangos(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 1);
	int32 i_time = atoi(_PARAMS[0].c_str());
    if (i_time <= 0 && _PARAMS[0]!="0") 
		{
			Send_IRC(ChanOrPM(CD), " 13["+CD->USER+"] : Please Enter A Number! And No Negative Numbers! "+_PARAMS[0]+" Seconds!?",true);	
			return;
		}
	if (i_time > 1) Send_IRC(ChanOrPM(CD), " 13["+CD->USER+"] : Has Requested Server To Be Shut Down In "+_PARAMS[0]+" Seconds!",true);
	sWorld.ShutdownServ(i_time);
	Delay(i_time*1000);
	Send_IRC(ChanOrPM(CD), " 4Server Will Now Shut Down.. Good Bye!",true);
	sIRC.Active = false;
	sIRC.ResetIRC();
}
void IRCCmd::Spell_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 3);
	uint32 spell         = atoi(_PARAMS[2].c_str());
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spell);
    if (Player* plr = GetPlayer(_PARAMS[0]))
    {
   	  if(spellInfo)
	  {
        std::string name = spellInfo->SpellName[sWorld.GetDBClang()];
        if(_PARAMS[1] == "cast")
        {
			plr->CastSpell(plr, spell, true);
			Send_IRC(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Had Spell "+name+" Casted On Them.",true);
        }
        if(_PARAMS[1] == "learn")
        {
			plr->learnSpell(spell);
			Send_IRC(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Learned Spell "+name+".",true);
        }
        if(_PARAMS[1] == "unlearn")
        {
			plr->removeSpell(spell);
			Send_IRC(ChanOrPM(CD), " 13["+_PARAMS[0]+"] : Has Unlearned Spell "+name+".",true);
        }
	  }
	  else
	  Send_IRCA(CD->USER, " 4[ERROR] : Incorrect Spell ID!", true, MSG_NOTICE);
    }
    else
    Send_IRCA(CD->USER, " 4[ERROR] : Player Not Online!", true, MSG_NOTICE);
}
void IRCCmd::Sysmsg_Server(_CDATA *CD)
{
        std::string* _PARAMS = getArray(CD->PARAMS, CD->PCOUNT);
        if(_PARAMS[0] == "a")
        {
          std::string str = LANG_SYSTEMMESSAGE + _PARAMS[1];
          std::string ancmsg = MakeMsg("04,08*[!]*SysMsg*[!]* %s 04,08*[!]*SysMsg*[!]*",_PARAMS[1].c_str());
          sWorld.SendWorldText(str.c_str(), NULL);
          Send_IRC(ChanOrPM(CD), ancmsg, true);
        }
        else if (_PARAMS[0] == "n")
        {
          std::string str = LANG_GLOBAL_NOTIFY + _PARAMS[1];
          std::string notmsg = MakeMsg("04,08*[!]*SysNotice*[!]* %s 04,08*[!]*SysNotice*[!]*",_PARAMS[1].c_str());
          WorldPacket data(SMSG_NOTIFICATION, (str.size()+1));
          data << str;
          sWorld.SendGlobalMessage(&data);
          Send_IRC(ChanOrPM(CD), notmsg, true);
        }
        else
        Send_IRCA(CD->USER, " 4[ERROR] : Please Use (a-Announce)(n-Notify) As Second Parameter!", true, MSG_NOTICE);
}
void IRCCmd::Tele_Player(_CDATA *CD)
{
	bool DoTeleport = false;
        float pX, pY, pZ, pO = 0;
        uint32 mapid = 0;
        std::string rMsg = " 4[ERROR] : Teleport Failed!";
		std::string wMsg = "Invalid Tele Location";
	std::string* _PARAMS = getArray(CD->PARAMS, 4);
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{
	  std::string* _PARAMS = getArray(CD->PARAMS, 3);
	   if(plr->isInFlight())
           {
            Send_IRCA(CD->USER, MakeMsg(" 4[ERROR] : %s Is Flying And Cannot Be Teleported!",_PARAMS[0].c_str()), true, MSG_NOTICE);
            return;
           }
       if(_PARAMS[1] == "l")
	   {	
           WorldDatabase.escape_string(_PARAMS[2]);
		   QueryResult *result = WorldDatabase.PQuery("SELECT `position_x`,`position_y`,`position_z`,`orientation`,`map` FROM `game_tele` WHERE `name`='%s';", _PARAMS[2].c_str());
                if (result)
                {
	                Field *fields = result->Fetch();
                    pX = fields[0].GetFloat();
                    pY = fields[1].GetFloat();
                    pZ = fields[2].GetFloat();
                    pO = fields[3].GetFloat();
                    mapid = fields[4].GetUInt16();
                    delete result;
					rMsg = MakeMsg(" 13[%s] : Teleported To %s! By: %s.",
					_PARAMS[0].c_str(),
					_PARAMS[2].c_str(),
                    CD->USER.c_str());
					wMsg = MakeMsg("You Have Been Teleported To %s By: %s.",
					_PARAMS[2].c_str(),
					CD->USER.c_str());
				   DoTeleport = true;
                }
				else
     			Send_IRC(ChanOrPM(CD), " 4[ERROR] : Location Not Found!", true);
		}
		else if(_PARAMS[1] == "c")
		{
			std::string* _PARAMSA = getArray(_PARAMS[2], 4);
			pX = atof(_PARAMSA[1].c_str());
			pY = atof(_PARAMSA[2].c_str());
			pZ = atof(_PARAMSA[3].c_str());
			mapid = atoi(_PARAMSA[0].c_str());
			rMsg = MakeMsg(" 13[%s] : Teleported To Map: %s. Position: X(%s) Y(%s) Z(%s)! By: %s.",
				_PARAMS[0].c_str(),
				_PARAMSA[0].c_str(),
				_PARAMSA[1].c_str(),
				_PARAMSA[2].c_str(),
				_PARAMSA[3].c_str(),
                CD->USER.c_str());
			wMsg = MakeMsg("You Have Been Teleported To Map: %s. Position: X(%s) Y(%s) Z(%s)! By: %s.",
				_PARAMSA[0].c_str(),
				_PARAMSA[1].c_str(),
				_PARAMSA[2].c_str(),
				_PARAMSA[3].c_str(),
                CD->USER.c_str());
			DoTeleport = true;
		}
		else if(_PARAMS[1] == "r")
		{
			pX = plr->m_recallX;
			pY = plr->m_recallY;
			pZ = plr->m_recallZ;
			pO = plr->m_recallO;
			mapid = plr->m_recallMap;
			rMsg = MakeMsg(" 13[%s] : Has Been Recalled To Their Previous Location.",
				_PARAMS[0].c_str());
			wMsg = MakeMsg("You Have Been Recalled To Your Previous Location. By: %s",
				CD->USER.c_str());
			DoTeleport = true;
		}
		else if(_PARAMS[1] == "to")
		{
			Player* plr2 = GetPlayer(_PARAMS[2]);
			uint64 guid = objmgr.GetPlayerGUIDByName(_PARAMS[2]);
				if(plr2)
				{
					Player::LoadPositionFromDB(mapid,pX,pY,pZ,pO,guid);
						rMsg = MakeMsg(" 13[%s] : Teleported To Player: [%s] By: %s.",
							_PARAMS[0].c_str(),
							_PARAMS[2].c_str(),
							CD->USER.c_str());
						wMsg = MakeMsg("You Are Being Teleported To: %s. By: %s.",
							_PARAMS[2].c_str(),
							CD->USER.c_str());
					DoTeleport = true;
				}
				else
				Send_IRC(ChanOrPM(CD), " 4[ERROR] : Second Player Not Found!", true);
		}
		if(DoTeleport)
		{
			if(MapManager::IsValidMapCoord(mapid, pX ,pY))
			{
				plr->SaveRecallPosition();
				plr->TeleportTo(mapid, pX, pY, pZ, pO);
				sIRC.Send_IRC_Channel(ChanOrPM(CD), rMsg, true);
				Send_Player(plr, wMsg);
			}
			else
			Send_IRCA(CD->USER, " 4[ERROR] : Invalid Location!", true, MSG_NOTICE);
		}
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : Player not online!", true, MSG_NOTICE);
}
void IRCCmd::Who_Logged(_CDATA *CD)
{
	std::string OPS = "";
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if(CLIENTS[i].LoggedIn)
		{
			OPS.append(MakeMsg(" [GM:%d IRC: %s - WoW: %s] ", CLIENTS[i].GMLevel, CLIENTS[i].Name.c_str() ,CLIENTS[i].UName.c_str()));
		}
	}
	Send_IRC(ChanOrPM(CD), OPS, true);
}

// OK THIS CODE IS COMPLETLY USESLESS AND HAVE ABSOLUTLY NOTHING TODO WITH IRC ANYMORE HEHE
// But its pretty funny
// What this does it create a new thread and execute a little "script"
// it spawns a npc near the player and roots it it greets the player
// then casts buff spells stamina, intellect, spirit then leaves
// the purpose of this all was to learn more about the unit classes and grid
// i tworks fine with 1 player (so far my own tests) and a few spawns
// i do not know the stability and functionality with a "loaded" server
// and multiple buffers in theorty each buffer is on its own thread
// and should be spawn and talk to the correct player regaredles where
// they are in the world
// ** USE THIS AT YOUR OWN RISK
#define NPC_ENTRY sIRC.ZBUFF_NPC
#define SPELL_ANIM sIRC.ZBUFF_ANIM
#define CAST_SPELL_1 sIRC.ZBUFF_SPELL1 // 25389 POWER WORD
#define CAST_SPELL_2 sIRC.ZBUFF_SPELL2 // 10157 //27126 //ARC INT
#define CAST_SPELL_3 sIRC.ZBUFF_SPELL3 // 25312// DIVINE SPIRIT
#ifdef _WIN32
DWORD	WINAPI Buff_Script(void* ptr)
#else
void *Buff_Script( void *ptr )
#endif
{
	if(sIRC._Max_Script_Inst == MAX_SCRIPT_INST)
		return 0;
	sIRC._Max_Script_Inst++;
	std::string pname = (char *)ptr;
	normalizePlayerName(pname);
	if (Player* plr = ObjectAccessor::Instance().FindPlayerByName(pname.c_str()))
	{
	    Creature* pCreature = new Creature(plr);
		if (!pCreature->Create(objmgr.GenerateLowGuid(HIGHGUID_UNIT),
			plr->GetMapId(),
			plr->GetPositionX()  - 3,
			plr->GetPositionY() - 3,
			plr->GetPositionZ() + 5,
			plr->GetOrientation() - 2.5,
			NPC_ENTRY,
			plr->GetTeam())) { delete pCreature; }
		else
		{
			Delay(500);
			pCreature->SaveToDB();
			pCreature->LoadFromDB(pCreature->GetGUIDLow(), plr->GetInstanceId());// To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
			plr->SetMovement(MOVE_ROOT);
			plr->PlaySound(S_ENTERWORLD ,true);
			MapManager::Instance().GetMap(pCreature->GetMapId(), pCreature)->Add(pCreature);
			Delay(500);
			pCreature->Say("Hello I Have Been Sent To Help You.", LANG_UNIVERSAL, plr->GetGUID());
			Delay(2000);
			pCreature->CastSpell(plr, sSpellStore.LookupEntry( SPELL_ANIM ), true);
			Delay(2000);
			pCreature->CombatStop();
			if(SpellEntry const *spellInfo = sSpellStore.LookupEntry( CAST_SPELL_1 ))
			{
				for(uint32 i = 0;i<3;i++)
				{
					uint8 eff = spellInfo->Effect[i];
					if (eff>=TOTAL_SPELL_EFFECTS)
						continue;
					if (eff == SPELL_EFFECT_APPLY_AURA || eff == SPELL_EFFECT_APPLY_AREA_AURA || eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
					{
						Aura *Aur = new Aura(spellInfo, i, NULL, plr);
						plr->AddAura(Aur);
					}
				}
			}
			Delay(1000);
			if(SpellEntry const *spellInfo = sSpellStore.LookupEntry( CAST_SPELL_2 ))
			{
				for(uint32 i = 0;i<3;i++)
				{
					uint8 eff = spellInfo->Effect[i];
					if (eff>=TOTAL_SPELL_EFFECTS)
						continue;
					if (eff == SPELL_EFFECT_APPLY_AURA || eff == SPELL_EFFECT_APPLY_AREA_AURA || eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
					{
						Aura *Aur = new Aura(spellInfo, i, NULL, plr);
						plr->AddAura(Aur);
					}
				}
			}
			Delay(1000);
			if(SpellEntry const *spellInfo = sSpellStore.LookupEntry( CAST_SPELL_3 ))
			{
				for(uint32 i = 0;i<3;i++)
				{
					uint8 eff = spellInfo->Effect[i];
					if (eff>=TOTAL_SPELL_EFFECTS)
						continue;
					if (eff == SPELL_EFFECT_APPLY_AURA || eff == SPELL_EFFECT_APPLY_AREA_AURA || eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
					{
						Aura *Aur = new Aura(spellInfo, i, NULL, plr);
						plr->AddAura(Aur);
					}
				}
			}
			Delay(2000);
			pCreature->Say("I Must Bid You Farewell Now.", LANG_UNIVERSAL, plr->GetGUID());
			Delay(1000);
			plr->SetMovement(MOVE_UNROOT);
			plr->PlaySound(S_STEALTH ,true);
			pCreature->CombatStop();
			pCreature->DeleteFromDB();
			pCreature->CleanupsBeforeDelete();
			ObjectAccessor::Instance().AddObjectToRemoveList(pCreature);
		}
	}
	sIRC._Max_Script_Inst--;
	return 0;
}
void IRCCmd::Zbuff_Player(_CDATA *CD)
{
	std::string* _PARAMS = getArray(CD->PARAMS, 1);
	if (Player* plr = GetPlayer(_PARAMS[0]))
	{
               #ifdef _WIN32
	    	   CreateThread(NULL, 0, Buff_Script, (void*)plr->GetName(), 0, NULL);
		       #else
               MThread *newthr = new MThread ();
               pthread_create(&newthr->tid, NULL, &Buff_Script, (void*)plr->GetName());
               #endif
	           sIRC.Send_IRC_Channel(ChanOrPM(CD), MakeMsg(" 13[%s] : Has Been Buffed!",_PARAMS[0].c_str()), true);
	}
	else
	Send_IRCA(CD->USER, " 4[ERROR] : Player Not Online!", true, MSG_NOTICE);
}

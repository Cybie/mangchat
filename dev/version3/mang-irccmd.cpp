#include "mang-irc.h"
#include "mang-ircmsg.h"

void MangIrc::SplitIn2(char *data, char *part1, char *part2)
{
	unsigned int tmplen = strlen(data);		// Temporairly variable for data size.
	char *tmpdata = (char*)("");			// Temporairly variable for split.

	if(tmplen)
	{
		// extract the sender by retrieving the first value before we find a space.
		part1 = strtok(data, "!");
		if (part1)
			part2 = &data[strlen(part1) + 1];
	}
}

void MangIrc::onConnect()
/* When a connection is confirmed, this trigger depends on the Irc Connect Code ICC. */
{
	printf("Connection to the server succesfull");
	this->SendData("JOIN %s", MC_IRC_CHANNEL);
}

void MangIrc::cmdPING(char *MMD)
/* Commands that where not supported *DEBUG FUNCTION* */
{
	printf("receieved ping command replying %s ", MMD);
		SendData(MMD);
		//printf("Unsupported command %s\n", MMD->command);
}


void MangIrc::cmdFalse(MANG_MSG_DATA *MMD)
/* Commands that where not supported *DEBUG FUNCTION* */
{
		printf("Unsupported command %s\n", MMD->command);
}

void MangIrc::cmdJoin(MANG_MSG_DATA *MMD)
/* IRC Command Channel JOIN */
{
	if(!strcmp(MMD->nick, this->nick))
	{
		printf("Joining channel %s %s\n", MMD->nick, this->nick);
		this->SendData("PRIVMSG %s :MangChat 3.0 Extreme, ready for action.", MC_IRC_CHANNEL);
	}
	else
	{
		printf("%s Joined channel %s\n", MMD->nick, "x");
	}
}


void MangIrc::cmdPart(MANG_MSG_DATA *MMD)
/* IRC Command Channel PART */
{

}

bool findmatch(char *source, char *what)
{
	char szFind[512];

	strcpy(szFind, source);

	char *pch = strtok(szFind, " ");
	int wordCnt = 0;
	int wordFnd = 0;

	while (pch != NULL)
	{
		wordCnt++;
		if(!strncmp(what, pch, strlen(what)))
		{
			
			wordFnd++;
			break;
		}
		pch = strtok (NULL, " ");
	}

	if(wordFnd)
		return true;
	else
		return false;
}

void MangIrc::cmdPrivmsg(MANG_MSG_DATA *MMD)
/* IRC Command PRIVMSG */
{
	char *tmpdata = new char[strlen(MMD->message)+1];
	strcpy(tmpdata, MMD->message);
	char *channel = strtok(tmpdata, " ");
	char *msg = &tmpdata[strlen(channel)+1];
	msg = &msg[1];

	if(!strncmp(msg, nick, strlen(nick)))
	{
		if(strlen(nick) == strlen(msg) - 1)
		{
			this->SendTo(channel, MSG_TYPE_ACTION, "ponders...yes?");
		}
		else
		{
			if(!strncmp("time", &msg[strlen(nick)+1], 4))
			{
				this->SendTo(channel, MSG_TYPE_PRIV, "I don't know what time it is %s, get lost!", MMD->nick);
			}
			else if(!strncmp("findword", &msg[strlen(nick)+1], 8))
			{
				if(findmatch(msg, "cybrax"))
					this->SendTo(channel, MSG_TYPE_PRIV, "My sensors detect the presence of your request%s, most likely it is there!", MMD->nick);
				else
					this->SendTo(channel, MSG_TYPE_PRIV, "Sorry i could not find it %s!", MMD->nick);
			}
			else
				this->SendTo(channel, MSG_TYPE_ACTION, "pukes out...error...does not compute...you retarded?");
		}
		return;
	}
		

	if((msg[0] == '$') && strlen(msg) > 1)
	{
		// We have receieved a command strip the prefix token.
		msg = &msg[1];
		

		char *command = strtok(msg, " ");
		msg = &msg[strlen(command) + 1];
		unsigned int alength = strlen(msg);

		if(!strcmp(IRC_USR_LOGIN, command))
		{
			
			if(char *tuser = strtok(msg, " "))
			{


				if(alength > strlen(tuser) + 1)
				{
					char *tpass = &msg[strlen(tuser) + 1];

					this->SendData("PRIVMSG %s :login attempted - user: %s, password: %s", channel,tuser ,tpass);
				}
				else
					this->SendData("PRIVMSG %s :you did not provide enough parameters given 1", channel);
			}
		}
		else if(!strcmp("nick", command))
		{
			nick = NULL;
			nick = new char[strlen(msg)];
			strcpy(nick, msg);
			this->SendData("NICK :%s", nick);
		}
		else if(!strcmp("say", command))
		{
			if(!strncmp(channel, nick, strlen(channel)))
			{
				if(!strcmp(MMD->nick, "Cybrax"))
				{
					if(!strncmp(msg, "clicker de click and i change my nick to click", 7))
					{
						strcpy(nick, "Clicker");
						this->SendData("NICK :%s", "Clicker");
					}

					this->SendTo(MC_IRC_CHANNEL, MSG_TYPE_PRIV, "%s", msg);
				}
				else
					this->SendTo(MMD->nick, MSG_TYPE_PRIV, "hahahahaha you are unauthorized to do this, we are watching you sonny", MMD->nick);
			}
			else
				this->SendTo(MC_IRC_CHANNEL, MSG_TYPE_PRIV, "sorry this is not allowed here");
		}
		else if(!strcmp("test", command))
		{
			char params[512];
			memset(params, 0, 512);
			char *pch;
			pch = strtok (msg, " ");
			int paramCnt = 0;

			while (pch != NULL)
			{
				strcat(params, pch);
				strcat(params, " - ");
				
				paramCnt++;
				//printf ("%s\n",pch);
				pch = strtok (NULL, " ");
			}
			this->SendData("PRIVMSG %s :you provided %d parameters %s", channel, paramCnt, params);
		}
		else if(!strcmp("channel", command))
		{
			this->SendTo(channel, MSG_TYPE_PRIV, "the current channel is %s you are %s, i am %s" , channel, MMD->nick, nick);
		}
		else
		{
			this->SendData("PRIVMSG %s :you did not provide enough parameters  given 0", channel);
		}

	}



	delete[] tmpdata;
}

void MangIrc::cmdNICK(MANG_MSG_DATA *MMD)
/* IRC Command Nick*/
{
	this->SendData("PRIVMSG %s :nick change by %s",MC_IRC_CHANNEL, MMD->nick);
	//printf("===start===\n%s\n%s\n%s\n===end===\n\n\n", MMD->sender, MMD->command, MMD->message);
}


void MangIrc::cmdDefault(MANG_MSG_DATA *MMD)
/* IRC Command Custom, The custom command is always called after the defaults are processed */
{
	//printf("===start===\n%s\n%s\n%s\n===end===\n\n\n", MMD->sender, MMD->command, MMD->message);
}



/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#include "mang-ircmsg.h"


MangIrcMsg::MangIrcMsg()
/* Class Constructor */
{
}

MangIrcMsg::MangIrcMsg(MangIrc *_irc)
/* Class Overloaded Constructor */
{
	irc = _irc;
}

MangIrcMsg::~MangIrcMsg()
/* Class Denstructor */
{
}

void MangIrcMsg::Process(char *data)
/* Desc */
{
	// Break the data into parts on every line break\n.
	char *pch = strtok (data, "\n");

	// loop through each line
	while (pch != NULL)
	{
		// send the line to the decode function
		Decode(pch);
		// process the next line 
		pch = strtok (NULL, "\n");
	}
}

void MangIrcMsg::Decode(char *data)
/* Decode - Cuts the passed argument into parts */
{
	if (!data)
		return;

	if(!strncmp(data, "PING", 4))
	{
		//if(!strcmp(IRC_CMD_PING, _MMD->command))
		data[1] = 'O';
		irc->SendTo(MC_IRC_CHANNEL, 0,"ping rquest received %s", data);
		irc->cmdPING(data);
		return;
	}

	bool bProcess = false;

	MANG_MSG_DATA MMD;

	// Only messages that contain a double dot are relevant to us
	if(data[0] == ':')
	{
		// We need the length of the entire string to extract the arguments later.
		unsigned int tmplen = strlen(data);
		char *tmpdata = (char*)(""); // Temporairly variable for split.
		if(tmplen)
		{
			// extract the sender by retrieving the first value before we find a space.
			MMD.sender = strtok(data, " ");
			if (MMD.sender)
			{
				//Cybrax!Cybrax@eveNet-c3660ed8.cable.wanadoo.nl
				//<nick>!<user>@<host>
				// Make sure we have a sender and check if there is something remaining in the string.
				if (tmplen > strlen(MMD.sender))
				{
					// tempdata will now contain the command and the rest of the message.
					tmpdata = &data[strlen(MMD.sender) + 1];

					// Again we take the srings length to perform the above operation again.
					tmplen = strlen(tmpdata);
					
					if(tmplen)
					{			
						// Extract the next value till the next space.
						MMD.command = strtok(tmpdata, " ");

						bProcess = true;

						// if the string contains more characters then command we have data left.
						if (tmplen > strlen(MMD.command))
							MMD.message = &tmpdata[strlen(MMD.command) + 1]; // Apply the remainder to message.
					}
				}

				// Extract the nickname.
				MMD.nick = strtok(MMD.sender, "!");

				// Get the remainder.
				char *newHost = &MMD.sender[strlen(MMD.nick) + 1];
				
				// Extract String before @
				MMD.sender = strtok(newHost, "@");;

				// Store host after @
				MMD.host = &newHost[strlen(MMD.sender) + 1];

				MMD.nick = &MMD.nick[1];

				// Now we can detect what kind of data we receieved by checking the command and act upon it.
				

			}
		}

		if(bProcess)
			this->Command(&MMD, data);
	} // end if(data[0] == ':')
}



void MangIrcMsg::Command(MANG_MSG_DATA *__MMD, char *data)
/* Decode - Cuts the passed argument into parts */
{
	// Make the message struct availlable
	_MMD = __MMD;

	if(!strcmp(IRC_CMD_ICC, _MMD->command))
		irc->onConnect();
	else if(!strcmp(IRC_CMD_ERR_1, _MMD->command))
		printf("===start===\n%s\n%s\n%s\n===end===\n\n", _MMD->sender, _MMD->command, _MMD->message);
	else if(!strcmp(IRC_CMD_JOIN, _MMD->command))
		irc->cmdJoin(_MMD);
	else if(!strcmp(IRC_CMD_PRIVMSG, _MMD->command))
		irc->cmdPrivmsg(_MMD);
	else if(!strcmp(IRC_CMD_NICK, _MMD->command))
		irc->cmdNICK(_MMD);
	else if(!strcmp(IRC_CMD_ACTION, _MMD->command))
		printf(data);	//irc->cmdACTION(_MMD);
	else  // No matching command found
		irc->cmdFalse(_MMD);

	irc->cmdDefault(_MMD); // Perform Default operation
}




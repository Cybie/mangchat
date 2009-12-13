#include "IRCIdent.h"
#include "IRCFunc.h"

void IRCIdent::run()
{
	this->Sock = new ServerSocket(113);
//	do {
		ClientSocket *Client = this->Sock->_Accept();
		char Buffer[BUFFER_SIZE];
		memset(Buffer, 0, BUFFER_SIZE);
		Client->_Recv(Buffer, BUFFER_SIZE);

		std::string data(Buffer);
		TrimCrLf(data);

		data += " : USERID : UNIX : MC3";
		Client->Send(data);

		delete Client;
//	} while(!this->IsStopped);
}
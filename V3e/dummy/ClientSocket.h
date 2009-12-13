#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "BaseSocket.h"

class ClientSocket : public BaseSocket
{
	public:
		ClientSocket(void);
		ClientSocket(SOCK_TYPE _sock_id);
		~ClientSocket(void);
	public:
		bool Connect(const char *addr, int port);
		void Close() {  _Close(); };
		void Send(std::string _data);
};

#endif

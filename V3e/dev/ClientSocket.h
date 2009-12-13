#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "Socket.h"

class ClientSocket : public Socket
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

#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

#include "ClientSocket.h"

class ServerSocket : public Socket
{
public:
    ServerSocket(void);
    ServerSocket(int _port);
    ~ServerSocket(void);
public:
    ClientSocket* _Accept();
    void Close() { };

};

#endif

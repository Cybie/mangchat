#include "ServerSocket.h"

ServerSocket::ServerSocket()
{
}

ServerSocket::ServerSocket(int _port)
{
    _Create(SOCK_STREAM, IPPROTO_TCP);
    _SetOptions(1);
    _Bind("", _port);
    _Listen(26);
}

ServerSocket::~ServerSocket()
{
}

ClientSocket* ServerSocket::_Accept()
{   
    SOCK_TYPE NewConn;
    if ((NewConn = ::accept(sock_id, NULL, 0)) < 0)     
        return (NULL);

    return new ClientSocket(NewConn);
}

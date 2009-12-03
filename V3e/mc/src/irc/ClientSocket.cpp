#include "ClientSocket.h"

ClientSocket::ClientSocket()
{
    _Create(SOCK_STREAM, IPPROTO_TCP);
    _SetOptions(1);
}

ClientSocket::ClientSocket(SOCK_TYPE _sock_id)
{
    sock_id = _sock_id;
}

ClientSocket::~ClientSocket()
{
}

bool ClientSocket::Connect(const char *addr, int port)
{
    return _Connect(addr, port);
}

void ClientSocket::Send(std::string _data)
{
	_Send(_data.c_str());
	_Send("\r\n");
}
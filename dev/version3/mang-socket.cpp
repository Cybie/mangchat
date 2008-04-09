/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#include "mang-socket.h"

MangSock::MangSock()
{
	this->Initialize();
}

MangSock::~MangSock()
{
}

bool MangSock::Initialize()
{
    #ifdef _WIN32
		WSADATA wsaData;                                        //WSAData
		if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0)
		{
		//	sLog.outError("IRC Error: Winsock Initialization Error");
			return false;
		}
    #endif

    if ((_SOCKET = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        //sLog.outError("IRC Error: Socket Error");
        return false;
    }

    int on = 1;
    if ( setsockopt ( _SOCKET, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    {
        //sLog.outError("IRC Error: Invalid Socket");
        return false;
    }
    #ifdef _WIN32
		u_long iMode = 0;
		ioctlsocket(_SOCKET, FIONBIO, &iMode);
    #else
		fcntl(sIRC.SOCKET, F_SETFL, O_ASYNC);                   // set to asynchronous I/O
		int opts = fcntl ( _SOCKET, F_GETFL );

		if(opts < 0)
		{
			//sLog.outError("IRC!: Error, non blocking failed!");
			return 0;
		}

		opts = (opts & ~O_NONBLOCK );
		fcntl(_SOCKET, F_SETFL, opts);
    #endif
    return true;
}

void MangSock::Close()
{
    if(_SOCKET)
    {
        #ifdef WIN32
			closesocket(_SOCKET);
			//WSACleanup(); // 
        #else
			close(_SOCKET);
        #endif
    }
}

bool MangSock::Connect(const char *cHost, int nPort)
{
    struct hostent *he;
    if ((he = gethostbyname(cHost)) == NULL)
    {
        //sLog.outError("IRC!: Error, Could not resolve host : %s", cHost);
        return false;
    }
    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(nPort);
    host_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(host_addr.sin_zero), '\0', 8);
    if (::connect(_SOCKET, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
    {
		//sLog.outError("IRC: Error, Cannot connect to : %s", cHost);
        return false;
    }
    FD_ZERO(&_SOCKSET);
    FD_SET(_SOCKET, &_SOCKSET);
    return true;
}


void MangSock::Transmit(const char *data)
{
    if(_SOCKET)
        if (send(_SOCKET, data, (size_t)strlen(data), 0) == -1)
            printf("Error");
}

bool MangSock::Receieve(char *data)
{
	memset(data, 0, MAXDATASIZE );
    
    int nBytesRecv = ::recv(_SOCKET, data, MAXDATASIZE - 1, 0 );

    if ( nBytesRecv == -1 )
	{
		printf("Error");
		Close();
		return false;
	}

	return true;
}


void MangSock::Cleanup()
{
	Close();
}

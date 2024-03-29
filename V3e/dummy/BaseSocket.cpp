#include "BaseSocket.h"
#include "IRCFunc.h"

#ifdef WIN32
    static bool ws_init = false;
#endif

BaseSocket::BaseSocket()
{
    #ifdef WIN32
    if (!ws_init)
	{
        WORD wVersionRequested;
        WSADATA wsaData;

        wVersionRequested = MAKEWORD(2, 0);              // Request WinSock v2.0
        if (WSAStartup(wVersionRequested, &wsaData) != 0) 
        {  // Load WinSock DLL
            ws_init = false;
            return;
        }
        ws_init = true;
    }
    #endif
	
	use_ssl = false;
}

bool BaseSocket::_Connect(const char *addr, int port)
{
	if(use_ssl)
	{
	#ifdef OPEN_SSL
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_METHOD *ssl_m = SSLv23_client_method();
		SSL_CTX *ssl_c = SSL_CTX_new(ssl_m);
		ssl_id = SSL_new(ssl_c);
	#endif
	}
    // Get the address of the requested host
    sockaddr_in destAddr;
    _fillAddr(addr, port, destAddr);

    // Try to connect to the given port
    if (::connect(sock_id, (sockaddr *) &destAddr, sizeof(destAddr)) < 0)
    {
        return false;
    }
	if(use_ssl)
	{
	#ifdef OPEN_SSL
		SSL_set_fd(ssl_id, sock_id);
		SSL_connect(ssl_id);
	#endif
	}
    return true;
}

int BaseSocket::_Recv(void *buffer, int bufferLen)
{
    int rtn = 0;

	if(use_ssl)
	{
		#ifdef OPEN_SSL
			rtn = SSL_read(ssl_id, buffer, bufferLen);
			if(rtn <= 0 || rtn > BUFFER_SIZE)
				return rtn;
		#endif
	}
	else
	{
	    if ((rtn = ::recv(sock_id, (raw_type *) buffer, bufferLen, 0)) < 0)
	        return rtn;
	}
    #ifdef USE_UTF8
		ToUTF8((char *)buffer);
    #endif
    return rtn;
}

void BaseSocket::_Send(const char *buffer)
{
    #ifdef USE_UTF8
		ToUTF8(buffer);
    #endif

    if(use_ssl)
    {
		#ifdef OPEN_SSL
			SSL_write(ssl_id, buffer, strlen(buffer));
		#endif
    }
	else
	{
		if (::send(sock_id, (raw_type *) buffer, strlen(buffer), 0) < 0)
		{};
	}
}

void BaseSocket::_Create(int _type, int _protocol)
{   
    if ((sock_id = socket(PF_INET, _type, _protocol)) < 0)
    {     
    }
}

void BaseSocket::_Bind(const std::string &_address, unsigned short _port)
{
    sockaddr_in _addr;
    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_port = htons(_port);

    if (bind(sock_id, (sockaddr *) &_addr, sizeof(sockaddr_in)) < 0)
    {
    }
}

void BaseSocket::_Listen(int _max)
{
    if (listen(sock_id, _max) < 0)
    {
    }
}

void BaseSocket::_Close()
{
    if(sock_id)
    {
        #ifdef WIN32
            ::closesocket(sock_id);
        #else
            ::close(sock_id);
        #endif
    }
    sock_id = -1;  
}

void BaseSocket::_SetOptions(int nOptions)
{
    if ( setsockopt ( sock_id, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &nOptions, sizeof ( nOptions ) ) == -1 )
    {
    }
}

void BaseSocket::_SetBlocking(bool bBlock)
{
    u_long iMode = 0;

    if(bBlock)
        iMode = 1;

    #ifdef WIN32      
        ioctlsocket(sock_id, FIONBIO, &iMode);
    #else
        if(iMode == 1)
        {
            //fcntl(sock_id, F_SETFL, O_NONBLOCK);  // set to non-blocking
            //fcntl(sock_id, F_SETFL, O_ASYNC);     // set to asynchronous I/O
        }
        else
        {
            //fcntl(sock_id, F_SETFL, O_NONBLOCK);  // set to non-blocking
            //fcntl(sock_id, F_SETFL, O_ASYNC);     // set to asynchronous I/O
        }
    #endif
}

void BaseSocket::_fillAddr(const std::string &address, unsigned short port, sockaddr_in &addr)
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    hostent *host;
    if ((host = gethostbyname(address.c_str())) == NULL)
    {
       // throw SocketException("Failed to resolve name (gethostbyname())");
        return;
    }
    addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

    addr.sin_port = htons(port);     // Assign port in network byte order
}

std::string BaseSocket::RemoteAddr()
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sock_id, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0)
    {
        return "0.0.0.0";
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short BaseSocket::RemotePort() 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sock_id, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0)
    {
        return 0;
    }
    return ntohs(addr.sin_port);
}

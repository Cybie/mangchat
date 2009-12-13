#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "common.h"

#ifdef WIN32
  #include <winsock.h>
  typedef SOCKET SOCK_TYPE;
  typedef int socklen_t;
  typedef char raw_type;
#else
  #include <sys/types.h>       // For data types
  #include <sys/socket.h>      // For socket(), connect(), send(), and recv()
  #include <netdb.h>           // For gethostbyname()
  #include <arpa/inet.h>       // For inet_addr()
  #include <unistd.h>          // For close()
  #include <netinet/in.h>      // For sockaddr_in
  typedef int SOCK_TYPE;
  typedef void raw_type;       // Type used for raw data on this platform
#endif

#define BUFFER_SIZE 1024

const int RCVBUFSIZE = 512;

class Socket
{
public:
    Socket(void);
    ~Socket(void);

public:
    std::string RemoteAddr();
    unsigned short RemotePort();

    int _Recv(void *buffer, int bufferLen);
    void _Send(const char *buffer);

private:
    void _fillAddr(const std::string &address, unsigned short port, sockaddr_in &addr);

protected:
    SOCK_TYPE sock_id;

protected:
    void _Create(int _type, int _protocol);
    void _Bind(const std::string &_address, unsigned short _port);
    void _Listen(int _max);

    bool _Connect(const char *addr, int port);

    void _Close();

    void _SetOptions(int nOptions);
    void _SetBlocking(bool bBlock);

};

#endif

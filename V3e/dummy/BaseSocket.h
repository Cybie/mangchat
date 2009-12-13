#ifndef _IRC_SOCKET_H_
#define _IRC_SOCKET_H_

#include "common.h"

#ifdef OPEN_SSL
	#define SSL_BSIZE 80
	#define BUFFER_SIZE 4096
#else
	#define BUFFER_SIZE 1024
#endif

const int RCVBUFSIZE = 512;

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

class BaseSocket
{
	public:
		BaseSocket(void);
		~BaseSocket(void){};

	public:
		std::string RemoteAddr();
		unsigned short RemotePort();

		int _Recv(void *buffer, int bufferLen);
		void _Send(const char *buffer);

		void SetSSL(bool bssl) { use_ssl = bssl; };

	private:
		void _fillAddr(const std::string &address, unsigned short port, sockaddr_in &addr);

	protected:
		void _Create(int _type, int _protocol);
		void _Bind(const std::string &_address, unsigned short _port);
		void _Listen(int _max);

		bool _Connect(const char *addr, int port);

		void _Close();

		void _SetOptions(int nOptions);
		void _SetBlocking(bool bBlock);

	protected:
		bool use_ssl;

	protected:
		SOCK_TYPE sock_id;
		#ifdef OPEN_SSL
			SSL *ssl_id;
		#endif
};

#endif

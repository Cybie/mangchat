/*
 * MangChat By Cybr@x Cybersp@ce (C) 2007-2008
 * Version 3.0.0.0 Codename: MangChat Extrme
 *
 * This Program Is Free Software; You Can Redistribute It And/Or Modify It Under The Terms Of The GNU General Public License
 * Written and Developed by Cybrax. <cybraxvd@gmail.com>
 * |Death| <death@hell360.net>, Lice <lice@yeuxverts.net>, Dj_baby, Sanaell, Tase, MaNGOS Community.
 * PLEASE RETAIN THE COPYRIGHT OF THE AUTHORS.
 */

#ifndef _MANG_SOCKET_H
#define _MANG_SOCKET_H

#define MAXDATASIZE 512

#ifdef WIN32
  #include <winsock.h>
  typedef SOCKET _SOCK;
  typedef int socklen_t;
  typedef char raw_type;       
#else
  typedef int _SOCKET;
  #include <sys/types.h>       // For data types
  #include <sys/socket.h>      // For socket(), connect(), send(), and recv()
  #include <netdb.h>           // For gethostbyname()
  #include <arpa/inet.h>       // For inet_addr()
  #include <unistd.h>          // For close()
  #include <netinet/in.h>      // For sockaddr_in
  typedef void raw_type;       
#endif

#include <fcntl.h>

#include <stdio.h>
#include <sys/types.h>

#include <errno.h>             // For errno

class MangSock
{

	public:
		MangSock();
		~MangSock();

	private:
		_SOCK _SOCKET;
        fd_set _SOCKSET;

	protected:
		bool Initialize();
		void Close();

		bool Connect(const char *cHost, int nPort);

		void Transmit(const char *data);

		bool Receieve(char *data);

		void Cleanup();

};


#endif

#ifndef _COMMON_H_
#define _COMMON_H_

#pragma warning(disable:4996)

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
#endif	

//#define MTN_STATIC
#define HAVE_OPENSSL

#ifdef HAVE_OPENSSL
	#include <openssl/ssl.h>
	#define OPEN_SSL
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

#ifdef WIN32
	#pragma comment(lib, "WS2_32.LIB")
	#ifdef HAVE_OPENSSL
		#pragma comment(lib, "libeay32.lib")
		#pragma comment(lib, "ssleay32.lib")		
	#endif
	#include <tchar.h>
    #include <winsock2.h>
    #include "windows.h"
#endif

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>
//using namespace std; // dont

#include <string>
#include <string.h>

#include <time.h>

#ifdef USE_MYSQL
	#include <mysql.h>
#endif

#endif

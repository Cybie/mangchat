#ifndef _COMMON_H_
#define _COMMON_H_

#pragma warning(disable:4996)

//#define MTN_STATIC

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
#endif						

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <signal.h>

#ifdef WIN32
	#include <tchar.h>
    #pragma comment(lib, "WS2_32.LIB")
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
//using namespace std;

#include <string>
#include <string.h>

#include <string.h>
#include <time.h>

#ifdef USE_MYSQL
	#include <mysql.h>
#endif

#endif

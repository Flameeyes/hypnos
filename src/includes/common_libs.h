/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file common_libs.h
\brief header
This is a small abstraction layer for threading and STL including
with the explicit purpose of tearing away some platform dependant
warnings/errors/issues.
*/

#ifndef __COMMON_LIBS_H__
#define __COMMON_LIBS_H__

#if defined(__GNUC__) && ( __GNUC__ < 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ < 1 ) )
	#error You need at least GCC 3.2 to compile this!
#endif

#ifdef __GNUC__
	#define PACK_NEEDED __attribute__ ((packed))
#else
	#define PACK_NEEDED
	#define strncasecmp strncmpi
	#define strcasecmp strcmpi
#endif

#ifdef __BORLANDC__
	#define NDEBUG
	#define WIN32
	#define _CONSOLE
	#include <stlport/hash_map>
#endif

#if defined WIN32 || defined _WIN32
	#ifndef _WIN32
		#define _WIN32
	#endif
	#pragma pack(1)

	#define snprintf _snprintf
	typedef int socklen_t;
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <iterator>
#include <algorithm>
#include <stack>
#include <fstream>
#include <utility>
#include <set>

#ifdef __GNUC__
	#include <ext/slist>
	#include <ext/hash_map>
	namespace std {
		using __gnu_cxx::slist;
		using __gnu_cxx::hash_map;
	}
#else
	#include <slist>
	#include <hash_map>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <arpa/inet.h>

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#include <xercesc/parsers/XercesDOMParser.hpp>

#include "typedefs.h"
#include "constants.h"
#include "console.h"

#ifdef __unix__
	#include "archs/hypunix.h"
#else
	#include "archs/hypwin32.h"
#endif

//#define TIMEOUT(X) (((X) <= getclock()) || overflow)
#define TIMEOUT(X) false

extern char* getOSVersionString();
enum OSVersion { OSVER_UNKNOWN, OSVER_WIN9X, OSVER_WINNT, OSVER_NONWINDOWS };
extern OSVersion getOSVersion();

//@{
/*!
\name System dependent functions

Functions defined into archs/hypunix and archs/hypwin32
*/
uint32_t getclock();
uint32_t getsysclock();
uint32_t getclockday();
void initclock();
//@}

//@{
/*!
\name Strings
\brief Strings functions
*/

char *linestart(char *line);
void strupr(std::string &str);
void strlwr(std::string &str);

int strtonum(int countx, int base= 0);
//@}

#endif //__COMMON_LIBS_H__

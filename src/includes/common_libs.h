  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

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

#ifdef __BORLANDC__
	#define NDEBUG
	#define WIN32
	#ifdef __CONSOLE__
		#define _CONSOLE
	#else
		#define _WINDOWS
	#endif
#endif

#if defined WIN32 || defined _WIN32
	#ifndef _WIN32
		#define _WIN32
	#endif
	#pragma pack(1)

	#define snprintf _snprintf
	typedef int socklen_t;
#endif

#ifndef __GNUC__
	#define strncasecmp strncmpi
	#define strcasecmp strcmpi
#endif

#ifdef __GNUC__
	#define PACK_NEEDED __attribute__ ((packed))
#else
	#define PACK_NEEDED
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>

#ifdef __GNUC__
	#if (__GNUC__ == 2)
		#include <slist.h>
	#else
		#include <ext/slist>
		using __gnu_cxx::slist;
	#endif
#else
	#include <slist>
#endif

#include <map>
#include <queue>
#include <iterator>
#include <algorithm>
#include <stack>
#include <fstream>
#include <utility>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <ctype.h>
#include <stdint.h>

extern char* getOSVersionString();
enum OSVersion { OSVER_UNKNOWN, OSVER_WIN9X, OSVER_WINNT, OSVER_NONWINDOWS };
extern OSVersion getOSVersion();

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#define USE_THREADS

#include "typedefs.h"
#include "constants.h"
#include "pointer.h"
#include "console.h"
#include "srvparms.h"

#endif //__COMMON_LIBS_H__

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

#if !defined(WIN32) && (__GNUC__ == 3) && ( (__GNUC_MINOR__ == 1) || ( __GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ < 2 ) )
	#warning The use of GCC 3 with NoX-Wizard is very very dangerous. \
	GCC3.2.2+ of Debian GNU/Linux seems to be ok.
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
	#ifdef _MSC_VER
		#define strncasecmp strncmp
		// can microsoft follow standards? no, obvious... I hate them.... - Akron
	#else
		#define strncasecmp strncmpi
		// on borland compiler exists strncmpi...
	#endif
	#define strcasecmp strcmpi
#endif

#ifdef __GNUC__
	#define PACK_NEEDED __attribute__ ((packed))
#else
	#define PACK_NEEDED
#endif

#ifdef  _MSC_VER
	#pragma pack(push,8)		//for Visual C++ using STLport
	#pragma warning(disable: 4786)	//Gets rid of BAD stl warnings
	#pragma warning(disable: 4800)	//needed couse now we can see the real warning
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

#ifdef  _MSC_VER
	#pragma pack(pop)
	#ifndef STLPORT
		#pragma warning(disable: 4103)
		#pragma warning(disable: 4786)
		#define vsnprintf _vsnprintf
	#endif
#endif

extern char* getOSVersionString();
enum OSVersion { OSVER_UNKNOWN, OSVER_WIN9X, OSVER_WINNT, OSVER_NONWINDOWS };
extern OSVersion getOSVersion();

#ifdef  _MSC_VER
	#pragma warning(disable: 4018)
#endif

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#endif //__COMMON_LIBS_H__


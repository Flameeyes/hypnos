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

#if defined(DOXYGEN)
	/*!
	\brief Defines a structure that needs the packaging
	
	This attribute is used for structs in \ref data.h which loasd the data
	raw from the files, and needs to not be padded.
	*/
	#define PACK_NEEDED
	
	//! Declare a function as deprecated
	#define DEPRECATED
	
	//! Define a function as pure (that don't use external source variables)
	#define PURE
	
	//! Define a function to conforms to the printf arguments formatting
	//! \see http://gcc.gnu.org/onlinedocs/gcc-3.3.3/gcc/Function-Attributes.html#Function%20Attributes
	#define PRINTF_LIKE(A,B) ;

#elif defined( __GNUC__ )
	#define PACK_NEEDED __attribute__ ((packed))
	#define DEPRECATED __attribute__ ((deprecated))
	#define PURE __attribute__ ((pure))
	#define PRINTF_LIKE(A,B) __attribute__ ((format (printf, A, B)));
#else
	#define PACK_NEEDED
	#define DEPRECATED
	#define PURE
	#define PRINTF_LIKE(A,B) ;
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
#endif

#ifdef __APPLE__
	#define __unix__
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

#include <assert.h>

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

#include <ctype.h>
#include <stdint.h>
#include <unistd.h>

// Only the one which is used it's actually included, the others will be
// ignored.
#include "archs/hypunix.h"
#include "archs/hypwin32.h"

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#include "typedefs.h"
#include "constants.h"
#include "console.h"

#define TIMEOUT(X) ((X) <= getclock())

extern std::string getOSVersionString();
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

#endif //__COMMON_LIBS_H__


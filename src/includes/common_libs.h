/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file common_libs.h
\brief Common library header

This is a small abstraction layer for threading and STL including
with the explicit purpose of tearing away some platform dependant
warnings/errors/issues.
*/

#ifndef __COMMON_LIBS_H__
#define __COMMON_LIBS_H__

// First of all, include the config.h and then libhypnos commons
#include "libhypnos/commons.h"

#if defined(__GNUC__) && ( __GNUC__ < 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ < 1 ) )
	#error You need at least GCC 3.2 to compile this!
#endif

#ifdef __BORLANDC__
	#define NDEBUG
	#define WIN32
	#define _CONSOLE
	#include <stlport/hash_map>
#endif

#ifdef __APPLE__
	#define __unix__
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ctype.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#include "typedefs.h"
#include "constants.h"

#define TIMEOUT(X) ((X) <= getclock())


using namespace nLibhypnos;

/*!
\brief Checks if a file exists already
\param filename Relative path of the file to check
\return true if the file exists, else false
*/
bool fileExists(std::string filename);

/*!
\brief Check if the directory of the given file exists, and if not, create it
\param dirname Relative path of the file to check the directory of
\return false if unable to create the directory, else true
*/
bool ensureDirectory(std::string filename);

#endif //__COMMON_LIBS_H__

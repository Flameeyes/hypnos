/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Win32 Architecture specific
*/

#ifdef WIN32

#ifndef _HYPNOS_WIN32_
#define _HYPNOS_WIN32_

#include <winsock.h>
#include <winbase.h>
#include <io.h>
#include <dos.h>
#include <limits.h>
#include <conio.h>
#include <process.h>
#include <errno.h>

char *basename(char *path);

namespace arch {

	char *getHKLMRegistryString(char *key, char *subkey);
	void setHKLMRegistryString(char *key, char *subkey, char *value);
	char *splitPath (char *p);

	void init_deamon();
	void initclock();

	inline bool pollHUPStatus () { return false; }
	inline bool pollCloseRequests () { return false; }
	inline void setup_signals (){ return; }
	inline void start_signal_thread() {return;}

	extern WSADATA wsaData;
	extern WORD wVersionRequested;
	extern long int oldtime, newtime;
} // namespace arch

typedef int FAR socklen_t ;

char *basename(char *path);

#endif

#endif

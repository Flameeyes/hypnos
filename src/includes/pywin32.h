/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Win32 Architecture specific
*/

#ifndef _PYUO_WIN32_
#define _PYUO_WIN32_

#include <winsock.h>
#include <winbase.h>
#include <io.h>
#include <dos.h>
#include <limits.h>
#include <conio.h>
#include <process.h>

char *getHKLMRegistryString(char *key, char *subkey);
void setHKLMRegistryString(char *key, char *subkey, char *value);
char *splitPath (char *p);

void init_deamon();
void initclock();

inline bool pollHUPStatus () { return false; }
inline bool pollCloseRequests () { return false; }
inline void setup_signals (){ return; }
inline void start_signal_thread() {return;}

#endif
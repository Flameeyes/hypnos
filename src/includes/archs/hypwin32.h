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
#ifndef __ARCHS_HYPWIN32_H__
#define __ARCHS_HYPWIN32_H__

#include <winsock.h>
#include <winbase.h>
#include <io.h>
#include <dos.h>
#include <limits.h>
#include <conio.h>
#include <process.h>
#include <errno.h>

char *basename(char *path);

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

bool winSockInit();

typedef int FAR socklen_t ;

char *basename(char *path);

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

#endif
#endif

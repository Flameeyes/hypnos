/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Unix Architecture specific
*/

#ifdef __unix__
#ifndef _HYPNOS_UNIX_
#define _HYPNOS_UNIX_

#include <unistd.h>
#include <termios.h>
#include <unistd.h>

#include <sys/utsname.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>

void initclock();
extern termios termstate ;
extern unsigned long int oldtime, newtime;

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

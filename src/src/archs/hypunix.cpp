/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2003 - NoX-Wizard Project                                  |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifdef __unix__

#include "common_libs.h"
#include "logsystem.h"
#include <fstream>

termios termstate;
unsigned long int oldtime, newtime;

std::string getOSVersionString()
{
#ifdef HAVE_UNAME
	char *temp;
	struct utsname info;
	uname(&info);
	asprintf(&temp, "%s %s on a %s", info.sysname, info.release, info.machine);
	
	std::string s(temp);
	free(temp);
	
	return s;
#else
	return "Unix";
#endif
}

OSVersion getOSVersion()
{
	return OSVER_NONWINDOWS;
}

#endif

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

void sockManageError(int bcode)
{
	LogError("ERROR: Unable to bind socket - Error code: %i\n", bcode);
}

termios termstate;
unsigned long int oldtime, newtime;
	
/*!
\brief Sleeps some milliseconds
\author AnomCwrd

\note thread-safe
*/
void Sleep(unsigned long msec)
{
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = msec * 1000;
	select(0, 0, 0, 0, &timeout);
}

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

//@{
/*!
\name Clock functions
*/

unsigned long initialserversec = 0;
unsigned long initialservermill = 0;

void initclock()
{
	timeval t ;
	gettimeofday(&t,NULL) ; // ftime has been obseloated
	initialserversec = t.tv_sec ;
	initialservermill = t.tv_usec/ 1000 ;
}

uint32_t getclockday()
{
	uint32_t seconds;
	uint32_t days ;
	timeval buffer ;
	gettimeofday(&buffer,NULL) ;
	seconds = buffer.tv_sec ;
	days = seconds/86400 ;  // (60secs/minute * 60 minute/hour * 24 hour/day)
	return days ;
}

uint32_t getclock()
{
	uint32_t milliseconds;
	uint32_t seconds ;
	timeval buffer ;
	gettimeofday(&buffer,NULL) ;
	seconds = buffer.tv_sec ;
	milliseconds = buffer.tv_usec/1000 ;
	if (milliseconds < initialservermill)
	{
		milliseconds = milliseconds + 1000 ;
		seconds  = seconds - 1 ;
	}
	milliseconds = ((seconds - initialserversec) * 1000) + (milliseconds -initialservermill ) ;
	return milliseconds ;
}

/*!
\author Keldan
\since 0.82r3
\brief get current system clock time

used by getSystemTime amx function
*/
uint32_t getsysclock()
{
	uint32_t seconds;
	timeval buffer;
	gettimeofday(&buffer,NULL);
	seconds = buffer.tv_sec;
	return seconds;
}

//@}

#endif

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "clock.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

uint32_t initialServerSecs = 0;
uint32_t initialServermSecs = 0;

/*!
\brief Gets the clock secs and msecs
\param[out] secs Seconds of the clock
\param[out] msecs Milliseconds of the clock

\note This function will automatically use gettimeofday if present or
	the obsolete ftime if not.
*/
void getClock(uint32_t &secs, uint32_t &msecs)
{
#ifdef HAVE_GETTIMEOFDAY
	timeval t;
	gettimeofday(&t,NULL); // ftime has been obseloated
	
	secs = t.tv_sec;
	msec = t.tv_usec/1000;
#elif defined(HAVE_FTIME)
	timeb t;
	::ftime(&t);
	
	secs = t.time;
	msec = t.millitm;
#else
	secs = 0;
	msecs = 0;
	LogError("Can't find a way to have clock data in this platformi, please report this.");
#endif
}

uint32_t getClockDay()
{
	uint32_t secs, dummy;
	getClock(secs, dummy);
	return seconds/DAYS;
}

uint32_t getClockmSecs()
{
	uint32_t secs, msecs;
	
	getClock(secs, msecs);
	
	if ( msecs < initialServermSecs )
	{
		msecs += 1000;
		secs -= 1;
	}
	
	msecs = ((secs - initialServerSecs) * 1000) + (msecs - initialServermSecs);
	
	return msecs;
}

/*!
\author Keldan
\brief Get current system clock time
*/
uint32_t getClockSys()
{
	uint32_t secs, dummy;
	getClock(secs, dummy);
	return secs;
}

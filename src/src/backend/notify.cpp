/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "backend/notify.h"

#include <wefts_mutex.h>
#include <stdarg.h>

/*!
\brief Gets a string representing the current date and time for the log
*/
static const std::string getDate()
{
	time_t TIME;
	tm* T;
	time(&TIME);
	T = localtime(&TIME);

	char *tmp;
	asprintf(&tmp, "[%02d/%02d/%04d %02d:%02d:%02d]",
	T->tm_mday, T->tm_mon+1, T->tm_year+1900, T->tm_hour, T->tm_min, T->tm_sec);
	
	std::string s(tmp);
	free(tmp);

	return s;
}

/*!
\brief Outputs a formatted message at the given level formatting
\param lev Level to output the message at
\param txt Formatting string to output (using \c printf() formatting)
\note This function calls nNotify::output() function after get the right
	formatting for the string
\note This function is thread-safe, a mutex prevent the static buffer to be
	overwritten.
*/
void nNotify::outputf(nNotify::Level lev, char *txt, ...)
{
	static char buffer[2048];
	static Wefts::Mutex m;
	
	m.lock();
	va_list argptr;
	va_start( argptr, txt );
	vsnprintf( buffer, sizeof(buffer)-1, txt, argptr );
	va_end( argptr );
	
	output(lev, buffer);
	
	m.unlock();
}

/*!
\brief Outputs the given string at the given level formatting
\param lev Level to output the string at
\param str String to output

This function calls the right low-level function to output the message, such as
the consoleOutput() function.

\todo Make this a runtime detection
*/
void nNotify::output(nNotify::Level lev, const std::string str)
{
#if ( defined(__unix__) || defined(_CONSOLE) )
	if ( ServerScp::g_nDeamonMode ) return;
	
	consoleOutput(lev, str);
#endif
}

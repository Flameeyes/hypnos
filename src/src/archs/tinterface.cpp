/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "archs/tinterface.h"

#include <sstream>
#include <stdarg.h>

tInterface *tInterface::instance = NULL;

/*!
\brief Constructor

This is the base interface constructor, which sets the instance variable.
*/
tInterface::tInterface()
{
	instance = this;
}

/*!
\brief Outputs a formatted message at the given level formatting
\param lev Level to output the message at
\param txt Formatting string to output (using \c printf() formatting)
\note This function calls virtual function tInterface::output() after get the
	right formatting for the string
*/
void tInterface::outputf(tInterface::Level lev, char *txt, ...)
{
	char *tmp;
	va_list argptr;
	va_start( argptr, txt );
	vasprintf( &tmp, txt, argptr );
	va_end( argptr );
	
	output(lev, buffer);
	free(tmp);
}

/*!
\brief Gets a string representing the current date and time for the log
*/
std::string tInterface::getDate()
{
	time_t TIME;
	tm* T;
	time(&TIME);
	T = localtime(&TIME);
	
	std::ostringsream sout;
	
	sout 	<< "["	<< setw(2) << setfill(0) << T->m_mday
		<< "/"	<< setw(2) << setfill(0) << T->tm_mon + 1
		<< "/"	<< setw(4) << setfill(0) << T->tm_year + 1900
		<< " "	<< setw(2) << setfill(0) << T->tm_hour 
		<< ":"	<< setw(2) << setfill(0) << T->tm_min
		<< ":"	<< setw(2) << setfill(0) << T->tm_sec
		<< "]";

	return sout.str();
}

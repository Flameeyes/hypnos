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
	
	output(lev, tmp);
	free(tmp);
}



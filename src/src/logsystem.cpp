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

#include "dirs.h"
#include "logsystem.h"
#include "inlines.h"
#include "version.h"
#include "archs/tinterface.h"

#ifdef HAVE_STDARG_H
	#include <stdarg.h>
#endif

#ifdef HAVE_SSTREAM
	#include <sstream>
	using std::ostringstream;
#elif HAVE_SSTREAM_H
	#include <sstream.h>
#endif

#ifdef HAVE_FSTREAM
	#include <fstream>
	using std::endl;
#elif defined HAVE_FSTREAM_H
	#include <fstream.h>
#endif

cLogFile *cLogFile::serverLog = NULL;
uint32_t cLogFile::logCounts[4] = {0,0,0,0};

/*!
\brief Constructor of the class
\param str Name of the log file

\todo Should throw exception instead of only output an error
*/
cLogFile::cLogFile(const string &str)
{
	filename = nDirs::getLogsPath() + "/" + name;

	file.open(filename, ios::out|ios::app);

	if( ! file )
		outError("Unable to open/create log file " + filename);
}

/*!
\brief Destructor of the class
\author Anthalir
*/
cLogFile::~cLogFile()
{
	if( file ) file.close();
}

/*!
\brief Write text to the log file
\author Anthalir
\param format msg to write
\remarks a timestamp is added before the string
*/
void cLogFile::Write(char *format, ...)
{
	if( !file ) return;
	
	char *tmp = NULL;
	va_list vargs;

	va_start(vargs, format);
	vasprintf(&tmp, format, vargs);
	va_end(vargs);
	
	Write(tmp);
	
	free(tmp);
}

void cLogFile::Write(string str)
{
	if( !file ) return;

	file << getDateString() << " " << str << endl;
}

/*!
\brief Commodity function for message logging

This function is called by the commodity macros LogMessage, LogWarning,
LogError, LogFatal, from which it takes the type of the message, the file path
(the __FILE__ directive), the line number where the message was thrown
(the __LINE__ directive) and the message to show.

\param type Tyle of the message, see cLogFile::LogType
\param fpath File path (__FILE__)
\param lnum Line number (__LINE__)
\param Message Message to log
*/
void cLogFile::log(cLogFile::LogType type, const char *fpath, int lnum, char *Message, ...)
{
	char *fullMessage = NULL;
	va_list argptr;

	va_start(argptr, Message);
	vasprintf(&fullMessage, Message, argptr);
	va_end(argptr);

	switch( type )
	{
		case logMessage: outInfo(fullMessage); break;
		case logWarning: outWarning(fullMessage); break;
		case logError: outError(fullMessage); break;
		case logCritical: outPanic(fullMessage); break;
	}
	
	logCount[type]++;

	ostringstream sout;
	
	if( type != logMessage )
		sout << "[" << basename(fpath) << ":" << lnum << "] " << fullMessage;
	else
		sout << fullMessage;

	serverLog.Write(fullMessage);
}

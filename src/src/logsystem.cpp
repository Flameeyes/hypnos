/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "logsystem.h"
#include "inlines.h"
#include "settings.h"
#include "version.h"
#include "objects/cpc.h"
#include "objects/cbody.h"
#include "objects/caccount.h"
#include "networking/cclient.h"

#include <stdarg.h>
#include <sstream>

int32_t entries_e=0, entries_c=0, entries_w=0, entries_m=0;
LogFile ServerLog("server.log");

/*!
\brief Write text to the GM log file
\author Anthalir
\param pc current PC
\param msg message
\remarks \remark msg is formatted so you can use "file_%s", my_str as a filename
*/
void WriteGMLog(pChar pc, char *msg, ...)
{
	va_list vargs;
	char *tmp = NULL;

	va_start(vargs, msg);
	vasprintf(&tmp, msg, vargs);
	va_end(vargs);

	LogFile gmlog("GM/%s.log", pc->getBody()->getCurrentName().c_str());
	gmlog.Write(tmp);
	free(tmp);
}

/*!
\brief Constructor of the class
\author Anthalir
\param format name of the log file
*/
LogFile::LogFile(char *format, ...)
{
	char *tmp = NULL;
	va_list vargs;

	va_start(vargs, format);
	vasprintf(&tmp, format, vargs);
	va_end(vargs);

	// add path
	filename = nSettings::Logging::getLogPath() + "/" + tmp;
	free(tmp);

	file = fopen(filename.c_str(), "a");

	if( file==NULL )
	{
		outErrorf("unable to open/create log file %s", filename.c_str());
		return;
	}
}

LogFile::LogFile(std::string name)
{
	filename = nSettings::Logging::getLogPath() + name;

	file = fopen(filename.c_str(), "a");

	if( file==NULL )
	{
		outErrorf("unable to open/create log file %s", filename.c_str());
		return;
	}
}

/*!
\brief Destructor of the class
\author Anthalir
*/
LogFile::~LogFile()
{
	if( file ) fclose(file);
}

/*!
\brief Write text to the log file
\author Anthalir
\param format msg to write
\remarks a timestamp is added before the string
*/
void LogFile::Write(char *format, ...)
{
	if( file==NULL ) return;
	
	char *tmp = NULL;
	va_list vargs;

	va_start(vargs, format);
	vasprintf(&tmp, format, vargs);
	va_end(vargs);

	time_t currtime = time(NULL);
	struct tm* T = localtime(&currtime);

	if ( fprintf(file, "[%02d/%02d/%04d %02d:%02d:%02d] %s", T->tm_mday, T->tm_mon+1, T->tm_year+1900,
		T->tm_hour, T->tm_min, T->tm_sec, tmp) == 0 )
	{
		outErrorf("Unable to write to log file %s", filename.c_str());
	}
	free(tmp);
}

void LogFile::Write(std::string str)
{
	if( file==NULL ) return;

	time_t currtime= time(NULL);
	struct tm* T= localtime(&currtime);

	if ( fprintf(file, "[%02d/%02d/%04d %02d:%02d:%02d] %s", T->tm_mday, T->tm_mon+1, T->tm_year+1900,
		T->tm_hour, T->tm_min, T->tm_sec, str.c_str()) == 0 )
	{
		outErrorf("Unable to write to log file %s", filename.c_str());
	}
}

/*!
\brief Function to be called when a string is ready to be written to the log.
\author LB

Insert access to your log in this function.
\todo Need to get the right ip from the server
*/
static void MessageReady(char logType, char *OutputMessage)
{
	std::ostringstream filename;

//	unsigned long int ip=inet_addr(serv[0][1]);
	uint32_t ip = 0;

	switch (logType)
	{
	case 'E':
		filename << "errors_log_";
		entries_e++;
		break;
	case 'C':
		filename << "critical_errors_log_";
		entries_c++;
		break;
	case 'W':
		filename << "warnings_log_";
		entries_w++;
		break;
	case 'M':
		filename << "messages_log_";
		entries_m++;
		break;
	}

	filename << (ip>>24)&0xFF << "_" << (ip>>16)&0xFF << "_" << (ip>>8)&0xFF << "_" ip&0xFF << ".txt"

	LogFile logerr(filename.c_str());

	if (entries_c == 1 && logType == 'C') // @serverstart, write out verison# !!!
	{
		logerr.Write("\nRunning Hypnos Version: %s\n\n", strVersion);
		logerr.Write("******************************************************************************************************************************************\n");
		logerr.Write("* to increase the stability and quality of this software please send this file to the Hypnos developers - thanks for your support !!! *\n");
		logerr.Write("******************************************************************************************************************************************\n\n");

	}

	if ( (entries_e==1 && logType=='E') || (entries_w==1 && logType=='W') || (entries_m==1 && logType=='M'))
	{
		logerr.Write("\nRunning Hypnos Version: %s\n\n", strVersion);
	}

	logerr.Write("%s", OutputMessage);
}

/********************************************************
 *                                                      *
 *  Rountine to process and stamp a message.            *
 *                                                      *
 ********************************************************/
void LogMessageF(char type, char *fpath, int lnum, char *Message, ...)
{
	char *fullMessage = NULL, *fullMessage2 = NULL;
	va_list argptr;

	va_start(argptr, Message);
	vasprintf(&fullMessage, Message, argptr);
	va_end(argptr);

	switch( type )
	{
//		case 'M': outInfof("%s\n", fullMessage); break;
		case 'W': outWarningf("%s\n",fullMessage); break;
		case 'E': outErrorf("%s\n",fullMessage); break;
		case 'C': outPanicf("%s\n",fullMessage); break;
	}

	if( type != 'M' )
		asprintf(&fullMessage2, "[%s:%d] %s\n", basename(fpath), lnum, fullMessage);
	else
		asprintf(&fullMessage2, "%s\n", fullMessage);

	MessageReady(type, fullMessage2);
}

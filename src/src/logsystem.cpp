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
#include "objects/cpc.h"
#include "objects/cbody.h"
#include "objects/caccount.h"
#include "objects/cclient.h"

#include <stdarg.h>

int32_t entries_e=0, entries_c=0, entries_w=0, entries_m=0;
LogFile ServerLog("server.log");

void ErrOut(char *txt, ...);

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
		ErrOut("unable to open/create log file %s", filename.c_str());
		return;
	}
}

LogFile::LogFile(std::string name)
{
	filename = nSettings::Logging::getLogPath() + name;

	file = fopen(filename.c_str(), "a");

	if( file==NULL )
	{
		ErrOut("unable to open/create log file %s", filename.c_str());
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
		ErrOut("Unable to write to log file %s", filename.c_str());
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
		ErrOut("Unable to write to log file %s", filename.c_str());
	}
}

std::string SpeechLogFile::MakeFilename(pPC pc)
{
	if( !pc ) return "bad npc";
	
	char *tmp;

	asprintf(&tmp, "speech/speech_[%s][%d][%s].txt", pc->getClient()->currAccount()->getName().c_str(), pc->getSerial(), pc->getBody()->getCurrentName().c_str());
	std::string str(tmp);

	free(tmp);
	return str;
}

// SpeechLogFile

/*!
\brief Constructor of speech log file
\author Anthalir
\since 0.82a
\param pc character pointer
*/
SpeechLogFile::SpeechLogFile(pPC pc) : LogFile(MakeFilename(pc))
{

}

std::string CurrentFile;

int CurrentLine;
char LogType= 'M';

void prepareLogs(char type, char *fpath, int lnum)
{
	CurrentFile = std::string(basename(fpath));
	CurrentLine= lnum;
	LogType= type;
}

/*!
\brief Function to be called when a string is ready to be written to the log.
\author LB

Insert access to your log in this function.
\todo Need to get the right ip from the server
*/
static void MessageReady(char *OutputMessage)
{
	char file_name[256];
	char b1[16],b2[16],b3[16],b4[16];
//	unsigned long int ip=inet_addr(serv[0][1]);
	uint32_t ip = 0;
	char i1,i2,i3,i4;

	i1=(char) (ip>>24);
	i2=(char) (ip>>16);
	i3=(char) (ip>>8);
	i4=(char) (ip%256);

	numtostr(i4 , b1);
	numtostr(i3 , b2);
	numtostr(i2, b3);
	numtostr(i1, b4);

	switch (LogType)
	{
	   case 'E': { strcpy(file_name,"errors_log_");          entries_e++; break; }
	   case 'C': { strcpy(file_name,"critical_errors_log_"); entries_c++; break; }
	   case 'W': { strcpy(file_name,"warnings_log_");        entries_w++; break; }
	   case 'M': { strcpy(file_name,"messages_log_");        entries_m++; break; }
	}

	strcat(file_name,b1);strcat(file_name,"_");
	strcat(file_name,b2);strcat(file_name,"_");
	strcat(file_name,b3);strcat(file_name,"_");
	strcat(file_name,b4);strcat(file_name,".txt");

	LogFile logerr(file_name);

	if (entries_c==1 && LogType=='C') // @serverstart, write out verison# !!!
	{
		logerr.Write("\nRunning Hypnos Version: %s\n\n",VERNUMB);
		logerr.Write("******************************************************************************************************************************************\n");
		logerr.Write("* to increase the stability and quality of this software please send this file to the Hypnos developers - thanks for your support !!! *\n");
		logerr.Write("******************************************************************************************************************************************\n\n");

	}

	if ( (entries_e==1 && LogType=='E') || (entries_w==1 && LogType=='W') || (entries_m==1 && LogType=='M'))
	{
		logerr.Write("\nRunning Hypnos Version: %s\n\n",VERNUMB);
	}

	logerr.Write("%s", OutputMessage);
}

/********************************************************
 *                                                      *
 *  Rountine to process and stamp a message.            *
 *                                                      *
 ********************************************************/
void LogMessageF(char *Message, ...)
{
	char *fullMessage = NULL, *fullMessage2 = NULL;
	va_list argptr;

	va_start(argptr, Message);
	vasprintf(&fullMessage, Message, argptr);
	va_end(argptr);

	switch( LogType )
	{
//		case 'M': InfoOut("%s\n", fullMessage); break;
		case 'W': WarnOut("%s\n",fullMessage); break;
		case 'E': ErrOut("%s\n",fullMessage); break;
		case 'C': PanicOut("%s\n",fullMessage); break;
	}

	if( LogType != 'M' )
		asprintf(&fullMessage2, "[%s:%d] %s\n", CurrentFile.c_str(), CurrentLine, fullMessage);
	else
		asprintf(&fullMessage2, "%s\n", fullMessage);

	MessageReady(fullMessage2);
}

void LogSocketError(char* message, int err) {
	#ifndef __unix__
   		LogError("Socket Send error WSA_%i\n", WSAGetLastError());
	#else
   		LogError("Socket Send error %i (%s)\n", err, strerror(err)) ;
	#endif
}


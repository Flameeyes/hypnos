/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Log System stuff
*/

#ifndef __LOGSYSTEM_H__
#define __LOGSYSTEM_H__

#include "common_libs.h"
#include "constants.h"
#include "typedefs.h"

class LogFile;

extern int32_t entries_e, entries_c, entries_w;
extern LogFile ServerLog;	//!< global log object

void WriteGMLog(pChar, char *msg, ...) PRINTF_LIKE(2,3)

/*!
\brief Manage a log file
\author Anthalir
\since 0.82a
*/
class LogFile
{
private:
	FILE *file;		//!< pointer to opened file
	std::string filename;	//!< name of the file

public:
	LogFile(std::string name);
	LogFile(char *format, ...) PRINTF_LIKE(2,3)	//!< path + filename
	~LogFile();
	void Write(std::string str);
	void Write(char *format, ...) PRINTF_LIKE(2,3)
};

/*!
\brief Manage a speech log file
\author Anthalir
\since 0.82a
*/
class SpeechLogFile : public LogFile
{
private:
	pChar pc;
	std::string MakeFilename(pPC pc);

public:
	SpeechLogFile(pPC pc);
};


/*!
\brief Check if new errors has been logged
\retval true New errors had been logged, warn the user
\retval false No new errors had been logged
*/
inline bool NewErrorsLogged()
{ return (entries_e > 0 || entries_c > 0); }

/*!
\brief Check if new warnings has been logged
\retval true New warnings had been logged, warn the user
\retval false No new warnings had been logged
*/
inline bool NewWarningsLogged()
{ return (entries_w > 0); }

void LogMessageF(char *Message, ...) PRINTF_LIKE(1,2)
void prepareLogs(char type, char *fpath, int lnum);

//---------------------------------------------------------------------------
//     Translator macros for functions
//---------------------------------------------------------------------------

#define LogMessage	prepareLogs('M', __FILE__, __LINE__);LogMessageF
#define LogWarning	prepareLogs('W', __FILE__, __LINE__);LogMessageF
#define LogError	prepareLogs('E', __FILE__, __LINE__);LogMessageF
#define LogCritical	prepareLogs('C', __FILE__, __LINE__);LogMessageF

void LogSocketError(char* message, int err);

#endif

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
#include "backend/notify.h"

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

void LogMessageF(char type, char *fpath, int lnum, char *Message, ...) PRINTF_LIKE(4,5)

//---------------------------------------------------------------------------
//     Translator macros for functions
//---------------------------------------------------------------------------

#define LogMessage(...)		LogMessageF('M', __FILE__, __LINE__, __VA_ARGS__)
#define LogWarning(...)		LogMessageF('W', __FILE__, __LINE__, __VA_ARGS__)
#define LogError(...)		LogMessageF('E', __FILE__, __LINE__, __VA_ARGS__)
#define LogCritical(...)	LogMessageF('C', __FILE__, __LINE__, __VA_ARGS__)

void LogSocketError(char* message, int err);

#endif

  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Log System stuff
\author Anthalir
*/

#ifndef __LOGSYSTEM_H
#define __LOGSYSTEM_H

#include "common_libs.h"
#include "constants.h"
#include "typedefs.h"
#include "version.h"

class LogFile;

extern int32_t entries_e, entries_c, entries_w;
extern LogFile ServerLog;	//! global log object

void WriteGMLog(pChar, char *msg, ...);

/*!
\brief Manage a log file
\author Anthalir
\since 0.82a
*/
class LogFile
{
private:
	FILE *file;		//!< pointer to opened file
	char *filename;		//!< name of the file

public:
	LogFile(std::string name);
	LogFile(char *format, ...);	//!< path + filename
	~LogFile();
	void Write(std::string str);
	void Write(char *format, ...);
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
	std::string MakeFilename(pChar pc);

public:
	SpeechLogFile(pChar pc);
};


/*!
\brief Check if new errors has been logged
\return true if new errors
*/
inline bool NewErrorsLogged()
{ return (entries_e > 0 || entries_c > 0); }

/*!
\brief Check if new warnings has been logged
\return true if new warnings
*/
inline bool NewWarningsLogged()
{ return (entries_w > 0); }

void LogMessageF(char *Message, ...);
void prepareLogs(char type, char *fpath, int lnum);

//---------------------------------------------------------------------------
//     Translator macros for functions
//---------------------------------------------------------------------------

#define LogMessage	prepareLogs('M', __FILE__, __LINE__);LogMessageF
#define LogWarning	prepareLogs('W', __FILE__, __LINE__);LogMessageF
#define LogError	prepareLogs('E', __FILE__, __LINE__);LogMessageF
#define LogCritical	prepareLogs('C', __FILE__, __LINE__);LogMessageF

// Spcific error code handling for sockets
void LogSocketError(char* message, int err);

//#define FAULTLOG { LogCritical("Invalidated function for parameters check at line %d\n", nInvalidate); }
//#define REPORTERROR	LogMessage("Error in %s:%d\n", __FILE__, __LINE__);
#endif // __LOGSYSTEM_H

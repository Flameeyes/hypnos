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
/*!
\file
\brief Log System stuff
*/

#ifndef __LOGSYSTEM_H__
#define __LOGSYSTEM_H__

#include "common_libs.hpp"
#include "libhypnos/hypstl/iosfwd.hpp"

/*!
\brief Manage a log file
\author Anthalir
*/
class cLogFile
{
private:
	ostream file;	//!< Log file opened for output

public:
	//! Type of message to log
	enum LogType {
		logMessage,	//!< Simple message
		logWarning,	//!< Warning
		logError,	//!< Error
		logCritical	//!< Critical error
	};

	cLogFile(const string &str);
	~cLogFile();
	
	void Write(const string &str);
	void Write(char *format, ...) PRINTF_LIKE(2,3);
	
	static cLogFile *serverLog;	//!< Global log object
	static void log(LogType type, const char *fpath, int lnum, char *Message, ...) PRINTF_LIKE(4,5);
	
	static uint32_t logCounts[4];	//!< Counter for different type of errors
	
	static bool newErrorsLogged()
	{ return logCounts[logError] || logCounts[logCritical]; }
	
	static bool newWarningsLogged()
	{ return logCounts[logWarning]; }
};

//@{
/*!
\brief Commodity Macros

These functions are called as 'commodity' macros to simplify the call of actual
logging facilities, stating the file name and the line number.
*/
#define LogMessage(...)		cLogFile::log(cLogFile::logMessage, __FILE__, __LINE__, __VA_ARGS__)
#define LogWarning(...)		cLogFile::log(cLogFile::logWarning, __FILE__, __LINE__, __VA_ARGS__)
#define LogError(...)		cLogFile::log(cLogFile::logError, __FILE__, __LINE__, __VA_ARGS__)
#define LogCritical(...)	cLogFile::log(cLogFile::logCritical, __FILE__, __LINE__, __VA_ARGS__)
//@}

// Special logging facility for debugging
#ifndef NDEBUG
	#define SWITCH_FALLOUT LogError("Switch fallout")
#else
	#define SWITCH_FALLOUT
#endif

#endif

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Declaration of Database namespace
*/

#ifndef __DATABASE_H__
#define __DATABASE_H__

#define SQLQUERY(r, query, pzTail, VM, errmsg) \
	if ( ( r = Database::execute(query, pzTail, VM, errmsg) ) != SQLITE_OK ) \
		Database::setFile(__FILE__, __LINE__)

#define SQLFINALIZE(r, VM, errmsg) \
	if ( ( r = sqlite_finalize(VM, errmsg) ) != SQLITE_OK ) \
		Database::setFile(__FILE__, __LINE__)

#define SqlLogMessage	prepareLogs('M', logFile, logLine);LogMessageF
#define SqlLogWarning	prepareLogs('W', logFile, logLine);LogMessageF
#define SqlLogError	prepareLogs('E', logFile, logLine);LogMessageF
#define SqlLogCritical	prepareLogs('C', logFile, logLine);LogMessageF

namespace Database
{
	static ZThread::FastMutex dbMutex;
		//!< Database mutex
	static sqlite *db;
		//!< Database pointer

	static char logFile[50];
		//!< File where the error was thrown
	static int logLine = 0;
		//!< Line where the error was thrown

	void setFile(const char* file, int line);
	void logQuery(int r, const char* query, const char* errmsg);
	void logFinalize(int r, const char* errmsg);

	int execute(const char*query, const char**pzTail, char **errmsg);
};

#endif

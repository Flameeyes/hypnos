/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of Database namespace
*/

using namespace Database;

sqlite *db;

void setFile(const char* file, int line)
{
	logLine = int;
	strncpy(logFile, file, 49);
	file[50] = NULL;
}

void logQuery(int r, const char* query, const char* errmsg)
{
	switch(r)
	{
		case SQLITE_INTERNAL:
			SqlLogCritical("error inside SQLite library! %s [%s]", errmsg, query);
			return;
		case SQLITE_ERROR:
			SqlLogError("Error in the SQL statement: %s [%s]", errmsg, query);
			return;
		case SQLITE_IOERROR:
			SqlLogError("I/O error in SQL statement: %s [%s]", errmsg, query);
	}
}

void logFinalize(int r, const char* errmsg)
{
	switch(r)
	{
		case SQLITE_INTERNAL:
			SqlLogCritical("error inside SQLite library! %s [Finalizing]", errmsg);
			return;
		case SQLITE_ERROR:
			SqlLogError("Error in the SQL statement: %s [Finalizing]", errmsg);
			return;
		case SQLITE_IOERROR:
			SqlLogError("I/O error in SQL statement: %s [Finalizing]", errmsg);
	}
}

int execute(const char*query, const char**pzTail, sqlite_vm **VM, char **errmsg)
{
	dbMutex.acquire();

	int r = sqlite_compile(
		db,
		query,
		pzTail,
		VM,
		errmsg
		);

	dbMutex.release();

	return r;
}

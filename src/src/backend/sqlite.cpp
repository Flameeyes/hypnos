/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "logsystem.h"
#include "backend/sqlite.h"

cSQLite *globalDB;

/*!
\brief Constructor for SQLite class
\param filename Name of the file which stores the sqlite database
*/
cSQLite::cSQLite(std::string filename)
{
	flags = 0;
	litedb = NULL;
	
	if ( ! arch::fileExists )
	{
		LogMessage("File %s non-existant, creating one...", filename.c_str());
		if ( ! arch::ensureDirectory(filename) )
		{
			LogCritical("Error ensuring directory path for %s. Closing up...", filename.c_str());
			setFlag(flagAborted, true);
			return;
		}
	}
	char *errmsg;
	litedb = sqlite_open(filename.c_str(), 0, &errmsg);
	// As of sqlite 2.8.13 the documentation still tell the second parameter is ignored.
	// Check if this is true, and eventually fix it if not.
	if ( ! litedb )
	{
		LogCritical("Error opening sqlite database: %s", *errmsg);
		setFlag(flagAborted, true);
		if ( errmsg )
			sqlite_freemem(errmsg)
		return;
	}
}

cSQLite::~cSQLite()
{
	if ( litedb )
		sqlite_close(litedb);
}

/*!
\brief Executes a query
\param query Query to execute
\return a pointer to a compiled and executed query

This function calls the facilities of sqlite to compile the query into a vm.
\note This function is thread safe
*/
cSQLite::pSQLiteQuery cSQLite::execQuery(std::string query)
{
	mutex.acquire();
	char * errmsg;
	sqlite_vm *vm;
	int result = sqlite_compile(litedb, query.c_str(), NULL, &vm, &errmsg);
	if ( ! result )
	{
		pSQLiteQuery ret = new pSQLiteQuery(vm);
		mutex.release();
		return ret;
	} else {
		LogError("SQLite error executing query %s: %d", query.c_str(), result);
		mutex.release();
		return NULL;
	}
}

void cSQLite::createDatabaseSchema()
{
	pSQLiteQuery q;
	
	q = execQuery("CREATE TABLE accounts ("
		      "id INT NOT NULL,"
		      "name VARCHAR(30) NOT NULL,"
		      "password VARCHAR(50) NOT NULL,"
		      "cryptotype INT NOT NULL,"
		      "privlevel INT NOT NULL,"
		      "creationdate INT NOT NULL,"
		      "banAuthor INT NOT NULL,"
		      "banReleaseTime INT NOT NULL,"
		      "jailtime INT NOT NULL,"
		      "lastConnIP INT NOT NULL,"
		      "lastConnTime INT NOT NULL,"
		      "lastchar INT NOT NULL,"
		      "PRIMARY KEY(id)"
		      ")");
	if ( q )
		delete q;
	
	q = execQuery("CREATE TABLE charAccounts ("
		      "account INT NOT NULL,"
		      "char INT NOT NULL,"
		      "PRIMARY KEY(account, char)"
		      ")");
	
	if ( q )
		delete q;
}

/*!
\brief Constructor for cSQLiteQuery class
\param nvm SQLite VM with a compiled query
*/
cSQLite::cSQLiteQuery::cSQLiteQuery(sqlite_vm *nvm)
{
	vm = nvm;
	flags = 0;
}

/*!
\brief Destructor for cSQLiteQuery class
\note This function finalize the supplied vm
*/
cSQLite::cSQLiteQuery::~cSQLiteQuery()
{
	char *errmsg;
	int ret = sqlite_finalize(vm, &errmsg);
	if (!ret)
		LogError("Error finalizing query: %d - %s", ret, errmsg);
}

/*!
\brief Fetch a new row from the results
\return true if the fetch was done correctly, else false
\note Please used the other provided functions to get values and datatypes
\note If flagArchiveResults is set, the results will be archived in the
 instance
*/
bool cSQLite::cSQLiteQuery::fetchRow()
{
	int columns;
	const char **coldata, **colnames;
	bool setColumnNames = ! flags & flagStarted;
	
	int ret = sqlite_step(vm, &columns, &coldata, setColumnNames ? &colnames : NULL);
	bool complete;
	switch(ret)
	{
		case SQLITE_BUSY:
			ZThread::Thread::wait(sqliteBusyTimeout);
			return fetchRow();
		case SQLITE_ERROR:
		case SQLITE_MISUSE:
			LogError("Error stepping query... %d", ret);
			return false;
		default:
			complete = ret == SQLITE_DONE;
	}
	flags |= flagStarted;
	
	setColumnNames &&
		columnNames.resize(columns);
	
	for(register int i = 0; i < columns; i++)
	{
		setColumnNames &&
		columnNames[i] = colNames[i];
		thisRow[columnNames[i]] = std::string(colData[i]);
	}

	if ( flags & flagArchiving )
		totalResults.push_back(thisRow);
	
	return complete;
}



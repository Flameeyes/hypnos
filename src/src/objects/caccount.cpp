/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cAccount class
*/

#include <backend/sqlite.h>

static ZThread::FastMutex cAccount::global_mt;
static cAccounts cAccount::accounts;

/*!
\brief Save all accounts

This function iterates on cAccount::accounts and run cAccount::save()
function for all the accounts, saving them into the SQLite database.

\note This function acquires cAccount::global_mt
*/
static void cAccount::saveAll()
{
	global_mt.acquire();

	for(tAccounts::iterator it = accounts.begin(); it != accounts.end(); it++)
		(*it)->save();

	global_mt.release();
}

/*!
\brief Load all accounts

This function load all the accounts from the SQLite database and create the
cAccount objects in the cAccount::accounts hashmap.

\note This function acquires cAccount::global_mt
*/
static void cAccount::loadAll()
{
	global_mt.acquire();
	
	cSQLite::pSQLiteQuery q = globalDB->execQuery("SELECT * FROM accounts");

	while( q->fetchRow() )
		accounts.add(cAccount(q->getLastRow()));
	
	delete q;
	
	global_mt.release();
}

/*!
\brief Consstructor from database
\param row The row returned by SQLite query

This function creates the account reading the 16 columns from the
supplied row.
*/
cAccount::cAccount(cSQLite::cSQLiteQuery::tRow r)
{
	name		= r["name"];
	password	= r["password"];
	cryptotype      = atoi(r["cryptotype"]);
	privlevel       = atoi(r["privlevel"]);
	creationdate    = atoi(r["creationdate"]);
	
	banAuthor       = cChar::findBySerial( atoi(r["banAuthor"]) );
	banReleaseTime  = atoi(r["banReleaseTime"]);
	jailtime	= atoi(r["jailtime"]);
	lastConnIP      = atoi(r["lastConnIP"]);
	lastConnTime    = atoi(r["lastConnTime"]);
	
	char *buffer;
	asprintf(buffer, "SELECT char FROM charAccounts WHERE account = %d", r["id"]);
	cSQLite::pSQLiteQuery q = globalDB->execQuery(buffer);
	free(buffer);
	
	if ( ! q )
		LogError("Error executing query %s, no char loaded for account %s", buffer, r["name"]);
	else
	{
		pChar pc;
		while(q->fetchRow())
		{
			cSQLite::cSQLiteQuery::tRow c = q->getLastRow();
			if ( pc = cChar::findBySerial( atoi(c["char"]) ) )
				chars.add(pc);
		}
		
		delete q;
	}
	
	lastchar = NULL;
	if ( pc = cChar::findBySerial( atoi(r["lastchar"]) ) && find(chars.begin(), chars.end(), pc) != chars.end() )
		lastchar = pc;
	
	currentChar = NULL;
}

/*!
\brief Save an account into database

This function insert into the database the needed row for the account.

\brief This function acquires Database::dbMutex
*/
void cAccount::save(int id)
{
	static char buffer[512];
	sprintf(buffer, "INSERT INTO accounts VALUES("
			"%u, '%s', '%s', %u, %u, %u,"
			"%u, %u, %u, %u, %u, %u)"
		name.c_str(),
		password.c_str(),
		cryptotype,
		creationdate,
		banAuthor ? banAuthor->getSerial() : 0,
		banReleaseTime,
		jailtime,
		lastConnIP,
		lastConnTime,
		lastchar
		);
	
	cSQLite::pSQLiteQuery q = globalDB->execQuery(buffer);
	if ( q )
		delete q;
	
	for( std::list<pChar>::iterator it = chars.begin(); it != chars.end(); it++)
	{
		sprintf(buffer, "INSERT INTO charAccounts VALUES(%u, %u)", id, (*it)->getSerial());
		q = globalDB->execQuery(buffer);
		if ( q )
			delete q;
	}
}


pPC cAccount::getChar(int index)
{
	if (index<0) || (index >= getCharsNumber()) ) return NULL;
	std::list<pChar>::iterator it = chars.front();
        for(int ind2 = 0; ind2 < index; ++ind2) { ++it; }
        return (pPC) *it;
}

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

#include <iostream>

#include "database.h"

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

	sqlite_vm *VM;
	const char*pzTail;
	char *errmsg;
	static const char *query = "SELECT * FROM accounts";
	int r;
	SQLQUERY(r, query, &pzTail, &VM, &errmsg);
	if ( r != SQLITE_OK )
		Database::logQuery(r, query, errmsg);

	int N;
	char **coldata;
	char **colnames;
	while( sqlite_step(VM, &N, &coldata &colnames) == SQLITE_ROW )
	{
		if ( N != 16 )
		{
			LogError("Invalid column number %n", N);
			continue;
		}

		accounts.add(cAccount(coldata));
	}

	SQLFINALIZE(r, VM, &errmsg);
	if ( r != SQLITE_OK )
		Database::logFinalize(r, errmsg);

	global_mt.release();
}

/*!
\brief Consstructor from database
\param row The row returned by SQLite query

This function creates the account reading the 16 columns from the
supplied row.
*/
cAccount::cAccount(const char **row)
{
	name = row[0];
	password = row[1];
	cryptotype = atoi(row[2]);
	privlevel = atoi(row[3]);
	creationdate = atoi(row[4]);

	ban_author = cChar::findBySerial(atoi(row[5]));
	ban_releasetime = atoi(row[6]);
	jailtime = atoi(row[7]);
	lastconn_ip = atoi(row[8]);
	lastconn_time = atoi(row[9]);

	pChar pc = cChar::findBySerial(atoi[10]);
	if ( pc )
		chars.add(pc);
	pc = cChar::findBySerial(atoi[11]);
	if ( pc )
		chars.add(pc);
	pc = cChar::findBySerial(atoi[12]);
	if ( pc )
		chars.add(pc);
	pc = cChar::findBySerial(atoi[13]);
	if ( pc )
		chars.add(pc);
	pc = cChar::findBySerial(atoi[14]);
	if ( pc )
		chars.add(pc);

	lastchar = NULL;
	pc = cChar::findBySerial(atoi[15]);
	if ( pc && find(chars.begin(), chars.end(), pc) != chars.end() )
		lastchar = pc;

	currentChar = NULL;
}

/*!
\brief Save an account into database

This function insert into the database the needed row for the account.

\brief This function acquires Database::dbMutex
*/
void cAccount::save()
{
	char *errmsg;

	UI32 tchars[5] = { 0, 0, 0, 0, 0 }; int i = 0;
	for( std::list<pChar>::iterator it = chars.begin(); it != chars.end(); it++)
		tchars[i++] = (*it)->getSerial();

	Database::dbMutex.acquire();
	int r = sqlite_exec_printf(
		Database::db,
		"INSERT INTO accounts VALUES("
		"'%q', '%q', '%u', '%u', '%n', '%u', '%n', '%n',"
		"'%u', '%n', '%u', '%u', '%u', '%u', '%u', '%u')",
		0,
		0,
		&errmsg,
		name.c_str(),
		password.c_str(),
		cryptotype,
		privlevel,
		creationdate,
		ban_author ? ban_author->getSerial() : 0,
		ban_releasetime,
		jailtime,
		lastconn_ip,
		lastconn_time,
		tchars[0],
		tchars[1],
		tchars[2],
		tchars[3],
		tchars[4],
		lastchar
		);

	if ( r != SQLITE_OK )
	{
		Database::setFile(__FILE__, __LINE__);
		Database::logQuery(r, errmsg);
	}
	Database::dbMutex.release();

	sqlite_freemem(errmsg);
}

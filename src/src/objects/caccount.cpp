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

static ZThread::FastMutex cAccount::global_mt;

static void cAccount::saveAll()
{
	global_mt.acquire();

	for(tAccounts::iterator it = accounts.begin(); it != accounts.end(); it++)
		(*it)->save();

	global_mt.release();
}

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



	SQLFINALIZE(r, VM, &errmsg);
	if ( r != SQLITE_OK )
		Database::logFinalize(r, errmsg);

	global_mt.release();
}
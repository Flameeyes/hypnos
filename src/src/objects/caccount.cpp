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
static cAccounts cAccount::accounts;

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


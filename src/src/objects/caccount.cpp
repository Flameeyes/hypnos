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

#include "objects/caccount.h"
#include "objects/cserializable.h"
#include "objects/cpc.h"
#include "settings.h"
#include "inlines.h"

ZThread::FastMutex cAccount::global_mt;
cAccounts cAccount::accounts;

/*!
\brief Save all accounts

This function iterates on cAccount::accounts and run cAccount::save()
function for all the accounts, saving them into the SQLite database.

\note This function acquires cAccount::global_mt
*/
void cAccount::saveAll()
{
	global_mt.acquire();

	uint32_t id = 0;
	for(cAccounts::iterator it = accounts.begin(); it != accounts.end(); it++)
		(*it).second->save(id++);

	global_mt.release();
}

/*!
\brief Load all accounts

This function load all the accounts from the SQLite database and create the
cAccount objects in the cAccount::accounts hashmap.

\note This function acquires cAccount::global_mt
*/
void cAccount::loadAll()
{
	global_mt.acquire();
	
	cSQLite::pSQLiteQuery q = globalDB->execQuery("SELECT * FROM accounts");

	while( q->fetchRow() )
	{
		pAccount acc = new cAccount(q->getLastRow());
		accounts[acc->getName()] = acc;
	}
	
	delete q;
	
	global_mt.release();
}

/*!
\brief Finds an account with the given name
\param name Name of the account to find
\return Pointer to the found account or NULL if not found
*/
pAccount cAccount::findAccount(std::string name)
{
	return accounts[name];
}

cAccount::cAccount(std::string aName, std::string aPassword)
{
	chars = PCVector( nSettings::Server::getMaximumPCs(), NULL );
	name = aName;
	password = aPassword;
	ctype = cryptoPlain;
	privlevel = 1;
	
	banAuthor = NULL;
	banReleaseTime = 0;
	jailtime = 0;
	lastConnIP = 0;
	lastConnTime = 0;
	
	client = NULL;
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
	ctype		= (CryptoType)atoi(r["cryptotype"]);
	privlevel       = atoi(r["privlevel"]);
	creationdate    = atoi(r["creationdate"]);
	
	banAuthor       = dynamic_cast<pPC>( cSerializable::findBySerial(atoi(r["banAuthor"])) );
	banReleaseTime  = atoi(r["banReleaseTime"]);
	jailtime	= atoi(r["jailtime"]);
	lastConnIP      = atoi(r["lastConnIP"]);
	lastConnTime    = atoi(r["lastConnTime"]);
	
	chars = PCVector( nSettings::Server::getMaximumPCs(), NULL );
	
	char *buffer;
	asprintf(&buffer, "SELECT char FROM charAccounts WHERE account = %d", r["id"].c_str());
	cSQLite::pSQLiteQuery q = globalDB->execQuery(buffer);
	free(buffer);
	
	if ( ! q )
	{
		LogError("Error executing query %s, no char loaded for account %s", buffer, r["name"].c_str());
	} else {
		int i = 0;
		pPC pc;
		while(q->fetchRow())
		{
			cSQLite::cSQLiteQuery::tRow c = q->getLastRow();
			chars[i++] = dynamic_cast<pPC>( cSerializable::findCharBySerial( atoi(c["char"]) ) );
		}
		
		delete q;
	}
	
	pPC pc = dynamic_cast<pPC>( cSerializable::findCharBySerial( atoi(r["lastchar"]) ) );
	lastchar = pc;
	
	client = NULL;
}

/*!
\brief Save an account into database

This function insert into the database the needed row for the account.

\note This function acquires Database::dbMutex
*/
void cAccount::save(uint32_t id)
{
	char *buffer;
	asprintf(&buffer, "INSERT INTO accounts VALUES("
			"%u, '%s', '%s', %u, %u, %u,"
			"%u, %u, %u, %u, %u, %u)",
		name.c_str(),
		password.c_str(),
		ctype,
		creationdate,
		banAuthor ? banAuthor->getSerial() : 0,
		banReleaseTime,
		jailtime,
		lastConnIP,
		lastConnTime,
		lastchar
		);
	
	cSQLite::pSQLiteQuery q = globalDB->execQuery(buffer);
	free(buffer);
	if ( q )
		delete q;
	
	for( PCVector::iterator it = chars.begin(); it != chars.end(); it++)
	{
		sprintf(buffer, "INSERT INTO charAccounts VALUES(%u, %u)", id, (*it)->getSerial());
		q = globalDB->execQuery(buffer);
		if ( q )
			delete q;
	}
}

/*!
\brief Gets the character at the given index
\param index Index of the character to find
\return A pointer to the playing character found or NULL if not found
\note index isn't the real index, but the ordinal of the character in the account
\todo Check if needed to provide the ordinal, else change it to the real index (faster)
*/
pPC cAccount::getChar(uint8_t index)
{
	if ( index >= getCharsNumber() )
		return NULL;
	
	for(PCVector::iterator it = chars.begin(); it != chars.end(); it++)
	{
		if ( (*it) )
		{
			if ( ! index ) return *it;
			index--;
		}
	}
}

/*!
\brief Adds a character to an account
\param pc Playing character to add
\return The index where the char is added or 255 if no more space
*/
uint8_t cAccount::addCharToAccount(pPC pc)
{
	uint8_t index = 0;
	for(PCVector::iterator it = chars.begin(); it != chars.end(); it++)
	{
		if ( ! (*it) )
		{
			(*it) = pc;
			return index;
		}
		index++;
	}
	
	return 255;
}

/*!
\brief Gets the count of the charactes in the account
\return The number of actual charactes in account
*/
const uint8_t cAccount::getCharsNumber() const
{
	uint8_t count = 0;
	for(PCVector::const_iterator it = chars.begin(); it != chars.end(); it++)
		if ( (*it) )
			count++;
	return count;
}

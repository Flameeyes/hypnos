/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "logsystem.h"
#include "inlines.h"
#include "archs/tinterface.h"
#include "extras/jails.h"
#include "libhypnos/cvariant.h"
#include "networking/cclient.h"
#include "objects/caccount.h"
#include "objects/cbody.h"
#include "objects/cpc.h"

#include <mxml.h>
#include <wefts_mutex.h>

namespace nJails {
	static LocationList jails;		//!< List of locations usable for jails
	static LocationList::iterator currentJail = jails.end();
						//!< Current jail used
	static Wefts::Mutex mutex;		//!< Mutex for the load of jails
	
	static PCList jailedPlayers;		//!< List of jailed players
	static AccountList jailedAccounts;	//!< List of jailed accounts
	
	static void checkJailedPlayers();
	static void checkJailedAccounts();
	static void release(pAccount acc);
	static void release(pPC pc);
}

//! Loads the jails data from the jails.xml file
void nJails::loadJails()
{
	mutex.lock();

	outPlain("Loading jails data...\t\t");
	
	ifstream xmlfile("config/jails.xml");
	try {
		MXML::Document doc(xmlfile);

		//!\todo Check this when decided for global or local settings
// 		if ( doc.main()->hasAttribute("jailtype") )
// 			jailAccounts = doc.main()->getAttribute("jailtype" )
		
		MXML::Node *n = doc.main()->child();
		if ( ! n ) return;
		do {
			if ( n->name() != "jail" )
			{
				LogWarning("Unknown element in jails.xml: %s", n->name().c_str());
				continue;
			}
			
			try {
				sLocation loc;
				loc.x = cVariant( n->getAttribute("x") ).toUInt16();
				loc.y = cVariant( n->getAttribute("y") ).toUInt16();
				loc.z = cVariant( n->getAttribute("z") ).toSInt8();
				
				if ( loc == sLocation(0,0,0) || ! isValidCoord(loc) )
				{
					LogWarning("Invalid jail location (%hu,%hu,%hhd)", loc.x, loc.y, loc.z);
					continue;
				}
				
				jails.push_back(loc);
			} catch ( MXML::NotFoundError e) {
				LogWarning("Incomplete node in jails.xml");
			}
			
		} while( (n = n->next()) );
		
		if ( ! jails.size() )
		{
			outPlain("[ Failed ]\n");
			LogCritical("No valid jails found on jails.xml");
			mutex.unlock();
			return;
		}
		
		currentJail = jails.begin();
		
		outPlain("[   OK   ]\n");
	} catch ( MXML::MalformedError e) {
		outPlain("[ Failed ]\n");
		LogCritical("jails.xml file not well formed.");
	}
	mutex.unlock();
}

/*!
\brief Do the periodic check for jailed accounts to teleport to the jail or
	to release
\see checkJailedAccounts()
\see checkJailedPlayers()
*/
void nJails::checkJailed()
{
	if ( nSettings::Jails::isJailAccountLevel() )
		checkJailedAccounts();
	else
		checkJailedPlayers();
}

void nJails::checkJailedPlayers()
{
	if ( nSettings::Jails::isJailAccountLevel() )
		return;
	
	for(PCList::iterator it = jailedPlayers.begin(); it != jailedPlayers.end(); it++)
	{
		if ( ! (*it)->getJailInfo() )
		{
			release(*it);
			return;
		}
		
		if ( (*it)->getJailInfo()->getReleaseTime() && TIMEOUT( (*it)->getJailInfo()->getReleaseTime() ) )
		{
			release(*it);
			continue;
		}
		
		if ( (*it)->getBody()->getPosition() != (*it)->getJailPositions()->jailed )
			(*it)->MoveTo((*it)->getJailPositions()->jailed);
	}
}

void nJails::checkJailedAccounts()
{
	for(AccountList::iterator it = jailedAccounts.begin(); it != jailedAccounts.end(); it++)
	{
		if ( ! (*it)->getJailInfo() )
		{
			release(*it);
			return;
		}
		
		if ( (*it)->getJailInfo()->getReleaseTime() && TIMEOUT( (*it)->getJailInfo()->getReleaseTime() ) )
		{
			release(*it);
			continue;
		}
		
		pPC pc = NULL;
		if ( (*it)->currClient() && ( pc = (*it)->currClient()->currChar() ) &&
			pc->getBody()->getPosition() != pc->getJailPositions()->jailed )
				pc->MoveTo(pc->getJailPositions()->jailed);
	}
}

const sLocation nJails::getCurrentJail()
{
	mutex.lock();
	
	if ( currentJail == jails.end() && ( ( currentJail = jails.begin() ) == jails.end() ) )
	{
		LogCritical("Trying to jail someone without jails present on jails.xml.. moving it to 10, 10, 0");
		mutex.unlock();
		return sLocation(10, 10, 0);
	}
	
	sLocation ret = *(++currentJail);
	mutex.unlock();
	
	return ret;
}

/*!
\brief Checks if a player is jailed or not
\param pc Player to check if jailed
\return true if the player (or the player's account) is jailed

This function is here (and not inside cPC class) because this checks for the
account also.
*/
bool nJails::isJailed(pPC pc)
{
	if ( nSettings::Jails::isJailAccountLevel() )
	{
		pAccount acc = pc->getAccount();
		if ( ! acc ) return false;
		
		if ( acc->getJailInfo() )
		{
			if ( ! TIMEOUT(acc->getJailInfo()->getReleaseTime()) )
				return true;
			
			release(acc);
		}
	} else if ( pc->getJailInfo() ) {
		if ( ! TIMEOUT(pc->getJailInfo()->getReleaseTime()) )
			return true;
		
		release(pc);
	}
	
	return false;
}

/*!
\brief Releases a client (calling the right release() function)
\param client Client to release
*/
void nJails::release(pClient client)
{
	if ( nSettings::Jails::isJailAccountLevel() )
		release(client->currAccount());
	else
		release(client->currChar());
}

/*!
\brief Releases a Playing Character
\param pc Character to release
\warning This shouldn't be called directly from outside nJails namespace, 'cause
	it fails without error if the jails settings aren't the one requested.
	'release command should use the nJails::release(pClient) function.

\note This function also sysmessage the player if it's online
*/
static void nJails::release(pPC pc)
{
	if ( nSettings::Jails::isJailAccountLevel() )
		return;
	
	delete pc->getJailInfo();
	
	pc->MoveTo( pc->getJailPositions()->original );
	delete pc->getJailPositions();
	
	pc->setJail(NULL, NULL);
	
	PCList::iterator it = find(jailedPlayers.begin(), jailedPlayers.end(), pc);
	if ( it != jailedPlayers.end() )
		jailedPlayers.erase(it);
	
	if ( pc->getClient() )
		pc->getClient()->sysmessage("You're now free");
}

/*!
\brief Releases an account
\param acc Account to release
\warning This shouldn't be called directly from outside nJails namespace, 'cause
	it fails without error if the jails settings aren't the one requested.
	'release command should use the nJails::release(pClient) function.

\note This function also sysmessage the player if it's online
*/
static void nJails::release(pAccount acc)
{
	if ( ! nSettings::Jails::isJailAccountLevel() )
		return;
	
	delete acc->getJailInfo();
	acc->setJailInfo(NULL);
	
	for( register int i = 0; i < acc->getCharsNumber(); i++ )
	{
		pPC pc = acc->getChar(i);
		pc->MoveTo( pc->getJailPositions()->original );
		
		delete pc->getJailPositions();
		pc->setJail(NULL, NULL);
	}
	
	if ( acc->currClient() )
		acc->currClient()->sysmessage("You're now free");
	
	AccountList::iterator it = find(jailedAccounts.begin(), jailedAccounts.end(), acc);
	if ( it != jailedAccounts.end() )
		jailedAccounts.erase(it);
}

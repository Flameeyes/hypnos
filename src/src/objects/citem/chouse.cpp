/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/chouse.h"
#include "objects/cnpc.h"
#include "objects/cpc.h"
#include "objects/cclient.h"
#include "archetypes/generic.h"
#include "map.h"

/*!
\brief Delets a house

This function deletes the player's vendors from the house, removes all the
house's keys and at the end calls the overloaded cMulti::Delete() function to
delete the content of the multi.
*/
void cHouse::Delete()
{
	for(CharSLIst::iterator it = chars.begin(); it != chars.end(); it++)
		if ( (*it)->getAIType() == cNPC::NPCAI_PLAYERVENDOR )
			(*it)->Delete();
		// cChar::Delete() takes care of removing the char from the multi
	
	killKeys();
	
	cMulti::Delete();
}

/*!
\brief Checks if a player is a Co-Owner of the house
\param pc Player to test
\retval true The player is the owner, a character of the same account of the
	owner, or a coowner of the house
\retval false The player isn't known by the house, is a friend or is banned.
*/
bool cHouse::isCoowner(pPC pc) const
{
	if ( pc->getAccount() == getOwner()->getAccount() )
		return true;
	
	PSMap::const_iterator it = playerStatus.find(pc->getSerial());
	if ( it != playerStatus.end() )
		return (*it).second == psCoowner;
	
	return false;
}

/*!
\brief Checks if a player is a Friend of the house
\param pc Player to test
\retval true The player is a friend of the house
\retval false The player is not a friend of the house
\note This functions return only if a player is exactly a friend. To check if
	it's at least a friend use canPerformCommand() function.
*/
bool cHouse::isFriend(pPC pc) const
{
	PSMap::const_iterator it = playerStatus.find(pc->getSerial());
	if ( it != playerStatus.end() )
		return (*it).second == psFriend;
	
	return false;
}

/*!
\brief Checks if a player is banned from the house
\param pc Player to test
\retval true The player is banned from the house
\retval false The player isn't banned from the house
*/
bool cHouse::isBanned(pPC pc) const
{
	PSMap::const_iterator it = playerStatus.find(pc->getSerial());
	if ( it != playerStatus.end() )
		return (*it).second == psBanned;
	
	return false;
}

/*!
\brief Sets the status of a player
\param pc Player to set the status if
\param status Status to set the player to (use psNone to remove from the map)
\retval true Update of the status correctly executed
\retval false Can't update the player's status (can be the owner or a character
	from the same account of the owner).
*/
bool cHouse::setStatus(pPC pc, PlayerStatus status)
{
	if ( getOwner() == pc || pc->getAccount() == getOwner()->getAccount() )
		return false;
	
	if ( status != psNone )
	{
		playerStatus[pc->getSerial()] = status;
		return true;
	}
	
	PSMap::iterator it = playerStatus.find(pc->getSerial());
	if ( it != playerStatus.end() )
		playerStatus.erase(it);
	
	return true;
}

/*!
\brief Retrasform the house into a deed, redeeding also the vendors inside
\param client Client who requested the redeed
*/
void cHouse::redeed(pClient client)
{
	pPC pc = NULL;
	if ( ! client || ! ( pc = client->currChar() ) )
		return;
	
	if ( getOwner() != pc && ! pc->isGM() )
		return;

	pItem deed = nArchetypes::createItem( deedID );
	if ( ! deed ) return;
	deed->setContainer( pc->getBody()->getBackpack() );
	deed->Refresh();
	
	for( CharSList::iterator it = chars.begin(); it != chars.end(); it++ )
	{
		//!\todo Change all this to a %redeed() like function for vendors when new vendor system is done
		assert(*it);
		pNPC npc = dynamic_cast<pNPC>(*it);
		if (	! npc || 
			! getArea().isInside( npc->getBody()->getPosition() ) ||
			npc->getAIType() != cNPC::NPCAI_PLAYERVENDOR
			)
				continue;
		
		pItem vdeed = nArchetypes::createItem("item_deed_vendor");
		if ( ! vdeed ) continue;
		vdeed->setContainer( pc->getBody()->getBackpack() );
		vdeed->setCurrentName("A vendor deed for %s", npc->getBody()->getCurrentName().c_str());
		vdeed->Refresh();
		
		client->sysmessage("Packed up vendor %s.", npc->getBody()->getCurrentName().c_str());
		
		npc->Delete();
	}
	
	killKeys();
	client->sysmessage("All house items and keys removed.");
	
	client->sysmessage("Demolishing House %s", getCurrentName().c_str());
	client->sysmessage("Converted into a %s.", deed->getCurrentName().c_str());

	int8_t newz = mapElevation(pc->getBody()->getPosition());
	pc->getBody()->setPositionZ(newz);
	pc->getBody()->setPositionDZ(newz);
	pc->teleport();
	
	Delete();
	return;
}

/*!
\brief Deletes the keys of a house
\todo This function need to be wrote when the cKey class is done
*/
void cHouse::killKeys()
{
}

/*!
\brief Checks for a house's speech
\param client Client who's performing the speech
\param speech Speech performed
\retval true The \c speech is recognized as a house's speech and is executed
\retval false The \c speech isn't recognized, or the player can't do speech in
	this house
\todo Change this to use a regexp capture using the pcreplus library when
	available
\todo Rewrite the target code when target rewrote
\note Socket and pc checking already done in cClient::talking() so here we
	assert them
*/
bool cHouse::doSpeech(pClient client, const std::string &speech)
{
	assert(client); assert(client->currChar());
	pPC pc = client->currChar();

	// if pc is not a friend or owner, we don't care what he says
	if ( ! canPerformCommand(pc) )
		return false;
	
	// house ban
	if ( speech == "I BAN THEE" )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cCharTarget() );
		targ->code_callback = target_houseBan;
		targ->buffer[0] = pi->getSerial();
		targ->send(client);
		client->sysmessage("Select person to ban from house.");
		return true;
	}
	
	// kick out of house
	if ( speech == "REMOVE THYSELF" )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cCharTarget() );
		targ->code_callback=target_houseEject;
		targ->buffer[0] = getSerial();
		targ->send(client);
		client->sysmessage("Select person to eject from house.");
		return true;
	}
	
	// Lock down item
	if ( speech == "I WISH TO LOCK THIS DOWN" )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->code_callback=target_houseLockdown;
		targ->buffer[0] = getSerial();
		targ->send(client);
		client->sysmessage("Select item to lock down");
		return true;
	}
	
	// Unlock down item
	if ( speech == "I WISH TO RELEASE THIS" )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->code_callback=target_houseRelease;
		targ->buffer[0] = getSerial();
		targ->send(client);
		client->sysmessage("Select item to release");
		return true;
	}
	
	// Secure item
	if ( speech == "I WISH TO SECURE THIS" )
	{
		pTarget targ = clientInfo[socket]->newTarget( new cItemTarget() );
		targ->code_callback=target_houseSecureDown;
		targ->buffer[0] = getSerial();
		targ->send(client);
		client->sysmessage("Select item to secure");
		return true;
	}
	return false;
}

/*!
\brief Checks for houses decay

This function is called by the garbage collector thread to check the houses
decay timer, and then delete them.

This function uses the cHouse::houses list to find out which houses must decay
and which not. For this, it acquires cHouse::housesMutex before checking the
list: trying to add, remove or refresh an house while checking this will make
something very awful.
*/
void cHouse::checkDecay()
{
	housesMutex.lock();
	
	for(HouseSList::iterator it = houses.begin(); it != houses.end())
		if ( TIMEOUT((*it).decaytimer) )
		{
			(*it)->Delete();
			houses.erase(it);
		}
	
	housesMutex.unlock();
}

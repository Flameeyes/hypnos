/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/chouse.h"
#include "objects/cclient.h"

/*!
\brief Checks if a player is a Co-Owner of the house
\param pc Player to test
\retval true The player is the owner, a character of the same account of the
	owner, or a coowner of the house
\retval false The player isn't known by the house, is a friend or is banned.
*/
bool cHouse::isCoowner(pPC pc) const
{
	if ( pc->getAccount() == owner->getAccount() )
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
	if ( owner == pc || pc->getAccount() == owner->getAccount() )
		return false;
	
	if ( status != psNone )
	{
		playerStatus[pc->getSerial()] = status;
		return true;
	}
	
	PSMap::iterator it = playerStatus.find(pc->getSerial());
	if ( it != playerStatus.end() )
		playerstatus.erase(it);
	
	return true;
}

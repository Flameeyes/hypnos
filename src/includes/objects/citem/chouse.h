/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CHOUSE_H__
#define __OBJECTS_CHOUSE_H__

#include "common_libs.h"
#include "objects/citem/cmulti.h"

/*!
\class cHouse chouse.h "objects/citem/chouse.h"
\brief House item class

The house are special items because they are multis, but, like the boats,
needs more data to describe them. For example, they have a serial-indexed map
of players which can be co-owners, friends or banned. They also have a list of
keys to be deleted when the lock of the doors are changed, and data about their
decay.

For this we have to use a new class, which stores the extended data about the
house and has the functions to work with them.

\paragraph house_serial_map About the serials for players' status ma

The playerStatus map is indexed by the serials of the players. This is because
in this case we need to search every time the list, and because a player can be
in more than one list, this time is simpler to search it by the serial (this 
makes it simpler to store on savefiles by the way).

This is also needed to not save the house a player is friend inside the
player's data, so when doing garbage collect we only need to check if serials
are still valid or not, without the risk of a null pointer dereference (but 
actually with a risk of unwanted player's status if the serial is recycled
before a garbage collection is called, this need to be fixed).

\todo Add support for garbage collection of house registers

\see cMulti
\see cBoat
*/
class cHouse : public cMulti
{
public:
	//! Status of a player against a house
	enum PlayerStatus {
		psNone,
		psCoowner,
		psFriend,
		psBanned
	};

public:	
	cHouse();
	cHouse(uint32_t nserial);
	
protected:
	//! Serial-indexed players' status map
	typedef std::map<uint32_t, PlayerStatus> PSMap;
	
	pPC owner;		//!< Owner of the house
	PSMap playerStatus;	//!< Status of registered players
	
	/*!
	\brief House's deed's identifier
	\todo Change this to an archetype pointer
	*/
	std::string deedID;
	
public:
	//! Gets the house's owner
	pPC getOwner() const
	{ return owner; }

	bool isCoowner(pPC pc) const;
	bool isFriend(pPC pc) const;
	bool isBanned(pPC pc) const;
	
	//! Tests if a player is an owner or a friend of the house
	bool canPerformCommand(pPC pc) const
	{ return isCoowner(pc) || isFriend(pc); }
	
	bool setStatus(pPC pc, PlayerStatus status);
	
	void redeed(pClient client);
	void killKeys();
	
	bool doSpeech(pClient client, const std::string &speech);
};

#endif

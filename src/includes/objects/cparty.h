/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CPARTY_H__
#define __OBJECTS_CPARTY_H__

#include "common_libs.h"
#include <wefts_mutex.h>

/*!
\class cParty cparty.h "objects/cparty.h"
\brief Party object
\author Flameeyes

This class is used to accessa and manage the players' parties.

A party is a group of players which can talk one another wherever they are,
avoiding to be listened by other players.
Members of the same party can also loots their own victims and can allow the
others members to loot them if they die (obviously without make them criminals).

To create a party a non-partied PC must ask another PC to join the party, then
it can accept or decline the party. If it's accepted, the party is acknowledged
and so it's 
*/
class cParty {
protected:
	//! Singly-linked list of parties
	typedef std::slist<pParty> PartySList; 
	
	static PartySList parties;	//!< List of all parties formed in the
					//!< Server
					
	static Wefts::Mutex mutex;	//!< Mutex for cParty::parties list
					//!< to disallow concurrent access to
					//!< that.
	
	//! Structure to define a party member
	struct sPartyMember {
		pPC player;	//!< Pointer to the actual member
		bool allowLoot;	//!< If true, the player allow other party
				//!< members to loot his corpse without making
				//!< them criminals
	};
	typedef std::<sPartyMember> MemberSList;
public:
	static void deleteParties();

public:
	cParty(pPC leader, pPC member);
	~cParty();
	
	void inviteMember(pPC member);
	void addMember(pPC member);
	bool removeMember(pPC member);
	void disband();

//@{
/*!
\name Talk and notices

There we have functions used to talk between party's members and for notices to
the party members.

The first case is when an user want to send a message to all the allies, this is
done in game using the / character.

The second case is when we want to inform the party's members of something, like
a player joined a party, or one leaved it, or the party was disbanded.
*/
	void notice(std::string text);
	void notice(char *str, ...) PRINTF_LIKE(2, 3)
	void talk(pPC member, std::string text);
	void talk(pPC member, char *str, ...) PRINTF_LIKE(2,3)
//@}
protected:
	sPartyMember leader;		//!< Party's leader (can kick and add)
	MemberSList members;		//!< Party's fully members
	MemberSList invited;		//!< Party's invited members
};

#endif


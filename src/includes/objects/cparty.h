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
#include "target.h"
#include <wefts_mutex.h>

/*!
\class cParty cparty.h "objects/cparty.h"
\brief Party object
\author Flameeyes

This class is used to accessa and manage the players' parties.

A party is a group of players which can talk one another wherever they are,
avoiding to be listened by other players (this is done in game using the /
character).
Members of the same party can also loots their own victims and can allow the
others members to loot them if they die (obviously without make them criminals).

To create a party a non-partied PC must ask another PC to join the party, then
it can accept or decline the party. If it's accepted, the party is acknowledged
and so it's isntance is sure. If the PC refuses to join the party, it will be
removed from it.
                                                                                   
When removing a PC (invited or already member) from the party, we'll check for
the content of the party: if the total members are less than 2, the party is
disbanded and its instance deleted.

Please also note that parties aren't saved in the worldfile, so after a reboot
of the server the party are resetted, and that the party's won't work with
offline players. However, if a player crashes or lose the connection, it will
remain in the party for a configurable time (it can be 0, so an admin can
disable the maintaining of disconnetted players).

\todo Add a tempfx to invited for timeout on accept, and one on both invited and
	members after a disconnection to remove them fromt he party

This class takes also to parse the party commands from the 0xBF/0x06 packet.
\see nPackets::Received::MiscCommand
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
	typedef std::slist<sPartyMember> MemberSList;
public:
	static void deleteParties();
	static void executeCommand(pClient, const uint8_t *buffer, uint16_t size);

	static void inviteMemberCB(pClient client, sTarget *targ);
	static void removeMemberCB(pClient client, sTarget *targ);

public:
	cParty(pPC leader);
	~cParty();
	
	void inviteMember(pClient client, pPC member);
	void addMember(pPC member);
	bool removeMember(pPC member);
	void disband();

	PCSList getMembersList();
	
	void setCanLootMe(pPC member, bool setting);

protected:
	sPartyMember leader;		//!< Party's leader (can kick and add)
	MemberSList members;		//!< Party's fully members
	PCSList invited;		//!< Party's invited members
public:
	inline pPC getLeader() const
	{ return leader.player; }
};

#endif


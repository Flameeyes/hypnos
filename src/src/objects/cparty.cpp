/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/cparty.h"

/*!
\brief Delete the pointers of the parties.
\author Flameeyes

This function is called when the emulator is exiting, to cleanup the instances
of the parties if they aren't already deleted.

\note The cParty::~cParty() function removes the instance from the list, but as
	we are using lists, the interators aren't invalidated by this.
*/
void cParty::deleteParties()                
{
	for( PartySList::iterator it = parties.begin(); it != parties.end(); it++)
		delete (*it);                                                           
}

/*!
\brief Function used to actually parse the packet 0xBF, subcommand 0x06 for
	parties' commands
\author Flameeyes
\param client Client which are receiving the command from
\param buffer Pointer to the actual package received
\param size Size of the received package

This function is here because there is too many things in the 0xBF packet, and
adding a new switch inside it is a true suicide.

This isn't a clean way to do this, but it's the better one to have all the
party related stuff inside this file.          

\see nPackets::Received::MiscCommand
*/
void cParty::executeCommand(pClient client, char *buffer, uint16_t size)
{
	assert(buffer[0] == '0xBF'); // make sure this is a 0xBF package.
	// Maybe we should also add an assert for make sure it's the true 0x06
	// subcommand
	
	pPC pc = client->currChar();
	if ( ! pc ) return; // MiscCommand doesn't validate it, so we do
	
	char *partyPkg = buffer+5; // Here we are at the start of the party
				   // Subpackage.
	
	switch( partyPkg[0] )
	{
	case 0x01: // Add a member
		{
			uint32_t newMemberSerial = LongFromCharPtr(partyPkg+1);
			if ( ! newMemberSerial )
			{
				// Now we should show to the client a target to select
				// the player to add
			} else {
				pPC newMember = dynamic_cast<pPC>(cSerializable::findBySerial(newMemberSerial));
			
				// If invalid serial
				if ( ! newMember ) return;
			
				if ( ! pc->getParty() )
					new cParty(pc);
			
				pc->getParty()->inviteMember(client, newMember);
			}                          
		}
			break;
		
	case 0x02: // Remove a member
		{
			uint32_t removedSerial = LongFromCharPtr(partyPkg+1);
			if ( ! removedSerial )
			{
			} else {
				pPC removed = dynamic_cast<pPC>(cSerializable::findBySerial(removedSerial));
				
				if (	! removed || // If invalid serial
					! pc->getParty() || // If the requester isn't in a party
					! removed->getParty() || // If the target isn't in a party
					pc->getParty() != removed->getParty() || // If requester and target is in different packets
					pc->getParty()->getLeader() != pc ) // If not the party leader
						return;
				
				pc->getParty()->removeMember(pc);
			}
		}
		break;
		
	case 0x03: // Party Private Message
		{
			uint32_t targetSerial = LongFromCharPtr(partyPkg+1);
			pPC target = dynamic_cast<pPC>(cSerializable::findBySerial(targetSerial));
			if ( ! target ) return;
			
			if ( ! target->getClient() ) return;
			
			if ( ! pc->getParty() || pc->getParty() != target->getParty() )
			{
				client->sysmessage("You aren't in the same party");
				return;
			}
			
			cSpeech msg(partyPkg+5);
			
			nPackets::Sent::PartyPrivateMessage pk(target, msg);
			//! And then resend it to the party
			target->getClient()->sendPacket(&pk);
		}
		break;
		
	case 0x04: // Party broadcast (needs unicode support)
		{
			if  
		}
		break;
	
	case 0x06: // Party can loot me
		{
			pPC pc = client->currChar();
			pc->getParty()->setCanLootMe(pc, partyPkg[1]);
		}
		break;
	case 0x08: // Accept join party invitation
		{
			uint32_t leaderSerial = LongFromCharPtr(partyPkg+1);
			pPC leader = dynamic_cast<pPC>(cSerializable::findBySerial(leaderSerial));
			if ( ! leader ) pc->setParty(NULL);
			
			leader->getParty()->addMember(pc);
		}
		break;
	case 0x09: // Decline join party invitation
		{
			uint32_t leaderSerial = LongFromCharPtr(partyPkg+1);
			pPC leader = dynamic_cast<pPC>(cSerializable::findBySerial(leaderSerial));
			if ( ! leader ) pc->setParty(NULL);
			
			leader->getParty()->removeMember(pc);
		}
		break;
	}
}

/*!
\brief Constructor
\note cParty can only be created having a leader and the first member
\param leader Leader (creator) of the party
\note After creating a new party, immediatly invite a member, else we have an
	invalid party.
\note Constructor register the party in the \ref parties list
*/
void cParty::cParty(pPC leader)
{
	leader->setParty(this);
	
	parties.push_front(this);
}

void cParty::~cParty()
{
	PartySList::iterator it = parties.find(this);
	if  ( it != parties.end() )
		parties.erase(it);
}


/*!
\brief Invites a player to join the party
\param client Client which is inviting the player to join the party
\param member Player whose invited to join the party

\note This function has to be called by the target callback after the party
	instance is find out and finished.
*/                                                      
void cParty::inviteMember(pClient client, pPC member)
{                                  
	assert(client); // We are quite sure we'll never call it with a NULL client
	if ( ! member ) return;	// This can be a bug, just ignore it
	
	std::string name = member->getBody()->getCurrentName();
	
	pClient invclient = NULL;
	if ( ! (invclient = member->getclient()) )
	{
		client->sysmessage("%s is not logged on", name.c_str());
		return;
	}
		
	
	if ( member->getParty() )
	{
		if ( member->getParty() == this )
			client->sysmessage("%s is already in this party", name.c_str())
		else
			client->sysmessage("%s is already in a party", name.c_str());
		return;
	}
	
	PCSList::iterator it = find(member, invited.begin(); invited.end());
	if ( it != invited.end() )
	{
		client->sysmessage("%s is already invited to join the party", name.c_str());
		return;
	}
	
	member->setParty(this);
	
	invclient->sysmessage("%s has just invited you to join his party.", client->currChar()->getBody()->getCurrentName().c_str());
	invclient->sysmessage("Type /accept to join the party, or /decline to decline the invitation");
	                              
	//!\todo Now we should change this to work as a targeted thing, I think
	//!\todo Also, we need to set a timer after which 
}

void cParty::addMember(pPC member);

/*!
\brief Removes a member from the party
\param member Member to remove from the party.
\retval true The party was disbanded
\retval false The party wasn't disbanded

This function remvoes a player from the party, and can disband the party itself            
in two cases:
	\li When the leader player leaves the party
	\li When the last player (but the leader) leaves the party

\note If the function returned true, the pointer will be dirty: the instance of 
	the party will be deleted.
\see cParty::disband()
*/
bool cParty::removeMember(pPC member)
{
	notice("%s left the party", member->getBody()->getCurrentName().c_str());
	
	if ( leader == member )
	{
		disband();
		return true;
	}
	
	MemberSList::iterator it = find(member, members.begin(), members.end()); 
	if ( it != members.end() )
		members.delete(it);
	                                                    
	PCSList::iterator it2 = find(member, invited.begin(), invited.end());
	if  ( it2 != invited.end() )
		invited.delete(it2);

	uint8_t totalmembers = invited.size() + members.size() + 1; // (+1 = leader)
	if ( totalmembers <= 1 ) // Should never be zero for the +1, but test for it :)
	{
		disband();
		return true;              
	}
	
	member->setParty(NULL);
	//! \todo Send the remove from the party status to the player
}                                      

/*!
\brief Disbands a party
                                              
This function disbands a party, notifying it to all the players inside the party
itself and then deleting the instance

\note After call this function, the pointer is no more valid, trying to access
	it after this, will cause an invalid access. Please be sure of what you
	are doing.
*/
void cParty::disband()      
{                                       
	for( PCSList::iterator it = invited.begin(); it != invited.end(); it++)
	{
		(*it)->setParty(NULL);
		//! \todo Here we should make the invites timeout and fail
		
		if ( (*it)->getClient() )    
			(*it)->getClient()->sysmessage("The party you were invited to join is now disbanded");
	}
	
	if ( leader )
		members.push_front(leader); // Lets use the leader as any other member from here
	                
	for( MemberSList::iterator it = members.begin(); it != members.end(); it++)
	{
		(*it).player->setParty(NULL);
		//! \todo Here we should send the client the disband status of the party itself
		
		if ( (*it).player->getClient() )         
			(*it).player->getClient()->sysmessage("The party you were in is now disbanded.");
	}
	
	// Last thing to do.. suicide :)
	delete this;
}

/*!
\brief Gets a simple list of PCs in the party

This function fills a list with the pointers of the party's members to be used
by packets which needs to send the members' list.

\see nPackets::Sent::PartyAddMember
\see nPackets::Sent::PartyRemoveMember
*/
PCSList cParty::getMembersList()
{
	PCSList ret;
	for( MemberSList::iterator it = members.begin(); it != members.end(); it++)
			ret.push_front( (*it).player );

	return ret;
}

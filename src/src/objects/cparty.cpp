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
\brief Constructor
\note cParty can only be created having a leader and the first member
\param leader Leader (creator) of the party
\param member First member invited to the party. The party can't be constructed
	without having a member
\note Constructor register the party in the \ref parties list
*/
void cParty::cParty(pPC leader, pPC member)
{
	
	parties.push_front(this);
}

void cParty::~cParty()
{
	PartySList::iterator it = parties.find(this);
	if  ( it != parties.end() )
		parties.erase(it);
}


void cParty::inviteMember(pPC member);

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
*/
bool cParty::removeMember(pPC member)
{
	if ( leader == member )
	{
		disband();
		return true;
	}
}

void cParty::disband();


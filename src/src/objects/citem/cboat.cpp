/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/cboat.h"
#include "objects/cclient.h"

/*!
\brief Ship items for directions

This table contains the IDs for the ships' items for a definite direction.
The first index (0..3) is the direction the ship is oriented.
The second index (0..5) is the item we want to get the id of
\see cBoat::Item
*/
const uint8_t cBoat::ShipItems[4][6]=
{
 {0xB1,0xD5,0xB2,0xD4,0xAE,0x4E},
 {0x8A,0x89,0x85,0x84,0x65,0x53},
 {0xB2,0xD4,0xB1,0xD5,0xB9,0x4B},
 {0x85,0x84,0x8A,0x89,0x93,0x50}
};

pBoat cBoat::searchByPlank(pItem pl)
{
	if ( ! pl )
		return NULL;
	
	return dynamic_cast<pBoat>( cSerializable::findBySerial(pl->more1.more) );
}

cBoat::cBoat() : cItem()
{
}

cBoat::cBoat(uint32_t nserial) : cItem(nserial)
{
}

/*!
\brief Moves the board one tile in the direction specified
\param client Client who requested the move
\param dir Direction to move the boat (if not specified, move it in the direction it is)
*/
void cBoat::step(pClient client, uint8_t dir)
{
	static sRect navigable( XBORDER, YBORDER, (map_width*8)-XBORDER, (map_height*8)-YBORDER );
	
	client->pause();

	if ( dir == 0xFF ) dir = getDirection();
	sLocation boatpos = getPosition();
	boatpos.move(dir&0x07, 1);

	if ( ! navigable.isInside(boatpos) )
	{
		type2=0;
		tillerMan->talk("Arr, Sir, we've hit rough waters!");
		client->resume();
		return;
	}

	if( ! good_position(this, boatpos, 0) )
	{
		type2=0;
		tillerMan->talk("Arr, somethings in the way!");
		client->resume();
		return;
	}
	if( collision(this, boatpos,0) )
	{
		type2=0;
		tillerMan->talk("Arr, another ship in the way");
		client->resume();
		return;
	}

	// The special items are also multi's items, so this moves all
	MoveTo( boatpos );

	client->resume();
}

/*!
\brief Turns the boat
\param turnRight If true, the boat will be turned right, else left
*/
void cBoat::turn(bool turnRight)
{
	ClientSList toResume;
	
	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		pClient ps_i = sw.getClient();
		
		if ( ! ps_i )
			continue;
		
		if( distFrom(ps_i->currChar()) > BUILDRANGE)
			continue;
			
		toResumes.push_back(ps_i);

		ps_i->pause();
	}

	uint8_t id1 = getId() >> 8;
	uint8_t id2 = getId() & 0xFF;
	
	if(turnRight)
	{
		setDirection(getDirection()+2);
		id2++;
	} else {
		setDirection(getDirection()-2);
		id2--;
	}
	
	//! \todo Try to figure out what this is!
	if(id2 < more1.moreb1)
		id2+=4;//make sure we don't have any id errors either
	if(id2 > more1.moreb2)
		id2-=4;//Now you know what the min/max id is for :-)

	setId( id1 << 8 | id2 );//set the id

	if(id2==pi->more1.moreb1)
		pi->setDirection(0);//extra DIR error checking
	if(id2==pi->more1.moreb2)
		pi->setDirection(6);

	//Set the DIR for use in the Offsets/IDs array
	uint8_t dir = (pi->getDirection() &0x07)/2;

	const uint8_t *pShipItems = ShipItems[ dir ];

	plankLeft->setId( plankLeft->getId() | pShipItems[idPortPlankClosed] );//change the ID
	plankRight->setId( pplankRight->getId() | pShipItems[idStarPlankClosed] );
	tillerMan->setId( tillerMan->getId() | pShipItems[idTiller] );
	hold->setId( hold->getId() | pShipItems[idHold] );

	TurnShip( pi->more1, dir, plankLeft, plankRight, tillerMan, hold );
	
	// This moves all the items and refreshes them.. but before we should change their 
	// offsets from the center..
	MoveTo(getPosition());

	for ( ClientSList::iterator it = toResume.begin(); it != toResume.end(); it++ )
		(*it)->resume():
}

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
	int8_t tx = 0, ty = 0;
	
	if ( dir == 0xFF ) dir = getDirection();
	
	// This packet pauses the client before move the board
	nPackets::Sent::PauseClient pk(0x01);
	client->sendPacket(&pk);

	sLocation boatpos = getPosition();
	
	switch(dir&0x07)//Which DIR is it going in?
	{
	case 0:
		--boatpos.y;
		break;
	case 1:
		++boatpos.x;
		--boatpos.y;
		break;
	case 2:
		++boatpos.x;
		break;
	case 3:
		++boatpos.x;
		++boatpos.y;
		break;
	case 4:
		++boatpos.y;
		break;
	case 5:
		--boatpos.x;
		++boatpos.y;
		break;
	case 6:
		--boatpos.x;
		break;
	case 7:
		--boatpos.x;
		--boatpos.y;
		break;
	}


	if( ( boatpos.x <= XBORDER || boatpos.x >= ((map_width*8)-XBORDER))
		|| (boatpos.y <= YBORDER || boatpos.y >= ((map_height*8)-YBORDER))) //bugfix LB
	{
		type2=0;
		itemtalk(tillerMan,"Arr, Sir, we've hit rough waters!");
		nPackets::Sent::PauseClient pk(0x00);
		client->sendPacket(&pk);
		return;
	}

	if(!good_position(this, boatpos, 0))
	{
		type2=0;
		itemtalk(tillerMan, "Arr, somethings in the way!");
		nPackets::Sent::PauseClient pk(0x00);
		client->sendPacket(&pk);
		return;
	}
	if(collision(this, boatpos,0))
	{
		type2=0;
		itemtalk(tillerMan, "Arr, another ship in the way");
		nPackets::Sent::PauseClient pk(0x00);
		client->sendPacket(&pk);

		return;
	}

	// The special items are also multi's items, so this moves all
	MoveTo( boatpos );

//!\todo wait until set hav appropriate function
#if 0
	for (a=0;a<imultisp[serial%HASHMAX].max;a++)  // move all item upside the boat
	{
		c=imultisp[serial%HASHMAX].pointer[a];
		if(c!=-1)
		{
			pItem pi= MAKE_ITEMREF_LOGGED(c,err);
			if(!err)
			{
				mapRegions->remove(pi);
				sLocation itmpos= pi->getPosition();
				itmpos.x+= tx;
				itmpos.y+= ty;
				pi->setPosition( itmpos );
				pi->Refresh();
				mapRegions->add(pi);
			}
		}
	}

	for (a=0;a<cmultisp[serial%HASHMAX].max;a++) // move all char upside the boat
	{
		c=cmultisp[serial%HASHMAX].pointer[a];
		if (c!=-1)
		{
		   pc_c=MAKE_CHARREF_LOGGED(c,err);
		   if (!err)
		   {
			   sLocation charpos= pc_c->getPosition();
			   mapRegions->remove(pc_c);
			   charpos.x+= tx;
			   charpos.y+= ty;
			   pc_c->MoveTo(charpos);
			   pc_c->teleport();
			   mapRegions->add(pc_c);
		   }
		}
	}
#endif

	//! Removes the pause in the client
	nPackets::Sent::PauseClient pk(0x00);
	client->sendPacket(&pk);
}

/*!
\brief Turns the boat
\param turnRight If true, the boat will be turned right, else left
*/
void cBoat::turn(bool turnRight)
{
	ClientSList toResume;
	
	nPackets::Sent::PauseClient pk(0x01);
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

		ps_i->sendPacket(&pk);
	}

	if(turnRight)
	{
		setDirection(getDirection()+2);
		id2++;
	} else {
		setDirection(getDirection()-2);
		id2--;
	}
	
	uint8_t id1 = getId() >> 8;
	uint8_t id2 = getId() & 0xFF;
	
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

	//set it's Z to 0,0 inside the boat
	sLocation bpos= getPosition();

	plankLeft->MoveTo( bpos.x, bpos.y, plankLeft->getPosition().z );
	plankLeft->setId( plankLeft->getId() | pShipItems[idPortPlankClosed] );//change the ID

	plankRight->MoveTo( bpos.x, bpos.y, plankRight->getPosition().z );
	plankRight->setId( pplankRight->getId() | pShipItems[idStarPlankClosed] );

	tillerMan->MoveTo( bpos.x, bpos.y, tillerMan->getPosition().z );
	tillerMan->setId( tillerMan->getId() | pShipItems[idTiller] );

	hold->MoveTo(bpos.x, bpos.y, hold->getPosition().z );
	hold->setId( hold->getId() | pShipItems[idHold] );

	TurnShip( pi->more1, dir, plankLeft, plankRight, tillerMan, hold );

	plankLeft->Refresh();
	plankRight->Refresh();
	hold->Refresh();
	tillerMan->Refresh();

	nPackets::Sent::PauseClient pk(0x00);
	for ( ClientSList::iterator it = toResume.begin(); it != toResume.end(); it++ )
		(*it)->sendPacket(&pk);
}

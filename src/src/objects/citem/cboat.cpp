/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "objects/citem/cboat.h"
#include "objects/cclient.h"
#include "misc.h"

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

/*!
\brief Get the boat from one of its planks
\param pl Plank to search the boat of
\return The boat of the given plank or NULL if not found

This function gets the multi of the item, and casts it to a pBoat, clenaing up
different type of multis (like houses and other multis), returing so a perfect
pointer to the pBoat instasnce the plank is.

This anyway should be done directly in the caller functions, so I think this
should be removed in the future.

\deprecated This function is merely a dynamic_cast, so we should no more use it
*/
pBoat cBoat::searchByPlank(pItem pl)
{
	return dynamic_cast<pBoat>( pl->getMulti() );
}

cBoat::cBoat() : cMulti()
{
}

cBoat::cBoat(uint32_t nserial) : cMulti(nserial)
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

	if( ! isGoodPosition(getId(), boatpos, 0) )
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

/*!
\brief Checks for a boat's speech
\param client Client who's performing the speech
\param speech Speech performed
\retval true The \c speech is recognized as a boat's speech and is executed
\retval false The \c speech isn't recognized, or the player can't do speech in
	this boat
\todo Change this to use a regexp capture using the pcreplus library when
	available
*/
bool cBoat::doSpeech(pClient client, const std::string &speech)
{
	assert(client); assert(client->currChar());
	pPC pc = client->currChar();
	
	// if the pc is not the boat owner (or a gm!) ..we don't care what he says
	if( getOwner() != pc && ! pc->isGM() )
		return false;
	
	uint8_t dir = getDirection() & 0x07;
	
	if( speech == "FORWARD" || speech == "UNFURL SAIL" )
	{
		tillerMan->talk("Aye, sir.");
		
		type2=1;//Moving
		step(client);
	} else  if( speech == "BACKWARD" ) {
		tillerMan->talk("Aye, sir.");
		
		type2=2;//Moving backward
		if(dir>=4)
			dir-=4;
		else
			dir+=4;
		
		step(client, dir);
	} else if( speech == "ONE LEFT" || speech == "DRIFT LEFT" ) {
		tillerMan->talk("Aye, sir.");
		
		step(client, (dir-2)%8);
	} else if( speech == "ONE RIGHT" || speech == "DRIFT RIGHT" ) {
		tillerMan->talk("Aye, sir.");
		
		step(client, (dir+2)%8);
	} else if( speech == "STOP" || speech == "FURL SAIL" ) {
		tillerMan->talk("Aye, sir.");
		type2=0;
	} else if( speech == "TURN LEFT" || speech == "TURN PORT" ) {
		if (isGoodPosition(getId(), getPosition(), -1) && !collision(pb,getPosition(),-1))
		{
			tillerMan->talk("Aye, sir.");
			turn(false);
		} else {
			type2=0;
			tillerMan->talk("Arr, somethings in the way");
		}
	} else if( speech == "TURN RIGHT" || speech == "TURN STARBOARD" )
	{
		if (isGoodPosition(getId(), getPosition(), 1) && !collision(pb,getPosition(),1))
		{
			tillerMan->talk("Aye, sir.");
			turn(true);
		} else {
			type2=0;
			tillerMan->talk("Arr, somethings in the way");
		}
	}else if( speech == "COME ABOUT" || speech == "TURN ABOUT" ) {

		if (isGoodPosition(getId(), getPosition(), 2) && !collision(pb,getPosition(),2))
		{
			tillerMan->talk("Aye, sir.");
			turn(true);
			turn(true);
		} else {
			type2=0;
			tillerMan->talk("Arr, somethings in the way");
		}
	} else if( talk.substr(0, 9) == "SET NAME " ) {
		tillerMan->setCurrentName( talk.substr( 9 ) );
	} else
		return false;
	
	return true;
}

/*!
\brief Check if this is a good position for building or moving a boat
\param id ID of the boat to check the collision of
\param w Point where the base of the boat is
\param dir Direction of boat (from -1 to 2, this is strange!)
\todo Fix the dir stuff
*/
bool cBoat::isGoodPosition(uint16_t id, sPoint w, int8_t dir)
{
	bool good_pos = false;

	multiVector m;
	data::seekMulti( id-0x4000, m );

	for(register int i = 0; i < m.size(); i++ )
	{

		map_st map;
		switch(dir)
		{
		case -1:
			data::seekMap(w.x-m[i].y, w.y+m[i].x, map);
			break;
		case 0:
			data::seekMap(w.x+m[i].x, w.y+m[i].y, map);
			break;
		case 1:
			data::seekMap(w.x+m[i].y, w.y-m[i].x, map);
			break;
		case 2:
			data::seekMap(w.x-m[i].x, w.y-m[i].y, map);
			break;
		}

		switch(map.id)
		{
//water tiles:
		case 0x00A8://168
		case 0x00A9://169
		case 0x00AA://170
		case 0x00Ab://171
		case 0x0136://310
		case 0x0137://311
		case 0x3FF0://16368
		case 0x3FF1://16369
		case 0x3FF2://16370
		case 0x3FF3://16371
//Lava tiles:
		case 0x01F4://500
		case 0x01F5://501
		case 0x01F6://502
		case 0x01F7://503
			good_pos=true;
			break;
		default:// we are in default if we are nearer coast
			good_pos = tileCheck( m[i], map, w, dir );
			if (!good_pos) return false;
		}
	}
	return good_pos;
}

/*!
\brief Check if all the boats tile are in water
\param multi Multi data of the boat to check
\param map Tile to check
\param w Base point for the check
\param dir Direction of boat (from -1 to 2, this is strange!)
\todo Fix the dir stuff
*/
bool cBoat::tileCheck(multi_st multi, map_st map, sPoint w, int8_t dir)
{
	int16_t dx,dy;
	switch(dir)
	{
	case -1:
		dx=w.x-multi.y;
		dy=w.y+multi.x;
		break;
	case 0:
		dx=w.x+multi.x;
		dy=w.y+multi.y;
		break;
	case 1:
		dx=w.x+multi.y;
		dy=w.y-multi.x;
		break;

	case 2:
		dx=w.x-multi.y;
		dy=w.y-multi.x;
		break;
	}

	staticVector s;
	data::collectStatics( dx, dy, s );
	for( register int i = 0; i < s.size(); i++ )
		if ( isWaterTile( sPoint(dx, dy) ) )
			return true;

	return false;
}

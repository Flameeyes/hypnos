/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "boats.h"
#include "sndpkg.h"
#include "map.h"
#include "inlines.h"
#include "objects/citem.h"
#include "objects/cchar.h"
#include "objects/cbody.h"
#include "objects/cobject.h"

#define X 0
#define Y 1

//============================================================================================
//UooS Item translations - You guys are the men! :o)

//[4]=direction of ship
//[4]=Which Item (PT Plank, SB Plank, Hatch, TMan)
//[2]=Coord (x,y) offsets
static const int16_t iSmallShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ { {-2, 0}, {2, 0}, {0, -4}, {1, 4} },//Dir
  { {0, -2}, {0, 2}, {4, 0}, {-4, 0} },
  { {2, 0}, {-2, 0}, {0, 4}, {0, -4}  },
  { {0, 2},  {0,-2}, {-4, 0}, {4, 0}  }
};
//  P1    P2   Hold  Tiller
static const int16_t iMediumShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ { {-2, 0}, {2, 0}, {0,-4}, {1, 5} },
  { {0,-2}, {0, 2}, {4, 0},{-5, 0} },
  { {2, 0}, {-2, 0}, {0, 4}, {0,-5} },
  { {0, 2}, {0,-2}, {-4, 0}, {5, 0} }
};
static const int16_t iLargeShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ { {-2,-1}, {2,-1}, {0,-5}, {1, 5} },
  { {1,-2}, {1, 2}, {5, 0},{-5, 0} },
  { {2, 1}, {-2, 1}, {0, 5}, {0,-5} },
  { {-1, 2}, {-1,-2}, {-5, 0}, {5, 0} }
};
//============================================================================================

/*!
\brief A sort of getboart() only more general
\todo delete or write it
*/
pItem findmulti(sLocation where)
{
/*	int lastdist=30;
	pItem pmulti=NULL;
	
	NxwItemWrapper si;
	si.fillItemsNearXYZ( where );
	for( ; !si.isEmpty(); si++ ) {
		pItem pi=si.getItem();
		if(!pi)
			continue;

		if (pi->id1>=0x40)
		{
			sLocation itmpos= pi->getPosition();

			int dx=abs((int)where.x - (int)itmpos.x);
			int dy=abs((int)where.y - (int)itmpos.y);
			int ret=(int)(hypot(dx, dy));

			if (ret<=lastdist)
			{
				lastdist=ret;
				if (inmulti(where,pi))
					pmulti=pi;
			}
		}
	}

	return pmulti;*/
	return NULL;
}

bool inmulti(sLocation where, pItem pi)//see if they are in the multi at these chords (Z is NOT checked right now)
{
	if ( ! pi )
		return false;

	multiVector m;

	data::seekMulti( pi->getId()-0x4000, m );
	sLocation itmpos= pi->getPosition();
	for( uint32_t i = 0; i < m.size(); i++ ) {
		if(/*(multi.visible)&&*/((itmpos.x+m[i].x) == where.x) && ((itmpos.y+m[i].y) == where.y))
		{
			return true;
		}
	}
	return false;
}

void PlankStuff(pChar pc , pItem pi)//If the plank is opened, double click Will send them here
{
	if ( ! pc )
		return;

	pItem boat =GetBoat(pc->getBody()->getPosition());
	if ( ! boat ) //we are on boat
	{
		boat->type2 = 0; //STOP the BOAT
		LeaveBoat(pc,pi);//get of form boat
		return;
	}

	pBoat boat2 = cBoat::searchByPlank(pi);

	if ( ! boat2 )
	{
		WarnOut("Can't find boats!\n");
		return;
	}

	boat2->type2 = 0; //STOP the BOAT

	//! \todo Change this to the new owner system when done
	NxwCharWrapper sc;
	sc.fillOwnedNpcs( pc, false, true );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) 
	{
		pChar pc_b=sc.getChar();

		if( ! pc_b ) continue;
		
		sLocation boatpos= boat2->getPosition();
		pc_b->MoveTo( boatpos.x+1, boatpos.y+1, boatpos.z+2 );
		pc_b->setMulti( boat2 );
		pc_b->teleport();
	}


	OpenPlank(pi); //lb

	sLocation boatpos= boat2->getPosition();
	pc->MoveTo( boatpos.x+1, boatpos.y+1, boatpos.z+3 );
	pc->teleport();
	pc->sysmsg("you entered a boat");
}

void LeaveBoat(pChar pc, pItem pi)//Get off a boat (dbl clicked an open plank while on the boat.
{
	if ( ! pc )
		return;

	//long int pos, pos2, length;
	uint16_t x,x2= pi->getPosition().x;
	uint16_t y,y2= pi->getPosition().y;
	int8_t z= pi->getPosition().z;
	int8_t mz,sz,typ;
	pItem pb=GetBoat(pc->getBody()->getPosition());


	if (pb==NULL) return;
	
	for(x=x2-6;x<=x2+6;x++)
	{
		for(y=y2-6;y<=y2+66;y++)
		{
			sz=(signed char) staticTop(sLocation(x,y,z)); // MapElevation() doesnt work cauz we are in a multi !!

			mz=(signed char) mapElevation(x,y);
			if (sz==illegal_z) typ=0;
			else typ=1;
			//o=Map->o_Type(x,y,z);

			if( ! ( (typ==0 && mz!=-5) || (typ==1 && sz!=-5) ) )
				continue;
			
			//! \todo Change this to the new owner system when done
			NxwCharWrapper sc;
			sc.fillOwnedNpcs( pc, false, true );
			for( sc.rewind(); !sc.isEmpty(); sc++ )
			{

				pChar pc_b=sc.getChar();
				if ( ! pc_b ) return;
				
				pc_b->getBody()->MoveTo( x,y, typ ? sz : mz );
				pc_b->setMulti(NULL);
				pc_b->teleport();
			}

			pc->setMulti(NULL);
			pc->getBody()->setPosition( sLocation( x, y, typ ? sz : mz, typ ? sz : mz ) );
			pointers::updateLocationMap(pc);
			pc->sysmsg("You left the boat.");
			pc->teleport();//Show them they moved.
			return;
		}//for y
	}//for x
	pc->sysmsg("You cannot get off here!");

}


void TurnStuff_i(pItem p_b, pItem pi, int dir, int type)//Turn an item that was on the boat when the boat was turned.
{
	if ( ! p_b || ! pi )
		return;

	int dx, dy;

	sLocation bpos	= p_b->getPosition();
	sLocation itmpos(bpos.x, bpos.y, pi->getPosition().z, pi->getPosition().dispz);

	dx= pi->getPosition().x - bpos.x;//get their distance x
	dy= pi->getPosition().y - bpos.y;//and distance Y

	if(dir)//turning right
	{
		itmpos.x +=dy*-1;
		itmpos.y +=dx;
	}
	else //turning left
	{
		itmpos.x+=dy;
		itmpos.y+=dx*-1;
	}
	pi->setPosition( itmpos );
	pointers::updateLocationMap(pi);
	pi->Refresh();
}


void TurnStuff_c(pItem p_b, pChar pc, int dir, int type)//Turn an item that was on the boat when the boat was turned.
{
	if ( ! p_b || ! pc )
		return;
	
	int dx, dy;
	sLocation bpos= p_b->getPosition();
	sLocation charpos= pc->getPosition();
	dx= charpos.x - bpos.x;
	dy= charpos.y - bpos.y;
	charpos.x= bpos.x;
	charpos.y= bpos.y;

	if(dir)
	{
		charpos.x+= dy*-1;
		charpos.y+= dx;

	} else {
		charpos.x+= dy;
		charpos.y+= dx*-1;

	}
	pc->setPosition( charpos );
	pointers::updateLocationMap(pc);
	pc->teleport();
}

void TurnShip( uint8_t size, int32_t dir, pItem pPort, pItem pStarboard, pItem pTiller, pItem pHold )
{
	sLocation itmpos;
	int16_t *pShipOffsets;

	switch( size )
	{
	case 0x00:
	case 0x04:
		pShipOffsets = iSmallShipOffsets[dir][PORT_PLANK];
		itmpos= pPort->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pPort->setPosition( itmpos );
		pointers::updateLocationMap( pPort );

		pShipOffsets = iSmallShipOffsets[dir][STARB_PLANK];
		itmpos= pStarboard->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pStarboard->setPosition( itmpos );
		pointers::updateLocationMap( pStarboard );

		pShipOffsets = iSmallShipOffsets[dir][TILLER];
		itmpos= pTiller->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pTiller->setPosition( itmpos );
		pointers::updateLocationMap( pTiller );

		pShipOffsets = iSmallShipOffsets[dir][HOLD];
		itmpos= pHold->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pHold->setPosition( itmpos );
		pointers::updateLocationMap( pHold );

		break;

	case 0x08:
	case 0x0C:
		pShipOffsets = iMediumShipOffsets[dir][PORT_PLANK];
		itmpos= pPort->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pPort->setPosition( itmpos );
		pointers::updateLocationMap( pPort );

		pShipOffsets = iMediumShipOffsets[dir][STARB_PLANK];
		itmpos= pStarboard->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pStarboard->setPosition( itmpos );
		pointers::updateLocationMap( pStarboard );

		pShipOffsets = iMediumShipOffsets[dir][TILLER];
		itmpos= pTiller->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pTiller->setPosition( itmpos );
		pointers::updateLocationMap( pTiller );

		pShipOffsets = iMediumShipOffsets[dir][HOLD];
		itmpos= pHold->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pHold->setPosition( itmpos );
		pointers::updateLocationMap( pHold );

		break;
	case 0x10:
	case 0x14:

		pShipOffsets = iLargeShipOffsets[dir][PORT_PLANK];
		itmpos= pPort->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pPort->setPosition( itmpos );
		pointers::updateLocationMap( pPort );

		pShipOffsets = iLargeShipOffsets[dir][STARB_PLANK];
		itmpos= pStarboard->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pStarboard->setPosition( itmpos );
		pointers::updateLocationMap( pStarboard );

		pShipOffsets = iLargeShipOffsets[dir][TILLER];
		itmpos= pTiller->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pTiller->setPosition( itmpos );
		pointers::updateLocationMap( pTiller );

		pShipOffsets = iLargeShipOffsets[dir][HOLD];
		itmpos= pHold->getPosition();
		itmpos.x+= pShipOffsets[X];
		itmpos.y+= pShipOffsets[Y];
		pHold->setPosition( itmpos );
		pointers::updateLocationMap( pHold );

		break;

	default:
		LogWarning("Turnboatstuff() more1 error! more1 = %c not found!\n", size);
		break;
	}
}


/*!
\brief See if they said a command.
\return true if the talk is a valid command and is accepted, else false
\note pc and client validation is done in talking()
\todo Should it be checked into talking directly?
*/
bool Speech(pClient client, std::string &talk)
{
	pPC pc = client->currChar();
	pBoat pb = GetBoat(pc->getPosition());
	if( ! pb )
		return false;
	
	// if the pc is not the boat owner (or a gm!) ..we don't care what he says
	if( pb->getOwner() != pc && ! pc->isGM() )
		return false;

	pItem tiller = boat->getTiller();
	if ( ! tiller )
		return false;

	uint8_t dir = pb->getDirection() & 0x07;
	
	if( talk == "FORWARD" || talk == "UNFURL SAIL" )
	{
		itemtalk(tiller, "Aye, sir.");
		
		pb->type2=1;//Moving
		pb->step(client);
	} else  if( talk == "BACKWARD" ) {
		itemtalk(tiller, "Aye, sir.");
		
		pb->type2=2;//Moving backward
		if(dir>=4)
			dir-=4;
		else
			dir+=4;
		
		pb->step(client, dir);
	} else if( talk == "ONE LEFT" || talk == "DRIFT LEFT" )
	{
		itemtalk(tiller,"Aye, sir.");
		
		pb->step(client, (dir-2)%8);
	} else if( talk == "ONE RIGHT" || talk == "DRIFT RIGHT" )
	{
		itemtalk(tiller,"Aye, sir.");
		
		pb->step(client, (dir+2)%8);
	} else if( talk == "STOP" || talk == "FURL SAIL" )
	{
		itemtalk(tiller,"Aye, sir.");
		pb->type2=0;
	} else if( talk == "TURN LEFT" || talk == "TURN PORT" )
	{
		if (good_position(pb, pb->getPosition(), -1) && !collision(pb,pb->getPosition(),-1))
		{
			itemtalk(tiller, "Aye, sir.");
			pb->turn(false);
		} else {
			pb->type2=0;
			itemtalk(tiller, "Arr, somethings in the way");
		}
	} else if( talk == "TURN RIGHT" || talk == "TURN STARBOARD" )
	{
		if (good_position(pb, pb->getPosition(), 1) && !collision(pb,pb->getPosition(),1))
		{
			itemtalk(tiller, "Aye, sir.");
			pb->turn(true);
		} else {
			pb->type2=0;
			itemtalk(tiller, "Arr, somethings in the way");
		}
	}else if( talk == "COME ABOUT" || talk == "TURN ABOUT" )
	{

		if (good_position(pb, pb->getPosition(), 2) && !collision(pb,pb->getPosition(),2))
		{
			itemtalk(tiller, "Aye, sir.");
			pb->turn(true);
			pb->turn(true);
		} else {
			pb->type2=0;
			itemtalk(tiller, "Arr, somethings in the way");
		}
	} else if( talk.substr(0, 9) == "SET NAME " ) {
		tiller->setCurrentName( talk.substr( 9 ) );
	} else
		return false;
	
	return true;
}

/*!
\author Elcabesa
\brief Check if all the boats tile are in water
*/
bool tile_check(multi_st multi,pItem pb,map_st map,int x, int y,int dir)
{
	int dx,dy;
	switch(dir)
		{
		case -1:
			dx=x-multi.y;
			dy=y+multi.x;
			break;
		case 0:
			dx=x+multi.x;
			dy=y+multi.y;
			break;
		case 1:
			dx=x+multi.y;
			dy=y-multi.x;
			break;

		case 2:

			dx=x-multi.y;

			dy=y-multi.x;

			break;
		}

	staticVector s;
	data::collectStatics( dx, dy, s );
	for( uint32_t i = 0; i < s.size(); i++ )
	{
		tile_st tile;
		if( data::seekTile( s[i].id, tile ) )
		{
			if(!(strstr((char *) tile.name, "water") || strstr((char *) tile.name, "lava")))
			{
				land_st land;
				if( data::seekLand( map.id, land ) )
					return !(land.flags&TILEFLAG_WET);	//not a "wet" tile
			}
			else
				return true;
		}

	}
	return false;
}

/*!
\author Elcabesa
\brief Check if this is a good position for building or moving a boat
*/
bool good_position(pItem pb, sLocation where, int dir)
{
	uint32_t x= where.x, y= where.y, i;
	bool good_pos = false;

	multiVector m;
	data::seekMulti( pb->getId()-0x4000, m );

	for( i = 0; i < m.size(); i++ )
	{

		map_st map;
		switch(dir)
		{
		case -1:
			data::seekMap(x-m[i].y,y+m[i].x, map);
			break;
		case 0:
			data::seekMap(x+m[i].x,y+m[i].y, map);
			break;
		case 1:
			data::seekMap(x+m[i].y,y-m[i].x, map);
			break;
		case 2:
			data::seekMap(x-m[i].x,y-m[i].y, map);
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
			good_pos=tile_check( m[i],pb,map,x,y,dir );
			if (!good_pos) return false;
		}
	}
	return good_pos;
}

/*!
\author Elcabesa
\brief Build a boat
*/
bool Build(pClient client, pItem pb, char id2)
{
	pPC pc_cs = NULL
	if ( ! client || ! ( pc_cs = client->currChar() ) )
		return false;

	int nid2=id2;

	if( !pb )
	{
		client->sysmessage("There was an error creating that boat.");
		return false;
	}

	if(id2!=0x00 && id2!=0x04 && id2!=0x08 && id2!=0x0C && id2!=0x10 && id2!=0x14)//Valid boat ids (must start pointing north!)
	{
		client->sysmessage("The deed is broken, please contact a Game Master.");
		return false;
	}
	
	//Start checking for a valid position:
	if (! good_position(pb, pb->getPosition(), 0))
		return false;
	
	if(collision(pb, pb->getPosition(),0))
		return false;
	// Okay we found a good  place....

	pb->setOwner(pc_cs);
	pb->more1.moreb1 = id2;//Set min ID
	pb->more1.moreb2 = nid2+3;//set MAX id
	pb->type=ITYPE_BOATS;//Boat type
	pb->setPositionZ(-5);//Z in water
//	strcpy(pb->name,"a mast");//Name is something other than "%s's house"
	pb->setCurrentName("a mast");

	pItem pTiller=item::CreateFromScript( "$item_tillerman" );
	if( !pTiller ) return false;
	pTiller->setPositionZ(-5);
	pTiller->priv=0;

	pItem pPlankR=item::CreateFromScript( "$item_plank2" );//Plank2 is on the RIGHT side of the boat
	if( !pPlankR ) return false;
	pPlankR->type=ITYPE_BOATS;
	pPlankR->type2=3;
	pPlankR->more1.more = pb->getSerial();	//Lock this item!
	pPlankR->setPositionZ(-5);
	pPlankR->priv=0;//Nodecay

	pItem pPlankL=item::CreateFromScript( "$item_plank1" );//Plank1 is on the LEFT side of the boat
	if( !pPlankL ) return false;
	pPlankL->type=ITYPE_BOATS;//Boat type
	pPlankL->type2=3;//Plank sub type
	pPlankL->more.more = pb->getSerial();
	pPlankL->setPositionZ(-5);
	pPlankL->priv=0;

	pItem pHold=item::CreateFromScript( "$item_hold1" );
	if( !pHold ) return false;
	pHold->more.more = pb->getSerial();	//Lock this too :-)

	pHold->type=ITYPE_CONTAINER;//Container
	pHold->setPositionZ(-5);
	pHold->priv=0;
	pHold->setContainer(0);

	//!\todo Now morex morey and morez aren't uint32_t, and boats must be different class objects
	pb->more2.more = pTiller->getSerial();	//Tiller ser stored in boat's Moreb
	pb->morex= pPlankL->getSerial();	//Store the other stuff anywhere it will fit :-)
	pb->morey= pPlankR->getSerial();
	pb->morez= pHold->getSerial();

	sLocation boatpos= pb->getPosition();

	switch(id2)//Give everything the right Z for it size boat
	{
	case 0x00:
	case 0x04:
		pTiller->setPosition( sLocation( boatpos.x + 1, boatpos.y + 4, pTiller->getPosition().z ) );
		pPlankR->setPosition( sLocation( boatpos.x + 2, boatpos.y, pPlankR->getPosition().z ) );
		pPlankL->setPosition( sLocation( boatpos.x - 2, boatpos.y, pPlankL->getPosition().z ) );
		pHold->setPosition( sLocation( boatpos.x, boatpos.y - 4, pHold->getPosition().z ) );
		break;
	case 0x08:
	case 0x0C:
		pTiller->setPosition( sLocation( boatpos.x + 1, boatpos.y + 5, pTiller->getPosition().z ) );
		pPlankR->setPosition( sLocation( boatpos.x + 2, boatpos.y, pPlankR->getPosition().z ) );
		pPlankL->setPosition( sLocation( boatpos.x - 2, boatpos.y, pPlankL->getPosition().z ) );
		pHold->setPosition( sLocation( boatpos.x, boatpos.y - 4, pHold->getPosition().z ) );
		break;
	case 0x10:
	case 0x14:
		pTiller->setPosition( sLocation( boatpos.x + 1, boatpos.y + 5, pTiller->getPosition().z ) );
		pPlankR->setPosition( sLocation( boatpos.x + 2, boatpos.y -1, pPlankR->getPosition().z ) );
		pPlankL->setPosition( sLocation( boatpos.x - 2, boatpos.y -1, pPlankL->getPosition().z ) );
		pHold->setPosition( sLocation( boatpos.x, boatpos.y - 5, pHold->getPosition().z ) );
		break;
	}

	pointers::addToLocationMap( pTiller );
	pointers::addToLocationMap( pPlankL );
	pointers::addToLocationMap( pPlankR );
	pointers::addToLocationMap( pHold );
	pointers::addToLocationMap( pb );
	//their x pos is set by BuildHouse(), so just fix their Z...
	boatpos.z+=3;
	boatpos.dispz=boatpos.z;

	pc_cs->MoveTo(boatpos);
	pc_cs->setMulti(pb);
	return true;
}

/*!
\author Elcabesa
\brief Check if there is another boat at these coord
\return true if collided, else false
*/
bool collision(pItem pi,sLocation where,int dir)
{
	int x= where.x, y= where.y;
	std::map<int,boat_db>::iterator iter_boat;
	for(iter_boat=s_boat.begin();iter_boat!=s_boat.end();iter_boat++)
	{
		boat_db coll=iter_boat->second;
		if ( coll.serial == pi->getSerial() )
			continue;
		
		int xx=abs(x - coll.p_serial->getPosition().x);
		int yy=abs(y - coll.p_serial->getPosition().y);
		double dist=hypot(xx, yy);
		if ( dist >= 10 )
			continue;
			
		if(boat_collision(pi,x,y,dir,coll.p_serial))
			return true;
	}
	return false;
}

/*!
\brief check if 2 boat are collided
\author Elcabesa
\return true if collided, else false
\see collision()
*/
bool boat_collision(pItem pb1,int x1, int y1,int dir,pItem pb2)
{
	uint32_t i1, i2;
	int x,y;

	multiVector m1, m2;
	data::seekMulti( pb1->getId()-0x4000, m1 );
	data::seekMulti( pb2->getId()-0x4000, m2 );

	for( i1 = 0; i1 < m1.size(); i1++ )
	{
		for( i2 = 0; i2 < m2.size(); i2++ )
		{
			/*multi_st multi1 = sm1.get();
			multi_st multi2 = sm2.get();*/

			switch(dir)
			{
			case -1:
				x=x1-m1[i1].y;
				y=y1+m1[i1].x;
				break;

			case 0:
				x=x1+m1[i1].x;
				y=y1+m1[i1].y;
				break;

			case 1:
				x=x1+m1[i1].y;
				y=y1-m1[i1].x;
				break;

			case 2:
				x=x1-m1[i1].x;
				y=y1-m1[i1].y;
				break;

			default:
				LogError("boat_collision() - bad boat turning direction\n");
			}

			if ( (x==m2[i2].x+pb2->getPosition().x) && (y==m2[i2].y+pb2->getPosition().y) )
			{
				return true;
			}
		}
	}
	return false;
}

/*!
\brief Open, or close the plank
*/
void OpenPlank(pItem pi)
{
	switch(pi->getId()&0xFF)
	{
		//Open plank->
		case 0xE9: pi->setId( 0xE984 ); break;
		case 0xB1: pi->setId( 0xB1D5 ); break;
		case 0xB2: pi->setId( 0xB2D4 ); break;
		case 0x8A: pi->setId( 0x8A89 ); break;
		case 0x85: pi->setId( 0x8584 ); break;
		//Close Plank->
		case 0x84: pi->setId( 0x84E9 ); break;
		case 0xD5: pi->setId( 0xD5B1 ); break;
		case 0xD4: pi->setId( 0xD4B2 ); break;
		case 0x89: pi->setId( 0x898A ); break;
		default: LogWarning("WARNING: Invalid plank ID called! Plank %i '%s' [ %04x ]\n", pi->getSerial(), pi->getCurrentName().c_str(), pi->getId()); break;
	}
}

/*!
\brief check if there is a boat at this position and return the boat
\return the pointer to the boat or NULL
\author Elcabesa
*/
pItem GetBoat(sLocation pos)
{
	uint32_t i;
	BOATS::iterator iter( s_boat.begin() ), end( s_boat.end() );
	for( ; iter!=end; iter++) {

		boat_db boat=iter->second;
		pItem pb=boat.p_serial;
		if( ! pb )
			continue;
		
		if( dist( pos, pb->getPosition() ) >= 10.0 )
			continue;
		multiVector m;
		data::seekMulti( pb->getId()-0x4000, m );

		for( i = 0; i < m.size(); i++ )
		{
			if( ((m[i].x + pb->getPosition().x) == pos.x) && ((m[i].y + pb->getPosition().y) == pos.y) )
				return  pb;
		}
	}
	return NULL;
}

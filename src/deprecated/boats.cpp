  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


#include "common_libs.h"
#include "boats.h"
#include "sndpkg.h"
#include "map.h"
#include "classes.h"
#include "inlines.h"
#include "objects/citem.h"
#include "objects/cchar.h"
#include "objects/cbody.h"

BOATS	s_boat;

cBoat* Boats=NULL;

#define X 0
#define Y 1

//============================================================================================
//UooS Item translations - You guys are the men! :o)

//[4]=direction of ship
//[4]=Which Item (PT Plank, SB Plank, Hatch, TMan)
//[2]=Coord (x,y) offsets
signed short int iSmallShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ { {-2, 0}, {2, 0}, {0, -4}, {1, 4} },//Dir
  { {0, -2}, {0, 2}, {4, 0}, {-4, 0} },
  { {2, 0}, {-2, 0}, {0, 4}, {0, -4}  },
  { {0, 2},  {0,-2}, {-4, 0}, {4, 0}  }
};
//  P1    P2   Hold  Tiller
signed short int iMediumShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ { {-2, 0}, {2, 0}, {0,-4}, {1, 5} },
  { {0,-2}, {0, 2}, {4, 0},{-5, 0} },
  { {2, 0}, {-2, 0}, {0, 4}, {0,-5} },
  { {0, 2}, {0,-2}, {-4, 0}, {5, 0} }
};
signed short int iLargeShipOffsets[4][4][2]=
// X  Y  X  Y  X  Y  X  Y
{ { {-2,-1}, {2,-1}, {0,-5}, {1, 5} },
  { {1,-2}, {1, 2}, {5, 0},{-5, 0} },
  { {2, 1}, {-2, 1}, {0, 5}, {0,-5} },
  { {-1, 2}, {-1,-2}, {-5, 0}, {5, 0} }
};
//Ship Items
//[4] = direction
//[6] = Which Item (PT Plank Up,PT Plank Down, SB Plank Up, SB Plank Down, Hatch, TMan)
char cShipItems[4][6]=
{
 {(unsigned char)0xB1,(unsigned char)0xD5,(unsigned char)0xB2,(unsigned char)0xD4,(unsigned char)0xAE,(unsigned char)0x4E},
 {(unsigned char)0x8A,(unsigned char)0x89,(unsigned char)0x85,(unsigned char)0x84,(unsigned char)0x65,(unsigned char)0x53},
 {(unsigned char)0xB2,(unsigned char)0xD4,(unsigned char)0xB1,(unsigned char)0xD5,(unsigned char)0xB9,(unsigned char)0x4B},
 {(unsigned char)0x85,(unsigned char)0x84,(unsigned char)0x8A,(unsigned char)0x89,(unsigned char)0x93,(unsigned char)0x50}
};
//============================================================================================

/*!
\brief A sort of getboart() only more general
\todo delete or write it
*/
pItem findmulti(Location where)
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
			Location itmpos= pi->getPosition();

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

bool inmulti(Location where, pItem pi)//see if they are in the multi at these chords (Z is NOT checked right now)
// PARAM WARNING: z is unreferenced
{
	if ( ! pi )
		return false;

	multiVector m;

	data::seekMulti( pi->getId()-0x4000, m );
	Location itmpos= pi->getPosition();
	for( uint32_t i = 0; i < m.size(); i++ ) {
		if(/*(multi.visible)&&*/((itmpos.x+m[i].x) == where.x) && ((itmpos.y+m[i].y) == where.y))
		{
			return true;
		}
	}
	return false;
}

void cBoat::PlankStuff(pChar pc , pItem pi)//If the plank is opened, double click Will send them here
{
	if ( ! pc )
		return;

	pItem boat =GetBoat(pc->getBody()->getPosition());
	if (boat!=NULL) //we are on boat
	{
		boat->type2 = 0; //STOP the BOAT
		LeaveBoat(pc,pi);//get of form boat
		return;
	}

	pItem boat2;

	boat2=search_boat_by_plank(pi);
	if (boat2 == NULL)
	{
		WarnOut("Can't find boats!\n");
		return;
	}

	boat2->type2 = 0; //STOP the BOAT

	NxwCharWrapper sc;
	sc.fillOwnedNpcs( pc, false, true );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) 
	{
		pChar pc_b=sc.getChar();

		if( pc_b )
		{
			Location boatpos= boat2->getPosition();
			pc_b->MoveTo( boatpos.x+1, boatpos.y+1, boatpos.z+2 );
			pc_b->setMultiSerial( boat2->getSerial() );
			pc_b->teleport();

		}
	}


	OpenPlank(pi); //lb

	Location boatpos= boat2->getPosition();
	pc->MoveTo( boatpos.x+1, boatpos.y+1, boatpos.z+3 );
	pc->teleport();
	pc->sysmsg("you entered a boat");
	// pc->setMultiSerial( boat2->getSerial() ); it's has just been called by pc->teleport, so wee need it not
}

void cBoat::LeaveBoat(pChar pc, pItem pi)//Get off a boat (dbl clicked an open plank while on the boat.
{
	if ( ! pc )
		return;

	//long int pos, pos2, length;
	uint16_t x,x2= pi->getPosition().x;
	uint16_t y,y2= pi->getPosition().y;
	int8_t z= pi->getPosition().z;
	int8_t mz,sz,typ;
	pItem pBoat=GetBoat(pc->getPosition());


	if (pBoat==NULL) return;
#define XX 6
#define YY 6
	for(x=x2-XX;x<=x2+XX;x++)
	{
		for(y=y2-YY;y<=y2+YY;y++)
		{
			sz=(signed char) staticTop(Location(x,y,z)); // MapElevation() doesnt work cauz we are in a multi !!

			mz=(signed char) mapElevation(x,y);
			if (sz==illegal_z) typ=0;
			else typ=1;
			//o=Map->o_Type(x,y,z);

			if( ! ( (typ==0 && mz!=-5) || (typ==1 && sz!=-5) ) )
				continue;
			
			NxwCharWrapper sc;
			sc.fillOwnedNpcs( pc, false, true );
			for( sc.rewind(); !sc.isEmpty(); sc++ )
			{

				pChar pc_b=sc.getChar();
				if ( ! pc_b ) return;
				
				pc_b->MoveTo( x,y, typ ? sz : mz );
				pc_b->setMultiSerial(INVALID);
				pc_b->teleport();
			}

			pc->setMultiSerial(INVALID);
#ifdef SPAR_C_LOCATION_MAP
			pc->setPosition( Location( x, y, typ ? sz : mz, typ ? sz : mz ) );
			pointers::updateLocationMap(pc);
#else
			mapRegions->remove(pc);
			pc->setPosition( Location( x, y, typ ? sz : mz, typ ? sz : mz ) );
			mapRegions->add(pc);
#endif
			pc->sysmsg("You left the boat.");
			pc->teleport();//Show them they moved.
			return;
		}//for y
	}//for x
	pc->sysmsg("You cannot get off here!");

}


void cBoat::TurnStuff_i(pItem p_b, pItem pi, int dir, int type)//Turn an item that was on the boat when the boat was turned.
{
	if ( ! p_b || ! pi )
		return;

	int dx, dy;

	Location bpos	= p_b->getPosition();
	Location itmpos = { bpos.x, bpos.y, pi->getPosition().z, pi->getPosition().dispz };

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
#ifdef SPAR_I_LOCATION_MAP
	pi->setPosition( itmpos );
	pointers::updateLocationMap(pi);
#else
	mapRegions->remove(pi);
	pi->setPosition( itmpos );
	mapRegions->add(pi);
#endif
	pi->Refresh();
}


void cBoat::TurnStuff_c(pItem p_b, pChar pc, int dir, int type)//Turn an item that was on the boat when the boat was turned.
{
	if ( ! p_b || ! pc )
		return;
	
	int dx, dy;
	Location bpos= p_b->getPosition();
	Location charpos= pc->getPosition();
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
#ifdef SPAR_C_LOCATION_MAP
	pc->setPosition( charpos );
	pointers::updateLocationMap(pc);
#else
	mapRegions->remove(pc);
	pc->setPosition( charpos );
	mapRegions->add(pc);
#endif
	pc->teleport();
}

void cBoat::Turn(pItem pi, int turn)//Turn the boat item, and send all the people/items on the boat to turnboatstuff()
{
	if ( ! pi )
		return;

	NXWSOCKET 	Send[MAXCLIENT];
	int32_t	id1,
			id2,
			serial,
			itiller,
			i1,
			i2,
			ihold,
			dir,
			d=0;
	pItem	tiller,
			p1,
			p2,
			hold;

	WORD2Duint8_t( pi->getId(), id1, id2 );

	//Of course we need the boat items!
	serial=calcserial(pi->moreb1,pi->moreb2,pi->moreb3,pi->moreb4);
	if(serial<0)
		return;
//!\todo Find a better way to handle this
#if 0
	itiller = calcItemFromSer( serial | 0x40000000 );
	tiller = MAKE_ITEM_REF( itiller );
	if ( ! tiller )
		return;

	i1 = calcItemFromSer( pi->morex | 0x40000000);
	p1 = MAKE_ITEM_REF( i1 );
	if ( ! tiller )
		return;

	i2 = calcItemFromSer( pi->morey | 0x40000000);
	p2 = MAKE_ITEM_REF( i2 );
	if ( ! tiller )
		return;

	ihold = calcItemFromSer( pi->morez | 0x40000000);
	hold = MAKE_ITEM_REF( ihold );
	if ( ! hold )
		return;
#endif

	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {

		NXWCLIENT ps_i=sw.getClient();
		
		if ( ! ps_i )
			continue;
		
		if( pi->distFrom(ps_i->currChar()) > BUILDRANGE)
			continue;
			
		Send[d]=ps_i->toInt();

		//////////////FOR ELCABESA VERY WARNING BY ENDYMION
		//////THIS PACKET PAUSE THE CLIENT
		SendPauseResumePkt(ps_i->toInt(), 0x01);
		d++;
	}

	if(turn)//Right
	{
		pi->dir+=2;
		id2++;
	}
	else
	{//Left
		pi->dir-=2;
		id2--;
	}
	if(pi->dir>7)
		pi->dir-=8;//Make sure we dont have any DIR errors
	if(pi->dir<0)
		pi->dir+=8;
	if(id2<pi->more1)
		id2+=4;//make sure we don't have any id errors either
	if(id2>pi->more2)
		id2-=4;//Now you know what the min/max id is for :-)

	pi->setId( Duint8_t2WORD( id1, id2 ) );//set the id

	if(id2==pi->more1)
		pi->dir=0;//extra DIR error checking
	if(id2==pi->more2)
		pi->dir=6;

	//Set the DIR for use in the Offsets/IDs array
	dir=(pi->dir&0x0F)/2;

	char *pShipItems = cShipItems[ dir ];

	//set it's Z to 0,0 inside the boat
	Location bpos= pi->getPosition();

	p1->MoveTo( bpos.x, bpos.y, p1->getPosition().z );
	p1->setId( p1->getId() | pShipItems[PORT_P_C] );//change the ID

	p2->MoveTo( bpos.x, bpos.y, p2->getPosition().z );
	p2->setId( p2->getId() | pShipItems[STAR_P_C] );

	tiller->MoveTo( bpos.x, bpos.y, tiller->getPosition().z );
	tiller->setId( tiller->getId() | pShipItems[TILLERID] );

	hold->MoveTo(bpos.x, bpos.y, hold->getPosition().z );
	hold->setId( hold->getId() | pShipItems[HOLDID] );

	TurnShip( pi->more1, dir, p1, p2, tiller, hold );

	p1->Refresh();
	p2->Refresh();
	hold->Refresh();
	tiller->Refresh();

	for ( int a=0; a<d; ++a)
	{
		/////////FOR ELCABESA VERY IMPORTAT BY ENDY
		///////THIS PACKET RESUME CLIENT
		SendPauseResumePkt( Send[a], 0x00 );
	}
}

void cBoat::TurnShip( uint8_t size, int32_t dir, pItem pPort, pItem pStarboard, pItem pTiller, pItem pHold )
{
	Location itmpos;
	signed short int *pShipOffsets;

#ifdef SPAR_I_LOCATION_MAP
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
#else
	switch( size )
	{
		case 0x00:
		case 0x04:
			pShipOffsets = iSmallShipOffsets[dir][PORT_PLANK];

			mapRegions->remove( pPort );
			itmpos= pPort->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pPort->setPosition( itmpos );
			mapRegions->add( pPort );

			pShipOffsets = iSmallShipOffsets[dir][STARB_PLANK];
			mapRegions->remove( pStarboard );
			itmpos= pStarboard->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pStarboard->setPosition( itmpos );
			mapRegions->add( pStarboard );

			pShipOffsets = iSmallShipOffsets[dir][TILLER];
			mapRegions->remove( pTiller );
			itmpos= pTiller->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pTiller->setPosition( itmpos );
			mapRegions->add( pTiller );

			pShipOffsets = iSmallShipOffsets[dir][HOLD];
			mapRegions->remove( pHold );
			itmpos= pHold->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pHold->setPosition( itmpos );
			mapRegions->add( pHold );

			break;

		case 0x08:
		case 0x0C:
			pShipOffsets = iMediumShipOffsets[dir][PORT_PLANK];
			mapRegions->remove( pPort );
			itmpos= pPort->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pPort->setPosition( itmpos );
			mapRegions->add( pPort );

			pShipOffsets = iMediumShipOffsets[dir][STARB_PLANK];
			mapRegions->remove( pStarboard );
			itmpos= pStarboard->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pStarboard->setPosition( itmpos );
			mapRegions->add( pStarboard );

			pShipOffsets = iMediumShipOffsets[dir][TILLER];
			mapRegions->remove( pTiller );
			itmpos= pTiller->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pTiller->setPosition( itmpos );
			mapRegions->add( pTiller );

			pShipOffsets = iMediumShipOffsets[dir][HOLD];
			mapRegions->remove( pHold );
			itmpos= pHold->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pHold->setPosition( itmpos );
			mapRegions->add( pHold );

			break;
		case 0x10:
		case 0x14:

			pShipOffsets = iLargeShipOffsets[dir][PORT_PLANK];
			mapRegions->remove( pPort );
			itmpos= pPort->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pPort->setPosition( itmpos );
			mapRegions->add( pPort );

			pShipOffsets = iLargeShipOffsets[dir][STARB_PLANK];
			mapRegions->remove( pStarboard );
			itmpos= pStarboard->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pStarboard->setPosition( itmpos );
			mapRegions->add( pStarboard );

			pShipOffsets = iLargeShipOffsets[dir][TILLER];
			mapRegions->remove( pTiller );
			itmpos= pTiller->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pTiller->setPosition( itmpos );
			mapRegions->add( pTiller );

			pShipOffsets = iLargeShipOffsets[dir][HOLD];
			mapRegions->remove( pHold );
			itmpos= pHold->getPosition();
			itmpos.x+= pShipOffsets[X];
			itmpos.y+= pShipOffsets[Y];
			pHold->setPosition( itmpos );
			mapRegions->add( pHold );

			break;

		default:
			LogWarning("Turnboatstuff() more1 error! more1 = %c not found!\n", size);
			break;
	}
#endif
}


bool cBoat::Speech(pChar pc, pClient clientocket, std::string &talk)//See if they said a command.
{
	/*
		pc & socket validation done in talking()
	*/
	pItem pBoat=GetBoat(pc->getPosition());
	if( ! pBoat )
		return false;
	//
	// if the pc is not the boat owner..we don't care what he says
	//
	if(pBoat->getOwnerSerial32()!= pc->getSerial())
	{
		return false;
	}
	boat_db* boat=search_boat(pBoat->getSerial());
	if(boat==NULL)
		return  false;

	pItem tiller=boat->p_tiller;
	if ( ! tiller )
		return false;
	//
	// Sparhawk: talk has allready been capitalized in talking
	//
	//char msg[512];
	//strncpy(msg,talk,512); // make a local copy of the string,and make sure to not have a overflow
	//strncpy(msg,strupr(msg),512);// then convert the string to uppercase

	int dir=pBoat->dir&0x0F;
	if( talk == "FORWARD" || talk == "UNFURL SAIL" )
	{
		pBoat->type2=1;//Moving
		Move(socket,dir,pBoat);
		itemtalk(tiller, "Aye, sir.");
		return true;
	}
	if( talk == "BACKWARD" )
	{
		pBoat->type2=2;//Moving backward
		if(dir>=4)
			dir-=4;
		else
			dir+=4;
		Move(socket,dir,pBoat);
		itemtalk(tiller, "Aye, sir.");
		return true;
	}
	if( talk == "ONE LEFT" || talk == "DRIFT LEFT" )
	{
		dir-=2;
		if(dir<0)
			dir+=8;
		Move(socket,dir,pBoat);
		itemtalk(tiller,"Aye, sir.");
		return true;
	}
	if( talk == "ONE RIGHT" || talk == "DRIFT RIGHT" )
	{
		dir+=2;
		if(dir>=8) dir-=8;
		Move(socket,dir,pBoat);
		itemtalk(tiller, "Aye, sir.");
		return true;
	}
	if( talk == "STOP" || talk == "FURL SAIL" )
	{
		pBoat->type2=0;
		itemtalk(tiller,"Aye, sir.");
		return true;
	}

	if( talk == "TURN LEFT" || talk == "TURN PORT" )
	{
		if (good_position(pBoat, pBoat->getPosition(), -1) && collision(pBoat,pBoat->getPosition(),-1)==false)
		{
		  Turn(pBoat,0);
		  itemtalk(tiller, "Aye, sir.");
		  return true;
		}
		else
		{
			pBoat->type2=0;
			itemtalk(tiller, "Arr,somethings in the way");
			return true;
		}
	}

	if( talk == "TURN RIGHT" || talk == "TURN STARBOARD" )
	{
		if (good_position(pBoat, pBoat->getPosition(), 1) && collision(pBoat,pBoat->getPosition(),1)==false)
		{
		  Turn(pBoat,1);
		  itemtalk(tiller, "Aye, sir.");
		  return true;
		} else
		{
			pBoat->type2=0;
			itemtalk(tiller, "Arr,somethings in the way");
			return true;
		}
	}
	if( talk == "COME ABOUT" || talk == "TURN ABOUT" )
	{

		if (good_position(pBoat, pBoat->getPosition(), 2) && collision(pBoat,pBoat->getPosition(),2)==false)
		{
			Turn(pBoat,1);
			Turn(pBoat,1);
			itemtalk(tiller, "Aye, sir.");
			return true;
		}
		else
		{
			pBoat->type2=0;
			itemtalk(tiller, "Arr,somethings in the way");
			return true;
		}
	}
	if( talk.substr(0, 9) == "SET NAME " )
	{
		tiller->setCurrentName( talk.substr( 9 ).c_str() );
		//char tmp[200];
		//sprintf(tmp,"%s%s", "a ship named ", &msg[9]);
		//tiller->setCurrentName(tmp);
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
////						NEW BOAT-SYSTEM					////
////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
// Function name     : tile_check ( helper function for good_position )
// Description       : check if all the boats tile are in water
// Return type       : bool
// Author            : Elcabesa
// Changes           : none yet
// Called form		 : cBoat:good_position()


bool cBoat::tile_check(multi_st multi,pItem pBoat,map_st map,int x, int y,int dir)
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
	for( uint32_t i = 0; i < s.size(); i++ ) {
		tile_st tile;
		if( data::seekTile( s[i].id, tile ) ) {
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



///////////////////////////////////////////////////////////////////
// Function name     : good_position
// Description       : check if this is a good position for building or moving a boat
// Return type       : void
// Author            : Elcabesa
// Changes           : none yet
// Called form		 : cBoat:build()

bool cBoat::good_position(pItem pBoat, Location where, int dir)
{
	uint32_t x= where.x, y= where.y, i;
	bool good_pos=false;

	multiVector m;
	data::seekMulti( pBoat->getId()-0x4000, m );

	for( i = 0; i < m.size(); i++ ) {

		map_st map;
/*		if (m.visible)
		{*/
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
					{
						good_pos=tile_check( m[i],pBoat,map,x,y,dir );
						if (good_pos==false)
							return false;
					}
			}
		//}
	}
	return good_pos;
}



///////////////////////////////////////////////////////////////////
// Function name     : Build
// Description       : build a boat
// Return type       : void
// Author            : Elcabesa
// Changes           : none yet
// Called form	     : buildhouse()
bool cBoat::Build(pClient client, pItem pBoat, char id2)
{
	if ( s < 0 || s >= now )
		return false;

	pChar pc_cs = cSerializable::findCharBySerial(currchar[s]);
	if ( !pc_cs ) return false;

	int nid2=id2;

	if( !pBoat )
	{
		pc_cs->sysmsg("There was an error creating that boat.");
		return false;
	}

	if(id2!=0x00 && id2!=0x04 && id2!=0x08 && id2!=0x0C && id2!=0x10 && id2!=0x14)//Valid boat ids (must start pointing north!)
	{
		pc_cs->sysmsg("The deed is broken, please contact a Game Master.");
		return false;
	}
	//Start checking for a valid position:
	if (good_position(pBoat, pBoat->getPosition(), 0)==false)
	{
		return false;
	}
	if(collision(pBoat, pBoat->getPosition(),0)==true)
	{
		return false;
	}
	// Okay we found a good  place....

	pBoat->setOwnerSerial32(pc_cs->getSerial());
	pBoat->more1=id2;//Set min ID
	pBoat->more2=nid2+3;//set MAX id
	pBoat->type=ITYPE_BOATS;//Boat type
	pBoat->setPosition("z", -5);//Z in water
//	strcpy(pBoat->name,"a mast");//Name is something other than "%s's house"
	pBoat->setCurrentName("a mast");

	pItem pTiller=item::CreateFromScript( "$item_tillerman" );
	if( !pTiller ) return false;
	pTiller->setPosition("z", -5);
	pTiller->priv=0;

	pItem pPlankR=item::CreateFromScript( "$item_plank2" );//Plank2 is on the RIGHT side of the boat
	if( !pPlankR ) return false;
	pPlankR->type=ITYPE_BOATS;
	pPlankR->type2=3;
	pPlankR->more1= pBoat->getSerial().ser1;//Lock this item!
	pPlankR->more2= pBoat->getSerial().ser2;
	pPlankR->more3= pBoat->getSerial().ser3;
	pPlankR->more4= pBoat->getSerial().ser4;
	pPlankR->setPosition("z", -5);
	pPlankR->priv=0;//Nodecay

	pItem pPlankL=item::CreateFromScript( "$item_plank1" );//Plank1 is on the LEFT side of the boat
	if( !pPlankL ) return false;
	pPlankL->type=ITYPE_BOATS;//Boat type
	pPlankL->type2=3;//Plank sub type
	pPlankL->more1= pBoat->getSerial().ser1;
	pPlankL->more2= pBoat->getSerial().ser2;//Lock this
	pPlankL->more3= pBoat->getSerial().ser3;
	pPlankL->more4= pBoat->getSerial().ser4;
	pPlankL->setPosition("z", -5);
	pPlankL->priv=0;

	pItem pHold=item::CreateFromScript( "$item_hold1" );
	if( !pHold ) return false;
	pHold->more1= pBoat->getSerial().ser1;//Lock this too :-)
	pHold->more2= pBoat->getSerial().ser2;
	pHold->more3= pBoat->getSerial().ser3;
	pHold->more4= pBoat->getSerial().ser4;

	pHold->type=ITYPE_CONTAINER;//Container
	pHold->setPosition("z", -5);
	pHold->priv=0;
	pHold->setContainer(0);




	pBoat->moreb1= pTiller->getSerial().ser1;//Tiller ser stored in boat's Moreb
	pBoat->moreb2= pTiller->getSerial().ser2;
	pBoat->moreb3= pTiller->getSerial().ser3;
	pBoat->moreb4= pTiller->getSerial().ser4;
	pBoat->morex= pPlankL->getSerial();//Store the other stuff anywhere it will fit :-)
	pBoat->morey= pPlankR->getSerial();
	pBoat->morez= pHold->getSerial();

	Location boatpos= pBoat->getPosition();



	switch(id2)//Give everything the right Z for it size boat
	{
	case 0x00:
	case 0x04:
		pTiller->setPosition("x", boatpos.x + 1);
		pTiller->setPosition("y", boatpos.y + 4);
		pPlankR->setPosition("x", boatpos.x + 2);
		pPlankR->setPosition("y", boatpos.y);
		pPlankL->setPosition("x", boatpos.x - 2);
		pPlankL->setPosition("y", boatpos.y);
		pHold->setPosition("x", boatpos.x);
		pHold->setPosition("y", boatpos.y - 4);
		break;
	case 0x08:
	case 0x0C:
		pTiller->setPosition("x", boatpos.x + 1);
		pTiller->setPosition("y", boatpos.y + 5);
		pPlankR->setPosition("x", boatpos.x + 2);
		pPlankR->setPosition("y", boatpos.y);
		pPlankL->setPosition("x", boatpos.x - 2);
		pPlankL->setPosition("y", boatpos.y);
		pHold->setPosition("x", boatpos.x);
		pHold->setPosition("y", boatpos.y - 4);
		break;
	case 0x10:
	case 0x14:
		pTiller->setPosition("x", boatpos.x + 1);
		pTiller->setPosition("y", boatpos.y + 5);
		pPlankR->setPosition("x", boatpos.x + 2);
		pPlankR->setPosition("y", boatpos.y - 1);
		pPlankL->setPosition("x", boatpos.x - 2);
		pPlankL->setPosition("y", boatpos.y - 1);
		pHold->setPosition("x", boatpos.x);
		pHold->setPosition("y", boatpos.y - 5);
		break;
	}

#ifdef SPAR_I_LOCATION_MAP
	pointers::addToLocationMap( pTiller );
	pointers::addToLocationMap( pPlankL );
	pointers::addToLocationMap( pPlankR );
	pointers::addToLocationMap( pHold );
	pointers::addToLocationMap( pBoat );
#else
	mapRegions->add(pTiller);//Make sure everything is in da regions!
	mapRegions->add(pPlankL);
	mapRegions->add(pPlankR);
	mapRegions->add(pHold);
	mapRegions->add(pBoat);
#endif
	//their x pos is set by BuildHouse(), so just fix their Z...
	boatpos.z+=3;
	boatpos.dispz=boatpos.z;

	pc_cs->MoveTo(boatpos);
	//setserial(DEREF_pChar(pc_cs),DEREF_pItem(pBoat),8);
	pc_cs->setMultiSerial( pBoat->getSerial() );
	insert_boat(pBoat); // insert the boat in the boat_database
	return true;
}

///////////////////////////////////////////////////////////////////
// Function name     : collision
// Description       : handle if at these coord there is another boat
// Return type       : bool TRUE boat collision,FALSE not obat collision
// Author            : Elcabesa
bool cBoat::collision(pItem pi,Location where,int dir)
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
			
		if(boat_collision(pi,x,y,dir,coll.p_serial)==true)
			return true;
	}
	return false;
}

/*!
\brief check if 2 boat are collided
\author Elcabesa
\return true if collided, else false
\see cBoat::collision()
*/
bool cBoat::boat_collision(pItem pBoat1,int x1, int y1,int dir,pItem pBoat2)
{
	uint32_t i1, i2;
	int x,y;

	multiVector m1, m2;
	data::seekMulti( pBoat1->getId()-0x4000, m1 );
	data::seekMulti( pBoat2->getId()-0x4000, m2 );

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

			if ( (x==m2[i2].x+pBoat2->getPosition().x) && (y==m2[i2].y+pBoat2->getPosition().y) )
			{
				return true;
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////
// Function name     : OpenPlank
// Description       : Open, or close the plank (called from keytarget() )
// Return type       : void
// Author            : unknow
// Changes           : none yet

void cBoat::OpenPlank(pItem pi)
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
pItem cBoat::GetBoat(Location pos)
{
	uint32_t i;
	BOATS::iterator iter( s_boat.begin() ), end( s_boat.end() );
	for( ; iter!=end; iter++) {

		boat_db boat=iter->second;
		pItem pBoat=boat.p_serial;
		if( ! pBoat )
			continue;
		
		if( dist( pos, pBoat->getPosition() ) >= 10.0 )
			continue;
		multiVector m;
		data::seekMulti( pBoat->getId()-0x4000, m );

		for( i = 0; i < m.size(); i++ )
		{
			if( ((m[i].x + pBoat->getPosition().x) == pos.x) && ((m[i].y + pBoat->getPosition().y) == pos.y) )
				return  pBoat;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////
// Function name     : Move
// Description       : move a boat, not turn it
// Return type       : int
// Author            : unknow
// Changes           : none yet

void cBoat::Move(pClient client, int dir, pItem pBoat)
{
	iMove(s,dir,pBoat,false);
}


///////////////////////////////////////////////////////////////////
// Function name     : iMove
// Description       : really move a boat, not turn it, and move all the items on a boat
// Return type       : int
// Parameter		 : bool forced TRUE the boat is forced to go, it doesn't check block
// Author            : Elcabesa
// Changes           : none yet

void cBoat::iMove(pClient client, int dir, pItem pBoat, bool forced)
{
	int tx=0,ty=0;
	int serial;

	if (pBoat==NULL) return;
	boat_db* boat=search_boat(pBoat->getSerial());

	if(boat==NULL)
		return;
	pItem tiller=boat->p_tiller;
	pItem p1=boat->p_l_plank;
	pItem p2=boat->p_r_plank;
	pItem hold=boat->p_container;

	//////////////FOR ELCABESA VERY WARNING BY ENDYMION
	//////THIS PACKET PAUSE THE CLIENT
	SendPauseResumePkt(s, 0x01);

	switch(dir&0x0F)//Which DIR is it going in?
	{
	case 0:
		--ty;
		break;
	case 1:
		++tx;
		--ty;
		break;
	case 2:
		++tx;
		break;
	case 3:
		++tx;
		++ty;
		break;
	case 4:
		++ty;
		break;
	case 5:
		--tx;
		++ty;
		break;
	case 6:
		--tx;
		break;
	case 7:
		--tx;
		--ty;
		break;
	default:
		{
		  LogWarning("Warning: Boat direction error: %i int boat %i\n", pBoat->dir&0x0F, pBoat->getSerial());
		  break;
		}
	}


#define XBORDER 200u
#define YBORDER 200u

	Location boatpos= pBoat->getPosition();

	if( (boatpos.x+tx<=XBORDER || boatpos.x+tx>=((map_width*8)-XBORDER))
		|| (boatpos.y+ty<=YBORDER || boatpos.y+ty>=((map_height*8)-YBORDER))) //bugfix LB
	{
		pBoat->type2=0;
		itemtalk(tiller,"Arr, Sir, we've hit rough waters!");
		SendPauseResumePkt(s, 0x00);
		return;
	}


	boatpos.x+= tx;
	boatpos.y+= ty;

	if(!good_position(pBoat, boatpos, 0) && (!forced))
	{
		pBoat->type2=0;
		itemtalk(tiller, "Arr, somethings in the way!");
		SendPauseResumePkt(s, 0x00);
		return;
	}
	if(collision(pBoat, boatpos,0)==true)
	{
		pBoat->type2=0;
		itemtalk(tiller, "Arr, another ship in the way");
		SendPauseResumePkt(s, 0x00);
		return;
	}

	Location tillerpos= tiller->getPosition();
	tillerpos.x+= tx;
	tillerpos.y+= ty;

	Location p1pos= p1->getPosition();
	p1pos.x+= tx;
	p1pos.y+= ty;

	Location p2pos= p2->getPosition();
	p2pos.x+= tx;
	p2pos.y+= ty;

	Location holdpos= hold->getPosition();
	holdpos.x+= tx;
	holdpos.y+= ty;

	//Move all the special items
	pBoat->MoveTo( boatpos );
	tiller->MoveTo( tillerpos );
	p1->MoveTo( p1pos );
	p2->MoveTo( p2pos );
	hold->MoveTo( holdpos );

	serial= pBoat->getSerial();

/*wait until set hav appropriate function
	for (a=0;a<imultisp[serial%HASHMAX].max;a++)  // move all item upside the boat
	{
		c=imultisp[serial%HASHMAX].pointer[a];
		if(c!=-1)
		{
			pItem pi= MAKE_ITEMREF_LOGGED(c,err);
			if(!err)
			{
				mapRegions->remove(pi);
				Location itmpos= pi->getPosition();
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
			   Location charpos= pc_c->getPosition();
			   mapRegions->remove(pc_c);
			   charpos.x+= tx;
			   charpos.y+= ty;
			   pc_c->MoveTo(charpos);
			   pc_c->teleport();
			   mapRegions->add(pc_c);
		   }
		}
	}
*/

	SendPauseResumePkt(s, 0x00);
	pBoat->Refresh();
	tiller->Refresh();
	p1->Refresh();
	p2->Refresh();
	hold->Refresh();
}


cBoat::cBoat()//Consturctor
{
	return;
}

cBoat::~cBoat()//Destructor
{
}



/*!
\brief insert a boat inside boat_db struct and add it to the s_boat map
\author elcabesa
\param pi pointer to the boat to be inserted
\since 0.8
*/
void insert_boat(pItem pi)
{
	boat_db boat;
	boat.serial = pi->getSerial();
	boat.tiller_serial = calcserial(pi->moreb1,pi->moreb2,pi->moreb3,pi->moreb4);
	boat.l_plank_serial = pi->morex;
	boat.r_plank_serial = pi->morey;
	boat.container = pi->morez;
	boat.p_serial = pi;
	boat.p_l_plank = cSerializable::findItemBySerial(boat.l_plank_serial);
	boat.p_r_plank = cSerializable::findItemBySerial(boat.r_plank_serial);
	boat.p_tiller = cSerializable::findItemBySerial(boat.tiller_serial);
	boat.p_container = cSerializable::findItemBySerial(boat.container);
	s_boat.insert(std::make_pair(pi->getSerial(), boat)); // insert a boat in the boat search tree
}

boat_db* search_boat(int32_t ser)
{
	std::map<int,boat_db>::iterator iter_boat;
	iter_boat= s_boat.find(ser);
	if (iter_boat == s_boat.end())
		return 0;
	else
		return &iter_boat->second;
}


pItem search_boat_by_plank(pItem pl)
{
	if ( ! pl )
		return NULL;
	Serial ser;
	ser.ser1=pl->more1;
	ser.ser2=pl->more2;
	ser.ser3=pl->more3;
	ser.ser4=pl->more4;
	boat_db*  boat=search_boat(ser.serial32);
	return boat->p_serial;
}

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief some utils functions
*/

#include "common_libs.h"
#include "utils.h"

/*!
\brief plays the proper door sfx for doors/gates/secretdoors
\author Dupois
\since Oct 8, 1998
*/
static void doorsfx(pItem pi, int x, int y)
{

	static const uint16_t OPENWOOD = 0x00EA;
	static const uint16_t OPENGATE = 0x00EB;
	static const uint16_t OPENSTEEL = 0x00EC;
	static const uint16_t OPENSECRET = 0x00ED;
	static const uint16_t CLOSEWOOD = 0x00F1;
	static const uint16_t CLOSEGATE = 0x00F2;
	static const uint16_t CLOSESTEEL = 0x00F3;
	static const uint16_t CLOSESECRET = 0x00F4;

	if (y==0) // Request open door sfx
	{
		if (((x>=0x0695)&&(x<0x06C5))|| // Open wooden / ratan door
			((x>=0x06D5)&&(x<=0x06F4)))
			soundeffect3(pi,OPENWOOD);

		if (((x>=0x0839)&&(x<=0x0848))|| // Open gate
			((x>=0x084C)&&(x<=0x085B))||
			((x>=0x0866)&&(x<=0x0875)))
			soundeffect3(pi,OPENGATE);

		if (((x>=0x0675)&&(x<0x0695))|| // Open metal
			((x>=0x06C5)&&(x<0x06D5)))
			soundeffect3(pi,OPENSTEEL);

		if ((x>=0x0314)&&(x<=0x0365)) // Open secret
			soundeffect3(pi,OPENSECRET);
	}
	else if (y==1) // Request close door sfx
	{
		if (((x>=0x0695)&&(x<0x06C5))|| // close wooden / ratan door
			((x>=0x06D5)&&(x<=0x06F4)))
			soundeffect3(pi,CLOSEWOOD);

		if (((x>=0x0839)&&(x<=0x0848))|| // close gate
			((x>=0x084C)&&(x<=0x085B))||
			((x>=0x0866)&&(x<=0x0875)))
			soundeffect3(pi,CLOSEGATE);

		if (((x>=0x0675)&&(x<0x0695))|| // close metal
			((x>=0x06C5)&&(x<0x06D5)))
			soundeffect3(pi,CLOSESTEEL);

		if ((x>=0x0314)&&(x<=0x0365)) // close secret
			soundeffect3(pi,CLOSESECRET);
	}

} // doorsfx() END


/*!
\brief Uses of a door
\param pc The Player who used the door (is NULL for automatic door close)
\param pi Door to close
\note Don't validate pc, because it can be NULL for automatic door close
*/
void dooruse(pChar pc, pItem pi)
{
	if ( ! pi )
		return;

	int i, db, x;
	bool changed=0;

	/*if (pc && ( !pc->hasInRange(pi, 2) ) && s>INVALID) {
		sysmessage(s, "You cannot reach the handle from here");
		return;
	}*/

	x=pi->getId();
	for (i=0;i<DOORTYPES;i++)
	{
		db=doorbase[i];

		if (x==(db+0))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x-1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+1))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x+1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		} else if (x==(db+2))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x+1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc , pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+3))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x-1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		} else if (x==(db+4))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x-1, pi->getPosition().y, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+5))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x+1, pi->getPosition().y, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		} else if (x==(db+6))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x+1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+7))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x-1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		} else if (x==(db+8))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x+1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+9))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x-1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		} else if (x==(db+10))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x+1, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+11))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x-1, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		}
		else if (x==(db+12))
		{
			pi->setId( pi->getId()+1 );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+13))
		{
			pi->setId( pi->getId()-1 );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		} else if (x==(db+14))
		{
			pi->setId( pi->getId()+1 );
			pi->setPosition( Location(pi->getPosition().x, pi->getPosition().y-1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 0);
			tempfx::add(pc, pi, tempfx::AUTODOOR, 0, 0, 0);
			pi->dooropen=1;
		} else if (x==(db+15))
		{
			pi->setId( pi->getId()-1 );
			pi->setPosition( Location(pi->getPosition().x, pi->getPosition().y+1, pi->getPosition().z) );
			pi->Refresh();
			changed=1;
			doorsfx(pi, x, 1);
			pi->dooropen=0;
		}
	}
	
	if ( !changed && pc )
	{
		pc->getClient()->sysmessage("This doesnt seem to be a valid door type. Contact a GM.");
		return;
	}
	
	if ( ! changed || ! pc )
		return;

	pc->objectdelay=getclock()+ (server_data.objectdelay/4)*MY_CLOCKS_PER_SEC;
	// house refreshment when a house owner or friend of a houe opens the house door

	int j, houseowner_serial,ds;
	pItem pi_house=findmulti( pi->getPosition() );
	
	if ( ! pi_house )
		return;
	
	const pItem pi2=pi_house;
	if ( ! pi_house->isHouse() )
		return;
	
	houseowner_serial=pi2->getOwnerSerial32();
	j=on_hlist(pi_house, pc->getSerial().ser1,  pc->getSerial().ser2,  pc->getSerial().ser3,  pc->getSerial().ser4, NULL);
	
	if ( j != H_FRIEND && ( pi->getOwner() != pc ) )
		return;
	
	if ( s == INVALID )
		return;
	
	if (SrvParms->housedecay_secs!=0)
		ds=((pi2->time_unused)*100)/(SrvParms->housedecay_secs);
	else ds=INVALID;

	if (ds < 50) // sysmessage iff decay status >=50%
		return;
		
	if (houseowner_serial!= pc->getSerial())
		sysmessage(s,"You refreshed your friend's house");
	else
		sysmessage(s,"You refreshed the house");

	pi2->time_unused=0;
	pi2->timeused_last=getclock();
}

void endmessage(int x) // If shutdown is initialized
{
	uint32_t igetclock = getclock();

	if (endtime<igetclock)
		endtime=igetclock;

	sysbroadcast("server going down in %i minutes.\n",
		((endtime-igetclock)/MY_CLOCKS_PER_SEC)/60);
	InfoOut("server going down in %i minutes.\n",
		((endtime-igetclock)/MY_CLOCKS_PER_SEC)/60);
}

int checkBoundingBox(int xPos, int yPos, int fx1, int fy1, int fz1, int fx2, int fy2)
{
	if (xPos>=((fx1<fx2)?fx1:fx2) && xPos<=((fx1<fx2)?fx2:fx1))
		if (yPos>=((fy1<fy2)?fy1:fy2) && yPos<=((fy1<fy2)?fy2:fy1))
			if (fz1==INVALID || abs(fz1-getHeight(Location(xPos, yPos, fz1)))<=5)
				return 1;
			return 0;
}

int checkBoundingCircle(int xPos, int yPos, int fx1, int fy1, int fz1, int radius)
{
	if ( (xPos-fx1)*(xPos-fx1) + (yPos-fy1)*(yPos-fy1) <= radius * radius)
		if (fz1==INVALID || abs(fz1-getHeight(Location(xPos, yPos, fz1)))<=5)
			return 1;
		return 0;
}

void setabovelight(uint8_t lightchar)
{
	if (lightchar == worldcurlevel)
		return;
	
	worldcurlevel=lightchar;
	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ )
	{
		client->light(worldcurlevel);
	}
}

/*!
\brief converts x,y coords to sextant coords
\author LB
\param x X-coord of object
\param y Y-coord of object
\param cx Central x-coord
\param cy Central y-coord
\note thanks goes to Balxan / UOAM for the basic alogithm
	could be optimized a lot, but the freuqency of being called is probably very low

Central coords for standard map are 1323,1624 (LB's throne) for Sosaria and 5936,3112 for
T2A.
New maps can redefine these values.
*/
std::string getSextantCoords(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy)
{
	float Tx, Ty, Dx, Dy, Wx, Wy, Mx, My, Hx, Hy;
	int16_t xH, xM, yH, yM;
	
	//  map dimensions
	Wx = 5120.0;
	Wy = 4096.0;

	// convert input ints to float;
	Tx = x;
	Ty = y;

	// main calculation
	Dx = ( (Tx - cx) * 360.0 ) / Wx;
	Dy = ( (Ty - cy) * 360.0 ) / Wy;
	
	////// now let's get minutes, hours & directions from it
	Hx = (int16_t) Dx; // get hours (cut off digits after comma, no idea if there's a cleaner/better way)

	Mx = Dx - Hx; // get minutes
	Mx *= 60;

	Hy = (int16_t) Dy;
	My = Dy - Hy;
	My *= 60;

	// convert the results to ints;
	xH = (signed int) Hx;
	xM = (signed int) Mx;
	yH = (signed int) Hy;
	yM = (signed int) My;

	char *temp;
	asprintf(&temp, "%u %u' %c  %u %u' %c",
		abs( (int16_t)Hx ), abs( (int16_t)Mx ), Hx > 0 ? 'E' : 'W',
		abs( (int16_t)Hy ), abs( (int16_t)My ), Hy > 0 ? 'S' : 'M'
		);
	
	std::string ret(temp);
	free(temp);
	
	return ret;
	
}

/*!
\brief trade System
\author Magius(CHE)
\return int ?
\param npc ?
\param pi ?
\param value ?
\param goodtype ?
\todo write documentation
*/
int calcGoodValue(pChar npc, pItem pi, int value,int goodtype)
{
	if ( ! npc ) return 0;
	if ( ! pi ) return value;
	
	int actreg=calcRegionFromXY( npc->getPosition() );
	int regvalue=0;
	int x;
	int good=pi->good;

	if (good<=INVALID || good >255 || actreg<=INVALID || actreg>255) return value;

	if (goodtype==1) regvalue=region[actreg].goodsell[pi->good]; // Vendor SELL
	if (goodtype==0) regvalue=region[actreg].goodbuy[pi->good]; // Vendor BUY

	x=(int) (value*abs(regvalue))/1000;

	if (regvalue<0)	value-=x;
	else value+=x;

	if (value<=0) value=1; // Added by Magius(CHE) (2)

	return value;
}


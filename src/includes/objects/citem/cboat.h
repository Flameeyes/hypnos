/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __CBOAT_H__
#define __CBOAT_H__

#include "common_libs.h"
#include "objects/citem/cmulti.h"

/*!
\class cBoat cboat.h "objects/citem/cboat.h"
\brief Boat

This class represent and manages the boats, taking care of turn them and
get the commands.
*/
class cBoat : public cMulti
{
public:
	cBoat();
	cBoat(uint32_t nserial);

	void step(pClient client, uint8_t dir = 0xFF);
	void turn(bool turnRight);
	
	static pBoat searchByPlank(pItem pl);
	
protected:
	pItem plankRight;	//!< Right plank
	pItem plankLeft;	//!< Left plank
	pItem tillerMan;	//!< Tillerman
	pContainer hold;	//!< Hold
public:
	//! Gets the tiller man
	pItem getTillerMan() const
	{ return tillerMan; }
	
	//! Gets the left plank
	pItem getPlankLeft() const
	{ return plankLeft; }
	
	//! Gets the right plank
	pItem getPlankRight() const
	{ return plankRight; }
	
	//! Gets the hold
	pContainer getHold() const
	{ return hold; }

private:
	enum Item {
		idPortPlankClosed,	//!< Port Plank Closed
		idPortPlankOpen,	//!< Port Planl Opened
		idStarPlankClosed,	//!< Starboard Plank Closed
		idStarPlankOpen,	//!< Starb Plank Open
		idTiller,		//!< Tiller
		idHold			//!< Hold
	};
	static const uint8_t ShipItems[4][6];
};

#endif

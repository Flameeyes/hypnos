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
class cBoat : public cItem
{
public:
	cBoat();
	cBoat(uint32_t nserial);

	void MoveTo(sLocation newloc);
	void step(pClient client, uint8_t dir = 0xFF);
	
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
};

#endif

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __OBJECTS_CITEM_CBOAT_H__
#define __OBJECTS_CITEM_CBOAT_H__

#include "common_libs.h"
#include "objects/citem/cmulti.h"

/*!
\class cBoat cboat.h "objects/citem/cboat.h"
\brief Boat

The boats are special cases in multi items, like the houses. In this case,
anyway, we have a lot more code regarding the items created with the multi
itself.

This because unlike an house, a boat can turn in four different directions,
so we should change and move the items with it.

And also we must provide 4 special multi items: the two planks (left and right)
the tillerman, and the hold (the container in the front of the boat).

\todo Need a way to recalc the char and items position when turning, before call
the cMulti::MoveTo() function

\see cMulti
\see cHouse
*/
class cBoat : public cMulti
{
public:
	cBoat();
	cBoat(uint32_t nserial);

//@{
/*!
\name Movement
\brief Movement and turning functions

A boat, obviously, move itself, so we have different functions to moves the
boat and turn it, with various checks.
*/
	void step(pClient client, uint8_t dir = 0xFF);
	void turn(bool turnRight);
//@}
	
	static pBoat searchByPlank(pItem pl) DEPRECATED;
	
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
	
	bool doSpeech(pClient client, const std::string &speech);

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
	
	static bool isGoodPosition(uint16_t id, sPoint w, int8_t dir);
	static bool tileCheck(multi_st multi, map_st map, sPoint w, int8_t dir);
};

#endif

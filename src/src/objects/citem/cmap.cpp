/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "enums.h"
#include "skills/skills.h"
#include "objects/citem/cmap.h"
#include "objects/cpacket.h"
#include "objects/cclient.h"

cMap::cMap() : cItem()
{
}

/*!
\brief cMap constructor
\param ser serial number
*/
cMap::cMap(uint32_t ser) : cItem(ser)
{
	flags |= flagWritable;
}

cMap &cMap::operator= (const cMap &oldmap)
{
	pinData = oldmap.pinData;
	return dynamic_cast<cMap&>(cItem::operator=(dynamic_cast<const cItem&>(oldmap)));
	//using the parent operator = function to copy all other parameters
}


/*!
\brief Adds a new pin to the map
\param p Coords of the pin to add
\retval true The pin is added correctly
\retval false The pin wasn't added because the map is not writable or there are
	already 50 ore more pins
*/
bool cMap::addPin(sPoint p)
{
	if ( pinData.size() >= 50 || !(flags & flagWritable)) return false;
	pinData.push_back(p);
        return true;
}

/*!
\brief Inserts a pin between 2 existing ones
\param p Coords of the pin to insert
\param pin Index of the new pin to insert at
\note Existing pins greater (or equal) pin numer get shifted by one

\retval true The pin was inserted correctly
\retval false The pin wasn't inserted because the map is not writable or there
	are already too much pins, or the index is out of bound

\bug This function doesn't restrict the size of the pin vector to 50
*/
bool cMap::insertPin(sPoint p, uint32_t pin)
{
	if (pin >= pinData.size() || !(flags & flagWritable)) return false;
	for( int i = pinData.size() -1; i >= (pin-1); i++ )
		pinData[i+1] = pinData[i];
	
	pinData[(pin-1)] = sPoint(x, y);
        
	return true;
}

/*!
\brief Moves pin to another position
\param p Coords to set for the pin
\param pin Index of the pin to edit
\retval true The pin was changed correctly
\retval false The pin wasn't changed because the map is not writable or the
	given index is out of bound
*/
bool cMap::changePin(sPoint p, uint32_t pin)
{
	if (pin > pinData.size() || !(flags & flagWritable)) return false;
	pinData[pin - 1] = sPoint(x,y);
        return true;
}

/*!
\brief Removes the given pin
\param pin Index to the pin to remove
\retval true The pin was removed correctly
\retval false The pin wasn't removed (the index is 0, the index is greater than
	the pins' number, or the map isn't writable.
*/
bool cMap::removePin(uint32_t pin)
{
	if ((pin < 1) || (pin > pinData.size()) || !(flags & flagWritable)) return false;
        pinData.erase(pinData.begin() + pin - 1);
        return true;
}

/*!
\brief Removes all the pin from the map
\retval true The map is writable and now is clear
\retval flase The map wasn't cleared because it's not writable
*/
bool cMap::clearAllPins()
{
	if (!(flags & flagWritable)) return false;
	pinData.clear();
        return true;
}

/*!
\brief Toggle pin addability and replies to client actual writeability status
\param client Client which is toggling the writable flag (and so the one to send the packet)
*/
bool cMap::toggleWritable(pClient client)
{
	if (!isTreasureMap()) flags ^= flagWritable;
        else flags &= ~flagWritable;
	nPackets::Sent::MapPlotCourse pk(this, pccWriteableStatus, writeable, 0, 0);
	client->sendPacket(&pk);
        return true;
}

/*!
\brief Execute the double click of a client on the map
\param client Client performing the double click

This function overloads the cItem::doubleClicked() function for Cmap items and
tests for internal data to show and internal functions to call.

\li If the map is not a treasure map it plots the course of the pins in it.
\li If the map is a treasure map and is not deciphered, it calls the
nSkills::Decipher() function to decipher it.
\li If the map is a deciphere treasure map I actually don't know what it does :)

\todo Add documentation about the third case described over here.
*/
void cMap::doubleClicked(pClient client)
{
	if ( (flags & flagTreasure) && !(flags & flagDeciphered) ) {
		//! \todo redo when treasures redone
		nSkills::Decipher(this, client);
		return;
	}
	
	// This is common both for ITYPE_MAP and ITYPE_DECIPHERED_MAP
	nPackets::Sent::OpenMapGump pk(this);
	client->sendPacket(&pk);
	nPackets::Sent::MapPlotCourse pk2(this, pccClearAllPins); //Sending clear all pins command
	client->sendPacket(&pk2);
	
	//!\todo Replace mores with some decent variables
	if ( (flags & flagTreasure) && (flags & flagDeciphered) ) {
		// Generate message to add a map point
		uint16_t posx, posy;					// tempoary storage for map point
		uint16_t tlx, tly, lrx, lry;				// tempoary storage for map extends
		
		tlx = (more1.moreb1 << 8)  | more1.moreb2;
		tly = (more1.moreb3 << 8)  | more1.moreb4;
		lrx = (more2.moreb1 << 8) | more2.moreb2;
		lry = (more2.moreb3 << 8) | more2.moreb4;
		posx = (256 * (morex - tlx)) / (lrx - tlx);		// Generate location for point
		posy = (256 * (morey - tly)) / (lry - tly);
		
		nPackets::Sent::MapPlotCourse pk3(this, pccAddPin, 0,posx, posy);//Sending add pin command
		client->sendPacket(&pk3);
	} else {
		for(PointVector::iterator it = pinData.begin(); it != pinData.end(); it++)
		{
			nPackets::Sent::MapPlotCourse pki(this, pccAddPin, 0, (*it).x, (*it).y);
			client->sendPacket(&pki);
		}
	}
}

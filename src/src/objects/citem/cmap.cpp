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
	return dynamic_cast<cMap&>(cItem::operator=(dynamic_cast<const cItem&>(oldmap)));//using the parent operator = function to copy all other parameters
}


/*!
\brief Adds a new pin to the map
*/
bool cMap::addPin(uint16_t x, uint16_t y)
{
	if ( pinData.size() > 50 || !(flags & flagWritable)) return false;
	pinData.push_back(sPoint(x,y));
        return true;
}

bool cMap::insertPin(uint16_t x,uint16_t y, uint32_t pin) 	//!< Inserts a pin between 2 existing ones. existing pins >= pin get shifted by one
{
	if (pin >= pinData.size() || !(flags & flagWritable)) return false;
	pinData.insert(pin - 1, sPoint(x,y));
        return true;
}

bool cMap::changePin(uint16_t x,uint16_t y, uint32_t pin)	//!< Moves pin to another position
{
	if (pin > pinData.size() || !(flags & flagWritable)) return false;
	pinData[pin - 1] = sPoint(x,y);
        return true;
}

bool cMap::removePin(uint32_t pin)		//!< Removes pin
{
	if ((pin < 1) || (pin > pinData.size()) || !(flags & flagWritable)) return false;
        pinData.erase(pinData.begin() + pin - 1);
        return true;
}

bool cMap::clearAllPins()		//!< Removes all pins
{
	if (!(flags & flagWritable)) return false;
	pinData.clear();
        return true;
}

bool cMap::toggleWritable()		//!< Toggle pin addability and replies to client actual writeability status
{
	if (!isTreasureMap()) flags ^= flagWritable;
        else flags &= ~flagWritable;
      	nPackets::Sent::MapPlotCourse pk(this, pccWriteableStatus, writeable, iter->x, iter->y);
		//!\todo Chrono you should take a look to this!
	client->sendPacket(&pk);
        return true;
}

void cMap::doubleClicked(pClient client)
{
	if ( (flags & flagTreasure) && !(flags & flagDeciphered) ) {
		//! \todo: redo when treasures redone
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

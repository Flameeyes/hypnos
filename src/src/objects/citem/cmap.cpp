/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cMap class
*/

#include "cmap.h"

/*!
\brief cMap constructor
\param ser serial number
*/
cMap::cMap(uint32_t ser) : cItem(ser)
{
	writeable = true;
//        pinData.clear();  //should not be necessary
}


cMap &operator = (const cMap &oldmap)
{
        pinData.clear();	//if assigned map was not "empty" (without pins), now it is :D

       	//Now copying pindata to the new map
        for(int i = 1; i <= oldmap.getPinsNumber(); i++) pinData.push_back(oldmap.getPin(int pin))
        *((pItem)this) = (cItem)oldmap; //using the parent operator = function to copy all other parameters
	return *this;
}


/*!
\brief Adds a new pin to the map
*/


bool cMap::addPin(uint16_t x, uint16_t y)
{
	if ((pin > 50) || !writeable) return false;
	pinData.push_back(pindataobject(x,y));
        return true;
}

bool cMap::insertPin(uint16_t x,uint16_t y, int pin) 	//!< Inserts a pin between 2 existing ones. existing pins >= pin get shifted by one
{
	if (pin >= pinData.size() || !writeable) return false;
	pinData.insert(pin - 1, pindataobject(x,y));
        return true;
}

bool cMap::changePin(uint16_t x,uint16_t y, int pin)	//!< Moves pin to another position
{
	if (pin > pinData.size() || !writeable) return false;
	pinData[pin - 1] = pindataobject(x,y);
        return true;
}

bool cMap::removePin(int pin)		//!< Removes pin
{
	if ((pin < 1) || (pin > pinData.size()) || !writeable ) return false;
        pinData.erase(pinData.begin() + pin - 1);
        return true;
}

bool cMap::clearAllPins()		//!< Removes all pins
{
	if (!writeable) return false;
	pinData.clear();
        return true;
}

bool cMap::toggleWritable()		//!< Toggle pin addability and replies to client actual writeability status
{
	if (!isTreasureMap()) writeable = !writeable;
        else writeable = false;
      	cPacketSendMapPlotCourse pk(this, WriteableStatus, writeable, iter->x, iter->y);
	client->sendPacket(&pk);
        return true;
}
